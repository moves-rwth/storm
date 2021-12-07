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

/*************************************************************************
  $Header: /Volumes/CVS/repository/spot/spot/buddy/src/bdd.h,v 1.6 2003/08/06 14:19:29 aduret Exp $
  FILE:  bdd.h
  DESCR: C,C++ User interface for the BDD package
  AUTH:  Jorn Lind
  DATE:  (C) feb 1997
*************************************************************************/

#ifndef _BDDX_H
#define _BDDX_H

#if __GNUC__ >= 3
#define __purefn __attribute__((__pure__))
#define __constfn __attribute__((__const__))
#define __noreturnfn __attribute__((__noreturn__))
#define __likely(expr)   __builtin_expect(!!(expr), 1)
#define __unlikely(expr) __builtin_expect(!!(expr), 0)
#else
#define __purefn
#define __constfn
#define __noreturnfn
#define __likely(expr) (expr)
#define __unlikely(expr) (expr)
#endif

#if defined _WIN32 || defined __CYGWIN__
  #define BUDDY_HELPER_DLL_IMPORT __declspec(dllimport)
  #define BUDDY_HELPER_DLL_EXPORT __declspec(dllexport)
  #define BUDDY_HELPER_DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define BUDDY_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
    #define BUDDY_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
    #define BUDDY_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define BUDDY_HELPER_DLL_IMPORT
    #define BUDDY_HELPER_DLL_EXPORT
    #define BUDDY_HELPER_DLL_LOCAL
  #endif
#endif

/* BUDDY_API is used for the public API symbols. It either DLL imports
** or DLL exports (or does nothing for static build) BUDDY_LOCAL is
** used for non-api symbols.
*/
#ifdef BUDDY_DLL
  #ifdef BUDDY_DLL_EXPORTS
    #define BUDDY_API BUDDY_HELPER_DLL_EXPORT
  #else
    #define BUDDY_API BUDDY_HELPER_DLL_IMPORT
  #endif
  #define BUDDY_LOCAL BUDDY_HELPER_DLL_LOCAL
#else
  #define BUDDY_API
  #define BUDDY_LOCAL
#endif
#define BUDDY_API_VAR extern BUDDY_API

   /* Allow this headerfile to define C++ constructs if requested */
#ifdef __cplusplus
#define CPLUSPLUS
#endif

#include <stdio.h>

/*=== Defined operators for apply calls ================================*/

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

   /* Should *not* be used in bdd_apply calls !!! */
#define bddop_not      10
#define bddop_simplify 11


/*=== User BDD types ===================================================*/

typedef int BDD;

#ifndef CPLUSPLUS
typedef BDD bdd;
#endif /* CPLUSPLUS */


typedef struct s_bddPair
{
   BDD *result;
   int last;
   int id;
   struct s_bddPair *next;
} bddPair;


/*=== Status information ===============================================*/

/*
NAME    {* bddStat *}
SECTION {* kernel *}
SHORT   {* Status information about the bdd package *}
PROTO   {* typedef struct s_bddStat
{
   long int produced;
   int nodenum;
   int maxnodenum;
   int freenodes;
   int minfreenodes;
   int varnum;
   int cachesize;
   int gbcnum;
} bddStat;  *}
DESCR   {* The fields are \\[\baselineskip] \begin{tabular}{lp{10cm}}
  {\tt produced}     & total number of new nodes ever produced \\
  {\tt nodenum}      & currently allocated number of bdd nodes \\
  {\tt maxnodenum}   & user defined maximum number of bdd nodes \\
  {\tt freenodes}    & number of currently free nodes \\
  {\tt minfreenodes} & minimum number of nodes that should be left after a
		       garbage collection. \\
  {\tt varnum}       & number of defined bdd variables \\
  {\tt cachesize}    & number of entries in the internal caches \\
  {\tt hashsize}     & number of entries in the node hash table \\
  {\tt gbcnum}       & number of garbage collections done until now
  \end{tabular} *}
ALSO    {* bdd\_stats *}
*/
typedef struct s_bddStat
{
   long int produced;
   int nodenum;
   int maxnodenum;
   int freenodes;
   int minfreenodes;
   int varnum;
   int cachesize;
   int hashsize;
   int gbcnum;
} bddStat;


/*
NAME    {* bddGbcStat *}
SECTION {* kernel *}
SHORT   {* Status information about garbage collections *}
PROTO   {* typedef struct s_bddGbcStat
{
   int nodes;
   int freenodes;
   long time;
   long sumtime;
   int num;
} bddGbcStat;  *}
DESCR   {* The fields are \\[\baselineskip] \begin{tabular}{ll}
  {\tt nodes}     & Total number of allocated nodes in the nodetable \\
  {\tt freenodes} & Number of free nodes in the nodetable \\
  {\tt time}      & Time used for garbage collection this time \\
  {\tt sumtime}   & Total time used for garbage collection \\
  {\tt num}       & number of garbage collections done until now
  \end{tabular} *}
ALSO    {* bdd\_gbc\_hook *}
*/
typedef struct s_bddGbcStat
{
   int nodes;
   int freenodes;
   long time;
   long sumtime;
   int num;
} bddGbcStat;


