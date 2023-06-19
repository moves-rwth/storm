#ifndef STORM_BLACKBOX_INTERFACE
#define STORM_BLACKBOX_INTERFACE

/*
 * This header defines the general interface expected from a blackbox MDP
 * 
*/
namespace storm {
namespace modelchecker {
namespace blackbox {

class blackboxMDP {
    public:
     typedef uint_fast64_t index_type;

     virtual void get_initial_state() = 0;
     virtual void get_avail_actions(index_type state) = 0;
     virtual index_type sample_suc(index_type state, index_type action) = 0;

     virtual bool is_greybox() = 0;
     virtual index_type get_suc_count(index_type state, index_type action);
};

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
#endif  // STORM_BLACKBOX_INTERFACE