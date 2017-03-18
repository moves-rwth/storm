#include "storm_function_wrapper.h"

#include <cstring>
#include <iostream>
#include <sstream>
#include <set>
#include <map>
#include <mutex>

#include "storm/adapters/CarlAdapter.h"
#include "storm/utility/constants.h"
#include "sylvan_storm_rational_function.h"

#include "storm/exceptions/InvalidOperationException.h"

#include <sylvan_config.h>
#include <sylvan.h>
#include <sylvan_common.h>
#include <sylvan_mtbdd.h>

std::mutex carlMutex;

void storm_rational_function_init(storm_rational_function_ptr* a) {
	std::lock_guard<std::mutex> lock(carlMutex);
	{
		storm_rational_function_ptr srf_ptr = new storm::RationalFunction(*((storm::RationalFunction*)(*a)));
		
		if (srf_ptr == nullptr) {
			std::cerr << "Could not allocate memory in storm_rational_function_init()!" << std::endl;
			return;
		}
	
		*a = srf_ptr;
	}
}

void storm_rational_function_destroy(storm_rational_function_ptr a) {
	std::lock_guard<std::mutex> lock(carlMutex);
	{
		storm::RationalFunction* srf = (storm::RationalFunction*)a;
		delete srf;
	}
}

int storm_rational_function_equals(storm_rational_function_ptr a, storm_rational_function_ptr b) {
	std::lock_guard<std::mutex> lock(carlMutex);
	int result = 0;
	{
		storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;
		storm::RationalFunction& srf_b = *(storm::RationalFunction*)b;

		if (srf_a == srf_b) {
			result = 1;
		}
	}

	return result;
}

storm_rational_function_ptr storm_rational_function_plus(storm_rational_function_ptr a, storm_rational_function_ptr b) {
	std::lock_guard<std::mutex> lock(carlMutex);
	storm_rational_function_ptr result = (storm_rational_function_ptr)nullptr;

	{
		storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;
		storm::RationalFunction& srf_b = *(storm::RationalFunction*)b;
	
		storm::RationalFunction* result_srf = new storm::RationalFunction(srf_a);
		if (result_srf == nullptr) {
			std::cerr << "Could not allocate memory in storm_rational_function_plus()!" << std::endl;
			return result;
		}
		
		*result_srf += srf_b;
		result = (storm_rational_function_ptr)result_srf;
	}

	return result;
}

storm_rational_function_ptr storm_rational_function_minus(storm_rational_function_ptr a, storm_rational_function_ptr b) {
	std::lock_guard<std::mutex> lock(carlMutex);
	storm_rational_function_ptr result = (storm_rational_function_ptr)nullptr;

	{
		storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;
		storm::RationalFunction& srf_b = *(storm::RationalFunction*)b;
	
		storm::RationalFunction* result_srf = new storm::RationalFunction(srf_a);
		if (result_srf == nullptr) {
			std::cerr << "Could not allocate memory in storm_rational_function_minus()!" << std::endl;
			return result;
		}
		
		*result_srf -= srf_b;
		result = (storm_rational_function_ptr)result_srf;
	}
	return result;
}

storm_rational_function_ptr storm_rational_function_times(storm_rational_function_ptr a, storm_rational_function_ptr b) {
	std::lock_guard<std::mutex> lock(carlMutex);
	storm_rational_function_ptr result = (storm_rational_function_ptr)nullptr;

	{
		storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;
		storm::RationalFunction& srf_b = *(storm::RationalFunction*)b;
	
		storm::RationalFunction* result_srf = new storm::RationalFunction(srf_a);
		if (result_srf == nullptr) {
			std::cerr << "Could not allocate memory in storm_rational_function_times()!" << std::endl;
			return result;
		}
		
		*result_srf *= srf_b;
		result = (storm_rational_function_ptr)result_srf;
	}
	return result;
}

storm_rational_function_ptr storm_rational_function_divide(storm_rational_function_ptr a, storm_rational_function_ptr b) {
	std::lock_guard<std::mutex> lock(carlMutex);
	storm_rational_function_ptr result = (storm_rational_function_ptr)nullptr;

	{
		storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;
		storm::RationalFunction& srf_b = *(storm::RationalFunction*)b;
	
		storm::RationalFunction* result_srf = new storm::RationalFunction(srf_a);
		if (result_srf == nullptr) {
			std::cerr << "Could not allocate memory in storm_rational_function_divide()!" << std::endl;
			return result;
		}
		        
		*result_srf /= srf_b;
		result = (storm_rational_function_ptr)result_srf;
	}
	return result;
}

