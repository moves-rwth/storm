/*
 * read_lab_file.h
 *
 */

#pragma once

#include "src/models/atomic_propositions_labeling.h"


namespace mrmc {

namespace parser {

mrmc::models::AtomicPropositionsLabeling * readLabFile(int node_count, const char * filename);

}

}
