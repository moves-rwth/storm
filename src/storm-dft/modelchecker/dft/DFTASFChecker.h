#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "storm/solver/SmtSolver.h"
#include "SmtConstraint.h"
#include "storm-dft/storage/dft/DFT.h"
#include "storm/utility/solver.h"

namespace storm {
    namespace modelchecker {
        class SpareAndChildPair {
        public:
            SpareAndChildPair(uint64_t spareIndex, uint64_t childIndex) : spareIndex(spareIndex), childIndex(childIndex) {
            }
            
            friend bool operator<(SpareAndChildPair const& p1, SpareAndChildPair const& p2) {
                return p1.spareIndex < p2.spareIndex || (p1.spareIndex == p2.spareIndex && p1.childIndex < p2.childIndex);
            }

        private:
            uint64_t spareIndex;
            uint64_t childIndex;
        };


        class DFTASFChecker {
            using ValueType = double;
        public:
            DFTASFChecker(storm::storage::DFT<ValueType> const&);
            /**
             * Generate general variables and constraints for the DFT and store them in the corresponding maps and vectors
             *
             */
            void convert();
            void toFile(std::string const&);

            /**
             * Generates a new solver instance and prepares it for SMT checking of the DFT. Needs to be called before all queries to the solver
             */
            void toSolver();

            /**
             * Check if the TLE of the DFT never fails
             *
             * @return  "Sat" if TLE never fails, "Unsat" if it does, otherwise "Unknown"
             */
            storm::solver::SmtSolver::CheckResult checkTleNeverFailed();

            /**
             * Check if there exists a sequence of BE failures of exactly given length such that the TLE of the DFT fails
             *
             * @param bound the length of the sequene
             * @return "Sat" if such a sequence exists, "Unsat" if it does not, otherwise "Unknown"
             */
            storm::solver::SmtSolver::CheckResult checkTleFailsWithEq(uint64_t bound);

            /**
             * Check if there exists a sequence of BE failures of at least given length such that the TLE of the DFT fails
             *
             * @param bound the length of the sequence
             * @return "Sat" if such a sequence exists, "Unsat" if it does not, otherwise "Unknown"
             */
            storm::solver::SmtSolver::CheckResult checkTleFailsWithLeq(uint64_t bound);

            /**
             * Get the minimal number of BEs necessary for the TLE to fail (lower bound for number of failures to check)
             *
             * @return the minimal number
             */
            uint64_t getLeastFailureBound();

            /**
             * Get the number of BE failures for which the TLE always fails (upper bound for number of failures to check)
             *
             * @return the number
             */
            uint64_t getAlwaysFailedBound();

            /**
             * Set the timeout of the solver
             *
             * @param milliseconds the timeout in milliseconds
             */
            void setSolverTimeout(uint_fast64_t milliseconds);

            /**
             * Unset the timeout for the solver
             */
            void unsetSolverTimeout();
            
        private:
            uint64_t getClaimVariableIndex(uint64_t spareIndex, uint64_t childIndex) const;

            /**
             * Generate constraint for spares such that when trying to claim child (i+1), other spares either claimed it
             * before or will never claim it
             *
             * @param spare Spare.
             * @param childIndex Index of child to consider in spare children
             * @param timepoint Timepoint to try to claim
             * @return
             */
            std::shared_ptr<SmtConstraint>
            generateClaimEarlyConstraint(std::shared_ptr<storm::storage::DFTSpare<ValueType> const> spare,
                                         uint64_t childIndex) const;

            /**
             * Generate constraint for 'spare (s) tries to claim the child (i) at the given timepoint (t)'.
             * This corresponds to the function \phi^s_i(t) in constraint 7.
             *
             * @param spare Spare.
             * @param childIndex Index of child to consider in spare children.
             * @param timepoint Timepoint to try to claim.
             *
             * @return Constraint encoding the claiming.
             */
            std::shared_ptr<SmtConstraint>
            generateTryToClaimConstraint(std::shared_ptr<storm::storage::DFTSpare<ValueType> const> spare,
                                         uint64_t childIndex, uint64_t timepoint) const;

            /**
             * Add constraints encoding AND gates.
             * This corresponds to constraint (1)
             */
            void generateAndConstraint(size_t i, std::vector<uint64_t> childVarIndices,
                                       std::shared_ptr<storm::storage::DFTElement<ValueType> const> element);

            /**
             * Add constraints encoding OR gates.
             * This corresponds to constraint (2)
             */
            void generateOrConstraint(size_t i, std::vector<uint64_t> childVarIndices,
                                      std::shared_ptr<storm::storage::DFTElement<ValueType> const> element);

            /**
             * Add constraints encoding VOT gates.
             */
            void generateVotConstraint(size_t i, std::vector<uint64_t> childVarIndices,
                                       std::shared_ptr<storm::storage::DFTElement<ValueType> const> element);

            /**
             * Add constraints encoding PAND gates.
             * This corresponds to constraint (3)
             */
            void generatePandConstraint(size_t i, std::vector<uint64_t> childVarIndices,
                                        std::shared_ptr<storm::storage::DFTElement<ValueType> const> element);

            /**
             * Add constraints encoding POR gates.
             */
            void generatePorConstraint(size_t i, std::vector<uint64_t> childVarIndices,
                                       std::shared_ptr<storm::storage::DFTElement<ValueType> const> element);

            /**
             * Add constraints encoding SEQ gates.
             * This corresponds to constraint (4)
             */
            void generateSeqConstraint(std::vector<uint64_t> childVarIndices,
                                       std::shared_ptr<storm::storage::DFTElement<ValueType> const> element);

            /**
            * Add constraints encoding SPARE gates.
            * This corresponds to constraints (5),(6),(7)
            */
            void generateSpareConstraint(size_t i, std::vector<uint64_t> childVarIndices,
                                         std::shared_ptr<storm::storage::DFTElement<ValueType> const> element);

            /**
            * Add constraints encoding claiming rules.
            * This corresponds to constraint (8) and addition
            */
            void addClaimingConstraints();

            /**
             * Add constraints encoding Markovian states.
             * This corresponds to constraints (9), (10) and (11)
             */
            void addMarkovianConstraints();
            
            storm::storage::DFT<ValueType> const& dft;
            std::shared_ptr<storm::solver::SmtSolver> solver = nullptr;
            std::vector<std::string> varNames;
            std::unordered_map<uint64_t, uint64_t> timePointVariables;
            std::vector<std::shared_ptr<SmtConstraint>> constraints;
            std::map<SpareAndChildPair, uint64_t> claimVariables;
            std::unordered_map<uint64_t, uint64_t> markovianVariables;
            std::vector<uint64_t> tmpTimePointVariables;
            uint64_t notFailed;
        };
    }
}
