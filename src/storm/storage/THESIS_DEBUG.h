#ifndef STORM_STORAGE_THESIS_DEBUG_H
#define STORM_STORAGE_THESIS_DEBUG_H

#include "storm/models/Model.h"
#include "storm/models/symbolic/NondeterministicModel.h"
#include "storm/storage/SymbolicMEC.h"
#include "storm/storage/SymbolicMEC_stats.h"
#include "storm/storage/dd/DdType.h"
#include "storm/utility/Stopwatch.h"
#include "storm/utility/macros.h"

#include <chrono>

enum class MecBenchmarkType { NAIVE, NAIVE_STATS, LOCKSTEP, LOCKSTEP_STATS, INTERLEAVE, INTERLEAVE_STATS };

MecBenchmarkType debugIntToBenchmarkType(uint64_t forcedMecAlgorithm) {
    switch (forcedMecAlgorithm) {
        case 1:
            return MecBenchmarkType::NAIVE;
        case 2:
            return MecBenchmarkType::NAIVE_STATS;
        case 3:
            return MecBenchmarkType::LOCKSTEP;
        case 4:
            return MecBenchmarkType::LOCKSTEP_STATS;
        case 5:
            return MecBenchmarkType::INTERLEAVE;
        case 6:
            return MecBenchmarkType::INTERLEAVE_STATS;
        default:
            STORM_LOG_ASSERT(false, "Unexpected symbolic benchmark int");
    }
    STORM_LOG_ASSERT(false, "Unreachable");
}

std::string benchmarkToString(const MecBenchmarkType type) {
    switch (type) {
        case MecBenchmarkType::NAIVE:
            return "NAIVE";
        case MecBenchmarkType::NAIVE_STATS:
            return "NAIVE-STATS";
        case MecBenchmarkType::LOCKSTEP:
            return "LOCKSTEP";
        case MecBenchmarkType::LOCKSTEP_STATS:
            return "LOCKSTEP-STATS";
        case MecBenchmarkType::INTERLEAVE:
            return "INTERLEAVE";
        case MecBenchmarkType::INTERLEAVE_STATS:
            return "INTERLEAVE-STATS";
    }
}

struct BenchmarkResult {
    MecBenchmarkType type;
    uint_fast64_t mecCount;
    storm::utility::Stopwatch::NanosecondType decompositionTimeInNanoseconds;

    BenchmarkResult(MecBenchmarkType type, uint_fast64_t mecCount, storm::utility::Stopwatch::NanosecondType decompositionTimeInNanoseconds) {
        this->type = type;
        this->mecCount = mecCount;
        this->decompositionTimeInNanoseconds = decompositionTimeInNanoseconds;
    }

    void print() {
        std::string algorithm = benchmarkToString(type);
        std::cout << "BENCHMARK MEC DECOMPOSITION (algorithm, time, mecCount): " << algorithm << ", " << decompositionTimeInNanoseconds << ", " << mecCount
                  << "\n";
    }
};

template<storm::dd::DdType DdType, typename ValueType>
uint_fast64_t getDdVariablesCount(storm::dd::DdManager<DdType>& manager, std::set<storm::expressions::Variable> variables) {
    uint_fast64_t count = 0;
    for (auto const& metaVariable : variables) {
        count += manager.getMetaVariable(metaVariable).getNumberOfDdVariables();
    }
    return count;
}

