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
  $Header: /Volumes/CVS/repository/spot/spot/buddy/src/pairs.c,v 1.7 2003/05/22 15:07:27 aduret Exp $
  FILE:  pairs.c
  DESCR: Pair management for BDD package.
  AUTH:  Jorn Lind
  DATE:  february 1997
*************************************************************************/
#include <stdlib.h>
#include <limits.h>
#include "kernel.h"

/*======================================================================*/

static int      pairsid;            /* Pair identifier */
static bddPair* pairs;              /* List of all replacement pairs in use */


/*************************************************************************
*************************************************************************/

void bdd_pairs_init(void)
{
   pairsid = 0;
   pairs = NULL;
}


void bdd_pairs_done(void)
{
   bddPair *p = pairs;
   int n;

   while (p != NULL)
   {
      bddPair *next = p->next;
      for (n=0 ; n<bddvarnum ; n++)
	 bdd_delref( p->result[n] );
      free(p->result);
      free(p);
      p = next;
   }
}


static int update_pairsid(void)
{
   pairsid++;

   if (pairsid == (INT_MAX >> 2))
   {
      bddPair *p;
      pairsid = 0;
      for (p=pairs ; p!=NULL ; p=p->next)
	 p->id = pairsid++;
      bdd_operator_reset();
   }

   return pairsid;
}


void bdd_register_pair(bddPair *p)
{
   p->next = pairs;
   pairs = p;
}


void bdd_pairs_vardown(int level)
{
   bddPair *p;

   for (p=pairs ; p!=NULL ; p=p->next)
   {
      int tmp;

      tmp = p->result[level];
      p->result[level] = p->result[level+1];
      p->result[level+1] = tmp;

      if (p->last == level)
	 p->last++;
   }
}


static bddPair *bdd_pairalloc(void)
{
   bddPair *p;
   if ((p=(bddPair*)malloc(sizeof(bddPair))) == NULL)
   {
      bdd_error(BDD_MEMORY);
      return NULL;
   }

   if ((p->result=(BDD*)malloc(sizeof(BDD)*bddvarnum)) == NULL)
   {
      free(p);
      bdd_error(BDD_MEMORY);
      return NULL;
   }
   return p;
}


int bdd_pairs_resize(int oldsize, int newsize)
{
   bddPair *p;
   int n;

   for (p=pairs ; p!=NULL ; p=p->next)
   {
      if ((p->result=(BDD*)realloc(p->result,sizeof(BDD)*newsize)) == NULL)
	 return bdd_error(BDD_MEMORY);

      for (n=oldsize ; n<newsize ; n++)
	 p->result[n] = bdd_ithvar(bddlevel2var[n]);
   }

   return 0;
}


/*
NAME    {* bdd\_newpair *}
SECTION {* kernel *}
SHORT   {* creates an empty variable pair table *}
PROTO   {* bddPair *bdd_newpair(void) *}
DESCR   {* Variable pairs of the type {\tt bddPair} are used in
	   {\tt bdd\_replace} to define which variables to replace with
	   other variables. This function allocates such an empty table. The
	   table can be freed by a call to {\tt bdd\_freepair}. *}
RETURN  {* Returns a new table of pairs. *}
ALSO    {* bdd\_freepair, bdd\_replace, bdd\_setpair, bdd\_setpairs *}
*/
bddPair *bdd_newpair(void)
{
   int n;
   bddPair *p;

   p = bdd_pairalloc();
   if (p == NULL)
     return NULL;

   for (n=0 ; n<bddvarnum ; n++)
      p->result[n] = bdd_ithvar(bddlevel2var[n]);

   p->id = update_pairsid();
   p->last = -1;

   bdd_register_pair(p);
   return p;
}


