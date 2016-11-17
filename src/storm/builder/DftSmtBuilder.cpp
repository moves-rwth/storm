#include "src/storm/builder/DftSmtBuilder.h"
#include "src/storm/exceptions/NotImplementedException.h"

namespace storm {
    namespace builder {

        template <typename ValueType>
        DFTSMTBuilder<ValueType>::DFTSMTBuilder() : manager(std::make_shared<storm::expressions::ExpressionManager>()) {
            solver = storm::utility::solver::SmtSolverFactory().create(*manager);
        }

        template <typename ValueType>
        void DFTSMTBuilder<ValueType>::convertToSMT(storm::storage::DFT<ValueType> const& dft) {
            std::cout << "Convert DFT to SMT" << std::endl;
            timeMax = manager->integer(dft.nrBasicElements());
            timeFailSafe = manager->integer(dft.nrBasicElements() + 1);
            timeZero = manager->integer(0);
            
            // Convert all elements
            for (size_t i = 0; i < dft.nrElements(); ++i) {
                std::shared_ptr<storm::storage::DFTElement<ValueType> const> element = dft.getElement(i);
                std::cout << "Consider " << element->toString() << std::endl;
                if (element->isBasicElement()) {
                    storm::expressions::Variable varBE = convert(std::static_pointer_cast<storm::storage::DFTBE<ValueType> const>(element));
                    varsBE.push_back(varBE);
                } else if (element->isGate()) {
                    storm::expressions::Variable varGate = convert(std::static_pointer_cast<storm::storage::DFTGate<ValueType> const>(element));
                    if (dft.getTopLevelIndex() == i) {
                        topLevel = varGate;
                    }
                } else if (element->isDependency()) {
                    convert(std::static_pointer_cast<storm::storage::DFTDependency<ValueType> const>(element));
                } else if (element->isRestriction()) {
                    convert(std::static_pointer_cast<storm::storage::DFTRestriction<ValueType> const>(element));
                }
            }
            
            // No simultaneous fails can occur
            for (size_t i = 0; i < varsBE.size() - 1; ++i) {
                storm::expressions::Expression be = varsBE[i];
                for (size_t j = i + 1; j < varsBE.size(); ++j) {
                    storm::expressions::Expression assertion = be != varsBE[j];
                    solver->add(assertion);
                }
            }
            
            // For every time-point one BE must fail
            for (size_t time = 1; time <= dft.nrBasicElements(); ++time) {
                storm::expressions::Expression exprTime = manager->integer(time);
                storm::expressions::Expression assertion = varsBE[0] == exprTime;
                for (size_t i = 1; i < varsBE.size(); ++i) {
                    assertion = assertion || varsBE[i] == exprTime;
                }
                assertion = assertion || topLevel <= exprTime;
                solver->add(assertion);
            }
            
        }
        
        template <typename ValueType>
        storm::expressions::Variable DFTSMTBuilder<ValueType>::convert(std::shared_ptr<storm::storage::DFTBE<ValueType> const> const& be) {
            storm::expressions::Variable var = manager->declareIntegerVariable(be->name());
            storm::expressions::Expression assertion = timeZero < var && var < timeFailSafe;
            solver->add(assertion);
            return var;
        }
        
        template <typename ValueType>
        storm::expressions::Variable DFTSMTBuilder<ValueType>::convert(std::shared_ptr<storm::storage::DFTGate<ValueType> const> const& gate) {
            storm::expressions::Variable var = manager->declareIntegerVariable(gate->name());
            storm::expressions::Expression assertion = timeZero < var && var <= timeFailSafe;
            solver->add(assertion);
            return var;
        }
        
        template <typename ValueType>
        storm::expressions::Variable DFTSMTBuilder<ValueType>::convert(std::shared_ptr<storm::storage::DFTDependency<ValueType> const> const& dependency) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "The dependency cannot be converted into SMT.");
        }
        
        template <typename ValueType>
        storm::expressions::Variable DFTSMTBuilder<ValueType>::convert(std::shared_ptr<storm::storage::DFTRestriction<ValueType> const> const& restriction) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "The restriction cannot be converted into SMT.");
        }

        template <typename ValueType>
        bool DFTSMTBuilder<ValueType>::check() const {
            std::cout << "Check" << std::endl;
            storm::solver::SmtSolver::CheckResult result = solver->check();
            switch (result) {
                case solver::SmtSolver::CheckResult::Sat:
                {
                    std::cout << "SAT with model:" << std::endl;
                    std::shared_ptr<storm::solver::SmtSolver::ModelReference> model = solver->getModel();
                    for (auto const& pair : *manager) {
                        std::cout << pair.first.getName() << "->" << model->getIntegerValue(pair.first) << std::endl;
                    }
                    return true;
                }
                case solver::SmtSolver::CheckResult::Unsat:
                    return false;
                case solver::SmtSolver::CheckResult::Unknown:
                default:
                    STORM_LOG_ASSERT(false, "Result is unknown.");
                    return false;
            }
        }
        

        // Explicitly instantiate the class.
        template class DFTSMTBuilder<double>;

#ifdef STORM_HAVE_CARL
        template class DFTSMTBuilder<storm::RationalFunction>;
#endif

    } // namespace builder
} // namespace storm


