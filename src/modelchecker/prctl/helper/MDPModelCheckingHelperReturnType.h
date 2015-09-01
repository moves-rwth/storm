#ifndef MDPMODELCHECKINGHELPERRETURNTYPE_H
#define	MDPMODELCHECKINGHELPERRETURNTYPE_H

#include <vector>
#include <memory>
#include "src/storage/PartialScheduler.h"

namespace storm {
    namespace storage {
        class BitVector;
    }
    
    
    namespace modelchecker {


        namespace helper {
            template<typename ValueType>
            struct MDPSparseModelCheckingHelperReturnType {
                MDPSparseModelCheckingHelperReturnType(MDPSparseModelCheckingHelperReturnType const&) = delete;
                MDPSparseModelCheckingHelperReturnType(MDPSparseModelCheckingHelperReturnType&&) = default;
                
                explicit MDPSparseModelCheckingHelperReturnType(std::vector<ValueType> && res) : result(std::move(res))
                {
                    
                }
                
                MDPSparseModelCheckingHelperReturnType(std::vector<ValueType> &&  res, std::unique_ptr<storm::storage::PartialScheduler> && pSched) :
                result(std::move(res)), partScheduler(std::move(pSched)) {}

                virtual ~MDPSparseModelCheckingHelperReturnType() { }
                
                
                std::vector<ValueType> result;
                std::unique_ptr<storm::storage::PartialScheduler> partScheduler;
            };
        }

    }
}


#endif	/* MDPMODELCHECKINGRETURNTYPE_H */

