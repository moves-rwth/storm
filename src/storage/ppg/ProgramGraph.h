#pragma once

#include "defines.h"
#include "src/storage/expressions/Expression.h"
#include "src/storage/expressions/Variable.h"
#include "src/storage/expressions/ExpressionManager.h"

#include "ProgramLocation.h"
#include "ProgramEdge.h"
#include "ProgramEdgeGroup.h"

namespace storm {
    namespace ppg {
        struct ProgramAction {
            ProgramAction(ProgramGraph* graph, ProgramActionIdentifier actId) : graph(graph), actId(actId) {
                
            }
            
            ProgramGraph* graph;
            ProgramActionIdentifier actId;
        };
        
         
        /**
         *  Program graph as based on Principles of Model Checking, Def 2.13
         *  Action effects are part of the action.
         */
        class ProgramGraph {
        public:
            using EdgeGroupIterator = ProgramLocation::EdgeGroupIterator;
            using ConstLocationIterator = std::unordered_map<ProgramLocationIdentifier, ProgramLocation>::const_iterator;
            
            ProgramGraph(std::shared_ptr<storm::expressions::ExpressionManager> const& expManager, std::vector<storm::expressions::Variable> const& variables) : expManager(expManager), variables(variables) {
                
            }
            
            virtual ~ProgramGraph() {
                std::cout << "remove graph" << std::endl;
            }
            
            ProgramActionIdentifier addAction() {
                ProgramActionIdentifier newId = freeActionIndex();
                assert(!hasAction(newId));
                actions.emplace(newId, ProgramAction(this, newId));
                return newId;
            }
            
            ProgramLocation* addLocation(bool isInitial = false) {
                ProgramLocationIdentifier newId = freeLocationIndex();
                assert(!hasLocation(newId));
                return &(locations.emplace(newId, ProgramLocation(this, newId, isInitial)).first->second);
            }
            
            
            ProgramEdgeGroup* addProgramEdgeGroup(ProgramLocation& source, storm::expressions::Expression const& probability) {
                ProgramEdgeGroupIdentifier newId = freeEdgeGroupIndex();
                return source.emplaceEdgeGroup(newId, probability);
            }
            
            ProgramEdgeGroup* addProgramEdgeGroup(ProgramLocationIdentifier sourceId, storm::expressions::Expression const& probability) {
                assert(hasLocation(sourceId));
                return addProgramEdgeGroup(getLocation(sourceId), probability);
            }
        
            ProgramEdge* addProgramEdge(ProgramEdgeGroup& group, ProgramActionIdentifier action, ProgramLocationIdentifier targetId) {
                return addProgramEdge(group, action, expManager->boolean(true), targetId);
            }
            
            ProgramEdge* addProgramEdge(ProgramEdgeGroup& group, ProgramActionIdentifier action, storm::expressions::Expression const& condition, ProgramLocationIdentifier targetId) {
                ProgramEdgeIdentifier newId = freeEdgeIndex();
                return group.emplaceEdge(newId, action, condition, targetId);
            }
            
            std::vector<ProgramEdge*> addProgramEdgeToAllGroups(ProgramLocation& source, ProgramActionIdentifier action, ProgramLocationIdentifier targetId) {
                return addProgramEdgeToAllGroups(source, action, expManager->boolean(true), targetId);
            }
            
            std::vector<ProgramEdge*> addProgramEdgeToAllGroups(ProgramLocation& source, ProgramActionIdentifier action, storm::expressions::Expression const& condition, ProgramLocationIdentifier targetId) {
                assert(hasLocation(targetId));
                assert(hasAction(action));
                
                if(source.nrOutgoingEdgeGroups() == 0) {
                    addProgramEdgeGroup(source, expManager->rational(1));
                }
                
                std::vector<ProgramEdge*> res;
                for(EdgeGroupIterator eg = source.getOutgoingEdgeGroupBegin(); eg != source.getOutgoingEdgeGroupEnd(); ++eg) {
                    ProgramEdgeIdentifier newId = freeEdgeIndex();
                    res.push_back((*eg)->emplaceEdge(newId, action, condition, targetId));
                    
                }
                
                return res;
                
            }
            
            
            std::vector<ProgramEdge*> addProgramEdgeToAllGroups(ProgramLocationIdentifier sourceId, ProgramActionIdentifier action, storm::expressions::Expression const& condition, ProgramLocationIdentifier targetId) {
                assert(hasLocation(sourceId));
                return addProgramEdgeToAllGroups(getLocation(sourceId), action, condition, targetId);
            }

            
            bool hasLocation(ProgramLocationIdentifier id) const {
                return locations.count(id) == 1;
            }
            
            bool hasAction(ProgramActionIdentifier id) const {
                return actions.count(id) == 1;
            }
            
            size_t nrLocations() const {
                return locations.size();
            }
            
            size_t nrVariables() const {
                return variables.size();
            }
            
            
            ConstLocationIterator locationBegin() const {
                return locations.begin();
            }
            
            ConstLocationIterator locationEnd() const {
                return locations.end();
            }
            
            std::vector<storm::expressions::Variable> const& getVariables() const {
                return variables;
            }
            
            std::shared_ptr<storm::expressions::ExpressionManager> const& getExpressionManager() const {
                return expManager;
            }
            
            void checkValid() {
                
            }
            
            void printInfo(std::ostream& os) const {
                os << "Number of variables: " << nrVariables() << std::endl;
                os << "Number of locations: " << nrLocations() << std::endl;
            }
            
        protected:
            
            ProgramLocation& getLocation(ProgramLocationIdentifier id) {
                return locations.at(id);
            }
            
            /**
             * Gets a free location index (based on whatever scheme we are using).
             */
            ProgramLocationIdentifier freeLocationIndex() {
                return newLocationId++;
            }
            
            ProgramActionIdentifier freeActionIndex() {
                return newActionId++;
            }
            
            ProgramEdgeIdentifier freeEdgeIndex() {
                return newEdgeId++;
            }
            
            ProgramEdgeGroupIdentifier freeEdgeGroupIndex() {
                return newEdgeGroupId++;
            }
            
            std::unordered_map<ProgramActionIdentifier, ProgramAction> actions;
            std::unordered_map<ProgramLocationIdentifier, ProgramLocation> locations;
            storm::expressions::Expression initialValueRestriction;
            std::vector<storm::expressions::Variable> variables;
            
            std::shared_ptr<storm::expressions::ExpressionManager> expManager;
        private:
            // Helper for IDs, may be changed later.
            ProgramEdgeGroupIdentifier newEdgeGroupId = 0;
            ProgramLocationIdentifier newLocationId = 0;
            ProgramEdgeIdentifier newEdgeId = 0;
            ProgramActionIdentifier newActionId = 0;
            
        };
    }
}