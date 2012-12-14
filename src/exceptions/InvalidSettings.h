#ifndef INVALIDSETTINGS_H_
#define INVALIDSETTINGS_H_

#include "src/exceptions/BaseException.h"

namespace mrmc {
namespace exceptions {

class InvalidSettings : public BaseException<InvalidSettings>
{
};

} // namespace exceptions
} // namespace mrmc

#endif // INVALIDSETTINGS_H_
