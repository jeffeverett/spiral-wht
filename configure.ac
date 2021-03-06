AC_INIT(spiral_wht.c)

# ----------------------------
# System info
# ----------------------------
# Some of the target information can be used in the Makefile;
# it also helps us pick a good default for besttrees
AC_CANONICAL_SYSTEM()
TARGET_CPU=$target_cpu
TARGET_VENDOR=$target_vendor
TARGET_OS=$target_os
AC_SUBST(TARGET_CPU)
AC_SUBST(TARGET_VENDOR)
AC_SUBST(TARGET_OS)

# ----------------------------
# RAM size provided by user
# ----------------------------
# specify RAM size to avoid timing of too large trees
AC_ARG_ENABLE(RAM, --enable-RAM=SIZE       system has SIZE RAM (in MB), RAM=$enableval, RAM=16)

MAXSIZE=20;

if test $RAM -gt   16; then MAXSIZE=21; fi
if test $RAM -gt   32; then MAXSIZE=22; fi
if test $RAM -gt   64; then MAXSIZE=23; fi
if test $RAM -gt  128; then MAXSIZE=24; fi
if test $RAM -gt  256; then MAXSIZE=25; fi
if test $RAM -gt  512; then MAXSIZE=26; fi
if test $RAM -gt 1024; then MAXSIZE=27; fi
if test $RAM -gt 2048; then MAXSIZE=28; fi
if test $RAM -gt 4096; then MAXSIZE=29; fi
if test $RAM -gt 8192; then MAXSIZE=30; fi
if test $RAM -le    8; then MAXSIZE=19; fi
if test $RAM -le    4; then MAXSIZE=18; fi

AC_SUBST(MAXSIZE)

# ----------------------------
# Enable DDL split method
# ----------------------------
# Enable Dynamic Data Layout (DDL) Approach
# since it might affect runtime; if this switch is turned on,
AC_ARG_ENABLE(DDL,
              --enable-DDL=BLSIZE   enable dynamic data layout (DDL) approach with Block SIZE (BLSIZE data points),
        [DDL=$enableval DDL_ON="yes"],
        [DDL=32 DDL_ON="no"])

BLSIZE=32

if test $DDL = "yes"; then DDL=32; fi

# BLSIZE = 4, 8, 16, 32, 64, 128, 256
if test $DDL -eq 4; then BLSIZE=4; fi
if test $DDL -eq 8; then BLSIZE=8; fi
if test $DDL -eq 16; then BLSIZE=16; fi
if test $DDL -eq 64; then BLSIZE=64; fi
if test $DDL -eq 128; then BLSIZE=128; fi
if test $DDL -eq 256; then BLSIZE=256; fi

AC_SUBST(BLSIZE)

# DDL options
# if not enabled do nothing
if test "$DDL_ON" = "yes"; then
   DDL_ON="-DDDL_ON"
   CHECK_DDL="checkddl"
else
   DDL_ON=
   CHECK_DDL="none"
fi

AC_SUBST(DDL_ON)
AC_SUBST(CHECK_DDL)


# ----------------------------
# Enable loop interleaving
# ----------------------------
# Enable Loop Interleaving approach
AC_ARG_ENABLE(IL,
              [--enable-IL=LEVEL   Enable loop interleaving method. LEVEL=1 will interleave twice. LEVEL=2 will interleave 2 and 4 times. LEVEL=3 will interleave 2, 4 or 8 times, etc.. Default LEVEL is 2.], 
        [IL=$enableval IL_ON="yes"],
        IL_ON="no")

if test "$IL" = "yes"; then
  IL=2
fi
if test "$IL" = "0"; then
  IL_ON="no"
fi

IL_LEVEL1=
IL_LEVEL2=
IL_LEVEL3=
IL_LEVEL4=
IL_LEVEL5=

if test "$IL_ON" = "yes"; then
  i=1
  while test "$i" -le "$IL"; do
    command="IL_LEVEL$i=-DIL_LEVEL$i"
    eval $command
    i=`expr $i + 1`
  done
else
  IL=0
fi

# Loop Interleaving options
# if not enabled, do nothing
if test "$IL_ON" = "yes"; then
  IL_ON="-DIL_ON"
  if test "$DDL_ON" = "-DDDL_ON" ; then
    CHECK_DDL_IL=checkddlil$IL
  fi
  CHECK_IL=checkil$IL
else
  IL_ON=
  CHECK_IL="none"
fi

AC_SUBST(IL)
AC_SUBST(IL_LEVEL1)
AC_SUBST(IL_LEVEL2)
AC_SUBST(IL_LEVEL3)
AC_SUBST(IL_LEVEL4)
AC_SUBST(IL_LEVEL5)

AC_SUBST(IL_ON)
AC_SUBST(CHECK_IL)
AC_SUBST(CHECK_DDL_IL)


# ----------------------------
# Enable PARA split method
# ----------------------------
# Enable Parallel Split (PARA) Approach
# since it might affect runtime; if this switch is turned on,
AC_ARG_ENABLE(PARA, --enable-PARA=PARA   enable parallel split approach with PARA number of threads,
        [PARA=$enableval PARA_ON="yes"], 
        [PARA=1 PARA_ON="no"])

if test "$PARA" = "yes"; then PARA=2; fi

AC_SUBST(PARA)

