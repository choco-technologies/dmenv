# dmenv - DMOD Environment Variables Manager

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

A lightweight environment variables manager designed specifically for the **DMOD (Dynamic Modules)** framework. dmenv provides a simple and efficient way to manage environment variables in embedded systems with limited resources.

## Table of Contents

- [Overview](#overview)
- [What is DMOD?](#what-is-dmod)
- [What is dmenv?](#what-is-dmenv)
- [Features](#features)
- [Building](#building)
- [Testing](#testing)
- [Usage](#usage)
- [API Reference](#api-reference)
- [Contributing](#contributing)
- [License](#license)

## Overview

dmenv is a custom environment variables manager that integrates seamlessly with the DMOD dynamic module system. It provides a context-based solution for storing and retrieving configuration parameters, making it ideal for embedded systems where flexibility and modularity are important. With support for context inheritance, multiple isolated environments can be managed efficiently.

## What is DMOD?

**DMOD (Dynamic Modules)** is a library that enables dynamic loading and unloading of modules in embedded systems at runtime. It allows you to:

- **Dynamically load modules**: Load functionality from `.dmf` files without recompiling
- **Manage dependencies**: Automatically handle module dependencies
- **Inter-module communication**: Modules can communicate via a common API
- **Resource management**: Efficiently manage system resources
- **Safe updates**: Update individual modules without affecting the entire system

DMOD provides a modular architecture that makes embedded systems more flexible, maintainable, and easier to extend. For more information, visit the [DMOD repository](https://github.com/choco-technologies/dmod).

## What is dmenv?

**dmenv** is an environment variables manager specifically designed to work with DMOD. It provides:

- **Simple key-value storage**: Store configuration parameters as name-value pairs
- **Context-based management**: Create multiple isolated environment contexts
- **Context inheritance**: Child contexts can inherit variables from parent contexts
- **Thread-safe operations**: Uses DMOD's critical section mechanisms
- **Prefix-based search**: Find all variables matching a specific prefix
- **Efficient lookup**: Fast variable retrieval using linked list structure
- **Integer support**: Set and get variables as unsigned integers (hex or decimal)
- **Module integration**: Seamless integration with DMOD logging and error handling

dmenv is ideal for storing application configuration, runtime parameters, and inter-module communication data in embedded DMOD-based systems.

## Features

- ✅ **Context-based management**: Create multiple isolated environment contexts
- ✅ **Context inheritance**: Child contexts can inherit variables from parent contexts
- ✅ **Key-value storage**: Store and retrieve environment variables by name
- ✅ **Variable update**: Update existing variables without creating duplicates
- ✅ **Variable removal**: Remove individual variables or clear all at once
- ✅ **Prefix search**: Find all variables matching a given prefix
- ✅ **Variable counting**: Track the number of stored variables
- ✅ **Integer support**: Set and get variables as unsigned integers (hex or decimal)
- ✅ **Dynamic allocation**: Uses DMOD's memory management system
- ✅ **Thread-safe**: Uses DMOD critical sections for synchronization
- ✅ **Comprehensive logging**: Integration with DMOD logging system
- ✅ **Zero external dependencies**: Only requires DMOD framework

## Building

### Prerequisites

- **CMake**: Version 3.10 or higher
- **C Compiler**: GCC or compatible
- **Make**: For Makefile-based builds (optional)

### Using CMake

```bash
# Clone the repository
git clone https://github.com/choco-technologies/dmenv.git
cd dmenv

# Create build directory
mkdir build
cd build

# Configure
cmake ..

# Build
make

# Run tests
ctest --verbose
```

### Build Options

You can customize the build with these CMake options:

```bash
# Enable tests
cmake -DDMENV_BUILD_TESTS=ON ..

# Disable DMOD API implementation
cmake -DDMENV_DONT_IMPLEMENT_DMOD_API=ON ..

# Change DMOD mode
cmake -DDMOD_MODE=DMOD_EMBEDDED ..
```

### Using Makefile

dmenv also supports traditional Makefile builds:

```bash
# Build the library
make

# The library will be created as libdmenv.a
```

## Testing

dmenv includes comprehensive test suites:

### Test Suites

1. **test_minimal**: Minimal smoke tests
   - Context creation and destruction
   - Basic set/get operations
   - Count and remove operations
   - Integer operations

2. **test_simple**: Simple functional tests
   - Set and get variables
   - Remove variables
   - Clear all variables
   - Find variables by prefix
   - Integer operations
   - Context inheritance

3. **test_dmenv_unit**: Comprehensive unit tests using Unity framework
   - Context creation and validation
   - Set/get operations
   - Update existing variables
   - Remove operations
   - Clear all variables
   - Count operations
   - Find operations with prefixes
   - Integer operations (seti/geti)
   - Context inheritance
   - Context override behavior
   - Default context management
   - Multiple variable stress tests

### Running Tests

```bash
# Run all tests
cd build
ctest

# Run with verbose output
ctest --verbose

# Run specific test
./tests/test_minimal
./tests/test_simple
./tests/test_dmenv_unit
```

## Usage

### Basic Usage

```c
#include "dmenv.h"
#include <string.h>

int main(void) {
    // 1. Create an environment context
    dmenv_ctx_t ctx = dmenv_create(NULL);
    if (ctx == NULL) {
        // Handle creation failure
        return -1;
    }
    
    // 2. Set environment variables
    dmenv_set(ctx, "APP_NAME", "MyApplication");
    dmenv_set(ctx, "APP_VERSION", "1.0.0");
    dmenv_set(ctx, "DEBUG_MODE", "true");
    
    // 3. Get environment variables
    const char* app_name = dmenv_get(ctx, "APP_NAME");
    if (app_name != NULL) {
        printf("Application: %s\n", app_name);
    }
    
    // 4. Clean up
    dmenv_destroy(ctx);
    
    return 0;
}
```

### Update Variables

```c
#include "dmenv.h"

void config_example(void) {
    dmenv_ctx_t ctx = dmenv_create(NULL);
    
    // Set initial value
    dmenv_set(ctx, "LOG_LEVEL", "INFO");
    
    // Update the value (no duplicate created)
    dmenv_set(ctx, "LOG_LEVEL", "DEBUG");
    
    // Get updated value
    const char* level = dmenv_get(ctx, "LOG_LEVEL");
    // level now contains "DEBUG"
    
    dmenv_destroy(ctx);
}
```

### Remove Variables

```c
#include "dmenv.h"

void cleanup_example(void) {
    dmenv_ctx_t ctx = dmenv_create(NULL);
    
    // Set a temporary variable
    dmenv_set(ctx, "TEMP_DATA", "temporary_value");
    
    // Remove it when done
    bool removed = dmenv_remove(ctx, "TEMP_DATA");
    if (removed) {
        printf("Temporary data removed\n");
    }
    
    // Variable no longer exists
    const char* value = dmenv_get(ctx, "TEMP_DATA");
    // value is NULL
    
    dmenv_destroy(ctx);
}
```

### Find Variables by Prefix

```c
#include "dmenv.h"

void print_var(const char* name, const char* value, void* user_data) {
    printf("%s = %s\n", name, value);
}

void find_example(void) {
    dmenv_ctx_t ctx = dmenv_create(NULL);
    
    // Set related variables with common prefix
    dmenv_set(ctx, "DB_HOST", "localhost");
    dmenv_set(ctx, "DB_PORT", "5432");
    dmenv_set(ctx, "DB_NAME", "mydb");
    dmenv_set(ctx, "APP_NAME", "myapp");
    
    // Find all database-related variables
    printf("Database configuration:\n");
    size_t found = dmenv_find(ctx, "DB_", print_var, NULL);
    printf("Found %zu database variables\n", found);
    
    dmenv_destroy(ctx);
}
```

### Clear All Variables

```c
#include "dmenv.h"

void reset_example(void) {
    dmenv_ctx_t ctx = dmenv_create(NULL);
    
    // Clear all environment variables
    dmenv_clear(ctx);
    
    // Verify count is 0
    size_t count = dmenv_count(ctx);
    printf("Variables remaining: %zu\n", count);  // Prints: 0
    
    dmenv_destroy(ctx);
}
```

### Integration Example

```c
#include "dmenv.h"
#include "dmod.h"
#include <stdio.h>

void load_configuration(dmenv_ctx_t ctx) {
    // Load configuration from persistent storage or hardcode
    dmenv_set(ctx, "SYSTEM_NAME", "EmbeddedDevice");
    dmenv_set(ctx, "FIRMWARE_VERSION", "2.1.3");
    dmenv_set(ctx, "NETWORK_ENABLED", "true");
    dmenv_set(ctx, "NETWORK_IP", "192.168.1.100");
    dmenv_set(ctx, "NETWORK_PORT", "8080");
    dmenv_set(ctx, "SENSOR_SAMPLE_RATE", "1000");
    dmenv_set(ctx, "SENSOR_THRESHOLD", "75.5");
}

void print_all_network_settings(const char* name, const char* value, void* user_data) {
    printf("  %s = %s\n", name, value);
}

int main(void) {
    printf("=== Embedded System Configuration ===\n\n");
    
    // Create environment context
    dmenv_ctx_t ctx = dmenv_create(NULL);
    if (ctx == NULL) {
        printf("ERROR: Failed to create environment context\n");
        return -1;
    }
    
    // Load configuration
    load_configuration(ctx);
    
    printf("Configuration loaded: %zu variables\n\n", dmenv_count(ctx));
    
    // Display system info
    printf("System: %s\n", dmenv_get(ctx, "SYSTEM_NAME"));
    printf("Firmware: %s\n\n", dmenv_get(ctx, "FIRMWARE_VERSION"));
    
    // Display network settings
    printf("Network Configuration:\n");
    dmenv_find(ctx, "NETWORK_", print_all_network_settings, NULL);
    
    // Display sensor settings
    printf("\nSensor Configuration:\n");
    dmenv_find(ctx, "SENSOR_", print_all_network_settings, NULL);
    
    // Clean up
    dmenv_destroy(ctx);
    
    return 0;
}
```

## API Reference

### Context Management

#### `dmenv_create`

```c
dmenv_ctx_t dmenv_create(dmenv_ctx_t parent);
```

Create a new environment variables context.

- **Parameters:**
  - `parent`: Optional parent context for variable inheritance. If a variable is not found in the current context, it will be searched in the parent context. Pass NULL for no inheritance.
- **Returns:** Pointer to the created context, or NULL on failure
- **Thread-safe:** Yes

#### `dmenv_destroy`

```c
void dmenv_destroy(dmenv_ctx_t ctx);
```

Destroy an environment variables context.

- **Parameters:**
  - `ctx`: Context to destroy
- **Thread-safe:** Yes

#### `dmenv_is_valid`

```c
bool dmenv_is_valid(dmenv_ctx_t ctx);
```

Check if a context is valid.

- **Parameters:**
  - `ctx`: Context to check
- **Returns:** `true` if valid, `false` otherwise
- **Thread-safe:** Yes

#### `dmenv_set_as_default`

```c
void dmenv_set_as_default(dmenv_ctx_t ctx);
```

Set the default context.

- **Parameters:**
  - `ctx`: Context to set as default
- **Thread-safe:** Yes

#### `dmenv_get_default`

```c
dmenv_ctx_t dmenv_get_default(void);
```

Get the default context.

- **Returns:** Pointer to the default context, or NULL if not set
- **Thread-safe:** Yes

### Variable Operations

#### `dmenv_set`

```c
bool dmenv_set(dmenv_ctx_t ctx, const char* name, const char* value);
```

Set an environment variable. If the variable already exists, its value is updated.

- **Parameters:**
  - `ctx`: Context to set the variable in
  - `name`: Name of the environment variable
  - `value`: Value to set
- **Returns:** `true` if the variable was set successfully, `false` otherwise
- **Thread-safe:** Yes

#### `dmenv_get`

```c
const char* dmenv_get(dmenv_ctx_t ctx, const char* name);
```

Get an environment variable value. If the variable is not found in the context and the context has a parent, the parent will be searched recursively.

- **Parameters:**
  - `ctx`: Context to get the variable from
  - `name`: Name of the environment variable
- **Returns:** Pointer to the value string, or NULL if not found
- **Thread-safe:** Yes
- **Note:** The returned pointer is valid until the variable is removed or modified

#### `dmenv_seti`

```c
bool dmenv_seti(dmenv_ctx_t ctx, const char* name, uint32_t value);
```

Set an environment variable (unsigned integer value in hex). The value is stored internally as a hexadecimal string (e.g., "0x2000").

- **Parameters:**
  - `ctx`: Context to set the variable in
  - `name`: Name of the environment variable
  - `value`: Unsigned integer value to set
- **Returns:** `true` if the variable was set successfully, `false` otherwise
- **Thread-safe:** Yes

#### `dmenv_geti`

```c
bool dmenv_geti(dmenv_ctx_t ctx, const char* name, uint32_t* out_value);
```

Get an environment variable value (unsigned integer). Parses the value as a hexadecimal or decimal number. If the variable is not found in the context and the context has a parent, the parent will be searched.

- **Parameters:**
  - `ctx`: Context to get the variable from
  - `name`: Name of the environment variable
  - `out_value`: Pointer to store the parsed value
- **Returns:** `true` if the variable was found and parsed successfully, `false` otherwise
- **Thread-safe:** Yes

#### `dmenv_remove`

```c
bool dmenv_remove(dmenv_ctx_t ctx, const char* name);
```

Remove an environment variable. Only removes from the current context, not from parent contexts.

- **Parameters:**
  - `ctx`: Context to remove the variable from
  - `name`: Name of the environment variable to remove
- **Returns:** `true` if the variable was removed successfully, `false` if not found
- **Thread-safe:** Yes

#### `dmenv_clear`

```c
bool dmenv_clear(dmenv_ctx_t ctx);
```

Clear all environment variables in a context. Only clears variables in the current context, not in parent contexts.

- **Parameters:**
  - `ctx`: Context to clear
- **Returns:** `true` if all variables were cleared successfully, `false` otherwise
- **Thread-safe:** Yes

### Query Operations

#### `dmenv_find`

```c
size_t dmenv_find(dmenv_ctx_t ctx, const char* prefix, 
                  void (*callback)(const char* name, const char* value, void* user_data), 
                  void* user_data);
```

Find environment variables matching a prefix. Only searches in the current context, not in parent contexts.

- **Parameters:**
  - `ctx`: Context to search in
  - `prefix`: Prefix to match against variable names
  - `callback`: Callback function to call for each matching variable
  - `user_data`: User data to pass to the callback
- **Returns:** Number of matching variables found
- **Thread-safe:** Yes

#### `dmenv_count`

```c
size_t dmenv_count(dmenv_ctx_t ctx);
```

Get the number of environment variables in a context. Only counts variables in the current context, not in parent contexts.

- **Parameters:**
  - `ctx`: Context to count variables in
- **Returns:** Number of environment variables
- **Thread-safe:** Yes

## Contributing

Contributions are welcome! Please feel free to submit issues, fork the repository, and create pull requests.

### Development Setup

1. Fork the repository
2. Clone your fork: `git clone https://github.com/YOUR_USERNAME/dmenv.git`
3. Create a feature branch: `git checkout -b feature/my-new-feature`
4. Make your changes and add tests
5. Run tests: `cd build && ctest`
6. Commit your changes: `git commit -am 'Add some feature'`
7. Push to the branch: `git push origin feature/my-new-feature`
8. Submit a pull request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Related Projects

- [DMOD](https://github.com/choco-technologies/dmod) - Dynamic Module Loading Framework
- [dmheap](https://github.com/choco-technologies/dmheap) - DMOD Heap Memory Manager
- [dmlog](https://github.com/choco-technologies/dmlog) - DMOD Logging System

---

**For more information and support, please visit the [dmenv repository](https://github.com/choco-technologies/dmenv) or contact the Choco-Technologies team.**
