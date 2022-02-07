// Copied and adapted from https://gitlab.lrde.epita.fr/spot

#ifndef STORM_TESTSPOT_H
#define STORM_TESTSPOT_H
#include <utility>
#include <vector>
#include "spot/misc/common.hh"
#include "spot/twa/fwd.hh"
#include "spot/twaalgos/powerset.hh"

namespace storm {
namespace spothelper {

using namespace spot;

typedef std::vector<std::pair<unsigned, unsigned>> product_states;

//twa_graph_ptr product(const const_twa_graph_ptr& left, const const_twa_graph_ptr& right);
void* product(const void* left, const void* right);

}  // namespace spothelper
}  // namespace storm
#endif  // STORM_TESTSPOT_H
