/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            PPPP   DDDD   FFFFF                              %
%                            P   P  D   D  F                                  %
%                            PPPP   D   D  FFF                                %
%                            P      D   D  F                                  %
%                            P      DDDD   F                                  %
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
#if defined(HasTIFF)
#define CCITTParam  "-1"
#else
#define CCITTParam  "0"
#endif

/*
  Forward declarations.
*/
static unsigned int
  WritePDFImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s P D F                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsPDF returns True if the image format type, identified by the
%  magick string, is PDF.
%
%  The format of the IsPDF method is:
%
%      unsigned int IsPDF(const unsigned char *magick,
%        const unsigned int length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsPDF returns True if the image format type is PDF.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsPDF(const unsigned char *magick,const unsigned int length)
{
  if (length < 5)
    return(False);
  if (strncmp((char *) magick,"%PDF-",5) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P D F I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPDFImage reads a Portable Document Format image file and
%  returns it.  It allocates the memory necessary for the new Image structure
%  and returns a pointer to the new image.
%
%  The format of the ReadPDFImage method is:
%
%      Image *ReadPDFImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPDFImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadPDFImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
#define MediaBox  "/MediaBox ["

  char
    density[MaxTextExtent],
    command[MaxTextExtent],
    filename[MaxTextExtent],
    geometry[MaxTextExtent],
    options[MaxTextExtent],
    postscript_filename[MaxTextExtent];

  DelegateInfo
    delegate_info;

  double
    dx_resolution,
    dy_resolution;

  FILE
    *file;

  Image
    *image,
    *next_image;

  ImageInfo
    *clone_info;

  int
    count,
    status;

  long int
    filesize;

  RectangleInfo
    box,
    page;

  register char
    *p;

  register int
    c;

  SegmentInfo
    bounding_box;

  unsigned int
    height,
    portrait,
    width;

  if (image_info->monochrome)
    {
      if (!GetDelegateInfo("gs-mono",(char *) NULL,&delegate_info))
        return((Image *) NULL);
    }
  else
    if (!GetDelegateInfo("gs-color",(char *) NULL,&delegate_info))
      return((Image *) NULL);
  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Open temporary output file.
  */
  TemporaryFilename(postscript_filename);
  file=fopen(postscript_filename,WriteBinaryType);
  if (file == (FILE *) NULL)
    ThrowReaderException(FileOpenWarning,"Unable to write file",image);
  /*
    Set the page geometry.
  */
  dx_resolution=72.0;
  dy_resolution=72.0;
  if ((image->x_resolution == 0.0) || (image->y_resolution == 0.0))
    {
     (void) strcpy(density,PSDensityGeometry);
      count=sscanf(density,"%lfx%lf",&image->x_resolution,&image->y_resolution);
      if (count != 2)
        image->y_resolution=image->x_resolution;
    }
  FormatString(density,"%gx%g",image->x_resolution,image->y_resolution);
  page.width=612;
  page.height=792;
  page.x=0;
  page.y=0;
  (void) ParseImageGeometry(PSPageGeometry,&page.x,&page.y,
    &page.width,&page.height);
  portrait=True;
  /*
    Determine page geometry from the PDF media box.
  */
  box.width=0;
  box.height=0;
  for (p=command; ; )
  {
    c=ReadByte(image);
    if (c == EOF)
      break;
    (void) fputc(c,file);
    *p++=c;
    if ((c != '\n') && (c != '\r') && ((p-command) < (MaxTextExtent-1)))
      continue;
    *p='\0';
    p=command;
    /*
      Continue unless this is a MediaBox statement.
    */
    if (strncmp(command,"/Rotate 90",10) == 0)
      portrait=False;
    if (strncmp(MediaBox,command,Extent(MediaBox)) != 0)
      continue;
    count=sscanf(command,"/MediaBox [ %lf %lf %lf %lf",&bounding_box.x1,
      &bounding_box.y1,&bounding_box.x2,&bounding_box.y2);
    if (count != 4)
      continue;
    if ((bounding_box.x1 > bounding_box.x2) ||
        (bounding_box.y1 > bounding_box.y2))
      continue;
    /*
      Set Postscript render geometry.
    */
    width=(unsigned int) (bounding_box.x2-bounding_box.x1);
    if ((float) ((int) bounding_box.x2) != bounding_box.x2)
      width++;
    height=(unsigned int) (bounding_box.y2-bounding_box.y1);
    if ((float) ((int) bounding_box.y2) != bounding_box.y2)
      height++;
    if ((width <= box.width) && (height <= box.height))
      continue;
    page.width=width;
    page.height=height;
    box=page;
  }
  if (image_info->page != (char *) NULL)
    (void) ParseImageGeometry(image_info->page,&page.x,&page.y,
      &page.width,&page.height);
  FormatString(geometry,"%ux%u",
    (unsigned int) ((page.width*image->x_resolution+0.5)/dx_resolution),
    (unsigned int) ((page.height*image->y_resolution+0.5)/dy_resolution));
  if (ferror(file))
    {
      (void) fclose(file);
      ThrowReaderException(FileOpenWarning,
        "An error has occurred writing to file",image);
    }
  (void) fclose(file);
  CloseBlob(image);
  filesize=image->filesize;
  DestroyImage(image);
  /*
    Use Ghostscript to convert Postscript image.
  */
  *options='\0';
  if (image_info->subrange != 0)
    FormatString(options,"-dFirstPage=%u -dLastPage=%u",
      image_info->subimage+1,image_info->subimage+image_info->subrange);
  (void) strcpy(filename,image_info->filename);
  TemporaryFilename((char *) image_info->filename);
  FormatString(command,delegate_info.commands,image_info->antialias ? 4 : 1,
    image_info->antialias ? 4 : 1,geometry,density,options,image_info->filename,
    postscript_filename);
  ProgressMonitor(RenderPostscriptText,0,8);
  status=SystemCommand(image_info->verbose,command);
  ProgressMonitor(RenderPostscriptText,7,8);
  if (status)
    {
      (void) remove(postscript_filename);
      ThrowReaderException(FileOpenWarning,"Portable Document delegate failed",
        image);
    }
  clone_info=CloneImageInfo(image_info);
  GetBlobInfo(&clone_info->blob);
  image=ReadImage(clone_info,exception);
  DestroyImageInfo(clone_info);
  (void) remove(postscript_filename);
  (void) remove(image_info->filename);
  if (image == (Image *) NULL)
    ThrowReaderException(CorruptImageWarning,
      "Portable Document delegate failed",image);
  (void) strcpy((char *) image_info->filename,filename);
  do
  {
    (void) strcpy(image->magick,"PDF");
    (void) strcpy(filename,image_info->filename);
    image->filesize=filesize;
    if (!portrait)
      {
        Image
          *rotated_image;

        /*
          Rotate image.
        */
        image->orphan=True;
        rotated_image=RotateImage(image,90,exception);
        if (rotated_image != (Image *) NULL)
          {
            DestroyImage(image);
            image=rotated_image;
          }
      }
    next_image=image->next;
    if (next_image != (Image *) NULL)
      image=next_image;
  } while (next_image != (Image *) NULL);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r P D F I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPDFImage adds attributes for the PDF image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPDFImage method is:
%
%      RegisterPDFImage(void)
%
*/
Export void RegisterPDFImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("EPDF");
  entry->decoder=ReadPDFImage;
  entry->encoder=WritePDFImage;
  entry->adjoin=False;
  entry->description=
    AllocateString("Encapsulated Portable Document Format");
  RegisterMagickInfo(entry);
  entry=SetMagickInfo("PDF");
  entry->decoder=ReadPDFImage;
  entry->encoder=WritePDFImage;
  entry->magick=IsPDF;
  entry->description=AllocateString("Portable Document Format");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P D F I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePDFImage writes an image in the Portable Document image
%  format.
%
%  The format of the WritePDFImage method is:
%
%      unsigned int WritePDFImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePDFImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WritePDFImage(const ImageInfo *image_info,Image *image)
{
#define CFormat  "/Filter [ /ASCII85Decode /%.1024s ]\n"
#define ObjectsPerImage  12

  char
    buffer[MaxTextExtent],
    date[MaxTextExtent],
    density[MaxTextExtent],
    geometry[MaxTextExtent],
    **labels;

  CompressionType
    compression;

  double
    dx_resolution,
    dy_resolution,
    x_resolution,
    x_scale,
    y_resolution,
    y_scale;

  ImageAttribute
    *attribute;

  int
    count,
    status,
    x,
    y;

  Image
    encode_image,
    *tile_image;

  RectangleInfo
    media_info;

  register IndexPacket
    *indexes;

  register PixelPacket
    *p;

  register unsigned char
    *q;

  register int
    i;

  time_t
    timer;

  unsigned char
    *pixels;

  unsigned int
    height,
    info_id,
    object,
    pages_id,
    root_id,
    scene,
    text_size,
    width;

  unsigned long
    length,
    number_packets,
    *xref;

  /*
    Open output image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  if ((image->file == stdout) || image->pipe)
    {
      /*
        Write standard output or pipe to temporary file.
      */
      encode_image=(*image);
      TemporaryFilename(image->filename);
      image->temporary=True;
      status=OpenBlob(image_info,image,WriteBinaryType);
      if (status == False)
        ThrowWriterException(FileOpenWarning,"Unable to open file",image);
    }
  compression=image->compression;
  if (image_info->compression != UndefinedCompression)
    compression=image_info->compression;
  /*
    Allocate X ref memory.
  */
  xref=(unsigned long *) AllocateMemory(2048*sizeof(unsigned long));
  if (xref == (unsigned long *) NULL)
    ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Write Info object.
  */
  object=0;
  (void) strcpy(buffer,"%PDF-1.2 \n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  xref[object++]=TellBlob(image);
  info_id=object;
  (void) sprintf(buffer,"%u 0 obj\n",object);
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) strcpy(buffer,"<<\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  timer=time((time_t *) NULL);
  (void) localtime(&timer);
  (void) strcpy(date,ctime(&timer));
  date[Extent(date)-1]='\0';
  (void) sprintf(buffer,"/CreationDate (%.1024s)\n",date);
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) sprintf(buffer,"/Producer (%.1024s)\n",MagickVersion);
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) strcpy(buffer,">>\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) strcpy(buffer,"endobj\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  /*
    Write Catalog object.
  */
  xref[object++]=TellBlob(image);
  root_id=object;
  (void) sprintf(buffer,"%u 0 obj\n",object);
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) strcpy(buffer,"<<\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) strcpy(buffer,"/Type /Catalog\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) sprintf(buffer,"/Pages %u 0 R\n",object+1);
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) strcpy(buffer,">>\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) strcpy(buffer,"endobj\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  /*
    Write Pages object.
  */
  xref[object++]=TellBlob(image);
  pages_id=object;
  (void) sprintf(buffer,"%u 0 obj\n",object);
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) strcpy(buffer,"<<\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) strcpy(buffer,"/Type /Pages\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) sprintf(buffer,"/Kids [ %u 0 R ",object+1);
  (void) WriteBlob(image,strlen(buffer),buffer);
  count=pages_id+ObjectsPerImage+1;
  if (image_info->adjoin)
    {
      Image
        *kid_image;

      /*
        Predict page object id's.
      */
      kid_image=image;
      for ( ; kid_image->next != (Image *) NULL; count+=ObjectsPerImage)
      {
        (void) sprintf(buffer,"%d 0 R ",count);
        (void) WriteBlob(image,strlen(buffer),buffer);
        kid_image=kid_image->next;
      }
      xref=(unsigned long *)
        ReallocateMemory((char *) xref,(count+2048)*sizeof(unsigned long));
      if (xref == (unsigned long *) NULL)
        ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",
          image);
    }
  (void) strcpy(buffer,"]\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) sprintf(buffer,"/Count %u\n",(count-pages_id)/ObjectsPerImage);
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) strcpy(buffer,">>\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) strcpy(buffer,"endobj\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  scene=0;
  do
  {
    /*
      Scale image to size of Portable Document page.
    */
    text_size=0;
    attribute=GetImageAttribute(image,"Label");
    if (attribute != (ImageAttribute *) NULL)
      text_size=MultilineCensus(attribute->value)*image_info->pointsize+12;
    width=image->columns;
    height=image->rows;
    x=0;
    y=text_size;
    FormatString(geometry,"%ux%u",image->columns,image->rows);
    if (image_info->page != (char *) NULL)
      (void) strcpy(geometry,image_info->page);
    else
      if ((image->page.width != 0) && (image->page.height != 0))
        (void) FormatString(geometry,"%ux%u%+d%+d",image->page.width,
          image->page.height,image->page.x,image->page.y);
      else
        if (Latin1Compare(image_info->magick,"PDF") == 0)
          (void) strcpy(geometry,PSPageGeometry);
    (void) ParseImageGeometry(geometry,&x,&y,&width,&height);
    (void) GetGeometry(geometry,&media_info.x,&media_info.y,
      &media_info.width,&media_info.height);
    /*
      Scale relative to dots-per-inch.
    */
    dx_resolution=72.0;
    dy_resolution=72.0;
    x_resolution=72.0;
    (void) strcpy(density,PSDensityGeometry);
    count=sscanf(density,"%lfx%lf",&x_resolution,&y_resolution);
    if (count != 2)
      y_resolution=x_resolution;
    if (image_info->density != (char *) NULL)
      {
        count=sscanf(image_info->density,"%lfx%lf",&x_resolution,&y_resolution);
        if (count != 2)
          y_resolution=x_resolution;
      }
    x_scale=(width*dx_resolution)/x_resolution;
    width=(unsigned int) (x_scale+0.5);
    y_scale=(height*dy_resolution)/y_resolution;
    height=(unsigned int) (y_scale+0.5);
    /*
      Write Page object.
    */
    xref[object++]=TellBlob(image);
    (void) sprintf(buffer,"%u 0 obj\n",object);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,"<<\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,"/Type /Page\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) sprintf(buffer,"/Parent %u 0 R\n",pages_id);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,"/Resources <<\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) sprintf(buffer,"/Font << /F%u %u 0 R >>\n",image->scene,
      object+4);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) sprintf(buffer,"/XObject << /Im%u %u 0 R >>\n",image->scene,
      object+5);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) sprintf(buffer,"/ProcSet %u 0 R >>\n",object+3);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) sprintf(buffer,"/MediaBox [ %d %d %d %d ]\n",0,0,
      media_info.width,media_info.height);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) sprintf(buffer,"/Contents %u 0 R\n",object+1);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) sprintf(buffer,"/Thumb %u 0 R\n",object+8);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,">>\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,"endobj\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    /*
      Write Contents object.
    */
    xref[object++]=TellBlob(image);
    (void) sprintf(buffer,"%u 0 obj\n",object);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,"<<\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) sprintf(buffer,"/Length %u 0 R\n",object+1);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,">>\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,"stream\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    length=TellBlob(image);
    (void) strcpy(buffer,"q\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    labels=(char **) NULL;
    attribute=GetImageAttribute(image,"Label");
    if (attribute != (ImageAttribute *) NULL)
      labels=StringToList(attribute->value);
    if (labels != (char **) NULL)
      {
        for (i=0; labels[i] != (char *) NULL; i++)
        {
          (void) strcpy(buffer,"BT\n");
          (void) WriteBlob(image,strlen(buffer),buffer);
          (void) sprintf(buffer,"/F%u %f Tf\n",image->scene,
            image_info->pointsize);
          (void) WriteBlob(image,strlen(buffer),buffer);
          (void) sprintf(buffer,"%d %f Td\n",x,y+height+
            i*image_info->pointsize+12);
          (void) WriteBlob(image,strlen(buffer),buffer);
          (void) sprintf(buffer,"(%.1024s) Tj\n",labels[i]);
          (void) WriteBlob(image,strlen(buffer),buffer);
          (void) strcpy(buffer,"ET\n");
          (void) WriteBlob(image,strlen(buffer),buffer);
          FreeMemory(labels[i]);
        }
        FreeMemory(labels);
      }
    (void) sprintf(buffer,"%g 0 0 %g %d %d cm\n",x_scale,y_scale,x,y);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) sprintf(buffer,"/Im%u Do\n",image->scene);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,"Q\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    length=TellBlob(image)-length;
    (void) strcpy(buffer,"endstream\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,"endobj\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    /*
      Write Length object.
    */
    xref[object++]=TellBlob(image);
    (void) sprintf(buffer,"%u 0 obj\n",object);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) sprintf(buffer,"%lu\n",length);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,"endobj\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    /*
      Write Procset object.
    */
    xref[object++]=TellBlob(image);
    (void) sprintf(buffer,"%u 0 obj\n",object);
    (void) WriteBlob(image,strlen(buffer),buffer);
    if (!IsPseudoClass(image) && !IsGrayImage(image))
      (void) strcpy(buffer,"[ /PDF /Text /ImageC");
    else
      if (IsFaxImage(image))
        (void) strcpy(buffer,"[ /PDF /Text /ImageB");
      else
        (void) strcpy(buffer,"[ /PDF /Text /ImageI");
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer," ]\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,"endobj\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    /*
      Write Font object.
    */
    xref[object++]=TellBlob(image);
    (void) sprintf(buffer,"%u 0 obj\n",object);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,"<<\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,"/Type /Font\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,"/Subtype /Type1\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) sprintf(buffer,"/Name /F%u\n",image->scene);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,"/BaseFont /Helvetica\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,"/Encoding /MacRomanEncoding\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,">>\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,"endobj\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    /*
      Write XObject object.
    */
    xref[object++]=TellBlob(image);
    (void) sprintf(buffer,"%u 0 obj\n",object);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,"<<\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,"/Type /XObject\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,"/Subtype /Image\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) sprintf(buffer,"/Name /Im%u\n",image->scene);
    (void) WriteBlob(image,strlen(buffer),buffer);
    if (compression == NoCompression)
      (void) strcpy(buffer,"/Filter /ASCII85Decode\n");
    else
      if (!IsFaxImage(image))
        {
          switch (compression)
          {
            case JPEGCompression: sprintf(buffer,CFormat,"DCTDecode"); break;
            case LZWCompression: sprintf(buffer,CFormat,"LZWDecode"); break;
            case ZipCompression: sprintf(buffer,CFormat,"FlateDecode"); break;
            default: sprintf(buffer,CFormat,"RunLengthDecode"); break;
          }
          (void) WriteBlob(image,strlen(buffer),buffer);
        }
      else
        {
          (void) strcpy(buffer,
            "/Filter [ /ASCII85Decode /CCITTFaxDecode ]\n");
          (void) WriteBlob(image,strlen(buffer),buffer);
          (void) sprintf(buffer,
            "/DecodeParms [ << >> << /K %.1024s /Columns %d /Rows %d >> ]\n",
            CCITTParam,image->columns,image->rows);
          (void) WriteBlob(image,strlen(buffer),buffer);
        }
    (void) sprintf(buffer,"/Width %u\n",image->columns);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) sprintf(buffer,"/Height %u\n",image->rows);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) sprintf(buffer,"/ColorSpace %u 0 R\n",object+2);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) sprintf(buffer,"/BitsPerComponent %d\n",
      IsFaxImage(image) ? 1 : 8);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) sprintf(buffer,"/Length %u 0 R\n",object+1);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,">>\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,"stream\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    length=TellBlob(image);
    if (!IsPseudoClass(image) && !IsGrayImage(image))
      switch (compression)
      {
        case JPEGCompression:
        {
          char
            filename[MaxTextExtent];

          FILE
            *file;

          Image
            *jpeg_image;

          int
            c;

          /*
            Write image to temporary file in JPEG format.
          */
          TemporaryFilename(filename);
          jpeg_image=
            CloneImage(image,image->columns,image->rows,True,&image->exception);
          if (jpeg_image == (Image *) NULL)
            ThrowWriterException(DelegateWarning,"Unable to clone image",image);
          (void) FormatString(jpeg_image->filename,"jpeg:%s",filename);
          status=WriteImage(image_info,jpeg_image);
          DestroyImage(jpeg_image);
          if (status == False)
            ThrowWriterException(DelegateWarning,"Unable to write image",image);
          file=fopen(filename,ReadBinaryType);
          if (file == (FILE *) NULL)
            ThrowWriterException(FileOpenWarning,"Unable to open file",image);
          Ascii85Initialize();
          for (c=fgetc(file); c != EOF; c=fgetc(file))
            Ascii85Encode(image,c);
          Ascii85Flush(image);
          (void) fclose(file);
          (void) remove(filename);
          break;
        }
        case RunlengthEncodedCompression:
        default:
        {
          /*
            Allocate pixel array.
          */
          number_packets=(image->colorspace == CMYKColorspace ? 4 : 3)*
            image->columns*image->rows;
          pixels=(unsigned char *) AllocateMemory(number_packets);
          if (pixels == (unsigned char *) NULL)
            ThrowWriterException(ResourceLimitWarning,
              "Memory allocation failed",image);
          /*
            Dump runlength encoded pixels.
          */
          q=pixels;
          for (y=0; y < (int) image->rows; y++)
          {
            p=GetPixelCache(image,0,y,image->columns,1);
            if (p == (PixelPacket *) NULL)
              break;
            for (x=0; x < (int) image->columns; x++)
            {
              if (image->matte && (p->opacity == Transparent))
                {
                  *q++=DownScale(MaxRGB);
                  *q++=DownScale(MaxRGB);
                  *q++=DownScale(MaxRGB);
                }
              else
                if (image->colorspace != CMYKColorspace)
                  {
                    *q++=DownScale(p->red);
                    *q++=DownScale(p->green);
                    *q++=DownScale(p->blue);
                  }
                else
                  {
                    *q++=DownScale(p->red);
                    *q++=DownScale(p->green);
                    *q++=DownScale(p->blue);
                    *q++=DownScale(p->opacity);
                  }
              p++;
            }
            if (image->previous == (Image *) NULL)
              if (QuantumTick(y,image->rows))
                ProgressMonitor(SaveImageText,y,image->rows);
          }
          if (compression == ZipCompression)
            status=
              ZLIBEncodeImage(image,number_packets,image_info->quality,pixels);
          else
            if (compression == LZWCompression)
              status=LZWEncodeImage(image,number_packets,pixels);
            else
              status=PackbitsEncodeImage(image,number_packets,pixels);
          FreeMemory(pixels);
          if (!status)
            {
              CloseBlob(image);
              return(False);
            }
          break;
        }
        case NoCompression:
        {
          /*
            Dump uncompressed DirectColor packets.
          */
          Ascii85Initialize();
          for (y=0; y < (int) image->rows; y++)
          {
            p=GetPixelCache(image,0,y,image->columns,1);
            if (p == (PixelPacket *) NULL)
              break;
            for (x=0; x < (int) image->columns; x++)
            {
              if (image->matte && (p->opacity == Transparent))
                {
                  Ascii85Encode(image,DownScale(MaxRGB));
                  Ascii85Encode(image,DownScale(MaxRGB));
                  Ascii85Encode(image,DownScale(MaxRGB));
                }
              else
                if (image->colorspace != CMYKColorspace)
                  {
                    Ascii85Encode(image,DownScale(p->red));
                    Ascii85Encode(image,DownScale(p->green));
                    Ascii85Encode(image,DownScale(p->blue));
                  }
                else
                  {
                    Ascii85Encode(image,DownScale(p->red));
                    Ascii85Encode(image,DownScale(p->green));
                    Ascii85Encode(image,DownScale(p->blue));
                    Ascii85Encode(image,DownScale(p->opacity));
                  }
              p++;
            }
            if (image->previous == (Image *) NULL)
              if (QuantumTick(y,image->rows))
                ProgressMonitor(SaveImageText,y,image->rows);
          }
          Ascii85Flush(image);
          break;
        }
      }
    else
      if (IsFaxImage(image))
        {
          unsigned char
            bit,
            byte,
            polarity;

          polarity=Intensity(image->colormap[0]) > (MaxRGB >> 1);
          if (image->colors == 2)
            polarity=
              Intensity(image->colormap[0]) < Intensity(image->colormap[1]);
          switch (compression)
          {
            case RunlengthEncodedCompression:
            default:
            {
              if (Latin1Compare(CCITTParam,"0") == 0)
                (void) HuffmanEncodeImage((ImageInfo *) image_info,image);
              else
                (void) Huffman2DEncodeImage((ImageInfo *) image_info,image);
              break;
            }
            case NoCompression:
            {
              /*
                Dump uncompressed PseudoColor packets.
              */
              Ascii85Initialize();
              for (y=0; y < (int) image->rows; y++)
              {
                p=GetPixelCache(image,0,y,image->columns,1);
                if (p == (PixelPacket *) NULL)
                  break;
                indexes=GetIndexes(image);
                bit=0;
                byte=0;
                for (x=0; x < (int) image->columns; x++)
                {
                  byte<<=1;
                  if (indexes[x] == polarity)
                    byte|=0x01;
                  bit++;
                  if (bit == 8)
                    {
                      Ascii85Encode(image,byte);
                      bit=0;
                      byte=0;
                    }
                  p++;
                }
                if (bit != 0)
                  Ascii85Encode(image,byte << (8-bit));
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    ProgressMonitor(SaveImageText,y,image->rows);
              }
              Ascii85Flush(image);
              break;
            }
          }
        }
      else
        {
          /*
            Dump number of colors and colormap.
          */
          switch (compression)
          {
            case RunlengthEncodedCompression:
            default:
            {
              /*
                Allocate pixel array.
              */
              number_packets=image->columns*image->rows;
              pixels=(unsigned char *) AllocateMemory(number_packets);
              if (pixels == (unsigned char *) NULL)
                ThrowWriterException(ResourceLimitWarning,
                  "Memory allocation failed",image);
              /*
                Dump Runlength encoded pixels.
              */
              q=pixels;
              for (y=0; y < (int) image->rows; y++)
              {
                p=GetPixelCache(image,0,y,image->columns,1);
                if (p == (PixelPacket *) NULL)
                  break;
                indexes=GetIndexes(image);
                for (x=0; x < (int) image->columns; x++)
                {
                  *q++=indexes[x];
                  p++;
                }
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    ProgressMonitor(SaveImageText,y,image->rows);
              }
              if (compression == ZipCompression)
                status=ZLIBEncodeImage(image,number_packets,image_info->quality,
                  pixels);
              else
                if (compression == LZWCompression)
                  status=LZWEncodeImage(image,number_packets,pixels);
                else
                  status=PackbitsEncodeImage(image,number_packets,pixels);
              FreeMemory(pixels);
              if (!status)
                {
                  CloseBlob(image);
                  return(False);
                }
              break;
            }
            case NoCompression:
            {
              /*
                Dump uncompressed PseudoColor packets.
              */
              Ascii85Initialize();
              for (y=0; y < (int) image->rows; y++)
              {
                p=GetPixelCache(image,0,y,image->columns,1);
                if (p == (PixelPacket *) NULL)
                  break;
                indexes=GetIndexes(image);
                for (x=0; x < (int) image->columns; x++)
                {
                  Ascii85Encode(image,indexes[x]);
                  p++;
                }
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    ProgressMonitor(SaveImageText,y,image->rows);
              }
              Ascii85Flush(image);
              break;
            }
          }
        }
    length=TellBlob(image)-length;
    (void) strcpy(buffer,"\nendstream\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,"endobj\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    /*
      Write Length object.
    */
    xref[object++]=TellBlob(image);
    (void) sprintf(buffer,"%u 0 obj\n",object);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) sprintf(buffer,"%lu\n",length);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,"endobj\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    /*
      Write Colorspace object.
    */
    xref[object++]=TellBlob(image);
    (void) sprintf(buffer,"%u 0 obj\n",object);
    (void) WriteBlob(image,strlen(buffer),buffer);
    if (image->colorspace == CMYKColorspace)
      (void) strcpy(buffer,"/DeviceCMYK\n");
    else
      if (!IsPseudoClass(image) && !IsGrayImage(image))
        (void) strcpy(buffer,"/DeviceRGB\n");
      else
        if (IsFaxImage(image))
          (void) strcpy(buffer,"/DeviceGray\n");
        else
          (void) sprintf(buffer,"[ /Indexed /DeviceRGB %u %u 0 R ]\n",
            image->colors-1,object+3);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,"endobj\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    /*
      Write Thumb object.
    */
    width=image->columns;
    height=image->rows;
    x=0;
    y=0;
    (void) ParseImageGeometry("106x106+0+0>",&x,&y,&width,&height);
    image->orphan=True;
    if (image->class == PseudoClass)
      tile_image=SampleImage(image,width,height,&image->exception);
    else
      tile_image=ZoomImage(image,width,height,&image->exception);
    if (tile_image == (Image *) NULL)
      ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",
        image);
    xref[object++]=TellBlob(image);
    (void) sprintf(buffer,"%u 0 obj\n",object);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,"<<\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    if (compression == NoCompression)
      (void) strcpy(buffer,"/Filter /ASCII85Decode\n");
    else
      if (!IsFaxImage(image))
        {
          (void) sprintf(buffer,"/Filter [ /ASCII85Decode /%.1024s ]\n",
            compression == ZipCompression ? "FlateDecode" :
            compression == LZWCompression ? "LZWDecode" : "RunLengthDecode");
          (void) WriteBlob(image,strlen(buffer),buffer);
        }
      else
        {
          (void) strcpy(buffer,
            "/Filter [ /ASCII85Decode /CCITTFaxDecode ]\n");
          (void) WriteBlob(image,strlen(buffer),buffer);
          (void) sprintf(buffer,
            "/DecodeParms [ << >> << /Columns %d /Rows %d >> ]\n",
            tile_image->columns,tile_image->rows);
          (void) WriteBlob(image,strlen(buffer),buffer);
        }
    (void) sprintf(buffer,"/Width %u\n",tile_image->columns);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) sprintf(buffer,"/Height %u\n",tile_image->rows);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) sprintf(buffer,"/ColorSpace %u 0 R\n",object-1);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) sprintf(buffer,"/BitsPerComponent %d\n",
      IsFaxImage(tile_image) ? 1 : 8);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) sprintf(buffer,"/Length %u 0 R\n",object+1);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,">>\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,"stream\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    length=TellBlob(image);
    if (!IsPseudoClass(tile_image) && !IsGrayImage(tile_image))
      switch (compression)
      {
        case RunlengthEncodedCompression:
        default:
        {
          /*
            Allocate pixel array.
          */
          number_packets=(image->colorspace == CMYKColorspace ? 4 : 3)*
            image->columns*image->rows;
          pixels=(unsigned char *) AllocateMemory(number_packets);
          if (pixels == (unsigned char *) NULL)
            {
              DestroyImage(tile_image);
              ThrowWriterException(ResourceLimitWarning,
                "Memory allocation failed",image);
            }
          /*
            Dump runlength encoded pixels.
          */
          q=pixels;
          for (y=0; y < (int) tile_image->rows; y++)
          {
            p=GetPixelCache(tile_image,0,y,tile_image->columns,1);
            if (p == (PixelPacket *) NULL)
              break;
            for (x=0; x < (int) tile_image->columns; x++)
            {
              if (tile_image->matte && (p->opacity == Transparent))
                {
                  *q++=DownScale(MaxRGB);
                  *q++=DownScale(MaxRGB);
                  *q++=DownScale(MaxRGB);
                }
              else
                {
                  *q++=DownScale(p->red);
                  *q++=DownScale(p->green);
                  *q++=DownScale(p->blue);
                  if (image->colorspace == CMYKColorspace)
                    *q++=DownScale(p->opacity);
                }
              p++;
            }
          }
          if (compression == ZipCompression)
            status=
              ZLIBEncodeImage(image,number_packets,image_info->quality,pixels);
          else
            if (compression == LZWCompression)
              status=LZWEncodeImage(image,number_packets,pixels);
            else
              status=PackbitsEncodeImage(image,number_packets,pixels);
          FreeMemory(pixels);
          if (!status)
            {
              CloseBlob(image);
              return(False);
            }
          break;
        }
        case NoCompression:
        {
          /*
            Dump uncompressed DirectColor packets.
          */
          Ascii85Initialize();
          for (y=0; y < (int) tile_image->rows; y++)
          {
            p=GetPixelCache(tile_image,0,y,tile_image->columns,1);
            if (p == (PixelPacket *) NULL)
              break;
            for (x=0; x < (int) tile_image->columns; x++)
            {
              if (tile_image->matte && (p->opacity == Transparent))
                {
                  Ascii85Encode(image,DownScale(MaxRGB));
                  Ascii85Encode(image,DownScale(MaxRGB));
                  Ascii85Encode(image,DownScale(MaxRGB));
                }
              else
                {
                  Ascii85Encode(image,DownScale(p->red));
                  Ascii85Encode(image,DownScale(p->green));
                  Ascii85Encode(image,DownScale(p->blue));
                  if (image->colorspace == CMYKColorspace)
                    Ascii85Encode(image,DownScale(p->opacity));
                }
              p++;
            }
          }
          Ascii85Flush(image);
          break;
        }
      }
    else
      if (IsFaxImage(tile_image))
        {
          register unsigned char
            bit,
            byte,
            polarity;

          polarity=Intensity(tile_image->colormap[0]) > (MaxRGB >> 1);
          if (image->colors == 2)
            polarity=Intensity(tile_image->colormap[0]) <
              Intensity(tile_image->colormap[1]);
          switch (compression)
          {
            case RunlengthEncodedCompression:
            default:
            {
              /*
                Allocate pixel array.
              */
              number_packets=((tile_image->columns+7) >> 3)*tile_image->rows;
              pixels=(unsigned char *) AllocateMemory(number_packets);
              if (pixels == (unsigned char *) NULL)
                {
                  DestroyImage(tile_image);
                  ThrowWriterException(ResourceLimitWarning,
                    "Memory allocation failed",image);
                }
              /*
                Dump Runlength encoded pixels.
              */
              q=pixels;
              for (y=0; y < (int) tile_image->rows; y++)
              {
                p=GetPixelCache(tile_image,0,y,tile_image->columns,1);
                if (p == (PixelPacket *) NULL)
                  break;
                indexes=GetIndexes(tile_image);
                bit=0;
                byte=0;
                for (x=0; x < (int) tile_image->columns; x++)
                {
                  byte<<=1;
                  if (indexes[x] == polarity)
                    byte|=0x01;
                  bit++;
                  if (bit == 8)
                    {
                      *q++=byte;
                      bit=0;
                      byte=0;
                    }
                  p++;
                }
                if (bit != 0)
                  *q++=byte << (8-bit);
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,tile_image->rows))
                    ProgressMonitor(SaveImageText,y,tile_image->rows);
              }
              if (compression == ZipCompression)
                status=ZLIBEncodeImage(image,number_packets,image_info->quality,
                  pixels);
              else
                if (compression == LZWCompression)
                  status=LZWEncodeImage(image,number_packets,pixels);
                else
                  status=PackbitsEncodeImage(image,number_packets,pixels);
              FreeMemory(pixels);
              if (!status)
                {
                  CloseBlob(image);
                  return(False);
                }
              break;
            }
            case NoCompression:
            {
              /*
                Dump uncompressed PseudoColor packets.
              */
              Ascii85Initialize();
              for (y=0; y < (int) tile_image->rows; y++)
              {
                p=GetPixelCache(tile_image,0,y,tile_image->columns,1);
                if (p == (PixelPacket *) NULL)
                  break;
                indexes=GetIndexes(tile_image);
                bit=0;
                byte=0;
                for (x=0; x < (int) tile_image->columns; x++)
                {
                  byte<<=1;
                  if (indexes[x] == polarity)
                    byte|=0x01;
                  bit++;
                  if (bit == 8)
                    {
                      Ascii85Encode(image,byte);
                      bit=0;
                      byte=0;
                    }
                  p++;
                }
                if (bit != 0)
                  Ascii85Encode(image,byte << (8-bit));
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,tile_image->rows))
                    ProgressMonitor(SaveImageText,y,tile_image->rows);
              }
              Ascii85Flush(image);
              break;
            }
          }
        }
      else
        {
          /*
            Dump number of colors and colormap.
          */
          switch (compression)
          {
            case RunlengthEncodedCompression:
            default:
            {
              /*
                Allocate pixel array.
              */
              number_packets=tile_image->columns*tile_image->rows;
              pixels=(unsigned char *) AllocateMemory(number_packets);
              if (pixels == (unsigned char *) NULL)
                {
                  DestroyImage(tile_image);
                  ThrowWriterException(ResourceLimitWarning,
                    "Memory allocation failed",image);
                }
              /*
                Dump Runlength encoded pixels.
              */
              q=pixels;
              for (y=0; y < (int) tile_image->rows; y++)
              {
                p=GetPixelCache(tile_image,0,y,tile_image->columns,1);
                if (p == (PixelPacket *) NULL)
                  break;
                indexes=GetIndexes(tile_image);
                for (x=0; x < (int) tile_image->columns; x++)
                {
                  *q++=indexes[x];
                  p++;
                }
              }
              if (compression == ZipCompression)
                status=ZLIBEncodeImage(image,number_packets,image_info->quality,
                  pixels);
              else
                if (compression == LZWCompression)
                  status=LZWEncodeImage(image,number_packets,pixels);
                else
                  status=PackbitsEncodeImage(image,number_packets,pixels);
              FreeMemory(pixels);
              if (!status)
                {
                  CloseBlob(image);
                  return(False);
                }
              break;
            }
            case NoCompression:
            {
              /*
                Dump uncompressed PseudoColor packets.
              */
              Ascii85Initialize();
              for (y=0; y < (int) tile_image->rows; y++)
              {
                p=GetPixelCache(tile_image,0,y,tile_image->columns,1);
                if (p == (PixelPacket *) NULL)
                  break;
                indexes=GetIndexes(tile_image);
                for (x=0; x < (int) tile_image->columns; x++)
                {
                  Ascii85Encode(image,indexes[x]);
                  p++;
                }
              }
              Ascii85Flush(image);
              break;
            }
          }
        }
    DestroyImage(tile_image);
    length=TellBlob(image)-length;
    (void) strcpy(buffer,"\nendstream\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,"endobj\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    /*
      Write Length object.
    */
    xref[object++]=TellBlob(image);
    (void) sprintf(buffer,"%u 0 obj\n",object);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) sprintf(buffer,"%lu\n",length);
    (void) WriteBlob(image,strlen(buffer),buffer);
    (void) strcpy(buffer,"endobj\n");
    (void) WriteBlob(image,strlen(buffer),buffer);
    if ((image->class == DirectClass) || IsFaxImage(image))
      {
        xref[object++]=0;
        xref[object++]=0;
      }
    else
      {
        /*
          Write Colormap object.
        */
        xref[object++]=TellBlob(image);
        (void) sprintf(buffer,"%u 0 obj\n",object);
        (void) WriteBlob(image,strlen(buffer),buffer);
        (void) strcpy(buffer,"<<\n");
        (void) WriteBlob(image,strlen(buffer),buffer);
        (void) strcpy(buffer,"/Filter /ASCII85Decode \n");
        (void) WriteBlob(image,strlen(buffer),buffer);
        (void) sprintf(buffer,"/Length %u 0 R\n",object+1);
        (void) WriteBlob(image,strlen(buffer),buffer);
        (void) strcpy(buffer,">>\n");
        (void) WriteBlob(image,strlen(buffer),buffer);
        (void) strcpy(buffer,"stream\n");
        (void) WriteBlob(image,strlen(buffer),buffer);
        length=TellBlob(image);
        Ascii85Initialize();
        for (i=0; i < (int) image->colors; i++)
        {
          Ascii85Encode(image,DownScale(image->colormap[i].red));
          Ascii85Encode(image,DownScale(image->colormap[i].green));
          Ascii85Encode(image,DownScale(image->colormap[i].blue));
        }
        Ascii85Flush(image);
        length=TellBlob(image)-length;
        (void) strcpy(buffer,"\nendstream\n");
        (void) WriteBlob(image,strlen(buffer),buffer);
        (void) strcpy(buffer,"endobj\n");
        (void) WriteBlob(image,strlen(buffer),buffer);
        /*
          Write Length object.
        */
        xref[object++]=TellBlob(image);
        (void) sprintf(buffer,"%u 0 obj\n",object);
        (void) WriteBlob(image,strlen(buffer),buffer);
        (void) sprintf(buffer,"%lu\n",length);
        (void) WriteBlob(image,strlen(buffer),buffer);
        (void) strcpy(buffer,"endobj\n");
        (void) WriteBlob(image,strlen(buffer),buffer);
      }
    if (image->next == (Image *) NULL)
      break;
    image=GetNextImage(image);
    ProgressMonitor(SaveImagesText,scene++,GetNumberScenes(image));
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  /*
    Write Xref object.
  */
  length=TellBlob(image)-xref[0]+10;
  (void) strcpy(buffer,"xref\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) sprintf(buffer,"0 %u\n",object+1);
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) strcpy(buffer,"0000000000 65535 f \n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  for (i=0; i < (int) object; i++)
  {
    (void) sprintf(buffer,"%010lu 00000 n \n",xref[i]);
    (void) WriteBlob(image,strlen(buffer),buffer);
  }
  (void) strcpy(buffer,"trailer\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) strcpy(buffer,"<<\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) sprintf(buffer,"/Size %u\n",object+1);
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) sprintf(buffer,"/Info %u 0 R\n",info_id);
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) sprintf(buffer,"/Root %u 0 R\n",root_id);
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) strcpy(buffer,">>\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) strcpy(buffer,"startxref\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) sprintf(buffer,"%lu\n",length);
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) strcpy(buffer,"%%EOF\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  FreeMemory(xref);
  CloseBlob(image);
  if (image->temporary)
    {
      FILE
        *file;

      int
        c;

      /*
        Copy temporary file to standard output or pipe.
      */
      file=fopen(image->filename,ReadBinaryType);
      if (file == (FILE *) NULL)
        ThrowWriterException(FileOpenWarning,"Unable to open file",image);
      for (c=fgetc(file); c != EOF; c=fgetc(file))
        (void) fputc(c,encode_image.file);
      (void) fclose(file);
      (void) remove(image->filename);
      image->temporary=False;
      CloseBlob(&encode_image);
    }
  return(True);
}
