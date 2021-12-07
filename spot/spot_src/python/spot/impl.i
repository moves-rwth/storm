// -*- coding: utf-8 -*-
// Copyright (C) 2009-2019 Laboratoire de Recherche et Développement
// de l'Epita (LRDE).
// Copyright (C) 2003-2006 Laboratoire d'Informatique de Paris 6
// (LIP6), département Systèmes Répartis Coopératifs (SRC), Université
// Pierre et Marie Curie.
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

%module(package="spot", director="1") impl

%include "std_shared_ptr.i"
%include "std_vector.i"
%include "std_string.i"
%include "std_list.i"
%include "std_set.i"
%include "std_map.i"
%include "std_pair.i"
%include "stdint.i"
%include "exception.i"
%include "typemaps.i"

 // git grep 'typedef.*std::shared_ptr' | grep -v const |
 //   sed 's/.*<\(.*\)>.*/%shared_ptr(spot::\1)/g'
%shared_ptr(spot::dstar_aut)
%shared_ptr(spot::parsed_aut)
%shared_ptr(spot::fair_kripke)
%shared_ptr(spot::kripke)
%shared_ptr(spot::kripke_graph)
%shared_ptr(spot::ta)
%shared_ptr(spot::ta_explicit)
%shared_ptr(spot::ta_product)
%shared_ptr(spot::tgta)
%shared_ptr(spot::tgta_explicit)
%shared_ptr(spot::bdd_dict)
%shared_ptr(spot::twa)
%shared_ptr(spot::twa_graph)
%shared_ptr(spot::twa_product)
%shared_ptr(spot::twa_product_init)
%shared_ptr(spot::taa_tgba)
%shared_ptr(spot::taa_tgba_string)
%shared_ptr(spot::taa_tgba_formula)
%shared_ptr(spot::twa_run)
%shared_ptr(spot::twa_word)
%shared_ptr(spot::twa_univ_remover)
%shared_ptr(spot::emptiness_check_result)
%shared_ptr(spot::emptiness_check)
%shared_ptr(spot::emptiness_check_instantiator)
%shared_ptr(spot::tgbasl)

%import "buddy.i"

%{
#include <iostream>
#include <fstream>
#include <sstream>
#include <signal.h>

#include <spot/misc/common.hh>
#include <spot/misc/version.hh>
#include <spot/misc/minato.hh>
#include <spot/misc/optionmap.hh>
#include <spot/misc/random.hh>
#include <spot/misc/escape.hh>
#include <spot/misc/trival.hh>

#include <spot/tl/formula.hh>

#include <spot/tl/environment.hh>
#include <spot/tl/declenv.hh>
#include <spot/tl/defaultenv.hh>

#include <spot/tl/parse.hh>

#include <spot/twa/bdddict.hh>

#include <spot/tl/apcollect.hh>
#include <spot/tl/contain.hh>
#include <spot/tl/dot.hh>
#include <spot/tl/nenoform.hh>
#include <spot/tl/print.hh>
#include <spot/tl/simplify.hh>
#include <spot/tl/unabbrev.hh>
#include <spot/tl/randomltl.hh>
#include <spot/tl/length.hh>
#include <spot/tl/ltlf.hh>
#include <spot/tl/hierarchy.hh>
#include <spot/tl/remove_x.hh>
#include <spot/tl/relabel.hh>

#include <spot/twa/bddprint.hh>
#include <spot/twa/formula2bdd.hh>
#include <spot/twa/fwd.hh>
#include <spot/twa/acc.hh>
#include <spot/twa/twa.hh>
#include <spot/twa/taatgba.hh>
#include <spot/twa/twaproduct.hh>

#include <spot/twaalgos/alternation.hh>
#include <spot/twaalgos/cleanacc.hh>
#include <spot/twaalgos/degen.hh>
#include <spot/twaalgos/dot.hh>
#include <spot/twaalgos/dualize.hh>
#include <spot/twaalgos/cobuchi.hh>
#include <spot/twaalgos/copy.hh>
#include <spot/twaalgos/complete.hh>
#include <spot/twaalgos/complement.hh>
#include <spot/twaalgos/emptiness.hh>
#include <spot/twaalgos/gtec/gtec.hh>
#include <spot/twaalgos/genem.hh>
#include <spot/twaalgos/lbtt.hh>
#include <spot/twaalgos/ltl2taa.hh>
#include <spot/twaalgos/ltl2tgba_fm.hh>
#include <spot/twaalgos/gfguarantee.hh>
#include <spot/twaalgos/compsusp.hh>
#include <spot/twaalgos/contains.hh>
#include <spot/twaalgos/determinize.hh>
#include <spot/twaalgos/magic.hh>
#include <spot/twaalgos/minimize.hh>
#include <spot/twaalgos/neverclaim.hh>
#include <spot/twaalgos/randomize.hh>
#include <spot/twaalgos/remfin.hh>
#include <spot/twaalgos/remprop.hh>
#include <spot/twaalgos/totgba.hh>
#include <spot/twaalgos/sbacc.hh>
#include <spot/twaalgos/sccinfo.hh>
#include <spot/twaalgos/strength.hh>
#include <spot/twaalgos/sccfilter.hh>
#include <spot/twaalgos/stats.hh>
#include <spot/twaalgos/isdet.hh>
#include <spot/twaalgos/iscolored.hh>
#include <spot/twaalgos/isunamb.hh>
#include <spot/twaalgos/isweakscc.hh>
#include <spot/twaalgos/langmap.hh>
#include <spot/twaalgos/simulation.hh>
#include <spot/twaalgos/split.hh>
#include <spot/twaalgos/sum.hh>
#include <spot/twaalgos/parity.hh>
#include <spot/twaalgos/postproc.hh>
#include <spot/twaalgos/powerset.hh>
#include <spot/twaalgos/product.hh>
#include <spot/twaalgos/stutter.hh>
#include <spot/twaalgos/translate.hh>
#include <spot/twaalgos/toweak.hh>
#include <spot/twaalgos/hoa.hh>
#include <spot/twaalgos/dtwasat.hh>
#include <spot/twaalgos/relabel.hh>
#include <spot/twaalgos/word.hh>
#include <spot/twaalgos/are_isomorphic.hh>
#include <spot/twaalgos/toparity.hh>

#include <spot/parseaut/public.hh>

#include <spot/kripke/fairkripke.hh>
#include <spot/kripke/kripke.hh>
#include <spot/kripke/kripkegraph.hh>

#include <spot/ta/ta.hh>
#include <spot/ta/tgta.hh>
#include <spot/ta/taexplicit.hh>
#include <spot/ta/tgtaexplicit.hh>
#include <spot/taalgos/tgba2ta.hh>
#include <spot/taalgos/dot.hh>
#include <spot/taalgos/stats.hh>
#include <spot/taalgos/minimize.hh>

using namespace spot;
%}


