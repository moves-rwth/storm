#pragma once

#include <memory>

#include "storm/modelchecker/abstraction/AbstractAbstractionRefinementModelChecker.h"

namespace storm {
namespace models {
template<typename ValueType>
class Model;
}

namespace dd {
template<storm::dd::DdType DdType, typename ValueType, typename ExportValueType>
class BisimulationDecomposition;
}

namespace modelchecker {

template<typename ModelType>
class BisimulationAbstractionRefinementModelChecker : public AbstractAbstractionRefinementModelChecker<ModelType> {
   public:
    typedef typename ModelType::ValueType ValueType;
    static const storm::dd::DdType DdType = ModelType::DdType;

    /*!
     * Constructs a model checker for the given model.
     */
    explicit BisimulationAbstractionRefinementModelChecker(ModelType const& model);

    virtual ~BisimulationAbstractionRefinementModelChecker();

   protected:
    virtual bool supportsReachabilityRewards() const override;
    virtual std::string const& getName() const override;
    virtual void initializeAbstractionRefinement() override;
    virtual std::shared_ptr<storm::models::Model<ValueType>> getAbstractModel() override;
    virtual std::pair<std::unique_ptr<storm::abstraction::StateSet>, std::unique_ptr<storm::abstraction::StateSet>> getConstraintAndTargetStates(
        storm::models::Model<ValueType> const& abstractModel) override;
    virtual uint64_t getAbstractionPlayer() const override;
    virtual bool requiresSchedulerSynthesis() const override;
    virtual void refineAbstractModel() override;

   private:
    template<typename QuotientModelType>
    std::pair<storm::dd::Bdd<DdType>, storm::dd::Bdd<DdType>> getConstraintAndTargetStates(QuotientModelType const& quotient);

    ModelType const& model;

    /// The bisimulation object that maintains and refines the model.
    std::unique_ptr<storm::dd::BisimulationDecomposition<DdType, ValueType, ValueType>> bisimulation;

    /// Maintains the last abstract model that was returned.
    std::shared_ptr<storm::models::Model<ValueType>> lastAbstractModel;

    /// The name of the method.
    const static std::string name;
};

}  // namespace modelchecker
}  // namespace storm
