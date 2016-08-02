#pragma once

#include <set>
#include <memory>

#include "src/storage/jani/Composition.h"

namespace storm {
    namespace jani {
        
        class ParallelComposition : public Composition {
        public:
            /*!
             * Creates a parallel composition of the two subcompositions.
             */
            ParallelComposition(std::shared_ptr<Composition> const& leftSubcomposition, std::shared_ptr<Composition> const& rightSubcomposition, std::set<std::string> const& alphabet = {});
            
            /*!
             * Retrieves the left subcomposition.
             */
            Composition const& getLeftSubcomposition() const;

            /*!
             * Retrieves the right subcomposition.
             */
            Composition const& getRightSubcomposition() const;

            /*!
             * Retrieves the alphabet of actions over which to synchronize the automata.
             */
            std::set<std::string> const& getSynchronizationAlphabet() const;
            
            virtual boost::any accept(CompositionVisitor& visitor, boost::any const& data) const override;
            
            virtual void write(std::ostream& stream) const override;
            
        private:
            // The left subcomposition.
            std::shared_ptr<Composition> leftSubcomposition;
            
            // The right subcomposition.
            std::shared_ptr<Composition> rightSubcomposition;
            
            // The alphabet of actions over which to synchronize.
            std::set<std::string> alphabet;
        };
        
    }
}