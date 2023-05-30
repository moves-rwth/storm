#include <utility>


/*!
 * Abstract Base Class for the function INTERVAL
 */
template <typename ValueType>
class boundFunc {
   public:
    /*!
     * Calculates the lower and upper bound for a transition for the eMDP
     * @param totalSamples : total Samples for one Action generated in Simulate
     * @param partialSample : Samples for (state,transition,state) generated in Simulate
     * @param delta : Inconvidence Value delta
     * @return : upper and lower bound
     */
    virtual std::pair<ValueType,ValueType> INTERVAL(int totalSamples, int partialSample, double delta) = 0; //abstract method
};

