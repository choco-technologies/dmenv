#include "dmenv.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef DMENV_MAGIC_NUMBER
#define DMENV_MAGIC_NUMBER 0x444D454E  // "DMEN" in hex
#endif

/**
 * @brief Structure to hold an environment variable entry
 */
typedef struct env_entry {
    char* name;
    char* value;
    struct env_entry* next;
} env_entry_t;

/**
 * @brief Context structure for the environment variables manager
 */
typedef struct dmenv_ctx {
    uint32_t magic;
    env_entry_t* head;
    dmenv_ctx_t parent;
    size_t entry_count;
} dmenv_ctx_internal_t;

static dmenv_ctx_t g_default_context = NULL;

/**
 * @brief Helper function to find an entry by name in a context
 */
static env_entry_t* find_entry(dmenv_ctx_t ctx, const char* name) {
    if (!ctx || !name) {
        return NULL;
    }
    
    env_entry_t* current = ((dmenv_ctx_internal_t*)ctx)->head;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

/**
 * @brief Helper function to recursively search in parent contexts
 */
static env_entry_t* find_entry_with_inheritance(dmenv_ctx_t ctx, const char* name) {
    if (!ctx || !name) {
        return NULL;
    }
    
    env_entry_t* entry = find_entry(ctx, name);
    if (entry != NULL) {
        return entry;
    }
    
    // Search in parent context
    dmenv_ctx_internal_t* internal = (dmenv_ctx_internal_t*)ctx;
    if (internal->parent != NULL) {
        return find_entry_with_inheritance(internal->parent, name);
    }
    
    return NULL;
}

// ============================================================================
//                           DMOD API Implementations
// ============================================================================

DMOD_INPUT_API_DECLARATION( dmenv, 1.0, dmenv_ctx_t, _create, ( dmenv_ctx_t parent ) )
{
    Dmod_EnterCritical();
    
    dmenv_ctx_internal_t* ctx = (dmenv_ctx_internal_t*)Dmod_MallocEx(sizeof(dmenv_ctx_internal_t), "dmenv");
    if (ctx == NULL) {
        DMOD_LOG_ERROR("Failed to allocate memory for context");
        Dmod_ExitCritical();
        return NULL;
    }
    
    ctx->magic = DMENV_MAGIC_NUMBER;
    ctx->head = NULL;
    ctx->parent = parent;
    ctx->entry_count = 0;
    
    DMOD_LOG_INFO("== dmenv ver. %s ==", DMENV_VERSION);
    DMOD_LOG_INFO("Created context %p with parent %p", ctx, parent);
    
    Dmod_ExitCritical();
    
    return (dmenv_ctx_t)ctx;
}

DMOD_INPUT_API_DECLARATION( dmenv, 1.0, void, _destroy, ( dmenv_ctx_t ctx ) )
{
    if (!dmenv_is_valid(ctx)) {
        return;
    }
    
    Dmod_EnterCritical();
    
    dmenv_ctx_internal_t* internal = (dmenv_ctx_internal_t*)ctx;
    
    // Free all entries
    env_entry_t* current = internal->head;
    while (current != NULL) {
        env_entry_t* next = current->next;
        Dmod_FreeEx(current->name, false);
        Dmod_FreeEx(current->value, false);
        Dmod_FreeEx(current, false);
        current = next;
    }
    
    // If this is the default context, clear it
    if (g_default_context == ctx) {
        g_default_context = NULL;
    }
    
    // Invalidate magic number
    internal->magic = 0;
    
    Dmod_FreeEx(ctx, false);
    
    DMOD_LOG_INFO("Destroyed context %p", ctx);
    
    Dmod_ExitCritical();
}

DMOD_INPUT_API_DECLARATION( dmenv, 1.0, bool, _is_valid, ( dmenv_ctx_t ctx ) )
{
    Dmod_EnterCritical();
    bool result = ctx != NULL && ((dmenv_ctx_internal_t*)ctx)->magic == DMENV_MAGIC_NUMBER;
    Dmod_ExitCritical();
    return result;
}

DMOD_INPUT_API_DECLARATION( dmenv, 1.0, void, _set_as_default, ( dmenv_ctx_t ctx ) )
{
    if (!dmenv_is_valid(ctx)) {
        DMOD_LOG_ERROR("Invalid context");
        return;
    }
    
    Dmod_EnterCritical();
    g_default_context = ctx;
    DMOD_LOG_INFO("Set context %p as default", ctx);
    Dmod_ExitCritical();
}

DMOD_INPUT_API_DECLARATION( dmenv, 1.0, dmenv_ctx_t, _get_default, ( void ) )
{
    return g_default_context;
}

DMOD_INPUT_API_DECLARATION( dmenv, 1.0, bool, _set, ( dmenv_ctx_t ctx, const char* name, const char* value ) )
{
    if (!dmenv_is_valid(ctx)) {
        DMOD_LOG_ERROR("Invalid context");
        return false;
    }
    
    if (!name || !value) {
        DMOD_LOG_ERROR("Invalid name or value");
        return false;
    }
    
    Dmod_EnterCritical();
    
    dmenv_ctx_internal_t* internal = (dmenv_ctx_internal_t*)ctx;
    
    // Check if variable already exists
    env_entry_t* existing = find_entry(ctx, name);
    if (existing != NULL) {
        // Update existing entry
        char* new_value = (char*)Dmod_MallocEx(strlen(value) + 1, "dmenv");
        if (new_value == NULL) {
            DMOD_LOG_ERROR("Failed to allocate memory for value");
            Dmod_ExitCritical();
            return false;
        }
        strcpy(new_value, value);
        Dmod_FreeEx(existing->value, false);
        existing->value = new_value;
        DMOD_LOG_INFO("Updated variable %s = %s", name, value);
        Dmod_ExitCritical();
        return true;
    }
    
    // Create new entry
    env_entry_t* entry = (env_entry_t*)Dmod_MallocEx(sizeof(env_entry_t), "dmenv");
    if (entry == NULL) {
        DMOD_LOG_ERROR("Failed to allocate memory for entry");
        Dmod_ExitCritical();
        return false;
    }
    
    entry->name = (char*)Dmod_MallocEx(strlen(name) + 1, "dmenv");
    if (entry->name == NULL) {
        DMOD_LOG_ERROR("Failed to allocate memory for name");
        Dmod_FreeEx(entry, false);
        Dmod_ExitCritical();
        return false;
    }
    
    entry->value = (char*)Dmod_MallocEx(strlen(value) + 1, "dmenv");
    if (entry->value == NULL) {
        DMOD_LOG_ERROR("Failed to allocate memory for value");
        Dmod_FreeEx(entry->name, false);
        Dmod_FreeEx(entry, false);
        Dmod_ExitCritical();
        return false;
    }
    
    strcpy(entry->name, name);
    strcpy(entry->value, value);
    
    // Add to linked list
    entry->next = internal->head;
    internal->head = entry;
    internal->entry_count++;
    
    DMOD_LOG_INFO("Set variable %s = %s", name, value);
    
    Dmod_ExitCritical();
    
    return true;
}

DMOD_INPUT_API_DECLARATION( dmenv, 1.0, const char*, _get, ( dmenv_ctx_t ctx, const char* name ) )
{
    if (!dmenv_is_valid(ctx)) {
        DMOD_LOG_ERROR("Invalid context");
        return NULL;
    }
    
    if (!name) {
        DMOD_LOG_ERROR("Invalid name");
        return NULL;
    }
    
    Dmod_EnterCritical();
    
    env_entry_t* entry = find_entry_with_inheritance(ctx, name);
    
    Dmod_ExitCritical();
    
    if (entry != NULL) {
        DMOD_LOG_INFO("Get variable %s = %s", name, entry->value);
        return entry->value;
    }
    
    DMOD_LOG_INFO("Variable %s not found", name);
    return NULL;
}

DMOD_INPUT_API_DECLARATION( dmenv, 1.0, bool, _seti, ( dmenv_ctx_t ctx, const char* name, uint32_t value ) )
{
    char buffer[32];
    Dmod_SnPrintf(buffer, sizeof(buffer), "0x%X", value);
    return dmenv_set(ctx, name, buffer);
}

DMOD_INPUT_API_DECLARATION( dmenv, 1.0, bool, _geti, ( dmenv_ctx_t ctx, const char* name, uint32_t* out_value ) )
{
    if (!out_value) {
        DMOD_LOG_ERROR("Invalid output pointer");
        return false;
    }
    
    const char* str_value = dmenv_get(ctx, name);
    if (str_value == NULL) {
        return false;
    }
    
    // Try to parse as hex or decimal
    char* endptr;
    unsigned long parsed = strtoul(str_value, &endptr, 0);  // 0 = auto-detect base
    
    if (endptr == str_value || *endptr != '\0') {
        DMOD_LOG_ERROR("Failed to parse value '%s' as integer", str_value);
        return false;
    }
    
    *out_value = (uint32_t)parsed;
    return true;
}

DMOD_INPUT_API_DECLARATION( dmenv, 1.0, size_t, _find, ( dmenv_ctx_t ctx, const char* prefix, dmenv_find_callback_t callback, void* user_data ) )
{
    if (!dmenv_is_valid(ctx)) {
        DMOD_LOG_ERROR("Invalid context");
        return 0;
    }
    
    if (!prefix || !callback) {
        DMOD_LOG_ERROR("Invalid prefix or callback");
        return 0;
    }
    
    size_t count = 0;
    size_t prefix_len = strlen(prefix);
    
    Dmod_EnterCritical();
    
    dmenv_ctx_internal_t* internal = (dmenv_ctx_internal_t*)ctx;
    env_entry_t* current = internal->head;
    while (current != NULL) {
        if (strncmp(current->name, prefix, prefix_len) == 0) {
            callback(current->name, current->value, user_data);
            count++;
        }
        current = current->next;
    }
    
    Dmod_ExitCritical();
    
    DMOD_LOG_INFO("Found %zu variables with prefix '%s'", count, prefix);
    
    return count;
}

DMOD_INPUT_API_DECLARATION( dmenv, 1.0, bool, _remove, ( dmenv_ctx_t ctx, const char* name ) )
{
    if (!dmenv_is_valid(ctx)) {
        DMOD_LOG_ERROR("Invalid context");
        return false;
    }
    
    if (!name) {
        DMOD_LOG_ERROR("Invalid name");
        return false;
    }
    
    Dmod_EnterCritical();
    
    dmenv_ctx_internal_t* internal = (dmenv_ctx_internal_t*)ctx;
    env_entry_t* current = internal->head;
    env_entry_t* prev = NULL;
    
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            // Found the entry to remove
            if (prev == NULL) {
                // Removing the head
                internal->head = current->next;
            } else {
                // Removing from middle or end
                prev->next = current->next;
            }
            
            Dmod_FreeEx(current->name, false);
            Dmod_FreeEx(current->value, false);
            Dmod_FreeEx(current, false);
            
            internal->entry_count--;
            
            DMOD_LOG_INFO("Removed variable %s", name);
            
            Dmod_ExitCritical();
            return true;
        }
        prev = current;
        current = current->next;
    }
    
    Dmod_ExitCritical();
    
    DMOD_LOG_INFO("Variable %s not found for removal", name);
    return false;
}