/*
NAME    {* bddCacheStat *}
SECTION {* kernel *}
SHORT   {* Status information about cache usage *}
PROTO   {* typedef struct s_bddCacheStat
{
   long unsigned int uniqueAccess;
   long unsigned int uniqueChain;
   long unsigned int uniqueHit;
   long unsigned int uniqueMiss;
   long unsigned int opHit;
   long unsigned int opMiss;
   long unsigned int swapCount;
} bddCacheStat; *}
DESCR   {* The fields are \\[\baselineskip] \begin{tabular}{ll}
  {\bf Name}         & {\bf Number of } \\
  uniqueAccess & accesses to the unique node table \\
  uniqueChain  & iterations through the cache chains in the unique node table\\
  uniqueHit    & entries actually found in the the unique node table \\
  uniqueMiss   & entries not found in the the unique node table \\
  opHit        & entries found in the operator caches \\
  opMiss       & entries not found in the operator caches \\
  swapCount    & number of variable swaps in reordering \\
\end{tabular} *}
ALSO    {* bdd\_cachestats *}
*/
typedef struct s_bddCacheStat
{
   long unsigned int uniqueAccess;
   long unsigned int uniqueChain;
   long unsigned int uniqueHit;
   long unsigned int uniqueMiss;
   long unsigned int opHit;
   long unsigned int opMiss;
   long unsigned int swapCount;
} bddCacheStat;

/*=== BDD interface prototypes =========================================*/

/*
NAME    {* bdd\_relprod *}
SECTION {* operator *}
SHORT   {* relational product *}
PROTO   {* #define bdd_relprod(a,b,var) bdd_appex(a,b,bddop_and,var) *}
DESCR   {* Calculates the relational product of {\tt a} and {\tt b} as
	   {\tt a AND b} with the variables in {\tt var} quantified out
	   afterwards. *}
RETURN  {* The relational product or {\tt bddfalse} on errors. *}
ALSO    {* bdd\_appex *}
*/
#define bdd_relprod(a,b,var) bdd_appex((a),(b),bddop_and,(var))


  /* In file "kernel.c" */