// Swig come with iterators that implement a decrement method.  This
// is not supported in our "successor" iterators.  Also we want
// iterators to return pointers so that data can be modified during
// iteration.
%fragment("ForwardIterator_T","header",fragment="SwigPyIterator_T") {
namespace swig
{
  template<typename OutIterator,
	   typename ValueType =
	   typename std::iterator_traits<OutIterator>::pointer,
	   typename FromOper = from_oper<ValueType> >
  class ForwardIterator_T :  public SwigPyIterator_T<OutIterator>
  {
  public:
    FromOper from;
    typedef OutIterator out_iterator;
    typedef ValueType value_type;
    typedef SwigPyIterator_T<out_iterator>  base;

    ForwardIterator_T(out_iterator curr, out_iterator first,
		      out_iterator last, PyObject *seq)
      : SwigPyIterator_T<OutIterator>(curr, seq), begin(first), end(last)
    {
    }

    PyObject *value() const {
      if (base::current == end) {
	throw stop_iteration();
      } else {
	return from(static_cast<value_type>(&*(base::current)));
      }
    }

    SwigPyIterator *copy() const
    {
      return new ForwardIterator_T(*this);
    }

    SwigPyIterator *incr(size_t n = 1)
    {
      while (n--) {
	if (base::current == end) {
	  throw stop_iteration();
	} else {
	  ++base::current;
	}
      }
      return this;
    }

  protected:
    out_iterator begin;
    out_iterator end;
  };


  template<typename OutIter>
  inline SwigPyIterator*
  make_forward_iterator(const OutIter& current,
			const OutIter& begin,
			const OutIter& end, PyObject *seq = 0)
  {
    return new ForwardIterator_T<OutIter>(current, begin, end, seq);
  }

  // Likewise, but without returning a pointer.
  template<typename OutIterator,
	   typename ValueType =
	   typename std::iterator_traits<OutIterator>::value_type,
	   typename FromOper = from_oper<ValueType> >
  class ForwardIterator_T_NP :  public SwigPyIterator_T<OutIterator>
  {
  public:
    FromOper from;
    typedef OutIterator out_iterator;
    typedef ValueType value_type;
    typedef SwigPyIterator_T<out_iterator>  base;

    ForwardIterator_T_NP(out_iterator curr, out_iterator first,
                         out_iterator last, PyObject *seq)
      : SwigPyIterator_T<OutIterator>(curr, seq), begin(first), end(last)
    {
    }

    PyObject *value() const {
      if (base::current == end) {
	throw stop_iteration();
      } else {
	return from(static_cast<value_type>(*(base::current)));
      }
    }

    SwigPyIterator *copy() const
    {
      return new ForwardIterator_T_NP(*this);
    }

    SwigPyIterator *incr(size_t n = 1)
    {
      while (n--) {
	if (base::current == end) {
	  throw stop_iteration();
	} else {
	  ++base::current;
	}
      }
      return this;
    }

  protected:
    out_iterator begin;
    out_iterator end;
  };

  template<typename OutIter>
  inline SwigPyIterator*
  make_forward_iterator_np(const OutIter& current,
			   const OutIter& begin,
			   const OutIter& end, PyObject *seq = 0)
  {
    return new ForwardIterator_T_NP<OutIter>(current, begin, end, seq);
  }
}
}
%fragment("ForwardIterator_T");

