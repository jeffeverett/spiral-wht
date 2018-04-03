# aclocal.m4 generated automatically by aclocal 1.5

# Copyright 1996, 1997, 1998, 1999, 2000, 2001
# Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.

dnl Copyright (c) 2001 Carnegie Mellon University
dnl 
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl 
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl 
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software
dnl Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

dnl Note: part of this file is taken from FFTW (as indicated below)
dnl www.fftw.org

dnl =====================================================================
dnl Overview:
dnl ACX_GAP_TARGET: sets the gap target used for compiling gap in 
dnl   gap/src/Makefile
dnl ACX_PROG_CC_FLAGS: sets CFLAGS for compiling Spiral
dnl ACX_PROG_F77_MAXOPT: sets SPL_FFLAGS which are use for compiling Fortran
dnl   code produced by Spiral
dnl ACX_PROG_CC_MAXOPT: sets SPL_CFLAGS which are use for compiling C code
dnl   produced by Spiral
dnl ACX_FIND_CC_BUGS: checks for bugs in C compiler
dnl ACX_CHECK_CC_FLAGS: checks whether guessed C flags work
dnl ACX_CHECK_F77_FLAGS: checks whether guessed Fortran flags work (not
dnl   implemented yet...)

dnl ------------------
dnl find compiler bugs
dnl ------------------
AC_DEFUN(ACX_FIND_CC_BUGS,
[
AC_REQUIRE([AC_PROG_CC])
echo 'void f(){}' > conftest.c
dnl catch cc WorkShop compiler warning for -native
if test "$CC" = cc && test -n "`cc -V 2>&1 | grep 'WorkShop 6'`"; then
  FORTE_WORKSHOP_6=1
else
  FORTE_WORKSHOP_6=0
fi
rm -f conftest.c
])

dnl ----------------------------------------------
dnl find Forte WorkShop Version for OpenMP Support
dnl ----------------------------------------------
dnl Sun Workshop Compilers 4.2, 5.0 and later support OpenMP.
AC_DEFUN(ACX_FIND_WORKSHOP_VER,
[
AC_REQUIRE([AC_PROG_CC])
echo 'void f(){}' > conftest.c
dnl catch cc WorkShop compiler warning for -native
WORKSHOP_OpenMP=0
if test "$CC" = cc && test -n "`cc -V 2>&1 | grep 'WorkShop 6'`"; then
  WORKSHOP_OpenMP=1 
fi  
if test "$CC" = cc && test -n "`cc -V 2>&1 | grep 'WorkShop 5'`"; then
  WORKSHOP_OpenMP=1
fi
if test "$CC" = cc && test -n "`cc -V 2>&1 | grep 'WorkShop 4.2'`"; then
  WORKSHOP_OpenMP=1
fi
rm -f conftest.c
])


dnl =====================================================================
dnl below are several functions taken from FFTW, 
dnl in ACX_PROG_CC_MAXOPT suppression of
dnl scheduling has been removed

AC_DEFUN(ACX_CHECK_CC_FLAGS,
[
AC_REQUIRE([AC_PROG_CC])
AC_CACHE_CHECK(whether ${CC-cc} accepts $1, ac_$2,
[echo 'void f(){}' > conftest.c
dnl catch cc WorkShop compiler warning for -native
if test "$CC" = cc && test -n "`cc -V 2>&1 | grep 'WorkShop 6'`"; then
  echo "no check for cc WorkShop 6"
  ac_$2=yes
else
  if test -z "`${CC-cc} $1 -c conftest.c 2>&1`"; then
    ac_$2=yes
  else
    ac_$2=no
  fi
fi
rm -f conftest*
])
if test "$ac_$2" = yes; then
	:
	$3
else
	:
	$4
fi
])

AC_DEFUN(ACX_PROG_GCC_VERSION,
[
AC_REQUIRE([AC_PROG_CC])
AC_CACHE_CHECK(whether we are using gcc $1.$2 or later, ac_cv_prog_gcc_$1_$2,
[
dnl The semicolon after "yes" below is to pacify NeXT's syntax-checking cpp.
cat > conftest.c <<EOF
#ifdef __GNUC__
#  if (__GNUC__ > $1) || (__GNUC__ == $1 && __GNUC_MINOR__ >= $2)
     yes;
#  endif
#endif
EOF
if AC_TRY_COMMAND(${CC-cc} -E conftest.c) | egrep yes >/dev/null 2>&1; then
  ac_cv_prog_gcc_$1_$2=yes
else
  ac_cv_prog_gcc_$1_$2=no
fi
])
if test "$ac_cv_prog_gcc_$1_$2" = yes; then
	:
	$3
else
	:
	$4
fi
])

