/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%               M   M   AAA   TTTTT  L       AAA   BBBB                       %
%               MM MM  A   A    T    L      A   A  B   B                      %
%               M M M  AAAAA    T    L      AAAAA  BBBB                       %
%               M   M  A   A    T    L      A   A  B   B                      %
%               M   M  A   A    T    LLLLL  A   A  BBBB                       %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                              Jaroslav Fojtik                                %
%                                 June 2001                                   %
%                                                                             %
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
#include "studio.h"
#include "blob.h"
#include "magick.h"
#include "utility.h"

/* Auto coloring method, sorry this creates some artefact inside data
MinReal+j*MaxComplex = red  MaxReal+j*MaxComplex = black
MinReal+j*0 = white          MaxReal+j*0 = black
MinReal+j*MinComplex = blue  MaxReal+j*MinComplex = black
*/

typedef struct {
        char identific[125];
        char idx[3];
        unsigned long unknown0;
        unsigned long ObjectSize;
        unsigned long unknown1;
        unsigned long unknown2;

        unsigned long StructureFlag;
        unsigned long unknown3;
        unsigned long unknown4;
        unsigned long DimFlag;

        unsigned long SizeX;
        unsigned long SizeY;
        unsigned short Flag1;
        unsigned short NameFlag;
        } MATHeader;


static void InsertRow(unsigned char *p,int y,Image *image)
{
int x;
register PixelPacket *q;
IndexPacket index;
register IndexPacket *indexes;

 switch (image->depth)
      {
      case 8: /* Convert PseudoColor scanline. */
           {
           q=SetImagePixels(image,0,y,image->columns,1);
           if (q == (PixelPacket *) NULL) break;
           indexes=GetIndexes(image);

           for (x=0; x < (long) image->columns; x++)
                {
                index=ConstrainColormapIndex(image,*p);
                indexes[x]=index;
                *q++=image->colormap[index];
                p++;
                }
           if (!SyncImagePixels(image))
                 break;
/*           if (image->previous == (Image *) NULL)
                 if (QuantumTick(y,image->rows))
                   ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);*/
           }
           return;
     
      case 16:
           {
     q=SetImagePixels(image,0,y,image->columns,1);
           if (q == (PixelPacket *) NULL)
       break;
           for (x=0; x < (long) image->columns; x++)
              {
              q->red=ScaleShortToQuantum(*(unsigned short *)p);
              q->green=ScaleShortToQuantum(*(unsigned short *)p);
              q->blue=ScaleShortToQuantum(*(unsigned short *)p);
        p+=2;
              q++;
              }
           if (!SyncImagePixels(image))
              break;
/*          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              MagickMonitor(LoadImageText,image->rows-y-1,image->rows);*/
          return;     
    } 
       }
}


static void InsertFloatRow(double *p,int y,Image *image,double Min,double Max)
{
double f;
int x;
register PixelPacket *q;

   if(Min>=Max) Max=Min+1;

   q=SetImagePixels(image,0,y,image->columns,1);
   if (q == (PixelPacket *) NULL)
        return;
   for (x=0; x < (long) image->columns; x++)
          {
    f=(double)MaxRGB* (*p-Min)/(Max-Min);
          q->red=ScaleShortToQuantum(f);
    q->green=ScaleShortToQuantum(f);
    q->blue=ScaleShortToQuantum(f);
          p++;
          q++;
          }
   if (!SyncImagePixels(image))
              return;
/*          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              MagickMonitor(LoadImageText,image->rows-y-1,image->rows);*/
   return;     
}


static void InsertComplexFloatRow(double *p,int y,Image *image,double Min,double Max)
{
double f;
int x;
register PixelPacket *q;

   if(Min==0) Min=-1;
   if(Max==0) Max=1;

   q=SetImagePixels(image,0,y,image->columns,1);
   if (q == (PixelPacket *) NULL)
        return;
   for (x=0; x < (long) image->columns; x++)
          {
    if(*p>0)
      {
      f=(*p/Max)*(MaxRGB-q->red);
      if(f+q->red>MaxRGB) q->red=MaxRGB;
                     else q->red+=(int) f;
            if((int) f/2.0>q->green) q->green=q->blue=0;
         else q->green=q->blue-=(int) (f/2.0);    
      }
    if(*p<0)
      {
      f=(*p/Max)*(MaxRGB-q->blue);
      if(f+q->blue>MaxRGB) q->blue=MaxRGB;
                      else q->blue+=(int) f;
            if((int) f/2.0>q->green) q->green=q->red=0;
        else q->green=q->red-=(int) (f/2.0);    
      }
    p++;
          q++;
          }
   if (!SyncImagePixels(image))
              return;
/*          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              MagickMonitor(LoadImageText,image->rows-y-1,image->rows);*/
   return;     
}


/*This function reads one block of unsigned shortS*/
static void ReadBlobWordLSB(Image *I,size_t len,unsigned short *data)
{
while(len>=2)
   {
   *data++=ReadBlobLSBShort(I);
   len-=2;
   }
if(len>0) (void) SeekBlob(I,len,SEEK_CUR);   
}


