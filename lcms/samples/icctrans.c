//
//  Little cms
//  Copyright (C) 1998-2003 Marti Maria
//
// Permission is hereby granted, free of charge, to any person obtaining 
// a copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the Software 
// is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in 
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE 
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#include "lcms.h"

#include <stdarg.h>
#include <ctype.h>

// xgetopt() interface -----------------------------------------------------

extern int   xoptind;
extern char *xoptarg;
extern int   xopterr;
extern char  SW;
int    cdecl xgetopt(int argc, char *argv[], char *optionS);

// ------------------------------------------------------------------------

static BOOL InHexa = FALSE;
static int  Verbose = 0;
static char *cInProf = NULL;
static char *cOutProf = NULL;
static int Intent = INTENT_PERCEPTUAL;
static BOOL Width16 = FALSE;
static BOOL WhiteBlackCompensation = FALSE;
static int PrecalcMode             = 0;
static BOOL lIsDeviceLink          = FALSE;
static BOOL lTerse     = FALSE;

static cmsHPROFILE hInput, hOutput, hLab, hXYZ;
static cmsHTRANSFORM hTrans, hTransXYZ, hTransLab;

static icColorSpaceSignature InputColorSpace, OutputColorSpace;
static cmsCIEXYZ xyz;
static cmsCIELab Lab;

#ifdef _MS_VER
#define xisatty(x) _isatty( _fileno( (x) ) )
#else
#define xisatty(x) isatty( fileno( (x) ) )
#endif



static
void FatalError(const char *frm, ...)
{
       va_list args;

       va_start(args, frm);
       vfprintf(stderr, frm, args);
       va_end(args);

       exit(1);
}



// The toggles stuff

static
void HandleSwitches(int argc, char *argv[])
{
       int s;
      
       while ((s = xgetopt(argc,argv,"C:c:VvWwxXhHbBnNI:i:O:o:T:t:L:l:")) != EOF) {

       switch (s){

       case 'b':
       case 'B': 
           WhiteBlackCompensation = TRUE;
           break;

     
       case 'c':
       case 'C':
            PrecalcMode = atoi(xoptarg);
            if (PrecalcMode < 0 || PrecalcMode > 3)
                    FatalError("Unknown precalc mode '%d'", PrecalcMode);
            break;

       case 'x':
       case 'X':
            InHexa = TRUE;
            break;

       case 'v':
       case 'V':
            Verbose = TRUE;
            break;

       case 'n':
       case 'N':
           lTerse = TRUE;
           break;

       case 'i':
       case 'I':
            if (lIsDeviceLink)
                   FatalError("Device-link already specified");

            cInProf = xoptarg;
            break;

       case 'o':
       case 'O':
            if (lIsDeviceLink)
                   FatalError("Device-link already specified"); 
            cOutProf = xoptarg;
            break;


       case 't':
       case 'T':
            Intent = atoi(xoptarg);
            if (Intent > 3) Intent = 3;
            if (Intent < 0) Intent = 0;
            break;
     
       case 'W':
       case 'w':
            Width16 = TRUE;
            break;

       case 'l':
       case 'L': 
                cInProf = xoptarg;
                lIsDeviceLink = TRUE;
                break;

  default:

       FatalError("Unknown option - run without args to see valid ones.\n");
    }
       
    }
}



static
cmsHPROFILE OpenProfile(const char* File)
{   
       if (!File) 
            return cmsCreate_sRGBProfile();    
       
       if (stricmp(File, "*Lab") == 0)
                return cmsCreateLabProfile(NULL);
       
       if (stricmp(File, "*LabD65") == 0) {

           cmsCIExyY D65xyY;
           
           cmsWhitePointFromTemp(6504, &D65xyY);           
           return cmsCreateLabProfile(&D65xyY);
       }

       if (stricmp(File, "*XYZ") == 0)
                return cmsCreateXYZProfile();
           
        return cmsOpenProfileFromFile(File, "r");
}

