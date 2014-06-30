#include "src/solver/NativeLinearEquationSolver.h"

#include <utility>

#include "src/settings/Settings.h"
#include "src/utility/vector.h"
#include "src/exceptions/InvalidStateException.h"

bool NativeLinearEquationSolverOptionsRegistered = storm::settings::Settings::registerNewModule([] (storm::settings::Settings* instance) -> bool {
    // Offer all available methods as a command line option.
	std::vector<std::string> methods;
	methods.push_back("jacobi");
	instance->addOption(storm::settings::OptionBuilder("NativeLinearEquationSolver", "nativelin", "", "The method to be used for solving linear equation systems with the native engine. Available are: jacobi.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the method to use.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(methods)).setDefaultValueString("jacobi").build()).build());
    
	instance->addOption(storm::settings::OptionBuilder("NativeLinearEquationSolver", "maxiter", "i", "The maximal number of iterations to perform before iterative solving is aborted.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The maximal iteration count.").setDefaultValueUnsignedInteger(10000).build()).build());
    
	instance->addOption(storm::settings::OptionBuilder("NativeLinearEquationSolver", "precision", "", "The precision used for detecting convergence of iterative methods.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-06).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
    
	instance->addOption(storm::settings::OptionBuilder("NativeLinearEquationSolver", "absolute", "", "Whether the relative or the absolute error is considered for deciding convergence.").build());
    
	return true;
});

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        NativeLinearEquationSolver<ValueType>::NativeLinearEquationSolver(SolutionMethod method, double precision, uint_fast64_t maximalNumberOfIterations, bool relative) : method(method), precision(precision), relative(relative), maximalNumberOfIterations(maximalNumberOfIterations) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        NativeLinearEquationSolver<ValueType>::NativeLinearEquationSolver() {
            // Get the settings object to customize linear solving.
            storm::settings::Settings* settings = storm::settings::Settings::getInstance();
            
            // Get appropriate settings.
            maximalNumberOfIterations = settings->getOptionByLongName("maxiter").getArgument(0).getValueAsUnsignedInteger();
            precision = settings->getOptionByLongName("precision").getArgument(0).getValueAsDouble();
            relative = !settings->isSet("absolute");
            
            // Determine the method to be used.
            std::string const& methodAsString = settings->getOptionByLongName("nativelin").getArgument(0).getValueAsString();
            if (methodAsString == "jacobi") {
                method = JACOBI;
            }
        }
        
        template<typename ValueType>
        LinearEquationSolver<ValueType>* NativeLinearEquationSolver<ValueType>::clone() const {
            return new NativeLinearEquationSolver<ValueType>(*this);
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::solveEquationSystem(storm::storage::SparseMatrix<ValueType> const& A, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult) const {
            // Get a Jacobi decomposition of the matrix A.
            std::pair<storm::storage::SparseMatrix<ValueType>, storm::storage::SparseMatrix<ValueType>> jacobiDecomposition = A.getJacobiDecomposition();
            
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
                jacobiDecomposition.first.multiplyWithVector(*currentX, tmpX);
                storm::utility::vector::scaleVectorInPlace(tmpX, -storm::utility::constantOne<ValueType>());
                storm::utility::vector::addVectorsInPlace(tmpX, b);
                jacobiDecomposition.second.multiplyWithVector(tmpX, *nextX);
                
                // Swap the two pointers as a preparation for the next iteration.
                std::swap(nextX, currentX);
                
                // Now check if the process already converged within our precision.
                converged = storm::utility::vector::equalModuloPrecision(*currentX, *nextX, precision, relative);
                
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
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::performMatrixVectorMultiplication(storm::storage::SparseMatrix<ValueType> const& A, std::vector<ValueType>& x, std::vector<ValueType>* b, uint_fast64_t n, std::vector<ValueType>* multiplyResult) const {
            // Set up some temporary variables so that we can just swap pointers instead of copying the result after
            // each iteration.
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
                A.multiplyWithVector(*currentX, *nextX);
                std::swap(nextX, currentX);
                
                // If requested, add an offset to the current result vector.
                if (b != nullptr) {
                    storm::utility::vector::addVectorsInPlace(*currentX, *b);
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
        std::string NativeLinearEquationSolver<ValueType>::methodToString() const {
            if (method == JACOBI) {
                return "jacobi";
            } else {
                throw storm::exceptions::InvalidStateException() << "Illegal method '" << method << "' set in NativeLinearEquationSolver.";
            }
        }
        
        // Explicitly instantiate the linear equation solver.
        template class NativeLinearEquationSolver<double>;
    }
}