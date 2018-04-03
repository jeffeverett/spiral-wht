#!/bin/bash
#
# Copyright (c) 2000 Carnegie Mellon University
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


FILE=$1
DAT_DIR=../../runs/ga

AVE_TMPFILE=/tmp/ga_ave_tmpfile.$$
BEST_TMPFILE=/tmp/ga_best_tmpfile.$$
TMPFILE=/tmp/ga_plotting_tmpfile.$$

grep  Ave ${DAT_DIR}/${FILE} | cut -d ':' -f 2 >  $AVE_TMPFILE
grep Best ${DAT_DIR}/${FILE} | cut -d '!' -f 2 > $BEST_TMPFILE

epochs=`wc -l < $AVE_TMPFILE`

(
   echo 'set terminal postscript monochrome'
   echo 'set output "/dev/null"'
   echo 'plot [0:'${epochs}'] "'${AVE_TMPFILE}'" title "average" with lines'
   echo 'replot "'${BEST_TMPFILE}'" title "best" with lines'
   echo 'set title "'${FILE%.ga}'"'
   echo 'set xlabel "generations"'
   echo 'set ylabel "time"'
   echo 'set output "'${DAT_DIR}/timesonly/${FILE%.ga}.best_ave.ps'"'
   echo 'replot'
) > $TMPFILE

gnuplot $TMPFILE

rm $AVE_TMPFILE $BEST_TMPFILE $TMPFILE
