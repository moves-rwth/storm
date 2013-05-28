/***** ltl2ba : alternating.c *****/

/* Written by Denis Oddoux, LIAFA, France                                 */
/* Copyright (c) 2001  Denis Oddoux                                       */
/* Modified by Paul Gastin, LSV, France                                   */
/* Copyright (c) 2007  Paul Gastin                                        */
/*                                                                        */
/* This program is free software; you can redistribute it and/or modify   */
/* it under the terms of the GNU General Public License as published by   */
/* the Free Software Foundation; either version 2 of the License, or      */
/* (at your option) any later version.                                    */
/*                                                                        */
/* This program is distributed in the hope that it will be useful,        */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of         */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          */
/* GNU General Public License for more details.                           */
/*                                                                        */
/* You should have received a copy of the GNU General Public License      */
/* along with this program; if not, write to the Free Software            */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA*/
/*                                                                        */
/* Based on the translation algorithm by Gastin and Oddoux,               */
/* presented at the 13th International Conference on Computer Aided       */
/* Verification, CAV 2001, Paris, France.                                 */
/* Proceedings - LNCS 2102, pp. 53-65                                     */
/*                                                                        */
/* Send bug-reports and/or questions to Paul Gastin                       */
/* http://www.lsv.ens-cachan.fr/~gastin                                   */

#include "ltl2ba.h"

/********************************************************************\
|*              Structures and shared variables                     *|
\********************************************************************/

extern FILE *tl_out;
extern int tl_verbose, tl_stats, tl_simp_diff;

Node **label;
char **sym_table;
ATrans **transition;
struct rusage tr_debut, tr_fin;
struct timeval t_diff;
int *final_set, node_id = 1, sym_id = 0, node_size, sym_size;
int astate_count = 0, atrans_count = 0;

ATrans *build_alternating(Node *p);

/********************************************************************\
|*              Generation of the alternating automaton             *|
\********************************************************************/

int calculate_node_size(Node *p) /* returns the number of temporal nodes */
{
  switch(p->ntyp) {
  case AND:
  case OR:
  case U_OPER:
  case V_OPER:
    return(calculate_node_size(p->lft) + calculate_node_size(p->rgt) + 1);
#ifdef NXT
  case NEXT:
    return(calculate_node_size(p->lft) + 1);
#endif
  default:
    return 1;
    break;
  }
}

int calculate_sym_size(Node *p) /* returns the number of predicates */
{
  switch(p->ntyp) {
  case AND:
  case OR:
  case U_OPER:
  case V_OPER:
    return(calculate_sym_size(p->lft) + calculate_sym_size(p->rgt));
#ifdef NXT
  case NEXT:
    return(calculate_sym_size(p->lft));
#endif
  case NOT:
  case PREDICATE:
    return 1;
  default:
    return 0;
  }
}

ATrans *dup_trans(ATrans *trans)  /* returns the copy of a transition */
{
  ATrans *result;
  if(!trans) return trans;
  result = emalloc_atrans();
  copy_set(trans->to,  result->to,  0);
  copy_set(trans->pos, result->pos, 1);
  copy_set(trans->neg, result->neg, 1);
  return result;
}

void do_merge_trans(ATrans **result, ATrans *trans1, ATrans *trans2) 
{ /* merges two transitions */
  if(!trans1 || !trans2) {
    free_atrans(*result, 0);
    *result = (ATrans *)0;
    return;
  }
  if(!*result)
    *result = emalloc_atrans();
  do_merge_sets((*result)->to, trans1->to,  trans2->to,  0);
  do_merge_sets((*result)->pos, trans1->pos, trans2->pos, 1);
  do_merge_sets((*result)->neg, trans1->neg, trans2->neg, 1);
  if(!empty_intersect_sets((*result)->pos, (*result)->neg, 1)) {
    free_atrans(*result, 0);
    *result = (ATrans *)0;
  }
}

ATrans *merge_trans(ATrans *trans1, ATrans *trans2) /* merges two transitions */
{
  ATrans *result = emalloc_atrans();
  do_merge_trans(&result, trans1, trans2);
  return result;
}

int already_done(Node *p) /* finds the id of the node, if already explored */
{
  int i;
  for(i = 1; i<node_id; i++) 
    if (isequal(p, label[i])) 
      return i;
  return -1;
}

int get_sym_id(char *s) /* finds the id of a predicate, or attributes one */
{
  int i;
  for(i=0; i<sym_id; i++) 
    if (!strcmp(s, sym_table[i])) 
      return i;
  sym_table[sym_id] = s;
  return sym_id++;
}

