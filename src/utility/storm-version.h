/** 
 * @file:   storm-version.h
 * @author: Sebastian Junges
 *
 * @since October 7, 2014
 */

#pragma once
#include <string>
#include <sstream>

namespace storm 
{
	namespace utility {
	struct StormVersion 
	{
		/// The major version of StoRM
		const static unsigned versionMajor;
		/// The minor version of StoRM
		const static unsigned versionMinor;
		/// The patch version of StoRM
		const static unsigned versionPatch;
		/// The short hash of the git commit this build is bases on
		const static std::string gitRevisionHash;
		/// How many commits passed since the tag was last set
		const static unsigned commitsAhead;
		/// 0 iff there no files were modified in the checkout, 1 else
		const static unsigned dirty;
		/// The system which has compiled storm
		const static std::string systemName;
		/// The system version which has compiled storm
		const static std::string systemVersion;
		/// The build type that was used to build storm
		const static std::string buildType;
		/// The compiler version that was used to build storm
		const static std::string cxxCompiler;
		
		static std::string shortVersionString() {
			std::stringstream sstream;
			sstream << "StoRM " << versionMajor << "." << versionMinor << "." << versionPatch;
			return sstream.str();
		}
		
		static std::string longVersionString() {
			std::stringstream sstream;
			sstream << "Version: " << versionMajor << "." <<  versionMinor << "." << versionPatch;
			if (commitsAhead != 0) {
				sstream << " (+" << commitsAhead << " commits)";
			}
			sstream << " build from revision " << gitRevisionHash;
			if (dirty == 1) {
				sstream << " (DIRTY)";
			}
			sstream << "." << std::endl;
			return sstream.str();
		}
		
		static std::string buildInfo() {
			std::stringstream sstream;
			sstream << "Compiled on " << systemName << " " << systemVersion << ",";
			sstream << "using " << cxxCompiler << " with " << buildType << " flags.";
			return sstream.str();	
		}
	};
	}
}