#ifdef CPLUSPLUS
extern "C" {
#endif

typedef void (*bddinthandler)(int);
typedef void (*bddgbchandler)(int,bddGbcStat*);
typedef void (*bdd2inthandler)(int,int);
typedef int  (*bddsizehandler)(void);
typedef void (*bddfilehandler)(FILE *, int);
typedef void (*bddallsathandler)(signed char*, int);
// The historical type of bddallsathandler is the following,
// unfortunately the signedness of char* is implementation defined.
// Now we have to support both for backward compatibility.
typedef void (*bddallsathandler_old)(char*, int);

BUDDY_API bddinthandler  bdd_error_hook(bddinthandler);
BUDDY_API bddgbchandler  bdd_gbc_hook(bddgbchandler);
BUDDY_API bdd2inthandler bdd_resize_hook(bdd2inthandler);
BUDDY_API bddinthandler  bdd_reorder_hook(bddinthandler);
BUDDY_API bddfilehandler bdd_file_hook(bddfilehandler);

BUDDY_API int      bdd_init(int, int);
BUDDY_API void     bdd_done(void);
BUDDY_API int      bdd_setvarnum(int);
BUDDY_API int      bdd_extvarnum(int);
BUDDY_API int      bdd_isrunning(void) __purefn;
BUDDY_API int      bdd_setmaxnodenum(int);
BUDDY_API int      bdd_setmaxincrease(int);
BUDDY_API int      bdd_setminfreenodes(int);
BUDDY_API int      bdd_getnodenum(void) __purefn;
BUDDY_API int      bdd_getallocnum(void) __purefn;
BUDDY_API char*    bdd_versionstr(void) __purefn;
BUDDY_API int      bdd_versionnum(void) __constfn;
BUDDY_API void     bdd_stats(bddStat *);
BUDDY_API void     bdd_cachestats(bddCacheStat *);
BUDDY_API void     bdd_fprintstat(FILE *);
BUDDY_API void     bdd_printstat(void);
BUDDY_API void     bdd_default_gbchandler(int, bddGbcStat *);
BUDDY_API void     bdd_default_errhandler(int) __noreturnfn;
BUDDY_API const char *bdd_errstring(int) __constfn;
BUDDY_API void     bdd_clear_error(void);
#ifndef CPLUSPLUS
BUDDY_API BDD      bdd_true(void) __constfn;
BUDDY_API BDD      bdd_false(void) __constfn;
#endif
BUDDY_API int      bdd_varnum(void) __purefn;
BUDDY_API BDD      bdd_ithvar(int) __purefn;
BUDDY_API BDD      bdd_nithvar(int) __purefn;
BUDDY_API int      bdd_var(BDD) __purefn;
BUDDY_API BDD      bdd_low(BDD) __purefn;
BUDDY_API BDD      bdd_high(BDD) __purefn;
BUDDY_API int      bdd_varlevel(int) __purefn;
BUDDY_API BDD      bdd_addref_nc(BDD);
BUDDY_API BDD      bdd_addref(BDD);
BUDDY_API BDD      bdd_delref_nc(BDD);
BUDDY_API BDD      bdd_delref(BDD);
BUDDY_API void     bdd_gbc(void);
BUDDY_API int      bdd_scanset(BDD, int**, int*);
BUDDY_API BDD      bdd_makeset(int *, int);
BUDDY_API bddPair* bdd_copypair(bddPair*);
BUDDY_API bddPair* bdd_mergepairs(bddPair*, bddPair*);
BUDDY_API bddPair* bdd_newpair(void);
BUDDY_API int      bdd_setpair(bddPair*, int, int);
BUDDY_API int      bdd_setpairs(bddPair*, int*, int*, int);
BUDDY_API int      bdd_setbddpair(bddPair*, int, BDD);
BUDDY_API int      bdd_setbddpairs(bddPair*, int*, BDD*, int);
BUDDY_API void     bdd_resetpair(bddPair *);
BUDDY_API void     bdd_freepair(bddPair*);

  /* In bddop.c */

BUDDY_API int      bdd_setcacheratio(int);
BUDDY_API BDD      bdd_buildcube(int, int, BDD *);
BUDDY_API BDD      bdd_ibuildcube(int, int, int *);
BUDDY_API BDD      bdd_not(BDD);
BUDDY_API BDD      bdd_apply(BDD, BDD, int);
BUDDY_API BDD      bdd_and(BDD, BDD);
BUDDY_API BDD      bdd_or(BDD, BDD);
BUDDY_API BDD      bdd_xor(BDD, BDD);
BUDDY_API BDD      bdd_imp(BDD, BDD);
BUDDY_API BDD      bdd_biimp(BDD, BDD);
BUDDY_API BDD      bdd_setxor(BDD, BDD);
BUDDY_API int      bdd_implies(BDD, BDD);
BUDDY_API BDD      bdd_ite(BDD, BDD, BDD);
BUDDY_API BDD      bdd_restrict(BDD, BDD);
BUDDY_API BDD      bdd_constrain(BDD, BDD);
BUDDY_API BDD      bdd_replace(BDD, bddPair*);
BUDDY_API BDD      bdd_compose(BDD, BDD, BDD);
BUDDY_API BDD      bdd_veccompose(BDD, bddPair*);
BUDDY_API BDD      bdd_simplify(BDD, BDD);
BUDDY_API BDD      bdd_exist(BDD, BDD);
BUDDY_API BDD      bdd_existcomp(BDD, BDD);
BUDDY_API BDD      bdd_forall(BDD, BDD);
BUDDY_API BDD      bdd_forallcomp(BDD, BDD);
BUDDY_API BDD      bdd_unique(BDD, BDD);
BUDDY_API BDD      bdd_uniquecomp(BDD, BDD);
BUDDY_API BDD      bdd_appex(BDD, BDD, int, BDD);
BUDDY_API BDD      bdd_appexcomp(BDD, BDD, int, BDD);
BUDDY_API BDD      bdd_appall(BDD, BDD, int, BDD);
BUDDY_API BDD      bdd_appallcomp(BDD, BDD, int, BDD);
BUDDY_API BDD      bdd_appuni(BDD, BDD, int, BDD);
BUDDY_API BDD      bdd_appunicomp(BDD, BDD, int, BDD);
BUDDY_API BDD      bdd_support(BDD);
BUDDY_API BDD      bdd_satone(BDD);
BUDDY_API BDD      bdd_satoneset(BDD, BDD, BDD);
BUDDY_API BDD      bdd_fullsatone(BDD);
BUDDY_API BDD	bdd_satprefix(BDD *);
BUDDY_API void     bdd_allsat(BDD r, bddallsathandler handler);
BUDDY_API double   bdd_satcount(BDD);
BUDDY_API double   bdd_satcountset(BDD, BDD);
BUDDY_API double   bdd_satcountln(BDD);
BUDDY_API double   bdd_satcountlnset(BDD, BDD);
BUDDY_API int      bdd_nodecount(BDD);
BUDDY_API int      bdd_anodecount(BDD *, int);
BUDDY_API int*     bdd_varprofile(BDD);
BUDDY_API double   bdd_pathcount(BDD);


/* In file "bddio.c" */

BUDDY_API void     bdd_printall(void);
BUDDY_API void     bdd_fprintall(FILE *);
BUDDY_API void     bdd_fprinttable(FILE *, BDD);
BUDDY_API void     bdd_printtable(BDD);
BUDDY_API void     bdd_fprintset(FILE *, BDD);
BUDDY_API void     bdd_printset(BDD);
BUDDY_API int      bdd_fnprintdot(char *, BDD);
BUDDY_API void     bdd_fprintdot(FILE *, BDD);
BUDDY_API void     bdd_printdot(BDD);
BUDDY_API int      bdd_fnsave(char *, BDD);
BUDDY_API int      bdd_save(FILE *, BDD);
BUDDY_API int      bdd_fnload(char *, BDD *);
BUDDY_API int      bdd_load(FILE *ifile, BDD *);

/* In file reorder.c */

BUDDY_API int      bdd_swapvar(int v1, int v2);
BUDDY_API void     bdd_default_reohandler(int);
BUDDY_API void     bdd_reorder(int);
BUDDY_API int      bdd_reorder_gain(void) __purefn;
BUDDY_API bddsizehandler bdd_reorder_probe(bddsizehandler);
BUDDY_API void     bdd_clrvarblocks(void);
BUDDY_API int      bdd_addvarblock(BDD, int);
BUDDY_API int      bdd_intaddvarblock(int, int, int);
BUDDY_API void     bdd_varblockall(void);
BUDDY_API bddfilehandler bdd_blockfile_hook(bddfilehandler);
BUDDY_API int      bdd_autoreorder(int);
BUDDY_API int      bdd_autoreorder_times(int, int);
BUDDY_API int      bdd_var2level(int);
BUDDY_API int      bdd_level2var(int);
BUDDY_API int      bdd_getreorder_times(void) __purefn;
BUDDY_API int      bdd_getreorder_method(void) __purefn;
BUDDY_API void     bdd_enable_reorder(void);
BUDDY_API void     bdd_disable_reorder(void);
BUDDY_API int      bdd_reorder_verbose(int);
BUDDY_API void     bdd_setvarorder(int *);
BUDDY_API void     bdd_printorder(void);
BUDDY_API void     bdd_fprintorder(FILE *);

#ifdef CPLUSPLUS
}
#endif


