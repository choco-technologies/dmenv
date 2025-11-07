#include "test_common.h"
#include "dmenv.h"
#include "unity.h"
#include <string.h>

static char test_buffer[TEST_BUFFER_SIZE];

void setUp(void) {
    memset(test_buffer, 0, TEST_BUFFER_SIZE);
    dmenv_init(test_buffer, TEST_BUFFER_SIZE);
}

void tearDown(void) {
    if (dmenv_is_initialized()) {
        dmenv_clear();
    }
}

void test_init_success(void) {
    char buffer[1024];
    TEST_ASSERT_TRUE(dmenv_init(buffer, 1024));
    TEST_ASSERT_TRUE(dmenv_is_initialized());
}

void test_init_invalid_buffer(void) {
    TEST_ASSERT_FALSE(dmenv_init(NULL, 1024));
}

void test_init_too_small(void) {
    char buffer[10];
    TEST_ASSERT_FALSE(dmenv_init(buffer, 10));
}

void test_set_and_get(void) {
    TEST_ASSERT_TRUE(dmenv_set("MY_VAR", "my_value"));
    const char* value = dmenv_get("MY_VAR");
    TEST_ASSERT_NOT_NULL(value);
    TEST_ASSERT_EQUAL_STRING("my_value", value);
}

void test_set_update_existing(void) {
    TEST_ASSERT_TRUE(dmenv_set("VAR", "value1"));
    TEST_ASSERT_TRUE(dmenv_set("VAR", "value2"));
    const char* value = dmenv_get("VAR");
    TEST_ASSERT_EQUAL_STRING("value2", value);
}

void test_get_nonexistent(void) {
    const char* value = dmenv_get("NONEXISTENT");
    TEST_ASSERT_NULL(value);
}

void test_set_null_name(void) {
    TEST_ASSERT_FALSE(dmenv_set(NULL, "value"));
}

void test_set_null_value(void) {
    TEST_ASSERT_FALSE(dmenv_set("VAR", NULL));
}

void test_remove_existing(void) {
    TEST_ASSERT_TRUE(dmenv_set("TEMP", "temp_value"));
    TEST_ASSERT_TRUE(dmenv_remove("TEMP"));
    TEST_ASSERT_NULL(dmenv_get("TEMP"));
}

void test_remove_nonexistent(void) {
    TEST_ASSERT_FALSE(dmenv_remove("NONEXISTENT"));
}

void test_clear(void) {
    dmenv_set("VAR1", "value1");
    dmenv_set("VAR2", "value2");
    dmenv_set("VAR3", "value3");
    
    TEST_ASSERT_TRUE(dmenv_clear());
    TEST_ASSERT_EQUAL_size_t(0, dmenv_count());
}

void test_count(void) {
    TEST_ASSERT_EQUAL_size_t(0, dmenv_count());
    
    dmenv_set("VAR1", "value1");
    TEST_ASSERT_EQUAL_size_t(1, dmenv_count());
    
    dmenv_set("VAR2", "value2");
    TEST_ASSERT_EQUAL_size_t(2, dmenv_count());
    
    dmenv_remove("VAR1");
    TEST_ASSERT_EQUAL_size_t(1, dmenv_count());
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
    
    dmenv_set("PREFIX_VAR1", "value1");
    dmenv_set("PREFIX_VAR2", "value2");
    dmenv_set("OTHER_VAR", "value3");
    
    size_t found = dmenv_find("PREFIX_", find_test_callback, &ctx);
    TEST_ASSERT_EQUAL_size_t(2, found);
    TEST_ASSERT_EQUAL_size_t(2, ctx.count);
}

void test_find_no_matches(void) {
    find_test_context_t ctx = {0};
    
    dmenv_set("VAR1", "value1");
    dmenv_set("VAR2", "value2");
    
    size_t found = dmenv_find("NOMATCH_", find_test_callback, &ctx);
    TEST_ASSERT_EQUAL_size_t(0, found);
}

void test_multiple_variables(void) {
    // Test setting multiple variables
    for (int i = 0; i < 10; i++) {
        char name[32];
        char value[32];
        snprintf(name, sizeof(name), "VAR_%d", i);
        snprintf(value, sizeof(value), "value_%d", i);
        TEST_ASSERT_TRUE(dmenv_set(name, value));
    }
    
    TEST_ASSERT_EQUAL_size_t(10, dmenv_count());
    
    // Verify all values
    for (int i = 0; i < 10; i++) {
        char name[32];
        char expected[32];
        snprintf(name, sizeof(name), "VAR_%d", i);
        snprintf(expected, sizeof(expected), "value_%d", i);
        
        const char* value = dmenv_get(name);
        TEST_ASSERT_NOT_NULL(value);
        TEST_ASSERT_EQUAL_STRING(expected, value);
    }
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_init_success);
    RUN_TEST(test_init_invalid_buffer);
    RUN_TEST(test_init_too_small);
    RUN_TEST(test_set_and_get);
    RUN_TEST(test_set_update_existing);
    RUN_TEST(test_get_nonexistent);
    RUN_TEST(test_set_null_name);
    RUN_TEST(test_set_null_value);
    RUN_TEST(test_remove_existing);
    RUN_TEST(test_remove_nonexistent);
    RUN_TEST(test_clear);
    RUN_TEST(test_count);
    RUN_TEST(test_find_with_prefix);
    RUN_TEST(test_find_no_matches);
    RUN_TEST(test_multiple_variables);
    
    return UNITY_END();
}
