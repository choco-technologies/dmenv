#ifndef DMENV_H
#define DMENV_H

#include "dmod.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Opaque context type for environment variables manager
 */
typedef struct dmenv_ctx* dmenv_ctx_t;

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
typedef void (*dmenv_find_callback_t)(const char* name, const char* value, void* user_data);

/**
 * @brief Create a new environment variables context
 * 
 * @param parent Optional parent context for variable inheritance. If a variable
 *               is not found in the current context, it will be searched in the
 *               parent context. Pass NULL for no inheritance.
 * 
 * @return Pointer to the created context, or NULL on failure
 */
DMOD_BUILTIN_API( dmenv, 1.0, dmenv_ctx_t, _create, ( dmenv_ctx_t parent ) );

/**
 * @brief Destroy an environment variables context
 * 
 * @param ctx Context to destroy
 */
DMOD_BUILTIN_API( dmenv, 1.0, void, _destroy, ( dmenv_ctx_t ctx ) );

/**
 * @brief Check if a context is valid
 * 
 * @param ctx Context to check
 * @return true if valid, false otherwise
 */
DMOD_BUILTIN_API( dmenv, 1.0, bool, _is_valid, ( dmenv_ctx_t ctx ) );

/**
 * @brief Set the default context
 * 
 * @param ctx Context to set as default
 */
DMOD_BUILTIN_API( dmenv, 1.0, void, _set_as_default, ( dmenv_ctx_t ctx ) );

/**
 * @brief Get the default context
 * 
 * @return Pointer to the default context, or NULL if not set
 */
DMOD_BUILTIN_API( dmenv, 1.0, dmenv_ctx_t, _get_default, ( void ) );

/**
 * @brief Set an environment variable (string value)
 * 
 * @param ctx   Context to set the variable in
 * @param name  Name of the environment variable
 * @param value Value to set for the environment variable
 * 
 * @return true if the variable was set successfully, false otherwise
 */
DMOD_BUILTIN_API( dmenv, 1.0, bool, _set, ( dmenv_ctx_t ctx, const char* name, const char* value ) );

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
DMOD_BUILTIN_API( dmenv, 1.0, const char*, _get, ( dmenv_ctx_t ctx, const char* name ) );

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
DMOD_BUILTIN_API( dmenv, 1.0, bool, _seti, ( dmenv_ctx_t ctx, const char* name, uint32_t value ) );

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
DMOD_BUILTIN_API( dmenv, 1.0, bool, _geti, ( dmenv_ctx_t ctx, const char* name, uint32_t* out_value ) );

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
DMOD_BUILTIN_API( dmenv, 1.0, size_t, _find, ( dmenv_ctx_t ctx, const char* prefix, dmenv_find_callback_t callback, void* user_data ) );

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
DMOD_BUILTIN_API( dmenv, 1.0, bool, _remove, ( dmenv_ctx_t ctx, const char* name ) );

/**
 * @brief Clear all environment variables in a context
 * 
 * Only clears variables in the current context, not in parent contexts.
 * 
 * @param ctx Context to clear
 * 
 * @return true if all variables were cleared successfully, false otherwise
 */
DMOD_BUILTIN_API( dmenv, 1.0, bool, _clear, ( dmenv_ctx_t ctx ) );

/**
 * @brief Get the number of environment variables in a context
 * 
 * Only counts variables in the current context, not in parent contexts.
 * 
 * @param ctx Context to count variables in
 * 
 * @return Number of environment variables
 */
DMOD_BUILTIN_API( dmenv, 1.0, size_t, _count, ( dmenv_ctx_t ctx ) );

#endif // DMENV_H
