#include "GmmxxLinearEquationSolver.h"

#include <cmath>
#include <utility>

#include "src/adapters/GmmxxAdapter.h"
#include "src/settings/Settings.h"
#include "src/utility/vector.h"
#include "src/utility/constants.h"
#include "src/exceptions/InvalidStateException.h"

#include "gmm/gmm_matrix.h"
#include "gmm/gmm_iter_solvers.h"

bool GmmxxLinearEquationSolverOptionsRegistered = storm::settings::Settings::registerNewModule([] (storm::settings::Settings* instance) -> bool {
    // Offer all available methods as a command line option.
	std::vector<std::string> methods;
	methods.push_back("bicgstab");
	methods.push_back("qmr");
	methods.push_back("gmres");
	methods.push_back("jacobi");
	instance->addOption(storm::settings::OptionBuilder("GmmxxLinearEquationSolver", "gmmlin", "", "The method to be used for solving linear equation systems with the gmm++ engine. Available are: bicgstab, qmr, gmres, jacobi.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the method to use.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(methods)).setDefaultValueString("gmres").build()).build());

    // Register available preconditioners.
    std::vector<std::string> preconditioner;
	preconditioner.push_back("ilu");
	preconditioner.push_back("diagonal");
	preconditioner.push_back("ildlt");
	preconditioner.push_back("none");
	instance->addOption(storm::settings::OptionBuilder("GmmxxLinearEquationSolver", "gmmpre", "", "The preconditioning technique used for solving linear equation systems with the gmm++ engine. Available are: ilu, diagonal, none.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the preconditioning method.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(preconditioner)).setDefaultValueString("ilu").build()).build());

    instance->addOption(storm::settings::OptionBuilder("GmmxxLinearEquationSolver", "gmmrestart", "", "The number of iteration until restarted methods are actually restarted.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The number of iterations.").setDefaultValueUnsignedInteger(50).build()).build());

	instance->addOption(storm::settings::OptionBuilder("GmmxxLinearEquationSolver", "maxiter", "i", "The maximal number of iterations to perform before iterative solving is aborted.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The maximal iteration count.").setDefaultValueUnsignedInteger(10000).build()).build());
    
	instance->addOption(storm::settings::OptionBuilder("GmmxxLinearEquationSolver", "precision", "", "The precision used for detecting convergence of iterative methods.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-06).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
    
	instance->addOption(storm::settings::OptionBuilder("GmmxxLinearEquationSolver", "absolute", "", "Whether the relative or the absolute error is considered for deciding convergence.").build());

	return true;
});

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        GmmxxLinearEquationSolver<ValueType>::GmmxxLinearEquationSolver(SolutionMethod method, double precision, uint_fast64_t maximalNumberOfIterations, Preconditioner preconditioner, bool relative, uint_fast64_t restart) : method(method), precision(precision), maximalNumberOfIterations(maximalNumberOfIterations), preconditioner(preconditioner), restart(restart) {
            // Intentionally left empty.
        }
        

        template<typename ValueType>
        GmmxxLinearEquationSolver<ValueType>::GmmxxLinearEquationSolver() {
            // Get the settings object to customize linear solving.
            storm::settings::Settings* settings = storm::settings::Settings::getInstance();
            
            // Get appropriate settings.
            maximalNumberOfIterations = settings->getOptionByLongName("maxiter").getArgument(0).getValueAsUnsignedInteger();
            precision = settings->getOptionByLongName("precision").getArgument(0).getValueAsDouble();
            relative = !settings->isSet("absolute");
            restart = settings->getOptionByLongName("gmmrestart").getArgument(0).getValueAsUnsignedInteger();
            
            // Determine the method to be used.
            std::string const& methodAsString = settings->getOptionByLongName("gmmlin").getArgument(0).getValueAsString();
            if (methodAsString == "bicgstab") {
                method = BICGSTAB;
            } else if (methodAsString == "qmr") {
                method = QMR;
            } else if (methodAsString == "gmres") {
                method = GMRES;
            } else if (methodAsString == "jacobi") {
                method = JACOBI;
            }
            
            // Check which preconditioner to use.
            std::string const& preconditionAsString = settings->getOptionByLongName("gmmpre").getArgument(0).getValueAsString();
            if (preconditionAsString == "ilu") {
                preconditioner = ILU;
            } else if (preconditionAsString == "diagonal") {
                preconditioner = DIAGONAL;
            } else if (preconditionAsString == "none") {
                preconditioner = NONE;
            }
        }
        
        template<typename ValueType>
        LinearEquationSolver<ValueType>* GmmxxLinearEquationSolver<ValueType>::clone() const {
            return new GmmxxLinearEquationSolver<ValueType>(*this);
        }
        
        template<typename ValueType>
        void GmmxxLinearEquationSolver<ValueType>::solveEquationSystem(storm::storage::SparseMatrix<ValueType> const& A, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult) const {
            LOG4CPLUS_INFO(logger, "Using method '" << methodToString() << "' with preconditioner " << preconditionerToString() << "'.");
            if (method == JACOBI && preconditioner != NONE) {
                LOG4CPLUS_WARN(logger, "Jacobi method currently does not support preconditioners. The requested preconditioner will be ignored.");
            }
            
            if (method == BICGSTAB || method == QMR || method == GMRES) {
                std::unique_ptr<gmm::csr_matrix<ValueType>> gmmA = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<ValueType>(A);
                
                // Prepare an iteration object that determines the accuracy and the maximum number of iterations.
                gmm::iteration iter(precision, 0, maximalNumberOfIterations);
                
                if (method == BICGSTAB) {
                    if (preconditioner == ILU) {
                        gmm::bicgstab(*gmmA, x, b, gmm::ilu_precond<gmm::csr_matrix<ValueType>>(*gmmA), iter);
                    } else if (preconditioner == DIAGONAL) {
                        gmm::bicgstab(*gmmA, x, b, gmm::diagonal_precond<gmm::csr_matrix<ValueType>>(*gmmA), iter);
                    } else if (preconditioner == NONE) {
                        gmm::bicgstab(*gmmA, x, b, gmm::identity_matrix(), iter);
                    }
                } else if (method == QMR) {
                    if (preconditioner == ILU) {
                        gmm::qmr(*gmmA, x, b, gmm::ilu_precond<gmm::csr_matrix<ValueType>>(*gmmA), iter);
                    } else if (preconditioner == DIAGONAL) {
                        gmm::qmr(*gmmA, x, b, gmm::diagonal_precond<gmm::csr_matrix<ValueType>>(*gmmA), iter);
                    } else if (preconditioner == NONE) {
                        gmm::qmr(*gmmA, x, b, gmm::identity_matrix(), iter);
                    }
                } else if (method == GMRES) {
                    if (preconditioner == ILU) {
                        gmm::gmres(*gmmA, x, b, gmm::ilu_precond<gmm::csr_matrix<ValueType>>(*gmmA), restart, iter);
                    } else if (preconditioner == DIAGONAL) {
                        gmm::gmres(*gmmA, x, b, gmm::diagonal_precond<gmm::csr_matrix<ValueType>>(*gmmA), restart, iter);
                    } else if (preconditioner == NONE) {
                        gmm::gmres(*gmmA, x, b, gmm::identity_matrix(), restart, iter);
                    }
                }
                
                // Check if the solver converged and issue a warning otherwise.
                if (iter.converged()) {
                    LOG4CPLUS_INFO(logger, "Iterative solver converged after " << iter.get_iteration() << " iterations.");
                } else {
                    LOG4CPLUS_WARN(logger, "Iterative solver did not converge.");
                }
            } else if (method == JACOBI) {
                uint_fast64_t iterations = solveLinearEquationSystemWithJacobi(A, x, b, multiplyResult);
                
                // Check if the solver converged and issue a warning otherwise.
                if (iterations < maximalNumberOfIterations) {
                    LOG4CPLUS_INFO(logger, "Iterative solver converged after " << iterations << " iterations.");
                } else {
                    LOG4CPLUS_WARN(logger, "Iterative solver did not converge.");
                }
            }
        }
        
        template<typename ValueType>
        void GmmxxLinearEquationSolver<ValueType>::performMatrixVectorMultiplication(storm::storage::SparseMatrix<ValueType> const& A, std::vector<ValueType>& x, std::vector<ValueType>* b, uint_fast64_t n, std::vector<ValueType>* multiplyResult) const {
            // Transform the transition probability A to the gmm++ format to use its arithmetic.
            std::unique_ptr<gmm::csr_matrix<ValueType>> gmmxxMatrix = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<ValueType>(A);

            // Set up some temporary variables so that we can just swap pointers instead of copying the result after
            // each iteration.
            std::vector<ValueType>* swap = nullptr;
            std::vector<ValueType>* currentX = &x;

            bool multiplyResultProvided = true;
            std::vector<ValueType>* nextX = multiplyResult;
            if (nextX == nullptr) {
                nextX = new std::vector<ValueType>(x.size());
                multiplyResultProvided = false;
            }
            std::vector<ValueType> const* copyX = nextX;
            
            // Now perform matrix-vector multiplication as long as we meet the bound.
            for (uint_fast64_t i = 0; i < n; ++i) {
                gmm::mult(*gmmxxMatrix, *currentX, *nextX);
                std::swap(nextX, currentX);
                
                // If requested, add an offset to the current result vector.
                if (b != nullptr) {
                    gmm::add(*b, *currentX);
                }
            }
            
            // If we performed an odd number of repetitions, we need to swap the contents of currentVector and x,
            // because the output is supposed to be stored in the input vector x.
            if (currentX == copyX) {
                std::swap(x, *currentX);
            }
            
            // If the vector for the temporary multiplication result was not provided, we need to delete it.
            if (!multiplyResultProvided) {
                delete copyX;
            }
        }
        
        template<typename ValueType>
        uint_fast64_t GmmxxLinearEquationSolver<ValueType>::solveLinearEquationSystemWithJacobi(storm::storage::SparseMatrix<ValueType> const& A, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult) const {
            // Get a Jacobi decomposition of the matrix A.
            std::pair<storm::storage::SparseMatrix<ValueType>, storm::storage::SparseMatrix<ValueType>> jacobiDecomposition = A.getJacobiDecomposition();
            
            // Convert the (inverted) diagonal matrix to gmm++'s format.
            std::unique_ptr<gmm::csr_matrix<ValueType>> gmmDinv = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<ValueType>(std::move(jacobiDecomposition.second));
            // Convert the LU matrix to gmm++'s format.
            std::unique_ptr<gmm::csr_matrix<ValueType>> gmmLU = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<ValueType>(std::move(jacobiDecomposition.first));
        
            // To avoid copying the contents of the vector in the loop, we create a temporary x to swap with.
            bool multiplyResultProvided = true;
            std::vector<ValueType>* nextX = multiplyResult;
            if (nextX == nullptr) {
                nextX = new std::vector<ValueType>(x.size());
                multiplyResultProvided = false;
            }
            std::vector<ValueType> const* copyX = nextX;
            std::vector<ValueType>* currentX = &x;
            
            // Target vector for precision calculation.
            std::vector<ValueType> tmpX(x.size());
            
            // Set up additional environment variables.
            uint_fast64_t iterationCount = 0;
            bool converged = false;
            
            while (!converged && iterationCount < maximalNumberOfIterations) {
                // Compute D^-1 * (b - LU * x) and store result in nextX.
                gmm::mult(*gmmLU, *currentX, tmpX);
                gmm::add(b, gmm::scaled(tmpX, -storm::utility::constantOne<ValueType>()), tmpX);
                gmm::mult(*gmmDinv, tmpX, *nextX);
                
                // Now check if the process already converged within our precision.
                converged = storm::utility::vector::equalModuloPrecision(*currentX, *nextX, precision, relative);

                // Swap the two pointers as a preparation for the next iteration.
                std::swap(nextX, currentX);

                // Increase iteration count so we can abort if convergence is too slow.
                ++iterationCount;
            }
            
            // If the last iteration did not write to the original x we have to swap the contents, because the
            // output has to be written to the input parameter x.
            if (currentX == copyX) {
                std::swap(x, *currentX);
            }
            
            // If the vector for the temporary multiplication result was not provided, we need to delete it.
            if (!multiplyResultProvided) {
                delete copyX;
            }
            
            return iterationCount;
        }
        
        template<typename ValueType>
        std::string GmmxxLinearEquationSolver<ValueType>::methodToString() const {
            if (method == BICGSTAB) {
                return "bicgstab";
            } else if (method == QMR) {
                return "qmr";
            } else if (method == GMRES) {
                return "gmres";
            } else if (method == JACOBI) {
                return "jacobi";
            } else {
                throw storm::exceptions::InvalidStateException() << "Illegal method '" << method << "' set in GmmxxLinearEquationSolver.";
            }
        }
        
        template<typename ValueType>
        std::string GmmxxLinearEquationSolver<ValueType>::preconditionerToString() const {
            if (preconditioner == ILU) {
                return "ilu";
            } else if (preconditioner == DIAGONAL) {
                return "diagonal";
            } else if (preconditioner == NONE) {
                return "none";
            } else {
                throw storm::exceptions::InvalidStateException() << "Illegal preconditioner '" << preconditioner << "' set in GmmxxLinearEquationSolver.";
            }
        }
        
        // Explicitly instantiate the solver.
        template class GmmxxLinearEquationSolver<double>;
    }
}