ATrans *boolean(Node *p) /* computes the transitions to boolean nodes -> next & init */
{
  ATrans *t1, *t2, *lft, *rgt, *result = (ATrans *)0;
  int id;
  switch(p->ntyp) {
  case TRUE:
    result = emalloc_atrans();
    clear_set(result->to,  0);
    clear_set(result->pos, 1);
    clear_set(result->neg, 1);
  case FALSE:
    break;
  case AND:
    lft = boolean(p->lft);
    rgt = boolean(p->rgt);
    for(t1 = lft; t1; t1 = t1->nxt) {
      for(t2 = rgt; t2; t2 = t2->nxt) {
	ATrans *tmp = merge_trans(t1, t2);
	if(tmp) {
	  tmp->nxt = result;
	  result = tmp;
	}
      }
    }
    free_atrans(lft, 1);
    free_atrans(rgt, 1);
    break;
  case OR:
    lft = boolean(p->lft);
    for(t1 = lft; t1; t1 = t1->nxt) {
      ATrans *tmp = dup_trans(t1);
      tmp->nxt = result;
      result = tmp;
    }
    free_atrans(lft, 1);
    rgt = boolean(p->rgt);
    for(t1 = rgt; t1; t1 = t1->nxt) {
      ATrans *tmp = dup_trans(t1);
      tmp->nxt = result;
      result = tmp;
    }
    free_atrans(rgt, 1);
    break;
  default:
    build_alternating(p);
    result = emalloc_atrans();
    clear_set(result->to,  0);
    clear_set(result->pos, 1);
    clear_set(result->neg, 1);
    add_set(result->to, already_done(p));
  }
  return result;
}

ATrans *build_alternating(Node *p) /* builds an alternating automaton for p */
{
  ATrans *t1, *t2, *t = (ATrans *)0;
  int node = already_done(p);
  if(node >= 0) return transition[node];

  switch (p->ntyp) {

  case TRUE:
    t = emalloc_atrans();
    clear_set(t->to,  0);
    clear_set(t->pos, 1);
    clear_set(t->neg, 1);
  case FALSE:
    break;

  case PREDICATE:
    t = emalloc_atrans();
    clear_set(t->to,  0);
    clear_set(t->pos, 1);
    clear_set(t->neg, 1);
    add_set(t->pos, get_sym_id(p->sym->name));
    break;

  case NOT:
    t = emalloc_atrans();
    clear_set(t->to,  0);
    clear_set(t->pos, 1);
    clear_set(t->neg, 1);
    add_set(t->neg, get_sym_id(p->lft->sym->name));
    break;

#ifdef NXT
  case NEXT:                                            
    t = boolean(p->lft);
    break;
#endif

  case U_OPER:    /* p U q <-> q || (p && X (p U q)) */
    for(t2 = build_alternating(p->rgt); t2; t2 = t2->nxt) {
      ATrans *tmp = dup_trans(t2);  /* q */
      tmp->nxt = t;
      t = tmp;
    }
    for(t1 = build_alternating(p->lft); t1; t1 = t1->nxt) {
      ATrans *tmp = dup_trans(t1);  /* p */
      add_set(tmp->to, node_id);  /* X (p U q) */
      tmp->nxt = t;
      t = tmp;
    }
    add_set(final_set, node_id);
    break;

  case V_OPER:    /* p V q <-> (p && q) || (p && X (p V q)) */
    for(t1 = build_alternating(p->rgt); t1; t1 = t1->nxt) {
      ATrans *tmp;

      for(t2 = build_alternating(p->lft); t2; t2 = t2->nxt) {
	tmp = merge_trans(t1, t2);  /* p && q */
	if(tmp) {
	  tmp->nxt = t;
	  t = tmp;
	}
      }

      tmp = dup_trans(t1);  /* p */
      add_set(tmp->to, node_id);  /* X (p V q) */
      tmp->nxt = t;
      t = tmp;
    }
    break;

  case AND:
    t = (ATrans *)0;
    for(t1 = build_alternating(p->lft); t1; t1 = t1->nxt) {
      for(t2 = build_alternating(p->rgt); t2; t2 = t2->nxt) {
	ATrans *tmp = merge_trans(t1, t2);
	if(tmp) {
	  tmp->nxt = t;
	  t = tmp;
	}
      }
    }
    break;

  case OR:
    t = (ATrans *)0;
    for(t1 = build_alternating(p->lft); t1; t1 = t1->nxt) {
      ATrans *tmp = dup_trans(t1);
      tmp->nxt = t;
      t = tmp;
    }
    for(t1 = build_alternating(p->rgt); t1; t1 = t1->nxt) {
      ATrans *tmp = dup_trans(t1);
      tmp->nxt = t;
      t = tmp;
    }
    break;

  default:
    break;
  }

  transition[node_id] = t;
  label[node_id++] = p;
  return(t);
}

