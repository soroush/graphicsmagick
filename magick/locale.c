/* 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
%                                                                             % 
%                                                                             % 
%                                                                             % 
%                  L       OOO    CCCC   AAA   L      EEEEE                   % 
%                  L      O   O  C      A   A  L      E                       % 
%                  L      O   O  C      AAAAA  L      EEE                     % 
%                  L      O   O  C      A   A  L      E                       % 
%                  LLLLL   OOO    CCCC  A   A  LLLLL  EEEEE                   % 
%                                                                             % 
%                                                                             % 
%                    ImageMagick Locale Message Methods                       % 
%                                                                             % 
%                                                                             % 
%                              Software Design                                % 
%                                John Cristy                                  % 
%                                Kyle Shorter                                 % 
%                               September 2002                                % 
%                                                                             % 
%                                                                             % 
%  Copyright (C) 2002 ImageMagick Studio, a non-profit organization dedicated % 
%  to making software imaging solutions freely available.                     % 
%                                                                             % 
%  Permission is hereby granted, free of charge, to any person obtaining a    % 
%  copy of this software and associated documentation files ("ImageMagick"),  % 
%  to deal in ImageMagick without restriction, including without limitation   % 
%  the rights to use, copy, modify, merge, publish, distribute, sublicense,   % 
%  and/or sell copies of ImageMagick, and to permit persons to whom the       % 
%  ImageMagick is furnished to do so, subject to the following conditions:    % 
%                                                                             % 
%  The above copyright notice and this permission notice shall be included in % 
%  all copies or substantial portions of ImageMagick.                         % 
%                                                                             % 
%  Licensor ("ImageMagick Studio LLC") warrants that the copyright in and to  % 
%  the Software ("ImageMagick") is owned by ImageMagick Studio LLC or that    % 
%  ImageMagick is distributed by ImageMagick Studio LLC under a valid current % 
%  license. Except as expressly stated in the immediately preceding           % 
%  sentence, ImageMagick is provided by ImageMagick Studio LLC, contributors, % 
%  and copyright owners "AS IS", without warranty of any kind, express or     % 
%  implied, including but not limited to the warranties of merchantability,   % 
%  fitness for a particular purpose and non-infringement.  In no event shall  % 
%  ImageMagick Studio LLC, contributors or copyright owners be liable for     % 
%  any claim, damages, or other liability, whether in an action of contract,  % 
%  tort or otherwise, arising from, out of or in connection with              % 
%  ImageMagick.                                                               % 
%                                                                             % 
%  Except as contained in this notice, the name of the ImageMagick Studio     % 
%  shall not be used in advertising or otherwise to promote the sale, use or  % 
%  other dealings in ImageMagick without prior written authorization from the % 
%  ImageMagick Studio.                                                        % 
%                                                                             % 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
% 
% 
% 
*/ 
 
/* 
  Include declarations. 
*/ 
#include "studio.h" 
#include "utility.h" 
 