# PARA options
# if not enabled do nothing
if test "$PARA_ON" = "yes"; then
   PARA_ON="-DPARA_ON"
   CHECK_PARA="checkpara"
   INSTALL_PARA="make installpara"
   BESTTREE_P=besttrees.para

   if test "$DDL_ON" = "-DDDL_ON" ; then
      CHECK_PARA="checkparaddl"
   else
      BLSIZE=32
      AC_SUBST(BLSIZE)
   fi

   if test "$IL_ON" = "-DIL_ON" ; then
      if test "$DDL_ON" = "-DDDL_ON" ; then
         CHECK_PARA="checkparaddlil"
      else
         CHECK_PARA="checkparail"
      fi
   fi
else
   PARA_ON=
   CHECK_PARA="none"
   INSTALL_PARA=""
   BESTTREE_P=
fi

AC_SUBST(PARA_ON)
AC_SUBST(CHECK_PARA)
AC_SUBST(INSTALL_PARA)
AC_SUBST(BESTTREE_P)


# ----------------------------
# PCL monitoring
# ----------------------------
# select basic pcl monitoring
AC_ARG_ENABLE(PCL, --enable-PCL    enable pcl monitoring, PCL=$enableval, PCL=no)
AC_SUBST(PCL)

# select pcl profiling, use only for experimental purposes
# since it might affect runtime; if this switch is turned on,
# then also PCL is turned on
AC_ARG_ENABLE(PCL_PROFILE, --enable-PCL_PROFILE    enable pcl profiling, PCL_PROFILE=$enableval, PCL_PROFILE=no)
if test "$PCL_PROFILE" = "yes"; then
  PCL="yes"
fi

# PCL
# if not enabled do nothing
INSTALL_PCL="none"
if test "$PCL" = "no"; then
  PCL_PROF=none
else
  AC_CHECK_PROG(PCL_PROF, hpm, pclprof, none)
fi
if test "$PCL_PROF" = "pclprof"; then
  INSTALL_PCL="installpcl"
  LIB_PCL="-lpcl"
  if test "$target_cpu" = "sparc"; then
    LIBPERF="-lperfmon"
    INCLUDE_PCL_PATH="-I/usr/local/include" 
  else
    LIBPERF=
    INCLUDE_PCL_PATH= 
  fi
  if test "$PCL_PROFILE" = "yes"; then
    PCL_PROFILE="-DPCL_PROFILE"
  else
    PCL_PROFILE=
  fi
else
  LIB_PCL= 
  INCLUDE_PCL_PATH= 
  LIBPERF=
  PCL_PROFILE=
fi
AC_SUBST(PCL_PROF)
AC_SUBST(LIB_PCL)
AC_SUBST(INCLUDE_PCL_PATH)
AC_SUBST(LIBPERF)
AC_SUBST(PCL_PROFILE)
AC_SUBST(INSTALL_PCL)

# ----------------------------
# Dynamic programming script
# ----------------------------
# Choose which version of the dynamic programming script to use

if test "$PARA_ON" = "-DPARA_ON"; then
  cp wht_dp_para.prl.in wht_dp.prl.in
else
  if test "$DDL_ON" = "-DDDL_ON"; then
    cp wht_dp_ddl_il.prl.in wht_dp.prl.in
  else
    cp wht_dp_sdl_il.prl.in wht_dp.prl.in
  fi
fi

# ----------------------------
# Tree table
# ----------------------------
# Choose which version of the tree tables to use
# Right now, there are only two specialized versions.
# Hopefully, more can be found.
TABLE="besttrees.default"

if test "$target_cpu" = "sparc"; then
  TABLE="besttrees.sparc";
fi

if test "$target_cpu" = "i686"; then
  TABLE="besttrees.p2";
fi

if test "$target_cpu" = ""; then
  TABLE="besttrees.p2";
fi

AC_SUBST(TABLE)

if test "$prefix" = "NONE"; then
  FULL_LOC=/usr/local/var/wht_trees
  MEASURE_LOC=/usr/local/bin
else
  if test "$localstatedir" = '${prefix}/var'; then
    FULL_LOC=$prefix/var/wht_trees
  else
    FULL_LOC=$localstatedir/wht_trees
  fi
  if test "$bindir" = '${exec_prefix}/bin'; then
    MEASURE_LOC=$prefix/bin
  else
    MEASURE_LOC=$bindir
  fi
fi

AC_SUBST(FULL_LOC)
AC_SUBST(MEASURE_LOC)

# ----------------------------
# Programs and options
# ----------------------------
# C compiler
AC_CHECK_PROG(CC, cc, cc)
AC_PROG_CC
ACX_PROG_CC_EGCS
ACX_PROG_CC_MAXOPT
AC_SUBST(CFLAGS)
ACX_FIND_CC_BUGS
AC_SUBST(FORTE_WORKSHOP_6)                                                                                                                       

if test "$PARA_ON" = "-DPARA_ON"; then
  ACX_CHECK_OPENMP
fi
AC_SUBST(ARFLAGS)

# misc
AC_PATH_PROG(PERL, perl, none)
AC_SUBST(PERL)
AC_PROG_INSTALL

AC_CONFIG_FILES(Makefile ga/Makefile wht_dp.prl retime.prl simplop.prl transpose.h parallel.h)
AC_CONFIG_COMMANDS([change_perms], [chmod 755 *.prl])
AC_OUTPUT

