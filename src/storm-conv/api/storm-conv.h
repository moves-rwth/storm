#pragma once

#include "storm-conv/converter/options/PrismToJaniConverterOptions.h"
#include "storm-conv/converter/options/JaniConversionOptions.h"

#include "storm/storage/prism/Program.h"
#include "storm/storage/jani/Property.h"
#include "storm/storage/jani/JaniLocationExpander.h"
#include "storm/storage/jani/JSONExporter.h"

namespace storm {
    namespace api {
        
        void postprocessJani(storm::jani::Model& janiModel, storm::converter::JaniConversionOptions options) {
        
            if (!options.locationVariables.empty()) {
                for (auto const& pair : options.locationVariables) {
                    storm::jani::JaniLocationExpander expander(janiModel);
                    expander.transform(pair.first, pair.second);
                    janiModel = expander.getResult();
                }
            }

            if (options.exportFlattened) {
                janiModel = janiModel.flattenComposition();
            }

            if (options.standardCompliant) {
                janiModel.makeStandardJaniCompliant();
            }
        }
        
        std::pair<storm::jani::Model, std::vector<storm::jani::Property>> convertPrismToJani(storm::prism::Program const& program, std::vector<storm::jani::Property> const& properties = std::vector<storm::jani::Property>(), storm::converter::PrismToJaniConverterOptions options = storm::converter::PrismToJaniConverterOptions()) {
            std::pair<storm::jani::Model, std::vector<storm::jani::Property>> res;
        
            // Perform conversion
            auto modelAndRenaming = program.toJaniWithLabelRenaming(options.allVariablesGlobal, options.suffix);
            res.first = std::move(modelAndRenaming.first);
        
            // Amend properties to potentially changed labels
            for (auto const& property : properties) {
                res.second.emplace_back(property.substituteLabels(modelAndRenaming.second));
            }
            
            // Postprocess Jani model based on the options
            postprocessJani(res.first, options.janiOptions);
            
            return res;
        }
        
        void exportJaniToFile(storm::jani::Model const& model, std::vector<storm::jani::Property> const& properties, std::string const& filename) {
            storm::jani::JsonExporter::toFile(model, properties, filename);
        }
        
        void printJaniToStream(storm::jani::Model const& model, std::vector<storm::jani::Property> const& properties, std::ostream& ostream) {
            storm::jani::JsonExporter::toStream(model, properties, ostream);
        }

        
    }
}