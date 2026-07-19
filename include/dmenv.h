#ifndef DMENV_H
#define DMENV_H

/*
 * Only dmod_types.h - not the full dmod.h - is needed here: it gives us the
 * DMOD_BUILTIN_API macro and the Dmod_ApiRegistration_t type it expands to,
 * without pulling in dmod_sal.h's kernel Built-in API declarations (Dmod_Malloc,
 * Dmod_FileOpen, ...) or dmod_system.h/dmod_module.h. Those get registered by
 * whoever actually owns them (dmod_system.c); if this header pulled them in
 * too, a translation unit that defines DMOD_ENABLE_REGISTRATION to register
 * dmenv's own API (see dmenv_registrations.c) would also try to re-register
 * every kernel API, causing "multiple definition" link errors against
 * dmod_system.c's own registrations.
 */
#include "dmod_types.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Opaque context type for environment variables manager
 */
typedef struct dmenv_ctx *dmenv_ctx_t;

/**
 * @brief Callback function type for dmenv_find operation
 *
 * This callback is invoked for each environment variable that matches the
 * search prefix. The callback is executed while holding an internal lock,
 * so it should not call other dmenv functions and should complete quickly.
 *
 * @param name      Name of the matched environment variable
 * @param value     Value of the matched environment variable
 * @param user_data User-provided data pointer passed to dmenv_find
 */
typedef void (*dmenv_find_callback_t)(const char *name, const char *value, void *user_data);

/**
 * @brief Create a new environment variables context
 *
 * @param parent Optional parent context for variable inheritance. If a variable
 *               is not found in the current context, it will be searched in the
 *               parent context. Pass NULL for no inheritance.
 *
 * @return Pointer to the created context, or NULL on failure
 */
DMOD_BUILTIN_API(dmenv, 1.0, dmenv_ctx_t, _create, (dmenv_ctx_t parent));

/**
 * @brief Destroy an environment variables context
 *
 * @param ctx Context to destroy
 */
DMOD_BUILTIN_API(dmenv, 1.0, void, _destroy, (dmenv_ctx_t ctx));

/**
 * @brief Check if a context is valid
 *
 * @param ctx Context to check
 * @return true if valid, false otherwise
 */
DMOD_BUILTIN_API(dmenv, 1.0, bool, _is_valid, (dmenv_ctx_t ctx));

/**
 * @brief Set the root context
 *
 * Sets the global root context. The root context serves as the base context
 * when no other contexts have been pushed onto the context stack.
 *
 * @param ctx Context to set as root context
 */
DMOD_BUILTIN_API(dmenv, 1.0, void, _set_root_context, (dmenv_ctx_t ctx));

/**
 * @brief Get the root context
 *
 * @return Pointer to the root context, or NULL if not set
 */
DMOD_BUILTIN_API(dmenv, 1.0, dmenv_ctx_t, _get_root_context, (void));

/**
 * @brief Push a context onto the context stack
 *
 * Pushes the specified context onto the context stack, making it the current
 * context. When accessing variables through the current context, this context
 * will be used instead of the root context.
 *
 * @param ctx Context to push onto the stack
 *
 * @return true if the context was pushed successfully, false otherwise (e.g., stack overflow or invalid context)
 */
DMOD_BUILTIN_API(dmenv, 1.0, bool, _push_context, (dmenv_ctx_t ctx));

/**
 * @brief Pop the current context from the context stack
 *
 * Removes the top context from the context stack. After popping, the previous
 * context on the stack becomes the current context. If the stack is empty,
 * the root context becomes the current context.
 *
 * @return Pointer to the popped context, or NULL if the stack was empty
 */
DMOD_BUILTIN_API(dmenv, 1.0, dmenv_ctx_t, _pop_context, (void));

/**
 * @brief Get the current context
 *
 * Returns the current active context. If contexts have been pushed onto the
 * stack, returns the top context. Otherwise, returns the root context.
 *
 * @return Pointer to the current context, or NULL if no context is set
 */
DMOD_BUILTIN_API(dmenv, 1.0, dmenv_ctx_t, _get_current_context, (void));

/**
 * @brief Set an environment variable (string value)
 *
 * @param ctx   Context to set the variable in
 * @param name  Name of the environment variable
 * @param value Value to set for the environment variable
 *
 * @return true if the variable was set successfully, false otherwise
 */
DMOD_BUILTIN_API(dmenv, 1.0, bool, _set, (dmenv_ctx_t ctx, const char *name, const char *value));

/**
 * @brief Get an environment variable value (string)
 *
 * If the variable is not found in the context and the context has a parent,
 * the parent will be searched recursively.
 *
 * @param ctx  Context to get the variable from
 * @param name Name of the environment variable
 *
 * @return Pointer to the value string, or NULL if not found
 */
DMOD_BUILTIN_API(dmenv, 1.0, const char *, _get, (dmenv_ctx_t ctx, const char *name));

/**
 * @brief Set an environment variable (unsigned integer value in hex)
 *
 * The value is stored internally as a hexadecimal string (e.g., "0x2000").
 *
 * @param ctx   Context to set the variable in
 * @param name  Name of the environment variable
 * @param value Unsigned integer value to set
 *
 * @return true if the variable was set successfully, false otherwise
 */
DMOD_BUILTIN_API(dmenv, 1.0, bool, _seti, (dmenv_ctx_t ctx, const char *name, uint32_t value));

/**
 * @brief Get an environment variable value (unsigned integer)
 *
 * Parses the value as a hexadecimal or decimal number. If the variable is not
 * found in the context and the context has a parent, the parent will be searched.
 *
 * @param ctx  Context to get the variable from
 * @param name Name of the environment variable
 * @param out_value Pointer to store the parsed value
 *
 * @return true if the variable was found and parsed successfully, false otherwise
 */
DMOD_BUILTIN_API(dmenv, 1.0, bool, _geti, (dmenv_ctx_t ctx, const char *name, uint32_t *out_value));

/**
 * @brief Find environment variables matching a prefix
 *
 * Only searches in the current context, not in parent contexts.
 *
 * @param ctx       Context to search in
 * @param prefix    Prefix to match against variable names
 * @param callback  Callback function to call for each matching variable
 * @param user_data User data to pass to the callback
 *
 * @return Number of matching variables found
 */
DMOD_BUILTIN_API(dmenv, 1.0, size_t, _find, (dmenv_ctx_t ctx, const char *prefix, dmenv_find_callback_t callback, void *user_data));

/**
 * @brief Remove an environment variable
 *
 * Only removes from the current context, not from parent contexts.
 *
 * @param ctx  Context to remove the variable from
 * @param name Name of the environment variable to remove
 *
 * @return true if the variable was removed successfully, false if not found
 */
DMOD_BUILTIN_API(dmenv, 1.0, bool, _remove, (dmenv_ctx_t ctx, const char *name));

/**
 * @brief Clear all environment variables in a context
 *
 * Only clears variables in the current context, not in parent contexts.
 *
 * @param ctx Context to clear
 *
 * @return true if all variables were cleared successfully, false otherwise
 */
DMOD_BUILTIN_API(dmenv, 1.0, bool, _clear, (dmenv_ctx_t ctx));

/**
 * @brief Get the number of environment variables in a context
 *
 * Only counts variables in the current context, not in parent contexts.
 *
 * @param ctx Context to count variables in
 *
 * @return Number of environment variables
 */
DMOD_BUILTIN_API(dmenv, 1.0, size_t, _count, (dmenv_ctx_t ctx));

#endif // DMENV_H