/*=== BDD constants ====================================================*/

#ifndef CPLUSPLUS

BUDDY_API_VAR const BDD bddfalse;
BUDDY_API_VAR const BDD bddtrue;

#endif /* CPLUSPLUS */


/*=== Reordering algorithms ============================================*/

#define BDD_REORDER_NONE     0
#define BDD_REORDER_WIN2     1
#define BDD_REORDER_WIN2ITE  2
#define BDD_REORDER_SIFT     3
#define BDD_REORDER_SIFTITE  4
#define BDD_REORDER_WIN3     5
#define BDD_REORDER_WIN3ITE  6
#define BDD_REORDER_RANDOM   7

#define BDD_REORDER_FREE     0
#define BDD_REORDER_FIXED    1


/*=== Error codes ======================================================*/

#define BDD_MEMORY (-1)   /* Out of memory */
#define BDD_VAR (-2)      /* Unknown variable */
#define BDD_RANGE (-3)    /* Variable value out of range (not in domain) */
#define BDD_DEREF (-4)    /* Removing external reference to unknown node */
#define BDD_RUNNING (-5)  /* Called bdd_init() twice whithout bdd_done() */
#define BDD_FILE (-6)     /* Some file operation failed */
#define BDD_FORMAT (-7)   /* Incorrect file format */
#define BDD_ORDER (-8)    /* Vars. not in order for vector based functions */
#define BDD_BREAK (-9)    /* User called break */
#define BDD_VARNUM (-10)  /* Different number of vars. for vector pair */
#define BDD_NODES (-11)   /* Tried to set max. number of nodes to be fewer */
			  /* than there already has been allocated */
#define BDD_OP (-12)      /* Unknown operator */
#define BDD_VARSET (-13)  /* Illegal variable set */
#define BDD_VARBLK (-14)  /* Bad variable block operation */
#define BDD_DECVNUM (-15) /* Trying to decrease the number of variables */
#define BDD_REPLACE (-16) /* Replacing to already existing variables */
#define BDD_NODENUM (-17) /* Number of nodes reached user defined maximum */
#define BDD_ILLBDD (-18)  /* Illegal bdd argument */
#define BDD_SIZE (-19)    /* Illegal size argument */

#define BVEC_SIZE (-20)    /* Mismatch in bitvector size */
#define BVEC_SHIFT (-21)   /* Illegal shift-left/right parameter */
#define BVEC_DIVZERO (-22) /* Division by zero */

#define BDD_INVMERGE (-23) /* Merging clashing rewriting rules */

#define BDD_ERRNUM 25

/*************************************************************************
   If this file is included from a C++ compiler then the following
   classes, wrappers and hacks are supplied.
*************************************************************************/
#ifdef CPLUSPLUS
#include <iostream>

/*=== User BDD class ===================================================*/

class BUDDY_API bvec;

class BUDDY_API bddxfalse;
class BUDDY_API bddxtrue;

class BUDDY_API bdd
{
 public:

   bdd(void) noexcept
   {
     root=0;
   }

   bdd(const bdd &r) noexcept
   {
     root=r.root;
     if (root > 1)
       bdd_addref_nc(root);
   }

   bdd(const bddxfalse &) noexcept
   {
     root = 0;
   }

   bdd(const bddxtrue &) noexcept
   {
     root = 1;
   }

   bdd(bdd&& r) noexcept
   {
     root=r.root;
     r.root = 0;
   }

   ~bdd(void) noexcept
   {
     if (root > 1)
       bdd_delref_nc(root);
   }

   int id(void) const noexcept;

   bdd& operator=(const bdd &r) noexcept;
   bdd& operator=(bdd&& r) noexcept;

   bdd& operator=(const bddxtrue&) noexcept;
   bdd& operator=(const bddxfalse&) noexcept;