/********************************************************************\
|*        Simplification of the alternating automaton               *|
\********************************************************************/

void simplify_atrans(ATrans **trans) /* simplifies the transitions */
{
  ATrans *t, *father = (ATrans *)0;
  for(t = *trans; t;) {
    ATrans *t1;
    for(t1 = *trans; t1; t1 = t1->nxt) {
      if((t1 != t) && 
	 included_set(t1->to,  t->to,  0) &&
	 included_set(t1->pos, t->pos, 1) &&
	 included_set(t1->neg, t->neg, 1))
	break;
    }
    if(t1) {
      if (father)
	father->nxt = t->nxt;
      else
	*trans = t->nxt;
      free_atrans(t, 0);
      if (father)
	t = father->nxt;
      else
	t = *trans;
      continue;
    }
    atrans_count++;
    father = t;
    t = t->nxt;
  }
}

void simplify_astates() /* simplifies the alternating automaton */
{
  ATrans *t;
  int i, *acc = make_set(-1, 0); /* no state is accessible initially */

  for(t = transition[0]; t; t = t->nxt, i = 0)
    merge_sets(acc, t->to, 0); /* all initial states are accessible */

  for(i = node_id - 1; i > 0; i--) {
    if (!in_set(acc, i)) { /* frees unaccessible states */
      label[i] = ZN;
      free_atrans(transition[i], 1);
      transition[i] = (ATrans *)0;
      continue;
    }
    astate_count++;
    simplify_atrans(&transition[i]);
    for(t = transition[i]; t; t = t->nxt)
      merge_sets(acc, t->to, 0);
  }

  tfree(acc);
}

/********************************************************************\
|*            Display of the alternating automaton                  *|
\********************************************************************/

void print_alternating() /* dumps the alternating automaton */
{
  int i;
  ATrans *t;

  fprintf(tl_out, "init :\n");
  for(t = transition[0]; t; t = t->nxt) {
    print_set(t->to, 0);
    fprintf(tl_out, "\n");
  }
  
  for(i = node_id - 1; i > 0; i--) {
    if(!label[i])
      continue;
    fprintf(tl_out, "state %i : ", i);
    dump(label[i]);
    fprintf(tl_out, "\n");
    for(t = transition[i]; t; t = t->nxt) {
      if (empty_set(t->pos, 1) && empty_set(t->neg, 1))
	fprintf(tl_out, "1");
      print_set(t->pos, 1);
      if (!empty_set(t->pos,1) && !empty_set(t->neg,1)) fprintf(tl_out, " & ");
      print_set(t->neg, 2);
      fprintf(tl_out, " -> ");
      print_set(t->to, 0);
      fprintf(tl_out, "\n");
    }
  }
}

/********************************************************************\
|*                       Main method                                *|
\********************************************************************/

void mk_alternating(Node *p) /* generates an alternating automaton for p */
{
  if(tl_stats) getrusage(RUSAGE_SELF, &tr_debut);

  node_size = calculate_node_size(p) + 1; /* number of states in the automaton */
  label = (Node **) tl_emalloc(node_size * sizeof(Node *));
  transition = (ATrans **) tl_emalloc(node_size * sizeof(ATrans *));
  node_size = node_size / (8 * sizeof(int)) + 1;

  sym_size = calculate_sym_size(p); /* number of predicates */
  if(sym_size) sym_table = (char **) tl_emalloc(sym_size * sizeof(char *));
  sym_size = sym_size / (8 * sizeof(int)) + 1;
  
  final_set = make_set(-1, 0);
  transition[0] = boolean(p); /* generates the alternating automaton */

  if(tl_verbose) {
    fprintf(tl_out, "\nAlternating automaton before simplification\n");
    print_alternating();
  }

  if(tl_simp_diff) {
    simplify_astates(); /* keeps only accessible states */
    if(tl_verbose) {
      fprintf(tl_out, "\nAlternating automaton after simplification\n");
      print_alternating();
    }
  }
  
  if(tl_stats) {
    getrusage(RUSAGE_SELF, &tr_fin);
    timeval_subtract (&t_diff, &tr_fin.ru_utime, &tr_debut.ru_utime);
    fprintf(tl_out, "\nBuilding and simplification of the alternating automaton: %i.%06is",
		t_diff.tv_sec, t_diff.tv_usec);
    fprintf(tl_out, "\n%i states, %i transitions\n", astate_count, atrans_count);
  }

  releasenode(1, p);
  tfree(label);
}
