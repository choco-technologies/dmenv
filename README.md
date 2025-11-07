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

dmenv is a custom environment variables manager that integrates seamlessly with the DMOD dynamic module system. It provides a static-buffer-based solution for storing and retrieving configuration parameters, making it ideal for embedded systems where dynamic memory allocation may be limited or undesirable.

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
- **Static buffer management**: Operates on a pre-allocated buffer (no reliance on system malloc)
- **Thread-safe operations**: Uses DMOD's critical section mechanisms
- **Prefix-based search**: Find all variables matching a specific prefix
- **Efficient lookup**: Fast variable retrieval using linked list structure
- **Module integration**: Seamless integration with DMOD logging and error handling

dmenv is ideal for storing application configuration, runtime parameters, and inter-module communication data in embedded DMOD-based systems.

## Features

- ✅ **Key-value storage**: Store and retrieve environment variables by name
- ✅ **Variable update**: Update existing variables without creating duplicates
- ✅ **Variable removal**: Remove individual variables or clear all at once
- ✅ **Prefix search**: Find all variables matching a given prefix
- ✅ **Variable counting**: Track the number of stored variables
- ✅ **Static buffer operation**: No dependency on system malloc/free
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
   - Initialization
   - Basic set/get operations
   - Count and remove operations

2. **test_simple**: Simple functional tests
   - Set and get variables
   - Remove variables
   - Clear all variables
   - Find variables by prefix

3. **test_dmenv_unit**: Comprehensive unit tests using Unity framework
   - Initialization with various conditions
   - Set/get operations
   - Update existing variables
   - Remove operations
   - Clear all variables
   - Count operations
   - Find operations with prefixes
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

// 1. Define your buffer
#define ENV_BUFFER_SIZE (8 * 1024)  // 8KB
static char env_buffer[ENV_BUFFER_SIZE];

int main(void) {
    // 2. Initialize the environment manager
    bool success = dmenv_init(env_buffer, ENV_BUFFER_SIZE);
    if (!success) {
        // Handle initialization failure
        return -1;
    }
    
    // 3. Set environment variables
    dmenv_set("APP_NAME", "MyApplication");
    dmenv_set("APP_VERSION", "1.0.0");
    dmenv_set("DEBUG_MODE", "true");
    
    // 4. Get environment variables
    const char* app_name = dmenv_get("APP_NAME");
    if (app_name != NULL) {
        printf("Application: %s\n", app_name);
    }
    
    return 0;
}
```

### Update Variables

```c
#include "dmenv.h"

void config_example(void) {
    // Set initial value
    dmenv_set("LOG_LEVEL", "INFO");
    
    // Update the value (no duplicate created)
    dmenv_set("LOG_LEVEL", "DEBUG");
    
    // Get updated value
    const char* level = dmenv_get("LOG_LEVEL");
    // level now contains "DEBUG"
}
```

### Remove Variables

```c
#include "dmenv.h"

void cleanup_example(void) {
    // Set a temporary variable
    dmenv_set("TEMP_DATA", "temporary_value");
    
    // Remove it when done
    bool removed = dmenv_remove("TEMP_DATA");
    if (removed) {
        printf("Temporary data removed\n");
    }
    
    // Variable no longer exists
    const char* value = dmenv_get("TEMP_DATA");
    // value is NULL
}
```

### Find Variables by Prefix

```c
#include "dmenv.h"

void print_var(const char* name, const char* value, void* user_data) {
    printf("%s = %s\n", name, value);
}

void find_example(void) {
    // Set related variables with common prefix
    dmenv_set("DB_HOST", "localhost");
    dmenv_set("DB_PORT", "5432");
    dmenv_set("DB_NAME", "mydb");
    dmenv_set("APP_NAME", "myapp");
    
    // Find all database-related variables
    printf("Database configuration:\n");
    size_t found = dmenv_find("DB_", print_var, NULL);
    printf("Found %zu database variables\n", found);
}
```

### Clear All Variables

```c
#include "dmenv.h"

void reset_example(void) {
    // Clear all environment variables
    dmenv_clear();
    
    // Verify count is 0
    size_t count = dmenv_count();
    printf("Variables remaining: %zu\n", count);  // Prints: 0
}
```

### Integration Example

```c
#include "dmenv.h"
#include "dmod.h"
#include <stdio.h>

