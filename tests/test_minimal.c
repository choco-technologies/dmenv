#include "test_common.h"
#include "dmenv.h"
#include <stdio.h>
#include <string.h>

static char test_buffer[TEST_BUFFER_SIZE];

int main(void) {
    printf("=== Minimal DMENV Test ===\n");
    
    // Test initialization
    if (!dmenv_init(test_buffer, TEST_BUFFER_SIZE)) {
        printf("Init: FAIL\n");
        return 1;
    }
    printf("Init: PASS\n");
    
    // Test is_initialized
    if (!dmenv_is_initialized()) {
        printf("Is Initialized: FAIL\n");
        return 1;
    }
    printf("Is Initialized: PASS\n");
    
    // Test set
    if (!dmenv_set("TEST", "value")) {
        printf("Set: FAIL\n");
        return 1;
    }
    printf("Set: PASS\n");
    
    // Test get
    const char* value = dmenv_get("TEST");
    if (value == NULL || strcmp(value, "value") != 0) {
        printf("Get: FAIL\n");
        return 1;
    }
    printf("Get: PASS\n");
    
    // Test count
    if (dmenv_count() != 1) {
        printf("Count: FAIL (expected 1, got %zu)\n", dmenv_count());
        return 1;
    }
    printf("Count: PASS\n");
    
    // Test remove
    if (!dmenv_remove("TEST")) {
        printf("Remove: FAIL\n");
        return 1;
    }
    printf("Remove: PASS\n");
    
    // Test count after remove
    if (dmenv_count() != 0) {
        printf("Count After Remove: FAIL (expected 0, got %zu)\n", dmenv_count());
        return 1;
    }
    printf("Count After Remove: PASS\n");
    
    printf("\nAll minimal tests passed!\n");
    
    return 0;
}
