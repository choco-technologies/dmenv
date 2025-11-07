#include "test_common.h"
#include "dmenv.h"
#include <stdio.h>
#include <string.h>

bool test_create(void) {
    dmenv_ctx_t ctx = dmenv_create(NULL);
    TEST_ASSERT(ctx != NULL, "Create should succeed");
    TEST_ASSERT(dmenv_is_valid(ctx), "Context should be valid");
    dmenv_destroy(ctx);
    return true;
}

bool test_set_get(void) {
    dmenv_ctx_t ctx = dmenv_create(NULL);
    const char* value;
    
    // Set a variable
    TEST_ASSERT(dmenv_set(ctx, "TEST_VAR", "test_value"), "Set should succeed");
    
    // Get the variable
    value = dmenv_get(ctx, "TEST_VAR");
    TEST_ASSERT(value != NULL, "Get should return a value");
    TEST_ASSERT(strcmp(value, "test_value") == 0, "Value should match");
    
    dmenv_destroy(ctx);
    return true;
}

bool test_remove(void) {
    dmenv_ctx_t ctx = dmenv_create(NULL);
    
    // Set a variable
    TEST_ASSERT(dmenv_set(ctx, "REMOVE_ME", "temporary"), "Set should succeed");
    
    // Verify it exists
    TEST_ASSERT(dmenv_get(ctx, "REMOVE_ME") != NULL, "Variable should exist");
    
    // Remove it
    TEST_ASSERT(dmenv_remove(ctx, "REMOVE_ME"), "Remove should succeed");
    
    // Verify it's gone
    TEST_ASSERT(dmenv_get(ctx, "REMOVE_ME") == NULL, "Variable should not exist");
    
    dmenv_destroy(ctx);
    return true;
}

bool test_clear(void) {
    dmenv_ctx_t ctx = dmenv_create(NULL);
    
    // Set some variables
    dmenv_set(ctx, "VAR1", "value1");
    dmenv_set(ctx, "VAR2", "value2");
    dmenv_set(ctx, "VAR3", "value3");
    
    TEST_ASSERT(dmenv_count(ctx) >= 3, "Should have at least 3 variables");
    
    // Clear all
    TEST_ASSERT(dmenv_clear(ctx), "Clear should succeed");
    
    // Verify count is 0
    TEST_ASSERT(dmenv_count(ctx) == 0, "Count should be 0 after clear");
    
    dmenv_destroy(ctx);
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
    dmenv_ctx_t ctx = dmenv_create(NULL);
    find_context_t find_ctx = {0};
    
    // Set variables with different prefixes
    dmenv_set(ctx, "APP_NAME", "myapp");
    dmenv_set(ctx, "APP_VERSION", "1.0");
    dmenv_set(ctx, "APP_DEBUG", "true");
    dmenv_set(ctx, "SYS_PATH", "/usr/bin");
    dmenv_set(ctx, "SYS_USER", "admin");
    
    // Find all APP_ variables
    size_t found = dmenv_find(ctx, "APP_", find_callback, &find_ctx);
    TEST_ASSERT(found == 3, "Should find 3 APP_ variables");
    TEST_ASSERT(find_ctx.count == 3, "Callback should be called 3 times");
    
    // Reset context
    find_ctx.count = 0;
    
    // Find all SYS_ variables
    found = dmenv_find(ctx, "SYS_", find_callback, &find_ctx);
    TEST_ASSERT(found == 2, "Should find 2 SYS_ variables");
    TEST_ASSERT(find_ctx.count == 2, "Callback should be called 2 times");
    
    dmenv_destroy(ctx);
    return true;
}

bool test_integer(void) {
    dmenv_ctx_t ctx = dmenv_create(NULL);
    uint32_t value;
    
    // Set integer value
    TEST_ASSERT(dmenv_seti(ctx, "NUM", 0x2000), "Set integer should succeed");
    
    // Get integer value
    TEST_ASSERT(dmenv_geti(ctx, "NUM", &value), "Get integer should succeed");
    TEST_ASSERT(value == 0x2000, "Integer value should match");
    
    dmenv_destroy(ctx);
    return true;
}

bool test_inheritance(void) {
    // Create parent context
    dmenv_ctx_t parent = dmenv_create(NULL);
    dmenv_set(parent, "PARENT_VAR", "from_parent");
    dmenv_set(parent, "OVERRIDE_VAR", "parent_value");
    
    // Create child context with parent
    dmenv_ctx_t child = dmenv_create(parent);
    dmenv_set(child, "CHILD_VAR", "from_child");
    dmenv_set(child, "OVERRIDE_VAR", "child_value");
    
    // Test inheritance
    const char* parent_var = dmenv_get(child, "PARENT_VAR");
    TEST_ASSERT(parent_var != NULL, "Should find parent variable");
    TEST_ASSERT(strcmp(parent_var, "from_parent") == 0, "Parent value should match");
    
    // Test child variable
    const char* child_var = dmenv_get(child, "CHILD_VAR");
    TEST_ASSERT(child_var != NULL, "Should find child variable");
    TEST_ASSERT(strcmp(child_var, "from_child") == 0, "Child value should match");
    
    // Test override
    const char* override_var = dmenv_get(child, "OVERRIDE_VAR");
    TEST_ASSERT(override_var != NULL, "Should find overridden variable");
    TEST_ASSERT(strcmp(override_var, "child_value") == 0, "Should get child value");
    
    dmenv_destroy(child);
    dmenv_destroy(parent);
    return true;
}

int main(void) {
    printf("=== DMENV Simple Test ===\n\n");
    
    TEST_PRINT_RESULT("Create", test_create());
    TEST_PRINT_RESULT("Set/Get", test_set_get());
    TEST_PRINT_RESULT("Remove", test_remove());
    TEST_PRINT_RESULT("Clear", test_clear());
    TEST_PRINT_RESULT("Find", test_find());
    TEST_PRINT_RESULT("Integer", test_integer());
    TEST_PRINT_RESULT("Inheritance", test_inheritance());
    
    printf("\nAll simple tests completed!\n");
    
    return 0;
}