AC_DEFUN(ACX_PROG_CC_EGCS,
[ACX_PROG_GCC_VERSION(2,90,acx_prog_egcs=yes,acx_prog_egcs=no)])

# Check to see if we are using a version of gcc that aligns the stack
# (true in gcc-2.95+, which have the -mpreferred-stack-boundary flag).
# Also check for stack alignment bug in gcc-2.95.x
# (see http://egcs.cygnus.com/ml/gcc-bugs/1999-11/msg00259.html), and
# whether main() is correctly aligned by the OS/libc/loader.
AC_DEFUN(ACX_GCC_ALIGNS_STACK,
[
AC_REQUIRE([AC_PROG_CC])
acx_gcc_aligns_stack=no
if test "$GCC" = "yes"; then
ACX_CHECK_CC_FLAGS(-mpreferred-stack-boundary=4, m_pref_stack_boundary_4)
if test "$ac_m_pref_stack_boundary_4" = "yes"; then
	AC_MSG_CHECKING([whether the stack is correctly aligned by gcc])
	save_CFLAGS="$CFLAGS"
	CFLAGS="-O -malign-double"
	AC_TRY_RUN([#include <stdlib.h>
#       include <stdio.h>
	struct yuck { int blechh; };
	int one(void) { return 1; }
	struct yuck ick(void) { struct yuck y; y.blechh = 3; return y; }
#       define CHK_ALIGN(x) if ((((long) &(x)) & 0x7)) { fprintf(stderr, "bad alignment of " #x "\n"); exit(1); }
	void blah(int foo) { double foobar; CHK_ALIGN(foobar); }
	int main(void) { double ok1; struct yuck y; double ok2; CHK_ALIGN(ok1);
                         CHK_ALIGN(ok2); y = ick(); blah(one()); return 0; }
	], [acx_gcc_aligns_stack=yes; acx_gcc_stack_align_bug=no], 
	acx_gcc_stack_align_bug=yes, acx_gcc_stack_align_bug=yes)
	CFLAGS="$save_CFLAGS"
	AC_MSG_RESULT($acx_gcc_aligns_stack)
fi
fi
if test "$acx_gcc_aligns_stack" = yes; then
	:
	$1
else
	:
	$2
fi
])

