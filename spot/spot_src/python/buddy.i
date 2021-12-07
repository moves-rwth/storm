// -*- coding: utf-8 -*-
// Copyright (C) 2010, 2011, 2012, 2014, 2016 Laboratoire de Recherche et
// Développement de l'EPITA.
// Copyright (C) 2003, 2004  Laboratoire d'Informatique de Paris 6 (LIP6),
// département Systèmes Répartis Coopératifs (SRC), Université Pierre
// et Marie Curie.
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

// This is derived from Buddy's headers, distributed with the
// following license:

/*========================================================================
	       Copyright (C) 1996-2003 by Jorn Lind-Nielsen
			    All rights reserved

    Permission is hereby granted, without written agreement and without
    license or royalty fees, to use, reproduce, prepare derivative
    works, distribute, and display this software and its documentation
    for any purpose, provided that (1) the above copyright notice and
    the following two paragraphs appear in all copies of the source code
    and (2) redistributions, including without limitation binaries,
    reproduce these notices in the supporting documentation. Substantial
    modifications to this software may be copyrighted by their authors
    and need not follow the licensing terms described here, provided
    that the new terms are clearly indicated in all files where they apply.

    IN NO EVENT SHALL JORN LIND-NIELSEN, OR DISTRIBUTORS OF THIS
    SOFTWARE BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL,
    INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS
    SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE AUTHORS OR ANY OF THE
    ABOVE PARTIES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    JORN LIND-NIELSEN SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
    BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
    FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS
    ON AN "AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO
    OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
    MODIFICATIONS.
========================================================================*/

%{
  // Workaround for SWIG 2.0.2 using ptrdiff_t but not including cstddef.
  // It matters with g++ 4.6.
#include <cstddef>
%}

%module buddy

%include "std_string.i"

%{
#include <sstream>
#include "bddx.h"
#include "fddx.h"
#include "bvecx.h"
%}

%typemap(in) (int* input_buf, int input_buf_size) {
  if (!PySequence_Check($input))
    {
      PyErr_SetString(PyExc_ValueError, "Expected a sequence");
      return 0;
    }
  $2 = PySequence_Length($input);
  $1 = (int*) malloc($2 * sizeof(int));
  for (int i = 0; i < $2; ++i)
    {
      PyObject* o = PySequence_GetItem($input, i);
      if (PyInt_Check(o))
        {
          $1[i] = PyInt_AsLong(o);
	}
      else
        {
          PyErr_SetString(PyExc_ValueError,
                          "Sequence elements must be integers");
          return 0;
        }
    }
}
%typemap(freearg) (int* input_buf, int input_buf_size) {
  if ($1)
    free($1);
}


%inline {
  struct const_int_ptr
  {
    const_int_ptr(const int* ptr)
      : ptr(ptr)
    {
    }
    const int* ptr;
  };
}

%extend const_int_ptr {
  int
  __getitem__(int i)
  {
    return self->ptr[i];
  }
}

struct bdd
{
  int id(void) const;
};

int      bdd_init(int, int);
void     bdd_done(void);
int      bdd_setvarnum(int);
int      bdd_extvarnum(int);
int      bdd_isrunning(void);
int      bdd_setmaxnodenum(int);
int      bdd_setmaxincrease(int);
int      bdd_setminfreenodes(int);
int      bdd_getnodenum(void);
int      bdd_getallocnum(void);
char*    bdd_versionstr(void);
int      bdd_versionnum(void);
void     bdd_fprintstat(FILE *);
void     bdd_printstat(void);
const char *bdd_errstring(int);
void     bdd_clear_error(void);

