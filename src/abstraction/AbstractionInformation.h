#pragma once

#include <vector>
#include <set>

#include "src/storage/dd/DdType.h"

#include "src/storage/dd/Bdd.h"

namespace storm {
    namespace expressions {
        class ExpressionManager;
        class Expression;
        class Variable;
    }
    
    namespace dd {
        template <storm::dd::DdType DdType>
        class DdManager;
    }
    
    namespace abstraction {
        
        template<storm::dd::DdType DdType>
        class AbstractionInformation {
        public:
            /*!
             * Creates a new abstraction information object.
             *
             * @param expressionManager The manager responsible for all variables and expressions during the abstraction process.
             */
            AbstractionInformation(storm::expressions::ExpressionManager& expressionManager, std::shared_ptr<storm::dd::DdManager<DdType>> ddManager = std::make_shared<storm::dd::DdManager<DdType>>());

            /*!
             * Adds the given variable.
             *
             * @param variable The variable to add.
             */
            void addExpressionVariable(storm::expressions::Variable const& variable);

            /*!
             * Retrieves all known variables that may be used in predicates.
             *
             * @return All known variables.
             */
            std::set<storm::expressions::Variable> getExpressionVariables() const;
            
            /*!
             * Adds the given variable whose range is restricted.
             *
             * @param variable The variable to add.
             * @param constraint An expression characterizing the legal values of the variable.
             */
            void addExpressionVariable(storm::expressions::Variable const& variable, storm::expressions::Expression const& constraint);

            /*!
             * Adds an expression that constrains the legal variable values.
             *
             * @param constraint The constraint.
             */
            void addConstraint(storm::expressions::Expression const& constraint);
            
            /*!
             * Retrieves a list of expressions that constrain the valid variable values.
             *
             * @return The constraint expressions.
             */
            std::vector<storm::expressions::Expression> const& getConstraints() const;
            
            /*!
             * Adds the given predicate.
             *
             * @param predicate The predicate to add.
             * @return The index of the newly added predicate in the global list of predicates.
             */
            uint_fast64_t addPredicate(storm::expressions::Expression const& predicate);
            
            /*!
             * Adds the given predicates.
             *
             * @param predicates The predicates to add.
             * @return A list of indices corresponding to the new predicates.
             */
            std::vector<uint_fast64_t> addPredicates(std::vector<storm::expressions::Expression> const& predicates);
            
            /*!
             * Retrieves the expression manager.
             *
             * @return The manager.
             */
            storm::expressions::ExpressionManager& getExpressionManager();
            
            /*!
             * Retrieves the expression manager.
             *
             * @return The manager.
             */
            storm::expressions::ExpressionManager const& getExpressionManager() const;
            
            /*!
             * Retrieves the DD manager.
             *
             * @return The manager.
             */
            storm::dd::DdManager<DdType>& getDdManager();
            
            /*!
             * Retrieves the DD manager.
             *
             * @return The manager.
             */
            storm::dd::DdManager<DdType> const& getDdManager() const;
            
            /*!
             * Retrieves the shared pointer to the DD manager.
             *
             * @return The shared pointer to the DD manager.
             */
            std::shared_ptr<storm::dd::DdManager<DdType>> getDdManagerAsSharedPointer();

            /*!
             * Retrieves the shared pointer to the DD manager.
             *
             * @return The shared pointer to the DD manager.
             */
            std::shared_ptr<storm::dd::DdManager<DdType> const> getDdManagerAsSharedPointer() const;
            
            /*!
             * Retrieves all currently known predicates.
             *
             * @return The list of known predicates.
             */
            std::vector<storm::expressions::Expression> const& getPredicates() const;

            /*!
             * Retrieves the predicate with the given index.
             *
             * @param index The index of the predicate.
             */
            storm::expressions::Expression const& getPredicateByIndex(uint_fast64_t index) const;
            
            /*!
             * Retrieves the source variable associated with the given predicate. Note that the given predicate must be
             * known to this abstraction information.
             *
             * @param predicate The predicate for which to retrieve the source variable.
             */
            storm::dd::Bdd<DdType> getPredicateSourceVariable(storm::expressions::Expression const& predicate) const;
            
