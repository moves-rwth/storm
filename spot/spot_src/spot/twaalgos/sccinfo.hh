// -*- coding: utf-8 -*-
// Copyright (C) 2014-2020 Laboratoire de Recherche et Développement
// de l'Epita (LRDE).
//
// This file is part of Spot, a model checking library.
//
// Spot is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// Spot is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <vector>
#include <spot/twa/twagraph.hh>
#include <spot/twaalgos/emptiness.hh>

namespace spot
{
  class scc_info;

  /// @{
  /// \ingroup twa_misc
  /// \brief An edge_filter may be called on each edge to decide what
  /// to do with it.
  ///
  /// The edge filter is called with an edge and a destination.  (In
  /// existential automata the destination is already given by the
  /// edge, but in alternating automata, one edge may have several
  /// destinations, and in this case the filter will be called for
  /// each destination.)  The filter should return a value from
  /// edge_filter_choice.
  ///
  /// \c keep means to use the edge normally, as if no filter had
  /// been given.  \c ignore means to pretend the edge does not
  /// exist (if the destination is only reachable through this edge,
  /// it will not be visited). \c cut also ignores the edge, but
  /// it remembers to visit the destination state (as if it were an
  /// initial state) in case it is not reachable otherwise.
  ///
  /// Note that successors between SCCs can only be maintained for
  /// edges that are kept.  If some edges are ignored or cut, the
  /// SCC graph that you can explore with scc_info::initial() and
  /// scc_info::succ() will be restricted to the portion reachable
  /// with "keep" edges.  Additionally SCCs might be created when
  /// edges are cut, but those will not be reachable from
  /// scc_info::initial()..
  enum class edge_filter_choice { keep, ignore, cut };
  typedef edge_filter_choice
  (*edge_filter)(const twa_graph::edge_storage_t& e, unsigned dst,
                 void* filter_data);
  /// @}

  namespace internal
  {
    struct keep_all
    {
      template <typename Iterator>
      bool operator()(Iterator, Iterator) const noexcept
      {
        return true;
      }
    };

    // Keep only transitions that have at least one destination in the
    // current SCC.
    struct keep_inner_scc
    {
    private:
      const std::vector<unsigned>& sccof_;
      unsigned desired_scc_;
    public:
      keep_inner_scc(const std::vector<unsigned>& sccof, unsigned desired_scc)
        : sccof_(sccof), desired_scc_(desired_scc)
      {
      }

      template <typename Iterator>
      bool operator()(Iterator begin, Iterator end) const noexcept
      {
        bool want = false;
        while (begin != end)
          if (sccof_[*begin++] == desired_scc_)
            {
              want = true;
              break;
            }
        return want;
      }
    };

    template <typename Graph, typename Filter>
    class SPOT_API scc_edge_iterator
    {
    public:
      typedef typename std::conditional<std::is_const<Graph>::value,
                                        const typename Graph::edge_storage_t,
                                        typename Graph::edge_storage_t>::type
        value_type;
      typedef value_type& reference;
      typedef value_type* pointer;
      typedef std::ptrdiff_t difference_type;
      typedef std::forward_iterator_tag iterator_category;

      typedef std::vector<unsigned>::const_iterator state_iterator;

      typedef typename std::conditional<std::is_const<Graph>::value,
                                        const typename Graph::edge_vector_t,
                                        typename Graph::edge_vector_t>::type
        tv_t;

      typedef typename std::conditional<std::is_const<Graph>::value,
                                        const typename Graph::state_vector,
                                        typename Graph::state_vector>::type
        sv_t;
      typedef const typename Graph::dests_vector_t dv_t;
    protected:

      state_iterator pos_;
      state_iterator end_;
      unsigned t_;
      tv_t* tv_;
      sv_t* sv_;
      dv_t* dv_;

      Filter filt_;
      edge_filter efilter_;
      void* efilter_data_;


      void inc_state_maybe_()
      {
        while (!t_ && (++pos_ != end_))
          t_ = (*sv_)[*pos_].succ;
      }

      void inc_()
      {
        t_ = (*tv_)[t_].next_succ;
        inc_state_maybe_();
      }

