#pragma once

#include "storm/abstraction/MenuGameAbstractor.h"

#include "storm/abstraction/prism/AbstractProgram.h"

namespace storm {
    namespace abstraction {
        namespace prism {
            
            template <storm::dd::DdType DdType, typename ValueType>
            class PrismMenuGameAbstractor : public MenuGameAbstractor<DdType, ValueType> {
            public:
                PrismMenuGameAbstractor(storm::prism::Program const& program, std::shared_ptr<storm::utility::solver::SmtSolverFactory> const& smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>());
                
                virtual storm::abstraction::MenuGame<DdType, ValueType> abstract() override;

                virtual AbstractionInformation<DdType> const& getAbstractionInformation() const override;
                virtual storm::expressions::Expression const& getGuard(uint64_t player1Choice) const override;
                virtual std::map<storm::expressions::Variable, storm::expressions::Expression> getVariableUpdates(uint64_t player1Choice, uint64_t auxiliaryChoice) const override;

                virtual void refine(std::vector<storm::expressions::Expression> const& predicates) override;

                void exportToDot(std::string const& filename, storm::dd::Bdd<DdType> const& highlightStates, storm::dd::Bdd<DdType> const& filter) const override;
                
            private:
                /// The abstract program that performs the actual abstraction.
                AbstractProgram<DdType, ValueType> abstractProgram;
            };
            
        }
    }
}
