/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%        AAA   TTTTT  TTTTT  RRRR   IIIII  BBBB   U   U  TTTTT  EEEEE         %
%       A   A    T      T    R   R    I    B   B  U   U    T    E             %
%       AAAAA    T      T    RRRR     I    BBBB   U   U    T    EEE           %
%       A   A    T      T    R R      I    B   B  U   U    T    E             %
%       A   A    T      T    R  R   IIIII  BBBB    UUU     T    EEEEE         %
%                                                                             %
%                                                                             %
%              Methods to Get/Set/Destroy Image Text Attributes               %
%                                                                             %
%                                                                             %
%                             Software Design                                 %
%                               John Cristy                                   %
%                              February 2000                                  %
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
%  license.  Except as expressly stated in the immediately preceding          %
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
%  The Attributes methods gets, sets, or destroys attributes associated
%  with a particular image (e.g. comments, copyright, author, etc).
%
%
*/

/*
  Include declarations.
*/
#include "studio.h"
#include "attribute.h"
#include "blob.h"
#include "list.h"
#include "render.h"
#include "utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y I m a g e A t t r i b u t e s                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyImageAttributes() deallocates memory associated with the image
%  attribute list.
%
%  The format of the DestroyImageAttributes method is:
%
%      DestroyImageAttributes(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport void DestroyImageAttributes(Image *image)
{
  ImageAttribute
    *attribute;

  register ImageAttribute
    *p;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  for (p=image->attributes; p != (ImageAttribute *) NULL; )
  {
    attribute=p;
    p=p->next;
    if (attribute->key != (char *) NULL)
      LiberateMemory((void **) &attribute->key);
    if (attribute->value != (char *) NULL)
      LiberateMemory((void **) &attribute->value);
    LiberateMemory((void **) &attribute);
  }
  image->attributes=(ImageAttribute *) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e A t t r i b u t e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageAttribute() searches the list of image attributes and returns
%  a pointer to the attribute if it exists otherwise NULL.
%
%  The format of the GetImageAttribute method is:
%
%      const ImageAttribute *GetImageAttribute(const Image *image,
%        const char *key)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o key:  These character strings are the name of an image attribute to
%      return.
%
%
*/

static unsigned int GenerateIPTCAttribute(Image *image,const char *key)
{
  char
    *attribute;

  int
    count,
    dataset,
    record;

  register long
    i;

  size_t
    length;

  if (image->iptc_profile.length == 0)
    return(False);
  count=sscanf(key,"IPTC:%d:%d",&dataset,&record);
  if (count != 2)
    return(False);
  for (i=0; i < (long) image->iptc_profile.length; i++)
  {
    if (image->iptc_profile.info[i] != 0x1c)
      continue;
    if (image->iptc_profile.info[i+1] != dataset)
      continue;
    if (image->iptc_profile.info[i+2] != record)
      continue;
    length=image->iptc_profile.info[i+3] << 8;
    length|=image->iptc_profile.info[i+4];
    attribute=(char *) AcquireMemory(length+MaxTextExtent);
    if (attribute == (char *) NULL)
      continue;
    (void) strncpy(attribute,(char *) image->iptc_profile.info+i+5,length);
    attribute[length]='\0';
    (void) SetImageAttribute(image,key,(const char *) attribute);
    LiberateMemory((void **) &attribute);
    break;
  }
  return(i < (long) image->iptc_profile.length);
}

static unsigned char ReadByte(char **p,size_t *length)
{
  unsigned char
    c;

  if (*length < 1)
    return(0xff);
  c=(*(*p)++);
  (*length)--;
  return(c);
}

static long ReadMSBLong(char **p,size_t *length)
{
  int
    c;

  long
    value;

  register long
    i;

  unsigned char
    buffer[4];

  if (*length < 4)
    return(-1);
  for (i=0; i < 4; i++)
  {
    c=(*(*p)++);
    (*length)--;
    buffer[i]=(unsigned char) c;
  }
  value=buffer[0] << 24;
  value|=buffer[1] << 16;
  value|=buffer[2] << 8;
  value|=buffer[3];
  return(value);
}

static int ReadMSBShort(char **p,size_t *length)
{
  int
    c,
    value;

  register long
    i;

  unsigned char
    buffer[2];

  if (*length < 2)
    return(-1);
  for (i=0; i < 2; i++)
  {
    c=(*(*p)++);
    (*length)--;
    buffer[i]=(unsigned char) c;
  }
  value=buffer[0] << 8;
  value|=buffer[1];
  return(value);
}

static char *TraceClippingPath(char *blob,size_t length,unsigned long columns,
  unsigned long rows)
{
  char
    *path,
    *message;

  int
    count,
    selector;

  long
    x,
    y;

  PointInfo
    first[3],
    last[3],
    point[3];

  register long
    i;

  unsigned int
    status;

  path=AllocateString((char *) NULL);
  if (path == (char *) NULL)
    return((char *) NULL);
  message=AllocateString((char *) NULL);
  while (length != 0)
  {
    selector=ReadMSBShort(&blob,&length);
    if (selector != 6)
      {
        /*
          Path fill record.
        */
        blob+=24;
        length-=24;
        continue;
      }
    blob+=24;
    length-=24;
    FormatString(message,"<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n");
    (void) ConcatenateString(&path,message);
    FormatString(message,"<svg width=\"%lu\" height=\"%lu\">\n",columns,rows);
    (void) ConcatenateString(&path,message);
    FormatString(message,"<g>\n");
    (void) ConcatenateString(&path,message);
    FormatString(message,"<path style=\"fill:#ffffff;stroke:none\" d=\"\n");
    (void) ConcatenateString(&path,message);
    while (length != 0)
    {
      selector=ReadMSBShort(&blob,&length);
      if (selector > 8)
        break;
      count=ReadMSBShort(&blob,&length);
      blob+=22;
      length-=22;
      status=True;
      while (count > 0)
      {
        selector=ReadMSBShort(&blob,&length);
        if ((selector == 1) || (selector == 2) || (selector == 4) ||
            (selector == 5))
          {
            for (i=0; i < 3; i++)
            {
              y=ReadMSBLong(&blob,&length);
              x=ReadMSBLong(&blob,&length);
              point[i].x=(double) x*columns/4096/4096;
              point[i].y=(double) y*rows/4096/4096;
            }
            if (status)
              {
                FormatString(message,"M %.1f,%.1f\n",point[1].x,point[1].y);
                for (i=0; i < 3; i++)
                {
                  first[i]=point[i];
                  last[i]=point[i];
                }
              }
            else
              {
                FormatString(message,"C %.1f,%.1f %.1f,%.1f %.1f,%.1f\n",
                  last[2].x,last[2].y,point[0].x,point[0].y,point[1].x,
                  point[1].y);
                for (i=0; i < 3; i++)
                  last[i]=point[i];
              }
          (void) ConcatenateString(&path,message);
          status=False;
          count--;
        }
      }
      if (!status)
        {
          FormatString(message,"C %.1f,%.1f %.1f,%.1f %.1f,%.1f Z\n",last[2].x,
            last[2].y,first[0].x,first[0].y,first[1].x,first[1].y);
          (void) ConcatenateString(&path,message);
        }
    }
    FormatString(message,"\"/>\n");
    (void) ConcatenateString(&path,message);
    FormatString(message,"</g>\n");
    (void) ConcatenateString(&path,message);
    FormatString(message,"</svg>\n");
    (void) ConcatenateString(&path,message);
    break;
  }
  LiberateMemory((void **) &message);
  return(path);
}

static int Generate8BIMAttribute(Image *image,const char *key)
{
  char
    *attribute,
    *string;

  int
    id,
    start,
    stop;

  register long
    i;

  size_t
    length;

  unsigned char
    *info;

  unsigned int
    status;

  unsigned long
    count;

  if (image->iptc_profile.length == 0)
    return(False);
  count=sscanf(key,"8BIM:%d,%d",&start,&stop);
  if (count != 2)
    return(False);
  status=False;
  length=image->iptc_profile.length;
  info=image->iptc_profile.info;
  while (length != 0)
  {
    if (ReadByte((char **) &info,&length) != '8')
      continue;
    if (ReadByte((char **) &info,&length) != 'B')
      continue;
    if (ReadByte((char **) &info,&length) != 'I')
      continue;
    if (ReadByte((char **) &info,&length) != 'M')
      continue;
    id=ReadMSBShort((char **) &info,&length);
    if (id < start)
      continue;
    if (id > stop)
      continue;
    count=ReadByte((char **) &info,&length);
    string=(char *) NULL;
    if ((count != 0) && (count <= length))
      {
        string=(char *) AcquireMemory(count+MaxTextExtent);
        if (string != (char *) NULL)
          {
            for (i=0; i < (long) count; i++)
              string[i]=(char) ReadByte((char **) &info,&length);
            string[count]=0;
            LiberateMemory((void **) &string);
          }
      }
    if (!(count & 0x01))
      (void) ReadByte((char **) &info,&length);
    count=ReadMSBLong((char **) &info,&length);
    attribute=(char *) AcquireMemory(count+MaxTextExtent);
    if (attribute != (char *) NULL)
      {
        memcpy(attribute,(char *) info,count);
        attribute[count]='\0';
        info+=count;
        length-=count;
        if ((id <= 1999) || (id >= 2999))
          (void) SetImageAttribute(image,key,(const char *) attribute);
        else
          {
            char
              *path;

            path=TraceClippingPath(attribute,count,image->columns,image->rows);
            (void) SetImageAttribute(image,key,(const char *) path);
            LiberateMemory((void **) &path);
          }
        LiberateMemory((void **) &attribute);
        status=True;
      }
  }
  return(status);
}

#define DE_STACK_SIZE  16
#define EXIF_DELIMITER  "\n"
#define EXIF_NUM_FORMATS  12
#define EXIF_FMT_BYTE  1
#define EXIF_FMT_STRING  2
#define EXIF_FMT_USHORT  3
#define EXIF_FMT_ULONG  4
#define EXIF_FMT_URATIONAL  5
#define EXIF_FMT_SBYTE  6
#define EXIF_FMT_UNDEFINED  7
#define EXIF_FMT_SSHORT  8
#define EXIF_FMT_SLONG  9
#define EXIF_FMT_SRATIONAL  10
#define EXIF_FMT_SINGLE  11
#define EXIF_FMT_DOUBLE  12
#define TAG_EXIF_OFFSET  0x8769
#define TAG_INTEROP_OFFSET  0xa005

typedef struct _TagInfo
{
  unsigned short
    tag;

  char
    *description;
} TagInfo;

static TagInfo
  tag_table[] =
  {
    {  0x100, "ImageWidth"},
    {  0x101, "ImageLength"},
    {  0x102, "BitsPerSample"},
    {  0x103, "Compression"},
    {  0x106, "PhotometricInterpretation"},
    {  0x10A, "FillOrder"},
    {  0x10D, "DocumentName"},
    {  0x10E, "ImageDescription"},
    {  0x10F, "Make"},
    {  0x110, "Model"},
    {  0x111, "StripOffsets"},
    {  0x112, "Orientation"},
    {  0x115, "SamplesPerPixel"},
    {  0x116, "RowsPerStrip"},
    {  0x117, "StripByteCounts"},
    {  0x11A, "XResolution"},
    {  0x11B, "YResolution"},
    {  0x11C, "PlanarConfiguration"},
    {  0x128, "ResolutionUnit"},
    {  0x12D, "TransferFunction"},
    {  0x131, "Software"},
    {  0x132, "DateTime"},
    {  0x13B, "Artist"},
    {  0x13E, "WhitePoint"},
    {  0x13F, "PrimaryChromaticities"},
    {  0x156, "TransferRange"},
    {  0x200, "JPEGProc"},
    {  0x201, "JPEGInterchangeFormat"},
    {  0x202, "JPEGInterchangeFormatLength"},
    {  0x211, "YCbCrCoefficients"},
    {  0x212, "YCbCrSubSampling"},
    {  0x213, "YCbCrPositioning"},
    {  0x214, "ReferenceBlackWhite"},
    {  0x828D, "CFARepeatPatternDim"},
    {  0x828E, "CFAPattern"},
    {  0x828F, "BatteryLevel"},
    {  0x8298, "Copyright"},
    {  0x829A, "ExposureTime"},
    {  0x829D, "FNumber"},
    {  0x83BB, "IPTC/NAA"},
    {  0x8769, "ExifOffset"},
    {  0x8773, "InterColorProfile"},
    {  0x8822, "ExposureProgram"},
    {  0x8824, "SpectralSensitivity"},
    {  0x8825, "GPSInfo"},
    {  0x8827, "ISOSpeedRatings"},
    {  0x8828, "OECF"},
    {  0x9000, "ExifVersion"},
    {  0x9003, "DateTimeOriginal"},
    {  0x9004, "DateTimeDigitized"},
    {  0x9101, "ComponentsConfiguration"},
    {  0x9102, "CompressedBitsPerPixel"},
    {  0x9201, "ShutterSpeedValue"},
    {  0x9202, "ApertureValue"},
    {  0x9203, "BrightnessValue"},
    {  0x9204, "ExposureBiasValue"},
    {  0x9205, "MaxApertureValue"},
    {  0x9206, "SubjectDistance"},
    {  0x9207, "MeteringMode"},
    {  0x9208, "LightSource"},
    {  0x9209, "Flash"},
    {  0x920A, "FocalLength"},
    {  0x927C, "MakerNote"},
    {  0x9286, "UserComment"},
    {  0x9290, "SubSecTime"},
    {  0x9291, "SubSecTimeOriginal"},
    {  0x9292, "SubSecTimeDigitized"},
    {  0xA000, "FlashPixVersion"},
    {  0xA001, "ColorSpace"},
    {  0xA002, "ExifImageWidth"},
    {  0xA003, "ExifImageLength"},
    {  0xA005, "InteroperabilityOffset"},
    {  0xA20B, "FlashEnergy"},
    {  0xA20C, "SpatialFrequencyResponse"},
    {  0xA20E, "FocalPlaneXResolution"},
    {  0xA20F, "FocalPlaneYResolution"},
    {  0xA210, "FocalPlaneResolutionUnit"},
    {  0xA214, "SubjectLocation"},
    {  0xA215, "ExposureIndex"},
    {  0xA217, "SensingMethod"},
    {  0xA300, "FileSource"},
    {  0xA301, "SceneType"},
    {  0x0000, (char *) NULL}
  };

static int
  format_bytes[] = {0,1,1,2,4,8,1,1,2,4,8,4,8};

static short Read16s(int morder,void *ishort)
{
  short
    value;

  if (morder)
    value=(((unsigned char *) ishort)[0] << 8) | ((unsigned char *) ishort)[1];
  else
    value=(((unsigned char *) ishort)[1] << 8) | ((unsigned char *) ishort)[0];
  return(value);
}

static unsigned short Read16u(int morder,void *ishort)
{
  unsigned short
    value;

  if (morder)
    value=(((unsigned char *) ishort)[0] << 8) | ((unsigned char *) ishort)[1];
  else
    value=(((unsigned char *) ishort)[1] << 8) | ((unsigned char *) ishort)[0];
  return(value);
}

static long Read32s(int morder,void *ilong)
{
  long
    value;

  if (morder)
    value=(((char *) ilong)[0] << 24) | (((unsigned char *) ilong)[1] << 16) |
      (((unsigned char *) ilong)[2] << 8) | (((unsigned char *) ilong)[3]);
  else
    value=(((char *) ilong)[3] << 24) | (((unsigned char *) ilong)[2] << 16) |
      (((unsigned char *) ilong)[1] << 8 ) | (((unsigned char *) ilong)[0]);
  return(value);
}

static unsigned long Read32u(int morder, void *ilong)
{
  return(Read32s(morder,ilong) & 0xffffffff);
}

static int GenerateEXIFAttribute(Image *image,const char *specification)
{
  char
    *key,
    *value,
    *final;

  int
    id,
    level,
    morder,
    all;

  long
    index,
    tag;

  register long
    i;

  size_t
    length;

  unsigned long
    offset;

  unsigned char
    *tiffp,
    *ifdstack[DE_STACK_SIZE],
    *ifdp,
    *info;

  unsigned int
    de,
    destack[DE_STACK_SIZE],
    nde;

  /*
    Determine if there is any EXIF data available in the image.
  */
  value=(char *) NULL;
  final=AllocateString("");
  index=(-1);
  for (i=0; i < (long) image->generic_profiles; i++)
  {
    if ((LocaleCompare(image->generic_profile[i].name,"APP1") == 0) &&
        (image->generic_profile[i].length != 0))
      {
        index=i;
        break;
      }
  }
  if (index < 0)
    return(False);
  /*
    If EXIF data exists, then try to parse the request for a tag.
  */
  key=(char *) &specification[5];
  if ((key == (char *) NULL) || (*key == '\0'))
    return(False);
  while (isspace((int) (*key)))
    key++;
  all=0;
  tag=(-1);
  switch(*key)
  {
    /*
      Caller has asked for all the tags in the EXIF data.
    */
    case '*':
    {
      tag=0;
      all=1; /* return the data in description=value format */
      break;
    }
    case '!':
    {
      tag=0;
      all=2; /* return the data in tageid=value format */
      break;
    }
    /*
      Check for a hex based tag specification first.
    */
    case '#':
    {
      char
        c;

      unsigned long
        n;

      tag=0;
      key++;
      n=strlen(key);
      if (n != 4)
        return(False);
      else
        {
          /*
            Parse tag specification as a hex number.
          */
          n/=4;
          do
          {
            for (i=(long) n-1; i >= 0; i--)
            {
              c=(*key++);
              tag<<=4;
              if ((c >= '0') && (c <= '9'))
                tag|=c-'0';
              else
                if ((c >= 'A') && (c <= 'F'))
                  tag|=c-('A'-10);
                else
                  if ((c >= 'a') && (c <= 'f'))
                    tag|=c-('a'-10);
                  else
                    return(False);
            }
          } while (*key != '\0');
        }
      break;
    }
    default:
    {
      /*
        Try to match the text with a tag name instead.
      */
      for (i=0; ; i++)
      {
        if (tag_table[i].tag == 0)
          break;
        if (LocaleCompare(tag_table[i].description,key) == 0)
          {
            tag=tag_table[i].tag;
            break;
          }
      }
      break;
    }
  }
  if (tag < 0)
    return(False);
  length=image->generic_profile[index].length;
  info=image->generic_profile[index].info;
  while (length != 0)
  {
    if (ReadByte((char **) &info,&length) != 0x45)
      continue;
    if (ReadByte((char **) &info,&length) != 0x78)
      continue;
    if (ReadByte((char **) &info,&length) != 0x69)
      continue;
    if (ReadByte((char **) &info,&length) != 0x66)
      continue;
    if (ReadByte((char **) &info,&length) != 0x00)
      continue;
    if (ReadByte((char **) &info,&length) != 0x00)
      continue;
    break;
  }
  if (length < 16)
    return(False);
  tiffp=info;
  id=Read16u(0,tiffp);
  morder=0;
  if (id == 0x4949) /* LSB */
    morder=0;
  else
    if (id == 0x4D4D) /* MSB */
      morder=1;
    else
      return(False);
  if (Read16u(morder,tiffp+2) != 0x002a)
    return(False);
  /*
    This is the offset to the first IFD.
  */
  offset=Read32u(morder,tiffp+4);
  if (offset >= length)
    return(False);
  /*
    Set the pointer to the first IFD and follow it were it leads.
  */
  ifdp=tiffp+offset;
  level=0;
  de=0;
  do
  {
    /*
      If there is anything on the stack then pop it off.
    */
    if (level > 0)
      {
        level--;
        ifdp=ifdstack[level];
        de=destack[level];
      }
    /*
      Determine how many entries there are in the current IFD.
    */
    nde=Read16u(morder,ifdp);
    for (; de < nde; de++)
    {
      long
        n,
        t,
        f,
        c;

      char
        *pde,
        *pval;

      pde=(char *) (ifdp+2+(12*de));
      t=Read16u(morder,pde); /* get tag value */
      f=Read16u(morder,pde+2); /* get the format */
      if ((f-1) >= EXIF_NUM_FORMATS)
        break;
      c=(long) Read32u(morder,pde+4); /* get number of components */
      n=c*format_bytes[f];
      if (n <= 4)
        pval=pde+8;
      else
        {
          unsigned long
            oval;

          /*
            The directory entry contains an offset.
          */
          oval=Read32u(morder,pde+8);
          if ((oval+n) > length)
            continue;
          pval=(char *)(tiffp+oval);
        }
      if (all || (tag == t))
        {
          char
            s[MaxTextExtent];

          switch (f)
          {
            case EXIF_FMT_SBYTE:
            {
              FormatString(s,"%ld",(long) (*(char *) pval));
              value=AllocateString(s);
              break;
            }
            case EXIF_FMT_BYTE:
            {
              FormatString(s,"%ld",(long) (*(unsigned char *) pval));
              value=AllocateString(s);
              break;
            }
            case EXIF_FMT_SSHORT:
            {
              FormatString(s,"%hd",Read16u(morder,pval));
              value=AllocateString(s);
              break;
            }
            case EXIF_FMT_USHORT:
            {
              FormatString(s,"%hu",Read16s(morder,pval));
              value=AllocateString(s);
              break;
            }
            case EXIF_FMT_ULONG:
            {
              FormatString(s,"%lu",Read32u(morder,pval));
              value=AllocateString(s);
              break;
            }
            case EXIF_FMT_SLONG:
            {
              FormatString(s,"%ld",Read32s(morder,pval));
              value=AllocateString(s);
              break;
            }
            case EXIF_FMT_URATIONAL:
            {
              FormatString(s,"%ld/%ld",Read32u(morder,pval),
                Read32u(morder,4+(char *) pval));
              value=AllocateString(s);
              break;
            }
            case EXIF_FMT_SRATIONAL:
            {
              FormatString(s,"%ld/%ld",Read32s(morder,pval),
                Read32s(morder,4+(char *) pval));
              value=AllocateString(s);
              break;
            }
            case EXIF_FMT_SINGLE:
            {
              FormatString(s,"%f",(double) *(float *) pval);
              value=AllocateString(s);
              break;
            }
            case EXIF_FMT_DOUBLE:
            {
              FormatString(s,"%f",*(double *) pval);
              value=AllocateString(s);
              break;
            }
            default:
            case EXIF_FMT_UNDEFINED:
            case EXIF_FMT_STRING:
            {
              value=(char *) AcquireMemory(n+1);
              if (value != (char *) NULL)
                {
                  long
                    a;

                  for (a=0; a < n; a++)
                  {
                    value[a]='.';
                    if (isprint((int) pval[a]) || (pval[a] == '\0'))
                      value[a]=pval[a];
                  }
                  value[a]='\0';
                  break;
                }
              break;
            }
          }
          if (value != (char *) NULL)
            {
              int
                i;

              char
                *description;

              if (strlen(final) != 0)
                (void) ConcatenateString(&final,EXIF_DELIMITER);
              description=(char *) NULL;
              switch (all)
              {
                case 1:
                {
                  description="unknown";
                  for (i=0; ; i++)
                  {
                    if (tag_table[i].tag == 0)
                      break;
                    if (tag_table[i].tag == t)
                      {
                        description=tag_table[i].description;
                        break;
                      }
                  }
                  FormatString(s,"%.1024s=",description);
                  (void) ConcatenateString(&final,s);
                  break;
                }
                case 2:
                {
                  FormatString(s,"#%04lx=",t);
                  (void) ConcatenateString(&final,s);
                  break;
                }
              }
              (void) ConcatenateString(&final,value);
              LiberateMemory((void **) &value);
            }
        }
        if ((t == TAG_EXIF_OFFSET) || (t == TAG_INTEROP_OFFSET))
          {
            long
              offset;

            offset=(long) Read32u(morder,pval);
            if ((offset < (long) length) || (level < (DE_STACK_SIZE-2)))
              {
                /*
                  Push our current directory state onto the stack.
                */
                ifdstack[level]=ifdp;
                de++; /* bump to the next entry */
                destack[level]=de;
                level++;
                /*
                  Push new state onto of stack to cause a jump.
                */
                ifdstack[level]=tiffp+offset;
                destack[level]=0;
                level++;
              }
            break; /* break out of the for loop */
          }
    }
  } while (level > 0);
  if (strlen(final) == 0)
    (void) ConcatenateString(&final,"unknown");
  (void) SetImageAttribute(image,specification,(const char *) final);
  LiberateMemory((void **) &final);
  return(True);
}

MagickExport const ImageAttribute *GetImageAttribute(const Image *image,
  const char *key)
{
  register ImageAttribute
    *p;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (key == (char *) NULL)
    return(image->attributes);
  for (p=image->attributes; p != (ImageAttribute *) NULL; p=p->next)
    if (LocaleCompare(key,p->key) == 0)
      return(p);
  if (LocaleNCompare("IPTC:",key,5) == 0)
    {
      if (GenerateIPTCAttribute((Image *) image,key) == True)
        return(GetImageAttribute(image,key));
    }
  if (LocaleNCompare("8BIM:",key,5) == 0)
    {
      if (Generate8BIMAttribute((Image *) image,key) == True)
        return(GetImageAttribute(image,key));
    }
  if (LocaleNCompare("EXIF:",key,5) == 0)
    {
      if (GenerateEXIFAttribute((Image *) image,key) == True)
        return(GetImageAttribute(image,key));
    }
  return(p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e C l i p p i n g P a t h A t t r i b u t e                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetImageClippingPathAttribute searches the list of image attributes
%  and returns a pointer to a clipping path if it exists otherwise NULL.
%
%  The format of the GetImageClippingPathAttribute method is:
%
%      const ImageAttribute *GetImageClippingPathAttribute(const Image *image)
%
%  A description of each parameter follows:
%
%    o attribute:  Method GetImageClippingPathAttribute returns the clipping
%      path if it exists otherwise NULL.
%
%    o image: The image.
%
%
*/
MagickExport const ImageAttribute *GetImageClippingPathAttribute(
  const Image *image)
{
  return(GetImageAttribute(image,"8BIM:1999,2998"));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t I m a g e I n f o A t t r i b u t e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageInfoAttribute() returns a "fake" attribute based on data in the
%  image info or image structures.
%
%  The format of the GetImageInfoAttribute method is:
%
%      const ImageAttribute *GetImageAttribute(const ImageInfo *image_info,
%        const Image *image,const char *key)
%
%  A description of each parameter follows:
%
%    o attribute:  Method GetImageInfoAttribute returns the attribute if it
%      exists otherwise NULL.
%
%    o image_info: The imageInfo.
%
%    o image: The image.
%
%    o key:  These character strings are the name of an image attribute to
%      return.
%
*/
MagickExport const ImageAttribute *GetImageInfoAttribute(
  const ImageInfo *image_info,const Image *image,const char *key)
{
  char
    attribute[MaxTextExtent],
    filename[MaxTextExtent];

  attribute[0]='\0';
  switch(*(key))
  {
    case 'b':
    {
      if (LocaleNCompare("base",key,2) == 0)
        {
          GetPathComponent(image->magick_filename,BasePath,filename);
          (void) strncpy(attribute,filename,MaxTextExtent-1);
          break;
        }
      break;
    }
    case 'd':
    {
      if (LocaleNCompare("depth",key,2) == 0)
        {
          FormatString(attribute,"%lu",image->depth);
          break;
        }
      if (LocaleNCompare("directory",key,2) == 0)
        {
          GetPathComponent(image->magick_filename,HeadPath,filename);
          (void) strncpy(attribute,filename,MaxTextExtent-1);
          break;
        }
      break;
    }
    case 'e':
    {
      if (LocaleNCompare("extension",key,2) == 0)
        {
          GetPathComponent(image->magick_filename,ExtensionPath,filename);
          (void) strncpy(attribute,filename,MaxTextExtent-1);
          break;
        }
      break;
    }
    case 'g':
    {
      if (LocaleNCompare("group",key,2) == 0)
        {
          FormatString(attribute,"0x%lx",image_info->group);
          break;
        }
      break;
    }
    case 'h':
    {
      if (LocaleNCompare("height",key,2) == 0)
        {
          FormatString(attribute,"%lu",
            image->magick_rows ? image->magick_rows : 256L);
          break;
        }
      break;
    }
    case 'i':
    {
      if (LocaleNCompare("input",key,2) == 0)
        {
          (void) strncpy(attribute,image->filename,MaxTextExtent-1);
          break;
        }
      break;
    }
    case 'm':
    {
      if (LocaleNCompare("magick",key,2) == 0)
        {
          (void) strncpy(attribute,image->magick,MaxTextExtent-1);
          break;
        }
      break;
    }
    case 'n':
    {
      if (LocaleNCompare("name",key,2) == 0)
        {
          (void) strncpy(attribute,filename,MaxTextExtent-1);
          break;
        }
     break;
    }
    case 's':
    {
      if (LocaleNCompare("size",key,2) == 0)
        {
          if (GetBlobSize(image) >= (1 << 24))
            FormatString(attribute,"%lumb",
              (unsigned long) (GetBlobSize(image)/1024/1024));
          else
            if (GetBlobSize(image) >= (1 << 16))
              FormatString(attribute,"%lukb",
                (unsigned long) (GetBlobSize(image)/1024));
            else
              FormatString(attribute,"%lu",(unsigned long) GetBlobSize(image));
          break;
        }
      if (LocaleNCompare("scene",key,2) == 0)
        {
          FormatString(attribute,"%lu",image->scene);
          if (image_info->subrange != 0)
            FormatString(attribute,"%lu",image_info->subimage);
          break;
        }
      if (LocaleNCompare("scenes",key,6) == 0)
        {
          FormatString(attribute,"%lu",(unsigned long) GetImageListSize(image));
          break;
        }
       break;
    }
    case 'o':
    {
      if (LocaleNCompare("output",key,2) == 0)
        {
          (void) strncpy(attribute,image_info->filename,MaxTextExtent-1);
          break;
        }
     break;
    }
    case 'p':
    {
      if (LocaleNCompare("page",key,2) == 0)
        {
          register const Image
            *p;

          unsigned int
            page;

          p=image;
          for (page=1; p->previous != (Image *) NULL; page++)
            p=p->previous;
          FormatString(attribute,"%u",page);
          break;
        }
      break;
    }
    case 'u':
    {
      if (LocaleNCompare("unique",key,2) == 0)
        {
          (void) strncpy(filename,image_info->unique,MaxTextExtent-1);
          if (*filename == '\0')
            TemporaryFilename(filename);
          (void) strncpy(attribute,filename,MaxTextExtent-1);
          break;
        }
      break;
    }
    case 'w':
    {
      if (LocaleNCompare("width",key,2) == 0)
        {
          FormatString(attribute,"%lu",
            image->magick_columns ? image->magick_columns : 256L);
          break;
        }
      break;
    }
    case 'x':
    {
      if (LocaleNCompare("xresolution",key,2) == 0)
        {
          FormatString(attribute,"%g",image->x_resolution);
          break;
        }
      break;
    }
    case 'y':
    {
      if (LocaleNCompare("yresolution",key,2) == 0)
        {
          FormatString(attribute,"%g",image->y_resolution);
          break;
        }
      break;
    }
    case 'z':
    {
      if (LocaleNCompare("zero",key,2) == 0)
        {
          (void) strncpy(filename,image_info->zero,MaxTextExtent-1);
          if (*filename == '\0')
            TemporaryFilename(filename);
          (void) strncpy(attribute,filename,MaxTextExtent-1);
          break;
        }
      break;
    }
  }
  if (strlen(image->magick_filename) != 0)
    return(GetImageAttribute(image,key));
  return((ImageAttribute *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t I m a g e A t t r i b u t e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImageAttribute() searches the list of image attributes and replaces the
%  attribute value.  If it is not found in the list, the attribute name
%  and value is added to the list.   If the attribute exists in the list,
%  the value is concatenated to the attribute.  SetImageAttribute returns
%  True if the attribute is successfully concatenated or added to the list,
%  otherwise False.  If the value is NULL, the matching key is deleted
%  from the list.
%
%  The format of the SetImageAttribute method is:
%
%      unsigned int SetImageAttribute(Image *image,const char *key,
%        const char *value)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o key,value:  These character strings are the name and value of an image
%      attribute to replace or add to the list.
%
%
*/
MagickExport unsigned int SetImageAttribute(Image *image,const char *key,
  const char *value)
{
  ImageAttribute
    *attribute;

  register ImageAttribute
    *p;

  /*
    Initialize new attribute.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if ((key == (const char *) NULL) || (*key == '\0'))
    return(False);
  if (value == (const char *) NULL)
    {
      /*
        Delete attribute from the image attributes list.
      */
      for (p=image->attributes; p != (ImageAttribute *) NULL; p=p->next)
        if (LocaleCompare(key,p->key) == 0)
          break;
      if (p == (ImageAttribute *) NULL)
        return(False);
      if (p->key != (char *) NULL)
        LiberateMemory((void **) &p->key);
      if (p->value != (char *) NULL)
        LiberateMemory((void **) &p->value);
      if (p->previous != (ImageAttribute *) NULL)
        p->previous->next=p->next;
      else
        {
          image->attributes=p->next;
          if (p->next != (ImageAttribute *) NULL)
            p->next->previous=(ImageAttribute *) NULL;
        }
      if (p->next != (ImageAttribute *) NULL)
        p->next->previous=p->previous;
      attribute=p;
      LiberateMemory((void **) &attribute);
      return(True);
    }
  if (*value == '\0')
    return(False);
  attribute=(ImageAttribute *) AcquireMemory(sizeof(ImageAttribute));
  if (attribute == (ImageAttribute *) NULL)
    return(False);
  attribute->key=AllocateString(key);
  if ((LocaleNCompare(key,"EXIF",4) == 0) ||
      (LocaleNCompare(key,"IPTC",4) == 0) ||
      (LocaleNCompare(key,"[Locale",7) == 0) ||
      (LocaleNCompare(key,"8BIM",4) == 0))
    attribute->value=AllocateString(value);
  else
    attribute->value=TranslateText((ImageInfo *) NULL,image,value);
  attribute->compression=False;
  attribute->previous=(ImageAttribute *) NULL;
  attribute->next=(ImageAttribute *) NULL;
  if (image->attributes == (ImageAttribute *) NULL)
    {
      image->attributes=attribute;
      return(True);
    }
  for (p=image->attributes; p != (ImageAttribute *) NULL; p=p->next)
  {
    if (LocaleCompare(attribute->key,p->key) == 0)
      {
        (void) ConcatenateString(&p->value,attribute->value);
        LiberateMemory((void **) &attribute->value);
        LiberateMemory((void **) &attribute->key);
        LiberateMemory((void **) &attribute);
        return(True);
      }
    if (p->next == (ImageAttribute *) NULL)
      break;
  }
  /*
    Place new attribute at the end of the attribute list.
  */
  attribute->previous=p;
  p->next=attribute;
  return(True);
}
