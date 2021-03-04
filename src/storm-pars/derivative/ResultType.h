#ifndef STORM_DERIVATIVERESULTTYPE_H
#define STORM_DERIVATIVERESULTTYPE_H
namespace storm {
	namespace derivative {
		/**
		* What the DerivativeEvaluationHelper should return the derivation for,
		* and what the GradientDescentInstantiationSearcher optimizes for.
		*/
		enum class ResultType {
			PROBABILITY, ///< The probability of eventually reaching the target.
			REWARD ///< The expected reward (only works if the probability of eventually reaching the target is one).
		};
	}
}
#endif
