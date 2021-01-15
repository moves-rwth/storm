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
                explicit Session(Model model);
                Model &getModel();
                void setModel(const Model &model);
                bool getFinished();
                void setFinished(bool finished);

                expressions::Expression getNewGuard(const Edge& edge, const EdgeDestination& dest, const Edge& outgoing);
                expressions::Expression getProbability(const EdgeDestination& first, const EdgeDestination& then);
                OrderedAssignments executeInSequence(const EdgeDestination& first, const EdgeDestination& then);
                bool hasLoops(const std::string &automatonName, std::string const& locationName);
            private:
                Model model;
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
                explicit UnfoldAction(const std::string &variableName);

                std::string getDescription() override;
                void doAction(Session &session) override;

                std::string variableName;
            };

            class EliminateAction : public Action {
            public:
                explicit EliminateAction(const std::string &locationName);

                std::string getDescription() override;
                void doAction(Session &session) override;
            private:
                void eliminateDestination(JaniLocalEliminator::Session &session, Automaton &automaton, Edge &edge, uint64_t destIndex, detail::Edges &outgoing);

                std::string locationName;
            };

            class FinishAction : public Action {
            public:
                explicit FinishAction();
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