// author: Roman Andriushchenko

#include "storm-synthesis/madp/Madp.h"

#include "storm-synthesis/madp/base/E.h"
#include "storm-synthesis/madp/base/POMDPDiscrete.h"
#include "storm-synthesis/madp/base/DecPOMDPDiscrete.h"
#include "storm-synthesis/madp/parser/MADPParser.h"

namespace storm {
    namespace synthesis {

        void test(std::string filename) {
            try {
                DecPOMDPDiscrete* pomdp = new DecPOMDPDiscrete("","",filename);
                pomdp->SetSparse(true);
                MADPParser parser(pomdp);
                std::cout << pomdp->SoftPrint() << std::endl;
            } catch(E& e){ e.Print(); }
        }

    } // namespace research
} // namespace storm

