#ifndef STORM_MODELS_ABSTRACTMODEL_H_
#define STORM_MODELS_ABSTRACTMODEL_H_

#include <memory>

#include "src/models/ModelType.h"
#include "src/utility/macros.h"

namespace storm {
    namespace models {
        
        class ModelBase : public std::enable_shared_from_this<ModelBase> {
        public:
            /*!
             * Constructs a model of the given type.
             *
             * @param modelType The type of the model.
             */
            ModelBase(ModelType const& modelType) : modelType(modelType) {
                // Intentionally left empty.
            }

            /*
             * Make destructor virtual to allow deleting objects through pointer to base classe(s).
             */
            virtual ~ModelBase() {
                // Intentionally left empty.
            }
            
            /*!
             * Casts the model into the model type given by the template parameter.
             *
             * @return A shared pointer of the requested type that points to the model if the cast succeeded and a null
             * pointer otherwise.
             */
            template <typename ModelType>
            std::shared_ptr<ModelType> as() {
                return std::dynamic_pointer_cast<ModelType>(this->shared_from_this());
            }
            
            /*!
             *	@brief Return the actual type of the model.
             *
             *	Each model must implement this method.
             *
             *	@return	Type of the model.
             */
            virtual ModelType getType() const;
            
            /*!
             * Returns the number of states of the model.
             *
             * @return The number of states of the model.
             */
            virtual uint_fast64_t getNumberOfStates() const = 0;
            
            /*!
             * Returns the number of (non-zero) transitions of the model.
             *
             * @return The number of (non-zero) transitions of the model.
             */
            virtual uint_fast64_t getNumberOfTransitions() const = 0;
            
            /*!
             * Retrieves (an approximation of) the size of the model in bytes.
             *
             * @return The size of th model in bytes.
             */
            virtual std::size_t getSizeInBytes() const = 0;
            
            /*!
             * Prints information about the model to the specified stream.
             *
             * @param out The stream the information is to be printed to.
             */
            virtual void printModelInformationToStream(std::ostream& out) const = 0;
            
            /*!
             * Checks whether the model is a sparse model.
             *
             * @return True iff the model is a sparse model.
             */
            virtual bool isSparseModel() const;
            
            /*!
             * Checks whether the model is a symbolic model.
             *
             * @return True iff the model is a symbolic model.
             */
            virtual bool isSymbolicModel() const;
            
            /*!
             * Checks whether the model is of the given type.
             *
             * @param modelType The model type to check for.
             * @return True iff the model is of the given type.
             */
            bool isOfType(storm::models::ModelType const& modelType) const;
            
        private:
            // The type of the model.
            ModelType modelType;
        };
        
    } // namespace models
} // namespace storm

#endif /* STORM_MODELS_ABSTRACTMODEL_H_ */
