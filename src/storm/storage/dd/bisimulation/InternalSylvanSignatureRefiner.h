#pragma once

#include "storm/storage/dd/bisimulation/InternalSignatureRefiner.h"

#include "storm/storage/dd/Bdd.h"

#include "storm/storage/expressions/Variable.h"

#include "storm/storage/dd/sylvan/utility.h"

#include <sparsepp/spp.h>

namespace storm {
    namespace dd {
        template <storm::dd::DdType DdType>
        class DdManager;
        
        namespace bisimulation {
            
            template <storm::dd::DdType DdType, typename ValueType>
            class Partition;
            
            template <storm::dd::DdType DdType, typename ValueType>
            class Signature;
            
            class InternalSylvanSignatureRefinerBase {
            public:
                InternalSylvanSignatureRefinerBase(storm::dd::DdManager<storm::dd::DdType::Sylvan> const& manager, storm::expressions::Variable const& blockVariable, std::set<storm::expressions::Variable> const& stateVariables, storm::dd::Bdd<storm::dd::DdType::Sylvan> const& nondeterminismVariables, storm::dd::Bdd<storm::dd::DdType::Sylvan> const& nonBlockVariables, InternalSignatureRefinerOptions const& options);
                
                BDD encodeBlock(uint64_t blockIndex);
                
                storm::dd::DdManager<storm::dd::DdType::Sylvan> const& manager;
                storm::expressions::Variable blockVariable;
                std::set<storm::expressions::Variable> stateVariables;
                
                storm::dd::Bdd<storm::dd::DdType::Sylvan> nondeterminismVariables;
                storm::dd::Bdd<storm::dd::DdType::Sylvan> nonBlockVariables;
                
                // The provided options.
                InternalSignatureRefinerOptions options;
                
                uint64_t numberOfBlockVariables;
                
                storm::dd::Bdd<storm::dd::DdType::Sylvan> blockCube;
                
                // The current number of blocks of the new partition.
                uint64_t nextFreeBlockIndex;
                
                // The number of completed refinements.
                uint64_t numberOfRefinements;
                
                // The cache used to identify states with identical signature.
                spp::sparse_hash_map<std::pair<MTBDD, MTBDD>, std::pair<BDD, BDD>, SylvanMTBDDPairHash> signatureCache;
                
                // The cache used to identify which old block numbers have already been reused.
                spp::sparse_hash_map<MTBDD, ReuseWrapper> reuseBlocksCache;
                
                // Data used by sylvan implementation.
                std::vector<BDD> signatures;
                std::vector<uint64_t> table;
                std::vector<uint64_t> oldTable;
                uint64_t resize;
            };
            
            template<typename ValueType>
            class InternalSignatureRefiner<storm::dd::DdType::Sylvan, ValueType> : public InternalSylvanSignatureRefinerBase {
            public:
                InternalSignatureRefiner(storm::dd::DdManager<storm::dd::DdType::Sylvan> const& manager, storm::expressions::Variable const& blockVariable, std::set<storm::expressions::Variable> const& stateVariables, storm::dd::Bdd<storm::dd::DdType::Sylvan> const& nondeterminismVariables, storm::dd::Bdd<storm::dd::DdType::Sylvan> const& nonBlockVariables, InternalSignatureRefinerOptions const& options);
                
                Partition<storm::dd::DdType::Sylvan, ValueType> refine(Partition<storm::dd::DdType::Sylvan, ValueType> const& oldPartition, Signature<storm::dd::DdType::Sylvan, ValueType> const& signature);

            private:
                void clearCaches();
                
                std::pair<storm::dd::Bdd<storm::dd::DdType::Sylvan>, boost::optional<storm::dd::Bdd<storm::dd::DdType::Sylvan>>> refine(Partition<storm::dd::DdType::Sylvan, ValueType> const& oldPartition, storm::dd::Add<storm::dd::DdType::Sylvan, ValueType> const& signatureAdd);
                
                std::pair<BDD, BDD> reuseOrRelabel(BDD partitionNode, BDD nondeterminismVariablesNode, BDD nonBlockVariablesNode);
                std::pair<BDD, BDD> refineParallel(BDD partitionNode, MTBDD signatureNode, BDD nondeterminismVariablesNode, BDD nonBlockVariablesNode);
                std::pair<BDD, BDD> refineSequential(BDD partitionNode, MTBDD signatureNode, BDD nondeterminismVariablesNode, BDD nonBlockVariablesNode);
                
            };
            
        }
    }
}
