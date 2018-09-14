#pragma once


#include <vector>


namespace storm {
    namespace jani {
        class Model;
        class Property;
        
        /*!
         * Eliminates all function references in the given model and the given properties by replacing them with their corresponding definitions.
         */
        void eliminateFunctions(Model& model, std::vector<Property>& properties);

    }
}