   bdd operator&(const bdd &r) const;
   bdd& operator&=(const bdd &r);
   bdd operator^(const bdd &r) const;
   bdd& operator^=(const bdd &r);
   bdd operator|(const bdd &r) const;
   bdd& operator|=(const bdd &r);
   bdd operator!(void) const;
   bdd operator>>(const bdd &r) const;
   bdd& operator>>=(const bdd &r);
   bdd operator-(const bdd &r) const;
   bdd& operator-=(const bdd &r);
   bdd operator>(const bdd &r) const;
   bdd operator<(const bdd &r) const;
   bdd operator<<(const bdd &r) const;
   bdd& operator<<=(const bdd &r);
   int operator==(const bdd &r) const noexcept;
   int operator==(const bddxfalse&) const noexcept;
   int operator==(const bddxtrue&) const noexcept;
   int operator!=(const bdd &r) const noexcept;
   int operator!=(const bddxfalse&) const noexcept;
   int operator!=(const bddxtrue&) const noexcept;

protected:
   BDD root;

   bdd(BDD r) noexcept { root=r; if (root > 1) bdd_addref_nc(root); }
   bdd& operator=(BDD r) noexcept;

   friend int      bdd_init(int, int);
   friend int      bdd_setvarnum(int);
   friend bddxtrue bdd_true(void);
   friend bddxfalse bdd_false(void);
   friend bdd      bdd_ithvarpp(int);
   friend bdd      bdd_nithvarpp(int);
   friend int      bdd_var(const bdd &);
   friend bdd      bdd_low(const bdd &);
   friend bdd      bdd_high(const bdd &);
   friend int      bdd_scanset(const bdd &, int *&, int &);
   friend bdd      bdd_makesetpp(int *, int);
   friend int      bdd_setbddpair(bddPair*, int, const bdd &);
   friend int      bdd_setbddpairs(bddPair*, int*, const bdd *, int);
   friend bdd      bdd_from_int(int i);
   friend bdd      bdd_buildcube(int, int, const bdd *);
   friend bdd      bdd_ibuildcubepp(int, int, int *);
   friend bdd      bdd_not(const bdd &);
   friend bdd      bdd_simplify(const bdd &, const bdd &);
   friend bdd      bdd_apply(const bdd &, const bdd &, int);
   friend bdd      bdd_and(const bdd &, const bdd &);
   friend bdd      bdd_or(const bdd &, const bdd &);
   friend bdd      bdd_xor(const bdd &, const bdd &);
   friend bdd      bdd_imp(const bdd &, const bdd &);
   friend bdd      bdd_biimp(const bdd &, const bdd &);
   friend bdd      bdd_setxor(const bdd &, const bdd &);
   friend int      bdd_implies(const bdd &, const bdd &) noexcept;
   friend bdd      bdd_ite(const bdd &, const bdd &, const bdd &);
   friend bdd      bdd_restrict(const bdd &, const bdd &);
   friend bdd      bdd_constrain(const bdd &, const bdd &);
   friend bdd      bdd_exist(const bdd &, const bdd &);
   friend bdd      bdd_existcomp(const bdd &, const bdd &);
   friend bdd      bdd_forall(const bdd &, const bdd &);
   friend bdd      bdd_forallcomp(const bdd &, const bdd &);
   friend bdd      bdd_unique(const bdd &, const bdd &);
   friend bdd      bdd_uniquecomp(const bdd &, const bdd &);
   friend bdd      bdd_appex(const bdd &, const bdd &, int, const bdd &);
   friend bdd      bdd_appexcomp(const bdd &, const bdd &, int, const bdd &);
   friend bdd      bdd_appall(const bdd &, const bdd &, int, const bdd &);
   friend bdd      bdd_appallcomp(const bdd &, const bdd &, int, const bdd &);
   friend bdd      bdd_appuni(const bdd &, const bdd &, int, const bdd &);
   friend bdd      bdd_appunicomp(const bdd &, const bdd &, int, const bdd &);
   friend bdd      bdd_replace(const bdd &, bddPair*);
   friend bdd      bdd_compose(const bdd &, const bdd &, int);
   friend bdd      bdd_veccompose(const bdd &, bddPair*);
   friend bdd      bdd_support(const bdd &);
   friend bdd      bdd_satone(const bdd &);
   friend bdd      bdd_satoneset(const bdd &, const bdd &, const bdd &);
   friend bdd      bdd_fullsatone(const bdd &);
   friend bdd      bdd_satprefix(bdd &);
   friend void     bdd_allsat(const bdd &r, bddallsathandler handler);
   friend void     bdd_allsat(const bdd &r, bddallsathandler_old handler);
   friend double   bdd_satcount(const bdd &);
   friend double   bdd_satcountset(const bdd &, const bdd &);
   friend double   bdd_satcountln(const bdd &);
   friend double   bdd_satcountlnset(const bdd &, const bdd &);
   friend int      bdd_nodecount(const bdd &);
   friend int      bdd_anodecountpp(const bdd *, int);
   friend int*     bdd_varprofile(const bdd &);
   friend double   bdd_pathcount(const bdd &);