bdd bdd_ithvar(int v);
bdd bdd_nithvar(int v);
int bdd_var(const bdd &r);
bdd bdd_low(const bdd &r);
bdd bdd_high(const bdd &r);
int bdd_scanset(const bdd &r, int *&v, int &n);
bdd bdd_makeset(int *v, int n);
int bdd_setbddpair(bddPair *p, int ov, const bdd &nv);
bdd bdd_replace(const bdd &r, bddPair *p);
bdd bdd_compose(const bdd &f, const bdd &g, int v);
bdd bdd_veccompose(const bdd &f, bddPair *p);
bdd bdd_restrict(const bdd &r, const bdd &var);
bdd bdd_constrain(const bdd &f, const bdd &c);
bdd bdd_simplify(const bdd &d, const bdd &b);
bdd bdd_ibuildcube(int v, int w, int *a);
bdd bdd_not(const bdd &r);
bdd bdd_apply(const bdd &l, const bdd &r, int op);
bdd bdd_and(const bdd &l, const bdd &r);
bdd bdd_or(const bdd &l, const bdd &r);
bdd bdd_xor(const bdd &l, const bdd &r);
bdd bdd_imp(const bdd &l, const bdd &r);
bdd bdd_biimp(const bdd &l, const bdd &r);
bdd bdd_setxor(const bdd &l, const bdd &r);
int bdd_implies(const bdd &l, const bdd &r);
bdd bdd_ite(const bdd &f, const bdd &g, const bdd &h);
bdd bdd_exist(const bdd &r, const bdd &var);
bdd bdd_existcomp(const bdd &r, const bdd &var);
bdd bdd_forall(const bdd &r, const bdd &var);
bdd bdd_forallcomp(const bdd &r, const bdd &var);
bdd bdd_unique(const bdd &r, const bdd &var);
bdd bdd_uniquecomp(const bdd &r, const bdd &var);
bdd bdd_appex(const bdd &l, const bdd &r, int op, const bdd &var);
bdd bdd_appexcomp(const bdd &l, const bdd &r, int op, const bdd &var);
bdd bdd_appall(const bdd &l, const bdd &r, int op, const bdd &var);
bdd bdd_appallcomp(const bdd &l, const bdd &r, int op, const bdd &var);
bdd bdd_appuni(const bdd &l, const bdd &r, int op, const bdd &var);
bdd bdd_appunicomp(const bdd &l, const bdd &r, int op, const bdd &var);
bdd bdd_support(const bdd &r);
bdd bdd_satone(const bdd &r);
bdd bdd_satoneset(const bdd &r, const bdd &var, const bdd &pol);
bdd bdd_fullsatone(const bdd &r);
void bdd_allsat(const bdd &r, bddallsathandler handler);
double bdd_satcount(const bdd &r);
double bdd_satcountset(const bdd &r, const bdd &varset);
double bdd_satcountln(const bdd &r);
double bdd_satcountlnset(const bdd &r, const bdd &varset);
int bdd_nodecount(const bdd &r);
int* bdd_varprofile(const bdd &r);
double bdd_pathcount(const bdd &r);
void bdd_fprinttable(FILE *file, const bdd &r);
void bdd_printtable(const bdd &r);
void bdd_fprintset(FILE *file, const bdd &r);
void bdd_printset(const bdd &r);
void bdd_printdot(const bdd &r);
void bdd_fprintdot(FILE* ofile, const bdd &r);
int bdd_fnprintdot(char* fname, const bdd &r);
int bdd_fnsave(char *fname, const bdd &r);
int bdd_save(FILE *ofile, const bdd &r);
int bdd_fnload(char *fname, bdd &r);
int bdd_load(FILE *ifile, bdd &r);
int bdd_addvarblock(const bdd &v, int f);

extern const bdd bddfalse;
extern const bdd bddtrue;

#define bddop_and       0
#define bddop_xor       1
#define bddop_or        2
#define bddop_nand      3
#define bddop_nor       4
#define bddop_imp       5
#define bddop_biimp     6
#define bddop_diff      7
#define bddop_less      8
#define bddop_invimp    9

#define BDD_REORDER_NONE     0
#define BDD_REORDER_WIN2     1
#define BDD_REORDER_WIN2ITE  2
#define BDD_REORDER_SIFT     3
#define BDD_REORDER_SIFTITE  4
#define BDD_REORDER_WIN3     5
#define BDD_REORDER_WIN3ITE  6
#define BDD_REORDER_RANDOM   7

%extend bdd {
  // For Python 2.0
  int __cmp__(bdd* b) { return b->id() - self->id(); }

  // For Python 2.1+ and Python 3
  bool __le__(bdd* b) { return self->id() <= b->id(); }
  bool __lt__(bdd* b) { return self->id() < b->id(); }
  bool __eq__(bdd* b) { return self->id() == b->id(); }
  bool __ne__(bdd* b) { return self->id() != b->id(); }
  bool __ge__(bdd* b) { return self->id() >= b->id(); }
  bool __gt__(bdd* b) { return self->id() > b->id(); }

  size_t __hash__() { return self->id(); }

  std::string
  __str__(void)
  {
    std::ostringstream res;
    res << "bdd(id=" << self->id() << ")";
    return res.str();
  }

  bdd __and__(bdd& other) { return *self & other; }
  bdd __xor__(bdd& other) { return *self ^ other; }
  bdd __or__(bdd& other) { return *self | other; }
  bdd __rshift__(bdd& other) { return *self >> other; }
  bdd __lshift__(bdd& other) { return *self << other; }
  bdd __sub__(bdd& other) { return *self - other; }
  bdd __neg__(void) { return !*self; }

}

