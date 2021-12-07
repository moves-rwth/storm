/* Echo Election Algorithm with Extinction in an Arbitrary Network. */
/* Variation 1: Node 0 wins every time.                             */

#define L	10	/* size of buffer */
#define udef    3

#define noLeader        (nr_leaders == 0)
#define zeroLeads	(nr_leaders == 1 && leader == 0)
#define oneLeads	(nr_leaders == 1 && leader == 1)
#define twoLeads	(nr_leaders == 1 && leader == 2)
#define threeLeads	(nr_leaders == 1 && leader == 3)

mtype = { tok, ldr };
chan zero_one = [L] of { mtype, byte};
chan zero_two = [L] of { mtype, byte};
chan one_zero = [L] of { mtype, byte};
chan one_two  = [L] of { mtype, byte};
chan two_zero = [L] of { mtype, byte};
chan two_one  = [L] of { mtype, byte};

chan nr0 = [0] of {mtype, byte};
chan nr1 = [0] of {mtype, byte};
chan nr2 = [0] of {mtype, byte};

byte nr_leaders, done, leader;

inline recvldr ()
{
	if
	:: lrec == 0 && r != myid ->
		out1!ldr(r);
		out2!ldr(r);
	:: else -> skip;
	fi;
	lrec++;
	win = r;
}

inline recvtok (q,c)
{
	if
	:: (r+turn)%3 < (caw+turn)%3 ->
		caw = r;
		rec = 0;
		father = q;
		c!tok(r);
	:: else -> skip;
	fi;

	if
	:: r == caw ->
		rec++;
		if
		:: rec == 2 && caw == myid
			-> out1!ldr(myid); out2!ldr(myid);
		:: rec == 2 && caw != myid && father == neigh1
			-> out1!tok(caw)
		:: rec == 2 && caw != myid && father == neigh2
			-> out2!tok(caw)
		:: else -> skip;
		fi;
	:: else -> skip;
	fi;
}

proctype node (chan nr; byte neigh1; chan out1, in1;
			byte neigh2; chan out2, in2)
{	byte myid = 3 - neigh1 - neigh2;
	byte caw, rec, father, lrec, win, r, turn;

	xr in1; xr in2;
	xs out1; xs out2;

restart:
	nr?tok(turn);
	caw = myid; rec = 0; lrec = 0;
	father = udef; win = udef; r = udef;

	out1!tok(myid);
	out2!tok(myid);
	do
	:: lrec == 2 -> break;
	:: in1?ldr(r) -> recvldr();
	:: in2?ldr(r) -> recvldr();
	:: in1?tok(r) -> recvtok(neigh1,out2);
	:: in2?tok(r) -> recvtok(neigh2,out1);
	od;

	if
	:: win == myid -> 
		leader = myid;
		nr_leaders++;
		assert(nr_leaders == 1);
	:: else ->
		skip;
	fi;

	done++;
	goto restart;
}

init {
	byte turn = 0;
	atomic {
		run node (nr0,1,zero_one,one_zero,2,zero_two,two_zero);
		run node (nr1,0,one_zero,zero_one,2,one_two,two_one);
		run node (nr2,0,two_zero,zero_two,1,two_one,one_two);
	}
	do
	:: true ->
		done = 0;
		nr_leaders = 0;
		leader = udef;
		nr0!tok(turn); nr1!tok(turn); nr2!tok(turn);
		done == 3;
		turn = (turn+1)%3;
	od;
}
