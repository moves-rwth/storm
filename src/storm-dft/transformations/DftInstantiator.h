#pragma once

#include "storm-dft/storage/dft/DFT.h"
#include "storm-pars/utility/parametric.h"


namespace storm {
    namespace transformations {
        namespace dft {

            /*!
             * Instantiator to yield a concrete DFT from a parametric DFT (with parametric failure rates).
             */
            template<typename ParametricType, typename ConstantType>
            class DftInstantiator {

            public:
                /*!
                 * Constructor.
                 *
                 * @param dft DFT
                 */
                DftInstantiator(storm::storage::DFT<ParametricType> const &dft);

                /*!
                 * Destructs the Instantiator
                 */
                virtual ~DftInstantiator() = default;

                /*!
                 * Evaluates the occurring parametric functions and retrieves the instantiated DFT.
                 * @param valuation Maps each occurring variables to the value with which it should be substituted.
                 * @return The instantiated DFT.
                 */
                std::shared_ptr<storm::storage::DFT<ConstantType>> instantiate(storm::utility::parametric::Valuation<ParametricType> const& valuation);

                /*!
                 *  Check validity
                 */
                void checkValid() const;


            private:
                storm::storage::DFT<ParametricType> const& dft;

                std::vector<std::string> getChildrenVector(std::shared_ptr<storm::storage::DFTElement<ParametricType> const> element);
            };
        }
    }
}
