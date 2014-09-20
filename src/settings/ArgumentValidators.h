#ifndef STORM_SETTINGS_ARGUMENTVALIDATORS_H_
#define STORM_SETTINGS_ARGUMENTVALIDATORS_H_

#include <iostream>
#include <ostream>
#include <fstream>
#include <list>
#include <utility>
#include <functional>
#include <vector>
#include <memory>
#include <string>

#include "src/settings/Argument.h"
#include "src/exceptions/ExceptionMacros.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
	namespace settings {
		class ArgumentValidators {
		public:
			/*!
             * Creates a validation function that checks whether an integer is in the given range (including the bounds).
             *
             * @param lowerBound The lower bound of the valid range.
             * @param upperBound The upper bound of the valid range.
             * @return The resulting validation function.
             */
			static std::function<bool (int_fast64_t const&)> integerRangeValidatorIncluding(int_fast64_t lowerBound, int_fast64_t upperBound) {
				return rangeValidatorIncluding<int_fast64_t>(lowerBound, upperBound);
			}
            
			/*!
             * Creates a validation function that checks whether an integer is in the given range (excluding the bounds).
             *
             * @param lowerBound The lower bound of the valid range.
             * @param upperBound The upper bound of the valid range.
             * @return The resulting validation function.
             */
			static std::function<bool (int_fast64_t const&)> integerRangeValidatorExcluding(int_fast64_t lowerBound, int_fast64_t upperBound) {
				return rangeValidatorExcluding<int_fast64_t>(lowerBound, upperBound);
			}

			/*!
             * Creates a validation function that checks whether an unsigned integer is in the given range (including the bounds).
             *
             * @param lowerBound The lower bound of the valid range.
             * @param upperBound The upper bound of the valid range.
             * @return The resulting validation function.
             */
			static std::function<bool (uint_fast64_t const&)> unsignedIntegerRangeValidatorIncluding(uint_fast64_t lowerBound, uint_fast64_t upperBound) {
				return rangeValidatorIncluding<uint_fast64_t>(lowerBound, upperBound);
			}
            
            /*!
             * Creates a validation function that checks whether an unsigned integer is in the given range (excluding the bounds).
             *
             * @param lowerBound The lower bound of the valid range.
             * @param upperBound The upper bound of the valid range.
             * @return The resulting validation function.
             */
			static std::function<bool (uint_fast64_t const&)> unsignedIntegerRangeValidatorExcluding(uint_fast64_t lowerBound, uint_fast64_t upperBound) {
				return rangeValidatorExcluding<uint_fast64_t>(lowerBound, upperBound);
			}

			/*!
             * Creates a validation function that checks whether a double is in the given range (including the bounds).
             *
             * @param lowerBound The lower bound of the valid range.
             * @param upperBound The upper bound of the valid range.
             * @return The resulting validation function.
             */
			static std::function<bool (double const&)> doubleRangeValidatorIncluding(double lowerBound, double upperBound) {
				return rangeValidatorIncluding<double>(lowerBound, upperBound);
			}
            
            /*!
             * Creates a validation function that checks whether a double is in the given range (excluding the bounds).
             *
             * @param lowerBound The lower bound of the valid range.
             * @param upperBound The upper bound of the valid range.
             * @return The resulting validation function.
             */
			static std::function<bool (double const&)> doubleRangeValidatorExcluding(double lowerBound, double upperBound) {
				return rangeValidatorExcluding<double>(lowerBound, upperBound);
			}

            /*!
             * Creates a validation function that checks whether a given string corresponds to an existing and readable
             * file.
             *
             * @return The resulting validation function.
             */
			static std::function<bool (std::string const&)> existingReadableFileValidator() {
				return [] (std::string const fileName) -> bool {
					std::ifstream targetFile(fileName);
					bool isFileGood = targetFile.good();

                    LOG_THROW(isFileGood, storm::exceptions::IllegalArgumentValueException, "The file " << fileName << " does not exist or is not readable.");
					return isFileGood;
				};
			}

            /*!
             * Creates a validation function that checks whether a given string is in a provided list of strings.
             *
             * @param list The list of valid strings.
             * @return The resulting validation function.
             */
			static std::function<bool (std::string const&)> stringInListValidator(std::vector<std::string> const& list) {
				return [list] (std::string const& inputString) -> bool {
                    for (auto const& validString : list) {
						if (inputString == validString) {
							return true;
						}
					}

                    LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Value " << inputString << " does not match any entry in the list of valid items.");
					return false;
				};
			}
            
		private:
            /*!
             * Creates a validation function that checks whether its argument is in a given range (including the bounds).
             *
             * @param lowerBound The lower bound of the valid range.
             * @param upperBound The upper bound of the valid range.
             * @return The resulting validation function.
             */
			template<typename T>
			static std::function<bool (T const&)> rangeValidatorIncluding(T lowerBound, T upperBound) {
				return std::bind([](T lowerBound, T upperBound, T value) -> bool {
                    LOG_THROW(lowerBound <= value && value <= upperBound, storm::exceptions::InvalidArgumentException, "Value " << value << " is out range.");
                    return true;
                }, lowerBound, upperBound, std::placeholders::_1);
			}
            
            /*!
             * Creates a validation function that checks whether its argument is in a given range (excluding the bounds).
             *
             * @param lowerBound The lower bound of the valid range.
             * @param upperBound The upper bound of the valid range.
             * @return The resulting validation function.
             */
			template<typename T>
			static std::function<bool (T const&)> rangeValidatorExcluding(T lowerBound, T upperBound) {
				return std::bind([](T lowerBound, T upperBound, T value) -> bool {
                    LOG_THROW(lowerBound < value && value < upperBound, storm::exceptions::InvalidArgumentException, "Value " << value << " is out range.");
                    return true;
				}, lowerBound, upperBound, std::placeholders::_1);
			}
		};
	}
}

#endif // STORM_SETTINGS_ARGUMENTVALIDATORS_H_