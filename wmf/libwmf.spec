Summary: library and utilities for displaying and converting metafile images
Name: libwmf
Version: 0.2.1
Release: 1
Copyright: LGPL
Group: System Environment/Libraries
Source: http://download.sourceforge.net/wvware/libwmf-%{version}.tar.gz
URL: http://wvware.sourceforge.net/

%description
This is a library for interpreting metafile images and either displaying them
using X or converting them to standard formats such as PNG, JPEG, PS, EPS,...

%package devel
Summary: Development tools for programs to manipulate metafile images
Group: Development/Libraries
Requires: libwmf = %{PACKAGE_VERSION}
Serial: 1

%description devel
The libwmf-devel package contains the header files and static
libraries necessary for developing programs using libwmf.

If you want to develop programs which will manipulate metafile images, you
should install libwmf-devel. You'll also need to install the libwmf package.

%prep
%setup
./configure --prefix=/usr --without-expat --without-xml

%build
make

%install
make install

%files
%defattr(-,root,root)
/usr/bin/wmf2eps
/usr/bin/wmf2fig
/usr/bin/wmf2svg
/usr/bin/wmf2gd
/usr/bin/wmf2x
/usr/bin/wmf2magick
/usr/bin/libwmf-fontmap
/usr/lib/libwmf-0.2.so.1.0.0
/usr/lib/libwmf-0.2.so.1
/usr/lib/libwmf.so
/usr/lib/libwmf.la
/usr/share/libwmf/fonts/n019003l.afm
/usr/share/libwmf/fonts/n019003l.pfb
/usr/share/libwmf/fonts/n019004l.afm
/usr/share/libwmf/fonts/n019004l.pfb
/usr/share/libwmf/fonts/n019023l.afm
/usr/share/libwmf/fonts/n019023l.pfb
/usr/share/libwmf/fonts/n019024l.afm
/usr/share/libwmf/fonts/n019024l.pfb
/usr/share/libwmf/fonts/n021003l.afm
/usr/share/libwmf/fonts/n021003l.pfb
/usr/share/libwmf/fonts/n021004l.afm
/usr/share/libwmf/fonts/n021004l.pfb
/usr/share/libwmf/fonts/n021023l.afm
/usr/share/libwmf/fonts/n021023l.pfb
/usr/share/libwmf/fonts/n021024l.afm
/usr/share/libwmf/fonts/n021024l.pfb
/usr/share/libwmf/fonts/n022003l.afm
/usr/share/libwmf/fonts/n022003l.pfb
/usr/share/libwmf/fonts/n022004l.afm
/usr/share/libwmf/fonts/n022004l.pfb
/usr/share/libwmf/fonts/n022023l.afm
/usr/share/libwmf/fonts/n022023l.pfb
/usr/share/libwmf/fonts/n022024l.afm
/usr/share/libwmf/fonts/n022024l.pfb
/usr/share/libwmf/fonts/s050000l.afm
/usr/share/libwmf/fonts/s050000l.pfb
/usr/share/libwmf/fonts/fontmap
%doc README COPYING ChangeLog
/usr/share/doc/libwmf/gd.html
/usr/share/doc/libwmf/index.html
/usr/share/doc/libwmf/tutapi.html
/usr/share/doc/libwmf/tutipa.html
/usr/share/doc/libwmf/dlayer.html
/usr/share/doc/libwmf/wmfpic.png
/usr/share/doc/libwmf/smlogo2.gif
/usr/share/doc/libwmf/caolan/2.html
/usr/share/doc/libwmf/caolan/Arc.html
/usr/share/doc/libwmf/caolan/BitBlt.html
/usr/share/doc/libwmf/caolan/Bmpfrmat.html
/usr/share/doc/libwmf/caolan/Chord.html
/usr/share/doc/libwmf/caolan/CreateBrushIndirect.html
/usr/share/doc/libwmf/caolan/CreatePenIndirect.html
/usr/share/doc/libwmf/caolan/CreateRegion.html
/usr/share/doc/libwmf/caolan/DeleteObject.html
/usr/share/doc/libwmf/caolan/DibBitBlt.html
/usr/share/doc/libwmf/caolan/DibCreatePatternBrush.html
/usr/share/doc/libwmf/caolan/DibStretchBlt.html
/usr/share/doc/libwmf/caolan/Ellipse.html
/usr/share/doc/libwmf/caolan/ExcludeClipRect.html
/usr/share/doc/libwmf/caolan/ExtFloodFill.html
/usr/share/doc/libwmf/caolan/ExtTextOut.html
/usr/share/doc/libwmf/caolan/FillRgn.html
/usr/share/doc/libwmf/caolan/FloodFill.html
/usr/share/doc/libwmf/caolan/FrameRgn.html
/usr/share/doc/libwmf/caolan/IntersectClipRect.html
/usr/share/doc/libwmf/caolan/InvertRgn.html
/usr/share/doc/libwmf/caolan/LineTo.html
/usr/share/doc/libwmf/caolan/MoveTo.html
/usr/share/doc/libwmf/caolan/OffsetClipRgn.html
/usr/share/doc/libwmf/caolan/OffsetViewportOrg.html
/usr/share/doc/libwmf/caolan/OffsetWindowOrg.html
/usr/share/doc/libwmf/caolan/PaintRgn.html
/usr/share/doc/libwmf/caolan/PatBlt.html
/usr/share/doc/libwmf/caolan/Pie.html
/usr/share/doc/libwmf/caolan/PolyPolygon.html
/usr/share/doc/libwmf/caolan/Polygon.html
/usr/share/doc/libwmf/caolan/Polyline.html
/usr/share/doc/libwmf/caolan/Rectangle.html
/usr/share/doc/libwmf/caolan/RestoreDC.html
/usr/share/doc/libwmf/caolan/RoundRect.html
/usr/share/doc/libwmf/caolan/SaveDC.html
/usr/share/doc/libwmf/caolan/ScaleViewportExt.html
/usr/share/doc/libwmf/caolan/ScaleWindowExt.html
/usr/share/doc/libwmf/caolan/SelectClipRgn.html
/usr/share/doc/libwmf/caolan/SelectObject.html
/usr/share/doc/libwmf/caolan/SetBkColor.html
/usr/share/doc/libwmf/caolan/SetBkMode.html
/usr/share/doc/libwmf/caolan/SetDIBitsToDevice.html
/usr/share/doc/libwmf/caolan/SetMapMode.html
/usr/share/doc/libwmf/caolan/SetMapperFlags.html
/usr/share/doc/libwmf/caolan/SetPixel.html
/usr/share/doc/libwmf/caolan/SetPolyFillMode.html
/usr/share/doc/libwmf/caolan/SetROP2.html
/usr/share/doc/libwmf/caolan/SetStretchBltMode.html
/usr/share/doc/libwmf/caolan/SetTextAlign.html
/usr/share/doc/libwmf/caolan/SetTextCharacterExtra.html
/usr/share/doc/libwmf/caolan/SetTextColor.html
/usr/share/doc/libwmf/caolan/SetTextJustification.html
/usr/share/doc/libwmf/caolan/SetViewportExt.html
/usr/share/doc/libwmf/caolan/SetViewportOrg.html
/usr/share/doc/libwmf/caolan/SetWindowExt.html
/usr/share/doc/libwmf/caolan/SetWindowOrg.html
/usr/share/doc/libwmf/caolan/StretchDib.html
/usr/share/doc/libwmf/caolan/TextOut.html
/usr/share/doc/libwmf/caolan/dibs.html
/usr/share/doc/libwmf/caolan/esc.html
/usr/share/doc/libwmf/caolan/index.html
/usr/share/doc/libwmf/caolan/mapmode.html
/usr/share/doc/libwmf/caolan/ora-clp.html
/usr/share/doc/libwmf/caolan/ora-wmf.html
/usr/share/doc/libwmf/caolan/rop.html
/usr/share/doc/libwmf/caolan/support.html
/usr/share/doc/libwmf/caolan/support2.html
/usr/share/doc/libwmf/caolan/template.html
/usr/share/doc/libwmf/caolan/unknown.html
/usr/share/doc/libwmf/caolan/msmeta01.gif
/usr/share/doc/libwmf/caolan/msmeta02.gif
/usr/share/doc/libwmf/caolan/msmeta03.gif
/usr/share/doc/libwmf/caolan/msmeta04.gif
/usr/share/doc/libwmf/caolan/pics/weave.jpg
/usr/share/doc/libwmf/html/annotated.html
/usr/share/doc/libwmf/html/api_8c.html
/usr/share/doc/libwmf/html/api_8h.html
/usr/share/doc/libwmf/html/bbuf_8c.html
/usr/share/doc/libwmf/html/classes.html
/usr/share/doc/libwmf/html/clip_8h.html
/usr/share/doc/libwmf/html/coord_8h.html
/usr/share/doc/libwmf/html/dc_8h.html
/usr/share/doc/libwmf/html/defaults_8h.html
/usr/share/doc/libwmf/html/defs_8h.html
/usr/share/doc/libwmf/html/doxygen.css
/usr/share/doc/libwmf/html/doxygen.gif
/usr/share/doc/libwmf/html/eps_2bmp_8h.html
/usr/share/doc/libwmf/html/eps_2device_8h.html
/usr/share/doc/libwmf/html/eps_2draw_8h.html
/usr/share/doc/libwmf/html/eps_8c.html
/usr/share/doc/libwmf/html/eps_8h.html
/usr/share/doc/libwmf/html/fig_2bmp_8h.html
/usr/share/doc/libwmf/html/fig_2device_8h.html
/usr/share/doc/libwmf/html/fig_2draw_8h.html
/usr/share/doc/libwmf/html/fig_2font_8h.html
/usr/share/doc/libwmf/html/fig_8c.html
/usr/share/doc/libwmf/html/fig_8h.html
/usr/share/doc/libwmf/html/files.html
/usr/share/doc/libwmf/html/functions.html
/usr/share/doc/libwmf/html/fund_8h.html
/usr/share/doc/libwmf/html/gd_2bmp_8h.html
/usr/share/doc/libwmf/html/gd_2device_8h.html
/usr/share/doc/libwmf/html/gd_2draw_8h.html
/usr/share/doc/libwmf/html/gd_8c.html
/usr/share/doc/libwmf/html/gd_8h.html
/usr/share/doc/libwmf/html/globals.html
/usr/share/doc/libwmf/html/include_2libwmf_2color_8h.html
/usr/share/doc/libwmf/html/index.html
/usr/share/doc/libwmf/html/ipa_2bmp_8h.html
/usr/share/doc/libwmf/html/ipa_2eps_2region_8h.html
/usr/share/doc/libwmf/html/ipa_2fig_2region_8h.html
/usr/share/doc/libwmf/html/ipa_2font_8h.html
/usr/share/doc/libwmf/html/ipa_2gd_2region_8h.html
/usr/share/doc/libwmf/html/ipa_2magick_2region_8h.html
/usr/share/doc/libwmf/html/ipa_2plot_2region_8h.html
/usr/share/doc/libwmf/html/ipa_2svg_2region_8h.html
/usr/share/doc/libwmf/html/ipa_2x_2region_8h.html
/usr/share/doc/libwmf/html/ipa_8c.html
/usr/share/doc/libwmf/html/ipa_8h.html
/usr/share/doc/libwmf/html/macro_8h.html
/usr/share/doc/libwmf/html/magick_2bmp_8h.html
/usr/share/doc/libwmf/html/magick_2device_8h.html
/usr/share/doc/libwmf/html/magick_2draw_8h.html
/usr/share/doc/libwmf/html/magick_8c.html
/usr/share/doc/libwmf/html/magick_8h.html
/usr/share/doc/libwmf/html/meta_8c.html
/usr/share/doc/libwmf/html/meta_8h.html
/usr/share/doc/libwmf/html/player_2region_8h.html
/usr/share/doc/libwmf/html/player_8c.html
/usr/share/doc/libwmf/html/plot_2bmp_8h.html
/usr/share/doc/libwmf/html/plot_2device_8h.html
/usr/share/doc/libwmf/html/plot_2draw_8h.html
/usr/share/doc/libwmf/html/plot_8c.html
/usr/share/doc/libwmf/html/plot_8h.html
/usr/share/doc/libwmf/html/record_8h.html
/usr/share/doc/libwmf/html/src_2ipa_2fig_2color_8h.html
/usr/share/doc/libwmf/html/src_2ipa_2ipa_2color_8h.html
/usr/share/doc/libwmf/html/src_2ipa_2x_2color_8h.html
/usr/share/doc/libwmf/html/src_2player_2color_8h.html
/usr/share/doc/libwmf/html/stream_8c.html
/usr/share/doc/libwmf/html/struct__wmfAPI.html
/usr/share/doc/libwmf/html/struct__wmfAPI__Options.html
/usr/share/doc/libwmf/html/struct__wmfBMP.html
/usr/share/doc/libwmf/html/struct__wmfBMP__Draw__t.html
/usr/share/doc/libwmf/html/struct__wmfBMP__Read__t.html
/usr/share/doc/libwmf/html/struct__wmfBrush.html
/usr/share/doc/libwmf/html/struct__wmfColorData.html
/usr/share/doc/libwmf/html/struct__wmfDC.html
/usr/share/doc/libwmf/html/struct__wmfD__Coord.html
/usr/share/doc/libwmf/html/struct__wmfD__Rect.html
/usr/share/doc/libwmf/html/struct__wmfDrawArc__t.html
/usr/share/doc/libwmf/html/struct__wmfDrawLine__t.html
/usr/share/doc/libwmf/html/struct__wmfDrawPixel__t.html
/usr/share/doc/libwmf/html/struct__wmfDrawRectangle__t.html
/usr/share/doc/libwmf/html/struct__wmfDrawText__t.html
/usr/share/doc/libwmf/html/struct__wmfFT__CacheEntry.html
/usr/share/doc/libwmf/html/struct__wmfFT__Mapping.html
/usr/share/doc/libwmf/html/struct__wmfFlood__t.html
/usr/share/doc/libwmf/html/struct__wmfFont.html
/usr/share/doc/libwmf/html/struct__wmfFontData.html
/usr/share/doc/libwmf/html/struct__wmfFontMap.html
/usr/share/doc/libwmf/html/struct__wmfFunctionReference.html
/usr/share/doc/libwmf/html/struct__wmfGS__FontData.html
/usr/share/doc/libwmf/html/struct__wmfGS__FontInfo.html
/usr/share/doc/libwmf/html/struct__wmfHead.html
/usr/share/doc/libwmf/html/struct__wmfMapping.html
/usr/share/doc/libwmf/html/struct__wmfMetaHeader.html
/usr/share/doc/libwmf/html/struct__wmfPen.html
/usr/share/doc/libwmf/html/struct__wmfPlaceableMetaHeader.html
/usr/share/doc/libwmf/html/struct__wmfPolyLine__t.html
/usr/share/doc/libwmf/html/struct__wmfPolyRectangle__t.html
/usr/share/doc/libwmf/html/struct__wmfRGB.html
/usr/share/doc/libwmf/html/struct__wmfROP__Draw__t.html
/usr/share/doc/libwmf/html/struct__wmfStream.html
/usr/share/doc/libwmf/html/struct__wmfUserData__t.html
/usr/share/doc/libwmf/html/struct__wmfXML__FontData.html
/usr/share/doc/libwmf/html/struct__wmfXML__FontInfo.html
/usr/share/doc/libwmf/html/struct__wmf__eps__t.html
/usr/share/doc/libwmf/html/struct__wmf__fig__t.html
/usr/share/doc/libwmf/html/struct__wmf__fig__t_1_1__wmf__fig__image.html
/usr/share/doc/libwmf/html/struct__wmf__gd__t.html
/usr/share/doc/libwmf/html/struct__wmf__gd__t_1_1__wmf__gd__sink.html
/usr/share/doc/libwmf/html/struct__wmf__magick__t.html
/usr/share/doc/libwmf/html/struct__wmf__magick__t_1_1__wmf__magick__image.html
/usr/share/doc/libwmf/html/struct__wmf__plot__t.html
/usr/share/doc/libwmf/html/struct__wmf__svg__t.html
/usr/share/doc/libwmf/html/struct__wmf__svg__t_1_1__wmf__svg__image.html
/usr/share/doc/libwmf/html/struct__wmf__template__t.html
/usr/share/doc/libwmf/html/struct__wmf__x__t.html
/usr/share/doc/libwmf/html/svg_2bmp_8h.html
/usr/share/doc/libwmf/html/svg_2device_8h.html
/usr/share/doc/libwmf/html/svg_2draw_8h.html
/usr/share/doc/libwmf/html/svg_8c.html
/usr/share/doc/libwmf/html/svg_8h.html
/usr/share/doc/libwmf/html/template_8c.html
/usr/share/doc/libwmf/html/template_8h.html
/usr/share/doc/libwmf/html/types_8h.html
/usr/share/doc/libwmf/html/x_2bmp_8h.html
/usr/share/doc/libwmf/html/x_2device_8h.html
/usr/share/doc/libwmf/html/x_2draw_8h.html
/usr/share/doc/libwmf/html/x_2font_8h.html
/usr/share/doc/libwmf/html/x_8c.html
/usr/share/doc/libwmf/html/x_8h.html

%files devel
%defattr(-,root,root)
/usr/bin/libwmf-config
/usr/include/libwmf/gd/gd.h
/usr/include/libwmf/gd/gdcache.h
/usr/include/libwmf/gd/gd_clip.h
/usr/include/libwmf/gd/gd_io.h
/usr/include/libwmf/gd/gdfontg.h
/usr/include/libwmf/gd/gdfontl.h
/usr/include/libwmf/gd/gdfontmb.h
/usr/include/libwmf/gd/gdfonts.h
/usr/include/libwmf/gd/gdfontt.h
/usr/include/libwmf/api.h
/usr/include/libwmf/color.h
/usr/include/libwmf/defs.h
/usr/include/libwmf/fund.h
/usr/include/libwmf/ipa.h
/usr/include/libwmf/types.h
/usr/include/libwmf/macro.h
/usr/include/libwmf/eps.h
/usr/include/libwmf/fig.h
/usr/include/libwmf/svg.h
/usr/include/libwmf/gd.h
/usr/include/libwmf/x.h
/usr/include/libwmf/plot.h
/usr/include/libwmf/magick.h
/usr/lib/libwmf.a
