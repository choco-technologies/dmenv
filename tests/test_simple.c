#include "test_common.h"
#include "dmenv.h"
#include <stdio.h>
#include <string.h>

static char test_buffer[TEST_BUFFER_SIZE];

bool test_init(void) {
    bool result = dmenv_init(test_buffer, TEST_BUFFER_SIZE);
    TEST_ASSERT(result, "Init should succeed");
    TEST_ASSERT(dmenv_is_initialized(), "Should be initialized");
    return true;
}

bool test_set_get(void) {
    const char* value;
    
    // Set a variable
    TEST_ASSERT(dmenv_set("TEST_VAR", "test_value"), "Set should succeed");
    
    // Get the variable
    value = dmenv_get("TEST_VAR");
    TEST_ASSERT(value != NULL, "Get should return a value");
    TEST_ASSERT(strcmp(value, "test_value") == 0, "Value should match");
    
    return true;
}

bool test_remove(void) {
    // Set a variable
    TEST_ASSERT(dmenv_set("REMOVE_ME", "temporary"), "Set should succeed");
    
    // Verify it exists
    TEST_ASSERT(dmenv_get("REMOVE_ME") != NULL, "Variable should exist");
    
    // Remove it
    TEST_ASSERT(dmenv_remove("REMOVE_ME"), "Remove should succeed");
    
    // Verify it's gone
    TEST_ASSERT(dmenv_get("REMOVE_ME") == NULL, "Variable should not exist");
    
    return true;
}

bool test_clear(void) {
    // Set some variables
    dmenv_set("VAR1", "value1");
    dmenv_set("VAR2", "value2");
    dmenv_set("VAR3", "value3");
    
    TEST_ASSERT(dmenv_count() >= 3, "Should have at least 3 variables");
    
    // Clear all
    TEST_ASSERT(dmenv_clear(), "Clear should succeed");
    
    // Verify count is 0
    TEST_ASSERT(dmenv_count() == 0, "Count should be 0 after clear");
    
    return true;
}

typedef struct {
    size_t count;
} find_context_t;

void find_callback(const char* name, const char* value, void* user_data) {
    find_context_t* ctx = (find_context_t*)user_data;
    ctx->count++;
    printf("  Found: %s = %s\n", name, value);
}

bool test_find(void) {
    find_context_t ctx = {0};
    
    // Set variables with different prefixes
    dmenv_set("APP_NAME", "myapp");
    dmenv_set("APP_VERSION", "1.0");
    dmenv_set("APP_DEBUG", "true");
    dmenv_set("SYS_PATH", "/usr/bin");
    dmenv_set("SYS_USER", "admin");
    
    // Find all APP_ variables
    size_t found = dmenv_find("APP_", find_callback, &ctx);
    TEST_ASSERT(found == 3, "Should find 3 APP_ variables");
    TEST_ASSERT(ctx.count == 3, "Callback should be called 3 times");
    
    // Reset context
    ctx.count = 0;
    
    // Find all SYS_ variables
    found = dmenv_find("SYS_", find_callback, &ctx);
    TEST_ASSERT(found == 2, "Should find 2 SYS_ variables");
    TEST_ASSERT(ctx.count == 2, "Callback should be called 2 times");
    
    return true;
}

int main(void) {
    printf("=== DMENV Simple Test ===\n\n");
    
    TEST_PRINT_RESULT("Init", test_init());
    TEST_PRINT_RESULT("Set/Get", test_set_get());
    TEST_PRINT_RESULT("Remove", test_remove());
    TEST_PRINT_RESULT("Clear", test_clear());
    TEST_PRINT_RESULT("Find", test_find());
    
    printf("\nAll simple tests completed!\n");
    
    return 0;
}
