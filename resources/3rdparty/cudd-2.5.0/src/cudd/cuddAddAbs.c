/**CFile***********************************************************************

  FileName    [cuddAddAbs.c]

  PackageName [cudd]

  Synopsis    [Quantification functions for ADDs.]

  Description [External procedures included in this module:
		<ul>
		<li> Cudd_addExistAbstract()
		<li> Cudd_addUnivAbstract()
		<li> Cudd_addOrAbstract()
        <li> Cudd_addMinAbstract()
        <li> Cudd_addMaxAbstract()
		</ul>
	Internal procedures included in this module:
		<ul>
		<li> cuddAddExistAbstractRecur()
		<li> cuddAddUnivAbstractRecur()
		<li> cuddAddOrAbstractRecur()
        <li> cuddAddMinAbstractRecur()
        <li> cuddAddMaxAbstractRecur()
		</ul>
	Static procedures included in this module:
		<ul>
		<li> addCheckPositiveCube()
		</ul>]

  Author      [Fabio Somenzi]

  Copyright   [Copyright (c) 1995-2012, Regents of the University of Colorado

  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

  Neither the name of the University of Colorado nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.]

******************************************************************************/

#include "util.h"
#include "cuddInt.h"

/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Stucture declarations                                                     */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

#ifndef lint
static char rcsid[] DD_UNUSED = "$Id: cuddAddAbs.c,v 1.16 2012/02/05 01:07:18 fabio Exp $";
#endif

static	DdNode	*two;

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static int addCheckPositiveCube (DdManager *manager, DdNode *cube);

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis    [Existentially Abstracts all the variables in cube from f.]

  Description [Abstracts all the variables in cube from f by summing
  over all possible values taken by the variables. Returns the
  abstracted ADD.]

  SideEffects [None]

  SeeAlso     [Cudd_addUnivAbstract Cudd_bddExistAbstract
  Cudd_addOrAbstract]

******************************************************************************/
DdNode *
Cudd_addExistAbstract(
  DdManager * manager,
  DdNode * f,
  DdNode * cube)
{
    DdNode *res;

    two = cuddUniqueConst(manager,(CUDD_VALUE_TYPE) 2);
    if (two == NULL) return(NULL);
    cuddRef(two);

    if (addCheckPositiveCube(manager, cube) == 0) {
        (void) fprintf(manager->err,"Error: Can only abstract cubes");
        return(NULL);
    }

    do {
	manager->reordered = 0;
	res = cuddAddExistAbstractRecur(manager, f, cube);
    } while (manager->reordered == 1);

    if (res == NULL) {
	Cudd_RecursiveDeref(manager,two);
	return(NULL);
    }
    cuddRef(res);
    Cudd_RecursiveDeref(manager,two);
    cuddDeref(res);

    return(res);

} /* end of Cudd_addExistAbstract */


/**Function********************************************************************

  Synopsis    [Universally Abstracts all the variables in cube from f.]

  Description [Abstracts all the variables in cube from f by taking
  the product over all possible values taken by the variable. Returns
  the abstracted ADD if successful; NULL otherwise.]

  SideEffects [None]

  SeeAlso     [Cudd_addExistAbstract Cudd_bddUnivAbstract
  Cudd_addOrAbstract]

******************************************************************************/
DdNode *
Cudd_addUnivAbstract(
  DdManager * manager,
  DdNode * f,
  DdNode * cube)
{
    DdNode		*res;

    if (addCheckPositiveCube(manager, cube) == 0) {
	(void) fprintf(manager->err,"Error:  Can only abstract cubes");
	return(NULL);
    }

    do {
	manager->reordered = 0;
	res = cuddAddUnivAbstractRecur(manager, f, cube);
    } while (manager->reordered == 1);

    return(res);

} /* end of Cudd_addUnivAbstract */


/**Function********************************************************************

  Synopsis    [Disjunctively abstracts all the variables in cube from the
  0-1 ADD f.]

  Description [Abstracts all the variables in cube from the 0-1 ADD f
  by taking the disjunction over all possible values taken by the
  variables.  Returns the abstracted ADD if successful; NULL
  otherwise.]

  SideEffects [None]

  SeeAlso     [Cudd_addUnivAbstract Cudd_addExistAbstract]

******************************************************************************/
DdNode *
Cudd_addOrAbstract(
  DdManager * manager,
  DdNode * f,
  DdNode * cube)
{
    DdNode *res;

    if (addCheckPositiveCube(manager, cube) == 0) {
        (void) fprintf(manager->err,"Error: Can only abstract cubes");
        return(NULL);
    }

    do {
	manager->reordered = 0;
	res = cuddAddOrAbstractRecur(manager, f, cube);
    } while (manager->reordered == 1);
    return(res);

} /* end of Cudd_addOrAbstract */

