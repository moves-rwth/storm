#include "storm_function_wrapper.h"

#include <cstring>
#include <iostream>
#include <sstream>
#include <set>
#include <map>
#include "storm/adapters/CarlAdapter.h"
#include "sylvan_storm_rational_function.h"

#include <sylvan_config.h>
#include <sylvan.h>
#include <sylvan_common.h>
#include <sylvan_mtbdd.h>

#undef DEBUG_STORM_FUNCTION_WRAPPER

#ifdef DEBUG_STORM_FUNCTION_WRAPPER
#define LOG_I(funcName) std::cout << "Entering function " << funcName << std::endl;
#define LOG_O(funcName) std::cout << "Leaving function " << funcName << std::endl;
#else
#define LOG_I(funcName)
#define LOG_O(funcName)
#endif

void storm_rational_function_init(storm_rational_function_ptr* a) {
	LOG_I("init")
#ifdef DEBUG_STORM_FUNCTION_WRAPPER
	std::cout << "storm_rational_function_init - ptr of old = " << *a << ", value = " << *((storm::RationalFunction*)(*a)) << std::endl;
#endif
	storm_rational_function_ptr srf_ptr = new storm::RationalFunction(*((storm::RationalFunction*)(*a)));
	
	if (srf_ptr == nullptr) {
		std::cerr << "Could not allocate memory in storm_rational_function_init()!" << std::endl;
		return;
	}

	*a = srf_ptr;
#ifdef DEBUG_STORM_FUNCTION_WRAPPER
	std::cout << "storm_rational_function_init - ptr of new = " << *a << ", value = " << *((storm::RationalFunction*)(*a)) << std::endl;
#endif
	LOG_O("init")
}

void storm_rational_function_destroy(storm_rational_function_ptr a) {
	LOG_I("destroy")
	delete (storm::RationalFunction*)a;
	LOG_O("destroy")
}

int storm_rational_function_equals(storm_rational_function_ptr a, storm_rational_function_ptr b) {
	LOG_I("equals")
	storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;
	storm::RationalFunction& srf_b = *(storm::RationalFunction*)b;
	
	LOG_O("equals")
	
	int result = 0;
	if (srf_a == srf_b) {
		result = 1;
	}

#ifdef DEBUG_STORM_FUNCTION_WRAPPER
	std::cout << "storm_rational_function_equals called with ptr = " << a << " value a = " << srf_a << " and ptr = " << b << " value b = " << srf_b << " result = " << result << "." << std::endl;
#endif

	return result;
}

storm_rational_function_ptr storm_rational_function_plus(storm_rational_function_ptr a, storm_rational_function_ptr b) {
	LOG_I("plus")
	storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;
	storm::RationalFunction& srf_b = *(storm::RationalFunction*)b;

	storm::RationalFunction* result_srf = new storm::RationalFunction(srf_a);
	if (result_srf == nullptr) {
		std::cerr << "Could not allocate memory in storm_rational_function_plus()!" << std::endl;
		return (storm_rational_function_ptr)nullptr;
	}
	
	*result_srf += srf_b;

	storm_rational_function_ptr result = (storm_rational_function_ptr)result_srf;

	LOG_O("plus")
	return result;
}

storm_rational_function_ptr storm_rational_function_minus(storm_rational_function_ptr a, storm_rational_function_ptr b) {
	LOG_I("minus")
	storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;
	storm::RationalFunction& srf_b = *(storm::RationalFunction*)b;

	storm::RationalFunction* result_srf = new storm::RationalFunction(srf_a);
	if (result_srf == nullptr) {
		std::cerr << "Could not allocate memory in storm_rational_function_minus()!" << std::endl;
		return (storm_rational_function_ptr)nullptr;
	}
	
	*result_srf -= srf_b;

	storm_rational_function_ptr result = (storm_rational_function_ptr)result_srf;

	LOG_O("minus")
	return result;
}

storm_rational_function_ptr storm_rational_function_times(storm_rational_function_ptr a, storm_rational_function_ptr b) {
	LOG_I("times")
	storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;
	storm::RationalFunction& srf_b = *(storm::RationalFunction*)b;

	storm::RationalFunction* result_srf = new storm::RationalFunction(srf_a);
	if (result_srf == nullptr) {
		std::cerr << "Could not allocate memory in storm_rational_function_times()!" << std::endl;
		return (storm_rational_function_ptr)nullptr;
	}
	
	*result_srf *= srf_b;

	storm_rational_function_ptr result = (storm_rational_function_ptr)result_srf;

	LOG_O("times")
	return result;
}

