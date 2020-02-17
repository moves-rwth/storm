#include "storm/utility/Engine.h"

#include "storm/utility/macros.h"

#include "storm/models/ModelType.h"

#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "storm/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "storm/modelchecker/csl/SparseCtmcCslModelChecker.h"
#include "storm/modelchecker/csl/SparseMarkovAutomatonCslModelChecker.h"

#include "storm/modelchecker/prctl/HybridDtmcPrctlModelChecker.h"
#include "storm/modelchecker/prctl/HybridMdpPrctlModelChecker.h"
#include "storm/modelchecker/csl/HybridCtmcCslModelChecker.h"

#include "storm/modelchecker/prctl/SymbolicDtmcPrctlModelChecker.h"
#include "storm/modelchecker/prctl/SymbolicMdpPrctlModelChecker.h"
#include "storm/modelchecker/CheckTask.h"

#include "storm/storage/SymbolicModelDescription.h"

#include "storm/utility/macros.h"

#include "storm/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace utility {
        
        // Returns a list of all available engines (excluding Unknown)
        std::vector<Engine> getEngines() {
            std::vector<Engine> res;
            for (int i = 0; i != static_cast<int>(Engine::Unknown); ++i) {
                res.push_back(static_cast<Engine>(i));
            }
            return res;
        }
        
        std::string toString(Engine const& engine) {
            switch (engine) {
                case Engine::Sparse:
                    return "sparse";
                case Engine::Hybrid:
                    return "hybrid";
                case Engine::Dd:
                    return "dd";
                case Engine::DdSparse:
                    return "dd-to-sparse";
                case Engine::Jit:
                    return "jit";
                case Engine::Exploration:
                    return "expl";
                case Engine::AbstractionRefinement:
                    return "abs";
                case Engine::Unknown:
                    return "UNKNOWN";
                default:
                    STORM_LOG_ASSERT(false, "The given engine has no name assigned to it.");
                    return "UNKNOWN";
            }
        }
        
        std::ostream& operator<<(std::ostream& os, Engine const& engine) {
            os << toString(engine);
            return os;
        }

        Engine engineFromString(std::string const& engineStr) {
            for (Engine const& e : getEngines()) {
                if (engineStr == toString(e)) {
                    return e;
                }
            }
            STORM_LOG_ERROR("The engine '" << engineStr << "' was not found.");
            return Engine::Unknown;
        }
        
        storm::builder::BuilderType getBuilderType(Engine const& engine) {
            switch (engine) {
                case Engine::Sparse:
                    return storm::builder::BuilderType::Explicit;
                case Engine::Hybrid:
                    return storm::builder::BuilderType::Dd;
                case Engine::Dd:
                    return storm::builder::BuilderType::Dd;
                case Engine::DdSparse:
                    return storm::builder::BuilderType::Dd;
                case Engine::Jit:
                    return storm::builder::BuilderType::Jit;
                case Engine::Exploration:
                return storm::builder::BuilderType::Explicit;
                case Engine::AbstractionRefinement:
                    return storm::builder::BuilderType::Dd;
                default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The given engine has no builder type to it.");
                    return storm::builder::BuilderType::Explicit;
            }
        }

        template <storm::dd::DdType ddType, typename ValueType>
        bool canHandle(storm::utility::Engine const& engine, storm::models::ModelType const& modelType, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& checkTask) {
            // Define types to improve readability
            typedef storm::models::ModelType ModelType;
#ifdef TODO_IMPLEMENT_CAN_HANDLE_STATIC
            switch (engine) {
                case Engine::Sparse:
                case Engine::DdSparse:
                    switch (modelType) {
                        case ModelType::Dtmc:
                            return storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<ValueType>>::canHandleStatic(checkTask);
                        case ModelType::Mdp:
                            return storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<ValueType>>::canHandleStatic(checkTask);
                        case ModelType::Ctmc:
                            return storm::modelchecker::SparseCtmcCslModelChecker<storm::models::sparse::Ctmc<ValueType>>::canHandleStatic(checkTask);
                        case ModelType::MarkovAutomaton:
                            return storm::modelchecker::SparseMarkovAutomatonCslModelChecker<storm::models::sparse::MarkovAutomaton<ValueType>>::canHandleStatic(checkTask);
                        case ModelType::S2pg:
                        case ModelType::Pomdp:
                            return false;
                    }
                    break;
                case Engine::Hybrid:
                    switch (modelType) {
                        case ModelType::Dtmc:
                            return storm::modelchecker::HybridDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<ddType, ValueType>>::canHandleStatic(checkTask);
                        case ModelType::Mdp:
                            return storm::modelchecker::HybridMdpPrctlModelChecker<storm::models::symbolic::Mdp<ddType, ValueType>>::canHandleStatic(checkTask);
                        case ModelType::Ctmc:
                            return storm::modelchecker::HybridCtmcCslModelChecker<storm::models::symbolic::Ctmc<ddType, ValueType>>::canHandleStatic(checkTask);
                        case ModelType::MarkovAutomaton:
                        case ModelType::S2pg:
                        case ModelType::Pomdp:
                            return false;
                    }
                    break;
                case Engine::Dd:
                    switch (modelType) {
                        case ModelType::Dtmc:
                            return storm::modelchecker::SymbolicDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<ddType, ValueType>>::canHandleStatic(checkTask);
                        case ModelType::Mdp:
                            return storm::modelchecker::SymbolicMdpPrctlModelChecker<storm::models::symbolic::Mdp<ddType, ValueType>>::canHandleStatic(checkTask);
                        case ModelType::Ctmc:
                        case ModelType::MarkovAutomaton:
                        case ModelType::S2pg:
                        case ModelType::Pomdp:
                            return false;
                    }
                    break;
                default:
                    STORM_LOG_ERROR("The selected engine" << engine << " is not considered.");
            }
#endif
            STORM_LOG_ERROR("The selected combination of engine (" << engine << ") and model type (" << modelType << ") does not seem to be supported for this value type.");
            return false;
        }
        
        template <storm::dd::DdType ddType, typename ValueType>
        bool canHandle(storm::utility::Engine const& engine, storm::storage::SymbolicModelDescription const& modelDescription, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& checkTask) {
            // Check handability based on model type
            if (!canHandle(engine, modelDescription.getModelType(), checkTask)) {
                return false;
            }
            // TODO
            return true;
        }
        

        
    }
}