#ifndef STORM_SOLVER_SMT2SMTSOLVER
#define STORM_SOLVER_SMT2SMTSOLVER

#include <iostream>
#include <fstream>

#include "storm-config.h"
#include "src/solver/SmtSolver.h"
#include "src/adapters/Smt2ExpressionAdapter.h"
#include "src/adapters/CarlAdapter.h"

namespace storm {
    namespace solver {

        class Smt2SmtSolver : public SmtSolver {
        public:

            class Smt2ModelReference : public SmtSolver::ModelReference {
            public:
                Smt2ModelReference(storm::expressions::ExpressionManager const& manager, storm::adapters::Smt2ExpressionAdapter& expressionAdapter);
                virtual bool getBooleanValue(storm::expressions::Variable const& variable) const override;
                virtual int_fast64_t getIntegerValue(storm::expressions::Variable const& variable) const override;
                virtual double getRationalValue(storm::expressions::Variable const& variable) const override;

            private:

                // The expression adapter that is used to translate the variable names.
                storm::adapters::Smt2ExpressionAdapter& expressionAdapter;
            };

        public:
            /*!
             * Creates a new solver with the given manager.
             * In addition to storm expressions, this solver also allows carl expressions (but not both).
             * Hence, there is a flag to chose between the two
             */
            Smt2SmtSolver(storm::expressions::ExpressionManager& manager, bool useCarlExpressions = false);
            virtual ~Smt2SmtSolver();

            virtual void push() override;

            virtual void pop() override;

            virtual void pop(uint_fast64_t n) override;

            virtual void reset() override;

            virtual void add(storm::expressions::Expression const& assertion) override;
#ifdef STORM_HAVE_CARL         
            //adds the constraint "leftHandSide relation rightHandSide"
            virtual void add(storm::RationalFunction const& leftHandSide, storm::CompareRelation const& relation, storm::RationalFunction const& rightHandSide=storm::RationalFunction(0));
            
            //adds the given carl constraint
            template<typename FunctionType>
            void add(typename carl::Constraint<FunctionType> const& constraint);
#endif

            virtual CheckResult check() override;

            virtual CheckResult checkWithAssumptions(std::set<storm::expressions::Expression> const& assumptions) override;

#ifndef WINDOWS
            virtual CheckResult checkWithAssumptions(std::initializer_list<storm::expressions::Expression> const& assumptions) override;
#endif
            //Todo: some of these might be added in the future
            //virtual storm::expressions::SimpleValuation getModelAsValuation() override;

            //virtual std::shared_ptr<SmtSolver::ModelReference> getModel() override;

            //	virtual std::vector<storm::expressions::SimpleValuation> allSat(std::vector<storm::expressions::Variable> const& important) override;

            //	virtual uint_fast64_t allSat(std::vector<storm::expressions::Variable> const& important, std::function<bool(storm::expressions::SimpleValuation&)> const& callback) override;

            //	virtual uint_fast64_t allSat(std::vector<storm::expressions::Variable> const& important, std::function<bool(ModelReference&)> const& callback) override;

            //	virtual std::vector<storm::expressions::Expression> getUnsatAssumptions() override;

        private:


            /*!
             * Initializes the solver
             */
            void init();


            /*! Writes the given command to the solver
             * @param smt2Command the command that the solver will receive
             */
            void writeCommand(std::string smt2Command);



            // a filestream where the commands that we send to the solver will be stored (can be used for debugging purposes)
            std::ofstream commandFile;

            // An expression adapter that is used for translating the expression into Smt2's format.
            std::unique_ptr<storm::adapters::Smt2ExpressionAdapter> expressionAdapter;

            // A flag storing whether the last call to a check method provided aussumptions.
            bool lastCheckAssumptions;

            // The last result that was returned by any of the check methods.
            CheckResult lastResult;
            
            // A flag that states whether we want to use carl expressions.
            bool useCarlExpressions;

        };
    }
}
#endif // STORM_SOLVER_SMT2SMTSOLVER