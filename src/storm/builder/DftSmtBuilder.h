#ifndef DFTSMTBUILDER_H
#define	DFTSMTBUILDER_H

#include "src/storm/solver/SmtSolver.h"
#include "src/storm/utility/solver.h"
#include "src/storm/storage/dft/DFT.h"

namespace storm {
    namespace builder {

        template<typename ValueType>
        class DFTSMTBuilder {

        public:
            DFTSMTBuilder();
            
            void convertToSMT(storm::storage::DFT<ValueType> const& dft);
            
            bool check() const;

        private:
            
            std::shared_ptr<storm::solver::SmtSolver> solver;
            
            std::shared_ptr<storm::expressions::ExpressionManager> manager;
            
            storm::expressions::Expression timeMax;
            
            storm::expressions::Expression timeFailSafe;
            
            storm::expressions::Expression timeZero;
            
            storm::expressions::Expression topLevel;
            
            std::vector<storm::expressions::Variable> varsBE;

            storm::expressions::Variable convert(std::shared_ptr<storm::storage::DFTBE<ValueType> const> const& be);

            storm::expressions::Variable convert(std::shared_ptr<storm::storage::DFTGate<ValueType> const> const& gate);
            
            storm::expressions::Variable convert(std::shared_ptr<storm::storage::DFTDependency<ValueType> const> const& dependency);
            
            storm::expressions::Variable convert(std::shared_ptr<storm::storage::DFTRestriction<ValueType> const> const& restriction);

        };
    }
}

#endif	/* DFTSMTBUILDER_H */
