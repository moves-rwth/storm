#pragma once

#include "storm/storage/jani/Model.h"
#include "storm/storage/jani/Automaton.h"


namespace storm {
    namespace jani {
        class JaniLocationExpander {
        public:
            JaniLocationExpander(Model const& original);
            void transform(std::string const& automatonName, std::string const& variableName);
            Model const& getResult() const;

        private:
            Model const& original;
            Model newModel;

            Automaton transformAutomaton(Automaton const& automaton, std::string const& variableName);



        };
    }

}
