set terminal postscript eps enhanced color
set ytics nomirror
set y2tics auto
set ylabel "states"
set y2label "seconds"
set key left top

set output 'resultsalpha.fm.eps'

plot 'resultsalpha.fm' using 1:4 '%lf,%lf,%lf,%lf' \
        with filledcurve x1 title "Translation Time" axes x1y2, \
     'resultsalpha.fm' using 1:2 '%lf,%lf,%lf,%lf' \
        with lines title "States"

set output 'resultsbeta.fm.eps'

plot 'resultsbeta.fm' using 1:4 '%lf,%lf,%lf,%lf' \
        with filledcurve x1 title "Translation Time" axes x1y2, \
     'resultsbeta.fm' using 1:2 '%lf,%lf,%lf,%lf' \
        with lines title "States"

set output 'resultsbeta-prime.fm.eps'

plot 'resultsbeta-prime.fm' using 1:4 '%lf,%lf,%lf,%lf' \
        with filledcurve x1 title "Translation Time" axes x1y2, \
     'resultsbeta-prime.fm' using 1:2 '%lf,%lf,%lf,%lf' \
        with lines title "States"

set output 'resultsphi.fm.eps'

plot 'resultsphi.fm' using 1:4 '%lf,%lf,%lf,%lf' \
        with filledcurve x1 title "Translation Time" axes x1y2, \
     'resultsphi.fm' using 1:2 '%lf,%lf,%lf,%lf' \
        with lines title "States"

set output 'resultsxi.fm.eps'

plot 'resultsxi.fm' using 1:4 '%lf,%lf,%lf,%lf' \
        with filledcurve x1 title "Translation Time" axes x1y2, \
     'resultsxi.fm' using 1:2 '%lf,%lf,%lf,%lf' \
        with lines title "States"
