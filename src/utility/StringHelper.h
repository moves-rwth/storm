/*
 * StringHelper.h
 *
 *  Created on: 01.09.2013
 *      Author: Philipp Berger
 */

#ifndef STORM_UTILITY_STRINGHELPER_H_
#define STORM_UTILITY_STRINGHELPER_H_

#include <iostream>
#include <string>
#include <functional>
#include <algorithm>


namespace storm {
	namespace utility {

		class StringHelper {
		public:
			/*!
			* Returns the String, transformed with toLower.
			*/
			static std::string stringToLower(std::string const& sourceString) {
				std::string targetString;
				targetString.resize(sourceString.size());
				std::transform(sourceString.begin(), sourceString.end(), targetString.begin(), ::tolower);

				return targetString;
			}

			/*!
			* Returns the String, transformed with toUpper.
			*/
			static std::string stringToUpper(std::string const& sourceString) {
				std::string targetString;
				targetString.resize(sourceString.size());
				std::transform(sourceString.begin(), sourceString.end(), targetString.begin(), ::toupper);

				return targetString;
			}

			/*!
			* Returns true IFF the two Strings are equal.
			*/
			static bool compareIgnoreCase(std::string const& A, std::string const& B) {
				std::string stringA;
				std::string stringB;
				std::transform(A.begin(), A.end(), stringA.begin(), ::tolower);
				std::transform(B.begin(), B.end(), stringB.begin(), ::tolower);

				return stringA.compare(stringB) == 0;
			}
		private:
			StringHelper() {}
			StringHelper(StringHelper& other) {}
			~StringHelper() {}
		};
	}
}

#endif // STORM_UTILITY_STRINGHELPER_H_