/*
NAME    {* bdd\_copypair *}
SECTION {* kernel *}
SHORT   {* clone a pair table *}
PROTO   {* bddPair *bdd_copypair(bddPair *from) *}
DESCR   {* Duplicate the table of pairs {\tt from}.
	   This function allocates the cloned table. The
	   table can be freed by a call to {\tt bdd\_freepair}. *}
RETURN  {* Returns a new table of pairs. *}
ALSO    {* bdd\_newpair, bdd\_freepair, bdd\_replace, bdd\_setpair, bdd\_setpairs *}
*/
bddPair *bdd_copypair(bddPair *from)
{
   int n;
   bddPair *p;

   p = bdd_pairalloc();
   if (p == NULL)
     return NULL;

   for (n=0 ; n<bddvarnum ; n++)
     p->result[n] = bdd_addref(from->result[n]);

   p->id = update_pairsid();
   p->last = from->last;

   bdd_register_pair(p);
   return p;
}

/*
NAME    {* bdd\_mergepairs *}
SECTION {* kernel *}
SHORT   {* merge two pair tables *}
PROTO   {* bddPair *bdd_mergepairs(bddPair *left, bddPairs *right) *}
DESCR   {* Create a table of pairs that can be used to perform the
	   rewritings of both {\tt left} and {\tt right}.  This cannot work if
	   the two tables contain incompatible rewrite pairs (for instance
	   if left rewrite 1 to 2, and right rewrite 1 to 3).

	   This function allocates a new table. The
	   table can be freed by a call to {\tt bdd\_freepair}. *}
RETURN  {* Returns a new table of pairs. *}
ALSO    {* bdd\_newpair, bdd\_freepair, bdd\_replace, bdd\_setpair, bdd\_setpairs *}
*/
bddPair *bdd_mergepairs(bddPair *left, bddPair *right)
{
   int n;
   bddPair *p;

   p = bdd_copypair(left);
   if (p == NULL)
     return NULL;

   for (n=0; n<bddvarnum; n++)
     {
       bdd nl = bdd_ithvar(n);

       /* If P performs no rewriting for N, blindly use that of RIGHT.  */
       if (nl == p->result[n])
	 {
	   bdd_delref(p->result[n]);
	   p->result[n] = bdd_addref(right->result[n]);
	 }
       else
	 /* If P rewrites N, make sure RIGHT doesn't rewrite it, or that
	    it rewrites it to the same variable.  */
	 if (nl != right->result[n] && right->result[n] != p->result[n])
	   {
	     bdd_freepair(p);
	     bdd_error(BDD_INVMERGE);
	     return NULL;
	   }
     }

   if (p->last < right->last)
     p->last = right->last;

   /* We don't have to update the ID, because this pair was not
      used since bdd_copypair were called. */
   return p;
}


/*
NAME    {* bdd\_setpair *}
EXTRA   {* bdd\_setbddpair *}
SECTION {* kernel *}
SHORT   {* set one variable pair *}
PROTO   {* int bdd_setpair(bddPair *pair, int oldvar, int newvar)
int bdd_setbddpair(bddPair *pair, int oldvar, BDD newvar) *}
DESCR   {* Adds the pair {\tt (oldvar,newvar)} to the table of pairs
	   {\tt pair}. This results in {\tt oldvar} being substituted
	   with {\tt newvar} in a call to {\tt bdd\_replace}. In the first
	   version {\tt newvar} is an integer representing the variable
	   to be replaced with the old variable.
	   In the second version {\tt oldvar} is a BDD.
	   In this case the variable {\tt oldvar} is substituted with the
	   BDD {\tt newvar}.
	   The possibility to substitute with any BDD as {\tt newvar} is
	   utilized in bdd\_compose, whereas only the topmost variable
	   in the BDD is used in bdd\_replace. *}
RETURN  {* Zero on success, otherwise a negative error code. *}
ALSO    {* bdd\_newpair, bdd\_setpairs, bdd\_resetpair, bdd\_replace, bdd\_compose *}
*/
int bdd_setpair(bddPair *pair, int oldvar, int newvar)
{
   if (pair == NULL)
      return 0;

   if (oldvar < 0  ||  oldvar > bddvarnum-1)
      return bdd_error(BDD_VAR);
   if (newvar < 0  ||  newvar > bddvarnum-1)
      return bdd_error(BDD_VAR);

   bdd_delref( pair->result[bddvar2level[oldvar]] );
   pair->result[bddvar2level[oldvar]] = bdd_ithvar(newvar);
   pair->id = update_pairsid();

   if (bddvar2level[oldvar] > pair->last)
      pair->last = bddvar2level[oldvar];

   return 0;
}