// For spot::emptiness_check_instantiator::construct and any other
// function that return errors via a "char **err" argument.
%typemap(in, numinputs=0) char** OUTPUT (char* temp) {
  $1 = &temp;
}
%typemap(argout) char** OUTPUT {
  PyObject *obj = SWIG_FromCharPtr(*$1);
  if (!$result) {
    $result = obj;
  //# If the function returns null_ptr (i.e. Py_None), we
  //# don't want to override it with OUTPUT as in the
  //# default implementation of t_output_helper.
  // } else if ($result == Py_None) {
  //   Py_DECREF($result);
  //   $result = obj;
  } else {
    if (!PyList_Check($result)) {
      PyObject *o2 = $result;
      $result = PyList_New(1);
      PyList_SetItem($result, 0, o2);
    }
    PyList_Append($result, obj);
    Py_DECREF(obj);
  }

 };
%apply char** OUTPUT { char** err };

// This is mainly for acc_cond::is_parity()
%typemap(in, numinputs=0) bool& (bool temp) {
  $1 = &temp;
 };
%typemap(argout) bool& {
  PyObject *obj = SWIG_From_bool(*$1);
  if (!$result) {
    $result = obj;
  } else {
    if (!PyList_Check($result)) {
      PyObject *o2 = $result;
      $result = PyList_New(1);
      PyList_SetItem($result, 0, o2);
    }
    PyList_Append($result, obj);
    Py_DECREF(obj);
  }
 };

// Allow None to be passed for formula.  Some functions like
// postprocessor::run() take an optional formula that defaults to
// nullptr.  So it make sense to have function that take formulas that
// default to None on the Python side.
%typemap(in) spot::formula {
    void *tmp;
    int res = SWIG_ConvertPtr($input, &tmp, $descriptor(spot::formula*),
                              SWIG_POINTER_IMPLICIT_CONV);
    if (!SWIG_IsOK(res)) {
      %argument_fail(res, "spot::formula", $symname, $argnum);
    }
    if (tmp) {
      spot::formula* temp = reinterpret_cast< spot::formula * >(tmp);
      $1 = *temp;
      if (SWIG_IsNewObj(res)) delete temp;
    }
    // if tmp == nullptr, then the default value of $1 is fine.
}

%typemap(typecheck, precedence=2000) spot::formula {
    $1 = SWIG_CheckState(SWIG_ConvertPtr($input, nullptr,
					 $descriptor(spot::formula*),
                                         SWIG_POINTER_IMPLICIT_CONV));
}

// Access to stucture members, such as spot::parsed_formula::f are done via
// the pointer typemap.  We want to enforce a copy.
%typemap(out) spot::formula* {
  if (!*$1)
    $result = SWIG_Py_Void();
  else
    $result = SWIG_NewPointerObj(new spot::formula(*$1), $descriptor(spot::formula*), SWIG_POINTER_OWN);
}

%typemap(out) spot::formula {
  if (!$1)
    $result = SWIG_Py_Void();
  else
    $result = SWIG_NewPointerObj(new spot::formula($1), $descriptor(spot::formula*), SWIG_POINTER_OWN);
}

%typemap(out) std::string* {
  if (!$1)
    $result = SWIG_Py_Void();
  else
    $result = SWIG_FromCharPtr($1->c_str());
}

%{
// This function is called whenever an exception has been caught.
// Doing all the conversion in a separate function (rather than
// in the %exception statement) saves a lot of space.
//
// The technique is also called "Lippincott function".
static void handle_any_exception()
{
  try {
    throw;
  }
  catch (const spot::parse_error& e)
  {
    std::string er("\n");
    er += e.what();
    SWIG_Error(SWIG_SyntaxError, er.c_str());
  }
  catch (const std::invalid_argument& e)
  {
    SWIG_Error(SWIG_ValueError, e.what());
  }
  catch (const std::runtime_error& e)
  {
    SWIG_Error(SWIG_RuntimeError, e.what());
  }
  catch (const std::out_of_range& e)
  {
    SWIG_Error(SWIG_IndexError, e.what());
  }
}
%}

%exception {
  try {
    $action
  }
  catch (...) {
    handle_any_exception();
    SWIG_fail;
  }
}

%include <spot/misc/common.hh>
%include <spot/misc/version.hh>
%include <spot/misc/minato.hh>
%include <spot/misc/optionmap.hh>
%include <spot/misc/random.hh>
%include <spot/misc/escape.hh>

%implicitconv spot::trival;
%include <spot/misc/trival.hh>

%implicitconv std::vector<spot::formula>;
%implicitconv spot::formula;

%include <spot/tl/formula.hh>

namespace std {
  %template(liststr) list<std::string>;
  %template(pairunsigned) pair<unsigned, unsigned>;
  %template(pairmark_t) pair<spot::acc_cond::mark_t, spot::acc_cond::mark_t>;
  %template(vectorformula) vector<spot::formula>;
  %template(vectorunsigned) vector<unsigned>;
  %template(vectorpairunsigned) vector<pair<unsigned, unsigned>>;
  %template(vectoracccond) vector<spot::acc_cond>;
  %template(vectoracccode) vector<spot::acc_cond::acc_code>;
  %template(vectorbool) vector<bool>;
  %template(vectorbdd) vector<bdd>;
  %template(vectorstring) vector<string>;
  %template(atomic_prop_set) set<spot::formula>;
  %template(relabeling_map) map<spot::formula, spot::formula>;
}