/**Function********************************************************************
 
 Synopsis    [Abstracts all the variables in cube from the
 ADD f by taking the minimum.]
 
 Description [Abstracts all the variables in cube from the ADD f
 by taking the minimum over all possible values taken by the
 variables.  Returns the abstracted ADD if successful; NULL
 otherwise.]
 
 SideEffects [None]
 
 SeeAlso     [Cudd_addUnivAbstract Cudd_addExistAbstract]
 
 Note: Added by Dave Parker 14/6/99
 
 ******************************************************************************/
DdNode *
Cudd_addMinAbstract(
                    DdManager * manager,
                    DdNode * f,
                    DdNode * cube)
{
    DdNode *res;
    
    if (addCheckPositiveCube(manager, cube) == 0) {
        (void) fprintf(manager->err,"Error: Can only abstract cubes");
        return(NULL);
    }
    
    do {
        manager->reordered = 0;
        res = cuddAddMinAbstractRecur(manager, f, cube);
    } while (manager->reordered == 1);
    return(res);
    
} /* end of Cudd_addMinAbstract */


/**Function********************************************************************
 
 Synopsis    [Abstracts all the variables in cube from the
 ADD f by taking the maximum.]
 
 Description [Abstracts all the variables in cube from the ADD f
 by taking the maximum over all possible values taken by the
 variables.  Returns the abstracted ADD if successful; NULL
 otherwise.]
 
 SideEffects [None]
 
 SeeAlso     [Cudd_addUnivAbstract Cudd_addExistAbstract]
 
 Note: Added by Dave Parker 14/6/99
 
 ******************************************************************************/
DdNode *
Cudd_addMaxAbstract(
                    DdManager * manager,
                    DdNode * f,
                    DdNode * cube)
{
    DdNode *res;
    
    if (addCheckPositiveCube(manager, cube) == 0) {
        (void) fprintf(manager->err,"Error: Can only abstract cubes");
        return(NULL);
    }
    
    do {
        manager->reordered = 0;
        res = cuddAddMaxAbstractRecur(manager, f, cube);
    } while (manager->reordered == 1);
    return(res);
    
} /* end of Cudd_addMaxAbstract */

/**Function********************************************************************
 
 Synopsis    [Just like Cudd_addMinAbstract, but instead of abstracting the
 variables in the given cube, picks a unique representative that realizes th
 minimal function value.]
 
 Description [Returns the resulting ADD if successful; NULL otherwise.]
 
 SideEffects [None]
 
 SeeAlso     [Cudd_addMaxAbstractRepresentative]
 
 Note: Added by Christian Dehnert 8/5/14
 
 ******************************************************************************/
DdNode *
Cudd_addMinAbstractRepresentative(
                                  DdManager * manager,
                                  DdNode * f,
                                  DdNode * cube)
{
    DdNode *res;
    
    if (addCheckPositiveCube(manager, cube) == 0) {
        (void) fprintf(manager->err,"Error: Can only abstract cubes");
        return(NULL);
    }
    
    do {
        manager->reordered = 0;
        res = cuddAddMinAbstractRepresentativeRecur(manager, f, cube);
    } while (manager->reordered == 1);
    return(res);
    
} /* end of Cudd_addMinRepresentative */

/**Function********************************************************************
 
 Synopsis    [Just like Cudd_addMaxAbstract, but instead of abstracting the
 variables in the given cube, picks a unique representative that realizes th
 maximal function value.]
 
 Description [Returns the resulting ADD if successful; NULL otherwise.]
 
 SideEffects [None]
 
 SeeAlso     [Cudd_addMinAbstractRepresentative]
 
 Note: Added by Christian Dehnert 8/5/14
 
 ******************************************************************************/
