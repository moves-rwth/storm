/* @(#)qsort.h	4.2 (Berkeley) 3/9/83 */

/*
 * qsort.h:
 * Our own version of the system qsort routine which is faster by an average
 * of 25%, with lows and highs of 10% and 50%.
 * The THRESHold below is the insertion sort threshold, and has been adjusted
 * for records of size 48 bytes.
 * The MTHREShold is where we stop finding a better median.
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef  int (*QSFP)(const void *, const void *);
void cudd__qsort (void* base, int n, int size, QSFP compar);

#define		THRESH		4		/* threshold for insertion */
#define		MTHRESH		6		/* threshold for median */

static  QSFP		qcmp;			/* the comparison routine */
static  int		qsz;			/* size of each record */
static  int		thresh;			/* THRESHold in chars */
static  int		mthresh;		/* MTHRESHold in chars */

static	void		qst (void *base, char *max);

#ifdef __cplusplus
}
#endif