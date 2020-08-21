#define newimplementation
    // to avoid multiple inclusions of header file
#ifndef STORM_GENERATOR_FIGARONEXTSTATEGENERATOR_H_
#define STORM_GENERATOR_FIGARONEXTSTATEGENERATOR_H_

#include "storm/generator/NextStateGenerator.h"
#include "storm/storage/BoostTypes.h"
#include "storm-figaro/model/FigaroModel.h"
#include <set>
//#include <iostream>
//#include <array>
//#include <sstream>
//#include <algorithm>
//#include "FigaroModel.h"
//#include <stdlib.h>
//
//#include <unordered_set>
//#include <cmath>



namespace storm {
    namespace bdmp{
        namespace generator{
            
            template<typename ValueType, typename StateType>
            class FigaroNextStateGenerator: public storm::generator::NextStateGenerator<ValueType, StateType>
            {
            public:
            typedef typename storm::generator::NextStateGenerator<ValueType, StateType>::StateToIdCallback StateToIdCallback;
//            Without expression Manger
            FigaroNextStateGenerator(storm::bdmp::FigaroProgram & model, storm::generator::NextStateGeneratorOptions const& options = storm::generator::NextStateGeneratorOptions());

            //with expression manager and variable information
            FigaroNextStateGenerator(storm::bdmp::FigaroProgram & model, storm::expressions::ExpressionManager const& manger, storm::generator::VariableInformation const& varinfo, storm::generator::NextStateGeneratorOptions const& options = storm::generator::NextStateGeneratorOptions());
            
            
//            following three functions require implementation
            virtual std::vector<StateType> getInitialStates(StateToIdCallback const& stateToIdCallback) override;
            void printmenue(std::vector<storm::generator::CompressedState> menuestates, std::vector<std::tuple<int, double,std::string,int>> menue,storm::generator::CompressedState compstate);
            void printinstmenue(std::vector<storm::generator::CompressedState> menuestates, std::vector<std::pair<std::string,double>> menue,storm::generator::CompressedState compstate);
#ifndef newimplementation
            uint_fast64_t getTotalBitOffset(uint_fast64_t result) const;
            virtual uint64_t getStateSize() const override;
#endif
            virtual void load(storm::generator::CompressedState const& state) override;
            virtual storm::generator::StateBehavior<ValueType, StateType>
            expand(StateToIdCallback const& stateToIdCallback) override;
            void retreiveFigaroModelState(storm::generator::CompressedState &state);
            
//            ~FigaroNextStateGenerator();
            void printhello();
            storm::generator::CompressedState maskstate(storm::generator::CompressedState &figarostate);
            virtual storm::generator::ModelType getModelType() const override;
            virtual bool isDeterministicModel() const override;
            virtual bool isDiscreteTimeModel() const override;
            virtual bool isPartiallyObservable() const override;
           void unpackStateIntoFigaroModel(storm::generator::CompressedState const& state);
            virtual std::size_t getNumberOfRewardModels() const override;
            virtual storm::builder::RewardModelInformation getRewardModelInformation(uint64_t const& index) const override;
            virtual storm::models::sparse::StateLabeling label(storm::storage::sparse::StateStorage<StateType> const& stateStorage, std::vector<StateType> const& initialStateIndices = {}, std::vector<StateType> const& deadlockStateIndices = {}) override;
            
            virtual std::shared_ptr<storm::storage::sparse::ChoiceOrigins> generateChoiceOrigins(std::vector<boost::any>& dataForChoiceOrigins) const override;
            std::string toBinary(int, int);
            std::vector<std::pair<std::string, double>> create_inst_menue(std::vector<std::tuple<int, double, std::string, int>> fireableOccurrences);
            private:
            /*!
             * Evaluate observation labels
             */
            storm::storage::BitVector evaluateObservationLabels(storm::generator::CompressedState const& state) const override;
            
            std::vector<std::reference_wrapper<storm::prism::RewardModel const>> rewardModels;
            
                // A flag that stores whether at least one of the selected reward models has state-action rewards.
            bool hasStateActionRewards;
            public:
            storm::bdmp::FigaroProgram &figaromodel;
            };
            
        }
    }
}

#endif /* STORM_GENERATOR_FIGAROMNEXTSTATEGENERATOR_H_ */
