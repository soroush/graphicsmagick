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
  'ef46da08c96ef96c02d67af2a2da7ea31041a449a8b14ea6d6b8fb200d7b2e08');

#
# 2) Test Center gravity
#    Image should be centered in frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'Center'/,
  '04823e3a866cf9b420d2c95bf5842ed745f83d18dc8dd4a21f1be6f04eef783a');

#
# 3) Test NorthWest gravity
#    Image should be at top-left in frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'NorthWest'/,
  'c16f6f91320f055ad7340c7159afa0dab3ffb27180ec363a2bcf58a59c4fe59e');

#
# 4) Test North gravity
#    Image should be at top-center of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'North'/,
  '00f246b3342753024022337830f8bb4161542c5fd149ada68e2a4f0435b457e4');

#
# 5) Test NorthEast gravity
#    Image should be at top-right of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'NorthEast'/,
  '8e2857d5c7fbb190ee451b79b89700155a512214c193144c81b7c4bcc37f0418');

#
# 6) Test West gravity
#    Image should be at left-center of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'West'/,
  '439ae095b95bcc3f27bb219f117c829111a5da0be03dc48fe3c9496267dffa8f');

#
# 7) Test East gravity
#    Image should be at right-center of frame.
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'East'/,
  '6ee310265eba9606c129525403bba503f4e77fc05332db58a0c4800f328fc763');

#
# 8) Test SouthWest gravity
#    Image should be at bottom-left of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'SouthWest'/,
  '0078385ecc70ef08d96a3ed2a803c15702b3c024e7a9ac08bb91713e896ae88f');

#
# 9) Test South gravity
#    Image should be at bottom of frame
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'South'/,
  '999675b35f94299dc6924cd4bffe237ec1b92cbae98c8de42754da8ce38f5688');

#
# 10) Test SouthEast gravity
#     Image should be at bottom-right of frame.
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', geometry=>'90x80+5+5>', gravity=>'SouthEast'/,
  'dfd6db3bf01bf5e95d652a127d9df995aaa833e0acdec514c53e215b65c61376');

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
  '7591aa60772aad7ac341e4709f547f76a1e4239b2d85c8a3ded87482b344ee84');

#
# 12) Test Framed Montage with drop-shadows
#
++$test;
testMontage( q/bordercolor=>'blue', mattecolor=>'red'/, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'8x10',
  borderwidth=>'0', gravity=>'Center', shadow=>'True',background=>'gray'/,
  'dec374514ce6a49ff384f0d9c4efd6c2796e94e5902929f91aad9457b963f198',
  '856ebafc6a212a2d12240256e1e35d18198d254711036fd4a12fa2eeda85e937');

#
# 13) Test Framed Montage with drop-shadows and background texture
#
++$test;
testMontage( q/bordercolor=>'blue', mattecolor=>'red'/, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'8x10',
  borderwidth=>'0', gravity=>'Center', shadow=>'True', texture=>'granite:'/,
  'dfdbd5ddd1d48484822160c22ae3327b9fc068415b5fdf992ee2fe5b7581389e',
  '4b25bd939c0773523a950b66129e00e3c303710ffea89e9ae8d49795c426d69b');

#
# 14) Test Un-bordered, Un-framed Montage
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'False',
  borderwidth=>'0', gravity=>'Center', background=>'gray'/,
  'fd7ad72249c7fbd3dff4942d916edc5b5d7211827196a6d50c717212de95961c',
  'e7b65f5b15aed2e85837710b5f15399daead253ff51e9841001cac29720845eb');

#
# 15) Test Bordered, Un-framed Montage (frame=>'False')
#
++$test;
testMontage( q/bordercolor=>'red'/, 
  q/background=>'#696e7e', label=>'', tile=>'4x4', geometry=>'90x80+6+6>', frame=>'False',
  borderwidth=>'5', gravity=>'Center', background=>'gray'/,
  'cea55f11c06a780b82e4125ee5f3105169f7b6bcbf24742e8a4a6c06f62ef327',
  '7350b8b1f48f6667f2dd5b32f677bb174bfae25f51c2f860a88e26d1dc0dd282');

#
# 16) Test Bordered, Un-framed Montage (mode=>'UnFrame')
#
++$test;
testMontage( q/bordercolor=>'red'/, 
  q/label=>'', tile=>'4x4', geometry=>'90x80+6+6>', mode=>'UnFrame',
  borderwidth=>'5', gravity=>'Center', background=>'gray'/,
  '16ecaa37dbeb88ebfbf17ebc216e5d0700b8c7926338b6f5086d7b9989bcc42b',
  '94d6e440ade9935a78e30d0a1cbf265bd0b58d62f16f122620a75716e49306de');

#
# 17) Test Un-bordered, Un-framed Montage with 16x1 tile
#
++$test;
testMontage( q//,
  q/background=>'#696e7e', label=>'', tile=>'16x1', geometry=>'90x80+0+0>', frame=>'False',
  borderwidth=>'0', gravity=>'Center', background=>'gray'/,
  'b60e513f1f90718ab5530f7902e58a1628f33598653131f584415c52b5ee1719',
  '9db617a5528f3bc7b4e2d41965f3cf495a8328738a7bb4e4bba0b538873e282a');

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
