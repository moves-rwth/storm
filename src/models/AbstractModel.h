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

/*!
 *	@brief	Stream output operator for ModelType.
 */
std::ostream& operator<<(std::ostream& os, ModelType const type);

/*!
 *	@brief	Base class for all model classes.
 *
 *	This is base class defines a common interface for all models to identify
 *	their type and obtain the special model.
 */
class AbstractModel {

	public:
		/*!
		 *	@brief Casts the model to the model type that was actually
		 *	created.
		 *
		 *	As all methods that work on generic models will use this
		 *	AbstractModel class, this method provides a convenient way to
		 *	cast an AbstractModel object to an object of a concrete model
		 *	type, which can be obtained via getType(). The mapping from an
		 *	element of the ModelType enum to the actual class must be done
		 *	by the caller.
		 *
		 *	This methods uses std::dynamic_pointer_cast internally.
		 *
		 *	@return Shared pointer of new type to this object.
		 */
		template <typename Model>
		std::shared_ptr<Model> as() {
			return std::dynamic_pointer_cast<Model>(std::shared_ptr<AbstractModel>(this));
		}
		
		/*!
		 *	@brief Return the actual type of the model.
		 *
		 *	Each model must implement this method.
		 *
		 *	@return	Type of the model.
		 */
		virtual ModelType getType() = 0;
		
};

} // namespace models
} // namespace storm

#endif /* STORM_MODELS_ABSTRACTMODEL_H_ */