unset surface
set term wxt enhanced
set pm3d at s
set palette
set key off
set view 50,50
set xlabel "time (ms)"
set ylabel "freq (MHz)"
set zlabel "PSD (dBW/Hz)" offset 15,0,0
splot "./SUs/spectrum-analyzer-output-5-0.tr" using ($1*1000.0):($2/1e6):(10*log10($3))
pause -1 "Hit any key to continue"
