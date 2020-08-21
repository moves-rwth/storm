#define newimplementation
#include <stdlib.h>

#include <iostream>
#include <string>
#include "FigaroModel.h"
#include "storm/models/symbolic/MarkovAutomaton.h"
//#include "storm-cli-utilities/cli.h"
//#include "storm-cli-utilities/model-handling.h"

#include "FigaroNextStateGenerator.h"
#include "FigaroStateGenerationInfo.h"
#include <random>
#include "storm/storage/SymbolicModelDescription.h"

#include "storm/storage/sparse/ModelComponents.h"
#include "storm/models/sparse/Ctmc.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Ctmc.h"

#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/builder/BuilderType.h"
#include "storm/builder/ExplicitModelBuilder.h"


#include "storm/utility/macros.h"
#include "storm/exceptions/NotSupportedException.h"

#include "storm-parsers/parser/AutoParser.h"
#include "storm-parsers/parser/DirectEncodingParser.h"
#include "storm-parsers/parser/ImcaMarkovAutomatonParser.h"

#include "storm/storage/SymbolicModelDescription.h"
#include "storm/storage/jani/ModelFeatures.h"

#include "storm/storage/sparse/ModelComponents.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Ctmc.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/Pomdp.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/StochasticTwoPlayerGame.h"
#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/builder/DdPrismModelBuilder.h"
#include "storm/builder/DdJaniModelBuilder.h"
#include "storm/builder/BuilderType.h"

#include "storm/generator/PrismNextStateGenerator.h"
#include "storm/generator/JaniNextStateGenerator.h"

#include "storm/builder/ExplicitModelBuilder.h"
#include "storm/builder/jit/ExplicitJitJaniModelBuilder.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotSupportedException.h"

#include "storm/api/export.h"
#include "storm/api/properties.h"
#include "storm/api/verification.h"
#include "storm-parsers/api/properties.h"
#include "storm-parsers/api/model_descriptions.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/modelchecker/csl/SparseCtmcCslModelChecker.h"
#include "storm/modelchecker/csl/helper/SparseCtmcCslHelper.h"
    #include "storm/api/export.h"
#include "storm/api/transformation.h"

#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/generator/VariableInformation.h"


namespace storm {
    namespace bdmp {
        
        template<typename ValueType>
        std::shared_ptr<storm::models::sparse::Model<ValueType>>
        buildSparseModel( storm::bdmp::FigaroProgram & model
            //, maybe at some later development stage we provide build options through commnad line
            //storm::builder::BuilderOptions const& options
        ) {
            // For expression Manager
#ifdef newimplementation
            std::vector<storm::expressions::Variable> boolean_variables;
            std::vector<storm::expressions::Variable> integer_variables;
//            std::vector<storm::expressions::Variable> integer_variables;

            auto manager = std::make_shared<storm::expressions::ExpressionManager>();
            
            for (auto & bool_var : model.mFigaroboolelementindex) {
                boolean_variables.emplace_back(manager->declareBooleanVariable(bool_var.first));
                }
            for (auto & float_var : model.mFigarofloatelementindex) {
                
                integer_variables.emplace_back(manager->declareIntegerVariable(float_var.first));
            }
            
            for (auto & int_var : model.mFigarointelementindex) {
                integer_variables.emplace_back(manager->declareIntegerVariable(int_var.first));
            }
            for (auto & enum_var : model.mFigaroenumelementindex) {
                integer_variables.emplace_back(manager->declareIntegerVariable(enum_var.first));
            }
            
            auto variable_info = std::make_shared<storm::generator::VariableInformation>(model, boolean_variables,integer_variables);
            
            
            std::shared_ptr<storm::bdmp::generator::FigaroNextStateGenerator<ValueType, uint32_t>> generator_new = std::make_shared<storm::bdmp::generator::FigaroNextStateGenerator<ValueType, uint32_t>>(model, *manager, *variable_info);
            
             storm::builder::ExplicitModelBuilder<ValueType> builder_new(generator_new);
            
              std::shared_ptr<storm::models::sparse::Model<ValueType>> hello = builder_new.build();
            
//            hello->printModelInformationToStream(std::cout);
            return hello;
#else
            
            
            
                // previous implementation
            std::shared_ptr<storm::bdmp::generator::FigaroNextStateGenerator<ValueType, uint32_t>> generator = std::make_shared<storm::bdmp::generator::FigaroNextStateGenerator<ValueType, uint32_t>>(model);
            storm::builder::ExplicitModelBuilder<ValueType> builder(generator);
            return builder.build();
#endif
        }


        template<typename ValueType>
        void checkfigaro() //main function for figaro api
        {

        FigaroProgram model = FigaroProgram();

        std::shared_ptr<storm::models::sparse::Model<ValueType>> sparsemodel = buildSparseModel<ValueType>(model);
       
            //change MA to CTMC
        sparsemodel->printModelInformationToStream(std::cout);
//          std::cout << sparsemodel->getTransitionMatrix() << std::endl;
//        auto ma = std::static_pointer_cast<storm::models::sparse::MarkovAutomaton<ValueType>>(sparsemodel);
        storm::api::exportSparseModelAsDot(sparsemodel,"hello.dot");
        storm::api::exportSparseModelAsDrn(sparsemodel,"hello.drn");
//
        if(model.figaromodelhasinstransitions()) {
         sparsemodel = storm::transformer::NonMarkovianChainTransformer<ValueType>::eliminateNonmarkovianStates(sparsemodel->template as<storm::models::sparse::MarkovAutomaton<ValueType>>(), storm::transformer::EliminationLabelBehavior::MergeLabels);
        }
//        auto sparsemodel_new = storm::transformer::NonMarkovianChainTransformer<ValueType>::eliminateNonmarkovianStates(ma, storm::transformer::EliminationLabelBehavior::DeleteLabels);
        //model check this sparse CTMC in next step
//        exit(3);
        sparsemodel->printModelInformationToStream(std::cout);
//        exit(2);
//        storm::api::exportSparseModelAsDot(sparsemodel,"hello.dot");
//        std::string property_string = "Pmax=? [F[20,20] \"failed\"]";
//        std::string property_string = "Pmax=? [F<=100 \"failed\"]";
        std::string property_string = "Pmax=? [F<=10000 \"failed\"]";
//        std::string property_string = "Pmax=? [F<=10000 \"failed\"]";
//        std::string property_string = "Pmax=? [F [10000, 10000] \"failed\"]";
        auto properties = storm::api::parseProperties(property_string);
        auto formulae = storm::api::extractFormulasFromProperties(properties);
        auto ctmc = sparsemodel->template as<storm::models::sparse::Ctmc<ValueType>>();
        auto checker = std::make_shared<storm::modelchecker::SparseCtmcCslModelChecker<storm::models::sparse::Ctmc<ValueType>>>(*ctmc);
        auto result =
        checker->check(storm::modelchecker::CheckTask<>(*(formulae[0]), false));
        assert(result->isExplicitQuantitativeCheckResult());
        auto quantRes = result->template asExplicitQuantitativeCheckResult<double>();
        std::cout << quantRes[(*sparsemodel->getInitialStates().begin())] << std::endl;
        result->writeToStream(std::cout);
        }
    }//namespace bdmp
}//namespace storm

