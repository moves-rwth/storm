#pragma once

#include <cstdint>
#include <functional>
#include <string>

namespace storm {
namespace cli {

/*!
 * Processes the options and returns the exit code.
 */
int process(std::string const& name, std::string const& executableName, std::function<void(std::string const&, std::string const&)> initSettingsFunc,
            std::function<void(void)> processOptionsFunc, const int argc, const char** argv);

}  // namespace cli
}  // namespace storm
