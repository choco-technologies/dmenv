#include "dmenv.h"
#include <string.h>
#include <stdio.h>

#ifndef DMENV_MAX_NAME_LENGTH
#define DMENV_MAX_NAME_LENGTH 64
#endif

#ifndef DMENV_MAX_VALUE_LENGTH
#define DMENV_MAX_VALUE_LENGTH 256
#endif

/**
 * @brief Structure to hold an environment variable entry
 */
typedef struct env_entry {
    char name[DMENV_MAX_NAME_LENGTH];
    char value[DMENV_MAX_VALUE_LENGTH];
    struct env_entry* next;
} env_entry_t;

/**
 * @brief Context structure for the environment variables manager
 */
typedef struct {
    void* buffer;
    size_t buffer_size;
    env_entry_t* head;
    bool initialized;
    size_t entry_count;
    size_t used_size;
} dmenv_context_t;

static dmenv_context_t g_dmenv_context = {0};

/**
 * @brief Helper function to allocate an entry from the buffer
 */
static env_entry_t* allocate_entry(void) {
    if (g_dmenv_context.used_size + sizeof(env_entry_t) > g_dmenv_context.buffer_size) {
        DMOD_LOG_ERROR("Buffer full, cannot allocate new entry");
        return NULL;
    }
    
    env_entry_t* entry = (env_entry_t*)((char*)g_dmenv_context.buffer + g_dmenv_context.used_size);
    g_dmenv_context.used_size += sizeof(env_entry_t);
    memset(entry, 0, sizeof(env_entry_t));
    return entry;
}

/**
 * @brief Helper function to find an entry by name
 */
