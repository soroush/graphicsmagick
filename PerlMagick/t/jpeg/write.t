#!/usr/local/bin/perl
#
# Test reading JPEG images
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1; print "1..2\n"; }
END {print "not ok $test\n" unless $loaded;}

use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/jpeg' || die 'Cd failed';

#
# 1) Test with non-interlaced image
#
print( "Non-interlaced JPEG ...\n" );
testReadWrite( 'input.jpg',
  'output_tmp.jpg',
  q/quality=>80, interlace=>'None'/,
  '4be31dc0bd8de3881beaad3f383c51d47eac9e195ed07a7bb03da5dcf1b87af8',
  '5ceb0fb09ebda1887a23985f16970b691a15a697b339cb9fb1d6f2fc9944fed2' );

#
# 2) Test with plane-interlaced image
#
++$test;
print( "Plane-interlaced JPEG ...\n" );
testReadWrite( 'input_plane.jpg', 'output_plane_tmp.jpg',
  q/quality=>80, interlace=>'Plane'/,
  '4be31dc0bd8de3881beaad3f383c51d47eac9e195ed07a7bb03da5dcf1b87af8',
  '5ceb0fb09ebda1887a23985f16970b691a15a697b339cb9fb1d6f2fc9944fed2' );

