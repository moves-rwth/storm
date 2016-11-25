#include "storm/abstraction/MenuGameRefiner.h"

#include "storm/abstraction/MenuGameAbstractor.h"

namespace storm {
    namespace abstraction {
        
        template<storm::dd::DdType Type, typename ValueType>
        MenuGameRefiner<Type, ValueType>::MenuGameRefiner(MenuGameAbstractor<Type, ValueType>& abstractor) : abstractor(abstractor) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        void MenuGameRefiner<Type, ValueType>::refine(std::vector<storm::expressions::Expression> const& predicates) const {
            abstractor.get().refine(predicates);
        }
        
        template class MenuGameRefiner<storm::dd::DdType::CUDD, double>;
        template class MenuGameRefiner<storm::dd::DdType::Sylvan, double>;
        
    }
}