storm_rational_function_ptr storm_rational_function_divide(storm_rational_function_ptr a, storm_rational_function_ptr b) {
	LOG_I("divide")
	storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;
	storm::RationalFunction& srf_b = *(storm::RationalFunction*)b;

	storm::RationalFunction* result_srf = new storm::RationalFunction(srf_a);
	if (result_srf == nullptr) {
		std::cerr << "Could not allocate memory in storm_rational_function_divide()!" << std::endl;
		return (storm_rational_function_ptr)nullptr;
	}
	
	*result_srf /= srf_b;

	storm_rational_function_ptr result = (storm_rational_function_ptr)result_srf;

	LOG_O("divide")
	return result;
}

uint64_t storm_rational_function_hash(storm_rational_function_ptr const a, uint64_t const seed) {
	LOG_I("hash")
	storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;

	size_t hash = carl::hash_value(srf_a);

#ifdef DEBUG_STORM_FUNCTION_WRAPPER
	std::cout << "storm_rational_function_hash of value " << srf_a << " is " << hash << std::endl;
#endif

	uint64_t result = hash ^ seed;

	LOG_O("hash")
	return result;
}

storm_rational_function_ptr storm_rational_function_negate(storm_rational_function_ptr a) {
	LOG_I("negate")
	storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;

	storm::RationalFunction* result_srf = new storm::RationalFunction(srf_a);
	if (result_srf == nullptr) {
		std::cerr << "Could not allocate memory in storm_rational_function_negate()!" << std::endl;
		return (storm_rational_function_ptr)nullptr;
	}
	
	*result_srf = -srf_a;

	storm_rational_function_ptr result = (storm_rational_function_ptr)result_srf;

	LOG_O("negate")
	return result;
}

int storm_rational_function_is_zero(storm_rational_function_ptr a) {
	LOG_I("isZero")
	storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;

	if (srf_a.isZero()) {
		return 1;
	} else {
		return 0;
	}
}

storm_rational_function_ptr storm_rational_function_get_zero() {
	static storm::RationalFunction zeroFunction(0);
	LOG_I("getZero")
	return (storm_rational_function_ptr)(&zeroFunction);
}

storm_rational_function_ptr storm_rational_function_get_one() {
	static storm::RationalFunction oneFunction(1);
	LOG_I("getOne")
	return (storm_rational_function_ptr)(&oneFunction);
}

void print_storm_rational_function(storm_rational_function_ptr a) {
	storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;
	std::cout << srf_a << std::flush;
}

void print_storm_rational_function_to_file(storm_rational_function_ptr a, FILE* out) {
	std::stringstream ss;
	storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;
	ss << srf_a;
	std::string s = ss.str();
	fprintf(out, "%s", s.c_str());
}

MTBDD testiTest(storm::RationalFunction const& currentFunction, std::map<storm::RationalFunctionVariable, std::pair<uint32_t, std::pair<storm::RationalNumber, storm::RationalNumber>>> const& replacements) {
	if (currentFunction.isConstant()) {
		return mtbdd_storm_rational_function((storm_rational_function_ptr)&currentFunction);
	}

	std::set<storm::RationalFunctionVariable> variablesInFunction = currentFunction.gatherVariables();
	std::map<storm::RationalFunctionVariable, std::pair<uint32_t, std::pair<storm::RationalNumber, storm::RationalNumber>>>::const_iterator it = replacements.cbegin();
	std::map<storm::RationalFunctionVariable, std::pair<uint32_t, std::pair<storm::RationalNumber, storm::RationalNumber>>>::const_iterator end = replacements.cend();

	// Walking the (ordered) map enforces an ordering on the MTBDD
	for (; it != end; ++it) {
		if (variablesInFunction.find(it->first) != variablesInFunction.cend()) {
			std::map<storm::RationalFunctionVariable, storm::RationalNumber> highReplacement = {{it->first, it->second.second.first}};
			std::map<storm::RationalFunctionVariable, storm::RationalNumber> lowReplacement = {{it->first, it->second.second.second}};
			MTBDD high = testiTest(currentFunction.substitute(highReplacement), replacements);
			MTBDD low = testiTest(currentFunction.substitute(lowReplacement), replacements);
			LACE_ME
			return mtbdd_ite(mtbdd_ithvar(it->second.first), high, low);
		}
	}

	return mtbdd_storm_rational_function((storm_rational_function_ptr)&currentFunction);
}


MTBDD storm_rational_function_leaf_parameter_replacement(MTBDD dd, storm_rational_function_ptr a, void* context) {
	storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;
	if (srf_a.isConstant()) {
		return dd;
	}
	
	std::map<storm::RationalFunctionVariable, std::pair<uint32_t, std::pair<storm::RationalNumber, storm::RationalNumber>>>* replacements = (std::map<storm::RationalFunctionVariable, std::pair<uint32_t, std::pair<storm::RationalNumber, storm::RationalNumber>>>*)context;
	return testiTest(srf_a, *replacements);
}