   friend void   bdd_fprinttable(FILE *, const bdd &);
   friend void   bdd_printtable(const bdd &);
   friend void   bdd_fprintset(FILE *, const bdd &);
   friend void   bdd_printset(const bdd &);
   friend void   bdd_printdot(const bdd &);
   friend int    bdd_fnprintdot(char*, const bdd &);
   friend void   bdd_fprintdot(FILE*, const bdd &);
   friend std::ostream &operator<<(std::ostream &, const bdd &);
   friend int    bdd_fnsave(char*, const bdd &);
   friend int    bdd_save(FILE*, const bdd &);
   friend int    bdd_fnload(char*, bdd &);
   friend int    bdd_load(FILE*, bdd &);

   friend bdd    fdd_ithvarpp(int, int);
   friend bdd    fdd_ithsetpp(int);
   friend bdd    fdd_domainpp(int);
   friend int    fdd_scanvar(const bdd &, int);
   friend int*   fdd_scanallvar(const bdd &);
   friend bdd    fdd_equalspp(int, int);
   friend void   fdd_printset(const bdd &);
   friend void   fdd_fprintset(FILE*, const bdd &);
   friend bdd    fdd_makesetpp(int*, int);
   friend int    fdd_scanset(const bdd &, int *&, int &);

   friend int    bdd_addvarblock(const bdd &, int);

   friend class bvec;
   friend bvec bvec_ite(const bdd& a, const bvec& b, const bvec& c);
   friend bvec bvec_shlfixed(const bvec &e, int pos, const bdd &c);
   friend bvec bvec_shl(const bvec &left, const bvec &right, const bdd &c);
   friend bvec bvec_shrfixed(const bvec &e, int pos, const bdd &c);
   friend bvec bvec_shr(const bvec &left, const bvec &right, const bdd &c);
   friend bdd  bvec_lth(const bvec &left, const bvec &right);
   friend bdd  bvec_lte(const bvec &left, const bvec &right);
   friend bdd  bvec_gth(const bvec &left, const bvec &right);
   friend bdd  bvec_gte(const bvec &left, const bvec &right);
   friend bdd  bvec_equ(const bvec &left, const bvec &right);
   friend bdd  bvec_neq(const bvec &left, const bvec &right);
};

class bddxfalse: public bdd
{
public:
  bddxfalse(void) noexcept
    {
      root=0;
    }
};

class bddxtrue: public bdd
{
public:
  bddxtrue(void) noexcept
    {
      root=1;
    }
};

/*=== BDD constants ====================================================*/

BUDDY_API_VAR const bddxfalse bddfalsepp;
BUDDY_API_VAR const bddxtrue bddtruepp;

#define bddtrue bddtruepp
#define bddfalse bddfalsepp

/*=== C++ interface ====================================================*/

BUDDY_API int bdd_cpp_init(int, int);

inline void bdd_stats(bddStat& s)
{ bdd_stats(&s); }

inline bdd bdd_ithvarpp(int v)
{ return bdd_ithvar(v); }

inline bdd bdd_nithvarpp(int v)
{ return bdd_nithvar(v); }

inline int bdd_var(const bdd &r)
{ return bdd_var(r.root); }

inline bdd bdd_low(const bdd &r)
{ return bdd_low(r.root); }

inline bdd bdd_high(const bdd &r)
{ return bdd_high(r.root); }

inline int bdd_scanset(const bdd &r, int *&v, int &n)
{ return bdd_scanset(r.root, &v, &n); }

inline bdd bdd_makesetpp(int *v, int n)
{ return bdd(bdd_makeset(v,n)); }

inline int bdd_setbddpair(bddPair *p, int ov, const bdd &nv)
{ return bdd_setbddpair(p,ov,nv.root); }

inline bdd bdd_from_int(int i)
{ return i; }

   /* In bddop.c */

inline bdd bdd_replace(const bdd &r, bddPair *p)
{ return bdd_replace(r.root, p); }

inline bdd bdd_compose(const bdd &f, const bdd &g, int v)
{ return bdd_compose(f.root, g.root, v); }

inline bdd bdd_veccompose(const bdd &f, bddPair *p)
{ return bdd_veccompose(f.root, p); }

inline bdd bdd_restrict(const bdd &r, const bdd &var)
{ return bdd_restrict(r.root, var.root); }

inline bdd bdd_constrain(const bdd &f, const bdd &c)
{ return bdd_constrain(f.root, c.root); }

inline bdd bdd_simplify(const bdd &d, const bdd &b)
{ return bdd_simplify(d.root, b.root); }

inline bdd bdd_ibuildcubepp(int v, int w, int *a)
{ return bdd_ibuildcube(v,w,a); }

inline bdd bdd_not(const bdd &r)
{ return bdd_not(r.root); }

inline bdd bdd_apply(const bdd &l, const bdd &r, int op)
{ return bdd_apply(l.root, r.root, op); }

inline bdd bdd_and(const bdd &l, const bdd &r)
{ return bdd_apply(l.root, r.root, bddop_and); }

inline bdd bdd_or(const bdd &l, const bdd &r)
{ return bdd_apply(l.root, r.root, bddop_or); }

inline bdd bdd_xor(const bdd &l, const bdd &r)
{ return bdd_apply(l.root, r.root, bddop_xor); }

inline bdd bdd_imp(const bdd &l, const bdd &r)
{ return bdd_apply(l.root, r.root, bddop_imp); }

inline bdd bdd_biimp(const bdd &l, const bdd &r)
{ return bdd_apply(l.root, r.root, bddop_biimp); }

