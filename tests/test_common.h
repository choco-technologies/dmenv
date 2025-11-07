#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include "dmenv.h"
#include <stdio.h>

/**
 * @brief Common test buffer size
 */
#define TEST_BUFFER_SIZE (16 * 1024)

/**
 * @brief Test helper to print results
 */
#define TEST_PRINT_RESULT(name, result) \
    printf("%s: %s\n", name, (result) ? "PASS" : "FAIL")

/**
 * @brief Test helper to verify condition
 */
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            printf("ASSERT FAILED: %s at %s:%d\n", message, __FILE__, __LINE__); \
            return false; \
        } \
    } while(0)

#endif // TEST_COMMON_H
