#ifndef STORM_UTILITY_STOPWATCH_H_
#define STORM_UTILITY_STOPWATCH_H_

#include <chrono>
#include <math.h>

#include "src/utility/macros.h"

namespace storm {
    namespace utility {
        // A class that provides convenience operations to display run times.
        class Stopwatch {
        public:
            Stopwatch(double initialValueInSeconds = 0.0) : accumulatedSeconds(initialValueInSeconds), paused(false), startOfCurrentMeasurement(std::chrono::high_resolution_clock::now()) {
                // Intentionally left empty
            }
            
            ~Stopwatch() = default;
            
            double getAccumulatedSeconds() const {
                if(paused) {
                    return accumulatedSeconds;
                } else {
                    return accumulatedSeconds + std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startOfCurrentMeasurement).count();
                }
            }
            
            void pause() {
                if(paused) {
                    STORM_LOG_WARN("Tried to pause a stopwatch that was already paused.");
                } else {
                    accumulatedSeconds = getAccumulatedSeconds();
                    paused = true;
                }
            }
        
            void unpause() {
                if(paused) {
                    startOfCurrentMeasurement = std::chrono::high_resolution_clock::now();
                    paused = false;
                } else {
                    STORM_LOG_WARN("Tried to unpause a stopwatch that was not paused.");
                }
            }
            
            // Note: Does NOT unpause if stopwatch is currently paused.
            void reset() {
                accumulatedSeconds = 0.0;
                startOfCurrentMeasurement = std::chrono::high_resolution_clock::now();
            }

            friend std::ostream& operator<<(std::ostream& out, Stopwatch const& sw) {
                out << (round(sw.getAccumulatedSeconds()*1000)/1000);
                return out;
            }
            
            
        private:
            double accumulatedSeconds;
            bool paused;
            std::chrono::high_resolution_clock::time_point startOfCurrentMeasurement;
            
            
        };
    }
}

#endif /* STORM_UTILITY_STOPWATCH_H_ */
