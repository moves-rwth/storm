#include "src/solver/Smt2SmtSolver.h"

#include "src/exceptions/NotSupportedException.h"
#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/NotImplementedException.h"
#include "utility/macros.h"
#include "adapters/CarlAdapter.h"
#include "exceptions/IllegalArgumentException.h"
#include "exceptions/IllegalFunctionCallException.h"

namespace storm {
    namespace solver {

        Smt2SmtSolver::Smt2ModelReference::Smt2ModelReference(storm::expressions::ExpressionManager const& manager, storm::adapters::Smt2ExpressionAdapter& expressionAdapter) : ModelReference(manager), expressionAdapter(expressionAdapter){
            // Intentionally left empty.
        }

        bool Smt2SmtSolver::Smt2ModelReference::getBooleanValue(storm::expressions::Variable const& variable) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "functionality not (yet) implemented");
        }

        int_fast64_t Smt2SmtSolver::Smt2ModelReference::getIntegerValue(storm::expressions::Variable const& variable) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "functionality not (yet) implemented");
        }

        double Smt2SmtSolver::Smt2ModelReference::getRationalValue(storm::expressions::Variable const& variable) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "functionality not (yet) implemented");
        }

        Smt2SmtSolver::Smt2SmtSolver(storm::expressions::ExpressionManager& manager, bool useCarlExpressions) : SmtSolver(manager), expressionAdapter(nullptr), lastCheckAssumptions(false), lastResult(CheckResult::Unknown), useCarlExpressions(useCarlExpressions) {
#ifndef STORM_HAVE_CARL
            STORM_LOG_THROW(!useCarlExpressions, storm::exceptions::IllegalArgumentException, "Tried to use carl expressions but storm is not linked with CARL");
#endif
            expressionAdapter = std::unique_ptr<storm::adapters::Smt2ExpressionAdapter>(new storm::adapters::Smt2ExpressionAdapter(this->getManager(), true));
            init();
        }

        Smt2SmtSolver::~Smt2SmtSolver() {
            writeCommand("( exit )");
            //todo make sure that the process exits
        }

        void Smt2SmtSolver::push() {
            expressionAdapter->increaseScope();
            writeCommand("( push 1 ) ");
        }

        void Smt2SmtSolver::pop() {
            expressionAdapter->decreaseScope();
            writeCommand("( pop 1 ) ");
        }

        void Smt2SmtSolver::pop(uint_fast64_t n) {
            expressionAdapter->decreaseScope(n);
            writeCommand("( pop " + std::to_string(n) + " ) ");
        }

        void Smt2SmtSolver::reset() {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "functionality not (yet) implemented");
        }

        void Smt2SmtSolver::add(storm::expressions::Expression const& assertion) {
            STORM_LOG_THROW(!useCarlExpressions, storm::exceptions::IllegalFunctionCallException, "This solver was initialized without allowing carl expressions");
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "functionality not (yet) implemented");
        }

#ifdef STORM_HAVE_CARL        
        void Smt2SmtSolver::add(storm::RationalFunction const& leftHandSide, storm::CompareRelation const& relation, storm::RationalFunction const& rightHandSide) {
            STORM_LOG_THROW(useCarlExpressions, storm::exceptions::IllegalFunctionCallException, "This solver was initialized without allowing carl expressions");
            //if some of the occurring variables are not declared yet, we will have to.
            std::set<storm::Variable> variables;
            leftHandSide.gatherVariables(variables);
            rightHandSide.gatherVariables(variables);
            std::vector<std::string> const varDeclarations = expressionAdapter->checkForUndeclaredVariables(variables);
            for (auto declaration : varDeclarations){
                writeCommand(declaration);
            }
            writeCommand("( assert " + expressionAdapter->translateExpression(leftHandSide, relation, rightHandSide) + " )");
        }
        
        template<>
        void Smt2SmtSolver::add(carl::Constraint<storm::RationalFunction> const& constraint) {
            add(constraint.lhs(), constraint.rel());
        }
        
        template<>
        void Smt2SmtSolver::add(carl::Constraint<storm::RawPolynomial> const& constraint) {
            //if some of the occurring variables are not declared yet, we will have to.
            std::set<storm::Variable> variables = constraint.lhs().gatherVariables();
            std::vector<std::string> const varDeclarations = expressionAdapter->checkForUndeclaredVariables(variables);
            for (auto declaration : varDeclarations){
                writeCommand(declaration);
            }
            writeCommand("( assert " + expressionAdapter->translateExpression(constraint) + " )");
        }
        
        
#endif

        SmtSolver::CheckResult Smt2SmtSolver::check() {
            writeCommand("( check-sat )");
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "functionality not (yet) implemented");
        }

        SmtSolver::CheckResult Smt2SmtSolver::checkWithAssumptions(std::set<storm::expressions::Expression> const& assumptions) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "functionality not (yet) implemented");
        }

#ifndef WINDOWS

        SmtSolver::CheckResult Smt2SmtSolver::checkWithAssumptions(std::initializer_list<storm::expressions::Expression> const& assumptions) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "functionality not (yet) implemented");
        }
#endif

        void Smt2SmtSolver::init() {
            //hard coded output file.. for now
            commandFile.open("/home/tim/Desktop/smtlibcommand.txt", std::ios::trunc);
            STORM_LOG_THROW(commandFile.is_open(), storm::exceptions::InvalidArgumentException, "The file where the smt2commands should be written to could not be opened");

            //some initial commands
            writeCommand("( set-logic QF_NRA )");

        }

        void Smt2SmtSolver::writeCommand(std::string smt2Command) {
            if (commandFile.is_open()) {
                commandFile << smt2Command << std::endl;
            } else{
                std::cout << "COMMAND FILE IS CLOSED" <<std::endl;
            }
            
            //todo actually write to the solver
        }
    }
}