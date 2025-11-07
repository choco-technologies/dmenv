#include "test_common.h"
#include "dmenv.h"
#include "unity.h"
#include <string.h>

static dmenv_ctx_t test_ctx;

void setUp(void) {
    test_ctx = dmenv_create(NULL);
}

void tearDown(void) {
    if (dmenv_is_valid(test_ctx)) {
        dmenv_destroy(test_ctx);
    }
}

void test_create_success(void) {
    dmenv_ctx_t ctx = dmenv_create(NULL);
    TEST_ASSERT_TRUE(dmenv_is_valid(ctx));
    dmenv_destroy(ctx);
}

void test_create_with_parent(void) {
    dmenv_ctx_t parent = dmenv_create(NULL);
    dmenv_ctx_t child = dmenv_create(parent);
    TEST_ASSERT_TRUE(dmenv_is_valid(child));
    dmenv_destroy(child);
    dmenv_destroy(parent);
}

void test_set_and_get(void) {
    TEST_ASSERT_TRUE(dmenv_set(test_ctx, "MY_VAR", "my_value"));
    const char* value = dmenv_get(test_ctx, "MY_VAR");
    TEST_ASSERT_NOT_NULL(value);
    TEST_ASSERT_EQUAL_STRING("my_value", value);
}

void test_set_update_existing(void) {
    TEST_ASSERT_TRUE(dmenv_set(test_ctx, "VAR", "value1"));
    TEST_ASSERT_TRUE(dmenv_set(test_ctx, "VAR", "value2"));
    const char* value = dmenv_get(test_ctx, "VAR");
    TEST_ASSERT_EQUAL_STRING("value2", value);
}

void test_get_nonexistent(void) {
    const char* value = dmenv_get(test_ctx, "NONEXISTENT");
    TEST_ASSERT_NULL(value);
}

void test_set_null_ctx(void) {
    TEST_ASSERT_FALSE(dmenv_set(NULL, "VAR", "value"));
}

void test_set_null_name(void) {
    TEST_ASSERT_FALSE(dmenv_set(test_ctx, NULL, "value"));
}

void test_set_null_value(void) {
    TEST_ASSERT_FALSE(dmenv_set(test_ctx, "VAR", NULL));
}

void test_remove_existing(void) {
    TEST_ASSERT_TRUE(dmenv_set(test_ctx, "TEMP", "temp_value"));
    TEST_ASSERT_TRUE(dmenv_remove(test_ctx, "TEMP"));
    TEST_ASSERT_NULL(dmenv_get(test_ctx, "TEMP"));
}

void test_remove_nonexistent(void) {
    TEST_ASSERT_FALSE(dmenv_remove(test_ctx, "NONEXISTENT"));
}

void test_clear(void) {
    dmenv_set(test_ctx, "VAR1", "value1");
    dmenv_set(test_ctx, "VAR2", "value2");
    dmenv_set(test_ctx, "VAR3", "value3");
    
    TEST_ASSERT_TRUE(dmenv_clear(test_ctx));
    TEST_ASSERT_EQUAL_size_t(0, dmenv_count(test_ctx));
}

void test_count(void) {
    TEST_ASSERT_EQUAL_size_t(0, dmenv_count(test_ctx));
    
    dmenv_set(test_ctx, "VAR1", "value1");
    TEST_ASSERT_EQUAL_size_t(1, dmenv_count(test_ctx));
    
    dmenv_set(test_ctx, "VAR2", "value2");
    TEST_ASSERT_EQUAL_size_t(2, dmenv_count(test_ctx));
    
    dmenv_remove(test_ctx, "VAR1");
    TEST_ASSERT_EQUAL_size_t(1, dmenv_count(test_ctx));
}

typedef struct {
    size_t count;
    char names[10][64];
} find_test_context_t;

void find_test_callback(const char* name, const char* value, void* user_data) {
    find_test_context_t* ctx = (find_test_context_t*)user_data;
    if (ctx->count < 10) {
        strncpy(ctx->names[ctx->count], name, 63);
        ctx->names[ctx->count][63] = '\0';
        ctx->count++;
    }
}

void test_find_with_prefix(void) {
    find_test_context_t ctx = {0};
    
    dmenv_set(test_ctx, "PREFIX_VAR1", "value1");
    dmenv_set(test_ctx, "PREFIX_VAR2", "value2");
    dmenv_set(test_ctx, "OTHER_VAR", "value3");
    
    size_t found = dmenv_find(test_ctx, "PREFIX_", find_test_callback, &ctx);
    TEST_ASSERT_EQUAL_size_t(2, found);
    TEST_ASSERT_EQUAL_size_t(2, ctx.count);
}

