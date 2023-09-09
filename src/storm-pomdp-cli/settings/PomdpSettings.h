#pragma once

#include <string>

namespace storm {
namespace settings {
/*!
 * Initialize the settings manager.
 */
void initializePomdpSettings(std::string const& name, std::string const& executableName);

}  // namespace settings
}  // namespace storm