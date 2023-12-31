#
# LCMS configure script
# Written by Bob Friesenhahn, June 2003
# The bugs were introduced by Marti Maria 
#

AC_PREREQ(2.57)

#
# Set the package name and version
#
AC_INIT(lcms,1.18)

#
# Libtool library revision control info
# See the libtool documentation under the heading "Libtool's versioning
# system" in order to understand the meaning of these fields
#
# Here are a set of rules to help you update your library version
# information:
#
#  1. Start with version information of `0:0:0' for each libtool library.
#  2. Update the version information only immediately before a public
#     release of your software. More frequent updates are unnecessary, and
#     only guarantee that the current interface number gets larger faster.
#  3. If the library source code has changed at all since the last update,
#     then increment revision (`c:r:a' becomes `c:r+1:a').
#  4. If any interfaces have been added, removed, or changed since the last
#     update, increment current, and set revision to 0.
#  5. If any interfaces have been added since the last public release, then
#     increment age.
#  6. If any interfaces have been removed since the last public release,
#     then set age to 0.
#
LIBRARY_CURRENT=1
LIBRARY_REVISION=18
LIBRARY_AGE=0


AC_SUBST(LIBRARY_CURRENT)dnl
AC_SUBST(LIBRARY_REVISION)dnl
AC_SUBST(LIBRARY_AGE)dnl


# Obtain system type by running config.guess
AC_CANONICAL_SYSTEM

# Ensure that make can run correctly
AM_SANITY_CHECK


AM_INIT_AUTOMAKE([foreign 1.7.2 no-define dist-zip dist-bzip2])

# Disable JPEG.
AC_ARG_WITH(jpeg,
	      [  --without-jpeg          disable JPEG support],
	      [with_jpeg=$withval],
	      [with_jpeg='yes'])

# Disable TIFF.
AC_ARG_WITH(tiff,
	      [  --without-tiff          disable TIFF support],
	      [with_tiff=$withval],
	      [with_tiff='yes'])

# Disable ZLIB
AC_ARG_WITH(zlib,
	      [  --without-zlib          disable ZLIB support],
	      [with_zlib=$withval],
	      [with_zlib='yes'])

# Disable Python
AC_ARG_WITH(python,
	      [  --with-python           enable building python extension],
	      [with_python=$withval],
	      [with_python='no'])

# Check for programs
AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_CPP
AM_PROG_LD
AC_SUBST(LD)
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LN_S
AC_PROG_CXX

# Configure python. AM_PATH_PYTHON is basically braindead 
PYTHON=''
if test "$with_python" = 'yes'
then
  AM_PATH_PYTHON(2.2,PYTHON_OK='yes',PYTHON_OK='no')  
  if test -n "$PYTHON"
  then
	LCMS_PYTHON_PREFIX=`$PYTHON -c "import sys; print sys.prefix"`
	LCMS_PYEXECDIR=`$PYTHON -c "from distutils import sysconfig; print sysconfig.get_python_lib(1)"`
    LCMS_PYINCLUDE=`$PYTHON -c "from distutils import sysconfig; print sysconfig.get_python_inc()"`	
	LCMS_PYLIB=`$PYTHON -c "from distutils import sysconfig; print sysconfig.get_python_lib(0,1)"`
	AC_CHECK_FILE([$LCMS_PYINCLUDE/Python.h],PYTHON_OK='yes',PYTHON_OK='no')
    AC_SUBST(LCMS_PYEXECDIR)
    AC_SUBST(LCMS_PYINCLUDE)
    AC_SUBST(LCMS_PYLIB)
#	AC_SUBST(PYTHON_OK)
  fi     
fi

# If python is found then enable python support
AM_CONDITIONAL(SUPPORT_PYTHON,test -n "$PYTHON" -a "$PYTHON_OK" = 'yes')

#
# Tests for Windows
#
AC_EXEEXT
AC_OBJEXT