inline bdd bdd_setxor(const bdd &l, const bdd &r)
{ return bdd_setxor(l.root, r.root); }

inline int bdd_implies(const bdd &l, const bdd &r) noexcept
{ return bdd_implies(l.root, r.root); }

inline bdd bdd_ite(const bdd &f, const bdd &g, const bdd &h)
{ return bdd_ite(f.root, g.root, h.root); }

inline bdd bdd_exist(const bdd &r, const bdd &var)
{ return bdd_exist(r.root, var.root); }

inline bdd bdd_existcomp(const bdd &r, const bdd &var)
{ return bdd_existcomp(r.root, var.root); }

inline bdd bdd_forall(const bdd &r, const bdd &var)
{ return bdd_forall(r.root, var.root); }

inline bdd bdd_forallcomp(const bdd &r, const bdd &var)
{ return bdd_forallcomp(r.root, var.root); }

inline bdd bdd_unique(const bdd &r, const bdd &var)
{ return bdd_unique(r.root, var.root); }

inline bdd bdd_uniquecomp(const bdd &r, const bdd &var)
{ return bdd_uniquecomp(r.root, var.root); }

inline bdd bdd_appex(const bdd &l, const bdd &r, int op, const bdd &var)
{ return bdd_appex(l.root, r.root, op, var.root); }

inline bdd bdd_appexcomp(const bdd &l, const bdd &r, int op, const bdd &var)
{ return bdd_appexcomp(l.root, r.root, op, var.root); }

inline bdd bdd_appall(const bdd &l, const bdd &r, int op, const bdd &var)
{ return bdd_appall(l.root, r.root, op, var.root); }

inline bdd bdd_appallcomp(const bdd &l, const bdd &r, int op, const bdd &var)
{ return bdd_appallcomp(l.root, r.root, op, var.root); }

inline bdd bdd_appuni(const bdd &l, const bdd &r, int op, const bdd &var)
{ return bdd_appuni(l.root, r.root, op, var.root); }

inline bdd bdd_appunicomp(const bdd &l, const bdd &r, int op, const bdd &var)
{ return bdd_appunicomp(l.root, r.root, op, var.root); }

inline bdd bdd_support(const bdd &r)
{ return bdd_support(r.root); }

inline bdd bdd_satone(const bdd &r)
{ return bdd_satone(r.root); }

inline bdd bdd_satoneset(const bdd &r, const bdd &var, const bdd &pol)
{ return bdd_satoneset(r.root, var.root, pol.root); }

inline bdd bdd_fullsatone(const bdd &r)
{ return bdd_fullsatone(r.root); }

inline bdd bdd_satprefix(bdd &r)
{ int ro = r.root; bdd res = bdd_satprefix(&ro); r = bdd(ro); return res; }

inline void bdd_allsat(const bdd &r, bddallsathandler handler)
{ bdd_allsat(r.root, handler); }

// backward compatibility for C++ users
inline void bdd_allsat(const bdd &r, bddallsathandler_old handler)
{ bdd_allsat(r.root, (bddallsathandler)handler); }

inline double bdd_satcount(const bdd &r)
{ return bdd_satcount(r.root); }

inline double bdd_satcountset(const bdd &r, const bdd &varset)
{ return bdd_satcountset(r.root, varset.root); }

inline double bdd_satcountln(const bdd &r)
{ return bdd_satcountln(r.root); }

inline double bdd_satcountlnset(const bdd &r, const bdd &varset)
{ return bdd_satcountlnset(r.root, varset.root); }

inline int bdd_nodecount(const bdd &r)
{ return bdd_nodecount(r.root); }

inline int* bdd_varprofile(const bdd &r)
{ return bdd_varprofile(r.root); }

inline double bdd_pathcount(const bdd &r)
{ return bdd_pathcount(r.root); }


   /* I/O extensions */

inline void bdd_fprinttable(FILE *file, const bdd &r)
{ bdd_fprinttable(file, r.root); }

inline void bdd_printtable(const bdd &r)
{ bdd_printtable(r.root); }

inline void bdd_fprintset(FILE *file, const bdd &r)
{ bdd_fprintset(file, r.root); }

inline void bdd_printset(const bdd &r)
{ bdd_printset(r.root); }

inline void bdd_printdot(const bdd &r)
{ bdd_printdot(r.root); }

inline void bdd_fprintdot(FILE* ofile, const bdd &r)
{ bdd_fprintdot(ofile, r.root); }

inline int bdd_fnprintdot(char* fname, const bdd &r)
{ return bdd_fnprintdot(fname, r.root); }

inline int bdd_fnsave(char *fname, const bdd &r)
{ return bdd_fnsave(fname, r.root); }

inline int bdd_save(FILE *ofile, const bdd &r)
{ return bdd_save(ofile, r.root); }

inline int bdd_fnload(char *fname, bdd &r)
{ int lr,e; e=bdd_fnload(fname, &lr); r=bdd(lr); return e; }

inline int bdd_load(FILE *ifile, bdd &r)
{ int lr,e; e=bdd_load(ifile, &lr); r=bdd(lr); return e; }

inline int bdd_addvarblock(const bdd &v, int f)
{ return bdd_addvarblock(v.root, f); }

   /* Hack to allow for overloading */