      // Do we ignore the current transition?
      bool ignore_current()
      {
        unsigned dst = (*this)->dst;
        if ((int)dst >= 0)
          {
            // Non-universal branching => a single destination.
            if (!filt_(&(*this)->dst, 1 + &(*this)->dst))
              return true;
            if (efilter_)
              return efilter_((*tv_)[t_], dst, efilter_data_)
                != edge_filter_choice::keep;
            return false;
          }
        else
          {
            // Universal branching => multiple destinations.
            const unsigned* d = dv_->data() + ~dst;
            if (!filt_(d + 1, d + *d + 1))
              return true;
            if (efilter_)
              {
                // Keep the transition if at least one destination
                // is not filtered.
                const unsigned* end = d + *d + 1;
                for (const unsigned* i = d + 1; i != end; ++i)
                  {
                    if (efilter_((*tv_)[t_], *i, efilter_data_)
                        == edge_filter_choice::keep)
                      return false;
                    return true;
                  }
              }
            return false;
          }
      }

    public:
      scc_edge_iterator(state_iterator begin, state_iterator end,
                        tv_t* tv, sv_t* sv, dv_t* dv, Filter filt,
                        edge_filter efilter, void* efilter_data) noexcept
        : pos_(begin), end_(end), t_(0), tv_(tv), sv_(sv), dv_(dv), filt_(filt),
          efilter_(efilter), efilter_data_(efilter_data)
      {
        if (pos_ == end_)
          return;

        t_ = (*sv_)[*pos_].succ;
        inc_state_maybe_();
        while (pos_ != end_ && ignore_current())
          inc_();
      }

      scc_edge_iterator& operator++()
      {
        do
          inc_();
        while (pos_ != end_ && ignore_current());
        return *this;
      }

      scc_edge_iterator operator++(int)
      {
        scc_edge_iterator old = *this;
        ++*this;
        return old;
      }

      bool operator==(scc_edge_iterator o) const
      {
        return pos_ == o.pos_ && t_ == o.t_;
      }

      bool operator!=(scc_edge_iterator o) const
      {
        return pos_ != o.pos_ || t_ != o.t_;
      }

      reference operator*() const
      {
        return (*tv_)[t_];
      }

      pointer operator->() const
      {
        return &**this;
      }
    };


    template <typename Graph, typename Filter>
    class SPOT_API scc_edges
    {
    public:
      typedef scc_edge_iterator<Graph, Filter> iter_t;
      typedef typename iter_t::tv_t tv_t;
      typedef typename iter_t::sv_t sv_t;
      typedef typename iter_t::dv_t dv_t;
      typedef typename iter_t::state_iterator state_iterator;
    private:
      state_iterator begin_;
      state_iterator end_;
      tv_t* tv_;
      sv_t* sv_;
      dv_t* dv_;
      Filter filt_;
      edge_filter efilter_;
      void* efilter_data_;
    public:

      scc_edges(state_iterator begin, state_iterator end,
                tv_t* tv, sv_t* sv, dv_t* dv, Filter filt,
                edge_filter efilter, void* efilter_data) noexcept
        : begin_(begin), end_(end), tv_(tv), sv_(sv), dv_(dv), filt_(filt),
          efilter_(efilter), efilter_data_(efilter_data)
      {
      }

      iter_t begin() const
      {
        return {begin_, end_, tv_, sv_, dv_, filt_, efilter_, efilter_data_};
      }

      iter_t end() const
      {
        return {end_, end_, nullptr, nullptr, nullptr, filt_, nullptr, nullptr};
      }
    };
  }


  /// \ingroup twa_misc
  /// \brief Storage for SCC related information.
  class SPOT_API scc_info_node
  {
  public:
    typedef std::vector<unsigned> scc_succs;
    friend class scc_info;
  protected:
    scc_succs succ_;
    std::vector<unsigned> states_; // States of the component
    unsigned one_state_;
    acc_cond::mark_t acc_;
    acc_cond::mark_t common_;
    bool trivial_:1;
    bool accepting_:1;        // Necessarily accepting
    bool rejecting_:1;        // Necessarily rejecting
    bool useful_:1;
  public:
    scc_info_node() noexcept:
      acc_({}), trivial_(true), accepting_(false),
      rejecting_(false), useful_(false)
    {
    }

    scc_info_node(acc_cond::mark_t acc,
                  acc_cond::mark_t common, bool trivial) noexcept
      : acc_(acc), common_(common),
      trivial_(trivial), accepting_(false),
      rejecting_(false), useful_(false)
    {
    }

    bool is_trivial() const
    {
      return trivial_;
    }

