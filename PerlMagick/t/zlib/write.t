#!/usr/local/bin/perl
#
# Test writing files using zlib-based compression
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1; print "1..1\n"; }
END {print "not ok $test\n" unless $loaded;}

use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't/zlib' || die 'Cd failed';

#
# 1) Test writing Zip-compressed MIFF
#

testReadWrite( 'input.miff',
  'output.miff',
  q/compression=>'Zip'/,
  'a5d122b2b7a0944d835243afd4a81815bbea5bc1ae6c3a6ba79301662b4474a1' );

$test = 0;  # Quench PERL compliaint

