/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            PPPP   DDDD   BBBB                               %
%                            P   P  D   D  B   B                              %
%                            PPPP   D   D  BBBB                               %
%                            P      D   D  B   B                              %
%                            P      DDDD   BBBB                               %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2000 ImageMagick Studio, a non-profit organization dedicated %
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
%  The software is provided "as is", without warranty of any kind, express or %
%  implied, including but not limited to the warranties of merchantability,   %
%  fitness for a particular purpose and noninfringement.  In no event shall   %
%  ImageMagick Studio be liable for any claim, damages or other liability,    %
%  whether in an action of contract, tort or otherwise, arising from, out of  %
%  or in connection with ImageMagick or the use or other dealings in          %
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
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "defines.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e c o d e I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DecodeImage unpacks the packed image pixels into runlength-encoded
%  pixel packets.
%
%  The format of the DecodeImage method is:
%
%      unsigned int DecodeImage(Image *image,unsigned char *pixels,
%        const unsigned int number_bytes)
%
%  A description of each parameter follows:
%
%    o status:  Method DecodeImage returns True if all the pixels are
%      uncompressed without error, otherwise False.
%
%    o image: The address of a structure of type Image.
%
%    o pixels:  The address of a byte (8 bits) array of pixel data created by
%      the decoding process.
%
%
*/
static unsigned int DecodeImage(Image *image,unsigned char *pixels,
  const unsigned int number_bytes)
{
  int
    count,
    pixel;

  register int
    i;

  register unsigned char
    *p;

  p=pixels;
  while (p < (pixels+number_bytes))
  {
    pixel=ReadByte(image);
    if (pixel <= 0x80)
      {
        count=pixel+1;
        for (i=0; i < count; i++)
          *p++=ReadByte(image);
        continue;
      }
    count=pixel+1-0x80;
    pixel=ReadByte(image);
    for (i=0; i < count; i++)
      *p++=pixel;
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s P D B                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsPDB returns True if the image format type, identified by the
%  magick string, is PDB.
%
%  The format of the ReadPDBImage method is:
%
%      unsigned int IsPDB(const unsigned char *magick,
%        const unsigned int length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsPDB returns True if the image format type is PDB.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsPDB(const unsigned char *magick,const unsigned int length)
{
  if (length < 68)
    return(False);
  if (LatinNCompare((char *) (magick+60),"vIMGView",8) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P D B I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPDBImage reads an Pilot image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadPDBImage method is:
%
%      Image *ReadPDBImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPDBImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadPDBImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  typedef struct _PDBHeader
  {
    char
      name[32];

    short int
      flags,
      version;

    long
      create_time,
      modify_time,
      archive_time,
      modify_number,
      application_info,
      sort_info,
      type,
      id,
      seed,
      next_record;

    short int
      number_records;
  } PDBHeader;

  typedef struct _PDBImage
  {
    char
      name[32],
      version,
      type;

    long
      reserved_1,
      note;

    short int
      x_last,
      y_last;

    long
      reserved_2;

    short int
      x_anchor,
      y_anchor,
      width,
      height;
  } PDBImage;

  char
    tag[3],
    record_type;

  Image
    *image;

  IndexPacket
    index;

  int
    y;

  long
    offset;

  PDBHeader
    pdb_header;

  PDBImage
    pdb_image;

  register IndexPacket
    *indexes;

  register int
    i,
    x;

  register PixelPacket
    *q;

  register unsigned char
    *p;

  unsigned char
    *pixels;

  unsigned int
    bits_per_pixel,
    packets,
    status;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Determine if this is a PDB image file.
  */
  status=ReadBlob(image,32,pdb_header.name);
  pdb_header.flags=MSBFirstReadShort(image);
  pdb_header.version=MSBFirstReadShort(image);
  pdb_header.create_time=MSBFirstReadLong(image);
  pdb_header.modify_time=MSBFirstReadLong(image);
  pdb_header.archive_time=MSBFirstReadLong(image);
  pdb_header.modify_number=MSBFirstReadLong(image);
  pdb_header.application_info=MSBFirstReadLong(image);
  pdb_header.sort_info=MSBFirstReadLong(image);
  pdb_header.type=MSBFirstReadLong(image);
  pdb_header.id=MSBFirstReadLong(image);
  pdb_header.seed=MSBFirstReadLong(image);
  pdb_header.next_record=MSBFirstReadLong(image);
  pdb_header.number_records=MSBFirstReadShort(image);
  if ((status == False) || (memcmp((char *) &pdb_header.type,"vIMG",4) != 0) ||
      (memcmp((char *) &pdb_header.id,"View",4) != 0))
    ThrowReaderException(CorruptImageWarning,"Not a PDB image file",image);
  /*
    Read record header.
  */
  offset=MSBFirstReadLong(image);
  (void) ReadBlob(image,3,tag);
  record_type=ReadByte(image);
  if (((record_type != 0x00) && (record_type != 0x01)) ||
      (memcmp(tag,"\x40\x6f\x80",3) != 0))
    ThrowReaderException(CorruptImageWarning,"Corrupt PDB image file",image);
  if ((offset-TellBlob(image)) == 6)
    {
      (void) ReadByte(image);
      (void) ReadByte(image);
    }
  if (pdb_header.number_records > 1)
    {
      offset=MSBFirstReadLong(image);
      (void) ReadBlob(image,3,tag);
      record_type=ReadByte(image);
      if (((record_type != 0x00) && (record_type != 0x01)) ||
          (memcmp(tag,"\x40\x6f\x80",3) != 0))
        ThrowReaderException(CorruptImageWarning,"Corrupt PDB image file",
          image);
      if ((offset-TellBlob(image)) == 6)
        {
          (void) ReadByte(image);
          (void) ReadByte(image);
        }
    }
  /*
    Read image header.
  */
  (void) ReadBlob(image,32,pdb_image.name);
  pdb_image.version=ReadByte(image);
  pdb_image.type=ReadByte(image);
  pdb_image.reserved_1=MSBFirstReadLong(image);
  pdb_image.note=MSBFirstReadLong(image);
  pdb_image.x_last=MSBFirstReadShort(image);
  pdb_image.y_last=MSBFirstReadShort(image);
  pdb_image.reserved_2=MSBFirstReadLong(image);
  pdb_image.x_anchor=MSBFirstReadShort(image);
  pdb_image.y_anchor=MSBFirstReadShort(image);
  pdb_image.width=MSBFirstReadShort(image);
  pdb_image.height=MSBFirstReadShort(image);
  /*
    Initialize image structure.
  */
  image->columns=pdb_image.width;
  image->rows=pdb_image.height;
  image->depth=8;
  image->class=PseudoClass;
  bits_per_pixel=pdb_image.type == 0 ? 2 : pdb_image.type == 2 ? 4 : 1;
  image->colors=1 << bits_per_pixel;
  image->colormap=(PixelPacket *)
    AllocateMemory(image->colors*sizeof(PixelPacket));
  if (image->colormap == (PixelPacket *) NULL)
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  for (i=0; i < (int) image->colors; i++)
  {
    image->colormap[i].red= ((unsigned long) (MaxRGB*i)/(image->colors-1));
    image->colormap[i].green= ((unsigned long) (MaxRGB*i)/(image->colors-1));
    image->colormap[i].blue= ((unsigned long) (MaxRGB*i)/(image->colors-1));
  }
  if (image_info->ping)
    {
      CloseBlob(image);
      return(image);
    }
  packets=(bits_per_pixel*image->columns/8)*image->rows;
  pixels=(unsigned char *) AllocateMemory(packets+256);
  if (pixels == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  if (pdb_image.version != 0)
    (void) DecodeImage(image,pixels,packets);
  else
    (void) ReadBlob(image,packets,(char *) pixels);
  p=pixels;
  switch (bits_per_pixel)
  {
    case 1:
    {
      int
        bit;

      /*
        Read 1-bit PDB image.
      */
      for (y=0; y < (int) image->rows; y++)
      {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        for (x=0; x < ((int) image->columns-7); x+=8)
        {
          for (bit=0; bit < 8; bit++)
          {
            index=(*p & (0x80 >> bit) ? 0x00 : 0x01);
            indexes[x+bit]=index;
            *q++=image->colormap[index];
          }
          p++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          ProgressMonitor(LoadImageText,y,image->rows);
      }
      break;
    }
    case 2:
    {
      /*
        Read 2-bit PDB image.
      */
      for (y=0; y < (int) image->rows; y++)
      {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        for (x=0; x < image->columns; x+=4)
        {
          index=3-((*p >> 6) & 0x03);
          indexes[x]=index;
          *q++=image->colormap[index];
          index=3-((*p >> 4) & 0x03);
          indexes[x+1]=index;
          *q++=image->colormap[index];
          index=3-((*p >> 2) & 0x03);
          indexes[x+2]=index;
          *q++=image->colormap[index];
          index=3-((*p) & 0x03);
          indexes[x+3]=index;
          *q++=image->colormap[index];
          p++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          ProgressMonitor(LoadImageText,y,image->rows);
      }
      break;
    }
    case 4:
    {
      /*
        Read 4-bit PDB image.
      */
      for (y=0; y < (int) image->rows; y++)
      {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        for (x=0; x < image->columns; x+=2)
        {
          index=15-((*p >> 4) & 0x0f);
          indexes[x]=index;
          *q++=image->colormap[index];
          index=15-((*p) & 0x0f);
          indexes[x+1]=index;
          *q++=image->colormap[index];
          p++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          ProgressMonitor(LoadImageText,y,image->rows);
      }
      break;
    }
    default:
      ThrowReaderException(CorruptImageWarning,"Not a PDB image file",image);
  }
  FreeMemory((void *) &pixels);
  if ((offset-TellBlob(image)) == 0)
    {
      char
        *comment;

      int
        c;

      register char
        *p;

      unsigned int
        length;

      /*
        Read comment.
      */
      c=ReadByte(image);
      length=MaxTextExtent;
      comment=(char *) AllocateMemory(length+1);
      p=comment;
      if (comment != (char *) NULL)
        for ( ; c != EOF; p++)
        {
          if ((p-comment) >= length)
            {
              length<<=1;
              length+=MaxTextExtent;
              comment=(char *) ReallocateMemory(comment,length+1);
              if (comment == (char *) NULL)
                break;
              p=comment+Extent(comment);
            }
          c=ReadByte(image);
          *p=c;
          *(p+1)='\0';
        }
      if (comment == (char *) NULL)
        ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
          image);
      (void) SetImageAttribute(image,"Comment",comment);
      FreeMemory((void *) &comment);
    }
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r P D B I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPDBImage adds attributes for the PDB image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPDBImage method is:
%
%      RegisterPDBImage(void)
%
*/
Export void RegisterPDBImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PDB");
  entry->decoder=ReadPDBImage;
  entry->magick=IsPDB;
  entry->description=AllocateString("Pilot Image Format");
  entry->module=AllocateString("PDB");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r P D B I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterPDBImage removes format registrations made by the
%  PDB module from the list of supported formats.
%
%  The format of the UnregisterPDBImage method is:
%
%      UnregisterPDBImage(void)
%
*/
Export void UnregisterPDBImage(void)
{
  UnregisterMagickInfo("PDB");
}
