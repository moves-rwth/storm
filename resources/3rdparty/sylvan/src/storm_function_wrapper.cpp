#include "storm_function_wrapper.h"

#include <cstring>
#include "src/adapters/CarlAdapter.h"

void storm_rational_function_init(storm_rational_function_ptr* a) {
	storm_rational_function_ptr srf_ptr = static_cast<storm_rational_function_ptr>(malloc(sizeof(storm_rational_function_ptr_struct)));
	
	if (srf_ptr == nullptr) {
		return;
	}
	
	srf_ptr->storm_rational_function = new storm::RationalFunction(*(storm::RationalFunction*)((*a)->storm_rational_function));

	*a = srf_ptr;
}

void storm_rational_function_destroy(storm_rational_function_ptr a) {
	delete (storm::RationalFunction*)a->storm_rational_function;
	a->storm_rational_function = nullptr;
	free((void*)a);
}

int storm_rational_function_equals(storm_rational_function_ptr a, storm_rational_function_ptr b) {
	storm::RationalFunction* srf_a = (storm::RationalFunction*)a->storm_rational_function;
	storm::RationalFunction* srf_b = (storm::RationalFunction*)b->storm_rational_function;

	if (*srf_a == *srf_b) {
		return 0;
	}

	return -1;
}

storm_rational_function_ptr storm_rational_function_plus(storm_rational_function_ptr a, storm_rational_function_ptr b) {
	storm::RationalFunction& srf_a = *(storm::RationalFunction*)a->storm_rational_function;
	storm::RationalFunction& srf_b = *(storm::RationalFunction*)b->storm_rational_function;

	storm::RationalFunction* result_srf = new storm::RationalFunction(srf_a);
	*result_srf += srf_b;

	storm_rational_function_ptr result = (storm_rational_function_ptr)malloc(sizeof(storm_rational_function_ptr_struct));
	result->storm_rational_function = (void*)result_srf;

	return result;
}

storm_rational_function_ptr storm_rational_function_minus(storm_rational_function_ptr a, storm_rational_function_ptr b) {
	storm::RationalFunction& srf_a = *(storm::RationalFunction*)a->storm_rational_function;
	storm::RationalFunction& srf_b = *(storm::RationalFunction*)b->storm_rational_function;

	storm::RationalFunction* result_srf = new storm::RationalFunction(srf_a);
	*result_srf -= srf_b;

	storm_rational_function_ptr result = (storm_rational_function_ptr)malloc(sizeof(storm_rational_function_ptr_struct));
	result->storm_rational_function = (void*)result_srf;

	return result;
}

storm_rational_function_ptr storm_rational_function_times(storm_rational_function_ptr a, storm_rational_function_ptr b) {
	storm::RationalFunction& srf_a = *(storm::RationalFunction*)a->storm_rational_function;
	storm::RationalFunction& srf_b = *(storm::RationalFunction*)b->storm_rational_function;

	storm::RationalFunction* result_srf = new storm::RationalFunction(srf_a);
	*result_srf *= srf_b;

	storm_rational_function_ptr result = (storm_rational_function_ptr)malloc(sizeof(storm_rational_function_ptr_struct));
	result->storm_rational_function = (void*)result_srf;

	return result;
}

storm_rational_function_ptr storm_rational_function_divide(storm_rational_function_ptr a, storm_rational_function_ptr b) {
	storm::RationalFunction& srf_a = *(storm::RationalFunction*)a->storm_rational_function;
	storm::RationalFunction& srf_b = *(storm::RationalFunction*)b->storm_rational_function;

	storm::RationalFunction* result_srf = new storm::RationalFunction(srf_a);
	*result_srf /= srf_b;

	storm_rational_function_ptr result = (storm_rational_function_ptr)malloc(sizeof(storm_rational_function_ptr_struct));
	result->storm_rational_function = (void*)result_srf;

	return result;
}

uint64_t storm_rational_function_hash(storm_rational_function_ptr const a, uint64_t const seed) {
	storm::RationalFunction& srf_a = *(storm::RationalFunction*)a->storm_rational_function;

	size_t hash = carl::hash_value(srf_a);
	uint64_t result = hash ^ seed;

	return result;
}

storm_rational_function_ptr storm_rational_function_negate(storm_rational_function_ptr a) {
	storm::RationalFunction& srf_a = *(storm::RationalFunction*)a->storm_rational_function;

	storm::RationalFunction* result_srf = new storm::RationalFunction(srf_a);
	*result_srf = -srf_a;

	storm_rational_function_ptr result = (storm_rational_function_ptr)malloc(sizeof(storm_rational_function_ptr_struct));
	result->storm_rational_function = (void*)result_srf;

	return result;
}

int storm_rational_function_is_zero(storm_rational_function_ptr a) {
	storm::RationalFunction& srf_a = *(storm::RationalFunction*)a->storm_rational_function;

	if (srf_a.isZero()) {
		return 1;
	} else {
		return 0;
	}
}
