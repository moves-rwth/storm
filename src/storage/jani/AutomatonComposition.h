#pragma once

#include "src/storage/jani/Composition.h"

namespace storm {
    namespace jani {
        
        class AutomatonComposition : public Composition {
        public:
            /*!
             * Creates a reference to an automaton to be used in a composition.
             */
            AutomatonComposition(std::string const& name);
            
            /*!
             * Retrieves the name of the automaton this composition element refers to.
             */
            std::string const& getAutomatonName() const;
            
            virtual boost::any accept(CompositionVisitor& visitor, boost::any const& data) const override;
            
            virtual void write(std::ostream& stream) const override;
            
            bool isAutomaton() const override  { return true; }
            
        private:
            // The name of the automaton this composition element refers to.
            std::string name;
        };
        
    }
}