static double ReadBlobLSBdouble(Image *image)
{
 typedef union
   {
   double d;
   char chars[8]; 
   } dbl;
  static unsigned long lsb_first=1;    
  dbl buffer;
  char c;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  
  if(ReadBlob(image,8,(unsigned char *)&buffer)==0)
  return(0.0);  
  if(*(char *)&lsb_first==1) return(buffer.d);

  c=buffer.chars[0]; buffer.chars[0]=buffer.chars[7]; buffer.chars[7]=c;
  c=buffer.chars[1]; buffer.chars[1]=buffer.chars[6]; buffer.chars[6]=c; 
  c=buffer.chars[2]; buffer.chars[2]=buffer.chars[5]; buffer.chars[5]=c;
  c=buffer.chars[3]; buffer.chars[3]=buffer.chars[4]; buffer.chars[4]=c; 
  return(buffer.d);
}


static void ReadBlobDoublesLSB(Image *I,size_t len,double *data)
{
while(len>=8)
   {
   *data++=ReadBlobLSBdouble(I);
   len-=sizeof(double);
   }
if(len>0) (void) SeekBlob(I,len,SEEK_CUR);   
}



/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d M A T L A B i m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadMATImage reads an MAT X image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadMATImage method is:
%
%      Image *ReadMATImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadMATImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadMATImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image *image,*rotated_image;
  unsigned int status;
  MATHeader MATLAB_HDR;
  unsigned long size;
  off_t filepos;
  unsigned long CellType;
  int i,x;
  long ldblk;
  unsigned char *BImgBuff=NULL;
  double Min,Max,*dblrow;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,"Unable to open file",image);
  /*
    Read MATLAB image.
  */
  (void) ReadBlob(image,125,&MATLAB_HDR.identific);
  (void) ReadBlob(image,3,&MATLAB_HDR.idx); 
  MATLAB_HDR.unknown0=ReadBlobLSBLong(image);
  MATLAB_HDR.ObjectSize=ReadBlobLSBLong(image);
  MATLAB_HDR.unknown1=ReadBlobLSBLong(image);
  MATLAB_HDR.unknown2=ReadBlobLSBLong(image);
  MATLAB_HDR.StructureFlag=ReadBlobLSBLong(image);
  MATLAB_HDR.unknown3=ReadBlobLSBLong(image);
  MATLAB_HDR.unknown4=ReadBlobLSBLong(image);
  MATLAB_HDR.DimFlag=ReadBlobLSBLong(image);
  MATLAB_HDR.SizeX=ReadBlobLSBLong(image);
  MATLAB_HDR.SizeY=ReadBlobLSBLong(image);
  MATLAB_HDR.Flag1=ReadBlobLSBShort(image);
  MATLAB_HDR.NameFlag=ReadBlobLSBShort(image);

  if (strncmp(MATLAB_HDR.identific,"MATLAB",6))
MATLAB_KO:  ThrowReaderException(CorruptImageError,"Not a MATLAB image file!",image);
  if (strncmp(MATLAB_HDR.idx,"\1IM",3)) goto MATLAB_KO;
  if(MATLAB_HDR.unknown0!=0x0E) goto MATLAB_KO;
  if(MATLAB_HDR.DimFlag!=8) 
            ThrowReaderException(CorruptImageError,"Multi-dimensional matrices are not supported!",image);

/*printf("MATLAB_HDR.StructureFlag %ld\n",MATLAB_HDR.StructureFlag);*/
  if(MATLAB_HDR.StructureFlag!=6 && MATLAB_HDR.StructureFlag!=0x806) goto MATLAB_KO;

  switch(MATLAB_HDR.NameFlag)
    {
    case 0:(void) ReadBlob(image,4,&size); /*Object name string*/
           size=4*(long)((size+3+1)/4);
           (void) SeekBlob(image,size,SEEK_CUR);
     break;
    case 1:
    case 2:
    case 3:
    case 4:(void) ReadBlob(image,4,&size); /*Object name string*/
     break;
    default:goto MATLAB_KO;
    }

   CellType=ReadBlobLSBLong(image);    /*Additional object type*/
/*fprintf(stdout,"Cell type:%ld\n",CellType);*/
   (void) ReadBlob(image,4,&size);      /*data size*/

   switch(CellType)
      {
      case 2:image->depth=8;    /*Byte type cell*/
             ldblk=(long) MATLAB_HDR.SizeX;
        if(MATLAB_HDR.StructureFlag==0x806) goto MATLAB_KO;    
             break;
      case 4:image->depth=16;      /*Word type cell*/
             ldblk=(long) (2*MATLAB_HDR.SizeX);
       if(MATLAB_HDR.StructureFlag==0x806) goto MATLAB_KO;    
             break; 
      case 9:image->depth=24;      /*double type cell*/
             if(sizeof(double)!=8) ThrowReaderException(CorruptImageError,"Incompatible size of double!",image);
       if(MATLAB_HDR.StructureFlag==0x806) 
          {      /*complex double type cell*/
    }
       ldblk=(long) (8*MATLAB_HDR.SizeX);
             break; 
      default:ThrowReaderException(CorruptImageError,"Unsupported cell type in the matrix!",image)
      }    

   image->columns= MATLAB_HDR.SizeX;
   image->rows= MATLAB_HDR.SizeY;
   image->colors=1l >> 8;
   if(image->columns==0 || image->rows==0) goto MATLAB_KO;

   
