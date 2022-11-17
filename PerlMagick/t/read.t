#!/usr/local/bin/perl
# Copyright (C) 2003-2022 GraphicsMagick Group
# Copyright (C) 2002 ImageMagick Studio
# Copyright (C) 1991-1999 E. I. du Pont de Nemours and Company
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
#
# Test reading formats supported directly by GraphicsMagick
#
# Whenever a new test is added/removed, be sure to update the
# 1..n ouput.
#
BEGIN { $| = 1; $test=1; print "TAP version 13\n1..88\n"; }
END {print "not ok $test\n" unless $loaded;}
use Graphics::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't' || die 'Cd failed';

print("AVS X image file ...\n");
testReadCompare('input.avs', 'reference/read/input_avs.miff', q//, 0, 0);

print("Microsoft Windows bitmap image file ...\n");
++$test;
testReadCompare('input.bmp', 'reference/read/input_bmp.miff', q//, 0, 0);

print("Microsoft Windows 24-bit bitmap image file ...\n");
++$test;
testReadCompare('input.bmp24', 'reference/read/input_bmp24.miff', q//, 0, 0);

print("Cineon Gray image file ...\n");
++$test;
testReadCompare('input_gray.cin', 'reference/read/input_gray_cin.miff', q//, 0, 0);

print("Cineon RGB image file ...\n");
++$test;
testReadCompare('input_rgb.cin', 'reference/read/input_rgb_cin.miff', q//, 0, 0);

print("ZSoft IBM PC multi-page Paintbrush file ...\n");
++$test;
testReadCompare('input.dcx', 'reference/read/input_dcx.miff', q//, 0, 0);

print("Microsoft Windows bitmap image file ...\n");
++$test;
testReadCompare('input.dib', 'reference/read/input_dib.miff', q//, 0, 0);

print("Flexible Image Transport System 8-bit ...\n");
++$test;
testReadCompare('input_gray_08bit.fits', 'reference/read/input_gray_08bit_fits.miff', q//, 0, 0);

print("Flexible Image Transport System LSB 16-bit ...\n");
++$test;
testReadCompare('input_gray_16bit.fits', 'reference/read/input_gray_16bit_fits.miff', q//, 0.002, 0.004);

print("Flexible Image Transport System LSB 32-bit ...\n");
++$test;
testReadCompare('input_gray_32bit.fits', 'reference/read/input_gray_32bit_fits.miff', q//, 0.002, 0.004);

print("Flexible Image Transport System LSB double ...\n");
++$test;
testReadCompare('input_gray_lsb_double.fits', 'reference/read/input_gray_lsb_double_fits.miff', q//, 0.002, 0.004);

print("Flexible Image Transport System MSB 16-bit ...\n");
++$test;
testReadCompare('input_gray_msb_16bit.fits', 'reference/read/input_gray_msb_16bit_fits.miff', q//, 0.002, 0.004);

print("Flexible Image Transport System MSB 32-bit ...\n");
++$test;
testReadCompare('input_gray_msb_32bit.fits', 'reference/read/input_gray_msb_32bit_fits.miff', q//, 0.002, 0.004);

print("Flexible Image Transport System MSB 64-bit ...\n");
++$test;
testReadCompare('input_gray_msb_64bit.fits', 'reference/read/input_gray_msb_64bit_fits.miff', q//, 0.002, 0.004);

print("Flexible Image Transport System MSB float ...\n");
++$test;
testReadCompare('input_gray_msb_float.fits', 'reference/read/input_gray_msb_float_fits.miff', q//, 0.002, 0.004);

print("Flexible Image Transport System MSB double ...\n");
++$test;
testReadCompare('input_gray_msb_double.fits', 'reference/read/input_gray_msb_double_fits.miff', q//, 0.002, 0.004);

print("CompuServe graphics interchange format ...\n");
++$test;
testReadCompare('input.gif', 'reference/read/input_gif.miff', q//, 0, 0);

print("CompuServe graphics interchange format (1987) ...\n");
++$test;
testReadCompare('input.gif87', 'reference/read/input_gif87.miff', q//, 0, 0);

print("Gradient (gradual passing from one shade to another) ...\n");
++$test;
testReadCompare('gradient:red-blue', 'reference/read/gradient.miff',
                q/size=>"256x256"/, 0.003, 0.004);

print("GRANITE (granite texture) ...\n");
++$test;
testReadCompare('granite:', 'reference/read/granite.miff', q/size=>"70x46"/, 0, 0);

print("HRZ Slow scan TV ...\n");
++$test;
testRead('input.hrz', 'cf3aed87b1364c93509e15b2519c26dd0302dcef024ac92d0c870aeacf472524');

print("MacPaint ...\n");
++$test;
testReadCompare('input.mac', 'reference/read/input_mac.bmp', q//, 0, 0);

print("MAT (MatLab logical 8-bit LSB integer) ...\n");
++$test;
testReadCompare('input_logical_lsb_08bit.mat', 'reference/read/input_logical_lsb_08bit_mat.miff', q//, 0, 0);

print("MAT (MatLab gray 8-bit LSB integer) ...\n");
++$test;
testReadCompare('input_gray_lsb_08bit.mat', 'reference/read/input_gray_lsb_08bit_mat.miff', q//, 0, 0);

print("MAT (MatLab gray 8-bit MSB integer) ...\n");
++$test;
testReadCompare('input_gray_msb_08bit.mat', 'reference/read/input_gray_msb_08bit_mat.miff', q//, 0, 0);

print("MAT (MatLab gray 16-bit LSB integer) ...\n");
++$test;
testReadCompare('input_gray_lsb_16bit.mat', 'reference/read/input_gray_lsb_16bit_mat.miff', q//, 0, 0);

print("MAT (MatLab gray 32-bit LSB integer) ...\n");
++$test;
testReadCompare('input_gray_lsb_32bit.mat', 'reference/read/input_gray_lsb_32bit_mat.miff', q//, 0, 0);

print("MAT (MatLab gray 32-bit LSB float) ...\n");
++$test;
testReadCompare('input_gray_lsb_float.mat', 'reference/read/input_gray_lsb_float_mat.miff', q//, 0.002, 0.004);

print("MAT (MatLab gray 64-bit LSB double) ...\n");
++$test;
testReadCompare('input_gray_lsb_double.mat', 'reference/read/input_gray_lsb_double_mat.miff', q//, 0.002, 0.004);

print("MAT (MatLab RGB 8-bit LSB integer) ...\n");
++$test;
testReadCompare('input_rgb_lsb_08bit.mat', 'reference/read/input_rgb_lsb_08bit_mat.miff', q//, 0, 0);

print("MAT (MatLab V4 gray 64-bit LSB double) ...\n");
++$test;
testReadCompare('input_gray_lsb_double_V4.mat', 'reference/read/input_gray_lsb_double_V4_mat.miff', q//, 0.002, 0.004);

print("Microsoft icon ...\n");
++$test;
testReadCompare('input.ico', 'reference/read/input_ico.miff', q//, 0, 0);

print("Magick image file format ...\n");
++$test;
testReadCompare('input.miff', 'reference/read/input_miff.miff', q//, 0, 0);

print("MTV Raytracing image format ...\n");
++$test;
testReadCompare('input.mtv', 'reference/read/input_mtv.miff', q//, 0, 0);

print("Xv's visual schnauzer format. ...\n");
++$test;
testReadCompare('input_p7.p7', 'reference/read/input_p7.miff', q//,
                0.002, 0.004);

print("NULL (white image) ...\n");
++$test;
testReadCompare('NULL:white', 'reference/read/input_null_white.miff', q/size=>"70x46"/, 0, 0);

print("NULL (black image) ...\n");
++$test;
testReadCompare('NULL:black', 'reference/read/input_null_black.miff', q/size=>"70x46"/, 0, 0);

print("NULL (DarkOrange image) ...\n");
++$test;
testReadCompare('NULL:DarkOrange', 'reference/read/input_null_DarkOrange.miff', q/size=>"70x46"/, 0, 0);

print("Portable bitmap format (black and white), ASCII format ...\n");
++$test;
testReadCompare('input_p1.pbm', 'reference/read/input_pbm_p1.miff', q//, 0, 0);

print("Portable bitmap format (black and white), binary format ...\n");
++$test;
testReadCompare('input_p4.pbm', 'reference/read/input_pbm_p4.miff', q//, 0, 0);

print("ZSoft IBM PC Paintbrush file ...\n");
++$test;
testReadCompare('input.pcx', 'reference/read/input_pcx.miff', q//, 0, 0);

print("Portable graymap format (gray scale), ASCII format ...\n");
++$test;
testReadCompare('input_p2.pgm', 'reference/read/input_pgm_p2.miff', q//, 0, 0);

print("Portable graymap format (gray scale), binary format ...\n");
++$test;
testReadCompare('input_p5.pgm', 'reference/read/input_pgm_p5.miff', q//, 0, 0);

print("Apple Macintosh QuickDraw/PICT file ...\n");
++$test;
testReadCompare('input.pict', 'reference/read/input_pict.miff', q//, 0, 0);

print("Alias/Wavefront RLA image format (gray scale) ...\n");
++$test;
testReadCompare('input_gray.rla', 'reference/read/input_gray_rla.miff', q//, 0, 0);

print("Alias/Wavefront RLA image format (RGB) ...\n");
++$test;
testReadCompare('input_rgb.rla', 'reference/read/input_rgb_rla.miff', q//, 0, 0);

print("Utah Raster Toolkit (URT) RLE image format (gray scale) ...\n");
++$test;
testReadCompare('input_gray.rle', 'reference/read/input_gray_rle.miff', q//, 0, 0);

print("Utah Raster Toolkit (URT) RLE image format (RGB) ...\n");
++$test;
testReadCompare('input.rle', 'reference/read/input_rle.miff', q//, 0, 0);

print("Portable pixmap format (color), ASCII format ...\n");
++$test;
testReadCompare('input_p3.ppm', 'reference/read/input_ppm_p3.miff', q//, 0, 0);

print("Portable pixmap format (color), binary format ...\n");
++$test;
testReadCompare('input_p6.ppm', 'reference/read/input_ppm_p6.miff', q//, 0, 0);

print("Adobe Photoshop bitmap file ...\n");
++$test;
if (featureSupported("PSD")) {
    testReadCompare('input.psd', 'reference/read/input_psd.miff', q//, 0, 0);
} else {
    print "ok $test # skip PSD not supported\n";
}

print("Irix RGB image file ...\n");
++$test;
testReadCompare('input.sgi', 'reference/read/input_sgi.miff', q//, 0, 0);

print("SUN 1-bit Rasterfile ...\n");
++$test;
testReadCompare('input.im1', 'reference/read/input_im1.miff', q//, 0, 0);

print("SUN 8-bit Rasterfile ...\n");
++$test;
testReadCompare('input.im8', 'reference/read/input_im8.miff', q//, 0, 0);

print("SUN TrueColor Rasterfile ...\n");
++$test;
testReadCompare('sun:input.im24', 'reference/read/input_im24.miff', q//, 0, 0);

print("Topol Type 1 ...\n");
++$test;
testReadCompare('topol:topol_1.ras', 'reference/read/topol_1.miff', q//, 0, 0);

print("Topol Type 2 ...\n");
++$test;
testReadCompare('topol:topol_2.ras', 'reference/read/topol_2.miff', q//, 0, 0);

print("Topol Type 3 ...\n");
++$test;
testReadCompare('topol:topol_3.ras', 'reference/read/topol_3.miff', q//, 0, 0);

print("Topol Type 4 ...\n");
++$test;
testReadCompare('topol:topol_4.ras', 'reference/read/topol_4.miff', q//, 0, 0);

print("Topol Type 5 ...\n");
++$test;
testReadCompare('topol:topol_5.ras', 'reference/read/topol_5.miff', q//, 0, 0);

print("Topol Type 7 ...\n");
++$test;
testReadCompare('topol:topol_7.ras', 'reference/read/topol_7.miff', q//, 0, 0);

print("Truevision Targa image file (true color) ...\n");
++$test;
testReadCompare('input_24.tga', 'reference/read/input_tga_24.miff', q//, 0, 0);

print("Truevision Targa image file (true color - RLE compressed) ...\n");
++$test;
testReadCompare('input_24rle.tga', 'reference/read/input_tga_24.miff', q//, 0, 0);

print("Truevision Targa image file (true color 16) ...\n");
++$test;
testReadCompare('input_16.tga', 'reference/read/input_tga_16.miff', q//, 0, 0);

print("Truevision Targa image file (true color 16 - RLE compressed) ...\n");
++$test;
testReadCompare('input_16rle.tga', 'reference/read/input_tga_16.miff', q//, 0, 0);

print("Truevision Targa image file (palette color) ...\n");
++$test;
testReadCompare('input_8_CC.tga', 'reference/read/input_tga_8_CC.miff', q//, 0, 0);

print("Truevision Targa image file (gray) ...\n");
++$test;
testReadCompare('input_8_BW.tga', 'reference/read/input_tga_8_BW.miff', q//, 0, 0);

print("Truevision Targa image file (bilevel) ...\n");
++$test;
testReadCompare('input_mono.tga', 'reference/read/input_mono.tga.miff', q//, 0, 0);

print("PSX TIM file ...\n");
++$test;
testReadCompare('input.tim', 'reference/read/input_tim.miff', q//, 0, 0);

print("Khoros Visualization image file ...\n");
++$test;
testReadCompare('input.viff', 'reference/read/input_viff.miff', q//, 0, 0);

print("WBMP (Wireless Bitmap (level 0) image) ...\n");
++$test;
testReadCompare('input.wbmp', 'reference/read/input_wbmp.miff', q//, 0, 0);

print("WPG (Word Perfect Graphics image, bilevel WPG level 1) ...\n");
++$test;
testRead('input1_1.wpg', '24f136b95afc7c11b9d5e1a22c4b53030b7aced49bf83eebaa5985128e0600f3');

print("WPG (Word Perfect Graphics image, 4 bit depth WPG level 1) ...\n");
++$test;
testRead('input1_4.wpg', '27b4d537ef0fac232f1f9cb8bc1b3e1378d6804a43d4b813770119cd6146e225');

print("WPG (Word Perfect Graphics image, 8 bit depth WPG level 1) ...\n");
++$test;
testRead('input1_8_1.wpg', '6c8a6e6e237e03a636e976b3a04fcab56470854d6051b8959bd968b526843c48');

print("WPG (Word Perfect Graphics image, 1 bit depth + 24 bit depth WPG level 2) ...\n");
++$test;
testReadCompare('input2_TC1.wpg', 'reference/read/input2_TC1_wpg.miff', q//, 0, 0);

print("WPG (Word Perfect Graphics image, 8 bit depth WPG level 2) ...\n");
++$test;
testReadCompare('input2_8.wpg', 'reference/read/input2_8_wpg.miff', q//, 0, 0);

print("X Windows system bitmap (black and white only) ...\n");
++$test;
testReadCompare('input.xbm', 'reference/read/input_xbm.miff', q//, 0, 0);

print("XC: Constant image of X server color ...\n");
++$test;
testReadCompare('xc:black', 'reference/read/input_xc_black.miff', q/size=>"70x46",, depth=>8/, 0, 0);

print("X Windows system pixmap file (bilevel) ...\n");
++$test;
testRead('input_bilevel.xpm', 'c100134c37f7d48c2908747f40f4f18e63a00ebf2b6124cdbe63f078bc74cc5f');

print("X Windows system pixmap file (color) ...\n");
++$test;
testRead('input.xpm', 'f84cb6b5817ea2f6f313c2bdd90c94af0d0e7fb144528994050b355f8d5e6a94');
# Q:32 mean-error=0.23551931713272, maximum-error=0.989543041912839

print("X Windows system pixmap file (color, 16bit palette) ...\n");
++$test;
testRead('input_pal16.xpm', '45f0083676dcd95255f24b897df490a310a28f8f180c86ba2450e1624656f1f8');

print("X Windows system pixmap file (color, 3 chars per pixel) ...\n");
++$test;
testRead('input_3chars.xpm', '3716883efcd24e9ea3500febb299b77a86484574a6b81e11707e6b06d3c208f7');

#print("X Windows system window dump file (color) ...\n");
#++$test;
#testReadCompare('input.xwd', 'reference/read/input_xwd.miff', q//, 0, 0);

print("TILE (Tile image with a texture) ...\n");
# This is an internal generated format
# We will tile using the default image and a MIFF file
#
++$test;
testReadCompare('TILE:input.miff', 'reference/read/input_tile.miff',
                q/size=>"140x92", depth=>8/, 0, 0);

print("CMYK format ...\n");
++$test;
testReadCompare('input_70x46.cmyk', 'reference/read/input_cmyk.miff',
                q/size=>"70x46", depth=>8/, 0, 0);

print("GRAY format ...\n");
++$test;
testReadCompare('input_70x46.gray', 'reference/read/input_gray.miff',
                q/size=>"70x46", depth=>8/, 0, 0);

print("RGB format ...\n");
++$test;
testReadCompare('input_70x46.rgb', 'reference/read/input_rgb.miff',
                q/size=>"70x46", depth=>8/, 0, 0);

print("RGBA format ...\n");
++$test;
testReadCompare('input_70x46.rgba', 'reference/read/input_rgba.miff',
                q/size=>"70x46", depth=>8/, 0, 0);

print("UYVY format ...\n");
++$test;
testReadCompare('input_70x46.uyvy', 'reference/read/input_uyvy.miff',
                q/size=>"70x46", depth=>8/, 0.006, 0.008);