storm_rational_function_ptr storm_rational_function_pow(storm_rational_function_ptr a, storm_rational_function_ptr b) {
    std::lock_guard<std::mutex> lock(carlMutex);
    storm_rational_function_ptr result = (storm_rational_function_ptr)nullptr;
    
    {
        storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;
        storm::RationalFunction& srf_b = *(storm::RationalFunction*)b;
        
        uint64_t exponentAsInteger = carl::toInt<unsigned long>(srf_b.nominatorAsNumber());
        storm::RationalFunction* result_srf = new storm::RationalFunction(carl::pow(srf_a, exponentAsInteger));
        if (result_srf == nullptr) {
            std::cerr << "Could not allocate memory in storm_rational_function_pow()!" << std::endl;
            return result;
        }
        result = (storm_rational_function_ptr)result_srf;
    }
    return result;
}

storm_rational_function_ptr storm_rational_function_mod(storm_rational_function_ptr a, storm_rational_function_ptr b) {
    std::lock_guard<std::mutex> lock(carlMutex);
    storm_rational_function_ptr result = (storm_rational_function_ptr)nullptr;
    
    {
        storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;
        storm::RationalFunction& srf_b = *(storm::RationalFunction*)b;
        if (!storm::utility::isInteger(srf_a) || !storm::utility::isInteger(srf_b)) {
            throw storm::exceptions::InvalidOperationException() << "Operands of mod must not be rational function.";
        }
        throw storm::exceptions::InvalidOperationException() << "Modulo not supported for rationals.";
        
        // storm::RationalFunction* result_srf = new storm::RationalFunction(carl::mod(srf_a.nominatorAsNumber(), srf_b.nominatorAsNumber()));
//        if (result_srf == nullptr) {
//            std::cerr << "Could not allocate memory in storm_rational_function_pow()!" << std::endl;
//            return result;
//        }
//        result = (storm_rational_function_ptr)result_srf;
    
    }
    return result;
}

storm_rational_function_ptr storm_rational_function_min(storm_rational_function_ptr a, storm_rational_function_ptr b) {
    std::lock_guard<std::mutex> lock(carlMutex);
    storm_rational_function_ptr result = (storm_rational_function_ptr)nullptr;
    
    {
        storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;
        storm::RationalFunction& srf_b = *(storm::RationalFunction*)b;
        if (!storm::utility::isInteger(srf_a) || !storm::utility::isInteger(srf_b)) {
            throw storm::exceptions::InvalidOperationException() << "Operands of min must not be rational function.";
        }

        storm::RationalFunction* result_srf = new storm::RationalFunction(std::min(srf_a.nominatorAsNumber(), srf_b.nominatorAsNumber()));
        if (result_srf == nullptr) {
            std::cerr << "Could not allocate memory in storm_rational_function_pow()!" << std::endl;
            return result;
        }
        result = (storm_rational_function_ptr)result_srf;
    
    }
    return result;
}

storm_rational_function_ptr storm_rational_function_max(storm_rational_function_ptr a, storm_rational_function_ptr b) {
    std::lock_guard<std::mutex> lock(carlMutex);
    storm_rational_function_ptr result = (storm_rational_function_ptr)nullptr;
    
    {
        storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;
        storm::RationalFunction& srf_b = *(storm::RationalFunction*)b;
        if (!storm::utility::isInteger(srf_a) || !storm::utility::isInteger(srf_b)) {
            throw storm::exceptions::InvalidOperationException() << "Operands of max must not be rational function.";
        }

        storm::RationalFunction* result_srf = new storm::RationalFunction(std::max(srf_a.nominatorAsNumber(), srf_b.nominatorAsNumber()));
        if (result_srf == nullptr) {
            std::cerr << "Could not allocate memory in storm_rational_function_pow()!" << std::endl;
            return result;
        }
        result = (storm_rational_function_ptr)result_srf;
    }
    return result;
}

int storm_rational_function_less(storm_rational_function_ptr a, storm_rational_function_ptr b) {
    std::lock_guard<std::mutex> lock(carlMutex);
    
    storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;
    storm::RationalFunction& srf_b = *(storm::RationalFunction*)b;
    if (!storm::utility::isInteger(srf_a) || !storm::utility::isInteger(srf_b)) {
        throw storm::exceptions::InvalidOperationException() << "Operands of less must not be rational functions.";
    }
    
    if (srf_a.nominatorAsNumber() < srf_b.nominatorAsNumber()) {
        return 1;
    } else {
        return 0;
    }
    return -1;
}