static env_entry_t* find_entry(const char* name) {
    if (!name || !g_dmenv_context.initialized) {
        return NULL;
    }
    
    env_entry_t* current = g_dmenv_context.head;
    while (current != NULL) {
        if (strncmp(current->name, name, DMENV_MAX_NAME_LENGTH) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// ============================================================================
//                           DMOD API Implementations
// ============================================================================

DMOD_INPUT_API_DECLARATION( dmenv, 1.0, bool, _init, ( void* buffer, size_t size ) )
{
    if (!buffer || size < sizeof(env_entry_t)) {
        DMOD_LOG_ERROR("Invalid buffer or size for initialization");
        return false;
    }
    
    Dmod_EnterCritical();
    
    g_dmenv_context.buffer = buffer;
    g_dmenv_context.buffer_size = size;
    g_dmenv_context.head = NULL;
    g_dmenv_context.initialized = true;
    g_dmenv_context.entry_count = 0;
    g_dmenv_context.used_size = 0;
    
    DMOD_LOG_INFO("dmenv: Initialized with buffer %p of size %zu", buffer, size);
    
    Dmod_ExitCritical();
    
    return true;
}

DMOD_INPUT_API_DECLARATION( dmenv, 1.0, bool, _is_initialized, ( void ) )
{
    return g_dmenv_context.initialized;
}

DMOD_INPUT_API_DECLARATION( dmenv, 1.0, bool, _set, ( const char* name, const char* value ) )
{
    if (!g_dmenv_context.initialized) {
        DMOD_LOG_ERROR("Environment manager not initialized");
        return false;
    }
    
    if (!name || !value) {
        DMOD_LOG_ERROR("Invalid name or value");
        return false;
    }
    
    if (strlen(name) >= DMENV_MAX_NAME_LENGTH) {
        DMOD_LOG_ERROR("Name too long: %s", name);
        return false;
    }
    
    if (strlen(value) >= DMENV_MAX_VALUE_LENGTH) {
        DMOD_LOG_ERROR("Value too long for name: %s", name);
        return false;
    }
    
    Dmod_EnterCritical();
    
    // Check if variable already exists
    env_entry_t* existing = find_entry(name);
    if (existing != NULL) {
        // Update existing entry
        strncpy(existing->value, value, DMENV_MAX_VALUE_LENGTH - 1);
        existing->value[DMENV_MAX_VALUE_LENGTH - 1] = '\0';
        DMOD_LOG_INFO("Updated variable %s = %s", name, value);
        Dmod_ExitCritical();
        return true;
    }
    
    // Create new entry
    env_entry_t* entry = allocate_entry();
    if (entry == NULL) {
        Dmod_ExitCritical();
        return false;
    }
    
    strncpy(entry->name, name, DMENV_MAX_NAME_LENGTH - 1);
    entry->name[DMENV_MAX_NAME_LENGTH - 1] = '\0';
    strncpy(entry->value, value, DMENV_MAX_VALUE_LENGTH - 1);
    entry->value[DMENV_MAX_VALUE_LENGTH - 1] = '\0';
    
    // Add to linked list
    entry->next = g_dmenv_context.head;
    g_dmenv_context.head = entry;
    g_dmenv_context.entry_count++;
    
    DMOD_LOG_INFO("Set variable %s = %s", name, value);
    
    Dmod_ExitCritical();
    
    return true;
}

DMOD_INPUT_API_DECLARATION( dmenv, 1.0, const char*, _get, ( const char* name ) )
{
    if (!g_dmenv_context.initialized) {
        DMOD_LOG_ERROR("Environment manager not initialized");
        return NULL;
    }
    
    if (!name) {
        DMOD_LOG_ERROR("Invalid name");
        return NULL;
    }
    
    Dmod_EnterCritical();
    
    env_entry_t* entry = find_entry(name);
    
    Dmod_ExitCritical();
    
    if (entry != NULL) {
        DMOD_LOG_INFO("Get variable %s = %s", name, entry->value);
        return entry->value;
    }
    
    DMOD_LOG_INFO("Variable %s not found", name);
    return NULL;
}

DMOD_INPUT_API_DECLARATION( dmenv, 1.0, size_t, _find, ( const char* prefix, void (*callback)(const char* name, const char* value, void* user_data), void* user_data ) )
{
    if (!g_dmenv_context.initialized) {
        DMOD_LOG_ERROR("Environment manager not initialized");
        return 0;
    }
    
    if (!prefix || !callback) {
        DMOD_LOG_ERROR("Invalid prefix or callback");
        return 0;
    }
    
    size_t count = 0;
    size_t prefix_len = strlen(prefix);
    
    Dmod_EnterCritical();
    
    env_entry_t* current = g_dmenv_context.head;
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

DMOD_INPUT_API_DECLARATION( dmenv, 1.0, bool, _remove, ( const char* name ) )
{
    if (!g_dmenv_context.initialized) {
        DMOD_LOG_ERROR("Environment manager not initialized");
        return false;
    }
    
    if (!name) {
        DMOD_LOG_ERROR("Invalid name");
        return false;
    }
    
    Dmod_EnterCritical();
    
    env_entry_t* current = g_dmenv_context.head;
    env_entry_t* prev = NULL;
    
    while (current != NULL) {
        if (strncmp(current->name, name, DMENV_MAX_NAME_LENGTH) == 0) {
            // Found the entry to remove
            if (prev == NULL) {
                // Removing the head
                g_dmenv_context.head = current->next;
            } else {
                // Removing from middle or end
                prev->next = current->next;
            }
            
            g_dmenv_context.entry_count--;
            
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

DMOD_INPUT_API_DECLARATION( dmenv, 1.0, bool, _clear, ( void ) )
{
    if (!g_dmenv_context.initialized) {
        DMOD_LOG_ERROR("Environment manager not initialized");
        return false;
    }
    
    Dmod_EnterCritical();
    
    g_dmenv_context.head = NULL;
    g_dmenv_context.entry_count = 0;
    g_dmenv_context.used_size = 0;
    
    DMOD_LOG_INFO("Cleared all environment variables");
    
    Dmod_ExitCritical();
    
    return true;
}

DMOD_INPUT_API_DECLARATION( dmenv, 1.0, size_t, _count, ( void ) )
{
    if (!g_dmenv_context.initialized) {
        return 0;
    }
    
    return g_dmenv_context.entry_count;
}
