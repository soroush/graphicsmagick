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
#include "magick.h"
#include "define.h"


typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef struct {
        char identific[125];
        char idx[3];
        DWORD unknown0;
        DWORD ObjectSize;
        DWORD unknown1;
        DWORD unknown2;

        DWORD StructureFlag;
        DWORD unknown3;
        DWORD unknown4;
        DWORD DimFlag;

        DWORD SizeX;
        DWORD SizeY;
        WORD Flag1;
        WORD NameFlag;
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
                index=ValidateColormapIndex(image,*p);
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
              q->red=XDownScale(*(WORD *)p);
              q->green=XDownScale(*(WORD *)p);
              q->blue=XDownScale(*(WORD *)p);
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
          q->red=XDownScale(f);
          q->green=XDownScale(f);
          q->blue=XDownScale(f);
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




/*This procedure saves 2D data into MATLAB MAT file
void MATPrint(const PosContainer3D & a,int x,int y,int z,FILE *f,int Header)
{
WORD iz,Pos3D;
PosContainer2D Cont2D,OldCont2D;
PosContainer2D_3DExtender *WData;
char MATLAB_HDR[192];

if(f==NULL) f=stdout;

if(Header)
        {
        for(iz=0;iz<sizeof(MATLAB_HDR);iz++)
               MATLAB_HDR[iz]=0;
        for(iz=0;iz<124;iz++)
               MATLAB_HDR[iz]=' ';
        strcpy(MATLAB_HDR,"MATLAB 5.0 MAT-file, Platform: LNX86, Created on: Tue May 16 22:35:06 2000");
        MATLAB_HDR[0x7D]=1;
        MATLAB_HDR[0x7E]='I';
        MATLAB_HDR[0x7F]='M';
        MATLAB_HDR[0x80]=0xE;
        *(DWORD *)&(MATLAB_HDR[0x84])=(DWORD)x * (DWORD)y * (DWORD)z + 56l;
        MATLAB_HDR[0x88]=0x6;
        MATLAB_HDR[0x8C]=0x8;
        MATLAB_HDR[0x90]=0x6;
        MATLAB_HDR[0x98]=0x5;
        MATLAB_HDR[0x9C]=0xC;
        *(DWORD *)&(MATLAB_HDR[0xA0])=x;
        *(DWORD *)&(MATLAB_HDR[0xA4])=y;
        *(DWORD *)&(MATLAB_HDR[0xA8])=z;
        MATLAB_HDR[0xB0]=1;
        MATLAB_HDR[0xB2]=1;
        MATLAB_HDR[0xB4]='M';
        MATLAB_HDR[0xB8]=0x2;
        *(DWORD *)&(MATLAB_HDR[0xBC])=(DWORD)x * (DWORD)y * (DWORD)z;
        fwrite(MATLAB_HDR,1,sizeof(MATLAB_HDR),f);
        }

Pos3D=0;
WData=a.data3D;
for(iz=0;iz<z;iz++)
        {
        if(Pos3D<a.DataSize3D)
          if(WData->Position3D<=iz)
                {
                OldCont2D=Cont2D;
                schXOR(Cont2D,*WData,OldCont2D);
                WData++;
                Pos3D++;
                }

        MATPrint(Cont2D,x,y,f,0);
        }
}*/



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
  DWORD size,filepos;
  DWORD CellType;
  int i,x;
  long ldblk;
  unsigned char *BImgBuff=NULL;
  double Min,Max,*dblrow;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  
  status=OpenBlob(image_info,image,ReadBinaryType,exception);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Read MATLAB image.
  */
  (void) ReadBlob(image,sizeof(MATLAB_HDR),&MATLAB_HDR);

  if (strncmp(MATLAB_HDR.identific,"MATLAB",6))
MATLAB_KO:  ThrowReaderException(CorruptImageWarning,"Not a MATLAB image file!",image);
  if (strncmp(MATLAB_HDR.idx,"\1IM",3)) goto MATLAB_KO;
  if(MATLAB_HDR.unknown0!=0x0E) goto MATLAB_KO;
  if(MATLAB_HDR.DimFlag!=8) 
            ThrowReaderException(CorruptImageWarning,"Multi-dimensional matrices are not supported!",image);
 
  if(MATLAB_HDR.StructureFlag!=6) goto MATLAB_KO;

  if(MATLAB_HDR.NameFlag==1)
        {
        (void) ReadBlob(image,4,&size); /*Object name string*/
        }
   else if(MATLAB_HDR.NameFlag==0)
        {
        (void) ReadBlob(image,4,&size); /*Object name string*/
        size=4*(long)((size+3+1)/4);
        (void) SeekBlob(image,size,SEEK_CUR);
        }
   else goto MATLAB_KO;

   CellType=ReadBlobLSBLong(image);    /*Additional object type*/
/*fprintf(stdout,"Cell type:%ld\n",CellType);*/
   (void) ReadBlob(image,4,&size);      /*data size*/

   switch(CellType)
      {
      case 2:image->depth=8;		/*Byte type cell*/
             ldblk=MATLAB_HDR.SizeX;
             break;
      case 4:image->depth=16;    	/*Word type cell*/
             ldblk=2*MATLAB_HDR.SizeX;break; 
      case 9:image->depth=24;    	/*double type cell*/
             if(sizeof(double)!=8) ThrowReaderException(CorruptImageWarning,"Incompatible size of double!",image);
             ldblk=8*MATLAB_HDR.SizeX;
             break; 
      default:ThrowReaderException(CorruptImageWarning,"Unsupported cell type in the matrix!",image)
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
NoMemory:  ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
                                image)
           }           
   
     for (i=0; i < (long)image->colors; i++)
           {
           image->colormap[i].red=UpScale(i);
           image->colormap[i].green=UpScale(i);
           image->colormap[i].blue=UpScale(i);
           }
     }	    

           
/* ----- Load raster data ----- */
   BImgBuff=(unsigned char *) malloc(ldblk);  /*Ldblk was set in the check phase*/
   if(BImgBuff==NULL) goto NoMemory;


   if(CellType==9) /*Find Min and Max Values for floats*/
     {
     filepos=TellBlob(image);
     for(i=0;i<(long) MATLAB_HDR.SizeY;i++)
        {
        (void) ReadBlob(image,ldblk,(char *)BImgBuff);
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
   
   for(i=0;i<(long) MATLAB_HDR.SizeY;i++)
        {
        (void) ReadBlob(image,ldblk,(char *)BImgBuff);
	if(CellType==9) InsertFloatRow((double *)BImgBuff,i,image,Min,Max);
	           else InsertRow(BImgBuff,i,image);
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
  entry->description=AllocateString("MATLAB image format");
  entry->module=AllocateString("MAT");
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