int storm_rational_function_less_or_equal(storm_rational_function_ptr a, storm_rational_function_ptr b) {
    std::lock_guard<std::mutex> lock(carlMutex);
    
    storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;
    storm::RationalFunction& srf_b = *(storm::RationalFunction*)b;
    if (!storm::utility::isInteger(srf_a) || !storm::utility::isInteger(srf_b)) {
        throw storm::exceptions::InvalidOperationException() << "Operands of less-or-equal must not be rational functions.";
    }
    if (srf_a.nominatorAsNumber() <= srf_b.nominatorAsNumber()) {
        return 1;
    } else {
        return 0;
    }
    
    return -1;
}

uint64_t storm_rational_function_hash(storm_rational_function_ptr const a, uint64_t const seed) {
	std::lock_guard<std::mutex> lock(carlMutex);
	
	uint64_t result = seed;
	{
		storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;
        // carl::hash_add(result, srf_a);
        result = seed ^ (carl::hash_value(srf_a) + 0x9e3779b9 + (seed<<6) + (seed>>2));
	}
	return result;
}

storm_rational_function_ptr storm_rational_function_negate(storm_rational_function_ptr a) {
	std::lock_guard<std::mutex> lock(carlMutex);
	storm_rational_function_ptr result = (storm_rational_function_ptr)nullptr;

	{
		storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;
	
		storm::RationalFunction* result_srf = new storm::RationalFunction(srf_a);
		if (result_srf == nullptr) {
			std::cerr << "Could not allocate memory in storm_rational_function_negate()!" << std::endl;
			return result;
		}
		
		*result_srf = -srf_a;
		result = (storm_rational_function_ptr)result_srf;
	}
	return result;
}

storm_rational_function_ptr storm_rational_function_floor(storm_rational_function_ptr a) {
    std::lock_guard<std::mutex> lock(carlMutex);
    storm_rational_function_ptr result = (storm_rational_function_ptr)nullptr;
    
    {
        storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;
        if (!storm::utility::isInteger(srf_a)) {
            throw storm::exceptions::InvalidOperationException() << "Operand of floor must not be rational function.";
        }
        storm::RationalFunction* result_srf = new storm::RationalFunction(srf_a);
        
        if (result_srf == nullptr) {
            std::cerr << "Could not allocate memory in storm_rational_function_negate()!" << std::endl;
            return result;
        }
        
        *result_srf = storm::RationalFunction(carl::floor(srf_a.nominatorAsNumber()));
        result = (storm_rational_function_ptr)result_srf;
        
    }
    return result;
}

storm_rational_function_ptr storm_rational_function_ceil(storm_rational_function_ptr a) {
    std::lock_guard<std::mutex> lock(carlMutex);
    storm_rational_function_ptr result = (storm_rational_function_ptr)nullptr;
    
    {
        storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;
        if (!storm::utility::isInteger(srf_a)) {
            throw storm::exceptions::InvalidOperationException() << "Operand of ceil must not be rational function.";
        }
        storm::RationalFunction* result_srf = new storm::RationalFunction(srf_a);
        
        if (result_srf == nullptr) {
            std::cerr << "Could not allocate memory in storm_rational_function_negate()!" << std::endl;
            return result;
        }
        
        *result_srf = storm::RationalFunction(carl::ceil(srf_a.nominatorAsNumber()));
        result = (storm_rational_function_ptr)result_srf;
    }
    return result;
}

int storm_rational_function_is_zero(storm_rational_function_ptr a) {
	std::lock_guard<std::mutex> lock(carlMutex);

	bool resultIsZero = false;
	{
		storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;
		resultIsZero = srf_a.isZero();
	}

	if (resultIsZero) {
		return 1;
	} else {
		return 0;
	}
}

double storm_rational_function_get_constant(storm_rational_function_ptr a) {
	std::lock_guard<std::mutex> lock(carlMutex);

	double result = -1.0;
	{
		storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;
	
		if (srf_a.isConstant()) {
			result = carl::toDouble(storm::RationalNumber(srf_a.nominatorAsNumber() / srf_a.denominatorAsNumber()));
		} else {
			std::cout << "Defaulting to -1.0 since this is not a constant: " << srf_a << std::endl;
		}
	}
	return result;
}

