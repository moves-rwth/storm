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

        class DependencyPair {
        public:
            DependencyPair(uint64_t depIndex, uint64_t childIndex) : depIndex(depIndex), childIndex(childIndex) {
            }

            friend bool operator<(DependencyPair const &p1, DependencyPair const &p2) {
                return p1.depIndex < p2.depIndex || (p1.depIndex == p2.depIndex && p1.childIndex < p2.childIndex);
            }

        private:
            uint64_t depIndex;
            uint64_t childIndex;
        };


        class DFTASFChecker {
            using ValueType = double;
        public:
            DFTASFChecker(storm::storage::DFT<ValueType> const&);

            /**
             * Activates the experimental support for constant BEs and possibly other not thoroughly tested features
             *
             */
            void activateExperimentalMode();

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
             * Check if two given dependencies are conflicting in their resolution, i.e. check if non-determinism may occur.
             * Note that this is a very conservative check using SMT formulae.
             * We only check if sequences exist, where one of the dependencies is triggered before the other is completely resolved
             *
             * @param dep1Index Index of the first dependency
             * @param dep2Index Index of the second dependency
             * @param timeout timeout for the solver
             * @return "Sat" if the dependencies are conflicting, "Unsat" if they are not, otherwise "Unknown"
             */
            storm::solver::SmtSolver::CheckResult
            checkDependencyConflict(uint64_t dep1Index, uint64_t dep2Index, uint64_t timeout = 10);

            /**
             * Get the minimal number of BEs necessary for the TLE to fail (lower bound for number of failures to check)
             *
             * @param timeout timeout for each query in seconds, defaults to 10 seconds
             * @return the minimal number
             */
            uint64_t getLeastFailureBound(uint_fast64_t timeout = 10);

            /**
             * Get the number of BE failures for which the TLE always fails (upper bound for number of failures to check).
             * Note that the returned value may be higher than the real one when dependencies are present.
             *
             * @param timeout timeout for each query in seconds, defaults to 10 seconds
             * @return the number
             */
            uint64_t getAlwaysFailedBound(uint_fast64_t timeout = 10);

            /**
             * Get a vector of index pairs of FDEPs which are conflicting according to a conservative definition
             *
             * @param timeout timeout for each query in seconds, defaults to 10 seconds
             * @return a vector of pairs of FDEP indices which are conflicting
             */
            std::vector<std::pair<uint64_t, uint64_t>> getDependencyConflicts(uint_fast64_t timeout = 10);

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
            /**
             * Helper function to check if the TLE fails before or at a given timepoint while visiting exactly
             * a given number of non-Markovian states
             *
             * @param checkbound timepoint to check against
             * @param nrNonMarkovian the number of non-Markovian states to check against
             * @return "Sat" if a sequence of BE failures exists such that the constraints are satisfied,
             * "Unsat" if it does not, otherwise "Unknown"
             */
            storm::solver::SmtSolver::CheckResult
            checkFailsLeqWithEqNonMarkovianState(uint64_t checkbound, uint64_t nrNonMarkovian);

            /**
             * Helper function that checks if the DFT can fail at a timepoint while visiting a given number of Markovian states
             *
             * @param timepoint point in time to check
             * @return "Sat" if a sequence of BE failures exists such that less than checkNumber Markovian states are visited,
             * "Unsat" if it does not, otherwise "Unknown"
             */
            storm::solver::SmtSolver::CheckResult
            checkFailsAtTimepointWithEqNonMarkovianState(uint64_t timepoint, uint64_t nrNonMarkovian);

            /**
             * Helper function for correction of least failure bound when dependencies are present.
             * The main idea is to check if a later point of failure for the TLE than the pre-computed bound exists, but
             * up until that point the number of non-Markovian states visited is so large, that less than the pre-computed bound BEs fail by themselves.
             * The corrected bound is then (newTLEFailureTimepoint)-(nrNonMarkovianStatesVisited). This term is minimized.
             *
             * @param bound known lower bound to be corrected
             * @param timeout timeout timeout for each query in seconds
             * @return the corrected bound
             */
            uint64_t correctLowerBound(uint64_t bound, uint_fast64_t timeout);

            /**
             * Helper function for correction of bound for number of BEs such that the DFT always fails when dependencies are present
             *
             * @param bound known bound to be corrected
             * @param timeout timeout timeout for each query in seconds
             * @return the corrected bound
             */
            uint64_t correctUpperBound(uint64_t bound, uint_fast64_t timeout);

            uint64_t getClaimVariableIndex(uint64_t spareIndex, uint64_t childIndex) const;

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
             * Add constraints encoding PDEP gates.
             *
             */
            void generatePdepConstraint(size_t i, std::vector<uint64_t> childVarIndices,
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
            std::unordered_map<uint64_t, uint64_t> dependencyVariables;
            std::unordered_map<uint64_t, uint64_t> markovianVariables;
            std::vector<uint64_t> tmpTimePointVariables;
            uint64_t notFailed;
            bool experimentalMode = false;
        };
    }
}
