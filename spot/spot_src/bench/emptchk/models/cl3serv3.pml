#define w1 client[0]@wait
#define s1 client[0]@served

#define C 3
#define S 3

chan clserv = [C] of { int };
chan servcl = [S] of { int };

active [C] proctype client() {
  /* the _pid's are: 0 .. C-1 */

served:
    if
    :: (1) -> goto request;
    fi;
request:
    if
    :: (1) -> clserv!_pid; goto wait;
    fi;
wait:
    if
    :: servcl?eval(_pid); goto served;
    fi;
}

active [S] proctype server() {
  /* the _pid's are: 0 .. S-1 */
  byte id;

wait:
    if
    :: clserv?id -> goto work;
    fi;
work:
    if
    :: (1) -> goto reply;
    fi;
reply:
    if
    :: (1) -> servcl!id; goto wait;
    fi;
}
