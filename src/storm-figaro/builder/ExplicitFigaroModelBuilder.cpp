#include "ExplicitFigaroModelBuilder.h"

#include <map>

#include <storm/exceptions/IllegalArgumentException.h>
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/Ctmc.h"
#include "storm/utility/bitoperations.h"
#include "storm/utility/constants.h"
#include "storm/utility/ProgressMeasurement.h"
#include "storm/utility/SignalHandler.h"
#include "storm/utility/vector.h"
#include "storm/settings/SettingsManager.h"
#include "storm/transformer/NonMarkovianChainTransformer.h"

#include "storm-figaro/settings/modules/FIGAROIOSettings.h"


namespace storm {
    namespace figaro{
        namespace builder {
            /*!
             * Constructor.
             *
             * @param figaro Figaro.
             * @param symmetries Symmetries in the figaro model.
             */
            template<typename ValueType, typename StateType>
            storm::figaro::builder::ExplicitFIGAROModelBuilder<ValueType, StateType>::ExplicitFIGAROModelBuilder(storm::figaro::FigaroProgram const& fogaromodel, storm::storage::DFTIndependentSymmetries const& symmetries){std::cout<<"hello world";}
            
} //builder
} //figaro
} ///storm
        
        
