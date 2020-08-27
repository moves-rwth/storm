
#include <stdlib.h>
#include <string>
#include "storm-figaro/model/FigaroModel.h"
#include "storm-figaro/model/FigaroModelTemplate.h"
#include "storm/models/symbolic/MarkovAutomaton.h"
#include "storm-figaro/api/storm-figaro.h"
//#include "storm-cli-utilities/cli.h"
//#include "storm-cli-utilities/model-handling.h"

#include "storm-figaro/generator/FigaroNextStateGenerator.h"

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
    
#include "storm/api/transformation.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/generator/VariableInformation.h"
#include "storm-dft/builder/DftExplorationHeuristic.h"


namespace storm {
    namespace figaro {
        namespace api{


            std::shared_ptr<storm::figaro::FigaroProgram> loadFigaroProgram(){
                std::shared_ptr<storm::figaro::FigaroProgram> figaromodel=  std::make_shared<storm::figaro::FigaroProgram1> (storm::figaro::FigaroProgram1());
                return figaromodel;
            }






    }//namespace figaro
}//namespace storm

}
