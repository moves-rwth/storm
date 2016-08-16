#pragma once

#include "src/abstraction/MenuGameAbstractor.h"

#include "src/abstraction/prism/AbstractProgram.h"

namespace storm {
    namespace abstraction {
        namespace prism {
            
            template <storm::dd::DdType DdType, typename ValueType>
            class PrismMenuGameAbstractor : public MenuGameAbstractor<DdType, ValueType> {
            public:
                PrismMenuGameAbstractor(storm::prism::Program const& program, std::vector<storm::expressions::Expression> const& initialPredicates, std::shared_ptr<storm::utility::solver::SmtSolverFactory> const& smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>());
                
                virtual storm::abstraction::MenuGame<DdType, ValueType> abstract() override;
                virtual void refine(std::vector<storm::expressions::Expression> const& predicates) override;

                void exportToDot(std::string const& filename) const;
                
            private:
                /// The abstract program that performs the actual abstraction.
                AbstractProgram<DdType, ValueType> abstractProgram;
            };
            
        }
    }
}