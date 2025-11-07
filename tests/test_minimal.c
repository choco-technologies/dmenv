#include "test_common.h"
#include "dmenv.h"
#include <stdio.h>
#include <string.h>

int main(void) {
    printf("=== Minimal DMENV Test ===\n");
    
    // Test create context
    dmenv_ctx_t ctx = dmenv_create(NULL);
    if (!ctx) {
        printf("Create: FAIL\n");
        return 1;
    }
    printf("Create: PASS\n");
    
    // Test is_valid
    if (!dmenv_is_valid(ctx)) {
        printf("Is Valid: FAIL\n");
        return 1;
    }
    printf("Is Valid: PASS\n");
    
    // Test set
    if (!dmenv_set(ctx, "TEST", "value")) {
        printf("Set: FAIL\n");
        return 1;
    }
    printf("Set: PASS\n");
    
    // Test get
    const char* value = dmenv_get(ctx, "TEST");
    if (value == NULL || strcmp(value, "value") != 0) {
        printf("Get: FAIL\n");
        return 1;
    }
    printf("Get: PASS\n");
    
    // Test count
    if (dmenv_count(ctx) != 1) {
        printf("Count: FAIL (expected 1, got %zu)\n", dmenv_count(ctx));
        return 1;
    }
    printf("Count: PASS\n");
    
    // Test remove
    if (!dmenv_remove(ctx, "TEST")) {
        printf("Remove: FAIL\n");
        return 1;
    }
    printf("Remove: PASS\n");
    
    // Test count after remove
    if (dmenv_count(ctx) != 0) {
        printf("Count After Remove: FAIL (expected 0, got %zu)\n", dmenv_count(ctx));
        return 1;
    }
    printf("Count After Remove: PASS\n");
    
    // Test integer set/get
    if (!dmenv_seti(ctx, "NUM", 0x2000)) {
        printf("Set Integer: FAIL\n");
        return 1;
    }
    printf("Set Integer: PASS\n");
    
    uint32_t num_value;
    if (!dmenv_geti(ctx, "NUM", &num_value) || num_value != 0x2000) {
        printf("Get Integer: FAIL (got 0x%X)\n", num_value);
        return 1;
    }
    printf("Get Integer: PASS\n");
    
    // Clean up
    dmenv_destroy(ctx);
    printf("Destroy: PASS\n");
    
    printf("\nAll minimal tests passed!\n");
    
    return 0;
}