    /// \brief True if we know that the SCC has an accepting cycle
    ///
    /// Note that both is_accepting() and is_rejecting() may return
    /// false if an SCC interesects a mix of Fin and Inf sets.
    /// Call determine_unknown_acceptance() to decide.
    bool is_accepting() const
    {
      return accepting_;
    }

    /// \brief True if we know that all cycles in the SCC are rejecting
    ///
    /// Note that both is_accepting() and is_rejecting() may return
    /// false if an SCC interesects a mix of Fin and Inf sets.
    /// Call determine_unknown_acceptance() to decide.
    bool is_rejecting() const
    {
      return rejecting_;
    }

    bool is_useful() const
    {
      return useful_;
    }

    acc_cond::mark_t acc_marks() const
    {
      return acc_;
    }

    acc_cond::mark_t common_marks() const
    {
      return common_;
    }

    const std::vector<unsigned>& states() const
    {
      return states_;
    }

    unsigned one_state() const
    {
      return one_state_;
    }

    const scc_succs& succ() const
    {
      return succ_;
    }
  };

  /// \ingroup twa_misc
  /// \brief Options to alter the behavior of scc_info
  enum class scc_info_options
  {
    /// Explore all SCCs, but do not track the states of each SCC and
    /// the successor SCC of each SCC.  This is enough to call the
    /// scc_of() method.
    NONE = 0,
    /// Stop exploring after an accepting SCC has been found.
    /// Using this option forbids future uses of is_useful_scc() and
    /// is_useful_state().  Using it will also cause the output of
    /// succ() to be incomplete.
    STOP_ON_ACC = 1,
    /// Keep a vector of all states belonging to each SCC.  Using this
    /// option is a precondition for using states_of(), edges_of(),
    /// inner_edges_of(), states_on_acc_cycle_of(), and
    /// determine_unknown_acceptance().
    TRACK_STATES = 2,
    /// Keep a list of successors of each SCCs.
    /// Using this option is a precondition for using succ(),
    /// is_useful_scc(), and is_useful_state().
    TRACK_SUCCS = 4,
    /// Conditionally track states if the acceptance conditions uses Fin.
    /// This is sufficiant for determine_unknown_acceptance().
    TRACK_STATES_IF_FIN_USED = 8,
    /// Default behavior: explore everything and track states and succs.
    ALL = TRACK_STATES | TRACK_SUCCS,
  };

  inline
  bool operator!(scc_info_options me)
  {
    return me == scc_info_options::NONE;
  }

  inline
  scc_info_options operator&(scc_info_options left, scc_info_options right)
  {
    typedef std::underlying_type_t<scc_info_options> ut;
    return static_cast<scc_info_options>(static_cast<ut>(left)
                                         & static_cast<ut>(right));
  }

  inline
  scc_info_options operator|(scc_info_options left, scc_info_options right)
  {
    typedef std::underlying_type_t<scc_info_options> ut;
    return static_cast<scc_info_options>(static_cast<ut>(left)
                                         | static_cast<ut>(right));
  }

  class SPOT_API scc_and_mark_filter;

  /// \ingroup twa_misc
  /// \brief Compute an SCC map and gather assorted information.
  ///
  /// This takes twa_graph as input and compute its SCCs.  This
  /// class maps all input states to their SCCs, and vice-versa.
  /// It allows iterating over all SCCs of the automaton, and checks
  /// their acceptance or non-acceptance.
  ///
  /// SCC are numbered in reverse topological order, i.e. the SCC of the
  /// initial state has the highest number, and if s1 is reachable from
  /// s2, then s1 < s2. Many algorithms depend on this property to
  /// determine in what order to iterate the SCCs.
  ///
  /// Additionally this class can be used on alternating automata, but
  /// in this case, universal transitions are handled like existential
  /// transitions.  It still makes sense to check which states belong
  /// to the same SCC, but the acceptance information computed by
  /// this class is meaningless.
  class SPOT_API scc_info
  {
  public:
    // scc_node used to be an inner class, but Swig 3.0.10 does not
    // support that yet.
    typedef scc_info_node scc_node;
    typedef scc_info_node::scc_succs scc_succs;

    // These types used to be defined here in Spot up to 2.9.
    typedef spot::edge_filter_choice edge_filter_choice;
    typedef spot::edge_filter edge_filter;

  protected:

