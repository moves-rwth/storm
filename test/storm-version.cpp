// AUTO GENERATED -- DO NOT CHANGE
#include "storm-version-info/storm-version.h"

namespace storm {

    const unsigned StormVersion::versionMajor = 1;
    const unsigned StormVersion::versionMinor = 6;
    const unsigned StormVersion::versionPatch = 1;
    const std::string StormVersion::versionLabel = "";
    const bool StormVersion::versionDev = true;
    const StormVersion::VersionSource StormVersion::versionSource = VersionSource::Git;
    const std::string StormVersion::gitRevisionHash = "6f166fc3eecad18c4733f86dfcaabd2178e6a098";
    const unsigned StormVersion::commitsAhead = 125;
    const StormVersion::DirtyState StormVersion::dirty = DirtyState::Clean;
    const std::string StormVersion::systemName = "Darwin";
    const std::string StormVersion::systemVersion = "19.6.0";
    const std::string StormVersion::cxxCompiler = "AppleClang 11.0.3.11030032";
#ifdef NDEBUG
    const std::string StormVersion::cxxFlags = " -std=c++14 -stdlib=libc++ -ftemplate-depth=1024" " " "-O3 -DNDEBUG -fno-stack-check -flto -march=native -fomit-frame-pointer";
#else
    const std::string StormVersion::cxxFlags = " -std=c++14 -stdlib=libc++ -ftemplate-depth=1024" " " "-g";
#endif

}
