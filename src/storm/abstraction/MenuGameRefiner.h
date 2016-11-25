#pragma once

#include <functional>
#include <vector>

#include "storm/storage/expressions/Expression.h"

#include "storm/storage/dd/DdType.h"

namespace storm {
    namespace abstraction {

        template <storm::dd::DdType DdType, typename ValueType>
        class MenuGameAbstractor;
        
        template<storm::dd::DdType Type, typename ValueType>
        class MenuGameRefiner {
        public:
            /*!
             * Creates a refiner for the provided abstractor.
             */
            MenuGameRefiner(MenuGameAbstractor<Type, ValueType>& abstractor);
            
            /*!
             * Refines the abstractor with the given set of predicates.
             */
            void refine(std::vector<storm::expressions::Expression> const& predicates) const;
            
            
            
        private:
            /// The underlying abstractor to refine.
            std::reference_wrapper<MenuGameAbstractor<Type, ValueType>> abstractor;
        };
        
    }
}