/* 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
%                                                                             % 
%                                                                             % 
%                                                                             % 
%   G e t L o c a l e M e s s a g e                                           % 
%                                                                             % 
%                                                                             % 
%                                                                             % 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
% 
%  GetLocaleMessage() returns a message in the current locale that matches the 
%  supplied tag. 
% 
%  The format of the GetLocaleMessage method is: 
% 
%      const char *GetLocaleMessage(const char *tag) 
% 
%  A description of each parameter follows: 
% 
%    o tag: Return a message that matches this tag in the current locale. 
% 
% 
*/ 
/* This method is autogenerated-- do not edit */
const char *GetLocaleMessage(const char *tag)
{
#define NEXT_FIELD ((p = (np = strchr((tp = np), '/')) ? np++ : (np = tp + strlen(tp))), tp)


   static const char *locale = 0;
   register const char *p, *tp, *np;
   if (!tag || *tag == '\0')
      return "";

   if ( (!locale &&
         ( (!(locale = setlocale(LC_CTYPE, 0)) || *locale == '\0') &&
           (!(locale = getenv("LC_ALL"))       || *locale == '\0') &&
           (!(locale = getenv("LC_MESSAGES"))     || *locale == '\0') &&
           (!(locale = getenv("LC_CTYPE"))     || *locale == '\0') &&
           (!(locale = getenv("LANG"))         || *locale == '\0') ) )
         || !LocaleCompare(locale, "C"))
      locale = "C";

   if (!LocaleNCompare(locale,"en_US",5))
      locale = "C";

   tp = locale;
   p = locale + strlen(locale);
   np = tag;
  if (LocaleNCompare(locale, "C", 1) || p - tp != 1)
    return tag;
  else
    switch (*NEXT_FIELD)
    {
    default:
      return tag;

    case 'b':  case 'B':
      if (p - tp == 4 && !LocaleNCompare(tp, "Blob", 4))
        if (LocaleNCompare(NEXT_FIELD, "Error", 5) || p - tp != 5)
          return tag;
        else
          switch (*NEXT_FIELD)
          {
          default:
            return tag;

          case 'u':  case 'U':
            if (p - tp == 18 && !LocaleNCompare(tp, "UnableToCreateBlob", 18))
              return *np ? tag : "Unable to create blob";
            else
            if (p - tp == 16 && !LocaleNCompare(tp, "UnableToOpenFile", 16))
              return *np ? tag : "Unable to open file";
            else
            if (p - tp == 16 && !LocaleNCompare(tp, "UnableToReadBlob", 16))
              return *np ? tag : "Unable to read blob";
            else
            if (p - tp == 16 && !LocaleNCompare(tp, "UnableToReadFile", 16))
              return *np ? tag : "Unable to read file";
            else
            if (p - tp == 17 && !LocaleNCompare(tp, "UnableToWriteBlob", 17))
              return *np ? tag : "Unable to write blob";
            else
            if (p - tp == 23 && !LocaleNCompare(tp, "UnrecognizedImageFormat", 23))
              return *np ? tag : "Unrecognized image format";
            else
              return tag;

          case 'z':  case 'Z':
            if (p - tp == 26 && !LocaleNCompare(tp, "ZeroLengthBlobNotPermitted", 26))
              return *np ? tag : "Zero-length blob not permitted";
            else
              return tag;
          }
      else
        return tag;

    case 'c':  case 'C':
      if (p - tp == 5 && !LocaleNCompare(tp, "Cache", 5))
        switch (*NEXT_FIELD)
        {
        default:
          return tag;

        case 'e':  case 'E':
          if (p - tp == 5 && !LocaleNCompare(tp, "Error", 5))
            switch (*NEXT_FIELD)
            {
            default:
              return tag;

            case 'i':  case 'I':
              if (p - tp == 31 && !LocaleNCompare(tp, "InconsistentPeristentCacheDepth", 31))
                return *np ? tag : "Inconsistent peristent cache depth";
              else
                return tag;

            case 'p':  case 'P':
              if (p - tp == 19 && !LocaleNCompare(tp, "PixelCacheIsNotOpen", 19))
                return *np ? tag : "Pixel cache is not open";
              else
                return tag;

            case 'u':  case 'U':
              if (p - tp == 18 && !LocaleNCompare(tp, "UnableToCloneCache", 18))
                return *np ? tag : "Unable to clone cache";
              else
              if (p - tp == 19 && !LocaleNCompare(tp, "UnableToExtendCache", 19))
                return *np ? tag : "Unable to extend cache";
              else
              if (p - tp == 21 && !LocaleNCompare(tp, "UnableToGetCacheNexus", 21))
                return *np ? tag : "Unable to get cache nexus";
              else
              if (p - tp == 26 && !LocaleNCompare(tp, "UnableToGetPixelsFromCache", 26))
                return *np ? tag : "Unable to get pixels from cache";
              else
              if (p - tp == 17 && !LocaleNCompare(tp, "UnableToOpenCache", 17))
                return *np ? tag : "Unable to open cache";
              else
              if (p - tp == 20 && !LocaleNCompare(tp, "UnableToPersistCache", 20))
                return *np ? tag : "Unable to persist pixel cache";
              else
              if (p - tp == 22 && !LocaleNCompare(tp, "UnableToReadPixelCache", 22))
                return *np ? tag : "Unable to read pixel cache";
              else
              if (p - tp == 17 && !LocaleNCompare(tp, "UnableToSyncCache", 17))
                return *np ? tag : "Unable to sync cache";
              else
                return tag;
            }
          else
            return tag;

        case 'f':  case 'F':
          if (p - tp == 10 && !LocaleNCompare(tp, "FatalError", 10))
            if (LocaleNCompare(NEXT_FIELD, "UnableToExtendPixelCache", 24) || p - tp != 24)
              return tag;
            else
              return *np ? tag : "Unable to extend pixel cache";
          else
            return tag;
        }
      else
      if (p - tp == 5 && !LocaleNCompare(tp, "Coder", 5))
        switch (*NEXT_FIELD)
        {
        default:
          return tag;

        case 'e':  case 'E':
          if (p - tp == 5 && !LocaleNCompare(tp, "Error", 5))
            switch (*NEXT_FIELD)
            {
            default:
              return tag;

            case 'c':  case 'C':
              if (p - tp == 24 && !LocaleNCompare(tp, "ColormapTypeNotSupported", 24))
                return *np ? tag : "Colormap type not supported";
              else
              if (p - tp == 29 && !LocaleNCompare(tp, "ColorspaceModelIsNotSupported", 29))
                return *np ? tag : "Colorspace model is not supported";
              else
              if (p - tp == 21 && !LocaleNCompare(tp, "ColorTypeNotSupported", 21))
                return *np ? tag : "Color type not supported";
              else
                return tag;

            case 'd':  case 'D':
              if (p - tp == 32 && !LocaleNCompare(tp, "DataEncodingSchemeIsNotSupported", 32))
                return *np ? tag : "Data encoding scheme is not supported";
              else
              if (p - tp == 29 && !LocaleNCompare(tp, "DataStorageTypeIsNotSupported", 29))
                return *np ? tag : "Data storage type is not supported";
              else
              if (p - tp == 20 && !LocaleNCompare(tp, "DeltaPNGNotSupported", 20))
                return *np ? tag : "Delta-PNG is not supported";
              else
                return tag;

            case 'e':  case 'E':
              if (p - tp == 33 && !LocaleNCompare(tp, "EncryptedWPGImageFileNotSupported", 33))
                return *np ? tag : "Encrypted WPG image file not supported";
              else
                return tag;

            case 'f':  case 'F':
              if (p - tp == 30 && !LocaleNCompare(tp, "FractalCompressionNotSupported", 30))
                return *np ? tag : "Fractal compression not supported";
              else
                return tag;

            case 'i':  case 'I':
              if (p - tp == 34 && !LocaleNCompare(tp, "ImageColumnOrRowSizeIsNotSupported", 34))
                return *np ? tag : "Image column or row size is not supported";
              else
              if (p - tp == 29 && !LocaleNCompare(tp, "ImageDoesNotHaveAMatteChannel", 29))
                return *np ? tag : "Image does not have a matte channel";
              else
              if (p - tp == 21 && !LocaleNCompare(tp, "ImageTypeNotSupported", 21))
                return *np ? tag : "Image type not supported";
              else
              if (p - tp == 24 && !LocaleNCompare(tp, "IncompatibleSizeOfDouble", 24))
                return *np ? tag : "Incompatible size of double";
              else
                return tag;

            case 'j':  case 'J':
              if (p - tp == 26 && !LocaleNCompare(tp, "JNGCompressionNotSupported", 26))
                return *np ? tag : "JNG compression not supported";
              else
              if (p - tp == 27 && !LocaleNCompare(tp, "JPEGCompressionNotSupported", 27))
                return *np ? tag : "JPEG compression not supported";
              else
              if (p - tp == 19 && !LocaleNCompare(tp, "JPEGEmbeddingFailed", 19))
                return *np ? tag : "JPEG embedding failed";
              else
                return tag;

            case 'l':  case 'L':
              if (p - tp == 26 && !LocaleNCompare(tp, "LocationTypeIsNotSupported", 26))
                return *np ? tag : "Location type is not supported";
              else
                return tag;

            case 'm':  case 'M':
              if (p - tp == 28 && !LocaleNCompare(tp, "MapStorageTypeIsNotSupported", 28))
                return *np ? tag : "Map storage type is not supported";
              else
              if (p - tp == 24 && !LocaleNCompare(tp, "MSBByteOrderNotSupported", 24))
                return *np ? tag : "MSB order not supported bitmap";
              else
              if (p - tp == 39 && !LocaleNCompare(tp, "MultidimensionalMatricesAreNotSupported", 39))
                return *np ? tag : "Multi-dimensional matrices are not supported";
              else
              if (p - tp == 30 && !LocaleNCompare(tp, "MultipleRecordListNotSupported", 30))
                return *np ? tag : "Multiple record list not supported";
              else
                return tag;

            case 'n':  case 'N':
              if (p - tp == 21 && !LocaleNCompare(tp, "No8BIMDataIsAvailable", 21))
                return *np ? tag : "No 8BIM data is available";
              else
              if (p - tp == 21 && !LocaleNCompare(tp, "NoAPP1DataIsAvailable", 21))
                return *np ? tag : "No APP1 data is available";
              else
              if (p - tp == 19 && !LocaleNCompare(tp, "NoBitmapOnClipboard", 19))
                return *np ? tag : "No bitmap on clipboard";
              else
              if (p - tp == 25 && !LocaleNCompare(tp, "NoColorProfileIsAvailable", 25))
                return *np ? tag : "No color profile is available";
              else
              if (p - tp == 14 && !LocaleNCompare(tp, "NoDataReturned", 14))
                return *np ? tag : "No data returned";
              else
              if (p - tp == 18 && !LocaleNCompare(tp, "NoIPTCInfoWasFound", 18))
                return *np ? tag : "No IPTC info was found";
              else
              if (p - tp == 22 && !LocaleNCompare(tp, "NoIPTCProfileAvailable", 22))
                return *np ? tag : "No IPTC profile available";
              else
              if (p - tp == 22 && !LocaleNCompare(tp, "NotASupportedImageFile", 22))
                return *np ? tag : "Not a support image file";
              else
              if (p - tp == 28 && !LocaleNCompare(tp, "NumberOfImagesIsNotSupported", 28))
                return *np ? tag : "Number of images is not supported";
              else
                return tag;

            case 'o':  case 'O':
              if (p - tp == 34 && !LocaleNCompare(tp, "OnlyContinuousTonePictureSupported", 34))
                return *np ? tag : "Only continuous tone picture supported";
              else
              if (p - tp == 27 && !LocaleNCompare(tp, "OnlyLevelZerofilesSupported", 27))
                return *np ? tag : "Only level zero files Supported";
              else
                return tag;

            case 'p':  case 'P':
              if (p - tp == 26 && !LocaleNCompare(tp, "PNGCompressionNotSupported", 26))
                return *np ? tag : "PNG compression not supported";
              else
                return tag;

            case 'r':  case 'R':
              if (p - tp == 26 && !LocaleNCompare(tp, "RLECompressionNotSupported", 26))
                return *np ? tag : "RLE compression not supported";
              else
                return tag;

            case 'u':  case 'U':
              if (p - tp == 19 && !LocaleNCompare(tp, "UnableToCopyProfile", 19))
                return *np ? tag : "Unable to copy profile";
              else
              if (p - tp == 17 && !LocaleNCompare(tp, "UnableToCreateADC", 17))
                return *np ? tag : "Unable to create a DC";
              else
              if (p - tp == 20 && !LocaleNCompare(tp, "UnableToCreateBitmap", 20))
                return *np ? tag : "Unable to create bitmap";
              else
              if (p - tp == 23 && !LocaleNCompare(tp, "UnableToDecompressImage", 23))
                return *np ? tag : "Unable to decompress image";
              else
              if (p - tp == 24 && !LocaleNCompare(tp, "UnableToReadCIELABImages", 24))
                return *np ? tag : "Unable to read CIELAB images";
              else
              if (p - tp == 21 && !LocaleNCompare(tp, "UnableToTranslateText", 21))
                return *np ? tag : "Unable to translate text";
              else
              if (p - tp == 27 && !LocaleNCompare(tp, "UnableToWriteMPEGParameters", 27))
                return *np ? tag : "Unable to write MPEG parameters";
              else
              if (p - tp == 24 && !LocaleNCompare(tp, "UnableToZipCompressImage", 24))
                return *np ? tag : "Unable to zip-compress image";
              else
              if (p - tp == 30 && !LocaleNCompare(tp, "UnsupportedCellTypeInTheMatrix", 30))
                return *np ? tag : "Unsupported cell type in the matrix";
              else
              if (p - tp == 16 && !LocaleNCompare(tp, "UnsupportedLevel", 16))
                return *np ? tag : "Unsupported level";
              else
              if (p - tp == 23 && !LocaleNCompare(tp, "UnsupportedRLEImagefile", 23))
                return *np ? tag : "Unsupported RLE image file";
              else
                return tag;

            case 'z':  case 'Z':
              if (p - tp == 26 && !LocaleNCompare(tp, "ZipCompressionNotSupported", 26))
                return *np ? tag : "Zip compression not supported";
              else
                return tag;
            }
          else
            return tag;

        case 'w':  case 'W':
          if (p - tp == 7 && !LocaleNCompare(tp, "Warning", 7))
            if (LocaleNCompare(NEXT_FIELD, "LosslessToLossyJPEGConversion", 29) || p - tp != 29)
              return tag;
            else
              return *np ? tag : "Lossless to lossy JPEG conversion";
          else
            return tag;
        }
      else
      if (p - tp == 9 && !LocaleNCompare(tp, "Configure", 9))
        if (LocaleNCompare(NEXT_FIELD, "Error", 5) || p - tp != 5)
          return tag;
        else
          switch (*NEXT_FIELD)
          {
          default:
            return tag;

          case 'i':  case 'I':
            if (p - tp == 29 && !LocaleNCompare(tp, "IncludeElementNestedTooDeeply", 29))
              return *np ? tag : "include element nested too deeply";
            else
              return tag;

          case 'u':  case 'U':
            if (p - tp == 27 && !LocaleNCompare(tp, "UnableToAccessConfigureFile", 27))
              return *np ? tag : "Unable to access configure file";
            else
            if (p - tp == 22 && !LocaleNCompare(tp, "UnableToAccessFontFile", 22))
              return *np ? tag : "Unable to access font file";
            else
            if (p - tp == 24 && !LocaleNCompare(tp, "UnableToAccessModuleFile", 24))
              return *np ? tag : "Unable to access module file";
            else
              return tag;
          }
      else
      if (p - tp == 7 && !LocaleNCompare(tp, "Corrupt", 7))
        if (LocaleNCompare(NEXT_FIELD, "Image", 5) || p - tp != 5)
          return tag;
        else
          switch (*NEXT_FIELD)
          {
          default:
            return tag;

          case 'e':  case 'E':
            if (p - tp == 5 && !LocaleNCompare(tp, "Error", 5))
              switch (*NEXT_FIELD)
              {
              default:
                return tag;

              case 'a':  case 'A':
                if (p - tp == 33 && !LocaleNCompare(tp, "AnErrorHasOccurredReadingFromFile", 33))
                  return *np ? tag : "An error has occurred reading from file";
                else
                if (p - tp == 31 && !LocaleNCompare(tp, "AnErrorHasOccurredWritingToFile", 31))
                  return *np ? tag : "An error has occurred writing to file";
                else
                  return tag;

              case 'c':  case 'C':
                if (p - tp == 24 && !LocaleNCompare(tp, "ColormapExceeds256Colors", 24))
                  return *np ? tag : "Colormap exceeded 256 colors";
                else
                if (p - tp == 19 && !LocaleNCompare(tp, "CompressionNotValid", 19))
                  return *np ? tag : "Compression not valid";
                else
                if (p - tp == 15 && !LocaleNCompare(tp, "CorruptGIFImage", 15))
                  return *np ? tag : "Corrupt GIF image";
                else
                if (p - tp == 18 && !LocaleNCompare(tp, "CorruptInlineImage", 18))
                  return *np ? tag : "Corrupt inline image";
                else
                if (p - tp == 15 && !LocaleNCompare(tp, "CorruptMNGImage", 15))
                  return *np ? tag : "Corrupt MNG image";
                else
                if (p - tp == 15 && !LocaleNCompare(tp, "CorruptOTBImage", 15))
                  return *np ? tag : "Corrupt OTB image";
                else
                if (p - tp == 15 && !LocaleNCompare(tp, "CorruptPCDImage", 15))
                  return *np ? tag : "Corrupt PCD image";
                else
                if (p - tp == 15 && !LocaleNCompare(tp, "CorruptPDBImage", 15))
                  return *np ? tag : "Corrupt PDB image";
                else
                if (p - tp == 16 && !LocaleNCompare(tp, "CorruptWBMPImage", 16))
                  return *np ? tag : "Corrupt WBMP image";
                else
                if (p - tp == 15 && !LocaleNCompare(tp, "CorruptXCFImage", 15))
                  return *np ? tag : "Corrupt XCF image";
                else
                if (p - tp == 15 && !LocaleNCompare(tp, "CorruptXPMImage", 15))
                  return *np ? tag : "Corrupt XPM image";
                else
                if (p - tp == 15 && !LocaleNCompare(tp, "CorruptXWDImage", 15))
                  return *np ? tag : "Corrupt XWD image";
                else
                  return tag;

              case 'f':  case 'F':
                if (p - tp == 25 && !LocaleNCompare(tp, "FileFormatVersionMismatch", 25))
                  return *np ? tag : "file format version mismatch";
                else
                  return tag;

              case 'i':  case 'I':
                if (p - tp == 35 && !LocaleNCompare(tp, "ImageFileDoesNotContainAnyImageData", 35))
                  return *np ? tag : "Image file or does not contain any image data";
                else
                if (p - tp == 19 && !LocaleNCompare(tp, "ImproperImageHeader", 19))
                  return *np ? tag : "Improper image header";
                else
                if (p - tp == 27 && !LocaleNCompare(tp, "InsufficientImageDataInFile", 27))
                  return *np ? tag : "Insufficient image data in file";
                else
                if (p - tp == 19 && !LocaleNCompare(tp, "InvalidBitsPerPixel", 19))
                  return *np ? tag : "Invalid bits per pixel";
                else
                if (p - tp == 20 && !LocaleNCompare(tp, "InvalidColormapIndex", 20))
                  return *np ? tag : "Invalid colormap index";
                else
                if (p - tp == 18 && !LocaleNCompare(tp, "InvalidCompression", 18))
                  return *np ? tag : "Invalid compression";
                else
                if (p - tp == 20 && !LocaleNCompare(tp, "InvalidNumerOfColors", 20))
                  return *np ? tag : "Invalid number of colors";
                else
                  return tag;

              case 'l':  case 'L':
                if (p - tp == 27 && !LocaleNCompare(tp, "LengthAndFilesizeDoNotMatch", 27))
                  return *np ? tag : "Length and filesize do not match";
                else
                  return tag;

              case 'n':  case 'N':
                if (p - tp == 23 && !LocaleNCompare(tp, "NegativeOrZeroImageSize", 23))
                  return *np ? tag : "Negative or zero image size";
                else
                if (p - tp == 21 && !LocaleNCompare(tp, "NonOS2HeaderSizeError", 21))
                  return *np ? tag : "Non OS2 BMP header size less than 40";
                else
                if (p - tp == 16 && !LocaleNCompare(tp, "NotABMPImageFile", 16))
                  return *np ? tag : "Not a BMP image file";
                else
                if (p - tp == 16 && !LocaleNCompare(tp, "NotACUTImageFile", 16))
                  return *np ? tag : "Not a CUT image file";
                else
                if (p - tp == 16 && !LocaleNCompare(tp, "NotADCMImageFile", 16))
                  return *np ? tag : "Not a DCM image file";
                else
                if (p - tp == 16 && !LocaleNCompare(tp, "NotADCXImageFile", 16))
                  return *np ? tag : "Not a DCX image file";
                else
                if (p - tp == 16 && !LocaleNCompare(tp, "NotADIBImageFile", 16))
                  return *np ? tag : "Not a DIB image file";
                else
                if (p - tp == 16 && !LocaleNCompare(tp, "NotADPXImageFile", 16))
                  return *np ? tag : "Not a DPX image file";
                else
                if (p - tp == 16 && !LocaleNCompare(tp, "NotAGIFImageFile", 16))
                  return *np ? tag : "Not a GIF image file";
                else
                if (p - tp == 19 && !LocaleNCompare(tp, "NotAMATLABImageFile", 19))
                  return *np ? tag : "Not a MATLAB image file";
                else
                if (p - tp == 16 && !LocaleNCompare(tp, "NotAMNGImageFile", 16))
                  return *np ? tag : "Not a MNG image file";
                else
                if (p - tp == 16 && !LocaleNCompare(tp, "NotAMTVImageFile", 16))
                  return *np ? tag : "Not a MTV image file";
                else
                if (p - tp == 17 && !LocaleNCompare(tp, "NotAnARTImageFile", 17))
                  return *np ? tag : "Not an ART image file";
                else
                if (p - tp == 17 && !LocaleNCompare(tp, "NotAnAVIImageFile", 17))
                  return *np ? tag : "Not an AVI image file";
                else
                if (p - tp == 17 && !LocaleNCompare(tp, "NotAnAVSImageFile", 17))
                  return *np ? tag : "Not an AVS image file";
                else
                if (p - tp == 17 && !LocaleNCompare(tp, "NotAnEMFImageFile", 17))
                  return *np ? tag : "Not an EMF image file";
                else
                if (p - tp == 17 && !LocaleNCompare(tp, "NotAnICOImageFile", 17))
                  return *np ? tag : "Not a ICO image file";
                else
                if (p - tp == 16 && !LocaleNCompare(tp, "NotAOTBImageFile", 16))
                  return *np ? tag : "Not a OTB image file";
                else
                if (p - tp == 16 && !LocaleNCompare(tp, "NotAPCDImageFile", 16))
                  return *np ? tag : "Not a PCD image file";
                else
                if (p - tp == 16 && !LocaleNCompare(tp, "NotAPCXImageFile", 16))
                  return *np ? tag : "Not a PCX image file";
                else
                if (p - tp == 17 && !LocaleNCompare(tp, "NotAPICTImageFile", 17))
                  return *np ? tag : "Not a PICT image file";
                else
                if (p - tp == 16 && !LocaleNCompare(tp, "NotAPIXImageFile", 16))
                  return *np ? tag : "Not a PIX image file";
                else
                if (p - tp == 16 && !LocaleNCompare(tp, "NotAPNMImageFile", 16))
                  return *np ? tag : "Not a PNM image file";
                else
                if (p - tp == 16 && !LocaleNCompare(tp, "NotAPSDImageFile", 16))
                  return *np ? tag : "Not a PSD image file";
                else
                if (p - tp == 16 && !LocaleNCompare(tp, "NotAPWPImageFile", 16))
                  return *np ? tag : "Not a PSP image file";
                else
                if (p - tp == 16 && !LocaleNCompare(tp, "NotARLEImageFile", 16))
                  return *np ? tag : "Not a RLE image file";
                else
                if (p - tp == 16 && !LocaleNCompare(tp, "NotASCTImageFile", 16))
                  return *np ? tag : "Not a SCT image file";
                else
                if (p - tp == 16 && !LocaleNCompare(tp, "NotASFWImageFile", 16))
                  return *np ? tag : "Not a SFW image file";
                else
                if (p - tp == 16 && !LocaleNCompare(tp, "NotASGIImageFile", 16))
                  return *np ? tag : "Not a SGI image file";
                else
                if (p - tp == 16 && !LocaleNCompare(tp, "NotASUNImageFile", 16))
                  return *np ? tag : "Not a SUN image file";
                else
                if (p - tp == 16 && !LocaleNCompare(tp, "NotATGAImageFile", 16))
                  return *np ? tag : "Not a TGA image file";
                else
                if (p - tp == 16 && !LocaleNCompare(tp, "NotATIMImageFile", 16))
                  return *np ? tag : "Not a TIM image file";
                else
                if (p - tp == 17 && !LocaleNCompare(tp, "NotAVIFFImageFile", 17))
                  return *np ? tag : "Not a VIFF image file";
                else
                if (p - tp == 17 && !LocaleNCompare(tp, "NotAWBMPImageFile", 17))
                  return *np ? tag : "Not a WBMP image file";
                else
                if (p - tp == 16 && !LocaleNCompare(tp, "NotAWPGImageFile", 16))
                  return *np ? tag : "Not a WPG image file";
                else
                if (p - tp == 16 && !LocaleNCompare(tp, "NotAXBMImageFile", 16))
                  return *np ? tag : "Not a XBM image file";
                else
                if (p - tp == 16 && !LocaleNCompare(tp, "NotAXCFImageFile", 16))
                  return *np ? tag : "Not a XCF image file";
                else
                if (p - tp == 16 && !LocaleNCompare(tp, "NotAXPMImageFile", 16))
                  return *np ? tag : "Not a XPM image file";
                else
                if (p - tp == 18 && !LocaleNCompare(tp, "NotEnoughPixelData", 18))
                  return *np ? tag : "Not enough pixel data";
                else
                if (p - tp == 14 && !LocaleNCompare(tp, "NotEnoughTiles", 14))
                  return *np ? tag : "Not enough tiles found in level";
                else
                  return tag;

              case 's':  case 'S':
                if (p - tp == 30 && !LocaleNCompare(tp, "StaticPlanesValueNotEqualToOne", 30))
                  return *np ? tag : "Static planes value not equal to 1";
                else
                  return tag;

              case 't':  case 'T':
                if (p - tp == 22 && !LocaleNCompare(tp, "TooMuchImageDataInFile", 22))
                  return *np ? tag : "Too much image data in file";
                else
                  return tag;

              case 'u':  case 'U':
                if (p - tp == 24 && !LocaleNCompare(tp, "UnableToReadColorProfile", 24))
                  return *np ? tag : "Unable to read color profile";
                else
                if (p - tp == 26 && !LocaleNCompare(tp, "UnableToReadExtensionBlock", 26))
                  return *np ? tag : "Unable to read extension block";
                else
                if (p - tp == 26 && !LocaleNCompare(tp, "UnableToReadGenericProfile", 26))
                  return *np ? tag : "Unable to read generic profile";
                else
                if (p - tp == 21 && !LocaleNCompare(tp, "UnableToReadImageData", 21))
                  return *np ? tag : "Unable to read image data";
                else
                if (p - tp == 23 && !LocaleNCompare(tp, "UnableToReadImageHeader", 23))
                  return *np ? tag : "Unable to read image header";
                else
                if (p - tp == 23 && !LocaleNCompare(tp, "UnableToReadIPTCProfile", 23))
                  return *np ? tag : "Unable to read IPTC profile";
                else
                if (p - tp == 20 && !LocaleNCompare(tp, "UnableToReadVIDImage", 20))
                  return *np ? tag : "Unable to read VID image";
                else
                if (p - tp == 28 && !LocaleNCompare(tp, "UnableToRunlengthDecodeImage", 28))
                  return *np ? tag : "Unable to runlength decode image";
                else
                if (p - tp == 23 && !LocaleNCompare(tp, "UnableToUncompressImage", 23))
                  return *np ? tag : "Unable to uncompress image";
                else
                if (p - tp == 19 && !LocaleNCompare(tp, "UnexpectedEndOfFile", 19))
                  return *np ? tag : "Unexpected end-of-file";
                else
                if (p - tp == 24 && !LocaleNCompare(tp, "UnexpectedSamplingFactor", 24))
                  return *np ? tag : "Unexpected sampling factor";
                else
                if (p - tp == 18 && !LocaleNCompare(tp, "UnknownPatternType", 18))
                  return *np ? tag : "Unknown pattern type";
                else
                if (p - tp == 29 && !LocaleNCompare(tp, "UnrecognizedCompressionMethod", 29))
                  return *np ? tag : "Unrecognized compression method";
                else
                  return tag;
              }
            else
              return tag;

          case 'f':  case 'F':
            if (p - tp == 10 && !LocaleNCompare(tp, "FatalError", 10))
              if (LocaleNCompare(NEXT_FIELD, "UnableToPersistKey", 18) || p - tp != 18)
                return tag;
              else
                return *np ? tag : "Unable to persist key";
            else
              return tag;

          case 'w':  case 'W':
            if (p - tp == 7 && !LocaleNCompare(tp, "Warning", 7))
              if (LocaleNCompare(NEXT_FIELD, "SkipToSyncByte", 14) || p - tp != 14)
                return tag;
              else
                return *np ? tag : "Corrupt PCD image, skipping to sync byte";
            else
              return tag;
          }
      else
        return tag;

    case 'd':  case 'D':
      if (p - tp == 8 && !LocaleNCompare(tp, "Delegate", 8))
        if (LocaleNCompare(NEXT_FIELD, "Error", 5) || p - tp != 5)
          return tag;
        else
          switch (*NEXT_FIELD)
          {
          default:
            return tag;

          case 'd':  case 'D':
            if (p - tp == 14 && !LocaleNCompare(tp, "DelegateFailed", 14))
              return *np ? tag : "Delegate failed";
            else
              return tag;

          case 'f':  case 'F':
            if (p - tp == 25 && !LocaleNCompare(tp, "FailedToComputeOutputSize", 25))
              return *np ? tag : "Failed to compute output size";
            else
            if (p - tp == 18 && !LocaleNCompare(tp, "FailedToRenderFile", 18))
              return *np ? tag : "Failed to render file";
            else
            if (p - tp == 16 && !LocaleNCompare(tp, "FailedToScanFile", 16))
              return *np ? tag : "Failed to scan file";
            else
              return tag;

          case 'n':  case 'N':
            if (p - tp == 10 && !LocaleNCompare(tp, "NoTagFound", 10))
              return *np ? tag : "No tag found";
            else
              return tag;

          case 'p':  case 'P':
            if (p - tp == 24 && !LocaleNCompare(tp, "PostscriptDelegateFailed", 24))
              return *np ? tag : "Postscript delegate failed";
            else
              return tag;

          case 'u':  case 'U':
            if (p - tp == 19 && !LocaleNCompare(tp, "UnableToCreateImage", 19))
              return *np ? tag : "Unable to create image";
            else
            if (p - tp == 23 && !LocaleNCompare(tp, "UnableToDecodeImageFile", 23))
              return *np ? tag : "Unable to decode image file";
            else
            if (p - tp == 23 && !LocaleNCompare(tp, "UnableToEncodeImageFile", 23))
              return *np ? tag : "Unable to encode image file";
            else
            if (p - tp == 28 && !LocaleNCompare(tp, "UnableToInitializeFPXLibrary", 28))
              return *np ? tag : "Unable to initialize FPX library";
            else
            if (p - tp == 28 && !LocaleNCompare(tp, "UnableToInitializeWMFLibrary", 28))
              return *np ? tag : "Unable to initialize WMF library";
            else
            if (p - tp == 23 && !LocaleNCompare(tp, "UnableToManageJP2Stream", 23))
              return *np ? tag : "Unable to manage JP2 stream";
            else
            if (p - tp == 23 && !LocaleNCompare(tp, "UnableToReadAspectRatio", 23))
              return *np ? tag : "Unable to read aspect ratio";
            else
            if (p - tp == 23 && !LocaleNCompare(tp, "UnableToReadSummaryInfo", 23))
              return *np ? tag : "Unable to read summary info";
            else
            if (p - tp == 23 && !LocaleNCompare(tp, "UnableToSetAffineMatrix", 23))
              return *np ? tag : "Unable to set affine matrix";
            else
            if (p - tp == 22 && !LocaleNCompare(tp, "UnableToSetAspectRatio", 22))
              return *np ? tag : "Unable to set aspect ratio";
            else
            if (p - tp == 21 && !LocaleNCompare(tp, "UnableToSetColorTwist", 21))
              return *np ? tag : "Unable to set color twist";
            else
            if (p - tp == 19 && !LocaleNCompare(tp, "UnableToSetContrast", 19))
              return *np ? tag : "Unable to set contrast";
            else
            if (p - tp == 25 && !LocaleNCompare(tp, "UnableToSetFilteringValue", 25))
              return *np ? tag : "Unable to set filtering value";
            else
            if (p - tp == 23 && !LocaleNCompare(tp, "UnableToSetImageComment", 23))
              return *np ? tag : "Unable to set image comment";
            else
            if (p - tp == 21 && !LocaleNCompare(tp, "UnableToSetImageTitle", 21))
              return *np ? tag : "Unable to set image title";
            else
            if (p - tp == 20 && !LocaleNCompare(tp, "UnableToSetJPEGLevel", 20))
              return *np ? tag : "Unable to set JPEG level";
            else
            if (p - tp == 27 && !LocaleNCompare(tp, "UnableToSetRegionOfInterest", 27))
              return *np ? tag : "Unable to set region of interest";
            else
            if (p - tp == 22 && !LocaleNCompare(tp, "UnableToSetSummaryInfo", 22))
              return *np ? tag : "Unable to set summary info";
            else
            if (p - tp == 22 && !LocaleNCompare(tp, "UnableToWriteSVGFormat", 22))
              return *np ? tag : "Unable to write SVG format";
            else
              return tag;
          }
      else
      if (p - tp == 4 && !LocaleNCompare(tp, "Draw", 4))
        if (LocaleNCompare(NEXT_FIELD, "Error", 5) || p - tp != 5)
          return tag;
        else
          switch (*NEXT_FIELD)
          {
          default:
            return tag;

          case 'a':  case 'A':
            if (p - tp == 31 && !LocaleNCompare(tp, "AlreadyPushingPatternDefinition", 31))
              return *np ? tag : "Already pushing pattern definition";
            else
              return tag;

          case 'n':  case 'N':
            if (p - tp == 39 && !LocaleNCompare(tp, "NonconformingDrawingPrimitiveDefinition", 39))
              return *np ? tag : "Non-conforming drawing primitive definition";
            else
            if (p - tp == 15 && !LocaleNCompare(tp, "NotARelativeURL", 15))
              return *np ? tag : "Not a relative URL";
            else
            if (p - tp == 36 && !LocaleNCompare(tp, "NotCurrentlyPushingPatternDefinition", 36))
              return *np ? tag : "Not currently pushing pattern definition";
            else
              return tag;

          case 'u':  case 'U':
            if (p - tp == 13 && !LocaleNCompare(tp, "UnableToPrint", 13))
              return *np ? tag : "Unable to print";
            else
            if (p - tp == 31 && !LocaleNCompare(tp, "UnbalancedGraphicContextPushPop", 31))
              return *np ? tag : "unbalanced graphic context push-pop";
            else
            if (p - tp == 11 && !LocaleNCompare(tp, "URLNotFound", 11))
              return *np ? tag : "URL not found";
            else
              return tag;
          }
      else
        return tag;

    case 'f':  case 'F':
      if (p - tp == 4 && !LocaleNCompare(tp, "File", 4))
        if (LocaleNCompare(NEXT_FIELD, "Open", 4) || p - tp != 4)
          return tag;
        else
        if (LocaleNCompare(NEXT_FIELD, "Error", 5) || p - tp != 5)
          return tag;
        else
          switch (*NEXT_FIELD)
          {
          default:
            return tag;

          case 'u':  case 'U':
            if (p - tp == 27 && !LocaleNCompare(tp, "UnableToCreateTemporaryFile", 27))
              return *np ? tag : "Unable to temporary file";
            else
            if (p - tp == 16 && !LocaleNCompare(tp, "UnableToOpenFile", 16))
              return *np ? tag : "Unable to open file";
            else
            if (p - tp == 17 && !LocaleNCompare(tp, "UnableToWriteFile", 17))
              return *np ? tag : "Unable to write file";
            else
              return tag;
          }
      else
        return tag;

    case 'i':  case 'I':
      if (p - tp == 5 && !LocaleNCompare(tp, "Image", 5))
        if (LocaleNCompare(NEXT_FIELD, "Error", 5) || p - tp != 5)
          return tag;
        else
          switch (*NEXT_FIELD)
          {
          default:
            return tag;

          case 'a':  case 'A':
            if (p - tp == 21 && !LocaleNCompare(tp, "AngleIs discontinuous", 21))
              return *np ? tag : "angle is discontinuous";
            else
              return tag;

          case 'c':  case 'C':
            if (p - tp == 27 && !LocaleNCompare(tp, "ColorSeparatedImageRequired", 27))
              return *np ? tag : "Color separated image required";
            else
              return tag;

          case 'i':  case 'I':
            if (p - tp == 22 && !LocaleNCompare(tp, "ImageColorspaceDiffers", 22))
              return *np ? tag : "image colorspace differs";
            else
            if (p - tp == 19 && !LocaleNCompare(tp, "ImageOpacityDiffers", 19))
              return *np ? tag : "image opacity differs";
            else
            if (p - tp == 23 && !LocaleNCompare(tp, "ImageSequenceIsRequired", 23))
              return *np ? tag : "Image sequence is required";
            else
            if (p - tp == 16 && !LocaleNCompare(tp, "ImageSizeDiffers", 16))
              return *np ? tag : "image size differs";
            else
            if (p - tp == 27 && !LocaleNCompare(tp, "InputImagesAlreadySpecified", 27))
              return *np ? tag : "Input images already specified";
            else
              return tag;

          case 'l':  case 'L':
            if (p - tp == 28 && !LocaleNCompare(tp, "LeftAndRightImageSizesDiffer", 28))
              return *np ? tag : "left and right image sizes differ";
            else
              return tag;

          case 'n':  case 'N':
            if (p - tp == 17 && !LocaleNCompare(tp, "NoImagesWereFound", 17))
              return *np ? tag : "No images were found";
            else
            if (p - tp == 18 && !LocaleNCompare(tp, "NoImagesWereLoaded", 18))
              return *np ? tag : "No images were loaded";
            else
            if (p - tp == 22 && !LocaleNCompare(tp, "NoLocaleImageAttribute", 22))
              return *np ? tag : "No [LOCALE] image attribute";
            else
              return tag;

          case 't':  case 'T':
            if (p - tp == 15 && !LocaleNCompare(tp, "TooManyClusters", 15))
              return *np ? tag : "too many cluster";
            else
              return tag;

          case 'u':  case 'U':
            if (p - tp == 19 && !LocaleNCompare(tp, "UnableToAppendImage", 19))
              return *np ? tag : "unable to append image";
            else
            if (p - tp == 20 && !LocaleNCompare(tp, "UnableToAverageImage", 20))
              return *np ? tag : "unable to average image";
            else
            if (p - tp == 21 && !LocaleNCompare(tp, "UnableToCompareImages", 21))
              return *np ? tag : "Unable to compare images";
            else
            if (p - tp == 25 && !LocaleNCompare(tp, "UnableToCreateImageMosaic", 25))
              return *np ? tag : "unable to create image mosaic";
            else
            if (p - tp == 25 && !LocaleNCompare(tp, "UnableToCreateStereoImage", 25))
              return *np ? tag : "Unable to create stereo image";
            else
            if (p - tp == 32 && !LocaleNCompare(tp, "UnableToDeconstructImageSequence", 32))
              return *np ? tag : "Unable to deconstruct image sequence";
            else
            if (p - tp == 20 && !LocaleNCompare(tp, "UnableToFlattenImage", 20))
              return *np ? tag : "Unable to flatten image";
            else
            if (p - tp == 19 && !LocaleNCompare(tp, "UnableToResizeImage", 19))
              return *np ? tag : "Unable to resize image";
            else
            if (p - tp == 20 && !LocaleNCompare(tp, "UnableToSegmentImage", 20))
              return *np ? tag : "Unable to segment image";
            else
            if (p - tp == 24 && !LocaleNCompare(tp, "UnableToSetImageClipMask", 24))
              return *np ? tag : "Unable to set image clip mask";
            else
              return tag;

          case 'w':  case 'W':
            if (p - tp == 25 && !LocaleNCompare(tp, "WidthOrHeightExceedsLimit", 25))
              return *np ? tag : "Width or height exceeds limit";
            else
              return tag;
          }
      else
      if (p - tp == 19 && !LocaleNCompare(tp, "InvalidColormapType", 19))
        return *np ? tag : "Invalid colormap type";
      else
      if (p - tp == 21 && !LocaleNCompare(tp, "InvalidColorspaceType", 21))
        return *np ? tag : "Invalid colorspace type";
      else
      if (p - tp == 17 && !LocaleNCompare(tp, "InvalidEndianType", 17))
        return *np ? tag : "Invalid endian type";
      else
      if (p - tp == 16 && !LocaleNCompare(tp, "InvalidImageType", 16))
        return *np ? tag : "Invalid image type";
      else
      if (p - tp == 20 && !LocaleNCompare(tp, "InvalidInterlaceType", 20))
        return *np ? tag : "Invalid interlace type";
      else
      if (p - tp == 25 && !LocaleNCompare(tp, "InvalidVirtualPixelMethod", 25))
        return *np ? tag : "Invalid virtual pixel method";
      else
        return tag;

    case 'm':  case 'M':
      if (p - tp == 7 && !LocaleNCompare(tp, "Missing", 7))
        if (LocaleNCompare(NEXT_FIELD, "Delegate", 8) || p - tp != 8)
          return tag;
        else
        if (LocaleNCompare(NEXT_FIELD, "Error", 5) || p - tp != 5)
          return tag;
        else
          switch (*NEXT_FIELD)
          {
          default:
            return tag;

          case 'd':  case 'D':
            if (p - tp == 24 && !LocaleNCompare(tp, "DPSLibraryIsNotAvailable", 24))
              return *np ? tag : "DPS library is not available";
            else
              return tag;

          case 'f':  case 'F':
            if (p - tp == 24 && !LocaleNCompare(tp, "FPXLibraryIsNotAvailable", 24))
              return *np ? tag : "FPX library is not available";
            else
            if (p - tp == 29 && !LocaleNCompare(tp, "FreeTypeLibraryIsNotAvailable", 29))
              return *np ? tag : "FreeType library is not available";
            else
              return tag;

          case 'j':  case 'J':
            if (p - tp == 25 && !LocaleNCompare(tp, "JPEGLibraryIsNotAvailable", 25))
              return *np ? tag : "JPEG compression library is not available";
            else
              return tag;

          case 'l':  case 'L':
            if (p - tp == 21 && !LocaleNCompare(tp, "LZWEncodingNotEnabled", 21))
              return *np ? tag : "LZW encoding not enabled";
            else
              return tag;

          case 'n':  case 'N':
            if (p - tp == 34 && !LocaleNCompare(tp, "NoDecodeDelegateForThisImageFormat", 34))
              return *np ? tag : "No decode delegate for this image format";
            else
            if (p - tp == 34 && !LocaleNCompare(tp, "NoEncodeDelegateForThisImageFormat", 34))
              return *np ? tag : "No encode delegate for this image format";
            else
              return tag;

          case 't':  case 'T':
            if (p - tp == 25 && !LocaleNCompare(tp, "TIFFLibraryIsNotAvailable", 25))
              return *np ? tag : "TIFF library is not available";
            else
              return tag;

          case 'x':  case 'X':
            if (p - tp == 24 && !LocaleNCompare(tp, "XMLLibraryIsNotAvailable", 24))
              return *np ? tag : "XML library is not available";
            else
            if (p - tp == 28 && !LocaleNCompare(tp, "XWindowLibraryIsNotAvailable", 28))
              return *np ? tag : "X Window library is not available";
            else
              return tag;

          case 'z':  case 'Z':
            if (p - tp == 24 && !LocaleNCompare(tp, "ZipLibraryIsNotAvailable", 24))
              return *np ? tag : "ZLIB compression library is not available";
            else
              return tag;
          }
      else
      if (p - tp == 22 && !LocaleNCompare(tp, "MissingAnImageFilename", 22))
        return *np ? tag : "Missing an image filename";
      else
      if (p - tp == 12 && !LocaleNCompare(tp, "MissingColor", 12))
        return *np ? tag : "Missing color";
      else
      if (p - tp == 14 && !LocaleNCompare(tp, "MissingCommand", 14))
        return *np ? tag : "Missing command";
      else
      if (p - tp == 14 && !LocaleNCompare(tp, "MissingDegrees", 14))
        return *np ? tag : "Missing degrees";
      else
      if (p - tp == 12 && !LocaleNCompare(tp, "MissingDepth", 12))
        return *np ? tag : "Missing depth";
      else
      if (p - tp == 16 && !LocaleNCompare(tp, "MissingEventMask", 16))
        return *np ? tag : "Missing event mask";
      else
      if (p - tp == 15 && !LocaleNCompare(tp, "MissingFontName", 15))
        return *np ? tag : "Missing font name";
      else
      if (p - tp == 17 && !LocaleNCompare(tp, "MissingForeground", 17))
        return *np ? tag : "Missing foreground";
      else
      if (p - tp == 15 && !LocaleNCompare(tp, "MissingGeometry", 15))
        return *np ? tag : "Missing geometry";
      else
      if (p - tp == 19 && !LocaleNCompare(tp, "MissingIDNameOrRoot", 19))
        return *np ? tag : "Missing id name or 'root";
      else
      if (p - tp == 17 && !LocaleNCompare(tp, "MissingImageDepth", 17))
        return *np ? tag : "Missing image depth";
      else
      if (p - tp == 14 && !LocaleNCompare(tp, "MissingMapType", 14))
        return *np ? tag : "Missing map type";
      else
      if (p - tp == 11 && !LocaleNCompare(tp, "MissingName", 11))
        return *np ? tag : "Missing name";
      else
      if (p - tp == 15 && !LocaleNCompare(tp, "MissingPassword", 15))
        return *np ? tag : "Missing password";
      else
      if (p - tp == 14 && !LocaleNCompare(tp, "MissingQuality", 14))
        return *np ? tag : "Missing quality";
      else
      if (p - tp == 20 && !LocaleNCompare(tp, "MissingResourceLimit", 20))
        return *np ? tag : "Missing resource limit";
      else
      if (p - tp == 19 && !LocaleNCompare(tp, "MissingResourceType", 19))
        return *np ? tag : "Missing resource limit";
      else
      if (p - tp == 18 && !LocaleNCompare(tp, "MissingSceneNumber", 18))
        return *np ? tag : "Missing scene number";
      else
      if (p - tp == 14 && !LocaleNCompare(tp, "MissingSeconds", 14))
        return *np ? tag : "Missing seconds";
      else
      if (p - tp == 17 && !LocaleNCompare(tp, "MissingServerName", 17))
        return *np ? tag : "Missing server name";
      else
      if (p - tp == 11 && !LocaleNCompare(tp, "MissingSize", 11))
        return *np ? tag : "Missing size";
      else
      if (p - tp == 12 && !LocaleNCompare(tp, "MissingSnaps", 12))
        return *np ? tag : "Missing snaps";
      else
      if (p - tp == 16 && !LocaleNCompare(tp, "MissingThreshold", 16))
        return *np ? tag : "Missing threshold";
      else
      if (p - tp == 12 && !LocaleNCompare(tp, "MissingTitle", 12))
        return *np ? tag : "Missing title";
      else
      if (p - tp == 11 && !LocaleNCompare(tp, "MissingType", 11))
        return *np ? tag : "Missing type";
      else
      if (p - tp == 25 && !LocaleNCompare(tp, "MissingVirtualPixelMethod", 25))
        return *np ? tag : "Missing virtual pixel method";
      else
      if (p - tp == 18 && !LocaleNCompare(tp, "MissingVisualClass", 18))
        return *np ? tag : "Missing visual class";
      else
      if (p - tp == 12 && !LocaleNCompare(tp, "MissingWidth", 12))
        return *np ? tag : "Missing width";
      else
      if (p - tp == 6 && !LocaleNCompare(tp, "Module", 6))
        switch (*NEXT_FIELD)
        {
        default:
          return tag;

        case 'e':  case 'E':
          if (p - tp == 5 && !LocaleNCompare(tp, "Error", 5))
            switch (*NEXT_FIELD)
            {
            default:
              return tag;

            case 'u':  case 'U':
              if (p - tp == 18 && !LocaleNCompare(tp, "UnableToLoadModule", 18))
                return *np ? tag : "Unable to load module";
              else
              if (p - tp == 27 && !LocaleNCompare(tp, "UnableToRegisterImageFormat", 27))
                return *np ? tag : "Unable to register image format";
              else
              if (p - tp == 18 && !LocaleNCompare(tp, "UnrecognizedModule", 18))
                return *np ? tag : "Unrecognized module";
              else
                return tag;
            }
          else
            return tag;

        case 'f':  case 'F':
          if (p - tp == 10 && !LocaleNCompare(tp, "FatalError", 10))
            if (LocaleNCompare(NEXT_FIELD, "UnableToInitializeModuleLoader", 30) || p - tp != 30)
              return tag;
            else
              return *np ? tag : "Unable to initialize module loader";
          else
            return tag;
        }
      else
        return tag;

    case 'o':  case 'O':
      if (p - tp == 6 && !LocaleNCompare(tp, "Option", 6))
        if (LocaleNCompare(NEXT_FIELD, "Error", 5) || p - tp != 5)
          return tag;
        else
          switch (*NEXT_FIELD)
          {
          default:
            return tag;

          case 'b':  case 'B':
            if (p - tp == 20 && !LocaleNCompare(tp, "BevelWidthIsNegative", 20))
              return *np ? tag : "bevel width is negative";
            else
              return tag;

          case 'f':  case 'F':
            if (p - tp == 24 && !LocaleNCompare(tp, "FrameIsLessThanImageSize", 24))
              return *np ? tag : "frame is less than image size";
            else
              return tag;

          case 'g':  case 'G':
            if (p - tp == 25 && !LocaleNCompare(tp, "GeometryDimensionsAreZero", 25))
              return *np ? tag : "Geometry dimensions are zero";
            else
            if (p - tp == 27 && !LocaleNCompare(tp, "GeometryDoesNotContainImage", 27))
              return *np ? tag : "Geometry does not contain image";
            else
              return tag;

          case 'i':  case 'I':
            if (p - tp == 23 && !LocaleNCompare(tp, "ImagesAreNotTheSameSize", 23))
              return *np ? tag : "Images are not the same size";
            else
            if (p - tp == 29 && !LocaleNCompare(tp, "ImageSizeMustExceedBevelWidth", 29))
              return *np ? tag : "size must exceed bevel width";
            else
            if (p - tp == 27 && !LocaleNCompare(tp, "ImageSmallerThanKernelWidth", 27))
              return *np ? tag : "image smaller than kernel width";
            else
            if (p - tp == 22 && !LocaleNCompare(tp, "ImageSmallerThanRadius", 22))
              return *np ? tag : "image smaller than radius";
            else
            if (p - tp == 26 && !LocaleNCompare(tp, "ImageWidthsOrHeightsDiffer", 26))
              return *np ? tag : "image widths or heights differ";
            else
            if (p - tp == 17 && !LocaleNCompare(tp, "InvalidColorspace", 17))
              return *np ? tag : "Invalid image colorspace";
            else
            if (p - tp == 22 && !LocaleNCompare(tp, "InvalidComposeOperator", 22))
              return *np ? tag : "Invalid compose operator";
            else
            if (p - tp == 18 && !LocaleNCompare(tp, "InvalidCompression", 18))
              return *np ? tag : "Invalid image compression";
            else
            if (p - tp == 15 && !LocaleNCompare(tp, "InvalidPixelMap", 15))
              return *np ? tag : "Invalid pixel map";
            else
              return tag;

          case 'k':  case 'K':
            if (p - tp == 22 && !LocaleNCompare(tp, "KernelRadiusIsTooSmall", 22))
              return *np ? tag : "kernel radius is too small";
            else
            if (p - tp == 28 && !LocaleNCompare(tp, "KernelWidthMustBeAnOddNumber", 28))
              return *np ? tag : "kernel width must be an odd number";
            else
              return tag;

          case 'm':  case 'M':
            if (p - tp == 19 && !LocaleNCompare(tp, "MissingAffineMatrix", 19))
              return *np ? tag : "Missing affine matrix";
            else
            if (p - tp == 16 && !LocaleNCompare(tp, "MissingAmplitude", 16))
              return *np ? tag : "Missing amplitude";
            else
            if (p - tp == 22 && !LocaleNCompare(tp, "MissingAnImageFilename", 22))
              return *np ? tag : "Missing an image filename";
            else
            if (p - tp == 15 && !LocaleNCompare(tp, "MissingArgument", 15))
              return *np ? tag : "Missing argument";
            else
            if (p - tp == 14 && !LocaleNCompare(tp, "MissingAzimuth", 14))
              return *np ? tag : "Missing azimuth";
            else
            if (p - tp == 22 && !LocaleNCompare(tp, "MissingBackgroundColor", 22))
              return *np ? tag : "Missing background color";
            else
            if (p - tp == 17 && !LocaleNCompare(tp, "MissingBevelWidth", 17))
              return *np ? tag : "Missing bevel width";
            else
            if (p - tp == 18 && !LocaleNCompare(tp, "MissingBorderColor", 18))
              return *np ? tag : "Missing border color";
            else
            if (p - tp == 20 && !LocaleNCompare(tp, "MissingColorizeValue", 20))
              return *np ? tag : "Missing colorize value";
            else
            if (p - tp == 13 && !LocaleNCompare(tp, "MissingColors", 13))
              return *np ? tag : "Missing colors";
            else
            if (p - tp == 14 && !LocaleNCompare(tp, "MissingComment", 14))
              return *np ? tag : "Missing comment";
            else
            if (p - tp == 25 && !LocaleNCompare(tp, "MissingCompressionQuality", 25))
              return *np ? tag : "Missing compression quality";
            else
            if (p - tp == 18 && !LocaleNCompare(tp, "MissingCycleAmount", 18))
              return *np ? tag : "Missing an cycle amount";
            else
            if (p - tp == 12 && !LocaleNCompare(tp, "MissingDelay", 12))
              return *np ? tag : "Missing delay";
            else
            if (p - tp == 20 && !LocaleNCompare(tp, "MissingDisposeMethod", 20))
              return *np ? tag : "Missing dispose method";
            else
            if (p - tp == 20 && !LocaleNCompare(tp, "MissingDissolveValue", 20))
              return *np ? tag : "Missing dissolve value";
            else
            if (p - tp == 23 && !LocaleNCompare(tp, "MissingDrawingPrimitive", 23))
              return *np ? tag : "Missing drawing primitive";
            else
            if (p - tp == 18 && !LocaleNCompare(tp, "MissingEnodingType", 18))
              return *np ? tag : "Missing encoding type";
            else
            if (p - tp == 16 && !LocaleNCompare(tp, "MissingEventMask", 16))
              return *np ? tag : "Missing event mask";
            else
            if (p - tp == 15 && !LocaleNCompare(tp, "MissingFilename", 15))
              return *np ? tag : "Missing filename";
            else
            if (p - tp == 16 && !LocaleNCompare(tp, "MissingFillColor", 16))
              return *np ? tag : "Missing fill color";
            else
            if (p - tp == 15 && !LocaleNCompare(tp, "MissingFontName", 15))
              return *np ? tag : "Missing font name";
            else
            if (p - tp == 22 && !LocaleNCompare(tp, "MissingFormatSpecifier", 22))
              return *np ? tag : "Missing format specifier";
            else
            if (p - tp == 15 && !LocaleNCompare(tp, "MissingGeometry", 15))
              return *np ? tag : "Missing geometry";
            else
            if (p - tp == 17 && !LocaleNCompare(tp, "MissingImageDepth", 17))
              return *np ? tag : "Missing image depth";
            else
            if (p - tp == 18 && !LocaleNCompare(tp, "MissingImageFrames", 18))
              return *np ? tag : "Missing image frames";
            else
            if (p - tp == 17 && !LocaleNCompare(tp, "MissingImageGamma", 17))
              return *np ? tag : "Missing image gamma";
            else
            if (p - tp == 22 && !LocaleNCompare(tp, "MissingImageIterations", 22))
              return *np ? tag : "Missing image iterations";
            else
            if (p - tp == 17 && !LocaleNCompare(tp, "MissingImageLabel", 17))
              return *np ? tag : "Missing image label";
            else
            if (p - tp == 19 && !LocaleNCompare(tp, "MissingImageProfile", 19))
              return *np ? tag : "Missing image profile";
            else
            if (p - tp == 20 && !LocaleNCompare(tp, "MissingImageRotation", 20))
              return *np ? tag : "Missing image rotation";
            else
            if (p - tp == 17 && !LocaleNCompare(tp, "MissingImageScene", 17))
              return *np ? tag : "Missing image scene";
            else
            if (p - tp == 20 && !LocaleNCompare(tp, "MissingImplodeAmount", 20))
              return *np ? tag : "Missing an implode amount";
            else
            if (p - tp == 15 && !LocaleNCompare(tp, "MissingListName", 15))
              return *np ? tag : "Missing list name";
            else
            if (p - tp == 17 && !LocaleNCompare(tp, "MissingMatteColor", 17))
              return *np ? tag : "Missing matte color";
            else
            if (p - tp == 18 && !LocaleNCompare(tp, "MissingOpaqueColor", 18))
              return *np ? tag : "Missing opaque color";
            else
            if (p - tp == 19 && !LocaleNCompare(tp, "MissingPageGeometry", 19))
              return *np ? tag : "Missing page geometry";
            else
            if (p - tp == 18 && !LocaleNCompare(tp, "MissingPaintRadius", 18))
              return *np ? tag : "Missing paint radius";
            else
            if (p - tp == 15 && !LocaleNCompare(tp, "MissingPassword", 15))
              return *np ? tag : "Missing password";
            else
            if (p - tp == 12 && !LocaleNCompare(tp, "MissingPoint", 12))
              return *np ? tag : "Missing point";
            else
            if (p - tp == 16 && !LocaleNCompare(tp, "MissingPointsize", 16))
              return *np ? tag : "Missing pointsize";
            else
            if (p - tp == 20 && !LocaleNCompare(tp, "MissingResourceLimit", 20))
              return *np ? tag : "Missing resource limit";
            else
            if (p - tp == 19 && !LocaleNCompare(tp, "MissingResourceType", 19))
              return *np ? tag : "Missing resource type";
            else
            if (p - tp == 16 && !LocaleNCompare(tp, "MissingSeedValue", 16))
              return *np ? tag : "Missing seed value";
            else
            if (p - tp == 17 && !LocaleNCompare(tp, "MissingServerName", 17))
              return *np ? tag : "Missing server name";
            else
            if (p - tp == 19 && !LocaleNCompare(tp, "MissingSpreadAmount", 19))
              return *np ? tag : "Missing an spread amount";
            else
            if (p - tp == 16 && !LocaleNCompare(tp, "MissingThreshold", 16))
              return *np ? tag : "Missing threshold";
            else
            if (p - tp == 11 && !LocaleNCompare(tp, "MissingTile", 11))
              return *np ? tag : "Missing tile";
            else
            if (p - tp == 12 && !LocaleNCompare(tp, "MissingTitle", 12))
              return *np ? tag : "Missing title";
            else
            if (p - tp == 23 && !LocaleNCompare(tp, "MissingTransparentColor", 23))
              return *np ? tag : "Missing transparent color";
            else
            if (p - tp == 16 && !LocaleNCompare(tp, "MissingTreeDepth", 16))
              return *np ? tag : "Missing tree depth";
            else
            if (p - tp == 11 && !LocaleNCompare(tp, "MissingType", 11))
              return *np ? tag : "Missing type";
            else
            if (p - tp == 20 && !LocaleNCompare(tp, "MissingViewTransform", 20))
              return *np ? tag : "Missing view transform";
            else
            if (p - tp == 25 && !LocaleNCompare(tp, "MissingVirtualPixelMethod", 25))
              return *np ? tag : "Missing virtual pixel method";
            else
            if (p - tp == 12 && !LocaleNCompare(tp, "MissingWidth", 12))
              return *np ? tag : "Missing width";
            else
            if (p - tp == 22 && !LocaleNCompare(tp, "MustSpecifyAnImageName", 22))
              return *np ? tag : "Must specify a image name";
            else
            if (p - tp == 20 && !LocaleNCompare(tp, "MustSpecifyImageSize", 20))
              return *np ? tag : "Must specify image size";
            else
              return tag;

          case 'n':  case 'N':
            if (p - tp == 15 && !LocaleNCompare(tp, "NoImagesDefined", 15))
              return *np ? tag : "No images defined";
            else
            if (p - tp == 21 && !LocaleNCompare(tp, "NoImageVectorGraphics", 21))
              return *np ? tag : "No image vector graphics";
            else
            if (p - tp == 29 && !LocaleNCompare(tp, "NonzeroWidthAndHeightRequired", 29))
              return *np ? tag : "Non-zero width and height required";
            else
            if (p - tp == 21 && !LocaleNCompare(tp, "NoProfileNameWasGiven", 21))
              return *np ? tag : "No profile name was given";
            else
              return tag;

          case 's':  case 'S':
            if (p - tp == 36 && !LocaleNCompare(tp, "SubimageSpecificationReturnsNoImages", 36))
              return *np ? tag : "Subimage specification returns no images profile name was given";
            else
              return tag;

          case 'u':  case 'U':
            if (p - tp == 26 && !LocaleNCompare(tp, "UnableToAddOrRemoveProfile", 26))
              return *np ? tag : "Unable to add or remove profile";
            else
            if (p - tp == 17 && !LocaleNCompare(tp, "UnableToBlurImage", 17))
              return *np ? tag : "Unable to blur image";
            else
            if (p - tp == 23 && !LocaleNCompare(tp, "UnableToConstituteImage", 23))
              return *np ? tag : "Unable to constitute image";
            else
            if (p - tp == 21 && !LocaleNCompare(tp, "UnableToConvolveImage", 21))
              return *np ? tag : "Unable to convolve image";
            else
            if (p - tp == 17 && !LocaleNCompare(tp, "UnableToEdgeImage", 17))
              return *np ? tag : "Unable to edge image";
            else
            if (p - tp == 19 && !LocaleNCompare(tp, "UnableToFilterImage", 19))
              return *np ? tag : "Unable to filter image";
            else
            if (p - tp == 27 && !LocaleNCompare(tp, "UnableToFormatImageMetadata", 27))
              return *np ? tag : "unable to format image meta data";
            else
            if (p - tp == 18 && !LocaleNCompare(tp, "UnableToFrameImage", 18))
              return *np ? tag : "Unable to frame image";
            else
            if (p - tp == 21 && !LocaleNCompare(tp, "UnableToOilPaintImage", 21))
              return *np ? tag : "unable to oil paint image";
            else
            if (p - tp == 18 && !LocaleNCompare(tp, "UnableToPaintImage", 18))
              return *np ? tag : "Unable to paint image";
            else
            if (p - tp == 18 && !LocaleNCompare(tp, "UnableToRaiseImage", 18))
              return *np ? tag : "Unable to raise image";
            else
            if (p - tp == 20 && !LocaleNCompare(tp, "UnableToSharpenImage", 20))
              return *np ? tag : "Unable to sharpen image";
            else
            if (p - tp == 22 && !LocaleNCompare(tp, "UnableToThresholdImage", 22))
              return *np ? tag : "Unable to threshold image";
            else
            if (p - tp == 17 && !LocaleNCompare(tp, "UnableToWaveImage", 17))
              return *np ? tag : "Unable to wave image";
            else
            if (p - tp == 21 && !LocaleNCompare(tp, "UnrecognizedAttribute", 21))
              return *np ? tag : "Unrecognized attribute";
            else
            if (p - tp == 23 && !LocaleNCompare(tp, "UnrecognizedChannelType", 23))
              return *np ? tag : "Unrecognized channel type";
            else
            if (p - tp == 17 && !LocaleNCompare(tp, "UnrecognizedColor", 17))
              return *np ? tag : "Unrecognized color";
            else
            if (p - tp == 25 && !LocaleNCompare(tp, "UnrecognizedDisposeMethod", 25))
              return *np ? tag : "Unrecognized dispose method";
            else
            if (p - tp == 19 && !LocaleNCompare(tp, "UnrecognizedElement", 19))
              return *np ? tag : "Unrecognized element";
            else
            if (p - tp == 22 && !LocaleNCompare(tp, "UnrecognizedEndianType", 22))
              return *np ? tag : "Unrecognized endian type";
            else
            if (p - tp == 23 && !LocaleNCompare(tp, "UnrecognizedGravityType", 23))
              return *np ? tag : "Unrecognized gravity type";
            else
            if (p - tp == 23 && !LocaleNCompare(tp, "UnrecognizedImageFilter", 23))
              return *np ? tag : "Unrecognized image filter";
            else
            if (p - tp == 26 && !LocaleNCompare(tp, "UnrecognizedImageInterlace", 26))
              return *np ? tag : "Unrecognized image interlace";
            else
            if (p - tp == 21 && !LocaleNCompare(tp, "UnrecognizedImageMode", 21))
              return *np ? tag : "Unrecognized image mode";
            else
            if (p - tp == 21 && !LocaleNCompare(tp, "UnrecognizedImageType", 21))
              return *np ? tag : "Unrecognized image type";
            else
            if (p - tp == 22 && !LocaleNCompare(tp, "UnrecognizedIntentType", 22))
              return *np ? tag : "Unrecognized intent type";
            else
            if (p - tp == 20 && !LocaleNCompare(tp, "UnrecognizedListType", 20))
              return *np ? tag : "Unrecognized list type";
            else
            if (p - tp == 18 && !LocaleNCompare(tp, "UnrecognizedOption", 18))
              return *np ? tag : "Unrecognized option";
            else
            if (p - tp == 24 && !LocaleNCompare(tp, "UnrecognizedResourceType", 24))
              return *np ? tag : "Unrecognized resource type";
            else
            if (p - tp == 16 && !LocaleNCompare(tp, "UnrecognizedType", 16))
              return *np ? tag : "Unrecognized list type";
            else
            if (p - tp == 30 && !LocaleNCompare(tp, "UnrecognizedVirtualPixelMethod", 30))
              return *np ? tag : "Unrecognized virtual pixel method";
            else
              return tag;
          }
      else
      if (p - tp == 24 && !LocaleNCompare(tp, "OptionLengthExceedsLimit", 24))
        return *np ? tag : "Option length exceeds limit";
      else
        return tag;

    case 'r':  case 'R':
      if (p - tp == 8 && !LocaleNCompare(tp, "Registry", 8))
        if (LocaleNCompare(NEXT_FIELD, "Error", 5) || p - tp != 5)
          return tag;
        else
          switch (*NEXT_FIELD)
          {
          default:
            return tag;

          case 'i':  case 'I':
            if (p - tp == 13 && !LocaleNCompare(tp, "ImageExpected", 13))
              return *np ? tag : "image expected";
            else
            if (p - tp == 17 && !LocaleNCompare(tp, "ImageInfoExpected", 17))
              return *np ? tag : "image info expected";
            else
              return tag;

          case 's':  case 'S':
            if (p - tp == 21 && !LocaleNCompare(tp, "StructureSizeMismatch", 21))
              return *np ? tag : "structure size mismatch";
            else
              return tag;

          case 'u':  case 'U':
            if (p - tp == 21 && !LocaleNCompare(tp, "UnableToGetRegistryID", 21))
              return *np ? tag : "Unable to get registry ID";
            else
            if (p - tp == 19 && !LocaleNCompare(tp, "UnableToLocateImage", 19))
              return *np ? tag : "Unable to locate image";
            else
            if (p - tp == 19 && !LocaleNCompare(tp, "UnableToSetRegistry", 19))
              return *np ? tag : "Unable to set registry";
            else
              return tag;
          }
      else
      if (p - tp == 8 && !LocaleNCompare(tp, "Resource", 8))
        if (LocaleNCompare(NEXT_FIELD, "Limit", 5) || p - tp != 5)
          return tag;
        else
          switch (*NEXT_FIELD)
          {
          default:
            return tag;

          case 'e':  case 'E':
            if (p - tp == 5 && !LocaleNCompare(tp, "Error", 5))
              switch (*NEXT_FIELD)
              {
              default:
                return tag;

              case 'c':  case 'C':
                if (p - tp == 23 && !LocaleNCompare(tp, "CacheResourcesExhausted", 23))
                  return *np ? tag : "Cache resources exhausted";
                else
                  return tag;

              case 'm':  case 'M':
                if (p - tp == 22 && !LocaleNCompare(tp, "MemoryAllocationFailed", 22))
                  return *np ? tag : "Memory allocation failed";
                else
                  return tag;

              case 'n':  case 'N':
                if (p - tp == 22 && !LocaleNCompare(tp, "NoPixelsDefinedInCache", 22))
                  return *np ? tag : "No pixels defined in cache";
                else
                  return tag;

              case 'p':  case 'P':
                if (p - tp == 26 && !LocaleNCompare(tp, "PixelCacheAllocationFailed", 26))
                  return *np ? tag : "Pixel cache allocation failed";
                else
                  return tag;

              case 'u':  case 'U':
                if (p - tp == 25 && !LocaleNCompare(tp, "UnableToAddGenericProfile", 25))
                  return *np ? tag : "unable to add generic profile";
                else
                if (p - tp == 21 && !LocaleNCompare(tp, "UnableToAddICMProfile", 21))
                  return *np ? tag : "unable to add ICM profile";
                else
                if (p - tp == 22 && !LocaleNCompare(tp, "UnableToAddIPTCProfile", 22))
                  return *np ? tag : "unable to add IPTC profile";
                else
                if (p - tp == 24 && !LocaleNCompare(tp, "UnableToAllocateColormap", 24))
                  return *np ? tag : "Unable to allocate colormap";
                else
                if (p - tp == 21 && !LocaleNCompare(tp, "UnableToAnnotateImage", 21))
                  return *np ? tag : "Unable to annotate image";
                else
                if (p - tp == 28 && !LocaleNCompare(tp, "UnableToAverageImageSequence", 28))
                  return *np ? tag : "unable to average image sequence";
                else
                if (p - tp == 17 && !LocaleNCompare(tp, "UnableToBlurImage", 17))
                  return *np ? tag : "unable to blur image";
                else
                if (p - tp == 17 && !LocaleNCompare(tp, "UnableToChopImage", 17))
                  return *np ? tag : "unable to chop image";
                else
                if (p - tp == 18 && !LocaleNCompare(tp, "UnableToCloneImage", 18))
                  return *np ? tag : "unable to clone image";
                else
                if (p - tp == 21 && !LocaleNCompare(tp, "UnableToCoalesceImage", 21))
                  return *np ? tag : "unable to coalesce image";
                else
                if (p - tp == 29 && !LocaleNCompare(tp, "UnableToComputeImageSignature", 29))
                  return *np ? tag : "unable to compute image signature";
                else
                if (p - tp == 23 && !LocaleNCompare(tp, "UnableToConstituteImage", 23))
                  return *np ? tag : "unable to constitute image";
                else
                if (p - tp == 19 && !LocaleNCompare(tp, "UnableToConvertFont", 19))
                  return *np ? tag : "unable to convert font";
                else
                if (p - tp == 22 && !LocaleNCompare(tp, "UnableToCreateColormap", 22))
                  return *np ? tag : "Unable to create colormap";
                else
                if (p - tp == 28 && !LocaleNCompare(tp, "UnableToCreateColorTransform", 28))
                  return *np ? tag : "unable to create color transform";
                else
                if (p - tp == 27 && !LocaleNCompare(tp, "UnableToCreateCommandWidget", 27))
                  return *np ? tag : "unable to create command widget";
                else
                if (p - tp == 24 && !LocaleNCompare(tp, "UnableToCreateImageGroup", 24))
                  return *np ? tag : "Unable to create image group";
                else
                if (p - tp == 26 && !LocaleNCompare(tp, "UnableToCreateImageMontage", 26))
                  return *np ? tag : "Unable to create image montage";
                else
                if (p - tp == 21 && !LocaleNCompare(tp, "UnableToCreateXWindow", 21))
                  return *np ? tag : "unable to create X window";
                else
                if (p - tp == 17 && !LocaleNCompare(tp, "UnableToCropImage", 17))
                  return *np ? tag : "unable to crop image";
                else
                if (p - tp == 22 && !LocaleNCompare(tp, "UnableToDespeckleImage", 22))
                  return *np ? tag : "unable to despeckle image";
                else
                if (p - tp == 27 && !LocaleNCompare(tp, "UnableToDetermineImageClass", 27))
                  return *np ? tag : "unable to determine image class";
                else
                if (p - tp == 39 && !LocaleNCompare(tp, "UnableToDetermineTheNumberOfImageColors", 39))
                  return *np ? tag : "unable to determine the number of image colors";
                else
                if (p - tp == 19 && !LocaleNCompare(tp, "UnableToDitherImage", 19))
                  return *np ? tag : "unable to dither image";
                else
                if (p - tp == 19 && !LocaleNCompare(tp, "UnableToDrawOnImage", 19))
                  return *np ? tag : "unable to draw on image";
                else
                if (p - tp == 17 && !LocaleNCompare(tp, "UnableToEdgeImage", 17))
                  return *np ? tag : "unable to edge image";
                else
                if (p - tp == 19 && !LocaleNCompare(tp, "UnableToEmbossImage", 19))
                  return *np ? tag : "unable to emboss image";
                else
                if (p - tp == 20 && !LocaleNCompare(tp, "UnableToEnhanceImage", 20))
                  return *np ? tag : "unable to enhance image";
                else
                if (p - tp == 22 && !LocaleNCompare(tp, "UnableToFloodfillImage", 22))
                  return *np ? tag : "unable to floodfill image";
                else
                if (p - tp == 25 && !LocaleNCompare(tp, "UnableToGammaCorrectImage", 25))
                  return *np ? tag : "unable to gamma correct image";
                else
                if (p - tp == 23 && !LocaleNCompare(tp, "UnableToGetBestIconSize", 23))
                  return *np ? tag : "unable to get best icon size";
                else
                if (p - tp == 23 && !LocaleNCompare(tp, "UnableToGetFromRegistry", 23))
                  return *np ? tag : "unable to get from registry";
                else
                if (p - tp == 18 && !LocaleNCompare(tp, "UnableToLevelImage", 18))
                  return *np ? tag : "unable to level image";
                else
                if (p - tp == 20 && !LocaleNCompare(tp, "UnableToMagnifyImage", 20))
                  return *np ? tag : "unable to magnify image";
                else
                if (p - tp == 19 && !LocaleNCompare(tp, "UnableToManageColor", 19))
                  return *np ? tag : "Unable to manage color";
                else
                if (p - tp == 16 && !LocaleNCompare(tp, "UnableToMapImage", 16))
                  return *np ? tag : "Unable to map image";
                else
                if (p - tp == 24 && !LocaleNCompare(tp, "UnableToMapImageSequence", 24))
                  return *np ? tag : "Unable to map image sequence";
                else
                if (p - tp == 25 && !LocaleNCompare(tp, "UnableToMedianFilterImage", 25))
                  return *np ? tag : "unable to median filter image";
                else
                if (p - tp == 23 && !LocaleNCompare(tp, "UnableToMotionBlurImage", 23))
                  return *np ? tag : "unable to motion blur image";
                else
                if (p - tp == 24 && !LocaleNCompare(tp, "UnableToNoiseFilterImage", 24))
                  return *np ? tag : "unable to noise filter image";
                else
                if (p - tp == 22 && !LocaleNCompare(tp, "UnableToNormalizeImage", 22))
                  return *np ? tag : "unable to normalize image";
                else
                if (p - tp == 24 && !LocaleNCompare(tp, "UnableToOpenColorProfile", 24))
                  return *np ? tag : "unable to open color profile";
                else
                if (p - tp == 21 && !LocaleNCompare(tp, "UnableToQuantizeImage", 21))
                  return *np ? tag : "unable to quantize image";
                else
                if (p - tp == 29 && !LocaleNCompare(tp, "UnableToQuantizeImageSequence", 29))
                  return *np ? tag : "unable to quantize image sequence";
                else
                if (p - tp == 18 && !LocaleNCompare(tp, "UnableToReadXImage", 18))
                  return *np ? tag : "unable to read X image";
                else
                if (p - tp == 27 && !LocaleNCompare(tp, "UnableToReadXServerColormap", 27))
                  return *np ? tag : "unable to read X server colormap";
                else
                if (p - tp == 19 && !LocaleNCompare(tp, "UnableToResizeImage", 19))
                  return *np ? tag : "unable to resize image";
                else
                if (p - tp == 19 && !LocaleNCompare(tp, "UnableToRotateImage", 19))
                  return *np ? tag : "unable to rotate image";
                else
                if (p - tp == 19 && !LocaleNCompare(tp, "UnableToSampleImage", 19))
                  return *np ? tag : "unable to sample image";
                else
                if (p - tp == 18 && !LocaleNCompare(tp, "UnableToScaleImage", 18))
                  return *np ? tag : "unable to scale image";
                else
                if (p - tp == 19 && !LocaleNCompare(tp, "UnableToSelectImage", 19))
                  return *np ? tag : "unable to select image";
                else
                if (p - tp == 20 && !LocaleNCompare(tp, "UnableToSharpenImage", 20))
                  return *np ? tag : "unable to sharpen image";
                else
                if (p - tp == 18 && !LocaleNCompare(tp, "UnableToShaveImage", 18))
                  return *np ? tag : "unable to shave image";
                else
                if (p - tp == 18 && !LocaleNCompare(tp, "UnableToShearImage", 18))
                  return *np ? tag : "unable to shear image";
                else
                if (p - tp == 25 && !LocaleNCompare(tp, "UnableToSortImageColormap", 25))
                  return *np ? tag : "unable to sort image colormap";
                else
                if (p - tp == 22 && !LocaleNCompare(tp, "UnableToThresholdImage", 22))
                  return *np ? tag : "unable to threshold image";
                else
                if (p - tp == 27 && !LocaleNCompare(tp, "UnableToTransformColorspace", 27))
                  return *np ? tag : "unable to transform colorspace";
                else
                  return tag;
              }
            else
              return tag;

          case 'f':  case 'F':
            if (p - tp == 10 && !LocaleNCompare(tp, "FatalError", 10))
              if (LocaleNCompare(NEXT_FIELD, "MemoryAllocationFailed", 22) || p - tp != 22)
                return tag;
              else
                return *np ? tag : "Memory allocation failed";
            else
              return tag;

          case 'w':  case 'W':
            if (p - tp == 7 && !LocaleNCompare(tp, "Warning", 7))
              if (LocaleNCompare(NEXT_FIELD, "MemoryAllocationFailed", 22) || p - tp != 22)
                return tag;
              else
                return *np ? tag : "Memory allocation failed";
            else
              return tag;
          }
      else
        return tag;

    case 's':  case 'S':
      if (p - tp == 6 && !LocaleNCompare(tp, "Stream", 6))
        if (LocaleNCompare(NEXT_FIELD, "Error", 5) || p - tp != 5)
          return tag;
        else
          switch (*NEXT_FIELD)
          {
          default:
            return tag;

          case 'i':  case 'I':
            if (p - tp == 36 && !LocaleNCompare(tp, "ImageDoesNotContainTheStreamGeometry", 36))
              return *np ? tag : "image does not contain the stream geometry";
            else
              return tag;

          case 'n':  case 'N':
            if (p - tp == 24 && !LocaleNCompare(tp, "NoStreamHandlerIsDefined", 24))
              return *np ? tag : "no stream handler is defined";
            else
              return tag;

          case 'p':  case 'P':
            if (p - tp == 19 && !LocaleNCompare(tp, "PixelCacheIsNotOpen", 19))
              return *np ? tag : "Pixel cache is not open";
            else
              return tag;

          case 'u':  case 'U':
            if (p - tp == 26 && !LocaleNCompare(tp, "UnableToAcquirePixelStream", 26))
              return *np ? tag : "Unable to acquire pixel stream";
            else
            if (p - tp == 22 && !LocaleNCompare(tp, "UnableToSetPixelStream", 22))
              return *np ? tag : "Unable to set pixel stream";
            else
            if (p - tp == 23 && !LocaleNCompare(tp, "UnableToSyncPixelStream", 23))
              return *np ? tag : "Unable to sync pixel stream";
            else
              return tag;
          }
      else
        return tag;

    case 't':  case 'T':
      if (p - tp == 4 && !LocaleNCompare(tp, "Type", 4))
        if (LocaleNCompare(NEXT_FIELD, "Error", 5) || p - tp != 5)
          return tag;
        else
          switch (*NEXT_FIELD)
          {
          default:
            return tag;

          case 'f':  case 'F':
            if (p - tp == 24 && !LocaleNCompare(tp, "FontSubstitutionRequired", 24))
              return *np ? tag : "Font substitution required";
            else
              return tag;

          case 'u':  case 'U':
            if (p - tp == 22 && !LocaleNCompare(tp, "UnableToGetTypeMetrics", 22))
              return *np ? tag : "Unable to get type metrics";
            else
            if (p - tp == 33 && !LocaleNCompare(tp, "UnableToInitializeFreetypeLibrary", 33))
              return *np ? tag : "Unable to initialize freetype library";
            else
            if (p - tp == 16 && !LocaleNCompare(tp, "UnableToReadFont", 16))
              return *np ? tag : "Unable to read font";
            else
            if (p - tp == 24 && !LocaleNCompare(tp, "UnrecognizedFontEncoding", 24))
              return *np ? tag : "Unrecognized font encoding";
            else
              return tag;
          }
      else
        return tag;

    case 'u':  case 'U':
      if (p - tp == 18 && !LocaleNCompare(tp, "UnrecognizedOption", 18))
        return *np ? tag : "Unrecognized option";
      else
        return tag;

    case 'x':  case 'X':
      if (p - tp == 7 && !LocaleNCompare(tp, "XServer", 7))
        switch (*NEXT_FIELD)
        {
        default:
          return tag;

        case 'e':  case 'E':
          if (p - tp == 5 && !LocaleNCompare(tp, "Error", 5))
            switch (*NEXT_FIELD)
            {
            default:
              return tag;

            case 'c':  case 'C':
              if (p - tp == 23 && !LocaleNCompare(tp, "ColorIsNotKnownToServer", 23))
                return *np ? tag : "Color is not known to server";
              else
                return tag;

            case 'i':  case 'I':
              if (p - tp == 22 && !LocaleNCompare(tp, "InvalidVisualSpecifier", 22))
                return *np ? tag : "Invalid visual specifier";
              else
                return tag;

            case 'n':  case 'N':
              if (p - tp == 29 && !LocaleNCompare(tp, "NoWindowWithSpecifiedIDExists", 29))
                return *np ? tag : "No window with specified ID exists";
              else
                return tag;

            case 's':  case 'S':
              if (p - tp == 32 && !LocaleNCompare(tp, "StandardColormapIsNotInitialized", 32))
                return *np ? tag : "Standard Colormap is not initialized";
              else
                return tag;

            case 'u':  case 'U':
              if (p - tp == 30 && !LocaleNCompare(tp, "UnableToConnectToRemoteDisplay", 30))
                return *np ? tag : "Unable to connect to remote display";
              else
              if (p - tp == 20 && !LocaleNCompare(tp, "UnableToCreateBitmap", 20))
                return *np ? tag : "Unable to create bitmap";
              else
              if (p - tp == 20 && !LocaleNCompare(tp, "UnableToCreatePixmap", 20))
                return *np ? tag : "Unable to create pixmap";
              else
              if (p - tp == 22 && !LocaleNCompare(tp, "UnableToCreateProperty", 22))
                return *np ? tag : "Unable to create property";
              else
              if (p - tp == 30 && !LocaleNCompare(tp, "UnableToCreateStandardColormap", 30))
                return *np ? tag : "Unable to create standard colormap";
              else
              if (p - tp == 19 && !LocaleNCompare(tp, "UnableToGetProperty", 19))
                return *np ? tag : "Unable to get property";
              else
              if (p - tp == 27 && !LocaleNCompare(tp, "UnableToGetStandardColormap", 27))
                return *np ? tag : "Unable to get Standard Colormap";
              else
              if (p - tp == 17 && !LocaleNCompare(tp, "UnableToGetVisual", 17))
                return *np ? tag : "Unable to get visual";
              else
              if (p - tp == 17 && !LocaleNCompare(tp, "UnableToGrabMouse", 17))
                return *np ? tag : "Unable to grab mouse";
              else
              if (p - tp == 16 && !LocaleNCompare(tp, "UnableToLoadFont", 16))
                return *np ? tag : "Unable to load font";
              else
              if (p - tp == 37 && !LocaleNCompare(tp, "UnableToMatchVisualToStandardColormap", 37))
                return *np ? tag : "Unable to match visual to Standard Colormap";
              else
              if (p - tp == 19 && !LocaleNCompare(tp, "UnableToOpenXServer", 19))
                return *np ? tag : "Unable to open X server";
              else
              if (p - tp == 29 && !LocaleNCompare(tp, "UnableToReadXWindowAttributes", 29))
                return *np ? tag : "Unable to read X window attributes";
              else
              if (p - tp == 24 && !LocaleNCompare(tp, "UnableToReadXWindowImage", 24))
                return *np ? tag : "Unable to read X window image";
              else
              if (p - tp == 24 && !LocaleNCompare(tp, "UnrecognizedColormapType", 24))
                return *np ? tag : "Unrecognized colormap type";
              else
              if (p - tp == 23 && !LocaleNCompare(tp, "UnrecognizedGravityType", 23))
                return *np ? tag : "Unrecognized gravity type";
              else
                return tag;
            }
          else
            return tag;

        case 'f':  case 'F':
          if (p - tp == 10 && !LocaleNCompare(tp, "FatalError", 10))
            switch (*NEXT_FIELD)
            {
            default:
              return tag;

            case 'u':  case 'U':
              if (p - tp == 20 && !LocaleNCompare(tp, "UnableToCreateCursor", 20))
                return *np ? tag : "Unable to create X cursor";
              else
              if (p - tp == 28 && !LocaleNCompare(tp, "UnableToCreateGraphicContext", 28))
                return *np ? tag : "Unable to create graphic context";
              else
              if (p - tp == 26 && !LocaleNCompare(tp, "UnableToCreateTextProperty", 26))
                return *np ? tag : "Unable to create text property";
              else
              if (p - tp == 20 && !LocaleNCompare(tp, "UnableToCreateXImage", 20))
                return *np ? tag : "Unable to create X image";
              else
              if (p - tp == 21 && !LocaleNCompare(tp, "UnableToCreateXPixmap", 21))
                return *np ? tag : "Unable to create X pixmap";
              else
              if (p - tp == 21 && !LocaleNCompare(tp, "UnableToCreateXWindow", 21))
                return *np ? tag : "Unable to create X window";
              else
              if (p - tp == 17 && !LocaleNCompare(tp, "UnableToGetVisual", 17))
                return *np ? tag : "Unable to get visual";
              else
              if (p - tp == 16 && !LocaleNCompare(tp, "UnableToLoadFont", 16))
                return *np ? tag : "Unable to load font";
              else
              if (p - tp == 19 && !LocaleNCompare(tp, "UnableToOpenXServer", 19))
                return *np ? tag : "Unable to open X server";
              else
                return tag;
            }
          else
            return tag;

        case 'w':  case 'W':
          if (p - tp == 7 && !LocaleNCompare(tp, "Warning", 7))
            switch (*NEXT_FIELD)
            {
            default:
              return tag;

            case 'u':  case 'U':
              if (p - tp == 17 && !LocaleNCompare(tp, "UnableToGetVisual", 17))
                return *np ? tag : "Unable to get visual";
              else
              if (p - tp == 18 && !LocaleNCompare(tp, "UsingDefaultVisual", 18))
                return *np ? tag : "UsingDefaultVisual";
              else
                return tag;
            }
          else
            return tag;
        }
      else
        return tag;
    }

   return tag;
}