DdNode *
Cudd_addMaxAbstractRepresentative(
                                  DdManager * manager,
                                  DdNode * f,
                                  DdNode * cube)
{
    DdNode *res;
    
    if (addCheckPositiveCube(manager, cube) == 0) {
        (void) fprintf(manager->err,"Error: Can only abstract cubes");
        return(NULL);
    }
    
    do {
        manager->reordered = 0;
        res = cuddAddMaxAbstractRepresentativeRecur(manager, f, cube);
    } while (manager->reordered == 1);
    return(res);
    
} /* end of Cudd_addMaxRepresentative */

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis    [Performs the recursive step of Cudd_addExistAbstract.]

  Description [Performs the recursive step of Cudd_addExistAbstract.
  Returns the ADD obtained by abstracting the variables of cube from f,
  if successful; NULL otherwise.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
DdNode *
cuddAddExistAbstractRecur(
  DdManager * manager,
  DdNode * f,
  DdNode * cube)
{
    DdNode	*T, *E, *res, *res1, *res2, *zero;

    statLine(manager);
    zero = DD_ZERO(manager);

    /* Cube is guaranteed to be a cube at this point. */	
    if (f == zero || cuddIsConstant(cube)) {  
        return(f);
    }

    /* Abstract a variable that does not appear in f => multiply by 2. */
    if (cuddI(manager,f->index) > cuddI(manager,cube->index)) {
	res1 = cuddAddExistAbstractRecur(manager, f, cuddT(cube));
	if (res1 == NULL) return(NULL);
	cuddRef(res1);
	/* Use the "internal" procedure to be alerted in case of
	** dynamic reordering. If dynamic reordering occurs, we
	** have to abort the entire abstraction.
	*/
	res = cuddAddApplyRecur(manager,Cudd_addTimes,res1,two);
	if (res == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    return(NULL);
	}
	cuddRef(res);
	Cudd_RecursiveDeref(manager,res1);
	cuddDeref(res);
        return(res);
    }

    if ((res = cuddCacheLookup2(manager, Cudd_addExistAbstract, f, cube)) != NULL) {
	return(res);
    }

    T = cuddT(f);
    E = cuddE(f);

    /* If the two indices are the same, so are their levels. */
    if (f->index == cube->index) {
	res1 = cuddAddExistAbstractRecur(manager, T, cuddT(cube));
	if (res1 == NULL) return(NULL);
        cuddRef(res1);
	res2 = cuddAddExistAbstractRecur(manager, E, cuddT(cube));
	if (res2 == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    return(NULL);
	}
        cuddRef(res2);
	res = cuddAddApplyRecur(manager, Cudd_addPlus, res1, res2);
	if (res == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    Cudd_RecursiveDeref(manager,res2);
	    return(NULL);
	}
	cuddRef(res);
	Cudd_RecursiveDeref(manager,res1);
	Cudd_RecursiveDeref(manager,res2);
	cuddCacheInsert2(manager, Cudd_addExistAbstract, f, cube, res);
	cuddDeref(res);
        return(res);
    } else { /* if (cuddI(manager,f->index) < cuddI(manager,cube->index)) */
	res1 = cuddAddExistAbstractRecur(manager, T, cube);
	if (res1 == NULL) return(NULL);
        cuddRef(res1);
	res2 = cuddAddExistAbstractRecur(manager, E, cube);
	if (res2 == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    return(NULL);
	}
        cuddRef(res2);
	res = (res1 == res2) ? res1 :
	    cuddUniqueInter(manager, (int) f->index, res1, res2);
	if (res == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    Cudd_RecursiveDeref(manager,res2);
	    return(NULL);
	}
	cuddDeref(res1);
	cuddDeref(res2);
	cuddCacheInsert2(manager, Cudd_addExistAbstract, f, cube, res);
        return(res);
    }	    

} /* end of cuddAddExistAbstractRecur */


/**Function********************************************************************

  Synopsis    [Performs the recursive step of Cudd_addUnivAbstract.]

  Description [Performs the recursive step of Cudd_addUnivAbstract.
  Returns the ADD obtained by abstracting the variables of cube from f,
  if successful; NULL otherwise.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
DdNode *
cuddAddUnivAbstractRecur(
  DdManager * manager,
  DdNode * f,
  DdNode * cube)
{
    DdNode	*T, *E, *res, *res1, *res2, *one, *zero;

    statLine(manager);
    one = DD_ONE(manager);
    zero = DD_ZERO(manager);

    /* Cube is guaranteed to be a cube at this point.
    ** zero and one are the only constatnts c such that c*c=c.
    */
    if (f == zero || f == one || cube == one) {  
	return(f);
    }

    /* Abstract a variable that does not appear in f. */
    if (cuddI(manager,f->index) > cuddI(manager,cube->index)) {
	res1 = cuddAddUnivAbstractRecur(manager, f, cuddT(cube));
	if (res1 == NULL) return(NULL);
	cuddRef(res1);
	/* Use the "internal" procedure to be alerted in case of
	** dynamic reordering. If dynamic reordering occurs, we
	** have to abort the entire abstraction.
	*/
	res = cuddAddApplyRecur(manager, Cudd_addTimes, res1, res1);
	if (res == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    return(NULL);
	}
	cuddRef(res);
	Cudd_RecursiveDeref(manager,res1);
	cuddDeref(res);
	return(res);
    }

    if ((res = cuddCacheLookup2(manager, Cudd_addUnivAbstract, f, cube)) != NULL) {
	return(res);
    }

    T = cuddT(f);
    E = cuddE(f);

    /* If the two indices are the same, so are their levels. */
    if (f->index == cube->index) {
	res1 = cuddAddUnivAbstractRecur(manager, T, cuddT(cube));
	if (res1 == NULL) return(NULL);
        cuddRef(res1);
	res2 = cuddAddUnivAbstractRecur(manager, E, cuddT(cube));
	if (res2 == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    return(NULL);
	}
        cuddRef(res2);
	res = cuddAddApplyRecur(manager, Cudd_addTimes, res1, res2);
	if (res == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    Cudd_RecursiveDeref(manager,res2);
	    return(NULL);
	}
	cuddRef(res);
	Cudd_RecursiveDeref(manager,res1);
	Cudd_RecursiveDeref(manager,res2);
	cuddCacheInsert2(manager, Cudd_addUnivAbstract, f, cube, res);
	cuddDeref(res);
        return(res);
    } else { /* if (cuddI(manager,f->index) < cuddI(manager,cube->index)) */
	res1 = cuddAddUnivAbstractRecur(manager, T, cube);
	if (res1 == NULL) return(NULL);
        cuddRef(res1);
	res2 = cuddAddUnivAbstractRecur(manager, E, cube);
	if (res2 == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    return(NULL);
	}
        cuddRef(res2);
	res = (res1 == res2) ? res1 :
	    cuddUniqueInter(manager, (int) f->index, res1, res2);
	if (res == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    Cudd_RecursiveDeref(manager,res2);
	    return(NULL);
	}
	cuddDeref(res1);
	cuddDeref(res2);
	cuddCacheInsert2(manager, Cudd_addUnivAbstract, f, cube, res);
        return(res);
    }

} /* end of cuddAddUnivAbstractRecur */


