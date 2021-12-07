# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2012, 2014, 2015, 2016 Laboratoire de Recherche et
# Développement de l'Epita
#
# This file is part of Spot, a model checking library.
#
# Spot is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# Spot is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
# License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import spot
import time
import signal
import sys
import os


def alarm_handler(signum, frame):
    sys.stdout.write("signaled\n")
    os.kill(child, signal.SIGTERM)
    exit(0)


f = """!(G(F(P_Rbt2.observe)&& F(P_Rbt3.observe) &&
F(P_rbt1.observe)&& F(P_Rbt1.plus || P_Rbt1.moins || P_Rbt1.stop)&&
F(P_Rbt3.plus || P_Rbt3.moins || P_Rbt3.stop) && F(P_Rbt2.plus ||
P_Rbt2.moins || P_Rbt2.stop))-> G((F "map[0]==1") && (F "map[1]==1")
&& (F "map[2]==1") && (F "map[3]==1") && (F "map[4]==1") && (F
"map[5]==1") && (F "map[6]==1") && (F "map[7]==1") && (F "map[8]==1")
&& (F "map[9]==1") && (F "map[0]==2") && (F "map[1]==2") && (F
"map[2]==2") && (F "map[3]==2") && (F "map[4]==2") && (F "map[5]==2")
&& (F "map[6]==2") && (F "map[7]==2") && (F "map[8]==2") && (F
"map[9]==2") && (F "map[0]==3") && (F "map[1]==3") && (F "map[2]==3")
&& (F "map[3]==3") && (F "map[4]==3") && (F "map[5]==3") && (F
"map[6]==3") && (F "map[7]==3") && (F "map[8]==3") && (F
"map[9]==3")))"""

e = spot.default_environment.instance()
pf = spot.parse_infix_psl(f, e)
d = spot.make_bdd_dict()

spot.unblock_signal(signal.SIGALRM)
spot.unblock_signal(signal.SIGTERM)
os.setpgrp()
child = os.fork()
if child != 0:
    signal.signal(signal.SIGALRM, alarm_handler)
    signal.alarm(2)
    os.waitpid(child, 0)
    # If the child returns, before we get the alarm it's a bug.
    exit(1)

# This is expected to take WAY more than 2s.
print("Before")
spot.ltl_to_tgba_fm(pf.f, d, True)
print("After")

exit(1)