/* ----- Create gray palette ----- */

   if(CellType==2)
     {
     image->colors=256;
     if (!AllocateImageColormap(image,image->colors))
           {
NoMemory:  ThrowReaderException(ResourceLimitError,"Memory allocation failed",
                                image)
           }           
   
     for (i=0; i < (long)image->colors; i++)
           {
           image->colormap[i].red=ScaleCharToQuantum(i);
           image->colormap[i].green=ScaleCharToQuantum(i);
           image->colormap[i].blue=ScaleCharToQuantum(i);
           }
     }      

           
/* ----- Load raster data ----- */
   BImgBuff=(unsigned char *) malloc(ldblk);  /*Ldblk was set in the check phase*/
   if(BImgBuff==NULL) goto NoMemory;


   Min=0;
   Max=0;
   if(CellType==9) /*Find Min and Max Values for floats*/
     {
     filepos=TellBlob(image);
     for(i=0;i<(long) MATLAB_HDR.SizeY;i++)
        {
  ReadBlobDoublesLSB(image,ldblk,(double *)BImgBuff);
  dblrow=(double *)BImgBuff;
  if(i==0) {Min=Max=*dblrow;}
  for(x=0;x<(long)MATLAB_HDR.SizeX;x++)
            {
      if(Min>*dblrow) Min=*dblrow;
      if(Max<*dblrow) Max=*dblrow;
      dblrow++;            
      }
  }       
     SeekBlob(image,filepos,SEEK_SET);
     }
     
  /*Main loop for reading all scanlines*/
   for(i=0;i<(long) MATLAB_HDR.SizeY;i++)
        {
  switch(CellType)
      {
      case 4:ReadBlobWordLSB(image,ldblk,(unsigned short *)BImgBuff);
       InsertRow(BImgBuff,i,image);
       break;
      case 9:ReadBlobDoublesLSB(image,ldblk,(double *)BImgBuff);
       InsertFloatRow((double *)BImgBuff,i,image,Min,Max);
       break;
      default:(void) ReadBlob(image,ldblk,(char *)BImgBuff);
        InsertRow(BImgBuff,i,image);
      }  
        }
  
  /*Read complex part of numbers here*/
  if(MATLAB_HDR.StructureFlag==0x806)
  {
  if(CellType==9) /*Find Min and Max Values for complex parts of floats*/
          {
    filepos=TellBlob(image);
          for(i=0;i<(long) MATLAB_HDR.SizeY;i++)
      {
      ReadBlobDoublesLSB(image,ldblk,(double *)BImgBuff);
      dblrow=(double *)BImgBuff;
      if(i==0) {Min=Max=*dblrow;}
      for(x=0;x<(long)MATLAB_HDR.SizeX;x++)
                {
          if(Min>*dblrow) Min=*dblrow;
    if(Max<*dblrow) Max=*dblrow;
    dblrow++;            
          }
      }       
          SeekBlob(image,filepos,SEEK_SET);
    
    for(i=0;i<(long) MATLAB_HDR.SizeY;i++)
            {
      ReadBlobDoublesLSB(image,ldblk,(double *)BImgBuff);
      InsertComplexFloatRow((double *)BImgBuff,i,image,Min,Max);
      }  
          }
        }
  
  
  if(BImgBuff!=NULL) {free(BImgBuff);BImgBuff=NULL;}
    
        /*  Rotate image. */             
  rotated_image=RotateImage(image,90.0,exception);
  if (rotated_image != (Image *) NULL)
        {
  DestroyImage(image);
        image=FlopImage(rotated_image,exception);
  if(image==NULL) image=rotated_image;  /*Obtain something if flop operation fails*/
       else DestroyImage(rotated_image);
  }

  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r M A T I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterMATImage adds attributes for the MAT image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterMATImage method is:
%
%      RegisterMATImage(void)
%
*/
ModuleExport void RegisterMATImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("MAT");
  entry->decoder=ReadMATImage;
  entry->description=AcquireString("MATLAB image format");
  entry->module=AcquireString("MAT");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r M A T I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterMATImage removes format registrations made by the
%  MAT module from the list of supported formats.
%
%  The format of the UnregisterMATImage method is:
%
%      UnregisterMATImage(void)
%
*/
ModuleExport void UnregisterMATImage(void)
{
  (void) UnregisterMagickInfo("MAT");
}
