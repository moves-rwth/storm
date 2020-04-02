#pragma once

#include "storm-dft/storage/dft/DFT.h"
#include "storm/storage/dd/DdManager.h"

namespace storm {
namespace transformations {
namespace dft {

template <storm::dd::DdType Type, typename ValueType>
class SftToBddTransformator {
   public:
    SftToBddTransformator(std::shared_ptr<storm::dd::DdManager<Type>> ddManager)
        : ddManager{ddManager} {}

    storm::dd::Bdd<Type> translate(
        std::shared_ptr<storm::storage::DFT<ValueType>> dft) {
        // create Variables for the BEs
        // auto basicElements = dft->getBasicElements();
        std::vector<storm::expressions::Variable> variables{};
        for (auto const& i : dft->getBasicElements()) {
            auto const& tmpVariables{ddManager->addMetaVariable(i->name(), 1)};
            variables.insert(variables.end(), tmpVariables.begin(),
                             tmpVariables.end());
        }

        return translate(dft->getTopLevelGate());
    }

   private:
    storm::dd::Bdd<Type> translate(
        std::shared_ptr<storm::storage::DFTElement<ValueType> const> element) {
        if (element->isGate()) {
            return translate(
                std::dynamic_pointer_cast<
                    storm::storage::DFTGate<ValueType> const>(element));
        } else if (element->isBasicElement()) {
            return translate(std::dynamic_pointer_cast<
                             storm::storage::DFTBE<ValueType> const>(element));
        } else {
            return ddManager->getBddZero();
        }
    }

    storm::dd::Bdd<Type> translate(
        std::shared_ptr<storm::storage::DFTGate<ValueType> const> gate) {
        if (gate->type() == storm::storage::DFTElementType::AND) {
            auto tmpBdd{ddManager->getBddOne()};
            for (const std::shared_ptr<
                     storm::storage::DFTElement<ValueType> const>& child :
                 gate->children()) {
                tmpBdd = tmpBdd && translate(child);
            }
            return tmpBdd;
        } else if (gate->type() == storm::storage::DFTElementType::OR) {
            auto tmpBdd{ddManager->getBddZero()};
            for (const std::shared_ptr<
                     storm::storage::DFTElement<ValueType> const>& child :
                 gate->children()) {
                tmpBdd = tmpBdd || translate(child);
            }
            return tmpBdd;
        } else {
            STORM_LOG_THROW(true, storm::exceptions::NotSupportedException,
                            "Gate not NotSupportedException");
        }
    }

    storm::dd::Bdd<Type> translate(
        std::shared_ptr<storm::storage::DFTBE<ValueType> const> const
            basicElement) {
        auto var = ddManager->getMetaVariable(basicElement->name());

        return ddManager->getEncoding(var, 1);
    }

    std::shared_ptr<storm::dd::DdManager<Type>> ddManager;
};

}  // namespace dft
}  // namespace transformations
}  // namespace storm