DMOD_INPUT_API_DECLARATION( dmenv, 1.0, bool, _clear, ( dmenv_ctx_t ctx ) )
{
    if (!dmenv_is_valid(ctx)) {
        DMOD_LOG_ERROR("Invalid context");
        return false;
    }
    
    Dmod_EnterCritical();
    
    dmenv_ctx_internal_t* internal = (dmenv_ctx_internal_t*)ctx;
    
    // Free all entries
    env_entry_t* current = internal->head;
    while (current != NULL) {
        env_entry_t* next = current->next;
        Dmod_FreeEx(current->name, false);
        Dmod_FreeEx(current->value, false);
        Dmod_FreeEx(current, false);
        current = next;
    }
    
    internal->head = NULL;
    internal->entry_count = 0;
    
    DMOD_LOG_INFO("Cleared all environment variables");
    
    Dmod_ExitCritical();
    
    return true;
}

DMOD_INPUT_API_DECLARATION( dmenv, 1.0, size_t, _count, ( dmenv_ctx_t ctx ) )
{
    if (!dmenv_is_valid(ctx)) {
        return 0;
    }
    
    Dmod_EnterCritical();
    dmenv_ctx_internal_t* internal = (dmenv_ctx_internal_t*)ctx;
    size_t count = internal->entry_count;
    Dmod_ExitCritical();
    
    return count;
}

