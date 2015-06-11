#include "src/solver/SymbolicGameSolver.h"

#include "src/storage/dd/CuddBdd.h"
#include "src/storage/dd/CuddAdd.h"

namespace storm {
    namespace solver {

        template<storm::dd::DdType Type>
        SymbolicGameSolver<Type>::SymbolicGameSolver(storm::dd::Add<Type> const& gameMatrix, storm::dd::Bdd<Type> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs, std::set<storm::expressions::Variable> const& player1Variables, std::set<storm::expressions::Variable> const& player2Variables) : gameMatrix(gameMatrix), allRows(allRows), rowMetaVariables(rowMetaVariables), columnMetaVariables(columnMetaVariables), rowColumnMetaVariablePairs(rowColumnMetaVariablePairs), player1Variables(player1Variables), player2Variables(player2Variables) {
            // Intentionally left empty.
        }
     
        template class SymbolicGameSolver<storm::dd::DdType::CUDD>;
        
    }
}