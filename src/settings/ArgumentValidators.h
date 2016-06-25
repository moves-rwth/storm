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
#include <stdio.h>

#include "src/settings/Argument.h"
#include "src/utility/macros.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/IllegalArgumentValueException.h"
#include "src/exceptions/IllegalFunctionCallException.h"

#include <sys/stat.h>

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
             * Creates a validation function that checks whether an integer is greater than or equal to the given threshold.
             *
             * @param threshold The threshold.
             * @return The resulting validation function.
             */
            static std::function<bool (int_fast64_t const&)> integerGreaterValidatorIncluding(int_fast64_t threshold) {
                return greaterValidatorIncluding<int_fast64_t>(threshold);
            }
            
            /*!
             * Creates a validation function that checks whether an integer is greater than the given threshold.
             *
             * @param threshold The threshold.
             * @return The resulting validation function.
             */
            static std::function<bool (int_fast64_t const&)> integerGreaterValidatorExcluding(int_fast64_t threshold) {
                return greaterValidatorExcluding<int_fast64_t>(threshold);
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
             * Creates a validation function that checks whether an unsigned integer is greater than or equal to the given threshold.
             *
             * @param threshold The threshold.
             * @return The resulting validation function.
             */
            static std::function<bool (uint_fast64_t const&)> unsignedIntegerGreaterValidatorIncluding(uint_fast64_t threshold) {
                return greaterValidatorIncluding<uint_fast64_t>(threshold);
            }
            
            /*!
             * Creates a validation function that checks whether an unsigned integer is greater than the given threshold.
             *
             * @param threshold The threshold.
             * @return The resulting validation function.
             */
            static std::function<bool (uint_fast64_t const&)> unsignedIntegerGreaterValidatorExcluding(uint_fast64_t threshold) {
                return greaterValidatorExcluding<uint_fast64_t>(threshold);
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
             * Creates a validation function that checks whether a double is greater than or equal to the given threshold.
             *
             * @param threshold The threshold.
             * @return The resulting validation function.
             */
            static std::function<bool (double const&)> doubleGreaterValidatorIncluding(double threshold) {
                return greaterValidatorIncluding<double>(threshold);
            }
            
            /*!
             * Creates a validation function that checks whether a double is greater than the given threshold.
             *
             * @param threshold The threshold.
             * @return The resulting validation function.
             */
            static std::function<bool (double const&)> doubleGreaterValidatorExcluding(double threshold) {
                return greaterValidatorExcluding<double>(threshold);
            }

            /*!
             * Creates a validation function that checks whether a given string corresponds to an existing and readable
             * file.
             *
             * @return The resulting validation function.
             */
			static std::function<bool (std::string const&)> existingReadableFileValidator() {
				return [] (std::string const fileName) -> bool {
                    // First check existence as ifstream::good apparently als returns true for directories.
                    struct stat info;
                    stat(fileName.c_str(), &info);
                    STORM_LOG_THROW(info.st_mode & S_IFREG, storm::exceptions::IllegalArgumentValueException, "Unable to read from non-existing file '" << fileName << "'.");
                    
                    // Now that we know it's a file, we can check its readability.
                    std::ifstream istream(fileName);
                    STORM_LOG_THROW(istream.good(), storm::exceptions::IllegalArgumentValueException, "Unable to read from file '" << fileName << "'.");
                    
                    return true;
				};
			}

            /*!
             * Creates a validation function that checks whether a given string corresponds to a path to a file in which we can write
             *
             * @return The resulting validation function.
             */
			static std::function<bool (std::string const&)> writableFileValidator() {
				return [] (std::string const fileName) -> bool {
                    struct stat info;
                    STORM_LOG_THROW(stat (fileName.c_str(), &info) != 0, storm::exceptions::IllegalArgumentValueException , "Could not open file '" << fileName << "' for writing because file or directory already exists.");
                    
                    std::ofstream filestream(fileName);
                    STORM_LOG_THROW(filestream.is_open(), storm::exceptions::IllegalArgumentValueException , "Could not open file '" << fileName << "' for writing.");
                    filestream.close();
                    std::remove(fileName.c_str());
                    
                    return true;
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

                    STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Value '" << inputString << "' does not match any entry in the list of valid items.");
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
                    STORM_LOG_THROW(lowerBound <= value && value <= upperBound, storm::exceptions::InvalidArgumentException, "Value " << value << " is out of range.");
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
                    STORM_LOG_THROW(lowerBound < value && value < upperBound, storm::exceptions::InvalidArgumentException, "Value " << value << " is out of range.");
                    return true;
				}, lowerBound, upperBound, std::placeholders::_1);
			}
            
            /*!
             * Creates a validation function that checks whether its argument is greater than the given threshold.
             *
             * @param threshold The threshold.
             * @return The resulting validation function.
             */
            template<typename T>
            static std::function<bool (T const&)> greaterValidatorExcluding(T threshold) {
                return std::bind([](T threshold, T value) -> bool {
                    STORM_LOG_THROW(threshold < value, storm::exceptions::InvalidArgumentException, "Value " << value << " is out of range.");
                    return true;
                }, threshold, std::placeholders::_1);
            }
            
            /*!
             * Creates a validation function that checks whether its argument is greater than or equal to the given threshold.
             *
             * @param threshold The threshold.
             * @return The resulting validation function.
             */
            template<typename T>
            static std::function<bool (T const&)> greaterValidatorIncluding(T threshold) {
                return std::bind([](T threshold, T value) -> bool {
                    STORM_LOG_THROW(threshold <= value, storm::exceptions::InvalidArgumentException, "Value " << value << " is out of range.");
                    return true;
                }, threshold, std::placeholders::_1);
            }
		};
	}
}

#endif // STORM_SETTINGS_ARGUMENTVALIDATORS_H_
