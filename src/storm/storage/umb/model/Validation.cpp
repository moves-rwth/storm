#include "storm/storage/umb/model/Validation.h"

#include <sstream>
#include <string_view>

#include "storm/storage/umb/model/UmbModel.h"
#include "storm/utility/macros.h"

#include "storm/exceptions/UnexpectedException.h"
#include "storm/exceptions/WrongFormatException.h"

namespace storm::umb {

namespace validation {
bool validateCsr(auto const& csr, std::string_view const name, uint64_t numMappedElements, uint64_t expectedlastEntry, std::ostream& err) {
    std::stringstream err_reason;
    if (csr) {
        // Check if csr has expected length and the form {0, ..., expectedlastEntry}
        if (csr->size() != numMappedElements + 1) {
            err_reason << "CSR has unexpected size: " << csr->size() << " != " << (numMappedElements + 1) << ".";
        }
        if (csr.value()[0] != 0) {
            err_reason << "CSR has unexpected first entry: " << csr.value()[0] << " != 0" << ".";
        }
        if (csr.value()[numMappedElements] != expectedlastEntry) {
            err_reason << "CSR has unexpected last entry: " << csr.value()[numMappedElements] << " != " << expectedlastEntry << ".";
        }
    } else if (numMappedElements != expectedlastEntry) {  // we assume a 1:1 mapping
        err_reason << "CSR is not given and the default 1:1 mapping {0, ... ," << numMappedElements << "} does not match. Expected the mapping to end with '"
                   << expectedlastEntry << "'" << ".";
    }
    if (!err_reason.view().empty()) {
        err << "Validation error in CSR mapping '" << name << "':\n\t" << err_reason.str() << "\n";
        return false;
    }
    return true;
}

bool validateTypeDeclaration(storm::umb::SizedType const& type, bool requireStandardSize, std::ostream& err) {
    using enum storm::umb::Type;
    uint64_t const size = type.bitSize();
    if (size == 0) {
        err << "Type declaration " << type.toString() << " has size 0.\n";
        return false;
    }
    uint64_t const defaultSize = defaultBitSize(type.type);
    bool sizeError = false;
    switch (type.type) {
        case Double:
        case DoubleInterval:
        case String:
            // types that always must be their default size
            sizeError = size != defaultSize;
            break;
        case Bool:
        case Int:
        case Uint:
        case IntInterval:
        case UintInterval:
            // types that occasionally must be their default size
            sizeError = requireStandardSize && (size != defaultSize);
            break;
            // types that occasionally must be a multiple of their default size
        case Rational:
        case RationalInterval:
            // types that occasionally must be their default size
            sizeError = requireStandardSize && ((size % defaultSize) != 0);
    }
    if (isIntervalType(type.type)) {
        // interval type sizes must be multiples of four or two
        sizeError = sizeError || (size % (type.type == RationalInterval ? 4 : 2) != 0);
    }
    if (sizeError) {
        err << "Type declaration " << type.toString() << " has invalid bit size: " << size << " (default size is " << defaultSize << ").\n";
        return false;
    }
    return true;
}

bool vectorMatchesType(storm::umb::GenericVector const& vector, storm::umb::SizedType const& type) {
    if (!vector.hasValue()) {
        return true;  // no values are given so nothing wrong with the vector.
    }

    using enum storm::umb::Type;
    switch (type.type) {
        case Bool:
            return vector.isType<bool>();
        case Int:
        case IntInterval:
            return vector.isType<int64_t>();
        case Uint:
        case UintInterval:
            return vector.isType<uint64_t>();
        case Double:
            return vector.isType<double>();
        case DoubleInterval:
            return vector.isType<double>() || vector.isType<storm::Interval>();
        case Rational:
        case RationalInterval:
            return vector.isType<storm::RationalNumber>() || vector.isType<uint64_t>();  // rationals might be encoded as uint64_t
        case String:
            return false;  // GenericVector currently does not have a string variant.
    }
    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unhandled type.");
}
}  // namespace validation

bool validate(storm::umb::UmbModel const& umbModel, std::ostream& err) {
    ////////////
    // Index
    ////////////
    auto const& index = umbModel.index;
    auto const& tsIndex = index.transitionSystem;
    bool isValid = true;

    // validate counts
    auto checkNum = [&err](uint64_t num, auto&& name, uint64_t lowerBound = 0) {
        if (num == storm::umb::ModelIndex::TransitionSystem::InvalidNumber) {
            err << "Number of " << name << " is not set.\n";
            return false;
        } else if (num < lowerBound) {
            err << "Number of " << name << " is " << num << " which is below lower bound " << lowerBound << ".\n";
            return false;
        }
        return true;
    };
    isValid &= checkNum(tsIndex.numPlayers, "players");
    isValid &= checkNum(tsIndex.numStates, "states", 1u);
    isValid &= checkNum(tsIndex.numInitialStates, "initial-states");
    isValid &= checkNum(tsIndex.numChoices, "choices");
    isValid &= checkNum(tsIndex.numChoiceActions, "choice-actions");
    isValid &= checkNum(tsIndex.numBranches, "branches");
    isValid &= checkNum(tsIndex.numBranchActions, "branch-actions");
    isValid &= checkNum(tsIndex.numObservations, "observations");

    // validate types
    if (tsIndex.branchProbabilityType) {
        isValid &= validation::validateTypeDeclaration(tsIndex.branchProbabilityType.value(), true, err);
        if (!isContinuousNumericType(tsIndex.branchProbabilityType->type)) {
            err << "Branch probability type must be a continuous numeric type.\n";
            isValid = false;
        }
    }
    if (tsIndex.exitRateType) {
        isValid &= validation::validateTypeDeclaration(tsIndex.exitRateType.value(), true, err);
        if (!isContinuousNumericType(tsIndex.exitRateType->type)) {
            err << "Exit rate type must be a continuous numeric type.\n";
            isValid = false;
        }
    }
    if (tsIndex.observationProbabilityType) {
        isValid &= validation::validateTypeDeclaration(tsIndex.observationProbabilityType.value(), true, err);
        if (!isContinuousNumericType(tsIndex.observationProbabilityType->type)) {
            err << "Observation probability type must be a continuous numeric type.\n";
            isValid = false;
        }
    }

    if (bool const hasObservations = tsIndex.numObservations > 0; hasObservations != tsIndex.observationsApplyTo.has_value()) {
        err << "observations-apply-to is " << (tsIndex.observationsApplyTo.has_value() ? "set" : "not set") << " although the number of observations is "
            << tsIndex.numObservations << ".\n";
        isValid = false;
    }

    if (index.annotations) {
        for (auto const& [annotationType, annotationMap] : index.annotations.value()) {
            for (auto const& [name, annotation] : annotationMap) {
                isValid &= validation::validateTypeDeclaration(annotation.type, true, err);
                if (annotation.probabilityType) {
                    isValid &= validation::validateTypeDeclaration(annotation.probabilityType.value(), true, err);
                    if (!isContinuousNumericType(annotation.probabilityType->type)) {
                        err << "Probability type for annotation '" << name << "' must be a continuous numeric type.\n";
                        isValid = false;
                    }
                }
                if (annotationType == "aps") {
                    if (!isBooleanType(annotation.type.type)) {
                        err << "Atomic proposition annotation '" << name << "' must be of boolean type.\n";
                        isValid = false;
                    }
                } else if (annotationType == "rewards") {
                    if (!isNumericType(annotation.type.type)) {
                        err << "Reward annotation '" << name << "' must have numeric type.\n";
                        isValid = false;
                    }
                }
            }
        }
    }

    if (index.valuations) {
        boost::pfr::for_each_field(index.valuations.value(), [&isValid, &err](auto const& description) {
            if (!description.has_value()) {
                return;
            }
            for (auto const& descr : description->classes) {
                for (auto const& var : descr.variables) {
                    if (std::holds_alternative<ValuationClassDescription::Variable>(var)) {
                        auto const& variable = std::get<ValuationClassDescription::Variable>(var);
                        if (variable.name.empty()) {
                            err << "A valuation description has a variable with an empty name.\n";
                            isValid = false;
                        }
                        isValid &= validation::validateTypeDeclaration(variable.type, false, err);
                    }
                }
                if (descr.sizeInBits() % 8 != 0) {
                    err << "A valuation description has size " << descr.sizeInBits() << " bits which is not a multiple of 8.\n";
                    isValid = false;
                }
            }
        });
    }

    ////////////
    // Files
    ////////////

    // States
    isValid &= validation::validateCsr(umbModel.stateToChoices, "state-to-choice", tsIndex.numStates, tsIndex.numChoices, err);
    if (umbModel.stateToPlayer.has_value()) {
        if (tsIndex.numPlayers == 0) {
            err << "state-to-player mapping is given but the model has no players.\n";
            isValid = false;
        } else if (umbModel.stateToPlayer->size() != tsIndex.numStates) {
            err << "state-to-player mapping has invalid size: " << umbModel.stateToPlayer->size() << " != #states=" << tsIndex.numStates << ".\n";
            isValid = false;
        }
    }
    if (umbModel.stateIsInitial.has_value() && umbModel.stateIsInitial->size() != tsIndex.numStates) {
        err << "state-is-initial has invalid size: " << umbModel.stateIsInitial->size() << " != #states=" << tsIndex.numStates << ".\n";
        isValid = false;
    }
    if (umbModel.stateIsMarkovian.has_value()) {
        if (tsIndex.time != ModelIndex::TransitionSystem::Time::UrgentStochastic) {
            err << "state-is-markovian is given but the model does not have urgent-stochastic time.\n";
            isValid = false;
        } else if (umbModel.stateIsMarkovian->size() != tsIndex.numStates) {
            err << "state-is-markovian has invalid size: " << umbModel.stateIsMarkovian->size() << " != #states=" << tsIndex.numStates << ".\n";
            isValid = false;
        }
    }
    if (umbModel.stateToExitRate.hasValue()) {
        if (tsIndex.time == ModelIndex::TransitionSystem::Time::Discrete) {
            err << "state-to-exit-rate mapping is given but the model has discrete time.\n";
            isValid = false;
        }
        if (umbModel.stateToExitRate.size() != tsIndex.numStates) {
            err << "state-to-exit-rate mapping has invalid size: " << umbModel.stateToExitRate.size() << " != #states=" << tsIndex.numStates << ".\n";
            isValid = false;
        }
        if (!tsIndex.exitRateType.has_value()) {
            err << "state-to-exit-rate mapping is given but exit rate type is not declared.\n";
            isValid = false;
        } else if (!validation::vectorMatchesType(umbModel.stateToExitRate, tsIndex.exitRateType.value())) {
            err << "state-to-exit-rate mapping has values that do not match the declared exit rate type " << tsIndex.exitRateType->toString() << ".\n";
            isValid = false;
        }
    }

    // Choices
    isValid &= validation::validateCsr(umbModel.choiceToBranches, "choice-to-branch", tsIndex.numChoices, tsIndex.numBranches, err);

    // Branches
    if (umbModel.branchToTarget.has_value() && umbModel.branchToTarget->size() != tsIndex.numBranches) {
        err << "branch-to-target mapping has invalid size: " << umbModel.branchToTarget->size() << " != #branches=" << tsIndex.numBranches << ".\n";
        isValid = false;
    }
    if (umbModel.branchToProbability.hasValue()) {
        if (umbModel.branchToProbability.size() != tsIndex.numBranches) {
            err << "branch-to-probability mapping has invalid size: " << umbModel.branchToProbability.size() << " != #branches=" << tsIndex.numBranches
                << ".\n";
            isValid = false;
        }
        if (!tsIndex.branchProbabilityType.has_value()) {
            err << "branch-to-probability mapping is given but branch probability type is not declared.\n";
            isValid = false;
        } else if (!validation::vectorMatchesType(umbModel.stateToExitRate, tsIndex.branchProbabilityType.value())) {
            err << "branch-to-probability mapping has values that do not match the declared branch probability type "
                << tsIndex.branchProbabilityType->toString() << ".\n";
            isValid = false;
        }
    }

    // Action labels
    // TODO

    // Observations
    // TODO

    // Annotations
    // TODO

    // Valuations
    // TODO

    // : add more validations

    //    // Validate state valuations
    //    if (index.stateValuations.has_value() != umbModel.states.stateValuations.has_value()) {
    //        if (index.stateValuations.has_value()) {
    //            err << "State valuations described in index file but not present.\n";
    //        } else {
    //            err << "State valuations present but not described in index file.\n";
    //        }
    //        isValid = false;
    //    } else if (index.stateValuations.has_value()) {
    //        uint64_t const alignment = index.stateValuations->alignment;
    //        uint64_t const numBytes = umbModel.states.stateValuations->size();
    //        if (alignment == 0) {
    //            err << "State valuation alignment is 0.\n";
    //            isValid = false;
    //        }
    //        if (numBytes % alignment != 0) {
    //            err << "State valuation data size is " << umbModel.states.stateValuations->size() << " which is not a multiple of the alignment '" <<
    //            alignment
    //                << "'.\n";
    //            isValid = false;
    //        }
    //        isValid &= validation::validateCsr(umbModel.states.stateToValuation, "state-to-valuation", tsIndex.numStates, numBytes / alignment, err);
    //    }
    //

    // If prob type is rational, probabilities are either rational or uint64
    // if annotation forStates/forChoices/forBranches is set, values are set too
    // annotations in index are unique, and present as files
    // rewards are numeric, aps are boolean
    // annotations appliesTo is consistent with present files, annotation types are consistent (aps = bool, rewards = numeric)
    return isValid;
}

void validateOrThrow(storm::umb::UmbModel const& umbModel) {
    std::stringstream errors;
    if (!validate(umbModel, errors)) {
        STORM_LOG_THROW(false, storm::exceptions::WrongFormatException,
                        "UMB model " << umbModel.getShortModelInformation() << " is invalid:\n"
                                     << errors.str());
    }
}

}  // namespace storm::umb
