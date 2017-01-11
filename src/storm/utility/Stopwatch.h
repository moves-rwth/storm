#ifndef STORM_UTILITY_STOPWATCH_H_
#define STORM_UTILITY_STOPWATCH_H_

#include <chrono>

#include "storm/utility/macros.h"

namespace storm {
    namespace utility {

        /*!
         * A class that provides convenience operations to display run times.
         */
        class Stopwatch {
        public:

            /*!
             * Constructor.
             *
             * @param startNow If true, the stopwatch starts right away.
             */
            Stopwatch(bool startNow = false) : accumulatedTime(std::chrono::nanoseconds::zero()), stopped(true), startOfCurrentMeasurement(std::chrono::nanoseconds::zero()) {
                if (startNow) {
                    start();
                }
            }

            /*!
             * Destructor.
             */
            ~Stopwatch() = default;

            /*!
             * Get measured time in seconds.
             *
             * @return seconds as floating point number.
             */
            double getTimeSeconds() const {
                return std::chrono::duration<float>(accumulatedTime).count();
            }

            /*!
             * Get measured time in milliseconds.
             *
             * @return Milliseconds.
             */
            unsigned long long int getTimeMilliseconds() const {
                return std::chrono::duration_cast<std::chrono::milliseconds>(accumulatedTime).count();
            }

            /*!
             * Get measured time in nanoseconds.
             *
             * @return Nanoseconds.
             */
            unsigned long long int getTimeNanoseconds() const {
                return accumulatedTime.count();
            }

            /*!
             * Add given time to measured time.
             *
             * @param timeNanoseconds Additional time in nanoseconds.
             */
            void addToTime(std::chrono::nanoseconds timeNanoseconds) {
                accumulatedTime += timeNanoseconds;
            }

            /*!
             * Stop stopwatch and add measured time to total time.
             */
            void stop() {
                if (stopped) {
                    // Assertions are only available in DEBUG build and therefore not used here.
                    STORM_LOG_WARN("Stopwatch is already paused.");
                }
                stopped = true;
                accumulatedTime += std::chrono::high_resolution_clock::now() - startOfCurrentMeasurement;
            }

            /*!
             * Start stopwatch (again) and start measuring time.
             */
            void start() {
                if (!stopped) {
                    // Assertions are only available in DEBUG build and therefore not used here.
                    STORM_LOG_WARN("Stopwatch is already running.");
                }
                stopped = false;
                startOfCurrentMeasurement = std::chrono::high_resolution_clock::now();
            }
            
            /*!
             * Reset the stopwatch. Reset the measured time to zero and stop the stopwatch.
             */
            void reset() {
                accumulatedTime = std::chrono::nanoseconds::zero();
                stopped = true;
            }

            friend std::ostream& operator<<(std::ostream& out, Stopwatch const& stopwatch) {
                out << stopwatch.getTimeSeconds();
                return out;
            }
            
            
        private:
            // Total measured time
            std::chrono::nanoseconds accumulatedTime;
            // Flag indicating if the stopwatch is stopped right now.
            bool stopped;
            // Timepoint when the stopwatch was started the last time.
            std::chrono::high_resolution_clock::time_point startOfCurrentMeasurement;
            
        };
    }
}

#endif /* STORM_UTILITY_STOPWATCH_H_ */