%include <spot/tl/environment.hh>
%include <spot/tl/declenv.hh>
%include <spot/tl/defaultenv.hh>

%include <spot/tl/parse.hh>

 /* these must come before apcollect.hh */
%include <spot/twa/bdddict.hh>
%include <spot/twa/bddprint.hh>
%include <spot/twa/formula2bdd.hh>
%include <spot/twa/fwd.hh>
 /* These operators may raise exceptions, and we do not
    want Swig4 to convert those exceptions to NotImplemented. */
%nopythonmaybecall spot::acc_cond::mark_t::operator<<;
%nopythonmaybecall spot::acc_cond::mark_t::operator>>;
%implicitconv spot::acc_cond::mark_t;
%implicitconv spot::acc_cond::acc_code;
%feature("flatnested") spot::acc_cond::mark_t;
%feature("flatnested") spot::acc_cond::acc_code;
%feature("flatnested") spot::acc_cond::rs_pair;
%apply bool* OUTPUT {bool& max, bool& odd};
namespace std {
  %template(vector_rs_pair) vector<spot::acc_cond::rs_pair>;
}
%apply std::vector<unsigned> &OUTPUT {std::vector<unsigned>& pairs}
%apply std::vector<spot::acc_cond::rs_pair> &OUTPUT {std::vector<spot::acc_cond::rs_pair>& pairs}
%include <spot/twa/acc.hh>
%template(pair_bool_mark) std::pair<bool, spot::acc_cond::mark_t>;

%pythonprepend spot::twa::prop_deterministic %{
  from warnings import warn
  warn("use prop_universal() instead of prop_deterministic()",
       DeprecationWarning)
%}

// Must occur before the twa declaration
%typemap(out) SWIGTYPE* spot::twa::get_product_states %{
  if (!$1)
    $result = SWIG_Py_Void();
  else
    {
      unsigned sz = $1->size();
      $result = PyList_New(sz);
      for (unsigned i = 0; i < sz; ++i)
        PyList_SetItem($result, i, swig::from((*$1)[i]));
    }
%}

%include <spot/twa/twa.hh>

%include <spot/tl/apcollect.hh>
%include <spot/tl/contain.hh>
%include <spot/tl/dot.hh>
%include <spot/tl/nenoform.hh>
%include <spot/tl/print.hh>
%include <spot/tl/simplify.hh>
%include <spot/tl/unabbrev.hh>
%include <spot/tl/randomltl.hh>
%include <spot/tl/length.hh>
%include <spot/tl/ltlf.hh>
%include <spot/tl/hierarchy.hh>
%include <spot/tl/remove_x.hh>
%include <spot/tl/relabel.hh>

%include <spot/twa/taatgba.hh>
%include <spot/twa/twaproduct.hh>

%include <spot/graph/graph.hh>
%nodefaultctor spot::digraph;
%nodefaultctor spot::internal::state_out;
%nodefaultctor spot::internal::killer_edge_iterator;
%nodefaultctor spot::internal::all_trans;
%nodefaultctor spot::internal::universal_dests;
%traits_swigtype(spot::internal::distate_storage<unsigned int, spot::internal::boxed_label<spot::kripke_graph_state, false> >);
%fragment(SWIG_Traits_frag(spot::internal::distate_storage<unsigned int, spot::internal::boxed_label<spot::kripke_graph_state, false> >));
%traits_swigtype(spot::internal::edge_storage<unsigned int, unsigned int, unsigned int, spot::internal::boxed_label<spot::twa_graph_edge_data, false> >);
%fragment(SWIG_Traits_frag(spot::internal::edge_storage<unsigned int, unsigned int, unsigned int, spot::internal::boxed_label<spot::twa_graph_edge_data, false> >));
%traits_swigtype(spot::internal::edge_storage<unsigned int, unsigned int, unsigned int, spot::internal::boxed_label<void, true> >);
%fragment(SWIG_Traits_frag(spot::internal::edge_storage<unsigned int, unsigned int, unsigned int, spot::internal::boxed_label<void, true> >));

%typemap(out, optimal="1") spot::internal::all_trans<spot::digraph<spot::twa_graph_state, spot::twa_graph_edge_data>> {
  $result = SWIG_NewPointerObj(new $1_ltype($1), $&1_descriptor,
			       SWIG_POINTER_OWN);
}

%typemap(out, optimal="1") spot::internal::all_trans<spot::digraph<spot::kripke_graph_state, void>> {
  $result = SWIG_NewPointerObj(new $1_ltype($1), $&1_descriptor,
			       SWIG_POINTER_OWN);
}

%typemap(out, optimal="1") spot::internal::const_universal_dests {
  $result = SWIG_NewPointerObj(new $1_ltype($1), $&1_descriptor,
                               SWIG_POINTER_OWN);
}

%noexception spot::kripke_graph::edges;
%noexception spot::twa_graph::edges;
%noexception spot::twa_graph::univ_dests;


