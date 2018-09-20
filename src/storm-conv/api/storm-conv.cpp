#include "storm-conv/api/storm-conv.h"

#include "storm/storage/prism/Program.h"
#include "storm/storage/jani/Property.h"
#include "storm/storage/jani/JaniLocationExpander.h"
#include "storm/storage/jani/JSONExporter.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"

namespace storm {
    namespace api {
        
        void transformJani(storm::jani::Model& janiModel, std::vector<storm::jani::Property>& properties, storm::converter::JaniConversionOptions const& options) {
        
            if (options.substituteConstants) {
                janiModel = janiModel.substituteConstants();
            }
            
            if (!options.locationVariables.empty()) {
                for (auto const& pair : options.locationVariables) {
                    storm::jani::JaniLocationExpander expander(janiModel);
                    expander.transform(pair.first, pair.second);
                    janiModel = expander.getResult();
                }
            }

            if (options.flatten) {
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
            
            auto uneliminatedFeatures = janiModel.restrictToFeatures(options.allowedModelFeatures);
            STORM_LOG_WARN_COND(uneliminatedFeatures.empty(), "The following model features could not be eliminated: " << uneliminatedFeatures.toString());
            
            if (options.modelName) {
                janiModel.setName(options.modelName.get());
            }
        }
        
        std::pair<storm::jani::Model, std::vector<storm::jani::Property>> convertPrismToJani(storm::prism::Program const& program, std::vector<storm::jani::Property> const& properties, storm::converter::PrismToJaniConverterOptions options) {
        
            // Perform conversion
            auto res = program.toJani(properties, true, "", false);
            if (res.second.empty()) {
                std::vector<storm::jani::Property> clondedProperties;
                for (auto const& p : properties) {
                    clondedProperties.push_back(p.clone());
                }
                res.second = std::move(clondedProperties);
            }
            
            // Postprocess Jani model based on the options
            transformJani(res.first, res.second, options.janiOptions);
            
            return res;
        }
        
        void exportJaniToFile(storm::jani::Model const& model, std::vector<storm::jani::Property> const& properties, std::string const& filename, bool compact) {
            storm::jani::JsonExporter::toFile(model, properties, filename, true, compact);
        }
        
        void printJaniToStream(storm::jani::Model const& model, std::vector<storm::jani::Property> const& properties, std::ostream& ostream, bool compact) {
            storm::jani::JsonExporter::toStream(model, properties, ostream, true, compact);
        }

        
    }
}