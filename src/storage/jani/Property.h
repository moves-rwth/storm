#pragma once

#include "src/logic/formulas.h"

namespace storm {
    namespace jani {
        class Property {
            /**
             * Constructs the property
             * @param name the name
             * @param formula the formula representation
             * @param comment An optional comment
             */
            Property(std::string const& name, std::shared_ptr<storm::logic::Formula const> const& formula, std::string const& comment = "");
            /**
             * Get the provided name
             * @return the name
             */
            std::string const& getName() const;
            /**
             * Get the provided comment, if any
             * @return the comment
             */
            std::string const& getComment() const;
            /**
             * Get the formula
             * @return the formula
             */
            std::shared_ptr<storm::logic::Formula const> const& getFormula() const;

        private:
            std::string name;
            std::shared_ptr<storm::logic::Formula const> formula;
            std::string comment;
        };
    }
}

