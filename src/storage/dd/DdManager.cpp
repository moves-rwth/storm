

namespace storm {
    namespace dd {
        template<DdType LibraryType>
        std::vector<uint_fast64_t> Dd<LibraryType>::getSortedVariableIndices(DdManager<LibraryType> const& manager, std::set<storm::expressions::Variable> const& metaVariables) {
            std::vector<uint_fast64_t> ddVariableIndices;
            for (auto const& metaVariableName : metaVariables) {
                auto const& metaVariable = manager.getMetaVariable(metaVariableName);
                for (auto const& ddVariable : metaVariable.getDdVariables()) {
                    ddVariableIndices.push_back(ddVariable.getIndex());
                }
            }
            
            // Next, we need to sort them, since they may be arbitrarily ordered otherwise.
            std::sort(ddVariableIndices.begin(), ddVariableIndices.end());
            return ddVariableIndices;
        }
    }
}