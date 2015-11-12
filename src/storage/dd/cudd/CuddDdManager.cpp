#include <cmath>
#include <string>
#include <algorithm>

#include "src/storage/dd/cudd/CuddDdManager.h"
#include "src/utility/macros.h"
#include "src/storage/expressions/Variable.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/settings/SettingsManager.h"
#include "src/settings/modules/CuddSettings.h"
#include "src/storage/expressions/ExpressionManager.h"
#include "src/storage/dd/cudd/CuddAdd.h"
#include "CuddBdd.h"


namespace storm {
    namespace dd {


        
        

        

        
        

        

        
        void DdManager<DdType::CUDD>::allowDynamicReordering(bool value) {
            if (value) {
                this->getCuddManager().AutodynEnable(this->reorderingTechnique);
            } else {
                this->getCuddManager().AutodynDisable();
            }
        }
        
        bool DdManager<DdType::CUDD>::isDynamicReorderingAllowed() const {
            Cudd_ReorderingType type;
            return this->getCuddManager().ReorderingStatus(&type);
        }
        
        void DdManager<DdType::CUDD>::triggerReordering() {
            this->getCuddManager().ReduceHeap(this->reorderingTechnique, 0);
        }
        
        std::shared_ptr<DdManager<DdType::CUDD> const> DdManager<DdType::CUDD>::asSharedPointer() const {
            return this->shared_from_this();
        }
    }
}