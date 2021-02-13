#pragma once

#include <queue>
#include "storm/storage/jani/Model.h"
#include "storm/storage/jani/Property.h"
#include "boost/variant.hpp"

namespace storm {
    namespace jani {
        class JaniLocalEliminator{
        private:
            class Session {
            public:
                explicit Session(Model model, Property property);
                Model &getModel();
                void setModel(const Model &model);
                Property &getProperty();
                bool getFinished();
                void setFinished(bool finished);

                expressions::Expression getNewGuard(const Edge& edge, const EdgeDestination& dest, const Edge& outgoing);
                expressions::Expression getProbability(const EdgeDestination& first, const EdgeDestination& then);
                OrderedAssignments executeInSequence(const EdgeDestination& first, const EdgeDestination& then);
                bool isEliminable(const std::string &automatonName, std::string const& locationName);
                bool hasLoops(const std::string &automatonName, std::string const& locationName);
                bool isPossiblyInitial(const std::string &automatonName, std::string const &locationName);
                bool isPartOfProp(const std::string &automatonName, std::string const &locationName);
            private:
                Model model;
                Property property;
                bool finished;
            };

        public:
            class Action {
            public:
                virtual std::string getDescription() = 0;
                virtual void doAction(Session &session) = 0;
            };

            class UnfoldAction : public Action {
            public:
                explicit UnfoldAction(const std::string &automatonName, const std::string &variableName);

                std::string getDescription() override;
                void doAction(Session &session) override;

                std::string automatonName;
                std::string variableName;
            };

            class EliminateAction : public Action {
            public:
                explicit EliminateAction(const std::string &automatonName, const std::string &locationName);

                std::string getDescription() override;
                void doAction(Session &session) override;
            private:
                void eliminateDestination(JaniLocalEliminator::Session &session, Automaton &automaton, Edge &edge, uint64_t destIndex, detail::Edges &outgoing);

                std::string automatonName;
                std::string locationName;
            };

            class EliminateAutomaticallyAction : public Action {
            public:
                enum EliminationOrder {
                    Arbitrary,
                    NewTransitionCount
                };

                explicit EliminateAutomaticallyAction(const std::string &automatonName, EliminationOrder eliminationOrder, uint32_t transitionCountThreshold = 1000);
                std::string getDescription() override;
                void doAction(Session &session) override;
            private:
                std::string automatonName;
                EliminationOrder eliminationOrder;
                std::string find_next_location(Session &session);
                uint32_t transitionCountThreshold;
            };

            class FinishAction : public Action {
            public:
                explicit FinishAction();
                std::string getDescription() override;
                void doAction(Session &session) override;
            };

            class RebuildWithoutUnreachableAction : public Action {
            public:
                explicit RebuildWithoutUnreachableAction();
                std::string getDescription() override;
                void doAction(Session &session) override;
            };

            class AutomaticAction : public Action {
            public:
                explicit AutomaticAction();
                std::string getDescription() override;
                void doAction(Session &session) override;
            };

            class EliminationScheduler {
            public:
                EliminationScheduler();
                std::unique_ptr<Action> getNextAction();
                void addAction(std::unique_ptr<Action> action);
            private:
                std::queue<std::unique_ptr<Action>> actionQueue;
            };

            EliminationScheduler scheduler;
            explicit JaniLocalEliminator(Model const& original, std::vector<storm::jani::Property>& properties);
            void eliminate();
            Model const& getResult();

        private:
            Model const& original;
            Model newModel;
            Property property;

            void cleanUpAutomaton(std::string const &automatonName);
        };
    }
}