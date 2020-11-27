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
            void eliminate_all();
        };
    }
}