            /*!
             * Retrieves the number of predicates.
             *
             * @return The number of predicates.
             */
            std::size_t getNumberOfPredicates() const;
            
            /*!
             * Retrieves all currently known variables.
             *
             * @return The set of known variables.
             */
            std::set<storm::expressions::Variable> const& getVariables() const;
            
            /*!
             * Creates the given number of variables used to encode the choices of player 1/2 and auxiliary information.
             *
             * @param player1VariableCount The number of variables to use for encoding player 1 choices.
             * @param player2VariableCount The number of variables to use for encoding player 2 choices.
             * @param auxVariableCount The number of variables to use for encoding auxiliary information.
             */
            void createEncodingVariables(uint64_t player1VariableCount, uint64_t player2VariableCount, uint64_t auxVariableCount);
            
            /*!
             * Encodes the given index using the indicated player 1 variables.
             *
             * @param index The index to encode.
             * @param end The index of the variable past the end of the range that is used to encode the index.
             * @return The index encoded as a BDD.
             */
            storm::dd::Bdd<DdType> encodePlayer1Choice(uint_fast64_t index, uint_fast64_t end) const;
            
            /*!
             * Encodes the given index using the indicated player 2 variables.
             *
             * @param index The index to encode.
             * @param end The index of the variable past the end of the range that is used to encode the index.
             * @return The index encoded as a BDD.
             */
            storm::dd::Bdd<DdType> encodePlayer2Choice(uint_fast64_t index, uint_fast64_t end) const;

            /*!
             * Encodes the given index using the indicated probabilistic branching variables.
             *
             * @param index The index to encode.
             * @param start The index of the first variable of the range that is used to encode the index.
             * @param end The index of the variable past the end of the range that is used to encode the index.
             * @return The index encoded as a BDD.
             */
            storm::dd::Bdd<DdType> encodeAux(uint_fast64_t index, uint_fast64_t start, uint_fast64_t end) const;
            
            /*!
             * Retrieves the cube of player 2 variables in the given range [offset, numberOfVariables).
             *
             * @param numberOfVariables The number of variables to use in total. The number of variables in the returned
             * cube is the number of variables minus the offset.
             * @param offset The first variable of the range to return.
             * @return The cube of variables starting from the offset until the given number of variables is reached.
             */
            storm::dd::Bdd<DdType> getPlayer2ZeroCube(uint_fast64_t numberOfVariables, uint_fast64_t offset) const;
            
            /*!
             * Retrieves the meta variables associated with the player 1 choices.
             *
             * @return The meta variables associated with the player 1 choices.
             */
            std::vector<storm::expressions::Variable> const& getPlayer1Variables() const;
            
            /*!
             * Retrieves the set of player 1 variables.
             *
             * @param count The number of player 1 variables to include.
             * @return The set of player 1 variables.
             */
            std::set<storm::expressions::Variable> getPlayer1VariableSet(uint_fast64_t count) const;
            
            /*!
             * Retrieves the number of player 1 variables.
             *
             * @return The number of player 1 variables.
             */
            std::size_t getPlayer1VariableCount() const;

            /*!
             * Retrieves the meta variables associated with the player 2 choices.
             *
             * @return The meta variables associated with the player 2 choices.
             */
            std::vector<storm::expressions::Variable> const& getPlayer2Variables() const;

            /*!
             * Retrieves the number of player 2 variables.
             *
             * @return The number of player 2 variables.
             */
            std::size_t getPlayer2VariableCount() const;

            /*!
             * Retrieves the set of player 2 variables.
             *
             * @param count The number of player 2 variables to include.
             * @return The set of player 2 variables.
             */
            std::set<storm::expressions::Variable> getPlayer2VariableSet(uint_fast64_t count) const;

            /*!
             * Retrieves the meta variables associated with auxiliary information.
             *
             * @return The meta variables associated with auxiliary information.
             */
            std::vector<storm::expressions::Variable> const& getAuxVariables() const;
            
