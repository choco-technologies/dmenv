/**
 * @file coverage_stubs.c
 * @brief Stub symbols for coverage builds without the embedded linker script.
 * 
 * When building with coverage enabled, we skip the custom linker script that
 * defines these symbols. This file provides stub definitions to satisfy the linker.
 */

/* Stub sections for DMOD input/output APIs */
char __dmod_inputs_start = 0;
char __dmod_inputs_end = 0;
char __dmod_inputs_size = 0;
char __dmod_outputs_start = 0;
char __dmod_outputs_end = 0;
char __dmod_outputs_size = 0;