dnl -------------------------------------------------
dnl find best SPL_CFLAGS, which are used for the code
dnl generated by spiral
dnl -------------------------------------------------
AC_DEFUN(ACX_PROG_CC_MAXOPT,
[
AC_REQUIRE([AC_PROG_CC])
AC_REQUIRE([ACX_PROG_CC_EGCS])
AC_REQUIRE([AC_CANONICAL_HOST])
AC_REQUIRE([ACX_FIND_WORKSHOP_VER])

# Try to determine "good" native compiler flags if none specified on command
# line
if test "$ac_test_CFLAGS" != "set"; then
  CFLAGS=""
  PC=""
  PCFLAGS=""
  ARFLAGS=""

  case "${host_cpu}-${host_os}" in

  *linux*) ;;

  # 32-bit Solaris 2.5.1, 2.6, and 64-bit Solaris 7
  # So if it is solaris2.7, turn on the 64-bit with option -xarch=v9 or v8plus.
  sparc-solaris2*)
                 if test "$CC" = cc; then
                    CFLAGS="-fast -xO5 -dalign"
                    case "${host_os}" in 
                    solaris2.7*) 
                       # to see if sysinfo -nw works. -nw turns off the GUI
                       if test -n "`sysinfo -nw 2>&1 | grep 'CPU Type'`"; then
                          SYSINFO="sysinfo -nw"
                       else
                          SYSINFO="sysinfo"
                       fi
 
                       if test -n "`$SYSINFO 2>&1 | grep 'sparcv9'`"; then
                          CFLAGS="$CFLAGS -xarch=v9"
                       fi
                       if test -n "`$SYSINFO 2>&1 | grep 'sparcv8plus'`"; then
                          CFLAGS="$CFLAGS -xarch=v8plus"
                       fi
                       ;;
                    esac
                    if test "$WORKSHOP_OpenMP" = 1; then
                       PCFLAGS="$CFLAGS -xopenmp"
                    fi
                 fi;;

  mips-irix*)    if test "$CC" = cc; then
                    CFLAGS="-O"
                 fi;;   

  alpha*-osf*)   if test "$CC" = cc; then
                    CFLAGS="-newc -w0 -O5 -ansi_alias -ansi_args -fp_reorder -tune host -arch host -std1"
                 fi;;

  hppa*-hpux*)   if test "$CC" = cc; then
                    CFLAGS="-Ae +O3 +Oall"
                 fi;;
  powerpc-apple-Darwin5*) if test "$CC" = cc; then
                              CFLAGS=""
                          fi;;
  # AIX 4.3 later supports 64 bit computation and has OpenMP package
  # POWER3, PowerPC RS64 III and II support 64 bit computation
  rs6000*-aix* | powerpc*-aix*) 
                 if test "$CC" = cc -o "$CC" = xlc; then
                    if test -n "`/usr/sbin/lsattr -El proc0 | grep 'POWER3'`"; then
                       CFLAGS="-O3 -qarch=pwr3 -qtune=pwr3 -qansialias -w -q64"
                       ARFLAGS="-X64"
                    elif `/usr/sbin/lsattr -El proc0 | grep 'PowerPC_RS64-III' >/dev/null 2>&1`; then
                       CFLAGS="-O3 -qarch=rs64c -qtune=rs64c -qansialias -w -q64"
                       ARFLAGS="-X64"
                    elif `/usr/sbin/lsattr -El proc0 | grep 'PowerPC_RS64-II' >/dev/null 2>&1`; then
                       CFLAGS="-O3 -qarch=rs64b -qtune=rs64b -qansialias -w -q64"
                       ARFLAGS="-X64"
                    elif `/usr/sbin/lsattr -El proc0 | grep 'POWER' >/dev/null 2>&1`; then
                       CFLAGS="-O3 -qarch=pwrx -qtune=pwrx -qansialias -w"
                    else
                       CFLAGS="-O3 -qarch=ppc -qansialias -w"
  		       echo "*******************************************************"
		       echo "*  You have AIX on an unknown powerpc system.  It is  *"
		       echo "*  recommended that you use                           *"
		       echo "*                                                     *"
		       echo "*    CFLAGS=-O3 -qarch=ppc -qtune=xxx -qansialias -w  *"
		       echo "*                                 ^^^                 *"
		       echo "*  where xxx is 601, 603, 604, or whatever kind of    *"
                       echo "*  PowerPC CPU you have.   For more info, man cc.     *"
		       echo "*******************************************************"
                    fi

                    case "${host_os}" in
                    aix4.3*)
                       PCFLAGS="$CFLAGS -qsmp=omp"
                       ;;
                    esac
                fi;;
  esac

  # use default flags for gcc on all systems
  if test $ac_cv_prog_gcc = yes; then