/**Function********************************************************************

  Synopsis    [Performs the recursive step of Cudd_addOrAbstract.]

  Description [Performs the recursive step of Cudd_addOrAbstract.
  Returns the ADD obtained by abstracting the variables of cube from f,
  if successful; NULL otherwise.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
DdNode *
cuddAddOrAbstractRecur(
  DdManager * manager,
  DdNode * f,
  DdNode * cube)
{
    DdNode	*T, *E, *res, *res1, *res2, *one;

    statLine(manager);
    one = DD_ONE(manager);

    /* Cube is guaranteed to be a cube at this point. */
    if (cuddIsConstant(f) || cube == one) {  
	return(f);
    }

    /* Abstract a variable that does not appear in f. */
    if (cuddI(manager,f->index) > cuddI(manager,cube->index)) {
	res = cuddAddOrAbstractRecur(manager, f, cuddT(cube));
	return(res);
    }

    if ((res = cuddCacheLookup2(manager, Cudd_addOrAbstract, f, cube)) != NULL) {
	return(res);
    }

    T = cuddT(f);
    E = cuddE(f);

    /* If the two indices are the same, so are their levels. */
    if (f->index == cube->index) {
	res1 = cuddAddOrAbstractRecur(manager, T, cuddT(cube));
	if (res1 == NULL) return(NULL);
        cuddRef(res1);
	if (res1 != one) {
	    res2 = cuddAddOrAbstractRecur(manager, E, cuddT(cube));
	    if (res2 == NULL) {
		Cudd_RecursiveDeref(manager,res1);
		return(NULL);
	    }
	    cuddRef(res2);
	    res = cuddAddApplyRecur(manager, Cudd_addOr, res1, res2);
	    if (res == NULL) {
		Cudd_RecursiveDeref(manager,res1);
		Cudd_RecursiveDeref(manager,res2);
		return(NULL);
	    }
	    cuddRef(res);
	    Cudd_RecursiveDeref(manager,res1);
	    Cudd_RecursiveDeref(manager,res2);
	} else {
	    res = res1;
	}
	cuddCacheInsert2(manager, Cudd_addOrAbstract, f, cube, res);
	cuddDeref(res);
        return(res);
    } else { /* if (cuddI(manager,f->index) < cuddI(manager,cube->index)) */
	res1 = cuddAddOrAbstractRecur(manager, T, cube);
	if (res1 == NULL) return(NULL);
        cuddRef(res1);
	res2 = cuddAddOrAbstractRecur(manager, E, cube);
	if (res2 == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    return(NULL);
	}
        cuddRef(res2);
	res = (res1 == res2) ? res1 :
	    cuddUniqueInter(manager, (int) f->index, res1, res2);
	if (res == NULL) {
	    Cudd_RecursiveDeref(manager,res1);
	    Cudd_RecursiveDeref(manager,res2);
	    return(NULL);
	}
	cuddDeref(res1);
	cuddDeref(res2);
	cuddCacheInsert2(manager, Cudd_addOrAbstract, f, cube, res);
        return(res);
    }

} /* end of cuddAddOrAbstractRecur */

/**Function********************************************************************
 
 Synopsis    [Performs the recursive step of Cudd_addMinAbstract.]
 
 Description [Performs the recursive step of Cudd_addMinAbstract.
 Returns the ADD obtained by abstracting the variables of cube from f,
 if successful; NULL otherwise.]
 
 SideEffects [None]
 
 SeeAlso     []
 
 ******************************************************************************/