    std::vector<unsigned> sccof_;
    std::vector<scc_node> node_;
    const_twa_graph_ptr aut_;
    unsigned initial_state_;
    edge_filter filter_;
    void* filter_data_;
    int one_acc_scc_ = -1;
    scc_info_options options_;

    // Update the useful_ bits.  Called automatically.
    void determine_usefulness();

    const scc_node& node(unsigned scc) const
    {
      return node_[scc];
    }

#ifndef SWIG
  private:
    [[noreturn]] static void report_need_track_states();
    [[noreturn]] static void report_need_track_succs();
    [[noreturn]] static void report_incompatible_stop_on_acc();
#endif

  public:
    /// @{
    /// \brief Create the scc_info map for \a aut
    scc_info(const_twa_graph_ptr aut,
             // Use ~0U instead of -1U to work around a bug in Swig.
             // See https://github.com/swig/swig/issues/993
             unsigned initial_state = ~0U,
             edge_filter filter = nullptr,
             void* filter_data = nullptr,
             scc_info_options options = scc_info_options::ALL);

    scc_info(const_twa_graph_ptr aut, scc_info_options options)
      : scc_info(aut, ~0U, nullptr, nullptr, options)
      {
      }
    /// @}

    /// @{
    /// \brief Create an scc_info map from some filter.
    ///
    /// This is usually used to prevent some edges from being
    /// considered as part of cycles, and can additionally restrict
    /// to exploration to some SCC discovered by another SCC.
    scc_info(const scc_and_mark_filter& filt, scc_info_options options);
    // we separate the two functions so that we can rename
    // scc_info(x,options) into scc_info_with_options(x,options) in Python.
    // Otherwrise calling scc_info(aut,options) can be confused with
    // scc_info(aut,initial_state).
    scc_info(const scc_and_mark_filter& filt)
    : scc_info(filt, scc_info_options::ALL)
    {
    }
    /// @}

    const_twa_graph_ptr get_aut() const
    {
      return aut_;
    }

    scc_info_options get_options() const
    {
      return options_;
    }

    edge_filter get_filter() const
    {
      return filter_;
    }

    const void* get_filter_data() const
    {
      return filter_data_;
    }

    unsigned scc_count() const
    {
      return node_.size();
    }

    /// \brief Return the number of one accepting SCC if any, -1 otherwise.
    ///
    /// If an accepting SCC has been found, return its number.
    /// Otherwise return -1.  Note that when the acceptance condition
    /// contains Fin, -1 does not implies that all SCCs are rejecting:
    /// it just means that no accepting SCC is known currently.  In
    /// that case, you might want to call
    /// determine_unknown_acceptance() first.
    int one_accepting_scc() const
    {
      return one_acc_scc_;
    }

    bool reachable_state(unsigned st) const
    {
      return scc_of(st) != -1U;
    }

    unsigned scc_of(unsigned st) const
    {
      return sccof_[st];
    }

    std::vector<scc_node>::const_iterator begin() const
    {
      return node_.begin();
    }

    std::vector<scc_node>::const_iterator end() const
    {
      return node_.end();
    }

    std::vector<scc_node>::const_iterator cbegin() const
    {
      return node_.cbegin();
    }

    std::vector<scc_node>::const_iterator cend() const
    {
      return node_.cend();
    }

    std::vector<scc_node>::const_reverse_iterator rbegin() const
    {
      return node_.rbegin();
    }

    std::vector<scc_node>::const_reverse_iterator rend() const
    {
      return node_.rend();
    }

    const std::vector<unsigned>& states_of(unsigned scc) const
    {
      if (SPOT_UNLIKELY(!(options_ & scc_info_options::TRACK_STATES)))
        report_need_track_states();
      return node(scc).states();
    }

    /// \brief A fake container to iterate over all edges leaving any
    /// state of an SCC.
    ///
    /// The difference with inner_edges_of() is that edges_of() include
    /// outgoing edges from all the states, even if they leave the SCC.
    internal::scc_edges<const twa_graph::graph_t, internal::keep_all>
    edges_of(unsigned scc) const
    {
      auto& states = states_of(scc);
      return {states.begin(), states.end(),
              &aut_->edge_vector(), &aut_->states(),
              &aut_->get_graph().dests_vector(),
              internal::keep_all(), filter_, const_cast<void*>(filter_data_)};
    }