static
void OpenTransforms(void)
{

    DWORD dwIn, dwOut, dwFlags;

    if (lIsDeviceLink) {

            hInput  = cmsOpenProfileFromFile(cInProf, "r");
            hOutput = NULL;
            InputColorSpace   = cmsGetColorSpace(hInput);
            OutputColorSpace = cmsGetPCS(hInput);
        
            
       }
    else {

            hInput  = OpenProfile(cInProf);
            hOutput = OpenProfile(cOutProf);    

            InputColorSpace   = cmsGetColorSpace(hInput);
            OutputColorSpace  = cmsGetColorSpace(hOutput);

            if (cmsGetDeviceClass(hInput) == icSigLinkClass ||
                cmsGetDeviceClass(hOutput) == icSigLinkClass)   
                        FatalError("Use %cl flag for devicelink profiles!\n", SW);
    
    }

       hXYZ    = cmsCreateXYZProfile();
       hLab    = cmsCreateLabProfile(NULL);


       if (Verbose) {

            printf("From: %s\n", cmsTakeProductName(hInput));
            if (hOutput) printf("To  : %s\n\n", cmsTakeProductName(hOutput));
       }


      

       dwIn  = BYTES_SH(2) | CHANNELS_SH(_cmsChannelsOf(InputColorSpace));
       dwOut = BYTES_SH(2) | CHANNELS_SH(_cmsChannelsOf(OutputColorSpace));

       dwFlags = 0; 
       
       switch (PrecalcMode) {
           
       case 0: dwFlags |= cmsFLAGS_NOTPRECALC; break;
       case 2: dwFlags |= cmsFLAGS_HIGHRESPRECALC; break;
	   case 3: dwFlags |= cmsFLAGS_LOWRESPRECALC; break;
	   case 1: break;

       default: FatalError("Unknown precalculation mode '%d'", PrecalcMode);
       }
       
       
       if (WhiteBlackCompensation) 
            dwFlags |= cmsFLAGS_WHITEBLACKCOMPENSATION;

       hTrans     = cmsCreateTransform(hInput,  dwIn,
                                       hOutput, dwOut,
                                        Intent, dwFlags);

       if (hOutput) {

       hTransXYZ = cmsCreateTransform(hInput, dwIn,
                                      hXYZ,  TYPE_XYZ_16,
                                      Intent, cmsFLAGS_NOTPRECALC);

       hTransLab = cmsCreateTransform(hInput, dwIn,
                                      hLab,  TYPE_Lab_16,
                                      Intent, cmsFLAGS_NOTPRECALC);    
       }
       
}


static
void CloseTransforms(void)
{
       cmsDeleteTransform(hTrans);
       if (hTransLab) cmsDeleteTransform(hTransLab);
       if (hTransXYZ) cmsDeleteTransform(hTransXYZ);
       cmsCloseProfile(hInput);
      if (hOutput) cmsCloseProfile(hOutput);      
       cmsCloseProfile(hXYZ);
       cmsCloseProfile(hLab);

}


static
void PrintOne(const char* C, double v)
{
    char Prefix[20];

    Prefix[0] = 0;
    if (!lTerse)
        sprintf(Prefix, "%s=", C);

    if (InHexa)
    {
        if (Width16)
            printf("%s0x%x ", Prefix, (int) floor(v + .5));
        else
            printf("%s0x%x ", Prefix, (int) floor(v / 257. + .5));

    }
    else
    {       
        printf("%s%.2f ", Prefix, v / 257.);
    }
}


static
void PrintCooked(const char* C, double v)
{
    if (lTerse)
        printf("%.4f ", v);
    else
        printf("%s=%.4f ", C, v);
}


