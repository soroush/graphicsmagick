#!/usr/local/bin/perl
#
# Test montage method.
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1, print "1..19\n"; }
END {print "not ok 1\n" unless $loaded;}
use Image::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't' || die 'Cd failed';

#
# 1) Test montage defaults (except no label that requires an exact font)
#
testMontage( q//,
  q/background=>'#696e7e', label=>''/,
  'bf45478f35a5cd632ed02041d4aadd84af091030f765c6b072451edb5ca37684');

#
# 2) Test Center gravity
#    Image should be centered in frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'Center'/,
  '208b621b0e8f11728c5daae37f7a296babb76554c90c80217e726fe9a586ed52');

#
# 3) Test NorthWest gravity
#    Image should be at top-left in frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'NorthWest'/,
  'b41b28173517a85ae0cce139f09c69ed397110ec785188ba0072ecb0aef881bd');

#
# 4) Test North gravity
#    Image should be at top-center of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'North'/,
  '4d72cc28a4e7322f835b24f079392d0bc1cca87561c9e5eff4018ad3e08d1408');

#
# 5) Test NorthEast gravity
#    Image should be at top-right of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'NorthEast'/,
  'bec35e5d0b3b2539ce1160da25c0a1dfc570161d379b4a69ac49c1d0b76993ea');

#
# 6) Test West gravity
#    Image should be at left-center of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'West'/,
  '1628db34e7a331501c2976b8f718b2d2f680b8c0c2bced9a9186843fd7921b80');

#
# 7) Test East gravity
#    Image should be at right-center of frame.
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'East'/,
  '2c2321957d274eeeb8d129fa8e89ab087c9b29a3a1a045f1e5fff654a8d59e0f');

#
# 8) Test SouthWest gravity
#    Image should be at bottom-left of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'SouthWest'/,
  '4392da66cc5c513a61b080ff34eb316140413d04cec9b3ecea4004516e659ddc');

#
# 9) Test South gravity
#    Image should be at bottom of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'South'/,
  '3d9588730ac1b618268f486e4f774201741ce8830f496e317f5cbab7fbebf75c');

#
# 10) Test SouthEast gravity
#     Image should be at bottom-right of frame.
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'SouthEast'/,
  'c5d36925275d62749504ee43c23646dd5717192b7920a912b4c868cac27a3beb');

#
# 11) Test Framed Montage
#
# Image border color 'bordercolor' controls frame background color
# Image matte color 'mattecolor' controls frame color
# Image pen color 'pen' controls label text foreground color
++$test;
testMontage( q/bordercolor=>'blue', mattecolor=>'red'/, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+3+3>', frame=>'8x10',
  borderwidth=>'0', gravity=>'Center', background=>'gray'/,
  '716c6cdfb75b3ce0f45b0c3ad8ac6bf16870b44829dafb7a88411f0f2eb17725',
  '8752bcb6bfb0376e726326f97d5619d280d80b79af0cc4178bcddb07b99027d8');

#
# 12) Test Framed Montage with drop-shadows
#
++$test;
testMontage( q/bordercolor=>'blue', mattecolor=>'red'/, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'8x10',
  borderwidth=>'0', gravity=>'Center', shadow=>'True',background=>'gray'/,
  'dec374514ce6a49ff384f0d9c4efd6c2796e94e5902929f91aad9457b963f198',
  '19bc6f6c4a2a3250894bd31ca729bf22c8ecfeeb3e0f1d876e8a9201f111859f');

#
# 13) Test Framed Montage with drop-shadows and background texture
#
++$test;
testMontage( q/bordercolor=>'blue', mattecolor=>'red'/, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'8x10',
  borderwidth=>'0', gravity=>'Center', shadow=>'True', texture=>'granite:'/,
  'dfdbd5ddd1d48484822160c22ae3327b9fc068415b5fdf992ee2fe5b7581389e',
  'dbb71f796bf5deb7c93adde7088a659349191f5dc002573079d46a5e60587d9a');

#
# 14) Test Un-bordered, Un-framed Montage
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'False',
  borderwidth=>'0', gravity=>'Center', background=>'gray'/,
  'fd7ad72249c7fbd3dff4942d916edc5b5d7211827196a6d50c717212de95961c',
  '16cd0fc709aee5928df7e7eb2298dd235a13e5d34d171fdee8e27aad6ddbfea1');

#
# 15) Test Bordered, Un-framed Montage (frame=>'False')
#
++$test;
testMontage( q/bordercolor=>'red'/, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'False',
  borderwidth=>'5', gravity=>'Center', background=>'gray'/,
  'cea55f11c06a780b82e4125ee5f3105169f7b6bcbf24742e8a4a6c06f62ef327',
  'fbc115a43a4f1645c84d9e4174577c6c343b5f0fb09b429dc4ee8d23aa7c061b');

#
# 16) Test Bordered, Un-framed Montage (mode=>'UnFrame')
#
++$test;
testMontage( q/bordercolor=>'red'/, 
  q/label=>'', tile=>'4x4', geometry=>'90x80+6+6>', mode=>'UnFrame',
  borderwidth=>'5', gravity=>'Center', background=>'gray'/,
  '16ecaa37dbeb88ebfbf17ebc216e5d0700b8c7926338b6f5086d7b9989bcc42b',
  '243cbaa33eaba0ff5e316124ad7620f25ef7cac2fd10393e01e3248eac793d7c');

#
# 17) Test Un-bordered, Un-framed Montage with 16x1 tile
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', tile=>'16x1', geometry=>'90x80+0+0>', frame=>'False',
  borderwidth=>'0', gravity=>'Center', background=>'gray'/,
  'b60e513f1f90718ab5530f7902e58a1628f33598653131f584415c52b5ee1719',
  '399a943baf51c8a1c26fa92460302af18b49f97d80527881b5b7cce595befb31');

#
# 18) Test concatenated thumbnail Montage (concatenated via special Concatenate mode)
#     Thumbnails should be compacted tightly together in a grid
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80>', mode=>'Concatenate'/,
  'aa24b5ea6f3775fd8f115730fc2ece8a9c6c7bccd2ae835caa6778c6c4db8e2f',
  '94f72cf702e54c52eda26f461fa7bdd541cbddfdca74d2619b12f1c308b0c37d');
#
# 19) Test concatenated thumbnail Montage (concatentated by setting params to zero)
#     Thumbnails should be compacted tightly together in a grid
#
++$test;
testMontage( q//, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'+0+0', frame=>'False', shadow=>'False',
  borderwidth=>'0', background=>'gray'/,
  'bae96cb788c8c5e231945548279605642560708bfca7ddd255a6ab08302031c5',
  'e3aa9c4c515bd8ddcc7dae52d608da62c20dbe0b30d3ae5c948a113401630eaf');
