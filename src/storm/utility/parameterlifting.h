#ifndef STORM_UTILITY_PARAMETERLIFTING_H
#define STORM_UTILITY_PARAMETERLIFTING_H

#include "storm/models/sparse/Model.h"
#include "storm/utility/parametric.h"
#include "storm/utility/macros.h"
#include "storm/logic/Formula.h"
#include "storm/logic/FragmentSpecification.h"


namespace storm {
    namespace utility {
        namespace parameterlifting {
            
            /*!
             * Checks whether the parameter lifting approach is sound on the given model with respect to the provided property
             *
             * This method is taylored to an efficient but incomplete check, i.e., if false is returned,
             * parameter lifting might still be applicable. Checking this, however, would be more involved.
             *
             * @param model
             * @param formula
             * @return true iff it was successfully validated that parameter lifting is sound on the provided model.
             */
            template<typename ValueType>
            static bool validateParameterLiftingSound(storm::models::sparse::Model<ValueType> const& model, storm::logic::Formula const& formula) {
                switch (model.getType()) {
                    default:
                        return false;
                }
            }
            
        }
    }
}


#endif /* STORM_UTILITY_PARAMETERLIFTING_H */