DdNode *
cuddAddMinAbstractRecur(
  DdManager * manager,
  DdNode * f,
  DdNode * cube)
{
	DdNode	*T, *E, *res, *res1, *res2, *zero;
    
	zero = DD_ZERO(manager);
    
	/* Cube is guaranteed to be a cube at this point. */
	if (f == zero || cuddIsConstant(cube)) {
		return(f);
	}
    
    /* Abstract a variable that does not appear in f. */
	if (cuddI(manager,f->index) > cuddI(manager,cube->index)) {
		res = cuddAddMinAbstractRecur(manager, f, cuddT(cube));
       	return(res);
	}
    
	if ((res = cuddCacheLookup2(manager, Cudd_addMinAbstract, f, cube)) != NULL) {
		return(res);
	}
    
    
	T = cuddT(f);
	E = cuddE(f);
    
	/* If the two indices are the same, so are their levels. */
	if (f->index == cube->index) {
		res1 = cuddAddMinAbstractRecur(manager, T, cuddT(cube));
		if (res1 == NULL) return(NULL);
        cuddRef(res1);
		res2 = cuddAddMinAbstractRecur(manager, E, cuddT(cube));
		if (res2 == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            return(NULL);
		}
		cuddRef(res2);
		res = cuddAddApplyRecur(manager, Cudd_addMinimum, res1, res2);
		if (res == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            Cudd_RecursiveDeref(manager,res2);
            return(NULL);
		}
		cuddRef(res);
		Cudd_RecursiveDeref(manager,res1);
		Cudd_RecursiveDeref(manager,res2);
		cuddCacheInsert2(manager, Cudd_addMinAbstract, f, cube, res);
		cuddDeref(res);
        return(res);
    }
	else { /* if (cuddI(manager,f->index) < cuddI(manager,cube->index)) */
		res1 = cuddAddMinAbstractRecur(manager, T, cube);
		if (res1 == NULL) return(NULL);
        cuddRef(res1);
		res2 = cuddAddMinAbstractRecur(manager, E, cube);
		if (res2 == NULL) {
			Cudd_RecursiveDeref(manager,res1);
			return(NULL);
		}
        cuddRef(res2);
		res = (res1 == res2) ? res1 :
        cuddUniqueInter(manager, (int) f->index, res1, res2);
		if (res == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            Cudd_RecursiveDeref(manager,res2);
            return(NULL);
		}
		cuddDeref(res1);
		cuddDeref(res2);
		cuddCacheInsert2(manager, Cudd_addMinAbstract, f, cube, res);
        return(res);
	}
    
} /* end of cuddAddMinAbstractRecur */


/**Function********************************************************************
 
 Synopsis    [Performs the recursive step of Cudd_addMaxAbstract.]
 
 Description [Performs the recursive step of Cudd_addMaxAbstract.
 Returns the ADD obtained by abstracting the variables of cube from f,
 if successful; NULL otherwise.]
 
 SideEffects [None]
 
 SeeAlso     []
 
 ******************************************************************************/
DdNode *
cuddAddMaxAbstractRecur(
                        DdManager * manager,
                        DdNode * f,
                        DdNode * cube)
{
	DdNode	*T, *E, *res, *res1, *res2, *zero;
    
	zero = DD_ZERO(manager);
    
	/* Cube is guaranteed to be a cube at this point. */
	if (f == zero || cuddIsConstant(cube)) {
		return(f);
	}
    
    /* Abstract a variable that does not appear in f. */
	if (cuddI(manager,f->index) > cuddI(manager,cube->index)) {
		res = cuddAddMaxAbstractRecur(manager, f, cuddT(cube));
       	return(res);
	}
    
	if ((res = cuddCacheLookup2(manager, Cudd_addMaxAbstract, f, cube)) != NULL) {
		return(res);
	}
    
    
	T = cuddT(f);
	E = cuddE(f);
    
	/* If the two indices are the same, so are their levels. */
	if (f->index == cube->index) {
		res1 = cuddAddMaxAbstractRecur(manager, T, cuddT(cube));
		if (res1 == NULL) return(NULL);
        cuddRef(res1);
		res2 = cuddAddMaxAbstractRecur(manager, E, cuddT(cube));
		if (res2 == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            return(NULL);
		}
		cuddRef(res2);
		res = cuddAddApplyRecur(manager, Cudd_addMaximum, res1, res2);
		if (res == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            Cudd_RecursiveDeref(manager,res2);
            return(NULL);
		}
		cuddRef(res);
		Cudd_RecursiveDeref(manager,res1);
		Cudd_RecursiveDeref(manager,res2);
		cuddCacheInsert2(manager, Cudd_addMaxAbstract, f, cube, res);
		cuddDeref(res);
        return(res);
    }
	else { /* if (cuddI(manager,f->index) < cuddI(manager,cube->index)) */
		res1 = cuddAddMaxAbstractRecur(manager, T, cube);
		if (res1 == NULL) return(NULL);
        cuddRef(res1);
		res2 = cuddAddMaxAbstractRecur(manager, E, cube);
		if (res2 == NULL) {
			Cudd_RecursiveDeref(manager,res1);
			return(NULL);
		}
        cuddRef(res2);
		res = (res1 == res2) ? res1 :
        cuddUniqueInter(manager, (int) f->index, res1, res2);
		if (res == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            Cudd_RecursiveDeref(manager,res2);
            return(NULL);
		}
		cuddDeref(res1);
		cuddDeref(res2);
		cuddCacheInsert2(manager, Cudd_addMaxAbstract, f, cube, res);
        return(res);
	}	    
    
} /* end of cuddAddMaxAbstractRecur */

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************
 
 Synopsis    [Performs the recursive step of Cudd_addMinAbstractRepresentative.]
 
 Description [Performs the recursive step of Cudd_addMinAbstractRepresentative.
 Returns the ADD obtained by picking a representative over the variables in
 the given cube for all other valuations. Returns the resulting ADD if successful;
 NULL otherwise.]
 
 SideEffects [None]
 
 SeeAlso     []
 
 ******************************************************************************/