static
void PrintResults(WORD Encoded[], icColorSpaceSignature ColorSpace)
{
    int i;

    switch (ColorSpace) {

    case icSigXYZData:
                    cmsXYZEncoded2Float(&xyz, Encoded);
                    PrintCooked("X", xyz.X * 100.); PrintCooked("Y", xyz.Y * 100.); PrintCooked("Z", xyz.Z * 100.);
                    break;

    case icSigLabData:
                    cmsLabEncoded2Float(&Lab, Encoded);
                    PrintCooked("L*", Lab.L); PrintCooked("a*", Lab.a); PrintCooked("b*", Lab.b);
                    break;

    case icSigLuvData:
                    PrintOne("L", Encoded[0]); PrintOne("u", Encoded[1]); PrintOne("v", Encoded[2]);
                    break;

    case icSigYCbCrData:
                    PrintOne("Y", Encoded[0]); PrintOne("Cb", Encoded[1]); PrintOne("Cr", Encoded[2]);
                    break;


    case icSigYxyData:
                    PrintOne("Y", Encoded[0]); PrintOne("x", Encoded[1]); PrintOne("y", Encoded[2]);
                    break;

    case icSigRgbData:
                    PrintOne("R", Encoded[0]); PrintOne("G", Encoded[1]); PrintOne("B", Encoded[2]);
                    break;

    case icSigGrayData:
                    PrintOne("L", Encoded[0]); 
                    break;

    case icSigHsvData:
                    PrintOne("H", Encoded[0]); PrintOne("s", Encoded[1]); PrintOne("v", Encoded[2]);
                    break;

    case icSigHlsData:
                    PrintOne("H", Encoded[0]); PrintOne("l", Encoded[1]); PrintOne("s", Encoded[2]);
                    break;

    case icSigCmykData:
                    PrintOne("C", Encoded[0]); PrintOne("M", Encoded[1]); PrintOne("Y", Encoded[2]); PrintOne("K", Encoded[3]);
                    break;

    case icSigCmyData:                        
                    PrintOne("C", Encoded[0]); PrintOne("M", Encoded[1]); PrintOne("Y", Encoded[2]); 
                    break;

    case icSig6colorData:
                            
                    PrintOne("C", Encoded[0]); PrintOne("M", Encoded[1]); PrintOne("Y", Encoded[2]); 
                    PrintOne("K", Encoded[3]); PrintOne("c", Encoded[1]); PrintOne("m", Encoded[2]); 
                    break;
    default:

        for (i=0; i < _cmsChannelsOf(OutputColorSpace); i++) {
        
            char Buffer[10];
            sprintf(Buffer, "CHAN%d", i + 1);
            PrintOne(Buffer, Encoded[i]);           
        }   
    }
}


static 
int GetVal(const char* AskFor)
{
    char Buffer[256];
    double tmp;
    char *Max;
    char* MaxTbl[] = { "255",
                     "65535",
                     "FF",
                     "FFFF" };

    Max = MaxTbl[InHexa * 2 + Width16];

    
    if (xisatty(stdin))
           printf("%s (0..%s)? ", AskFor, Max);

    scanf("%s", Buffer);

    // Quit?

    if (toupper(Buffer[0]) == 'Q') {
        CloseTransforms();

        if (xisatty(stdin))  
            printf("Done.\n");

        exit(0);
        
    }

    sscanf(Buffer, (InHexa ? "%x" : "%lf"), &tmp);

    if (!Width16)
        tmp *= 257.0;

    return (int) floor(tmp + 0.5);
}


static 
double GetDbl(const char* AskFor)
{
  char Buffer[256];
  
  if (xisatty(stdin))
           printf("%s? ", AskFor);

    scanf("%s", Buffer);

    // Quit?

    if (toupper(Buffer[0]) == 'Q') {
        CloseTransforms();

        if (xisatty(stdin))  
            printf("Done.\n");

        exit(0);
        
    }

   
    return atof(Buffer);
}


