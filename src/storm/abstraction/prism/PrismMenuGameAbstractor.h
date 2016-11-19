#pragma once

#include "storm/abstraction/MenuGameAbstractor.h"

#include "storm/abstraction/prism/AbstractProgram.h"

namespace storm {
    namespace abstraction {
        namespace prism {
            
            template <storm::dd::DdType DdType, typename ValueType>
            class PrismMenuGameAbstractor : public MenuGameAbstractor<DdType, ValueType> {
            public:
                PrismMenuGameAbstractor(storm::prism::Program const& program, std::vector<storm::expressions::Expression> const& initialPredicates, std::shared_ptr<storm::utility::solver::SmtSolverFactory> const& smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>());
                
                virtual storm::abstraction::MenuGame<DdType, ValueType> abstract() override;
                virtual void refine(std::vector<storm::expressions::Expression> const& predicates) override;
                virtual void refine(storm::dd::Bdd<DdType> const& pivotState, storm::dd::Bdd<DdType> const& player1Choice, storm::dd::Bdd<DdType> const& lowerChoice, storm::dd::Bdd<DdType> const& upperChoice) override;

                void exportToDot(std::string const& filename, storm::dd::Bdd<DdType> const& highlightStates, storm::dd::Bdd<DdType> const& filter) const;
                
            private:
                /// The abstract program that performs the actual abstraction.
                AbstractProgram<DdType, ValueType> abstractProgram;
            };
            
        }
    }
}