#define bdd_init bdd_cpp_init
#define bdd_ithvar bdd_ithvarpp
#define bdd_nithvar bdd_nithvarpp
#define bdd_makeset bdd_makesetpp
#define bdd_ibuildcube bdd_ibuildcubepp
#define bdd_anodecount bdd_anodecountpp

/*=== Inline C++ functions =============================================*/

inline int bdd::id(void) const noexcept
{ return root; }

inline bdd bdd::operator&(const bdd &r) const
{ return bdd_apply(*this,r,bddop_and); }

inline bdd& bdd::operator&=(const bdd &r)
{ return (*this=bdd_apply(*this,r,bddop_and)); }

inline bdd bdd::operator^(const bdd &r) const
{ return bdd_apply(*this,r,bddop_xor); }

inline bdd& bdd::operator^=(const bdd &r)
{ return (*this=bdd_apply(*this,r,bddop_xor)); }

inline bdd bdd::operator|(const bdd &r) const
{ return bdd_apply(*this,r,bddop_or); }

inline bdd& bdd::operator|=(const bdd &r)
{ return (*this=bdd_apply(*this,r,bddop_or)); }

inline bdd bdd::operator!(void) const
{ return bdd_not(*this);}

inline bdd bdd::operator>>(const bdd &r) const
{ return bdd_apply(*this,r,bddop_imp); }

inline bdd& bdd::operator>>=(const bdd &r)
{ return (*this=bdd_apply(*this,r,bddop_imp)); }

inline bdd bdd::operator-(const bdd &r) const
{ return bdd_apply(*this,r,bddop_diff); }

inline bdd& bdd::operator-=(const bdd &r)
{ return (*this=bdd_apply(*this,r,bddop_diff)); }

inline bdd bdd::operator>(const bdd &r) const
{ return bdd_apply(*this,r,bddop_diff); }

inline bdd bdd::operator<(const bdd &r) const
{ return bdd_apply(*this,r,bddop_less); }

inline bdd bdd::operator<<(const bdd &r) const
{ return bdd_apply(*this,r,bddop_invimp); }

inline bdd& bdd::operator<<=(const bdd &r)
{ return (*this=bdd_apply(*this,r,bddop_invimp)); }

inline int bdd::operator==(const bdd &r) const noexcept
{ return r.root==root; }

inline int bdd::operator==(const bddxfalse&) const noexcept
{ return root==0; }

inline int bdd::operator==(const bddxtrue&) const noexcept
{ return root==1; }

inline int bdd::operator!=(const bdd &r) const noexcept
{ return r.root!=root; }

inline int bdd::operator!=(const bddxfalse&) const noexcept
{ return root!=0; }

inline int bdd::operator!=(const bddxtrue&) const noexcept
{ return root!=1; }

inline bdd& bdd::operator=(const bdd &r) noexcept
{
  if (__likely(root != r.root))
    {
      if (root > 1)
	bdd_delref_nc(root);
      root = r.root;
      if (root > 1)
	bdd_addref_nc(root);
    }
  return *this;
}

inline bdd& bdd::operator=(bdd&& r) noexcept
{
  if (root > 1)
    bdd_delref_nc(root);
  root = r.root;
  r.root = 0;
  return *this;
}

inline bdd& bdd::operator=(int r) noexcept
{
  if (__likely(root != r))
    {
      if (root > 1)
	bdd_delref_nc(root);
      root = r;
      if (root > 1)
	bdd_addref_nc(root);
    }
  return *this;
}

inline bdd& bdd::operator=(const bddxfalse &) noexcept
{
  if (root > 1)
    bdd_delref_nc(root);
  root = 0;
  return *this;
}

inline bdd& bdd::operator=(const bddxtrue &) noexcept
{
  if (root > 1)
    bdd_delref_nc(root);
  root = 1;
  return *this;
}

inline bddxtrue bdd_true(void)
{ return bddxtrue(); }

inline bddxfalse bdd_false(void)
{ return bddxfalse(); }


/*=== Iostream printing ================================================*/

class BUDDY_API bdd_ioformat
{
 public:
   bdd_ioformat(int f) { format=f; }
 private:
   BUDDY_LOCAL bdd_ioformat(void)  { }
   int format;
   BUDDY_LOCAL static int curformat;

   friend std::ostream &operator<<(std::ostream &, const bdd_ioformat &);
   friend std::ostream &operator<<(std::ostream &, const bdd &);
};

BUDDY_API std::ostream &operator<<(std::ostream &, const bdd &);
BUDDY_API std::ostream &operator<<(std::ostream &, const bdd_ioformat &);

BUDDY_API_VAR bdd_ioformat bddset;
BUDDY_API_VAR bdd_ioformat bddtable;
BUDDY_API_VAR bdd_ioformat bdddot;
BUDDY_API_VAR bdd_ioformat bddall;
BUDDY_API_VAR bdd_ioformat fddset;

typedef void (*bddstrmhandler)(std::ostream &, int);

BUDDY_API bddstrmhandler bdd_strm_hook(bddstrmhandler);

#endif /* CPLUSPLUS */

#endif /* _BDDX_H */

/* EOF */
