// Copied and adapted from https://gitlab.lrde.epita.fr/spot

#ifndef STORM_TESTSPOT_H
#define STORM_TESTSPOT_H
#include "spot/misc/common.hh"
#include "spot/twa/fwd.hh"
#include "spot/twaalgos/powerset.hh"
#include <vector>
#include <utility>

namespace storm {
namespace spothelper {
unsigned int testFunc();
using namespace spot;

typedef std::vector<std::pair<unsigned, unsigned>> product_states;

twa_graph_ptr product(const const_twa_graph_ptr& left, const const_twa_graph_ptr& right, const output_aborter* aborter = nullptr);
}
}
#endif  // STORM_TESTSPOT_H