    /// \brief A fake container to iterate over all edges between
    /// states of an SCC.
    ///
    /// The difference with edges_of() is that inner_edges_of()
    /// ignores edges leaving the SCC.  In the case of an
    /// alternating automaton, an edge is considered to be part of the
    /// SCC of one of its destination is in the SCC.
    internal::scc_edges<const twa_graph::graph_t, internal::keep_inner_scc>
    inner_edges_of(unsigned scc) const
    {
      auto& states = states_of(scc);
      return {states.begin(), states.end(),
              &aut_->edge_vector(), &aut_->states(),
              &aut_->get_graph().dests_vector(),
              internal::keep_inner_scc(sccof_, scc), filter_,
              const_cast<void*>(filter_data_)};
    }

    unsigned one_state_of(unsigned scc) const
    {
      return node(scc).one_state();
    }

    /// \brief Get number of the SCC containing the initial state.
    unsigned initial() const
    {
      SPOT_ASSERT(filter_ || scc_count() - 1 == scc_of(initial_state_));
      return scc_of(initial_state_);
    }

    const scc_succs& succ(unsigned scc) const
    {
      if (SPOT_UNLIKELY(!(options_ & scc_info_options::TRACK_SUCCS)))
        report_need_track_succs();
      return node(scc).succ();
    }

    bool is_trivial(unsigned scc) const
    {
      return node(scc).is_trivial();
    }

    SPOT_DEPRECATED("use acc_sets_of() instead")
    acc_cond::mark_t acc(unsigned scc) const
    {
      return acc_sets_of(scc);
    }

    bool is_accepting_scc(unsigned scc) const
    {
      return node(scc).is_accepting();
    }

    bool is_rejecting_scc(unsigned scc) const
    {
      return node(scc).is_rejecting();
    }

    /// \brief Study the SCCs that are currently reported neither as
    /// accepting nor as rejecting because of the presence of Fin sets
    ///
    /// This simply calls check_scc_emptiness() on undeterminate SCCs.
    void determine_unknown_acceptance();

    /// \brief Recompute whether an SCC is accepting or not.
    ///
    /// This is an internal function of
    /// determine_unknown_acceptance().
    bool check_scc_emptiness(unsigned n) const;

    /// \brief Retrieves an accepting run of the automaton whose cycle is in the
    /// SCC.
    ///
    /// \param scc an accepting scc
    /// \param r a run to fill
    ///
    /// This method needs the STOP_ON_ACC option.
    void get_accepting_run(unsigned scc, twa_run_ptr r) const;

    bool is_useful_scc(unsigned scc) const
    {
      if (SPOT_UNLIKELY(!!(options_ & scc_info_options::STOP_ON_ACC)))
        report_incompatible_stop_on_acc();
      if (SPOT_UNLIKELY(!(options_ & scc_info_options::TRACK_SUCCS)))
        report_need_track_succs();
      return node(scc).is_useful();
    }

    bool is_useful_state(unsigned st) const
    {
      return reachable_state(st) && is_useful_scc(scc_of(st));
    }

    /// \brief Returns, for each accepting SCC, the set of all marks appearing
    /// in it.
    std::vector<std::set<acc_cond::mark_t>> marks() const;
    std::set<acc_cond::mark_t> marks_of(unsigned scc) const;

    // Same as above, with old names.
    SPOT_DEPRECATED("use marks() instead")
    std::vector<std::set<acc_cond::mark_t>> used_acc() const
    {
      return marks();
    }
    SPOT_DEPRECATED("use marks_of() instead")
    std::set<acc_cond::mark_t> used_acc_of(unsigned scc) const
    {
      return marks_of(scc);
    }

    /// \brief Returns, for a given SCC, the set of all colors appearing in it.
    /// It is the set of colors that appear in some mark among those returned by
    /// marks_of().
    acc_cond::mark_t acc_sets_of(unsigned scc) const
    {
      return node(scc).acc_marks();
    }

    /// Returns, for a given SCC, the set of colors that appear on all of its
    /// transitions.
    acc_cond::mark_t common_sets_of(unsigned scc) const
    {
      return node(scc).common_marks();
    }

    std::vector<bool> weak_sccs() const;

    bdd scc_ap_support(unsigned scc) const;

