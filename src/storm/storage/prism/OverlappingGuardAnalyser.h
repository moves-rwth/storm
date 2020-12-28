#pragma once
#include <cstdint>
#include <memory>

namespace storm {
    namespace utility {
        namespace solver {
            class SmtSolverFactory;
        }
    }

    namespace solver {
        class SmtSolver;
    }

    namespace prism {
        class Program;
        class Module;

        class OverlappingGuardAnalyser {
        public:
            OverlappingGuardAnalyser(Program const& program, std::shared_ptr<storm::utility::solver::SmtSolverFactory>& smtSolverFactory);
            bool hasModuleWithInnerActionOverlap();

        private:
            Program const& program;
            std::unique_ptr<storm::solver::SmtSolver> smtSolver;
            bool initializedWithStateConstraints;
        };
    }
}