# Configure libtool
AC_ENABLE_SHARED
AC_ENABLE_STATIC
AC_LIBTOOL_WIN32_DLL
AC_LIBTOOL_SETUP
AC_PROG_LIBTOOL
AC_SUBST(LIBTOOL_DEPS)

# Add configure option --enable-maintainer-mode which enables dependency
# checking and generation useful to package maintainers.  This is made an
# option to avoid confusing end users.
AM_MAINTAINER_MODE

# If the C compiler does not fully support the ANSI C qualifier const,
# define const to be empty.
#AC_C_CONST

# If the C compiler supports the keyword inline, do nothing. Otherwise
# define inline to __inline__ or __inline if it accepts one of those,
# otherwise define inline to be empty.
AC_C_INLINE
AC_SUBST(inline)

# If words are stored with the most significant byte first (like
# Motorola and SPARC CPUs), define `WORDS_BIGENDIAN'.
AC_C_BIGENDIAN

# Obtain size of an 'char' and define as SIZEOF_CHAR
# Sets $ac_cv_sizeof_char
#AC_CHECK_SIZEOF(char)

# Obtain size of an 'unsigned char' and define as SIZEOF_UNSIGNED_CHAR
# Sets $ac_cv_sizeof_unsigned_char
#AC_CHECK_SIZEOF(unsigned char)

# Obtain size of an 'short' and define as SIZEOF_SHORT
# Sets $ac_cv_sizeof_short
#AC_CHECK_SIZEOF(short)

# Obtain size of an 'unsigned short' and define as SIZEOF_UNSIGNED_SHORT
# Sets $ac_cv_sizeof_unsigned_short
#AC_CHECK_SIZEOF(unsigned short)

# Obtain size of an 'int' and define as SIZEOF_INT
# Sets $ac_cv_sizeof_int
AC_CHECK_SIZEOF(int)

# Obtain size of an 'unsigned int' and define as SIZEOF_UNSIGNED_INT
# Sets $ac_cv_sizeof_unsigned_int
AC_CHECK_SIZEOF(unsigned int)

# Obtain size of a 'long' and define as SIZEOF_LONG
# Sets $ac_cv_sizeof_long
AC_CHECK_SIZEOF(long)

# Obtain size of a 'unsigned long' and define as SIZEOF_UNSIGNED_LONG
# Sets $ac_cv_sizeof_unsigned_long
AC_CHECK_SIZEOF(unsigned long)

# Obtain size of a 'long long' and define as SIZEOF_LONG_LONG.  If
# 'long long' is not supported then the value defined is zero.
# Sets $ac_cv_sizeof_long_long
AC_CHECK_SIZEOF(long long)

# Obtain size of a 'unsigned long long' and define as
# SIZEOF_UNSIGNED_LONG_LONG.  If 'unsigned long long' is not
# supported then the value defined is zero.
# Sets $ac_cv_sizeof_unsigned_long_long
AC_CHECK_SIZEOF(unsigned long long)

#
# Compute sized types for current CPU and compiler options
#

AC_MSG_CHECKING(for signed 8-bit type)
INT8_T='char'
AC_MSG_RESULT($INT8_T)
AC_SUBST(INT8_T)

AC_MSG_CHECKING(for unsigned 8-bit type)
UINT8_T='unsigned char'
AC_MSG_RESULT($UINT8_T)
AC_SUBST(UINT8_T)

AC_MSG_CHECKING(for signed 16-bit type)
INT16_T='short'
AC_MSG_RESULT($INT16_T)
AC_SUBST(INT16_T)

AC_MSG_CHECKING(for unsigned 16-bit type)
UINT16_T='unsigned short'
AC_MSG_RESULT($UINT16_T)
AC_SUBST(UINT16_T)

AC_MSG_CHECKING(for signed 32-bit type)
INT32_T='none'
if test $ac_cv_sizeof_int -eq 4
then
  INT32_T='int'
elif test $ac_cv_sizeof_long -eq 4
then
  INT32_T='long'
fi
AC_MSG_RESULT($INT32_T)
AC_SUBST(INT32_T)

