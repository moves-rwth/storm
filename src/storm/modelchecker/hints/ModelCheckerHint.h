#ifndef STORM_MODELCHECKER_HINTS_MODELCHECKERHINT_H
#define STORM_MODELCHECKER_HINTS_MODELCHECKERHINT_H

namespace storm {
    namespace modelchecker {
        
        template<typename ValueType>
        class ExplicitModelCheckerHint;
        
        
        /*
         * This class contains information that might accelerate the solving process
         */
        class ModelCheckerHint {
        public:
            
            ModelCheckerHint() = default;
            
            // Returns true iff this hint does not contain any information
            virtual bool isEmpty() const;
            
            // Returns true iff this is an explicit model checker hint
            virtual bool isExplicitModelCheckerHint() const;
            
            template<typename ValueType>
            ExplicitModelCheckerHint<ValueType>& asExplicitModelCheckerHint();
            
            template<typename ValueType>
            ExplicitModelCheckerHint<ValueType> const& asExplicitModelCheckerHint() const;
            
        };
        
    }
}

#endif /* STORM_MODELCHECKER_HINTS_MODELCHECKERHINT_H */