// Instead of %feature("shadow") we would like to use just
//   %pythonprepend spot::twa_graph::out %{ self.report_univ_dest(src) %}
// However Swig 3.0.2 (from Debian stable) names the argument "*args"
// while Swig 3.0.10 uses "src", and we want to be compatible with both.
%feature("shadow") spot::twa_graph::out %{
def out(self, src: 'unsigned int'):
    self.report_univ_dest(src)
    return $action(self, src)
%}
%feature("shadow") spot::twa_graph::state_from_number %{
def state_from_number(self, src: 'unsigned int') -> "spot::twa_graph_state const *":
    self.report_univ_dest(src)
    return $action(self, src)
%}
%feature("shadow") spot::twa_graph::state_acc_sets %{
def state_acc_sets(self, src: 'unsigned int') -> "spot::acc_cond::mark_t":
    self.report_univ_dest(src)
    return $action(self, src)
%}
%feature("shadow") spot::twa_graph::state_is_accepting %{
def state_is_accepting(self, src) -> "bool":
    if type(src) == int:
        self.report_univ_dest(src)
    return $action(self, src)
%}

%include <spot/twa/twagraph.hh>
%template(twa_graph_state_out) spot::internal::state_out<spot::digraph<spot::twa_graph_state, spot::twa_graph_edge_data>>;
%template(twa_graph_killer_edge_iterator) spot::internal::killer_edge_iterator<spot::digraph<spot::twa_graph_state, spot::twa_graph_edge_data>>;
%template(twa_graph_all_trans) spot::internal::all_trans<spot::digraph<spot::twa_graph_state, spot::twa_graph_edge_data>>;
%template(twa_graph_edge_boxed_data) spot::internal::boxed_label<spot::twa_graph_edge_data, false>;
%template(twa_graph_edge_storage) spot::internal::edge_storage<unsigned int, unsigned int, unsigned int, spot::internal::boxed_label<spot::twa_graph_edge_data, false> >;

// Should come after the definition of twa_graph

%include <spot/twaalgos/alternation.hh>
%include <spot/twaalgos/cleanacc.hh>
%include <spot/twaalgos/degen.hh>
%include <spot/twaalgos/dot.hh>
%include <spot/twaalgos/cobuchi.hh>
%include <spot/twaalgos/copy.hh>
%include <spot/twaalgos/complete.hh>
%feature("flatnested") spot::twa_run::step;
%include <spot/twaalgos/emptiness.hh>
%template(list_step) std::list<spot::twa_run::step>;
%include <spot/twaalgos/gtec/gtec.hh>
%include <spot/twaalgos/genem.hh>
%include <spot/twaalgos/lbtt.hh>
%include <spot/twaalgos/ltl2taa.hh>
%include <spot/twaalgos/ltl2tgba_fm.hh>
%include <spot/twaalgos/gfguarantee.hh>
%include <spot/twaalgos/compsusp.hh>
%include <spot/twaalgos/contains.hh>
%include <spot/twaalgos/determinize.hh>
%include <spot/twaalgos/dualize.hh>
%include <spot/twaalgos/langmap.hh>
%include <spot/twaalgos/magic.hh>
%include <spot/twaalgos/minimize.hh>
%include <spot/twaalgos/neverclaim.hh>
%include <spot/twaalgos/randomize.hh>
%include <spot/twaalgos/remfin.hh>
%include <spot/twaalgos/remprop.hh>
%include <spot/twaalgos/totgba.hh>
%include <spot/twaalgos/sbacc.hh>
%traits_swigtype(spot::scc_info_node);
%fragment(SWIG_Traits_frag(spot::scc_info_node));
%nodefaultctor spot::internal::scc_edges;
%typemap(out, optimal="1") spot::internal::scc_edges<spot::digraph<spot::twa_graph_state, spot::twa_graph_edge_data> const, spot::internal::keep_all> {
  $result = SWIG_NewPointerObj((new $1_ltype($1)), $&1_descriptor, SWIG_POINTER_OWN);
}
%typemap(out, optimal="1") spot::internal::scc_edges<spot::digraph<spot::twa_graph_state, spot::twa_graph_edge_data> const, spot::internal::keep_inner_scc> {
  $result = SWIG_NewPointerObj((new $1_ltype($1)), $&1_descriptor, SWIG_POINTER_OWN);
}
%noexception spot::scc_info::edges_of;
%noexception spot::scc_info::inner_edges_of;
%rename(scc_info_with_options) spot::scc_info::scc_info(const_twa_graph_ptr aut, scc_info_options options);
%rename(scc_info_with_options) spot::scc_info::scc_info(const scc_and_mark_filter& filt, scc_info_options options);

%include <spot/twaalgos/sccinfo.hh>
%template(scc_info_scc_edges) spot::internal::scc_edges<spot::digraph<spot::twa_graph_state, spot::twa_graph_edge_data> const, spot::internal::keep_all>;
%template(scc_info_inner_scc_edges) spot::internal::scc_edges<spot::digraph<spot::twa_graph_state, spot::twa_graph_edge_data> const, spot::internal::keep_inner_scc>;
%template(vector_twa_graph) std::vector<spot::twa_graph_ptr>;
%include <spot/twaalgos/strength.hh>
%include <spot/twaalgos/sccfilter.hh>
%include <spot/twaalgos/stats.hh>
%include <spot/twaalgos/isdet.hh>
%include <spot/twaalgos/iscolored.hh>
%include <spot/twaalgos/isunamb.hh>
%include <spot/twaalgos/isweakscc.hh>
%include <spot/twaalgos/simulation.hh>
%include <spot/twaalgos/parity.hh>
%include <spot/twaalgos/postproc.hh>
%include <spot/twaalgos/powerset.hh>
%include <spot/twaalgos/product.hh>
%include <spot/twaalgos/split.hh>
%include <spot/twaalgos/sum.hh>
%include <spot/twaalgos/stutter.hh>
%include <spot/twaalgos/translate.hh>
%include <spot/twaalgos/toweak.hh>
%include <spot/twaalgos/hoa.hh>
%include <spot/twaalgos/dtwasat.hh>
%include <spot/twaalgos/relabel.hh>
%include <spot/twaalgos/word.hh>
%template(list_bdd) std::list<bdd>;
%include <spot/twaalgos/are_isomorphic.hh>
%include <spot/twaalgos/toparity.hh>