DdNode *
cuddAddMinAbstractRepresentativeRecur(
                                      DdManager * manager,
                                      DdNode * f,
                                      DdNode * cube)
{
    DdNode	*T, *E, *res, *res1, *res2, *zero, *one, *res1Inf, *res2Inf, *left, *right, *tmp, *tmp2;
    
    zero = DD_ZERO(manager);
    one = DD_ONE(manager);
    
    /* Cube is guaranteed to be a cube at this point. */
    if (cuddIsConstant(f)) {
        if (cuddIsConstant(cube)) {
            return(one);
        } else {
            return(cube);
        }
    }
    
    /* Abstract a variable that does not appear in f. */
    if (cuddI(manager,f->index) > cuddI(manager,cube->index)) {
        res = cuddAddMinAbstractRepresentativeRecur(manager, f, cuddT(cube));
        if (res == NULL) {
            return(NULL);
        }
        
        // Fill in the missing variables to make representative unique. Set the "non-used-branch" to infinity, otherwise
        // this may interfere with the minimum calculation.
        cuddRef(res);
        cuddRef(zero);
        res1 = cuddUniqueInter(manager, (int) cube->index, zero, res);
        if (res1 == NULL) {
            Cudd_RecursiveDeref(manager,res);
            Cudd_RecursiveDeref(manager,zero);
            return(NULL);
        }
        Cudd_RecursiveDeref(manager, res);
        cuddDeref(zero);
       	return(res1);
    }
    
    if ((res = cuddCacheLookup2(manager, Cudd_addMinAbstractRepresentative, f, cube)) != NULL) {
        return(res);
    }
    
    
    E = cuddE(f);
    T = cuddT(f);
    
    /* If the two indices are the same, so are their levels. */
    if (f->index == cube->index) {
        res1 = cuddAddMinAbstractRepresentativeRecur(manager, E, cuddT(cube));
        if (res1 == NULL) {
            return(NULL);
        }
        cuddRef(res1);
        
        res2 = cuddAddMinAbstractRepresentativeRecur(manager, T, cuddT(cube));
        if (res2 == NULL) {
            Cudd_RecursiveDeref(manager, res1);
            return(NULL);
        }
        cuddRef(res2);
        
        left = cuddAddMinAbstractRecur(manager, E, cuddT(cube));
        if (left == NULL) {
            Cudd_RecursiveDeref(manager, res1);
            Cudd_RecursiveDeref(manager, res2);
            return(NULL);
        }
        cuddRef(left);
        right = cuddAddMinAbstractRecur(manager, T, cuddT(cube));
        if (right == NULL) {
            Cudd_RecursiveDeref(manager, res1);
            Cudd_RecursiveDeref(manager, res2);
            Cudd_RecursiveDeref(manager, left);
            return(NULL);
        }
        cuddRef(right);
        
        tmp = cuddAddApplyRecur(manager, Cudd_addLessThanEquals, left, right);
        if (tmp == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            Cudd_RecursiveDeref(manager,res2);
            Cudd_RecursiveDeref(manager,left);
            Cudd_RecursiveDeref(manager,right);
            return(NULL);
        }
        cuddRef(tmp);
        
        Cudd_RecursiveDeref(manager, left);
        Cudd_RecursiveDeref(manager, right);
        
        cuddRef(zero);
        res1Inf = cuddAddIteRecur(manager, tmp, res1, zero);
        if (res1Inf == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            Cudd_RecursiveDeref(manager,res2);
            Cudd_RecursiveDeref(manager,tmp);
            cuddDeref(zero);
            return(NULL);
        }
        cuddRef(res1Inf);
        Cudd_RecursiveDeref(manager,res1);
        cuddDeref(zero);
        
        tmp2 = cuddAddCmplRecur(manager,tmp);
        if (tmp2 == NULL) {
            Cudd_RecursiveDeref(manager,res2);
            Cudd_RecursiveDeref(manager,left);
            Cudd_RecursiveDeref(manager,right);
            Cudd_RecursiveDeref(manager,tmp);
            return(NULL);
        }
        cuddRef(tmp2);
        Cudd_RecursiveDeref(manager,tmp);
        
        cuddRef(zero);
        res2Inf = cuddAddIteRecur(manager, tmp2, res2, zero);
        if (res2Inf == NULL) {
            Cudd_RecursiveDeref(manager,res2);
            Cudd_RecursiveDeref(manager,res1Inf);
            Cudd_RecursiveDeref(manager,tmp2);
            cuddDeref(zero);
            return(NULL);
        }
        cuddRef(res2Inf);
        Cudd_RecursiveDeref(manager,res2);
        Cudd_RecursiveDeref(manager,tmp2);
        cuddDeref(zero);
        
        res = (res1Inf == res2Inf) ? res1Inf : cuddUniqueInter(manager, (int) f->index, res2Inf, res1Inf);
        if (res == NULL) {
            Cudd_RecursiveDeref(manager,res1Inf);
            Cudd_RecursiveDeref(manager,res2Inf);
            return(NULL);
        }
        cuddRef(res);
        Cudd_RecursiveDeref(manager,res1Inf);
        Cudd_RecursiveDeref(manager,res2Inf);
        cuddCacheInsert2(manager, Cudd_addMinAbstractRepresentative, f, cube, res);
        cuddDeref(res);
        return(res);
    }
    else { /* if (cuddI(manager,f->index) < cuddI(manager,cube->index)) */
        res1 = cuddAddMinAbstractRepresentativeRecur(manager, E, cube);
        if (res1 == NULL) return(NULL);
        cuddRef(res1);
        res2 = cuddAddMinAbstractRepresentativeRecur(manager, T, cube);
        if (res2 == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            return(NULL);
        }
        cuddRef(res2);
        res = (res1 == res2) ? res1 : cuddUniqueInter(manager, (int) f->index, res2, res1);
        if (res == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            Cudd_RecursiveDeref(manager,res2);
            return(NULL);
        }
        cuddDeref(res1);
        cuddDeref(res2);
        cuddCacheInsert2(manager, Cudd_addMinAbstractRepresentative, f, cube, res);
        return(res);
    }
    
} /* end of cuddAddMinAbstractRepresentativeRecur */

