#pragma once

#include <sstream>
#include <string>

namespace storm {

struct StormVersion {
    /// The major version of Storm.
    const static unsigned versionMajor;

    /// The minor version of Storm.
    const static unsigned versionMinor;

    /// The patch version of Storm.
    const static unsigned versionPatch;

    /// The label version of Storm (might be empty).
    const static std::string versionLabel;

    /// Flag indicating if the version of Storm is a development version.
    const static bool versionDev;

    enum class VersionSource { Git, Static };

    /// The source of the versioning information.
    const static VersionSource versionSource;

    /// The short hash of the git commit this build is based on
    const static std::string gitRevisionHash;

    /// How many commits passed since the tag was last set.
    const static unsigned commitsAhead;

    enum class DirtyState {
        Clean,   /// no files were modified in the checkout
        Dirty,   /// some files were modified
        Unknown  /// No information about dirtyness is given.
    };

    /// Indicates whether files were modified
    const static DirtyState dirty;

    /// The system which has compiled Storm.
    const static std::string systemName;

    /// The system version which has compiled Storm.
    const static std::string systemVersion;

    /// The compiler version that was used to build Storm.
    const static std::string cxxCompiler;

    /// The flags that were used to build Storm.
    const static std::string cxxFlags;

    static std::string shortVersionString() {
        std::stringstream sstream;
        sstream << versionMajor << "." << versionMinor << "." << versionPatch;
        if (!versionLabel.empty()) {
            sstream << "-" << versionLabel;
        }
        if (versionDev) {
            sstream << " (dev)";
        }
        return sstream.str();
    }

    static std::string longVersionString() {
        std::stringstream sstream;
        sstream << "Version " << shortVersionString();
        if (versionSource == VersionSource::Static) {
            sstream << " (derived statically)";
        }
        if (commitsAhead > 0) {
            sstream << " (+ " << commitsAhead << " commits)";
        }
        if (!gitRevisionHash.empty()) {
            sstream << " build from revision " << gitRevisionHash;
        } else {
            sstream << " built from archive";
        }
        switch (dirty) {
            case DirtyState::Clean:
                sstream << " (clean)";
                break;
            case DirtyState::Dirty:
                sstream << " (dirty)";
                break;
            default:
                sstream << " (potentially dirty)";
                break;
        }
        return sstream.str();
    }

    static std::string buildInfo() {
        std::stringstream sstream;
        sstream << "Compiled on " << systemName << " " << systemVersion << " using " << cxxCompiler << " with flags '" << cxxFlags << "'";
        return sstream.str();
    }
};
}  // namespace storm