static
void TakeValues(WORD Encoded[])
{
    switch (InputColorSpace) {

    case icSigXYZData:
                    xyz.X = GetDbl("X"); xyz.Y = GetDbl("Y"); xyz.Z = GetDbl("Z");
                    cmsFloat2XYZEncoded(Encoded, &xyz);                 
                    break;

    case icSigLabData:
                    Lab.L= GetDbl("L*"); Lab.a= GetDbl("a*"); Lab.b = GetDbl("b*");
                    cmsFloat2LabEncoded(Encoded, &Lab);                 
                    break;

    case icSigLuvData:
                    Encoded[0] = GetVal("L"); Encoded[1] = GetVal("u"); Encoded[2] = GetVal("v"); 
                    break;

    case icSigYCbCrData:
                    Encoded[0] = GetVal("Y"); Encoded[1] = GetVal("Cb"); Encoded[2] = GetVal("Cr"); 
                    break;


    case icSigYxyData:
                    Encoded[0] = GetVal("Y"); Encoded[1] = GetVal("x"); Encoded[2] = GetVal("y"); 
                    break;

    case icSigRgbData:
                    Encoded[0] = GetVal("R"); Encoded[1] = GetVal("G"); Encoded[2] = GetVal("B"); 
                    break;

    case icSigGrayData:
                    Encoded[0] = GetVal("L");
                    break;

    case icSigHsvData:
                    Encoded[0] = GetVal("H"); Encoded[1] = GetVal("s"); Encoded[2] = GetVal("v"); 
                    break;

    case icSigHlsData:
                    Encoded[0] = GetVal("H"); Encoded[1] = GetVal("l"); Encoded[2] = GetVal("s"); 
                    break;

    case icSigCmykData:
                    Encoded[0] = GetVal("C"); Encoded[1] = GetVal("M"); Encoded[2] = GetVal("Y"); Encoded[3] = GetVal("K"); 
                    break;

    case icSigCmyData:                        
                    Encoded[0] = GetVal("C"); Encoded[1] = GetVal("M"); Encoded[2] = GetVal("Y"); 
                    break;

    case icSig6colorData:

                    Encoded[0] = GetVal("C"); Encoded[1] = GetVal("M"); Encoded[2] = GetVal("Y"); Encoded[3] = GetVal("K"); 
                    Encoded[4] = GetVal("c"); Encoded[5] = GetVal("m");                                       
                    break;
    default:

                    FatalError("Unsupported %d channel profile", _cmsChannelsOf(InputColorSpace));
    }

}


static
void Help(void)
{
             
     fprintf(stderr, "usage: icctrans [flags]\n\n");

     fprintf(stderr, "flags:\n\n");
     fprintf(stderr, "%cv - Verbose\n", SW);
     fprintf(stderr, "%ci<profile> - Input profile (defaults to sRGB)\n", SW);
     fprintf(stderr, "%co<profile> - Output profile (defaults to sRGB)\n", SW);   
     fprintf(stderr, "%cl<profile> - Transform by device-link profile\n", SW);   

     fprintf(stderr, "\nYou can use '*Lab' and '*xyz' as built-in profiles\n\n");

     fprintf(stderr, "%ct<0,1,2,3> - Intent (0=Perceptual, 1=Colorimetric, 2=Saturation, 3=Absolute)\n", SW);    
     
     fprintf(stderr, "%cw - use 16 bits\n", SW);
     fprintf(stderr, "%cx - Hexadecimal\n\n", SW);
     fprintf(stderr, "%cb - Black point compensation\n", SW);
     fprintf(stderr, "%cc<0,1,2,3> - Precalculates transform (0=Off, 1=Normal, 2=Hi-res, 3=LoRes) [defaults to 1]\n", SW);     
     fprintf(stderr, "%cn - Terse output, intended for pipe usage\n\n", SW);

     fprintf(stderr, "This program is intended to be a demo of the little cms\n"
                     "engine. Both lcms and this program are freeware. You can\n"
                     "obtain both in source code at http://www.littlecms.com\n"
                     "For suggestions, comments, bug reports etc. send mail to\n"
                     "info@littlecms.com\n\n");
     exit(0);
}




int main(int argc, char *argv[])
{
    WORD Input[MAXCHANNELS], Output[MAXCHANNELS], PCSLab[MAXCHANNELS], PCSxyz[MAXCHANNELS];

    
    fprintf(stderr, "little cms ColorSpace conversion calculator - v1.7\n\n");

      if (argc == 1)  
              Help();              


      HandleSwitches(argc, argv);
                   
      OpenTransforms();
      
      for(;;) {

          if (xisatty(stdin))
              printf("\nEnter values, 'q' to quit\n");

          if (feof(stdin))
              break;

          TakeValues(Input);
          cmsDoTransform(hTrans, Input, Output, 1);

          if (hTransXYZ) cmsDoTransform(hTransXYZ, Input, PCSxyz, 1);
          if (hTransLab) cmsDoTransform(hTransLab, Input, PCSLab, 1);
    
          if (xisatty(stdin))
                printf("\n\n");

          
          PrintResults(Output, OutputColorSpace); printf("\n");

          if (Verbose && hTransXYZ && hTransLab) {

            PrintResults(PCSxyz, icSigXYZData); printf("\n");
            PrintResults(PCSLab, icSigLabData); printf("\n");
          }

      }

          
      return 0;     
}