void test_find_no_matches(void) {
    find_test_context_t ctx = {0};
    
    dmenv_set(test_ctx, "VAR1", "value1");
    dmenv_set(test_ctx, "VAR2", "value2");
    
    size_t found = dmenv_find(test_ctx, "NOMATCH_", find_test_callback, &ctx);
    TEST_ASSERT_EQUAL_size_t(0, found);
}

void test_multiple_variables(void) {
    // Test setting multiple variables
    for (int i = 0; i < 10; i++) {
        char name[32];
        char value[32];
        snprintf(name, sizeof(name), "VAR_%d", i);
        snprintf(value, sizeof(value), "value_%d", i);
        TEST_ASSERT_TRUE(dmenv_set(test_ctx, name, value));
    }
    
    TEST_ASSERT_EQUAL_size_t(10, dmenv_count(test_ctx));
    
    // Verify all values
    for (int i = 0; i < 10; i++) {
        char name[32];
        char expected[32];
        snprintf(name, sizeof(name), "VAR_%d", i);
        snprintf(expected, sizeof(expected), "value_%d", i);
        
        const char* value = dmenv_get(test_ctx, name);
        TEST_ASSERT_NOT_NULL(value);
        TEST_ASSERT_EQUAL_STRING(expected, value);
    }
}

void test_seti_and_geti(void) {
    uint32_t value;
    
    TEST_ASSERT_TRUE(dmenv_seti(test_ctx, "NUM", 0x2000));
    TEST_ASSERT_TRUE(dmenv_geti(test_ctx, "NUM", &value));
    TEST_ASSERT_EQUAL_UINT32(0x2000, value);
}

void test_geti_decimal(void) {
    uint32_t value;
    
    // Set as string with decimal value
    TEST_ASSERT_TRUE(dmenv_set(test_ctx, "DEC_NUM", "12345"));
    TEST_ASSERT_TRUE(dmenv_geti(test_ctx, "DEC_NUM", &value));
    TEST_ASSERT_EQUAL_UINT32(12345, value);
}

void test_inheritance(void) {
    dmenv_ctx_t parent = dmenv_create(NULL);
    dmenv_ctx_t child = dmenv_create(parent);
    
    dmenv_set(parent, "PARENT_VAR", "parent_value");
    dmenv_set(child, "CHILD_VAR", "child_value");
    
    // Child should find parent variable
    const char* parent_val = dmenv_get(child, "PARENT_VAR");
    TEST_ASSERT_NOT_NULL(parent_val);
    TEST_ASSERT_EQUAL_STRING("parent_value", parent_val);
    
    // Child should find its own variable
    const char* child_val = dmenv_get(child, "CHILD_VAR");
    TEST_ASSERT_NOT_NULL(child_val);
    TEST_ASSERT_EQUAL_STRING("child_value", child_val);
    
    // Parent should not find child variable
    const char* not_found = dmenv_get(parent, "CHILD_VAR");
    TEST_ASSERT_NULL(not_found);
    
    dmenv_destroy(child);
    dmenv_destroy(parent);
}

void test_inheritance_override(void) {
    dmenv_ctx_t parent = dmenv_create(NULL);
    dmenv_ctx_t child = dmenv_create(parent);
    
    dmenv_set(parent, "VAR", "parent_value");
    dmenv_set(child, "VAR", "child_value");
    
    // Child should get its own value, not parent's
    const char* val = dmenv_get(child, "VAR");
    TEST_ASSERT_NOT_NULL(val);
    TEST_ASSERT_EQUAL_STRING("child_value", val);
    
    dmenv_destroy(child);
    dmenv_destroy(parent);
}

void test_default_context(void) {
    dmenv_ctx_t ctx = dmenv_create(NULL);
    dmenv_set_as_default(ctx);
    
    dmenv_ctx_t retrieved = dmenv_get_default();
    TEST_ASSERT_EQUAL_PTR(ctx, retrieved);
    
    dmenv_destroy(ctx);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_create_success);
    RUN_TEST(test_create_with_parent);
    RUN_TEST(test_set_and_get);
    RUN_TEST(test_set_update_existing);
    RUN_TEST(test_get_nonexistent);
    RUN_TEST(test_set_null_ctx);
    RUN_TEST(test_set_null_name);
    RUN_TEST(test_set_null_value);
    RUN_TEST(test_remove_existing);
    RUN_TEST(test_remove_nonexistent);
    RUN_TEST(test_clear);
    RUN_TEST(test_count);
    RUN_TEST(test_find_with_prefix);
    RUN_TEST(test_find_no_matches);
    RUN_TEST(test_multiple_variables);
    RUN_TEST(test_seti_and_geti);
    RUN_TEST(test_geti_decimal);
    RUN_TEST(test_inheritance);
    RUN_TEST(test_inheritance_override);
    RUN_TEST(test_default_context);
    
    return UNITY_END();
}
