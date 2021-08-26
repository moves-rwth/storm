#ifndef STORM_SETTINGS_MODULES_DERIVATIVESETTINGS_H_
#define STORM_SETTINGS_MODULES_DERIVATIVESETTINGS_H_

#include "adapters/RationalFunctionAdapter.h"
#include "storm-pars/derivative/GradientDescentMethod.h"
#include "storm/settings/modules/ModuleSettings.h"
#include <boost/optional.hpp>
#include <boost/none.hpp>

namespace storm {
    namespace settings {
        namespace modules {

            /*!
             * This class represents the settings for Gradient Descent.
             */
            class DerivativeSettings : public ModuleSettings {
            public:

                /*!
                 * Creates a new set of monotonicity checking settings.
                 */
                DerivativeSettings();

								/*!
								 * Retrieves whether a feasible instance should be found by Gradient Descent.
								 */
								bool isFeasibleInstantiationSearchSet() const;

								/*!
								 * Retrieves whether an extremum should be found by Gradient Descent.
								 */
                boost::optional<std::string> getDerivativeAtInstantiation() const;

                /*!
								 * Retrieves the learning rate for the gradient descent.
								 */
                double getLearningRate() const;

                /*!
                 * Retrieves the mini batch size of the gradient descent.
                 */
                uint_fast64_t getMiniBatchSize() const;

                /*!
                 * Retrieves the decay of the decaying step average of the ADAM algorithm.
                 */
                double getAverageDecay() const;

                /*!
                 * Retrieves the decay of the squared decaying step average of the ADAM algorithm.
                 */
                double getSquaredAverageDecay() const;

                /*!
                 * Retrieves whether the GradientDescentInstantiationSearcher should print the run as json after finishing.
                 */
                bool isPrintJsonSet() const;

                /*!
                 * Retrieves the termination epsilon.
                 */
                double getTerminationEpsilon() const;

                /*!
                 * Retrieves the gradient descent method.
                 */
                boost::optional<derivative::GradientDescentMethod> getGradientDescentMethod() const;

                /*!
                 * Retrieves the gradient descent method as a string.
                 */
                std::string getGradientDescentMethodAsString() const;

                /*!
                 * Are inconsequential parameters omitted?
                 */
                bool areInconsequentialParametersOmitted() const;

                /*!
                 * Get start point
                 */
                boost::optional<std::string> getStartPoint() const;

                const static std::string moduleName;
            private:
                const static std::string extremumSearch;
                const static std::string feasibleInstantiationSearch;
                const static std::string derivativeAtInstantiation;
                const static std::string learningRate;
                const static std::string miniBatchSize;
                const static std::string adamParams;
                const static std::string averageDecay;
                const static std::string squaredAverageDecay;
                const static std::string printJson;
                const static std::string terminationEpsilon;
                const static std::string gradientDescentMethod;
                const static std::string omitInconsequentialParams;
                const static std::string startPoint;
                boost::optional<derivative::GradientDescentMethod> methodFromString(const std::string &str) const;
            };

        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_DERIVATIVESETTINGS_H_ */