AC_MSG_CHECKING(for unsigned 32-bit type)
UINT32_T='none'
if test $ac_cv_sizeof_unsigned_int -eq 4
then
  UINT32_T='unsigned int'
elif test $ac_cv_sizeof_unsigned_long -eq 4
then
  UINT32_T='unsigned long'
fi
AC_MSG_RESULT($UINT32_T)
AC_SUBST(UINT32_T)

AC_MSG_CHECKING(for signed 64-bit type)
INT64_T='none'
if test $ac_cv_sizeof_long -eq 8
then
  INT64_T='long'
elif test $ac_cv_sizeof_long_long -eq 8
then
  INT64_T='long long'
fi
AC_MSG_RESULT($INT64_T)
AC_SUBST(INT64_T)

AC_MSG_CHECKING(for unsigned 64-bit type)
UINT64_T='none'
if test $ac_cv_sizeof_unsigned_long -eq 8
then
  UINT64_T='unsigned long'
elif test $ac_cv_sizeof_unsigned_long_long -eq 8
then
  UINT64_T='unsigned long long'
fi
AC_MSG_RESULT($UINT64_T)
AC_SUBST(UINT64_T)


#
# Find math library
#
LIB_MATH=''
AC_CHECK_LIB(m,sqrt,LIB_MATH="-lm",,)
LIBS="$LIB_MATH $LIBS"
AC_SUBST(LIB_MATH)

