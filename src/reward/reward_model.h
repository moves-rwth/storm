/*
 * reward_model.h
 *
 *  Created on: 25.10.2012
 *      Author: Philipp Berger
 */

#ifndef MRMC_REWARD_REWARD_MODEL_H_
#define MRMC_REWARD_REWARD_MODEL_H_

#include <stdexcept>

#include "boost/integer/integer_mask.hpp"

namespace mrmc {

namespace reward {

/*! This class represents a single reward model with a type DataClass value for each state contained in a Vector of type VectorImpl
 */
template<template<class, class> class VectorImpl, class DataClass>
class RewardModel {

	//! Shorthand for a constant reference to the DataClass type
	typedef const DataClass& crDataClass;

	public:
		RewardModel(const uint_fast32_t state_count, const DataClass& null_value) : state_count(state_count), null_value(null_value) {

			this->reward_vector = new VectorImpl<DataClass, std::allocator<DataClass>>(this->state_count);
			// init everything to zero
			for (uint_fast32_t i = 0; i < this->state_count; ++i) {
				this->setReward(i, this->null_value);
			}
		}

		virtual ~RewardModel() {
			delete reward_vector;
		}

		bool setReward(const uint_fast32_t state_index, crDataClass reward) {
			if (state_index < this->state_count) {
				this->reward_vector->at(state_index) = reward;
				//	[state_index] = reward;
				return true;
			}
			return false;
		}

		crDataClass getReward(const uint_fast32_t state_index) {
			if (state_index < this->state_count) {
				return this->reward_vector->at(state_index);
			}
			return this->null_value;
		}

		bool resetReward(const uint_fast32_t state_index) {
			if (state_index < this->state_count) {
				this->reward_vector[state_index] = this->null_value;
				return true;
			}
			return false;
		}

		uint_fast32_t getSize() const {
			return this->state_count;
		}
	private:
		uint_fast32_t state_count;
		VectorImpl<DataClass, std::allocator<DataClass>>* reward_vector;
		const DataClass& null_value;
      
};

} //namespace reward

} //namespace mrmc

#endif /* MRMC_REWARD_REWARD_MODEL_H_ */
