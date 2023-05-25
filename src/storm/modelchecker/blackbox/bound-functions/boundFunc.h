//
// Created by Maximilian Kamps on 25.05.23.
//

#ifndef STORM_BOUNDFUNC_H
#define STORM_BOUNDFUNC_H

#include <utility>

template <typename ValueType>
class BoundFunc {
   public:
     virtual std::pair<ValueType,ValueType> INTERVAL(int totalSamples, int partialSample, double delta) = 0; //abstract method
};

#endif  // STORM_BOUNDFUNC_H