%pythonprepend spot::twa::dtwa_complement %{
  from warnings import warn
  warn("use dualize() instead of dtwa_complement()",
       DeprecationWarning)
%}

%include <spot/twaalgos/complement.hh>

%include <spot/kripke/fairkripke.hh>
%include <spot/kripke/kripke.hh>
%include <spot/kripke/kripkegraph.hh>
%template(kripke_graph_state_out) spot::internal::state_out<spot::digraph<spot::kripke_graph_state, void>>;
%template(kripke_graph_all_trans) spot::internal::all_trans<spot::digraph<spot::kripke_graph_state, void>>;
%template(kripke_graph_edge_boxed_data) spot::internal::boxed_label<void, true>;
%template(kripke_graph_edge_storage) spot::internal::edge_storage<unsigned int, unsigned int, unsigned int, spot::internal::boxed_label<void, true> >;
%template(kripke_graph_state_boxed_data) spot::internal::boxed_label<spot::kripke_graph_state, false>;
%template(kripke_graph_state_storage) spot::internal::distate_storage<unsigned int, spot::internal::boxed_label<spot::kripke_graph_state, false> >;
%template(kripke_graph_state_vector) std::vector<spot::internal::distate_storage<unsigned, internal::boxed_label<kripke_graph_state, false>>>;

%include <spot/parseaut/public.hh>

%include <spot/ta/ta.hh>
%include <spot/ta/tgta.hh>
%include <spot/ta/taexplicit.hh>
%include <spot/ta/tgtaexplicit.hh>
%include <spot/taalgos/tgba2ta.hh>
%include <spot/taalgos/dot.hh>
%include <spot/taalgos/stats.hh>
%include <spot/taalgos/minimize.hh>

%extend std::set<spot::formula> {
  std::string __str__()
  {
    std::ostringstream os;
    os << '{';
    const char* sep = "";
    for (spot::formula s: *self)
      {
        os << sep << '"' << spot::escape_str(spot::str_psl(s)) << '"';
        sep = ", ";
      }
    os << '}';
    return os.str();
  }
  std::string __repr__()
  {
    std::ostringstream os;
    os << "spot.atomic_prop_set([";
    const char* sep = "";
    for (spot::formula s: *self)
      {
        os << sep
           << "spot.formula(\"" << spot::escape_str(spot::str_psl(s)) << "\")";
        sep = ", ";
      }
    os << "])";
    return os.str();
  }
}

%extend spot::acc_cond::rs_pair {
  std::string __repr__()
  {
    std::ostringstream os;
    os << "spot.rs_pair(fin=[";
    const char* sep = "";
    for (unsigned s: self->fin.sets())
      {
        os << sep << s;
        sep = ", ";
      }
    os << "], inf=[";
    sep = "";
    for (unsigned s: self->inf.sets())
      {
        os << sep << s;
        sep = ", ";
      }
    os << "])";
    return os.str();
  }
}

%extend spot::trival {
  std::string __repr__()
  {
    if (self->is_true())
      return "spot.trival(True)";
    if (self->is_false())
      return "spot.trival(False)";
    return "spot.trival_maybe()";
  }

  std::string __str__()
  {
    std::ostringstream os;
    os << *self;
    return os.str();
  }

  spot::trival __neg__()
  {
    return !*self;
  }

  spot::trival __and__(spot::trival other)
  {
    return *self && other;
  }

  spot::trival __or__(spot::trival other)
  {
    return *self || other;
  }

  bool __eq__(spot::trival other)
  {
    return self->val() == other.val();
  }

  bool __ne__(spot::trival other)
  {
    return self->val() != other.val();
  }
}


%exception spot::formula::__getitem__ {
  try {
    $action
  }
  catch (const std::runtime_error& e)
  {
    SWIG_exception(SWIG_IndexError, e.what());
  }
}

%extend spot::formula {
  // __cmp__ is for Python 2.0
  int __cmp__(spot::formula b) { return self->id() - b.id(); }
  size_t __hash__() { return self->id(); }
  unsigned __len__() { return self->size(); }
  formula __getitem__(unsigned pos) { return (*self)[pos]; }

  std::string __repr__() {
    return "spot.formula(\"" + spot::escape_str(spot::str_psl(*self)) + "\")";
  }
  std::string __str__() { return spot::str_psl(*self); }
}

