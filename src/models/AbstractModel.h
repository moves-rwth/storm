#ifndef STORM_MODELS_ABSTRACTMODEL_H_
#define STORM_MODELS_ABSTRACTMODEL_H_

#include <memory>

namespace storm {
namespace models {

/*!
 *  @brief  Enumeration of all supported types of models.
 */
enum ModelType {
    Unknown, DTMC, CTMC, MDP, CTMDP
};
/*
// TODO: If we want this to work, it has to be in a cpp... :-)
std::ostream& operator<<(std::ostream& os, const ModelType type)
{
    switch (type) {
        case Unknown: os << "Unknown"; break;
        case DTMC: os << "DTMC"; break;
        case CTMC: os << "CTMC"; break;
        case MDP: os << "MDP"; break;
        case CTMDP: os << "CTMDP"; break;
        default: os << "Invalid ModelType"; break;
    }   
    return os;
}
*/

class AbstractModel {

	public:
		template <typename Model>
		std::shared_ptr<Model> as() {
			//return *dynamic_cast<Model*>(this);
			return std::dynamic_pointer_cast<Model>(std::shared_ptr<AbstractModel>(this));
		}
		
		virtual ModelType getType() = 0;
		
};

} // namespace models
} // namespace storm

#endif /* STORM_MODELS_ABSTRACTMODEL_H_ */