#define ENV_SIZE (16 * 1024)
static char env_buffer[ENV_SIZE];

void load_configuration(void) {
    // Load configuration from persistent storage or hardcode
    dmenv_set("SYSTEM_NAME", "EmbeddedDevice");
    dmenv_set("FIRMWARE_VERSION", "2.1.3");
    dmenv_set("NETWORK_ENABLED", "true");
    dmenv_set("NETWORK_IP", "192.168.1.100");
    dmenv_set("NETWORK_PORT", "8080");
    dmenv_set("SENSOR_SAMPLE_RATE", "1000");
    dmenv_set("SENSOR_THRESHOLD", "75.5");
}

void print_all_network_settings(const char* name, const char* value, void* user_data) {
    printf("  %s = %s\n", name, value);
}

int main(void) {
    printf("=== Embedded System Configuration ===\n\n");
    
    // Initialize environment manager
    if (!dmenv_init(env_buffer, ENV_SIZE)) {
        printf("ERROR: Failed to initialize environment manager\n");
        return -1;
    }
    
    // Load configuration
    load_configuration();
    
    printf("Configuration loaded: %zu variables\n\n", dmenv_count());
    
    // Display system info
    printf("System: %s\n", dmenv_get("SYSTEM_NAME"));
    printf("Firmware: %s\n\n", dmenv_get("FIRMWARE_VERSION"));
    
    // Display network settings
    printf("Network Configuration:\n");
    dmenv_find("NETWORK_", print_all_network_settings, NULL);
    
    // Display sensor settings
    printf("\nSensor Configuration:\n");
    dmenv_find("SENSOR_", print_all_network_settings, NULL);
    
    return 0;
}
```

## API Reference

### Initialization

#### `dmenv_init`

```c
bool dmenv_init(void* buffer, size_t size);
```

Initialize the environment variables manager with a buffer.

- **Parameters:**
  - `buffer`: Pointer to the memory buffer to be used for storage
  - `size`: Size of the memory buffer in bytes
- **Returns:** `true` if initialization is successful, `false` otherwise
- **Thread-safe:** Yes

#### `dmenv_is_initialized`

```c
bool dmenv_is_initialized(void);
```

Check if the environment variables manager is initialized.

- **Returns:** `true` if initialized, `false` otherwise
- **Thread-safe:** Yes

### Variable Operations

#### `dmenv_set`

```c
bool dmenv_set(const char* name, const char* value);
```

Set an environment variable. If the variable already exists, its value is updated.

- **Parameters:**
  - `name`: Name of the environment variable (max 63 characters)
  - `value`: Value to set (max 255 characters)
- **Returns:** `true` if the variable was set successfully, `false` otherwise
- **Thread-safe:** Yes

#### `dmenv_get`

```c
const char* dmenv_get(const char* name);
```

Get an environment variable value.

- **Parameters:**
  - `name`: Name of the environment variable
- **Returns:** Pointer to the value string, or NULL if not found
- **Thread-safe:** Yes
- **Note:** The returned pointer is valid until the variable is removed or modified

#### `dmenv_remove`

```c
bool dmenv_remove(const char* name);
```

Remove an environment variable.

- **Parameters:**
  - `name`: Name of the environment variable to remove
- **Returns:** `true` if the variable was removed successfully, `false` if not found
- **Thread-safe:** Yes

#### `dmenv_clear`

```c
bool dmenv_clear(void);
```

Clear all environment variables.

- **Returns:** `true` if all variables were cleared successfully, `false` otherwise
- **Thread-safe:** Yes

### Query Operations

#### `dmenv_find`

```c
size_t dmenv_find(const char* prefix, 
                  void (*callback)(const char* name, const char* value, void* user_data), 
                  void* user_data);
```

Find environment variables matching a prefix.

- **Parameters:**
  - `prefix`: Prefix to match against variable names
  - `callback`: Callback function to call for each matching variable
  - `user_data`: User data to pass to the callback
- **Returns:** Number of matching variables found
- **Thread-safe:** Yes

#### `dmenv_count`

```c
size_t dmenv_count(void);
```

Get the number of environment variables currently stored.

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
