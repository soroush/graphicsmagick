# Copyright (C) 2003 GraphicsMagick Group
# Copyright (C) 2002 ImageMagick Studio
# Copyright (C) 1998,1999 E. I. du Pont de Nemours and Company
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
# NOTE: set TEST_VERBOSE=1 in order to see verbose output from tests.
#
#   In order to run just one test verbosely do something like
#     make TEST_VERBOSE=1 TEST_FILES=t/filter.t test

use ExtUtils::MakeMaker;
use Config;

# Compute test specification
my $delegate_tests='t/*.t';
my $delegate;
foreach $delegate (qw/ bzlib cgm hpgl jbig jng jp2 jpeg lcms mpeg png ps rad tiff ttf wmf x xfig zlib/) {
  if( -d "t/$delegate" ) {
    $delegate_tests .= " t/$delegate/*.t";
  }
}

# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.
WriteMakefile
  (
   # Module description
   'ABSTRACT'	=> 'GraphicsMagick PERL Extension',

   # Perl module name is Graphics::Magick
   'NAME'	=> 'Graphics::Magick',

   # Module author
   'AUTHOR'    => 'GraphicsMagick Group',

   # Module version
   'VERSION'   => '1.3.6',

   # Preprocessor defines
   'DEFINE'	=> ' -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES=1  -DHAVE_CONFIG_H',     # e.g., '-DHAVE_SOMETHING' 

   # Header search specfication and preprocessor flags
   'INC'	=> '-I../ -I.. -I/usr/local/include/freetype2 -I/usr/openwin/include -I/usr/openwin/include/X11 -I/usr/local/include/libxml2',

   # C compiler
   'CC'	        => 'gcc-4.3.4 -std=gnu99',

   # C pre-processor flags (e.g. -I & -D options)
   # 'CPPFLAGS'    => "$Config{'cppflags'} -I/usr/local/include/freetype2 -I/usr/openwin/include -I/usr/openwin/include/X11 -I/usr/local/include/libxml2",

   # C compiler flags (e.g. -O -g)
   'CCFLAGS'     => "$Config{'ccflags'} -fopenmp -O0 -Wall -D_REENTRANT -pthreads",

   # Linker
   'LD'          => $Config{'ld'} == $Config{'cc'} ? 'gcc-4.3.4 -std=gnu99' : $Config{'ld'},

   # Linker flags for building an executable
   'LDFLAGS'    =>  "-L/usr/local/lib $Config{'ldflags'}",

   # Linker flags for building a dynamically loadable module
   'LDDLFLAGS'  => "-L/usr/local/lib $Config{'lddlflags'}",

   # Install PerlMagick binary into GraphicsMagick bin directory
   'INSTALLBIN'	=> '/usr/local/bin',

   # Library specification
   'LIBS'	=> ['-L/usr/local/lib -lGraphicsMagick -Wl,-zlazyload -L/usr/local/lib -R/usr/local/lib -L/usr/openwin/lib -R/usr/openwin/lib -L/usr/local/lib -L/usr/local/lib -ljbig -llcms -ltiff -lfreetype -ljasper -ljpeg -lpng -lwmflite -ldpstk -ldps -lXext -lSM -lICE -lX11 -lsocket -lnsl -lbz2 -lxml2 -lz -lm -lgomp -lpthread'],

   # Perl binary name (if a Perl binary is built)
   'MAP_TARGET'	=> 'PerlMagick',

   # Let CFLAGS drive optimization flags by setting OPTIMIZE to empty
   # 'OPTIMIZE'	=> '',

   # Use same compiler as GraphicsMagick
   'PERLMAINCC'	=> 'gcc-4.3.4',

   # Set Perl installation prefix to GraphicsMagick installation prefix
#   'PREFIX'	=> '/usr/local',

   # Include delegate directories in tests
   test		=> { TESTS	=>	$delegate_tests},

   ($Config{'archname'} =~ /-object$/i ? ('CAPI' => 'TRUE') : ()),
);


#
# Substitutions for "makeaperl" section.
#
sub MY::makeaperl {
     package MY; # so that "SUPER" works right
     my $inherited = shift->SUPER::makeaperl(@_);

     # Stinky ExtUtils::MM_Unix likes to append its own library path to $(CC),
     # prior to any user-specified library path so that an installed library is
     # used rather than the library just built.  This substitution function
     # tries to insert our library path first. Also, use the same compiler used
     # to build perlmain.c to link so that a C++ compiler may be used if
     # necessary.
     $inherited =~ s:MAP_LINKCMD\s.*\s*\$\(CC\):MAP_LINKCMD   = \$(PERLMAINCC) -L/usr/local/lib: ;
     $inherited;
 }

#
# Substitutions for "test" section.
#
#sub MY::test {
#     package MY; # so that "SUPER" works right
#     my $inherited = shift->SUPER::test(@_);
#
     # Run tests in our environment
#     $inherited =~ s:PERL_DL_NONLAZY=1:/bin/bash ../rungm.sh PERL_DL_NONLAZY=1:g ;
#     $inherited;
# }