#ifndef DMENV_DONT_IMPLEMENT_DMOD_API
/**
 * @brief Set an environment variable in the default context (DMOD API)
 * 
 * @param Name Name of the environment variable
 * @param Value Value to set
 * @return bool true if successful, false otherwise
 */
DMOD_INPUT_API_DECLARATION( Dmod, 1.0, bool, _SetEnv, ( const char* Name, const char* Value ) )
{
    dmenv_ctx_t ctx = dmenv_get_default();
    if (ctx == NULL) {
        DMOD_LOG_ERROR("No default context set for Dmod_SetEnv");
        return false;
    }
    return dmenv_set(ctx, Name, Value);
}

/**
 * @brief Get an environment variable from the default context (DMOD API)
 * 
 * @param Name Name of the environment variable
 * @return const char* Value if found, NULL otherwise
 */
DMOD_INPUT_API_DECLARATION( Dmod, 1.0, const char*, _GetEnv, ( const char* Name ) )
{
    dmenv_ctx_t ctx = dmenv_get_default();
    if (ctx == NULL) {
        DMOD_LOG_ERROR("No default context set for Dmod_GetEnv");
        return NULL;
    }
    return dmenv_get(ctx, Name);
}
#endif // DMENV_DONT_IMPLEMENT_DMOD_API
