/** 
 * @file:   CollectConstraints.h
 * @author: Sebastian Junges
 *
 * @since October 8, 2014
 */

#pragma once
#include "src/models/Dtmc.h"

namespace storm {
namespace modelchecker {
	namespace reachability {
		template<typename ValueType>
		class CollectConstraints
		{
			private:
				std::unordered_set<carl::Constraint<ValueType>> wellformedConstraintSet;
				std::unordered_set<carl::Constraint<ValueType>> graphPreservingConstraintSet;
            storm::utility::ConstantsComparator<ValueType> comparator;

			public:
				std::unordered_set<carl::Constraint<ValueType>> const&  wellformedConstraints() const {
					return this->wellformedConstraintSet;
				}
				
				std::unordered_set<carl::Constraint<ValueType>> const&  graphPreservingConstraints() const {
					return this->graphPreservingConstraintSet;
				}
				
				void process(storm::models::Dtmc<ValueType> const& dtmc) 
				{
					for(uint_fast64_t state = 0; state < dtmc.getNumberOfStates(); ++state)
					{
						ValueType sum;
						assert(comparator.isZero(sum));
						for(auto const& transition : dtmc.getRows(state))
						{
							sum += transition.getValue();
							if(!transition.getValue().isConstant())
							{
								wellformedConstraintSet.emplace(transition.getValue() - 1, storm::CompareRelation::LEQ);
								wellformedConstraintSet.emplace(transition.getValue(), storm::CompareRelation::GEQ);
								graphPreservingConstraintSet.emplace(transition.getValue(), storm::CompareRelation::GT);
							}
						}
						assert(!comparator.isConstant(sum) || comparator.isOne(sum));
						if(!sum.isConstant()) {
							wellformedConstraintSet.emplace(sum - 1, storm::CompareRelation::EQ);
						}

					}
				}

				void operator()(storm::models::Dtmc<ValueType> const& dtmc) 
				{
					process(dtmc);
				}

		};	
	}
}
}
