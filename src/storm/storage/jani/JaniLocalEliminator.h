#pragma once

#include "storm/storage/jani/Model.h"
#include "storm/storage/jani/Property.h"

namespace storm {
    namespace jani {
        class JaniLocalEliminator{
        public:
            explicit JaniLocalEliminator(Model const& original, std::vector<storm::jani::Property>& properties);
            void eliminate();
            Model const& getResult();

        private:
            Model const& original;
            Model newModel;
            Property property;

            void unfold(std::string const& variableName);
            void eliminate(const std::string &automatonName, std::string const& locationName);
            void eliminateDestination(Automaton &automaton, Edge &edge, const uint64_t destIndex, detail::Edges &outgoing);
            void eliminate_all();

            expressions::Expression getNewGuard(const Edge& edge, const EdgeDestination& dest, const Edge& outgoing);
            expressions::Expression getProbability(const EdgeDestination& first, const EdgeDestination& then);
            OrderedAssignments executeInSequence(const EdgeDestination& first, const EdgeDestination& then);

            bool hasLoops(const std::string &automatonName, std::string const& locationName);

            void cleanUpAutomaton(std::string const &automatonName);
        };
    }
}