/************************************************************************/

int  fdd_extdomain(int* input_buf, int input_buf_size);
int  fdd_overlapdomain(int, int);
void fdd_clearall(void);
int  fdd_domainnum(void);
int  fdd_domainsize(int);
int  fdd_varnum(int);
const_int_ptr fdd_vars(int);
bdd  fdd_ithvar(int, int);
int  fdd_scanvar(bdd, int);
int* fdd_scanallvar(bdd);
bdd  fdd_ithset(int);
bdd  fdd_domain(int);
bdd  fdd_equals(int, int);
void fdd_printset(bdd);
void fdd_fprintset(FILE*, bdd);
int  fdd_scanset(const bdd &, int *&, int &);
bdd  fdd_makeset(int*, int);
int  fdd_intaddvarblock(int, int, int);
int  fdd_setpair(bddPair*, int, int);
int  fdd_setpairs(bddPair*, int*, int*, int);

/************************************************************************/

bvec bvec_copy(bvec v);
bvec bvec_true(int bitnum);
bvec bvec_false(int bitnum);
bvec bvec_con(int bitnum, int val);
bvec bvec_var(int bitnum, int offset, int step);
bvec bvec_varfdd(int var);
bvec bvec_varvec(int bitnum, int *var);
bvec bvec_coerce(int bitnum, bvec v);
int  bvec_isconst(bvec e);
int  bvec_val(bvec e);
bvec bvec_map1(const bvec&, bdd (*fun)(const bdd &));
bvec bvec_map2(const bvec&, const bvec&, bdd (*fun)(const bdd &, const bdd &));
bvec bvec_map3(const bvec&, const bvec&, const bvec &,
               bdd (*fun)(const bdd &, const bdd &, const bdd &));

bvec bvec_add(bvec left, bvec right);
bvec bvec_sub(bvec left, bvec right);
bvec bvec_mulfixed(bvec e, int c);
bvec bvec_mul(bvec left, bvec right);
int  bvec_divfixed(const bvec &, int c, bvec &, bvec &);
int  bvec_div(const bvec &, const bvec &, bvec &, bvec &);
bvec bvec_ite(bdd a, bvec b, bvec c);
bvec bvec_shlfixed(bvec e, int pos, bdd c);
bvec bvec_shl(bvec l, bvec r, bdd c);
bvec bvec_shrfixed(bvec e, int pos, bdd c);
bvec bvec_shr(bvec l, bvec r, bdd c);
bdd  bvec_lth(bvec left, bvec right);
bdd  bvec_lte(bvec left, bvec right);
bdd  bvec_gth(bvec left, bvec right);
bdd  bvec_gte(bvec left, bvec right);
bdd  bvec_equ(bvec left, bvec right);
bdd  bvec_neq(bvec left, bvec right);

class bvec
{
 public:
   bvec(void);
   bvec(int bitnum);
   bvec(int bitnum, int val);
   bvec(const bvec &v);
   ~bvec(void);

   void set(int i, const bdd &b);
   int bitnum(void) const;
   int empty(void) const;
   bvec operator=(const bvec &src);

   bvec operator&(const bvec &a) const;
   bvec operator^(const bvec &a) const;
   bvec operator|(const bvec &a) const;
   bvec operator!(void) const;
   bvec operator<<(int a)   const;
   bvec operator<<(const bvec &a) const;
   bvec operator>>(int a)   const;
   bvec operator>>(const bvec &a) const;
   bvec operator+(const bvec &a) const;
   bvec operator-(const bvec &a) const;
   bvec operator*(int a) const;
   bvec operator*(const bvec a) const;
   bdd operator<(const bvec &a) const;
   bdd operator<=(const bvec &a) const;
   bdd operator>(const bvec &a) const;
   bdd operator>=(const bvec &a) const;
   bdd operator==(const bvec &a) const;
   bdd operator!=(const bvec &a) const;
};


%extend bvec {
  std::string
  __str__(void)
  {
    std::ostringstream res;
    res << "bvec(bitnum=" << self->bitnum() << ")";
    return res.str();
  }

  bdd
  __getitem__(int i)
  {
    return (*self)[i];
  }
}
