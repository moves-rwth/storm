#pragma once

#include <string>

#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/environment/modelchecker/ModelCheckerEnvironment.h"
#include "storm/modelchecker/multiobjective/MultiObjectiveModelCheckingMethod.h"
#include "storm/storage/SchedulerClass.h"

namespace storm {

class MultiObjectiveModelCheckerEnvironment {
   public:
    enum class PrecisionType {
        Absolute,       /// Absolute precision
        RelativeToDiff  /// Relative to the difference between largest and smallest objective value(s)
    };

    enum class EncodingType {
        Auto,     /// Pick automatically
        Classic,  /// The classic backwards encoding
        Flow      /// The encoding as a flow network
    };

    MultiObjectiveModelCheckerEnvironment();
    ~MultiObjectiveModelCheckerEnvironment();

    storm::modelchecker::multiobjective::MultiObjectiveMethod const& getMethod() const;
    void setMethod(storm::modelchecker::multiobjective::MultiObjectiveMethod value);

    bool isExportPlotSet() const;
    boost::optional<std::string> getPlotPathUnderApproximation() const;
    void setPlotPathUnderApproximation(std::string const& path);
    void unsetPlotPathUnderApproximation();
    boost::optional<std::string> getPlotPathOverApproximation() const;
    void setPlotPathOverApproximation(std::string const& path);
    void unsetPlotPathOverApproximation();
    boost::optional<std::string> getPlotPathParetoPoints() const;
    void setPlotPathParetoPoints(std::string const& path);
    void unsetPlotPathParetoPoints();

    storm::RationalNumber const& getPrecision() const;
    void setPrecision(storm::RationalNumber const& value);
    PrecisionType const& getPrecisionType() const;
    void setPrecisionType(PrecisionType const& value);

    EncodingType const& getEncodingType() const;
    void setEncodingType(EncodingType const& value);

    bool isMaxStepsSet() const;
    uint64_t const& getMaxSteps() const;
    void setMaxSteps(uint64_t const& value);
    void unsetMaxSteps();

    bool isSchedulerRestrictionSet() const;
    storm::storage::SchedulerClass const& getSchedulerRestriction() const;
    void setSchedulerRestriction(storm::storage::SchedulerClass const& value);
    void unsetSchedulerRestriction();

    bool isPrintResultsSet() const;
    void setPrintResults(bool value);

    bool isLexicographicModelCheckingSet() const;
    void setLexicographicModelChecking(bool value);

   private:
    storm::modelchecker::multiobjective::MultiObjectiveMethod method;
    boost::optional<std::string> plotPathUnderApprox, plotPathOverApprox, plotPathParetoPoints;
    storm::RationalNumber precision;
    PrecisionType precisionType;
    EncodingType encodingType;
    boost::optional<uint64_t> maxSteps;
    boost::optional<storm::storage::SchedulerClass> schedulerRestriction;
    bool printResults;
    bool useLexicographicModelChecking;
};
}  // namespace storm
