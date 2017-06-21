#include "storm/utility/Stopwatch.h"

namespace storm {
    namespace utility {
        
        Stopwatch::Stopwatch(bool startNow) : accumulatedTime(std::chrono::nanoseconds::zero()), stopped(true), startOfCurrentMeasurement(std::chrono::nanoseconds::zero()) {
            if (startNow) {
                start();
            }
        }
        
        Stopwatch::SecondType Stopwatch::getTimeInSeconds() const {
            return std::chrono::duration_cast<std::chrono::seconds>(accumulatedTime).count();
        }
        
        Stopwatch::MilisecondType Stopwatch::getTimeInMilliseconds() const {
            return std::chrono::duration_cast<std::chrono::milliseconds>(accumulatedTime).count();
        }
        
        Stopwatch::NanosecondType Stopwatch::getTimeInNanoseconds() const {
            return accumulatedTime.count();
        }
        
        void Stopwatch::addToTime(std::chrono::nanoseconds timeNanoseconds) {
            accumulatedTime += timeNanoseconds;
        }
        
        void Stopwatch::stop() {
            STORM_LOG_WARN_COND(!stopped, "Stopwatch is already paused.");
            stopped = true;
            accumulatedTime += std::chrono::high_resolution_clock::now() - startOfCurrentMeasurement;
        }
        
        void Stopwatch::start() {
            STORM_LOG_WARN_COND(stopped, "Stopwatch is already running.");
            stopped = false;
            startOfCurrentMeasurement = std::chrono::high_resolution_clock::now();
        }
        
        void Stopwatch::reset() {
            accumulatedTime = std::chrono::nanoseconds::zero();
            stopped = true;
        }
        
        std::ostream& operator<<(std::ostream& out, Stopwatch const& stopwatch) {
            char oldFillChar = out.fill('0');
            out << stopwatch.getTimeInSeconds() << "." << std::setw(3) << (stopwatch.getTimeInMilliseconds() % 1000) << "s";
            out.fill(oldFillChar);
            return out;
        }
        
    }
}
