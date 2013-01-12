#include "src/models/AbstractModel.h"

#include <iostream>

/*!
 *	This method will output the name of the model type or "Unknown".
 *	If something went terribly wrong, i.e. if type does not contain any value
 *	that is valid for a ModelType or some value of the enum was not
 *	implemented here, it will output "Invalid ModelType".
 *
 *	@param os Output stream.
 *	@param type	Model type.
 *	@return Output stream os.
 */
std::ostream& storm::models::operator<<(std::ostream& os, storm::models::ModelType const type)
{
    switch (type) {
        case storm::models::Unknown: os << "Unknown"; break;
        case storm::models::DTMC: os << "DTMC"; break;
        case storm::models::CTMC: os << "CTMC"; break;
        case storm::models::MDP: os << "MDP"; break;
        case storm::models::CTMDP: os << "CTMDP"; break;
        default: os << "Invalid ModelType"; break;
    }   
    return os;
}
