#ifndef DMENV_H
#define DMENV_H

#include "dmod.h"
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Initialize the environment variables manager with a buffer.
 * 
 * @param buffer Pointer to the memory buffer to be used for environment variables.
 * @param size   Size of the memory buffer.
 * 
 * @return true if initialization is successful, false otherwise.
 */
DMOD_BUILTIN_API( dmenv, 1.0, bool             , _init, ( void* buffer, size_t size ) );

/**
 * @brief Check if the environment variables manager is initialized.
 * 
 * @return true if initialized, false otherwise.
 */
DMOD_BUILTIN_API( dmenv, 1.0, bool             , _is_initialized, ( void ) );

/**
 * @brief Set an environment variable.
 * 
 * @param name  Name of the environment variable.
 * @param value Value to set for the environment variable.
 * 
 * @return true if the variable was set successfully, false otherwise.
 */
DMOD_BUILTIN_API( dmenv, 1.0, bool             , _set, ( const char* name, const char* value ) );

/**
 * @brief Get an environment variable value.
 * 
 * @param name Name of the environment variable.
 * 
 * @return Pointer to the value string, or NULL if not found.
 */
DMOD_BUILTIN_API( dmenv, 1.0, const char*      , _get, ( const char* name ) );

/**
 * @brief Find environment variables matching a prefix.
 * 
 * @param prefix    Prefix to match against variable names.
 * @param callback  Callback function to call for each matching variable.
 *                  WARNING: The callback is executed while holding the internal
 *                  lock. Do not call other dmenv functions from within the callback
 *                  as this will cause a deadlock. Keep callback execution short.
 * @param user_data User data to pass to the callback.
 * 
 * @return Number of matching variables found.
 */
DMOD_BUILTIN_API( dmenv, 1.0, size_t           , _find, ( const char* prefix, void (*callback)(const char* name, const char* value, void* user_data), void* user_data ) );

/**
 * @brief Remove an environment variable.
 * 
 * @param name Name of the environment variable to remove.
 * 
 * @return true if the variable was removed successfully, false if not found.
 */
DMOD_BUILTIN_API( dmenv, 1.0, bool             , _remove, ( const char* name ) );

/**
 * @brief Clear all environment variables.
 * 
 * @return true if all variables were cleared successfully, false otherwise.
 */
DMOD_BUILTIN_API( dmenv, 1.0, bool             , _clear, ( void ) );

/**
 * @brief Get the number of environment variables currently stored.
 * 
 * @return Number of environment variables.
 */
DMOD_BUILTIN_API( dmenv, 1.0, size_t           , _count, ( void ) );

#endif // DMENV_H