            /*!
             * Retrieves the auxiliary variable with the given index.
             *
             * @param index The index of the auxiliary variable to retrieve.
             * @return The auxiliary variable with the given index.
             */
            storm::expressions::Variable const& getAuxVariable(uint_fast64_t index) const;

            /*!
             * Retrieves the requested set of auxiliary variables.
             *
             * @param start The index of the first auxiliary variable to include.
             * @param end The index of the auxiliary variable past the end of the range to include.
             * @return The set of auxiliary variables.
             */
            std::set<storm::expressions::Variable> getAuxVariableSet(uint_fast64_t start, uint_fast64_t end) const;

            /*!
             * Retrieves the number of auxiliary variables.
             *
             * @return The number of auxiliary variables.
             */
            std::size_t getAuxVariableCount() const;
            
            /*!
             * Retrieves the set of source meta variables.
             *
             * @return All source meta variables.
             */
            std::set<storm::expressions::Variable> const& getSourceVariables() const;

            /*!
             * Retrieves the set of successor meta variables.
             *
             * @return All successor meta variables.
             */
            std::set<storm::expressions::Variable> const& getSuccessorVariables() const;

            /*!
             * Retrieves a BDD representing the identities of all predicates.
             *
             * @return All predicate identities.
             */
            storm::dd::Bdd<DdType> const& getAllPredicateIdentities() const;
            
            /*!
             * Retrieves a mapping of the known predicates to the BDDs that represent the corresponding states.
             *
             * @return A mapping from predicates to their representing BDDs.
             */
            std::map<storm::expressions::Expression, storm::dd::Bdd<DdType>> getPredicateToBddMap() const;
            
            /*!
             * Retrieves the meta variables pairs for all predicates.
             *
             * @return The meta variable pairs for all predicates.
             */
            std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& getSourceSuccessorVariablePairs() const;

            /*!
             * Retrieves the meta variables pairs for all predicates together with the meta variables marking the bottom states.
             *
             * @return The meta variable pairs for all predicates and bottom states.
             */
            std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& getExtendedSourceSuccessorVariablePairs() const;

            /*!
             * Retrieves the meta variable marking the bottom states.
             *
             * @param source A flag indicating whether the source or successor meta variable is to be returned.
             * @return The meta variable marking the bottom states.
             */
            storm::expressions::Variable const& getBottomStateVariable(bool source) const;
            
            /*!
             * Retrieves the BDD that can be used to mark the bottom states.
             *
             * @param source A flag indicating whether the source or successor BDD is to be returned.
             * @param negated A flag indicating whether the BDD should encode the bottom states or the non-bottom states.
             * @return The BDD that can be used to mark bottom states.
             */
            storm::dd::Bdd<DdType> getBottomStateBdd(bool source, bool negated) const;
            
            /*!
             * Retrieves the BDD for the predicate with the given index over the source variables.
             *
             * @param predicateIndex The index of the predicate.
             * @return The encoding the predicate over the source variables.
             */
            storm::dd::Bdd<DdType> const& encodePredicateAsSource(uint_fast64_t predicateIndex) const;

            /*!
             * Retrieves the BDD for the predicate with the given index over the successor variables.
             *
             * @param predicateIndex The index of the predicate.
             * @return The encoding the predicate over the successor variables.
             */
            storm::dd::Bdd<DdType> const& encodePredicateAsSuccessor(uint_fast64_t predicateIndex) const;

            /*!
             * Retrieves a BDD representing the identity for the predicate with the given index.
             *
             * @param predicateIndex The index of the predicate.
             * @return The identity for the predicate.
             */
            storm::dd::Bdd<DdType> const& getPredicateIdentity(uint_fast64_t predicateIndex) const;
            
            /*!
             * Retrieves the predicate associated with the given DD variable index.
             *
             * @param ddVariableIndex The DD variable index for which to retrieve the predicate.
             * @return The predicate associated with the given DD variable index.
             */
            storm::expressions::Expression const& getPredicateForDdVariableIndex(uint_fast64_t ddVariableIndex) const;
            
