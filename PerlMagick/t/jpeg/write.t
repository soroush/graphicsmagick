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
  '26f6885893c15f21a782ea1544e923fda5f815bbf6c1298f9dbc1feab9d228be',
  'aa3722317d53b101637aa2bbaea5b33317ee9cfd609e5ad0e9d583fd9e13afcd' );

#
# 2) Test with plane-interlaced image
#
++$test;
print( "Plane-interlaced JPEG ...\n" );
testReadWrite( 'input_plane.jpg', 'output_plane_tmp.jpg',
  q/quality=>80, interlace=>'Plane'/,
  '26f6885893c15f21a782ea1544e923fda5f815bbf6c1298f9dbc1feab9d228be',
  'aa3722317d53b101637aa2bbaea5b33317ee9cfd609e5ad0e9d583fd9e13afcd' );

