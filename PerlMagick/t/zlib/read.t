#!/usr/local/bin/perl
#
# Test reading an image which uses Zip compression
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
# 1) Test reading Zip compressed MIFF
# 
testRead( 'input.miff', 'eb4d6e084afe2835a1ad28ad7fc12ced' );
