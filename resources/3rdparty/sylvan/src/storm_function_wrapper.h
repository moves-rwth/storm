#ifndef SYLVAN_STORM_FUNCTION_WRAPPER_H
#define SYLVAN_STORM_FUNCTION_WRAPPER_H

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* storm_rational_function_ptr;

// equals, plus, minus, divide, times, create, destroy
void storm_rational_function_init(storm_rational_function_ptr* a);
void storm_rational_function_destroy(storm_rational_function_ptr a);
int storm_rational_function_equals(storm_rational_function_ptr a, storm_rational_function_ptr b);
storm_rational_function_ptr storm_rational_function_plus(storm_rational_function_ptr a, storm_rational_function_ptr b);
storm_rational_function_ptr storm_rational_function_minus(storm_rational_function_ptr a, storm_rational_function_ptr b);
storm_rational_function_ptr storm_rational_function_times(storm_rational_function_ptr a, storm_rational_function_ptr b);
storm_rational_function_ptr storm_rational_function_divide(storm_rational_function_ptr a, storm_rational_function_ptr b);
storm_rational_function_ptr storm_rational_function_negate(storm_rational_function_ptr a);
uint64_t storm_rational_function_hash(storm_rational_function_ptr const a, uint64_t const seed);
int storm_rational_function_is_zero(storm_rational_function_ptr a);

storm_rational_function_ptr storm_rational_function_get_zero();
storm_rational_function_ptr storm_rational_function_get_one();

void print_storm_rational_function(storm_rational_function_ptr a);
void print_storm_rational_function_to_file(storm_rational_function_ptr a, FILE* out);

int storm_rational_function_is_zero(storm_rational_function_ptr a);

#ifdef __cplusplus
}
#endif

#endif // SYLVAN_STORM_FUNCTION_WRAPPER_H