            /*!
             * Declares new variables for the missing predicates.
             *
             * @param oldPredicates The old predicates.
             * @param newPredicates The new predicates.
             * @return A list of the missing variables together with the predicate index they represent.
             */
            std::vector<std::pair<storm::expressions::Variable, uint_fast64_t>> declareNewVariables(std::vector<std::pair<storm::expressions::Variable, uint_fast64_t>> const& oldPredicates, std::set<uint_fast64_t> const& newPredicates) const;
            
        private:
            /*!
             * Encodes the given index with the given number of variables from the given variables.
             *
             * @param index The index to encode.
             * @param start The index of the first variable to use for the encoding.
             * @param end The index of the variable past the end of the range to use for the encoding.
             * @param variables The BDDs of the variables to use to encode the index.
             */
            storm::dd::Bdd<DdType> encodeChoice(uint_fast64_t index, uint_fast64_t start, uint_fast64_t end, std::vector<storm::dd::Bdd<DdType>> const& variables) const;
                        
            // The expression related data.
            
            /// The manager responsible for the expressions of the program and the SMT solvers.
            std::reference_wrapper<storm::expressions::ExpressionManager> expressionManager;
            
            /// A mapping from predicates to their indices in the predicate list.
            // FIXME: Does this properly store the expressions? What about equality checking?
            std::unordered_map<storm::expressions::Expression, uint64_t> predicateToIndexMap;
            
            /// The current set of predicates used in the abstraction.
            std::vector<storm::expressions::Expression> predicates;
            
            /// The set of all variables.
            std::set<storm::expressions::Variable> variables;
            
            /// The expressions characterizing legal variable values.
            std::vector<storm::expressions::Expression> constraints;
            
            // The DD-related data.
            
            /// The manager responsible for the DDs.
            std::shared_ptr<storm::dd::DdManager<DdType>> ddManager;
            
            /// The DD variables corresponding to the predicates.
            std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> predicateDdVariables;
            
            /// The DD variables corresponding to the predicates together with the DD variables marking the bottom states.
            std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> extendedPredicateDdVariables;
            
            /// The set of all source variables.
            std::set<storm::expressions::Variable> sourceVariables;
            
            /// The set of all source variables.
            std::set<storm::expressions::Variable> successorVariables;
            
            /// The BDDs corresponding to the predicates.
            std::vector<std::pair<storm::dd::Bdd<DdType>, storm::dd::Bdd<DdType>>> predicateBdds;
            
            /// The BDDs representing the predicate identities (i.e. source and successor variable have the same truth value).
            std::vector<storm::dd::Bdd<DdType>> predicateIdentities;
            
            /// A BDD that represents the identity of all predicate variables.
            storm::dd::Bdd<DdType> allPredicateIdentities;
            
            /// A meta variable pair that marks bottom states.
            std::pair<storm::expressions::Variable, storm::expressions::Variable> bottomStateVariables;
            
            /// The BDDs associated with the bottom state variable pair.
            std::pair<storm::dd::Bdd<DdType>, storm::dd::Bdd<DdType>> bottomStateBdds;
            
            /// A mapping from DD variable indices to the predicate index they represent.
            std::unordered_map<uint_fast64_t, uint_fast64_t> ddVariableIndexToPredicateIndexMap;
            
            /// Variables that encode the choices of player 1.
            std::vector<storm::expressions::Variable> player1Variables;
            
            /// The BDDs associated with the meta variables of player 1.
            std::vector<storm::dd::Bdd<DdType>> player1VariableBdds;

            /// Variables that encode the choices of player 2.
            std::vector<storm::expressions::Variable> player2Variables;
            
            /// The BDDs associated with the meta variables of player 2.
            std::vector<storm::dd::Bdd<DdType>> player2VariableBdds;
            
            /// Variables that can be used to encode auxiliary information.
            std::vector<storm::expressions::Variable> auxVariables;
            
            /// The BDDs associated with the meta variables encoding auxiliary information.
            std::vector<storm::dd::Bdd<DdType>> auxVariableBdds;
        };
        
    }
}