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

#include "Argument.h"

namespace storm {
	namespace settings {
		class ArgumentValidators {
		public:
			// Integer - int_fast64_t
			static std::function<bool (int_fast64_t const, std::string&)> integerRangeValidatorIncluding(int_fast64_t const lowerBound, int_fast64_t const upperBound) {
				return rangeValidatorIncluding<int_fast64_t>(lowerBound, upperBound);
			} 
			static std::function<bool (int_fast64_t const, std::string&)> integerRangeValidatorExcluding(int_fast64_t const lowerBound, int_fast64_t const upperBound) {
				return rangeValidatorExcluding<int_fast64_t>(lowerBound, upperBound);
			}

			// UnsignedInteger - uint_fast64_t
			static std::function<bool (uint_fast64_t const, std::string&)> unsignedIntegerRangeValidatorIncluding(uint_fast64_t const lowerBound, uint_fast64_t const upperBound) {
				return rangeValidatorIncluding<uint_fast64_t>(lowerBound, upperBound);
			} 
			static std::function<bool (uint_fast64_t const, std::string&)> unsignedIntegerRangeValidatorExcluding(uint_fast64_t const lowerBound, uint_fast64_t const upperBound) {
				return rangeValidatorExcluding<uint_fast64_t>(lowerBound, upperBound);
			}

			// Double - double
			static std::function<bool (double const, std::string&)> doubleRangeValidatorIncluding(double const lowerBound, double const upperBound) {
				return rangeValidatorIncluding<double>(lowerBound, upperBound);
			} 
			static std::function<bool (double const, std::string&)> doubleRangeValidatorExcluding(double const lowerBound, double const upperBound) {
				return rangeValidatorExcluding<double>(lowerBound, upperBound);
			}

			static std::function<bool (std::string const, std::string&)> existingReadableFileValidator() {
				return [] (std::string const fileName, std::string& errorMessageTarget) -> bool {
					std::ifstream targetFile(fileName);
					bool isFileGood = targetFile.good();

					if (!isFileGood) {
						std::ostringstream stream;
						stream << "Given file does not exist or is not readable by this process: \"" << fileName << "\"" << std::endl; 
						errorMessageTarget.append(stream.str());
					}
					return isFileGood;
				};
			}

			static std::function<bool (std::string const, std::string&)> stringInListValidator(std::vector<std::string> list) {
				return [list] (std::string const inputString, std::string& errorMessageTarget) -> bool {
					std::string const lowerInputString = storm::utility::StringHelper::stringToLower(inputString);
					for (auto it = list.cbegin(); it != list.cend(); ++it) {
						if (storm::utility::StringHelper::stringToLower(*it).compare(lowerInputString) == 0) {
							return true;
						}
					}

					std::ostringstream stream;
					stream << "The given Input \"" << inputString << "\" is not in the list of valid Items (";
					bool first = true;
					for (auto it = list.cbegin(); it != list.cend(); ++it) {
						if (!first) {
							stream << ", ";
						}
						stream << *it;
						first = false;
					}
					stream << ")" << std::endl;
					errorMessageTarget.append(stream.str());
					
					return false;
				};
			}
		private:
			template<typename T>
			static std::function<bool (T const, std::string&)> rangeValidatorIncluding(T const lowerBound, T const upperBound) {
				return std::bind([](T const lowerBound, T const upperBound, T const value, std::string& errorMessageTarget) -> bool {
					bool lowerBoundCondition = (lowerBound <= value);
					bool upperBoundCondition = (value <= upperBound);
					if (!lowerBoundCondition) { 
						std::ostringstream stream;
						stream << " Lower Bound Condition not met: " << lowerBound << " is not <= " << value;
						errorMessageTarget.append(stream.str());
					}
					if (!upperBoundCondition) { 
						std::ostringstream stream;
						stream << " Upper Bound Condition not met: " << value << " is not <= " << upperBound; 
						errorMessageTarget.append(stream.str());
					}
					return (lowerBoundCondition && upperBoundCondition);
				}, lowerBound, upperBound, std::placeholders::_1, std::placeholders::_2);
			}
			template<typename T>
			static std::function<bool (T const, std::string&)> rangeValidatorExcluding(T const lowerBound, T const upperBound) {
				return std::bind([](T const lowerBound, T const upperBound, T const value, std::string& errorMessageTarget) -> bool { 
					bool lowerBoundCondition = (lowerBound < value);
					bool upperBoundCondition = (value < upperBound);
					if (!lowerBoundCondition) { 
						std::ostringstream stream;
						stream << " Lower Bound Condition not met: " << lowerBound << " is not < " << value;
						errorMessageTarget.append(stream.str());
					}
					if (!upperBoundCondition) { 
						std::ostringstream stream;
						stream << " Upper Bound Condition not met: " << value << " is not < " << upperBound; 
						errorMessageTarget.append(stream.str());
					}
					return (lowerBoundCondition && upperBoundCondition);
				}, lowerBound, upperBound, std::placeholders::_1, std::placeholders::_2);
			}
		};
	}
}

#endif // STORM_SETTINGS_ARGUMENTVALIDATORS_H_