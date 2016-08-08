#pragma once

#include "src/abstraction/MenuGameAbstractor.h"

#include "src/abstraction/prism/AbstractProgram.h"

namespace storm {
    namespace abstraction {
        namespace prism {
            
            template <storm::dd::DdType DdType, typename ValueType>
            class PrismMenuGameAbstractor : public MenuGameAbstractor<DdType, ValueType> {
            public:
                PrismMenuGameAbstractor(storm::expressions::ExpressionManager& expressionManager, storm::prism::Program const& program, std::vector<storm::expressions::Expression> const& initialPredicates, std::unique_ptr<storm::utility::solver::SmtSolverFactory>&& smtSolverFactory = std::make_unique<storm::utility::solver::MathsatSmtSolverFactory>());
                
                virtual storm::abstraction::MenuGame<DdType, ValueType> abstract() override;
                virtual void refine(std::vector<storm::expressions::Expression> const& predicates) override;

            private:
                /// The abstract program that performs the actual abstraction.
                AbstractProgram<DdType, ValueType> abstractProgram;
            };
            
        }
    }
}