#
# Check for JPEG
#
have_jpeg='no'
LIB_JPEG=''
if test ! "$with_jpeg" = 'no'
then
    AC_MSG_CHECKING(for JPEG support )
    AC_MSG_RESULT()
    failed=0;
    passed=0;
    AC_CHECK_HEADER(jconfig.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
    AC_CHECK_HEADER(jerror.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
    AC_CHECK_HEADER(jmorecfg.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
    AC_CHECK_HEADER(jpeglib.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
    AC_CHECK_LIB(jpeg,jpeg_read_header,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)

# Test for compatible JPEG library
if test ! "$ac_cv_jpeg_version_ok" = 'yes' ; then
AC_CACHE_CHECK(for JPEG library is version 6b or later, ac_cv_jpeg_version_ok,
[AC_TRY_COMPILE(
#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
,
changequote(<<, >>)dnl
<<
#if JPEG_LIB_VERSION < 62
#error IJG JPEG library must be version 6b or newer!
#endif
return 0;
>>,
changequote([, ])dnl
ac_cv_jpeg_version_ok='yes',
ac_cv_jpeg_version_ok='no')])
if test "$ac_cv_jpeg_version_ok" = 'yes' ; then
 AC_MSG_RESULT(yes)
 passed=`expr $passed + 1`
else
 AC_MSG_RESULT(no)
 failed=`expr $failed + 1`
fi
fi
    AC_MSG_CHECKING(if JPEG package is complete)
    if test $passed -gt 0
    then
    if test $failed -gt 0
    then
	AC_MSG_RESULT(no -- some components failed test)
        have_jpeg='no (failed tests)'
    else
	LIB_JPEG='-ljpeg'
	LIBS="$LIB_JPEG $LIBS"
	AC_DEFINE(HasJPEG,1,Define if you have JPEG library)
	AC_MSG_RESULT(yes)
        have_jpeg='yes'
    fi
    else
        AC_MSG_RESULT(no)
    fi
fi
AM_CONDITIONAL(HasJPEG, test "$have_jpeg" = 'yes')
AC_SUBST(LIB_JPEG)

#
# Check for ZLIB
#
have_zlib='no'
dnl PNG requires zlib so enable zlib check if PNG is requested
if test ! "$with_zlib" = 'no' || test ! "$with_png" = 'no'
then
  LIB_ZLIB=''
  AC_MSG_CHECKING(for ZLIB support )
  AC_MSG_RESULT()
  failed=0;
  passed=0;
  AC_CHECK_HEADER(zconf.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
  AC_CHECK_HEADER(zlib.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
  AC_CHECK_LIB(z,compress,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
  AC_CHECK_LIB(z,uncompress,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
  AC_CHECK_LIB(z,deflate,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
  AC_CHECK_LIB(z,inflate,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
  AC_CHECK_LIB(z,gzseek,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
  AC_CHECK_LIB(z,gztell,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
  AC_MSG_CHECKING(if ZLIB package is complete)
  if test $passed -gt 0
  then
    if test $failed -gt 0
    then
      AC_MSG_RESULT(no -- some components failed test)
      have_zlib='no (failed tests)'
    else
      LIB_ZLIB='-lz'
      LIBS="$LIB_ZLIB $LIBS"
      AC_DEFINE(HasZLIB,1,Define if you have zlib compression library)
      AC_MSG_RESULT(yes)
      have_zlib='yes'
    fi
  else
    AC_MSG_RESULT(no)
  fi
fi
AM_CONDITIONAL(HasZLIB, test "$have_zlib" = 'yes')
AC_SUBST(LIB_ZLIB)

#
# Check for TIFF
#
have_tiff='no'
LIB_TIFF=''
if test ! "$with_tiff" = 'no'
then
    AC_MSG_CHECKING(for TIFF support )
    AC_MSG_RESULT()
    failed=0;
    passed=0;
    AC_CHECK_HEADER(tiff.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
    AC_CHECK_HEADER(tiffio.h,passed=`expr $passed + 1`,failed=`expr $failed + 1`)
    AC_CHECK_LIB(tiff,TIFFOpen,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
    AC_CHECK_LIB(tiff,TIFFClientOpen,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
    AC_CHECK_LIB(tiff,TIFFIsByteSwapped,passed=`expr $passed + 1`,failed=`expr $failed + 1`,)
    AC_MSG_CHECKING(if TIFF package is complete)
    if test $passed -gt 0
    then
    if test $failed -gt 0
    then
	AC_MSG_RESULT(no -- some components failed test)
	have_tiff='no (failed tests)'
    else
	LIB_TIFF='-ltiff'
	LIBS="$LIB_TIFF $LIBS"
	AC_DEFINE(HasTIFF,1,Define if you have TIFF library)
	AC_MSG_RESULT(yes)
	have_tiff='yes'
	AC_CHECK_HEADERS(tiffconf.h)
    fi
    else
    AC_MSG_RESULT(no)
    fi
fi
AM_CONDITIONAL(HasTIFF, test "$have_tiff" = 'yes')
AC_SUBST(LIB_TIFF)

# Libraries that the LCMS library depends on
LCMS_LIB_DEPLIBS="$LIB_MATH"
LCMS_LIB_DEPLIBS=`echo $LCMS_LIB_DEPLIBS | sed -e 's/  */ /g'`
AC_SUBST(LCMS_LIB_DEPLIBS)

# Libraries that the jpegicc program depends on
JPEGICC_DEPLIBS="$LIB_JPEG $LIB_MATH"
JPEGICC_DEPLIBS=`echo $JPEGICC_DEPLIBS | sed -e 's/  */ /g'`
AC_SUBST(JPEGICC_DEPLIBS)

# Libraries that the tifficc program depends on
TIFFICC_DEPLIBS="$LIB_TIFF $LIB_JPEG $LIB_ZLIB $LIB_MATH"
TIFFICC_DEPLIBS=`echo $TIFFICC_DEPLIBS | sed -e 's/  */ /g'`
AC_SUBST(TIFFICC_DEPLIBS)

LIBS=''


#
# Perform substitutions
#
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([lcms.pc])
AC_CONFIG_FILES([include/icc34.h])
AC_CONFIG_FILES([include/Makefile])
AC_CONFIG_FILES([jpegicc/Makefile])
AC_CONFIG_FILES([python/Makefile])
AC_CONFIG_FILES([samples/Makefile])
AC_CONFIG_FILES([testbed/Makefile])
AC_CONFIG_FILES([tifficc/Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_OUTPUT