/**Function********************************************************************
 
 Synopsis    [Performs the recursive step of Cudd_addMaxAbstractRepresentative.]
 
 Description [Performs the recursive step of Cudd_addMaxAbstractRepresentative.
 Returns the ADD obtained by picking a representative over the variables in
 the given cube for all other valuations. Returns the resulting ADD if successful;
 NULL otherwise.]
 
 SideEffects [None]
 
 SeeAlso     []
 
 ******************************************************************************/
DdNode *
cuddAddMaxAbstractRepresentativeRecur(
                                      DdManager * manager,
                                      DdNode * f,
                                      DdNode * cube)
{
    DdNode	*T, *E, *res, *res1, *res2, *zero, *one, *res1Inf, *res2Inf, *left, *right, *tmp, *tmp2;
    
    zero = DD_ZERO(manager);
    one = DD_ONE(manager);
    
    /* Cube is guaranteed to be a cube at this point. */
    if (cuddIsConstant(f)) {
        if (cuddIsConstant(cube)) {
            return(one);
        } else {
            return(cube);
        }
    }
    
    /* Abstract a variable that does not appear in f. */
    if (cuddI(manager,f->index) > cuddI(manager,cube->index)) {
        res = cuddAddMaxAbstractRepresentativeRecur(manager, f, cuddT(cube));
        
        if (res == NULL) {
            return(NULL);
        }
        
        // Fill in the missing variables to make representative unique.
        cuddRef(res);
        cuddRef(zero);
        res1 = cuddUniqueInter(manager, (int) cube->index, zero, res);
        if (res1 == NULL) {
            Cudd_RecursiveDeref(manager, res);
            Cudd_RecursiveDeref(manager,zero);
            return(NULL);
        }
        Cudd_RecursiveDeref(manager,res);
        Cudd_RecursiveDeref(manager,zero);
       	return(res1);
    }
    
    if ((res = cuddCacheLookup2(manager, Cudd_addMaxAbstractRepresentative, f, cube)) != NULL) {
        return(res);
    }
    
    
    E = cuddE(f);
    T = cuddT(f);
    
    /* If the two indices are the same, so are their levels. */
    if (f->index == cube->index) {
        res1 = cuddAddMaxAbstractRepresentativeRecur(manager, E, cuddT(cube));
        if (res1 == NULL) {
            return(NULL);
        }
        cuddRef(res1);
        
        res2 = cuddAddMaxAbstractRepresentativeRecur(manager, T, cuddT(cube));
        if (res2 == NULL) {
            Cudd_RecursiveDeref(manager, res1);
            return(NULL);
        }
        cuddRef(res2);
        
        left = cuddAddMaxAbstractRecur(manager, E, cuddT(cube));
        if (left == NULL) {
            Cudd_RecursiveDeref(manager, res1);
            Cudd_RecursiveDeref(manager, res2);
            return(NULL);
        }
        cuddRef(left);
        right = cuddAddMaxAbstractRecur(manager, T, cuddT(cube));
        if (right == NULL) {
            Cudd_RecursiveDeref(manager, res1);
            Cudd_RecursiveDeref(manager, res2);
            Cudd_RecursiveDeref(manager, left);
            return(NULL);
        }
        cuddRef(right);
        
        tmp = cuddAddApplyRecur(manager, Cudd_addGreaterThanEquals, left, right);
        if (tmp == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            Cudd_RecursiveDeref(manager,res2);
            Cudd_RecursiveDeref(manager,left);
            Cudd_RecursiveDeref(manager,right);
            return(NULL);
        }
        cuddRef(tmp);
        
        Cudd_RecursiveDeref(manager, left);
        Cudd_RecursiveDeref(manager, right);
        
        cuddRef(zero);
        res1Inf = cuddAddIteRecur(manager, tmp, res1, zero);
        if (res1Inf == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            Cudd_RecursiveDeref(manager,res2);
            Cudd_RecursiveDeref(manager,tmp);
            cuddDeref(zero);
            return(NULL);
        }
        cuddRef(res1Inf);
        Cudd_RecursiveDeref(manager,res1);
        cuddDeref(zero);
        
        tmp2 = cuddAddCmplRecur(manager,tmp);
        if (tmp2 == NULL) {
            Cudd_RecursiveDeref(manager,res2);
            Cudd_RecursiveDeref(manager,left);
            Cudd_RecursiveDeref(manager,right);
            Cudd_RecursiveDeref(manager,tmp);
            return(NULL);
        }
        cuddRef(tmp2);
        Cudd_RecursiveDeref(manager,tmp);
        
        cuddRef(zero);
        res2Inf = cuddAddIteRecur(manager, tmp2, res2, zero);
        if (res2Inf == NULL) {
            Cudd_RecursiveDeref(manager,res2);
            Cudd_RecursiveDeref(manager,res1Inf);
            Cudd_RecursiveDeref(manager,tmp2);
            cuddDeref(zero);
            return(NULL);
        }
        cuddRef(res2Inf);
        Cudd_RecursiveDeref(manager,res2);
        Cudd_RecursiveDeref(manager,tmp2);
        cuddDeref(zero);
        
        res = (res1Inf == res2Inf) ? res1Inf : cuddUniqueInter(manager, (int) f->index, res2Inf, res1Inf);
        if (res == NULL) {
            Cudd_RecursiveDeref(manager,res1Inf);
            Cudd_RecursiveDeref(manager,res2Inf);
            return(NULL);
        }
        cuddRef(res);
        Cudd_RecursiveDeref(manager,res1Inf);
        Cudd_RecursiveDeref(manager,res2Inf);
        cuddCacheInsert2(manager, Cudd_addMaxAbstractRepresentative, f, cube, res);
        cuddDeref(res);
        return(res);
    }
    else { /* if (cuddI(manager,f->index) < cuddI(manager,cube->index)) */
        res1 = cuddAddMaxAbstractRepresentativeRecur(manager, E, cube);
        if (res1 == NULL) return(NULL);
        cuddRef(res1);
        res2 = cuddAddMaxAbstractRepresentativeRecur(manager, T, cube);
        if (res2 == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            return(NULL);
        }
        cuddRef(res2);
        res = (res1 == res2) ? res1 : cuddUniqueInter(manager, (int) f->index, res2, res1);
        if (res == NULL) {
            Cudd_RecursiveDeref(manager,res1);
            Cudd_RecursiveDeref(manager,res2);
            return(NULL);
        }
        cuddDeref(res1);
        cuddDeref(res2);
        cuddCacheInsert2(manager, Cudd_addMaxAbstractRepresentative, f, cube, res);
        return(res);
    }
} /* end of cuddAddMaxAbstractRepresentativeRecur */

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis    [Checks whether cube is an ADD representing the product
  of positive literals.]

  Description [Checks whether cube is an ADD representing the product of
  positive literals. Returns 1 in case of success; 0 otherwise.]

  SideEffects [None]

  SeeAlso     []

******************************************************************************/
static int
addCheckPositiveCube(
  DdManager * manager,
  DdNode * cube)
{
    if (Cudd_IsComplement(cube)) return(0);
    if (cube == DD_ONE(manager)) return(1);
    if (cuddIsConstant(cube)) return(0);
    if (cuddE(cube) == DD_ZERO(manager)) {
        return(addCheckPositiveCube(manager, cuddT(cube)));
    }
    return(0);

} /* end of addCheckPositiveCube */