template<storm::dd::DdType DdType, typename ValueType>
BenchmarkResult doSymbolicBenchmark(storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& symbolicModel, MecBenchmarkType type) {
    auto& manager = symbolicModel.getManager();
    std::vector<storm::dd::Bdd<DdType>> mecs;
    std::chrono::high_resolution_clock::time_point decompositionTimestampStart;
    std::chrono::high_resolution_clock::time_point decompositionTimestampEnd;
    uint_fast64_t countSymbolicOps = 0;

    switch (type) {
        case MecBenchmarkType::NAIVE:
            decompositionTimestampStart = std::chrono::high_resolution_clock::now();
            mecs = symbolicMEC::symbolicMECDecompositionNaive<DdType, ValueType>(
                symbolicModel.getReachableStates(), symbolicModel.getTransitionMatrix().toBdd(), symbolicModel.getRowVariables(),
                symbolicModel.getColumnVariables(), symbolicModel.getNondeterminismVariables(), symbolicModel.getRowColumnMetaVariablePairs());
            decompositionTimestampEnd = std::chrono::high_resolution_clock::now();
            break;

        case MecBenchmarkType::NAIVE_STATS:
            decompositionTimestampStart = std::chrono::high_resolution_clock::now();
            mecs = symbolicMEC_stats::symbolicMECDecompositionNaive_stats<DdType, ValueType>(
                symbolicModel.getReachableStates(), symbolicModel.getTransitionMatrix().toBdd(), symbolicModel.getRowVariables(),
                symbolicModel.getColumnVariables(), symbolicModel.getNondeterminismVariables(), symbolicModel.getRowColumnMetaVariablePairs(),
                countSymbolicOps);
            decompositionTimestampEnd = std::chrono::high_resolution_clock::now();
            std::cout << "DEBUG SYMBOLIC OPS " << countSymbolicOps << std::endl;
            break;

        case MecBenchmarkType::LOCKSTEP:
            decompositionTimestampStart = std::chrono::high_resolution_clock::now();
            mecs = symbolicMEC::symbolicMECDecompositionLockstep<DdType, ValueType>(
                symbolicModel.getReachableStates(), symbolicModel.getTransitionMatrix().toBdd(), symbolicModel.getRowVariables(),
                symbolicModel.getColumnVariables(), symbolicModel.getNondeterminismVariables(), symbolicModel.getRowColumnMetaVariablePairs());
            decompositionTimestampEnd = std::chrono::high_resolution_clock::now();
            break;

        case MecBenchmarkType::LOCKSTEP_STATS:
            decompositionTimestampStart = std::chrono::high_resolution_clock::now();
            mecs = symbolicMEC_stats::symbolicMECDecompositionLockstep_stats<DdType, ValueType>(
                symbolicModel.getReachableStates(), symbolicModel.getTransitionMatrix().toBdd(), symbolicModel.getRowVariables(),
                symbolicModel.getColumnVariables(), symbolicModel.getNondeterminismVariables(), symbolicModel.getRowColumnMetaVariablePairs(),
                countSymbolicOps);
            decompositionTimestampEnd = std::chrono::high_resolution_clock::now();
            std::cout << "DEBUG SYMBOLIC OPS " << countSymbolicOps << std::endl;
            break;

        case MecBenchmarkType::INTERLEAVE:
            decompositionTimestampStart = std::chrono::high_resolution_clock::now();
            mecs = symbolicMEC::symbolicMECDecompositionInterleave<DdType, ValueType>(
                symbolicModel.getReachableStates(), symbolicModel.getTransitionMatrix().toBdd(), symbolicModel.getRowVariables(),
                symbolicModel.getColumnVariables(), symbolicModel.getNondeterminismVariables(), symbolicModel.getRowColumnMetaVariablePairs());
            decompositionTimestampEnd = std::chrono::high_resolution_clock::now();
            break;

        case MecBenchmarkType::INTERLEAVE_STATS:
            decompositionTimestampStart = std::chrono::high_resolution_clock::now();
            mecs = symbolicMEC_stats::symbolicMECDecompositionInterleave_stats<DdType, ValueType>(
                symbolicModel.getReachableStates(), symbolicModel.getTransitionMatrix().toBdd(), symbolicModel.getRowVariables(),
                symbolicModel.getColumnVariables(), symbolicModel.getNondeterminismVariables(), symbolicModel.getRowColumnMetaVariablePairs(),
                countSymbolicOps);
            std::cout << "DEBUG SYMBOLIC OPS " << countSymbolicOps << std::endl;
            decompositionTimestampEnd = std::chrono::high_resolution_clock::now();
            break;

        default:
            STORM_LOG_ASSERT(false, "Unexpected symbolic benchmark type");
            break;
    }

    // Output Model stats
    uint_fast64_t ddRowVariableCount = getDdVariablesCount<DdType, ValueType>(manager, symbolicModel.getRowVariables());
    uint_fast64_t ddColumnVariableCount = getDdVariablesCount<DdType, ValueType>(manager, symbolicModel.getColumnVariables());
    uint_fast64_t ddNondeterminismCount = getDdVariablesCount<DdType, ValueType>(manager, symbolicModel.getNondeterminismVariables());
    std::cout << "Symbolic model stats: " << std::endl
              << "Type: " << symbolicModel.getType() << std::endl
              << "States: " << symbolicModel.getNumberOfStates() << " (" << symbolicModel.getReachableStates().getNodeCount() << " nodes)" << std::endl
              << "Transitions: " << symbolicModel.getNumberOfTransitions() << " (" << symbolicModel.getTransitionMatrix().toBdd().getNodeCount() << " nodes)"
              << std::endl
              << "Choices: " << symbolicModel.getNumberOfChoices() << std::endl
              << "Variables Total: "
              << (symbolicModel.getRowVariables().size() + symbolicModel.getColumnVariables().size() + symbolicModel.getNondeterminismVariables().size())
              << " (" << (ddRowVariableCount + ddColumnVariableCount + ddNondeterminismCount) << " DD variables)" << std::endl
              << "Variables Row: " << symbolicModel.getRowVariables().size() << " (" << ddRowVariableCount << " DD variables)" << std::endl
              << "Variables Column: " << symbolicModel.getColumnVariables().size() << " (" << ddColumnVariableCount << " DD variables)" << std::endl
              << "Variables Nondeterminism: " << symbolicModel.getNondeterminismVariables().size() << " (" << ddNondeterminismCount << " DD variables)"
              << std::endl
              << std::endl;

    uint_fast64_t decompositionTimeInNanoseconds = (decompositionTimestampEnd - decompositionTimestampStart).count();
    return BenchmarkResult(type, mecs.size(), decompositionTimeInNanoseconds);
}

template<storm::dd::DdType DdType, typename ValueType>
void doMecBenchmark(storm::models::ModelBase const& model, uint64_t forcedMecAlgorithm) {
    assert(forcedMecAlgorithm > 0);
    MecBenchmarkType type = debugIntToBenchmarkType(forcedMecAlgorithm);
    BenchmarkResult benchmark = doSymbolicBenchmark<DdType, ValueType>((storm::models::symbolic::NondeterministicModel<DdType, ValueType>&)model, type);
    benchmark.print();
}

#endif