%runtime %{
#include <memory>

// The bdd_dict object stores pointers to keep track of who
// uses which BDD variables.  If this is a C++ object, we
// would like to use its this pointer (not the Python pointer).
// If this is a shared_ptr, we would like to use the address of
// of the pointed object, not that of the shared ptr.  Finally,
// we also want to allow Python objects in there,

static void* ptr_for_bdddict(PyObject* obj)
{
  void* ptr = obj;
  if (SwigPyObject* swig_obj = SWIG_Python_GetSwigThis(obj))
    {
      ptr = swig_obj->ptr;
      const char *type = SWIG_TypePrettyName(swig_obj->ty);
      if (strncmp(type, "std::shared_ptr<", 16) == 0)
        ptr = reinterpret_cast<std::shared_ptr<void>*>(ptr)->get();
    }
  return ptr;
}
%}

%extend spot::bdd_dict {
  bool operator==(const spot::bdd_dict& b) const
  {
    return self == &b;
  }

  bool operator!=(const spot::bdd_dict& b) const
  {
    return self != &b;
  }

  int register_proposition(formula f, PyObject* for_me)
  {
    return $self->register_proposition(f, ptr_for_bdddict(for_me));
  }

  void unregister_all_my_variables(PyObject* me)
  {
    return $self->unregister_all_my_variables(ptr_for_bdddict(me));
  }

  void unregister_variable(int var, PyObject* me)
  {
    return $self->unregister_variable(var, ptr_for_bdddict(me));
  }

  void register_all_variables_of(PyObject* from_other, PyObject* for_me)
  {
    return $self->register_all_variables_of(ptr_for_bdddict(from_other),
                                            ptr_for_bdddict(for_me));
  }

  int register_anonymous_variables(int n, PyObject* for_me)
  {
    return $self->register_anonymous_variables(n, ptr_for_bdddict(for_me));
  }

}

%extend spot::twa {
  void set_name(std::string name)
  {
    self->set_named_prop("automaton-name", new std::string(name));
  }

  std::string* get_name()
  {
    return self->get_named_prop<std::string>("automaton-name");
  }

  void set_state_names(std::vector<std::string> names)
  {
    self->set_named_prop("state-names",
			 new std::vector<std::string>(std::move(names)));
  }

  std::vector<std::string>* get_state_names()
  {
    return self->get_named_prop<std::vector<std::string>>("state-names");
  }


  void set_product_states(std::vector<std::pair<unsigned, unsigned>> pairs)
  {
    self->set_named_prop("product-states", new
                         std::vector<std::pair<unsigned,
                                               unsigned>>(std::move(pairs)));
  }

  std::vector<std::pair<unsigned, unsigned>>* get_product_states()
  {
    return self->get_named_prop
      <std::vector<std::pair<unsigned, unsigned>>>("product-states");
  }


  twa* highlight_state(unsigned state, unsigned color)
  {
    auto hs =
      self->get_named_prop<std::map<unsigned, unsigned>>("highlight-states");
    if (!hs)
      {
	hs = new std::map<unsigned, unsigned>;
	self->set_named_prop("highlight-states", hs);
      }
    (*hs)[state] = color;
    return self;
  }

  twa* highlight_edge(unsigned edge, unsigned color)
  {
    auto ht =
      self->get_named_prop<std::map<unsigned, unsigned>>("highlight-edges");
    if (!ht)
      {
	ht = new std::map<unsigned, unsigned>;
	self->set_named_prop("highlight-edges", ht);
      }
    (*ht)[edge] = color;
    return self;
  }
}

%extend spot::internal::state_out<spot::digraph<spot::twa_graph_state, spot::twa_graph_edge_data>> {
  swig::SwigPyIterator* __iter__(PyObject **PYTHON_SELF)
   {
      return swig::make_forward_iterator(self->begin(), self->begin(),
				         self->end(), *PYTHON_SELF);
   }
}

%extend spot::internal::state_out<spot::digraph<spot::kripke_graph_state, void>> {
  swig::SwigPyIterator* __iter__(PyObject **PYTHON_SELF)
   {
      return swig::make_forward_iterator(self->begin(), self->begin(),
				         self->end(), *PYTHON_SELF);
   }
}

%extend spot::internal::killer_edge_iterator<spot::digraph<spot::twa_graph_state, spot::twa_graph_edge_data>> {

  spot::internal::edge_storage<unsigned int, unsigned int, unsigned int, spot::internal::boxed_label<spot::twa_graph_edge_data, false> >& current()
  {
    return **self;
  }

  void advance()
  {
    ++*self;
  }
  bool __bool__()
  {
    return *self;
  }
}

%extend spot::internal::all_trans<spot::digraph<spot::kripke_graph_state, void>> {
  swig::SwigPyIterator* __iter__(PyObject **PYTHON_SELF)
   {
      return swig::make_forward_iterator(self->begin(), self->begin(),
				         self->end(), *PYTHON_SELF);
   }
}

%extend spot::internal::all_trans<spot::digraph<spot::twa_graph_state, spot::twa_graph_edge_data>> {
  swig::SwigPyIterator* __iter__(PyObject **PYTHON_SELF)
   {
      return swig::make_forward_iterator(self->begin(), self->begin(),
				         self->end(), *PYTHON_SELF);
   }
}