#     CFLAGS="-O6 -fomit-frame-pointer -Wall -W -Wcast-qual -Wpointer-arith -Wcast-align -pedantic"
     CFLAGS="-O6 -fomit-frame-pointer"
  fi

  # test for gcc-specific flags:
  if test $ac_cv_prog_gcc = yes; then
    # -malign-double for x86 systems
    ACX_CHECK_CC_FLAGS(-malign-double,align_double,
	CFLAGS="$CFLAGS -malign-double")
    # -fstrict-aliasing for gcc-2.95+
    ACX_CHECK_CC_FLAGS(-fstrict-aliasing,fstrict_aliasing,
	CFLAGS="$CFLAGS -fstrict-aliasing")
  fi

  CPU_FLAGS=""
  if test "$GCC" = "yes"; then
	  dnl try to guess correct CPU flags, at least for linux
	  case "${host_cpu}" in
	  i586*)  ACX_CHECK_CC_FLAGS(-mcpu=pentium,cpu_pentium,
			[CPU_FLAGS=-mcpu=pentium],
			[ACX_CHECK_CC_FLAGS(-mpentium,pentium,
				[CPU_FLAGS=-mpentium])])
		  ;;
	  i686*)  ACX_CHECK_CC_FLAGS(-mcpu=pentiumpro,cpu_pentiumpro,
			[CPU_FLAGS=-mcpu=pentiumpro],
			[ACX_CHECK_CC_FLAGS(-mpentiumpro,pentiumpro,
				[CPU_FLAGS=-mpentiumpro])])
		  ;;
	  powerpc*)
		cputype=`(grep cpu /proc/cpuinfo | head -1 | cut -d: -f2 | sed 's/ //g') 2> /dev/null`
		is60x=`echo $cputype | egrep "^60[0-9]e?$"`
		if test -n "$is60x"; then
			ACX_CHECK_CC_FLAGS(-mcpu=$cputype,m_cpu_60x,
				CPU_FLAGS=-mcpu=$cputype)
		elif test "$cputype" = 750; then
                        ACX_PROG_GCC_VERSION(2,95,
                                ACX_CHECK_CC_FLAGS(-mcpu=750,m_cpu_750,
					CPU_FLAGS=-mcpu=750))
		fi
		if test -z "$CPU_FLAGS"; then
		        ACX_CHECK_CC_FLAGS(-mcpu=powerpc,m_cpu_powerpc,
				CPU_FLAGS=-mcpu=powerpc)
		fi
		if test -z "$CPU_FLAGS"; then
			ACX_CHECK_CC_FLAGS(-mpowerpc,m_powerpc,
				CPU_FLAGS=-mpowerpc)
		fi
	  esac
  fi

  if test -n "$CPU_FLAGS"; then
        CFLAGS="$CFLAGS $CPU_FLAGS"
  fi

  if test -z "$CFLAGS"; then
	echo ""
	echo "************************************************************"
        echo "* WARNING: Don't know the best CFLAGS for this system  *"
        echo "* Use  make CFLAGS=..., or edit the top level Makefile *"
	echo "* (otherwise, a default of CFLAGS=-O3 will be used)    *"
	echo "************************************************************"
	echo ""
        CFLAGS="-O3"
  fi

  ACX_CHECK_CC_FLAGS(${CFLAGS}, guessed_splcflags, , [
	echo ""
        echo "************************************************************"
        echo "* WARNING: The guessed CFLAGS don't seem to work with  *"
        echo "* your compiler.                                           *"
        echo "* Use  make CFLAGS=..., or edit the top level Makefile *"
        echo "************************************************************"
        echo ""
        CFLAGS=""
  ])
fi
])

dnl --------------------------------------------------
dnl Check if the OpenMP compiler and flags work or not
dnl --------------------------------------------------
dnl omp_set_num_threads() is a function of OpenMP standard.
AC_DEFUN(ACX_CHECK_OPENMP,
[
  AC_REQUIRE([ACX_PROG_CC_MAXOPT])
        
  echo 'void main(){omp_set_num_threads(2);}' > confptest.c
  sh -c "${CC} ${PCFLAGS} -o confptest confptest.c"
  if test -x confptest; then
    echo "checking whether OpenMP works: ${CC} ${PCFLAGS} yes"
    CFLAGS="$PCFLAGS"
  else
    echo "checking whether OpenMP works: ${CC} ${PCFLAGS} no"
    echo "**************************************************************"
    echo "* WARNING: The guessed OpenMP compiler and options don't     *"
    echo "* seem to work.                                              *"
    echo "* Use make CC=.. CCFLAGS=.., or edit the top level Makefile  *"
    echo "**************************************************************"
    echo
  fi
  rm -f confptest*
])

dnl like AC_SUBST, but replace XXX_variable_XXX instead of @variable@
dnl This macro protects VARIABLE from being diverted twice
dnl if this macro is called twice for it.
dnl AC_SUBST(VARIABLE)
define(ACX_SUBST_XXX,
[ifdef([ACX_SUBST_XXX_$1], ,
[define([ACX_SUBST_XXX_$1], )dnl
AC_DIVERT_PUSH(AC_DIVERSION_SED)dnl
s=XXX_$1_XXX=[$]$1=g
AC_DIVERT_POP()dnl
])])

