#pragma once

#include "src/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            /*!
             * This class represents the settings for Eigen.
             */
            class EigenEquationSolverSettings : public ModuleSettings {
            public:
                // An enumeration of all available methods for solving linear equations.
                enum class LinearEquationMethod { SparseLU, Bicgstab };
                
                // An enumeration of all available preconditioning methods.
                enum class PreconditioningMethod { Ilu, Diagonal, None };
                
                // An enumeration of all available convergence criteria.
                enum class ConvergenceCriterion { Absolute, Relative };
                
                /*!
                 * Creates a new set of Eigen settings.
                 */
                EigenEquationSolverSettings();
                
                /*!
                 * Retrieves whether the linear equation system method has been set.
                 *
                 * @return True iff the linear equation system method has been set.
                 */
                bool isLinearEquationSystemMethodSet() const;
                
                /*!
                 * Retrieves the method that is to be used for solving systems of linear equations.
                 *
                 * @return The method to use.
                 */
                LinearEquationMethod getLinearEquationSystemMethod() const;
                
                /*!
                 * Retrieves whether the preconditioning method has been set.
                 *
                 * @return True iff the preconditioning method has been set.
                 */
                bool isPreconditioningMethodSet() const;
                
                /*!
                 * Retrieves the method that is to be used for preconditioning solving systems of linear equations.
                 *
                 * @return The method to use.
                 */
                PreconditioningMethod getPreconditioningMethod() const;
                
                /*!
                 * Retrieves whether the maximal iteration count has been set.
                 *
                 * @return True iff the maximal iteration count has been set.
                 */
                bool isMaximalIterationCountSet() const;
                
                /*!
                 * Retrieves the maximal number of iterations to perform until giving up on converging.
                 *
                 * @return The maximal iteration count.
                 */
                uint_fast64_t getMaximalIterationCount() const;
                
                /*!
                 * Retrieves whether the precision has been set.
                 *
                 * @return True iff the precision has been set.
                 */
                bool isPrecisionSet() const;
                
                /*!
                 * Retrieves the precision that is used for detecting convergence.
                 *
                 * @return The precision to use for detecting convergence.
                 */
                double getPrecision() const;
                                
                bool check() const override;
                
                // The name of the module.
                static const std::string moduleName;
                
            private:
                // Define the string names of the options as constants.
                static const std::string techniqueOptionName;
                static const std::string preconditionOptionName;
                static const std::string maximalIterationsOptionName;
                static const std::string maximalIterationsOptionShortName;
                static const std::string precisionOptionName;
            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm
