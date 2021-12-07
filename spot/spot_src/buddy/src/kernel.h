/*========================================================================
	       Copyright (C) 1996-2002 by Jorn Lind-Nielsen
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
  $Header: /Volumes/CVS/repository/spot/spot/buddy/src/kernel.h,v 1.5 2004/06/28 15:22:13 adl Exp $
  FILE:  kernel.h
  DESCR: Kernel specific definitions for BDD package
  AUTH:  Jorn Lind
  DATE:  (C) june 1997
*************************************************************************/

#ifndef _KERNEL_H
#define _KERNEL_H

/*=== Includes =========================================================*/

#include <limits.h>
#include <setjmp.h>
#include "bddx.h"
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/*=== SANITY CHECKS ====================================================*/

   /* Make sure we use at least 32 bit integers */
#if (INT_MAX < 0x7FFFFFFF)
#error The compiler does not support 4 byte integers!
#endif

#ifdef NDEBUG
#define CHECK(r) (void)(r);
#define CHECKa(r,a) (void)(r); (void)(a);
#define CHECKn(r) (void)(r);
#define CHECKnc(r) (void)(r);
#else
   /* Sanity check argument and return eventual error code */
#define CHECK(r)\
   if (!bddrunning) return bdd_error(BDD_RUNNING);\
   else if ((r) < 0  ||  (r) >= bddnodesize) return bdd_error(BDD_ILLBDD);\
   else if (r >= 2 && LOW(r) == -1) return bdd_error(BDD_ILLBDD)\

   /* Sanity check argument and return eventually the argument 'a' */
#define CHECKa(r,a)\
   if (!bddrunning) { bdd_error(BDD_RUNNING); return (a); }\
   else if ((r) < 0  ||  (r) >= bddnodesize)\
     { bdd_error(BDD_ILLBDD); return (a); }\
   else if (r >= 2 && LOW(r) == -1)\
     { bdd_error(BDD_ILLBDD); return (a); }

#define CHECKn(r)\
   if (!bddrunning) { bdd_error(BDD_RUNNING); return; }\
   else if ((r) < 0  ||  (r) >= bddnodesize)\
     { bdd_error(BDD_ILLBDD); return; }\
   else if (r >= 2 && LOW(r) == -1)\
     { bdd_error(BDD_ILLBDD); return; }

   /* r is non-constant */
#define CHECKnc(r)\
   if (root < 2) \
      return bdd_error(BDD_ILLBDD)

#endif

/*=== SEMI-INTERNAL TYPES ==============================================*/

typedef struct s_BddNode /* Node table entry */
{
   unsigned int refcou : 10;
   unsigned int level  : 22;
   int low;
   int high;
   int next;
} BddNode;

/* This structure is used during initialization of the above
   structure, to help gcc vectorize the initializing loop. */
typedef struct s_BddNodeInit
{
   int z;
   int low;
   int high;
   int next;
} BddNodeInit;

/*=== KERNEL VARIABLES =================================================*/

#ifdef CPLUSPLUS
extern "C" {
#endif

extern int       bddrunning;         /* Flag - package initialized */
extern int       bdderrorcond;       /* Some error condition was met */
extern int       bddnodesize;        /* Number of allocated nodes */
extern int       bddhashsize;        /* Size of node hash tableq */
extern int       bddmaxnodesize;     /* Maximum allowed number of nodes */
extern int       bddmaxnodeincrease; /* Max. # of nodes used to inc. table */
extern BddNode*  bddnodes;           /* All of the bdd nodes */
extern int*      bddhash;            /* Unicity hash table */
extern int       bddvarnum;          /* Number of defined BDD variables */
extern int*      bddrefstack;        /* Internal node reference stack */
extern int*      bddrefstacktop;     /* Internal node reference stack top */
extern int*      bddvar2level;
extern int*      bddlevel2var;
extern jmp_buf   bddexception;
extern int       bddreorderdisabled;
extern int       bddresized;
extern int       bddcachesize;
extern bddCacheStat bddcachestats;

  /* from reorder.c */
extern int bddreordermethod;

#ifdef CPLUSPLUS
}
#endif


/*=== KERNEL DEFINITIONS ===============================================*/

#define MAXVAR 0x1FFFFF
#define MAXREF 0x3FF

   /* Reference counting */
#define DECREF(n) if (bddnodes[n].refcou!=MAXREF && bddnodes[n].refcou>0) bddnodes[n].refcou--
#define INCREF(n) if (bddnodes[n].refcou<MAXREF) bddnodes[n].refcou++
#define DECREFp(n) if (n->refcou!=MAXREF && n->refcou>0) n->refcou--
#define INCREFp(n) if (n->refcou<MAXREF) n->refcou++
#define HASREF(n) (bddnodes[n].refcou > 0)

   /* Marking BDD nodes */
#define MARKON   0x200000    /* Bit used to mark a node (1) */
#define MARKOFF  0x1FFFFF    /* - unmark */
#define MARKHIDE 0x1FFFFF
#define SETMARK(n)  (bddnodes[n].level |= MARKON)
#define UNMARK(n)   (bddnodes[n].level &= MARKOFF)
#define MARKED(n)   (bddnodes[n].level & MARKON)
#define SETMARKp(p) (node->level |= MARKON)
#define UNMARKp(p)  (node->level &= MARKOFF)
#define MARKEDp(p)  (node->level & MARKON)

   /* Hashfunctions */

#define PAIR(a,b)     (((unsigned)a)+((unsigned)b)*12582917U)
#define TRIPLE(a,b,c) (((unsigned)a)+((unsigned)b)*12582917U+((unsigned)c)*4256249U)

   /* Inspection of BDD nodes */
#define ISCONST(a) ((a) < 2)
#define ISNONCONST(a) ((a) >= 2)
#define ISONE(a)   ((a) == 1)
#define ISZERO(a)  ((a) == 0)
#define LEVEL(a)   (bddnodes[a].level)
#define LOW(a)     (bddnodes[a].low)
#define HIGH(a)    (bddnodes[a].high)
#define LEVELp(p)   ((p)->level)
#define LOWp(p)     ((p)->low)
#define HIGHp(p)    ((p)->high)

   /* Stacking for garbage collector */
#define INITREF    bddrefstacktop = bddrefstack
/*
** The following (original) definition of PUSHREF is incorrect
** when (a) can trigger a garbage collect:
**
**   #define PUSHREF(a) *(bddrefstacktop++) = (a)
**
** The reason is that bddrefstacktop can be incremented before computing
** (a) and assigning its value to bddrefstacktop[-1].  So if the garbage
** collector is triggered during the computation of (a), it will find
** uninitialized values in the stack.
**
** Such a situation occur in most algorithms of the form:
**
**   static int restrict_rec(int r) {
**     // ...
**     PUSHREF( restrict_rec(LOW(r)) );
**     PUSHREF( restrict_rec(HIGH(r)) );
**     res = bdd_makenode(LEVEL(r), READREF(2), READREF(1));
**     POPREF(2);
**
** Using a function forces the value of a to be computed before pushing
** it to the stack.
*/
static inline int PUSHREF(int a)
{
  return *bddrefstacktop++ = a;
}
#define READREF(a) *(bddrefstacktop-(a))
#define POPREF(a)  bddrefstacktop -= (a)

#define BDDONE 1
#define BDDZERO 0

#ifndef CLOCKS_PER_SEC
  /* Pass `CPPFLAGS=-DDEFAULT_CLOCK=1000' as an argument to ./configure
     to override this setting.  */
# ifndef DEFAULT_CLOCK
#  define DEFAULT_CLOCK 60
# endif
# define CLOCKS_PER_SEC DEFAULT_CLOCK
#endif

#define DEFAULTMAXNODEINC 50000

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define NEW(t,n) ( (t*)malloc(sizeof(t)*(n)) )


/*=== KERNEL PROTOTYPES ================================================*/

#ifdef CPLUSPLUS
extern "C" {
#endif

extern int    bdd_error(int);
extern int    bdd_makenode(unsigned int, int, int);
extern int    bdd_noderesize(int);
extern void   bdd_checkreorder(void);
extern void   bdd_mark(int);
extern void   bdd_mark_upto(int, int);
extern void   bdd_markcount(int, int*);
extern void   bdd_unmark(int);
extern void   bdd_unmark_upto(int, int);
extern void   bdd_register_pair(bddPair*);
extern int   *fdddec2bin(int, int);

extern int    bdd_operator_init(int);
extern void   bdd_operator_done(void);
extern void   bdd_operator_varresize(void);
extern void   bdd_operator_reset(void);

extern void   bdd_pairs_init(void);
extern void   bdd_pairs_done(void);
extern int    bdd_pairs_resize(int,int);
extern void   bdd_pairs_vardown(int);

extern void   bdd_fdd_init(void);
extern void   bdd_fdd_done(void);

extern void   bdd_reorder_init(void);
extern void   bdd_reorder_done(void);
extern int    bdd_reorder_ready(void) __purefn;
extern void   bdd_reorder_auto(void);
extern int    bdd_reorder_vardown(int);
extern int    bdd_reorder_varup(int);

extern void   bdd_cpp_init(void);

#ifdef CPLUSPLUS
}
#endif

#endif /* _KERNEL_H */


/* EOF */