int bdd_setbddpair(bddPair *pair, int oldvar, BDD newvar)
{
   int oldlevel;

   if (pair == NULL)
      return 0;

   CHECK(newvar);
   if (oldvar < 0  ||  oldvar >= bddvarnum)
      return bdd_error(BDD_VAR);
   oldlevel = bddvar2level[oldvar];

   bdd_delref( pair->result[oldlevel] );
   pair->result[oldlevel] = bdd_addref(newvar);
   pair->id = update_pairsid();

   if (oldlevel > pair->last)
      pair->last = oldlevel;

   return 0;
}


/*
NAME    {* bdd\_setpairs *}
EXTRA   {* bdd\_setbddpairs *}
SECTION {* kernel *}
SHORT   {* defines a whole set of pairs *}
PROTO   {* int bdd_setpairs(bddPair *pair, int *oldvar, int *newvar, int size)
int bdd_setbddpairs(bddPair *pair, int *oldvar, BDD *newvar, int size) *}
DESCR   {* As for {\tt bdd\_setpair} but with {\tt oldvar} and {\tt newvar}
	   being arrays of variables (BDDs) of size {\tt size}. *}
RETURN  {* Zero on success, otherwise a negative error code. *}
ALSO    {* bdd\_newpair, bdd\_setpair, bdd\_replace, bdd\_compose *}
*/
int bdd_setpairs(bddPair *pair, int *oldvar, int *newvar, int size)
{
   int n,e;
   if (pair == NULL)
      return 0;

   for (n=0 ; n<size ; n++)
      if ((e=bdd_setpair(pair, oldvar[n], newvar[n])) < 0)
	 return e;

   return 0;
}


int bdd_setbddpairs(bddPair *pair, int *oldvar, BDD *newvar, int size)
{
   int n,e;
   if (pair == NULL)
      return 0;

   for (n=0 ; n<size ; n++)
      if ((e=bdd_setbddpair(pair, oldvar[n], newvar[n])) < 0)
	 return e;

   return 0;
}


/*
NAME    {* bdd\_freepair *}
SECTION {* kernel *}
SHORT   {* frees a table of pairs *}
PROTO   {* void bdd_freepair(bddPair *pair) *}
DESCR   {* Frees the table of pairs {\tt pair} that has been allocated
	   by a call to {\tt bdd\_newpair}. *}
ALSO    {* bdd\_replace, bdd\_newpair, bdd\_setpair, bdd\_resetpair *}
*/
void bdd_freepair(bddPair *p)
{
   int n;

   if (p == NULL)
      return;

   if (pairs != p)
   {
      bddPair *bp = pairs;
      while (bp != NULL  &&  bp->next != p)
	 bp = bp->next;

      if (bp != NULL)
	 bp->next = p->next;
   }
   else
      pairs = p->next;

   for (n=0 ; n<bddvarnum ; n++)
      bdd_delref( p->result[n] );
   free(p->result);
   free(p);
}


/*
NAME    {* bdd\_resetpair *}
SECTION {* kernel *}
SHORT   {* clear all variable pairs *}
PROTO   {* void bdd_resetpair(bddPair *pair) *}
DESCR   {* Resets the table of pairs {\tt pair} by setting all substitutions
	   to their default values (that is no change). *}
ALSO    {* bdd\_newpair, bdd\_setpair, bdd\_freepair *}
*/
void bdd_resetpair(bddPair *p)
{
   int n;

   for (n=0 ; n<bddvarnum ; n++)
      p->result[n] = bdd_ithvar(n);
   p->last = 0;
}


/* EOF */