storm_rational_function_ptr storm_rational_function_get_zero() {
	std::lock_guard<std::mutex> lock(carlMutex);
	storm_rational_function_ptr result = (storm_rational_function_ptr)nullptr;

	{
		storm::RationalFunction* result_srf = new storm::RationalFunction(0);
		if (result_srf == nullptr) {
			std::cerr << "Could not allocate memory in storm_rational_function_get_zero()!" << std::endl;
			return result;
		}
		result = (storm_rational_function_ptr)result_srf;
	}
	
	return result;
}

storm_rational_function_ptr storm_rational_function_get_one() {
	std::lock_guard<std::mutex> lock(carlMutex);
	storm_rational_function_ptr result = (storm_rational_function_ptr)nullptr;

	{
		storm::RationalFunction* result_srf = new storm::RationalFunction(1);
		if (result_srf == nullptr) {
			std::cerr << "Could not allocate memory in storm_rational_function_get_one()!" << std::endl;
			return result;
		}
		result = (storm_rational_function_ptr)result_srf;
	}
	
	return result;
}

void print_storm_rational_function(storm_rational_function_ptr a) {
	std::lock_guard<std::mutex> lock(carlMutex);
	{
		storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;
		std::cout << srf_a << std::flush;
	}
}

void print_storm_rational_function_to_file(storm_rational_function_ptr a, FILE* out) {
	std::lock_guard<std::mutex> lock(carlMutex);
	{
		std::stringstream ss;
		storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;
		ss << srf_a;
		std::string s = ss.str();
		fprintf(out, "%s", s.c_str());
	}
}

MTBDD testiTest(storm::RationalFunction const& currentFunction, std::map<uint32_t, std::pair<storm::RationalFunctionVariable, std::pair<storm::RationalNumber, storm::RationalNumber>>> const& replacements) {
	if (currentFunction.isConstant()) {
		return mtbdd_storm_rational_function((storm_rational_function_ptr)&currentFunction);
	}

	std::set<storm::RationalFunctionVariable> variablesInFunction = currentFunction.gatherVariables();
	std::map<uint32_t, std::pair<storm::RationalFunctionVariable, std::pair<storm::RationalNumber, storm::RationalNumber>>>::const_iterator it = replacements.cbegin();
	std::map<uint32_t, std::pair<storm::RationalFunctionVariable, std::pair<storm::RationalNumber, storm::RationalNumber>>>::const_iterator end = replacements.cend();

	// Walking the (ordered) map enforces an ordering on the MTBDD
	for (; it != end; ++it) {
		if (variablesInFunction.find(it->second.first) != variablesInFunction.cend()) {
			std::map<storm::RationalFunctionVariable, storm::RationalNumber> highReplacement = {{it->second.first, it->second.second.first}};
			std::map<storm::RationalFunctionVariable, storm::RationalNumber> lowReplacement = {{it->second.first, it->second.second.second}};

			std::lock_guard<std::mutex>* lock = new std::lock_guard<std::mutex>(carlMutex);
			storm::RationalFunction const highSrf = currentFunction.substitute(highReplacement);
			storm::RationalFunction const lowSrf = currentFunction.substitute(lowReplacement);
			delete lock;
			
			MTBDD high = testiTest(highSrf, replacements);
			MTBDD low = testiTest(lowSrf, replacements);
			LACE_ME
			return mtbdd_ite(mtbdd_ithvar(it->first), high, low);
		} else {
			//std::cout << "No match for variable " << it->second.first << std::endl;
		}
	}

	return mtbdd_storm_rational_function((storm_rational_function_ptr)&currentFunction);
}


MTBDD storm_rational_function_leaf_parameter_replacement(MTBDD dd, storm_rational_function_ptr a, void* context) {	
	storm::RationalFunction& srf_a = *(storm::RationalFunction*)a;
	{
		// Scope the lock.
		std::lock_guard<std::mutex> lock(carlMutex);
		if (srf_a.isConstant()) {
			return dd;
		}
	}
	
	std::map<uint32_t, std::pair<storm::RationalFunctionVariable, std::pair<storm::RationalNumber, storm::RationalNumber>>>* replacements = (std::map<uint32_t, std::pair<storm::RationalFunctionVariable, std::pair<storm::RationalNumber, storm::RationalNumber>>>*)context;
	return testiTest(srf_a, *replacements);
}