%extend spot::internal::const_universal_dests {
  swig::SwigPyIterator* __iter__(PyObject **PYTHON_SELF)
   {
      return swig::make_forward_iterator_np(self->begin(), self->begin(),
				            self->end(), *PYTHON_SELF);
   }
}

%extend spot::internal::mark_container {
  swig::SwigPyIterator* __iter__(PyObject **PYTHON_SELF)
   {
      return swig::make_forward_iterator_np(self->begin(), self->begin(),
				         self->end(), *PYTHON_SELF);
   }
}

%extend spot::internal::scc_edges<spot::digraph<spot::twa_graph_state, spot::twa_graph_edge_data> const, spot::internal::keep_all> {
  swig::SwigPyIterator* __iter__(PyObject **PYTHON_SELF)
   {
      return swig::make_forward_iterator(self->begin(), self->begin(),
				         self->end(), *PYTHON_SELF);
   }
}

%extend spot::internal::scc_edges<spot::digraph<spot::twa_graph_state, spot::twa_graph_edge_data> const, spot::internal::keep_inner_scc> {
  swig::SwigPyIterator* __iter__(PyObject **PYTHON_SELF)
   {
      return swig::make_forward_iterator(self->begin(), self->begin(),
				         self->end(), *PYTHON_SELF);
   }
}

%extend spot::twa_graph {
  unsigned new_univ_edge(unsigned src, const std::vector<unsigned>& v,
                         bdd cond, acc_cond::mark_t acc = {})
  {
    return self->new_univ_edge(src, v.begin(), v.end(), cond, acc);
  }

  void set_univ_init_state(const std::vector<unsigned>& v)
  {
    self->set_univ_init_state(v.begin(), v.end());
  }

  void report_univ_dest(unsigned src)
  {
    if (self->is_univ_dest(src))
      throw std::runtime_error
        ("universal destinations should be explored with univ_dest()");
  }
 }

%extend spot::scc_info {
  swig::SwigPyIterator* __iter__(PyObject **PYTHON_SELF)
   {
      return swig::make_forward_iterator(self->begin(), self->begin(),
                                         self->end(), *PYTHON_SELF);
   }
}

%extend spot::acc_cond::acc_code {
  std::string __repr__()
  {
    std::ostringstream os;
    os << "spot.acc_code(\"" << *self << "\")";
    return os.str();
  }

  std::string __str__()
  {
    std::ostringstream os;
    os << *self;
    return os.str();
  }
}

%extend spot::acc_cond::mark_t {
  // http://comments.gmane.org/gmane.comp.programming.swig/14822
  mark_t(const std::vector<unsigned>& f)
  {
    return new spot::acc_cond::mark_t(f.begin(), f.end());
  }

  std::string __repr__()
  {
    std::ostringstream os;
    os << "spot.mark_t([";
    const char* sep = "";
    for (unsigned s: self->sets())
      {
        os << sep << s;
        sep = ", ";
      }
    os << "])";
    return os.str();
  }

  std::string __str__()
  {
    std::ostringstream os;
    os << *self;
    return os.str();
  }
}

%extend spot::acc_cond {
  std::string __repr__()
  {
    std::ostringstream os;
    os << "spot.acc_cond(" << self->num_sets() << ", \""
       << self->get_acceptance() << "\")";
    return os.str();
  }

  std::string __str__()
  {
    std::ostringstream os;
    os << *self;
    return os.str();
  }
}

%extend spot::twa_run {
  std::string __str__()
  {
    std::ostringstream os;
    os << *self;
    return os.str();
  }
}

%extend spot::twa_word {
  std::string __str__()
  {
    std::ostringstream os;
    os << *self;
    return os.str();
  }
}

%nodefaultctor std::ostream;
namespace std {
  class ostream {};
  class ofstream : public ostream
  {
  public:
     ofstream(const char *fn);
     ~ofstream();
  };
  class ostringstream : public ostream
  {
  public:
     ostringstream();
     std::string str() const;
     ~ostringstream();
  };
}

%inline %{
bool fnode_instances_check()
{
  return spot::fnode::instances_check();
}

spot::twa_graph_ptr
ensure_digraph(const spot::twa_ptr& a)
{
  auto aa = std::dynamic_pointer_cast<spot::twa_graph>(a);
  if (aa)
    return aa;
  return spot::make_twa_graph(a, spot::twa::prop_set::all());
}

std::ostream&
get_cout()
{
  return std::cout;
}

void
nl_cout()
{
  std::cout << std::endl;
}

std::ostream&
get_cerr()
{
  return std::cerr;
}

void
nl_cerr()
{
  std::cerr << std::endl;
}

void
print_on(std::ostream& on, const std::string& what)
{
  on << what;
}

int
unblock_signal(int signum)
{
  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, signum);
  return sigprocmask(SIG_UNBLOCK, &set, 0);
}

// for alternation.hh
unsigned states_and(const spot::twa_graph_ptr& aut,
                    const std::vector<unsigned>& il)
{
  return states_and(aut, il.begin(), il.end());
}

%}

%extend spot::parse_error_list {

bool
__nonzero__()
{
  return !self->empty();
}

bool
__bool__()
{
  return !self->empty();
}

}

%extend spot::parse_aut_error_list {

bool
__nonzero__()
{
  return !self->empty();
}

bool
__bool__()
{
  return !self->empty();
}

}