    /// \brief Split an SCC into multiple automata separated by some
    /// acceptance sets.
    ///
    /// Pretend that the transitions of SCC \a scc that belong to any
    /// of the sets given in \a sets have been removed, and return a
    /// set of automata necessary to cover all remaining states.
    ///
    /// Set \a preserve_names to True if you want to keep the original
    /// name of each states for display.  (This is a bit slower.)
    std::vector<twa_graph_ptr> split_on_sets(unsigned scc,
                                             acc_cond::mark_t sets,
                                             bool preserve_names = false) const;
  protected:
    /// \brief: Recursive function used by states_on_acc_cycle_of().
    void
    states_on_acc_cycle_of_rec(unsigned scc,
                               acc_cond::mark_t all_fin,
                               acc_cond::mark_t all_inf,
                               unsigned nb_pairs,
                               std::vector<acc_cond::rs_pair>& pairs,
                               std::vector<unsigned>& res,
                               std::vector<unsigned>& old) const;
  public:
    /// \brief: Get all states visited by any accepting cycles of the 'scc'.
    ///
    /// Throws an exception if the automaton does not have a 'Streett-like'
    /// acceptance condition.
    std::vector<unsigned>
    states_on_acc_cycle_of(unsigned scc) const;
  };


  /// \brief Create a filter for SCC and marks.
  ///
  /// An scc_and_mark_filter can be passed to scc_info to explore only
  /// a specific SCC of the original automaton, and to prevent some
  /// acceptance sets from being considered as part of SCCs.
  class SPOT_API scc_and_mark_filter
  {
  protected:
    const scc_info* lower_si_;
    unsigned lower_scc_;
    acc_cond::mark_t cut_sets_;
    const_twa_graph_ptr aut_;
    acc_cond old_acc_;
    bool restore_old_acc_ = false;

    static scc_info::edge_filter_choice
    filter_scc_and_mark_(const twa_graph::edge_storage_t& e,
                         unsigned dst, void* data);
    static scc_info::edge_filter_choice
    filter_mark_(const twa_graph::edge_storage_t& e, unsigned, void* data);

  public:
    /// \brief Specify how to restrict scc_info to some SCC and acceptance sets
    ///
    /// \param lower_si the original scc_info that specifies the SCC
    /// \param lower_scc the SCC number in lower_si
    /// \param cut_sets the acceptance sets that should not be part of SCCs.
    scc_and_mark_filter(const scc_info& lower_si,
                        unsigned lower_scc,
                        acc_cond::mark_t cut_sets)
      : lower_si_(&lower_si), lower_scc_(lower_scc), cut_sets_(cut_sets),
        aut_(lower_si_->get_aut()), old_acc_(aut_->get_acceptance())
    {
      auto f = lower_si.get_filter();
      if (f == &filter_mark_ || f == &filter_scc_and_mark_)
        {
          const void* data = lower_si.get_filter_data();
          auto& d = *reinterpret_cast<const scc_and_mark_filter*>(data);
          cut_sets_ |= d.cut_sets_;
        }
    }

    /// \brief Specify how to restrict scc_info to some acceptance sets
    ///
    /// \param aut the automaton to filter
    /// \param cut_sets the acceptance sets that should not be part of SCCs.
    scc_and_mark_filter(const const_twa_graph_ptr& aut,
                        acc_cond::mark_t cut_sets)
      : lower_si_(nullptr), cut_sets_(cut_sets), aut_(aut),
        old_acc_(aut_->get_acceptance())
    {
    }

    ~scc_and_mark_filter()
    {
      restore_acceptance();
    }

    void override_acceptance(const acc_cond& new_acc)
    {
      std::const_pointer_cast<twa_graph>(aut_)->set_acceptance(new_acc);
      restore_old_acc_ = true;
    }

    void restore_acceptance()
    {
      if (!restore_old_acc_)
        return;
      std::const_pointer_cast<twa_graph>(aut_)->set_acceptance(old_acc_);
      restore_old_acc_ = false;
    }

    const_twa_graph_ptr get_aut() const
    {
      return aut_;
    }

    unsigned start_state() const
    {
      if (lower_si_)
        return lower_si_->one_state_of(lower_scc_);
      return aut_->get_init_state_number();
    }

    scc_info::edge_filter get_filter() const
    {
      if (lower_si_)
        return filter_scc_and_mark_;
      if (cut_sets_)
        return filter_mark_;
      return nullptr;
    }
  };


  /// \brief Dump the SCC graph of \a aut on \a out.
  ///
  /// If \a sccinfo is not given, it will be computed.
  SPOT_API std::ostream&
  dump_scc_info_dot(std::ostream& out,
                    const_twa_graph_ptr aut, scc_info* sccinfo = nullptr);

}
