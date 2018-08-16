#include "storm-conv/api/storm-conv.h"

#include "storm/storage/prism/Program.h"
#include "storm/storage/jani/Property.h"
#include "storm/storage/jani/JaniLocationExpander.h"
#include "storm/storage/jani/JSONExporter.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"

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
                std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory;
                if (storm::settings::hasModule<storm::settings::modules::CoreSettings>()) {
                    smtSolverFactory = std::make_shared<storm::utility::solver::SmtSolverFactory>();
                } else {
                    smtSolverFactory = std::make_shared<storm::utility::solver::Z3SmtSolverFactory>();
                }
                janiModel = janiModel.flattenComposition(smtSolverFactory);
            }

            if (options.standardCompliant) {
                janiModel.makeStandardJaniCompliant();
            }
            
            if (options.modelName) {
                janiModel.setName(options.modelName.get());
            }
        }
        
        std::pair<storm::jani::Model, std::vector<storm::jani::Property>> convertPrismToJani(storm::prism::Program const& program, std::vector<storm::jani::Property> const& properties, storm::converter::PrismToJaniConverterOptions options) {
            std::pair<storm::jani::Model, std::vector<storm::jani::Property>> res;
        
            // Perform conversion
            auto modelAndRenaming = program.toJaniWithLabelRenaming(options.allVariablesGlobal, options.suffix, options.janiOptions.standardCompliant);
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