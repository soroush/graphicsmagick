// Configure.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Configure.h"
#include "configure_wizard.h"
#include "CStringEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const char *standard_include =
" /I \".\" /I \"..\\..\" /I \"..\\..\\magick\" /I \"..\\..\\xlib\" /I \"..\\..\\Magick++\\lib\" /I \"..\\..\\MagickArgs\"";

// /base:"0x66200000"
unsigned long dllbaselocation = 0x67000000L;

/////////////////////////////////////////////////////////////////////////////
// CConfigureApp

BEGIN_MESSAGE_MAP(CConfigureApp, CWinApp)
	//{{AFX_MSG_MAP(CConfigureApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigureApp construction

CConfigureApp::CConfigureApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CConfigureApp object

CConfigureApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CConfigureApp initialization

enum {MULTITHREADEDDLL, SINGLETHREADEDSTATIC, MULTITHREADEDSTATIC};

enum {DISABLED, UTILITY, LIBRARY, STATICLIB, MODULE, THIRDPARTY};
typedef struct _ConfigureInfo
{
  char
    *name;
  int
    type;
} ConfigureInfo;

BOOL useX11Stubs = TRUE;
BOOL decorateFilenames = FALSE;
CString release_loc;
CString debug_loc;
CString bin_loc;
CString lib_loc;
std::list<std::string> libs_list_shared;
std::list<std::string> defines_list;
std::list<std::string> dependency_list;

void CConfigureApp::process_utility(ofstream &dsw,
  WIN32_FIND_DATA	&data, const char *filename, bool mt)
{
	CStringEx basename = filename;
  std::string name = basename.GetField('.',0);
  std::string extn = basename.GetField('.',1);
  std::string prefix = "UTIL_";
  std::string staging = data.cFileName;

	std::list<std::string> libs_list_release;
	std::list<std::string> libs_list_debug;
	std::list<std::string> includes_list;

	write_exe_dsp(
    mt,
    staging,
		"",
		name,
		prefix,
		libs_list_shared,
		libs_list_release,
		libs_list_debug,
		defines_list,
    includes_list);

  std::string project;
  name = prefix + name;
  project += ".\\";
  project += staging;
  project += "\\";
  project += name;
  if (mt)
    project += "_mt_exe.dsp";
  else
    project += "_st_exe.dsp";
	begin_project(dsw, name.c_str(), project.c_str());
  if (mt)
  {
	  add_project_dependency(dsw, "CORE_magick");
    if (useX11Stubs)
	    add_project_dependency(dsw, "CORE_xlib");
    if (extn.compare("cpp") == 0)
    {
		  add_project_dependency(dsw, "CORE_MagickArgs");
		  add_project_dependency(dsw, "CORE_Magick++");
    }
  } else {
    CStringEx strDepends;
		for (
			std::list<std::string>::iterator it1a = dependency_list.begin();
			it1a != dependency_list.end();
			it1a++)
		{
      strDepends = (*it1a).c_str();
      if (strDepends.FindNoCase("LIBR_zlib",0) == 0)
		    add_project_dependency(dsw, strDepends );
		}

		for (
			std::list<std::string>::iterator it1b = dependency_list.begin();
			it1b != dependency_list.end();
			it1b++)
		{
      strDepends = (*it1b).c_str();
      if (strDepends.FindNoCase("LIBR_bzlib",0) == 0)
		    add_project_dependency(dsw, strDepends );
		}

		for (
			std::list<std::string>::iterator it1c = dependency_list.begin();
			it1c != dependency_list.end();
			it1c++)
		{
      strDepends = (*it1c).c_str();
      if (strDepends.FindNoCase("LIBR_jpeg",0) == 0)
		    add_project_dependency(dsw, strDepends );
		}

		for (
			std::list<std::string>::iterator it1 = dependency_list.begin();
			it1 != dependency_list.end();
			it1++)
		{
      strDepends = (*it1).c_str();
      if (strDepends.FindNoCase("LIBR_zlib",0) == 0)
        continue;
      if (strDepends.FindNoCase("LIBR_bzlib",0) == 0)
        continue;
      if (strDepends.FindNoCase("LIBR_jpeg",0) == 0)
        continue;
      if (strDepends.FindNoCase("LIBR_",0) == 0)
		    add_project_dependency(dsw, strDepends );
		}
		for (
			std::list<std::string>::iterator it2 = dependency_list.begin();
			it2 != dependency_list.end();
			it2++)
		{
      strDepends = (*it2).c_str();
      if (strDepends.FindNoCase("CORE_",0) == 0)
      {
        if (strDepends.Find("CORE_Magick",0) == 0)
          continue;
		    add_project_dependency(dsw, strDepends );
      }
		}
    if (extn.compare("cpp") == 0)
    {
		  for (
			  std::list<std::string>::iterator it3 = dependency_list.begin();
			  it3 != dependency_list.end();
			  it3++)
		  {
        strDepends = (*it3).c_str();
        if (strDepends.FindNoCase("CORE_Magick",0) == 0)
		      add_project_dependency(dsw, strDepends );
		  }
    }
		for (
			std::list<std::string>::iterator it4 = dependency_list.begin();
			it4 != dependency_list.end();
			it4++)
		{
      strDepends = (*it4).c_str();
      if (strDepends.FindNoCase("IM_MOD_",0) == 0)
		    add_project_dependency(dsw, strDepends );
		}
  }
	end_project(dsw);
}

void CConfigureApp::process_library(ofstream &dsw,
                      const char *filename, bool dll, bool mt)
{
	CStringEx basename = filename;
  std::string name = basename.GetField('.',0);
  std::string prefix = "CORE_";

	std::list<std::string> libs_list_release;
	std::list<std::string> libs_list_debug;
	std::list<std::string> includes_list;

	includes_list.push_back("..\\..\\zlib");

  std::string libpath;
  libpath = "..\\..\\";
  libpath += name;
  std::string dependency;
	WIN32_FIND_DATA	libdata;
	HANDLE libhandle = FindFirstFile(libpath.c_str(), &libdata);
	if (libhandle != INVALID_HANDLE_VALUE)
  {
		includes_list.push_back(libpath);

    FindClose(libhandle);
    // also look for a subdirectory of the form lib and
    // libxxx and add an include path if it exists
    libpath += "\\lib";
    libhandle = FindFirstFile(libpath.c_str(), &libdata);
	  if (libhandle != INVALID_HANDLE_VALUE)
    {
		  includes_list.push_back(libpath);
      FindClose(libhandle);
    }
    libpath += name;
    libhandle = FindFirstFile(libpath.c_str(), &libdata);
	  if (libhandle != INVALID_HANDLE_VALUE)
    {
		  includes_list.push_back(libpath);
      FindClose(libhandle);
    }
  }

	write_lib_dsp(
    dll,
    mt, // multi-threaded
    filename,
		"*",
		name,
		prefix,
		libs_list_shared,
		libs_list_release,
		libs_list_debug,
		defines_list,
    includes_list);

  std::string staging = filename;
  std::string project;
  std::string pname;
  pname = prefix + name;
  project += ".\\";
  project += staging;
  project += "\\";
  project += pname;
  if (dll)
    project += "_mt_dll.dsp";
  else
    project += "_st_lib.dsp";
	begin_project(dsw, pname.c_str(), project.c_str());
  if (dll)
  {
    if (name.compare("magick") == 0)
    {
      if (useX11Stubs)
	      add_project_dependency(dsw, "CORE_xlib");
	    add_project_dependency(dsw, "LIBR_JPEG");
	    add_project_dependency(dsw, "LIBR_ZLIB");
    }
    if (name.compare("Magick++") == 0)
    {
	    add_project_dependency(dsw, "CORE_magick");
    }
  }
  else
  {
		dependency_list.push_back(pname.c_str());
  }
	end_project(dsw);
}

void CConfigureApp::process_3rd_party_library(ofstream &dsw,
                      const char *filename, bool dll, bool mt)
{
	CStringEx basename = filename;
  std::string name = basename.GetField('.',0);
  std::string prefix = "LIBR_";

	std::list<std::string> libs_list_release;
	std::list<std::string> libs_list_debug;
	std::list<std::string> includes_list;
  std::string libpath;
  bool do_extra_stuff = false;

  libpath = "..\\..\\";
  libpath += name;

  std::string dependency;
	WIN32_FIND_DATA	libdata;
  HANDLE libhandle;

do_it_again:
	libhandle = FindFirstFile(libpath.c_str(), &libdata);
	if (libhandle != INVALID_HANDLE_VALUE)
  {
    FindClose(libhandle);

    std::string staging = filename;
    std::string project;
    std::string pname;
    pname = prefix + name;
    project += "..\\";
    project += staging;
    project += "\\";
    project += pname;
    if (dll)
      project += "_mt_dll.dsp";
    else
      project += "_st_lib.dsp";

    libhandle = FindFirstFile(project.c_str(), &libdata);
	  if (libhandle != INVALID_HANDLE_VALUE)
    {
      FindClose(libhandle);

      const char *path = project.c_str();
	    begin_project(dsw, pname.c_str(), &path[1]);
      if (dll)
      {
        if (name.compare("png") == 0)
        {
	        add_project_dependency(dsw, "LIBR_ZLIB");
        }
        if (name.compare("hdf") == 0)
        {
	        add_project_dependency(dsw, "LIBR_JPEG");
	        add_project_dependency(dsw, "LIBR_ZLIB");
        }
        if (name.compare("tiff") == 0)
        {
	        add_project_dependency(dsw, "LIBR_JPEG");
	        add_project_dependency(dsw, "LIBR_ZLIB");
        }
        if (name.compare("fpx") == 0)
        {
	        add_project_dependency(dsw, "LIBR_FPXjpeg");
          name = "FPXjpeg";
          do_extra_stuff = true;
        }
      }
      else
      {
		    dependency_list.push_back(pname.c_str());
      }
	    end_project(dsw);
      if (do_extra_stuff)
      {
        do_extra_stuff = false;
        goto do_it_again; // A total hack!
      }
    }
  }
}

void CConfigureApp::process_module(ofstream &dsw,
  WIN32_FIND_DATA	&data, const char *filename, bool dll)
{
	CStringEx basename = filename;
  std::string name = basename.GetField('.',0);
  std::string prefix = "IM_MOD_";

	std::list<std::string> libs_list_release;
	std::list<std::string> libs_list_debug;

  // look for a top level directory with the same name
  // as this file format and add an include directive in
  // order to allow it to see its include files
	std::list<std::string> includes_list;
  if (name.compare("png") == 0)
  {
		includes_list.push_back("..\\..\\zlib");
  }
  if (name.compare("label") == 0)
  {
		includes_list.push_back("..\\..\\ttf");
		includes_list.push_back("..\\..\\ttf\\lib");
  }
  if (name.compare("miff") == 0)
  {
		includes_list.push_back("..\\..\\zlib");
		includes_list.push_back("..\\..\\bzlib");
  }

  // generate the includes paths required for this module
  std::string libpath;
  libpath = "..\\..\\";
  libpath += name;
  std::string dependency;
	WIN32_FIND_DATA	libdata;
	HANDLE libhandle = FindFirstFile(libpath.c_str(), &libdata);
	if (libhandle != INVALID_HANDLE_VALUE)
  {
		includes_list.push_back(libpath);

    dependency = "LIBR_";
    dependency += name;

    FindClose(libhandle);
    // also look for a subdirectory of the form lib and
    // libxxx and add an include path if it exists
    libpath += "\\lib";
    libhandle = FindFirstFile(libpath.c_str(), &libdata);
	  if (libhandle != INVALID_HANDLE_VALUE)
    {
		  includes_list.push_back(libpath);
      FindClose(libhandle);
    }
    libpath += name;
    libhandle = FindFirstFile(libpath.c_str(), &libdata);
	  if (libhandle != INVALID_HANDLE_VALUE)
    {
		  includes_list.push_back(libpath);
      FindClose(libhandle);
    }
  }

  // look for any libs that exist and add them into the module
  libpath = "..\\..\\";
  libpath += name;
  libpath += "\\*.lib";
	libhandle = FindFirstFile(libpath.c_str(), &libdata);
	if (libhandle != INVALID_HANDLE_VALUE)
  {
    std::string extralibrary;

	  do
	  {
      extralibrary = "..\\..\\";
      extralibrary += name;
      extralibrary += "\\";
      extralibrary += libdata.cFileName;
		  libs_list_release.push_back(extralibrary);
		  libs_list_debug.push_back(extralibrary);
	  } while (FindNextFile(libhandle, &libdata));
    FindClose(libhandle);
  }

 	write_lib_dsp(
    dll,
    dll, // multi-threaded
    data.cFileName,
		"",
		name,
		prefix,
		libs_list_shared,
		libs_list_release,
		libs_list_debug,
		defines_list,
    includes_list);

  std::string project;
  std::string pname;
  pname = prefix + name;
  project += ".\\";
  project += data.cFileName;
  project += "\\";
  project += pname;
  if (dll)
    project += "_mt_dll.dsp";
  else
    project += "_st_lib.dsp";
	begin_project(dsw, pname.c_str(), project.c_str());
  if (dll)
  {
	  add_project_dependency(dsw, "CORE_magick");
    if (dependency.length() > 0)
	    add_project_dependency(dsw, dependency.c_str());
    if (name.compare("label") == 0)
    {
		  add_project_dependency(dsw, "LIBR_ttf");
      if (useX11Stubs)
		    add_project_dependency(dsw, "CORE_xlib");
    }
    if (name.compare("miff") == 0)
    {
	    add_project_dependency(dsw, "LIBR_ZLIB");
		  add_project_dependency(dsw, "LIBR_BZLIB");
    }
    if (name.compare("png") == 0)
    {
	    add_project_dependency(dsw, "LIBR_ZLIB");
    }
    if (name.compare("pdf") == 0)
    {
	    add_project_dependency(dsw, "IM_MOD_tiff");
    }
    if (name.compare("ps2") == 0)
    {
	    add_project_dependency(dsw, "IM_MOD_tiff");
    }
    if (name.compare("x") == 0)
    {
      if (useX11Stubs)
		    add_project_dependency(dsw, "CORE_xlib");
    }
    if (name.compare("xwd") == 0)
    {
      if (useX11Stubs)
		    add_project_dependency(dsw, "CORE_xlib");
    }
  }
  else
  {
		dependency_list.push_back(pname.c_str());
  }
	end_project(dsw);
}

BOOL CConfigureApp::InitInstance()
{
	// Standard initialization
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	CConfigureWizard wizard;
	m_pMainWnd = &wizard;

  release_loc = "..\\Release\\";
  debug_loc = "..\\Debug\\";
  bin_loc = "..\\bin\\";
  lib_loc = "..\\lib\\";

  wizard.m_Page2.m_useX11Stubs = useX11Stubs;
  wizard.m_Page2.m_decorateFiles = decorateFilenames;

  wizard.m_Page3.m_tempRelease = release_loc;
  wizard.m_Page3.m_tempDebug = debug_loc;
  wizard.m_Page3.m_outputBin = bin_loc;
  wizard.m_Page3.m_outputLib = lib_loc;

	int nResponse = wizard.DoModal();
  //if (nResponse == IDOK)
	{
    useX11Stubs = wizard.m_Page2.m_useX11Stubs;
    decorateFilenames = wizard.m_Page2.m_decorateFiles;
    release_loc = wizard.m_Page3.m_tempRelease;
    debug_loc = wizard.m_Page3.m_tempDebug;
    bin_loc = wizard.m_Page3.m_outputBin;
    lib_loc = wizard.m_Page3.m_outputLib;

    int projectType = wizard.m_Page2.m_projectType;

    CString theprojectname;
    switch (projectType)
    {
      case SINGLETHREADEDSTATIC:
	      theprojectname = "..\\VisualStaticST.dsw";
        break;
      case MULTITHREADEDSTATIC:
	      theprojectname = "..\\VisualStaticMT.dsw";
        break;
      default:
      case MULTITHREADEDDLL:
	      theprojectname = "..\\VisualDynamicMT.dsw";
        break;
    }
    ofstream dsw(theprojectname);

		write_dsw_start(dsw);

    // Write all library project files:
		//if (wizard.m_Page2.m_disptarget == 1)
    if (projectType == MULTITHREADEDDLL)
		{
      if (!useX11Stubs)
		    libs_list_shared.push_back("..\\lib\\X11.lib");
		  libs_list_shared.push_back("kernel32.lib");
		  libs_list_shared.push_back("user32.lib");
		  libs_list_shared.push_back("gdi32.lib");
#ifdef OLD_METHOD
		  libs_list_shared.push_back("winspool.lib");
		  libs_list_shared.push_back("comdlg32.lib");
		  libs_list_shared.push_back("advapi32.lib");
		  libs_list_shared.push_back("shell32.lib");
		  libs_list_shared.push_back("ole32.lib");
		  libs_list_shared.push_back("oleaut32.lib");
		  libs_list_shared.push_back("uuid.lib");
		  libs_list_shared.push_back("odbc32.lib");
		  libs_list_shared.push_back("odbccp32.lib");
#endif
			defines_list.push_back("_DLL");
		  defines_list.push_back("_MAGICKMOD_");
		}
    else
    {
		  libs_list_shared.push_back("kernel32.lib");
		  libs_list_shared.push_back("user32.lib");
		  libs_list_shared.push_back("gdi32.lib");
		  defines_list.push_back("_LIB");
    }

    const ConfigureInfo
      valid_dirs[] = {
      { "bzlib",        THIRDPARTY },
      { "fpx",          THIRDPARTY },
      { "hdf",          THIRDPARTY },
      { "jbig",         THIRDPARTY },
      { "jpeg",         THIRDPARTY },
      { "magick",       LIBRARY    },
      { "coders",       MODULE     },
      { "digimarc",     MODULE     },
      { "PerlMagick",   DISABLED   },
      { "png",          THIRDPARTY },
      { "tiff",         THIRDPARTY },
      { "ttf",          THIRDPARTY },
      { "xlib",         LIBRARY    },
      { "zlib",         THIRDPARTY },
      { "MagickArgs",   STATICLIB  },
      { "Magick++",     STATICLIB  },
      { "utilities",    UTILITY    },
      { "tests",        UTILITY    },
      { "Convert++",    UTILITY    },
		  { NULL,           -1 }
	  };

    int project_type = DISABLED;
    HANDLE handle;
    // Scan all top level directories and process the ones
    // that we are allowed to.
	  WIN32_FIND_DATA	data;
		for (int i=0; valid_dirs[i].name != NULL; i++)
    {
      std::string searchpath = "..\\..\\";
      searchpath += valid_dirs[i].name;

	    handle = FindFirstFile(searchpath.c_str(), &data);
	    if (handle == INVALID_HANDLE_VALUE)
        continue;
      project_type = valid_dirs[i].type;
	    do
	    {
		    if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ==
                FILE_ATTRIBUTE_DIRECTORY)
		    {
	        CString subpath;

          switch (project_type)
          {
          case UTILITY:
            {
	            subpath = "..\\..\\";
              subpath += data.cFileName;
              subpath += "\\*.c";
	            WIN32_FIND_DATA	subdata;
	            HANDLE subhandle = FindFirstFile(subpath, &subdata);
	            if (subhandle != INVALID_HANDLE_VALUE)
              {
	              do
	              {
                  process_utility(dsw, data, subdata.cFileName,
                    projectType == MULTITHREADEDDLL);
	              } while (FindNextFile(subhandle, &subdata));
                FindClose(subhandle);
              }

	            subpath = "..\\..\\";
              subpath += data.cFileName;
              subpath += "\\*.cpp";
	            subhandle = FindFirstFile(subpath, &subdata);
	            if (subhandle != INVALID_HANDLE_VALUE)
              {
	              do
	              {
                  process_utility(dsw, data, subdata.cFileName,
                    projectType == MULTITHREADEDDLL);
	              } while (FindNextFile(subhandle, &subdata));
                FindClose(subhandle);
              }
            }
            break;
          case LIBRARY:
		        defines_list.push_back("_MAGICKLIB_");
            if (projectType != MULTITHREADEDDLL)
              project_type=STATICLIB;
            process_library(dsw, data.cFileName, (project_type==LIBRARY),
                projectType == MULTITHREADEDDLL);
		        defines_list.pop_back();
            break;
          case STATICLIB:
            if (projectType != MULTITHREADEDDLL)
              project_type=STATICLIB;
            process_library(dsw, data.cFileName, (project_type==LIBRARY),
                projectType == MULTITHREADEDDLL);
            break;
          case MODULE:
            {
	            subpath = "..\\..\\";
              subpath += data.cFileName;
              subpath += "\\*.c";
	            WIN32_FIND_DATA	subdata;
	            HANDLE subhandle = FindFirstFile(subpath, &subdata);
	            if (subhandle != INVALID_HANDLE_VALUE)
              {
	              do
	              {
                  process_module(dsw, data, subdata.cFileName,
                    projectType == MULTITHREADEDDLL);
	              } while (FindNextFile(subhandle, &subdata));
                FindClose(subhandle);
              }

	            subpath = "..\\..\\";
              subpath += data.cFileName;
              subpath += "\\*.cpp";
	            subhandle = FindFirstFile(subpath, &subdata);
	            if (subhandle != INVALID_HANDLE_VALUE)
              {
	              do
	              {
                  process_module(dsw, data, subdata.cFileName,
                    projectType == MULTITHREADEDDLL);
	              } while (FindNextFile(subhandle, &subdata));
                FindClose(subhandle);
              }
            }
            break;
          case THIRDPARTY:
		        defines_list.push_back("_MAGICKLIB_");
            process_3rd_party_library(dsw, data.cFileName, (projectType == MULTITHREADEDDLL),
                projectType == MULTITHREADEDDLL);
		        defines_list.pop_back();
            break;
          }
        }
	    } while (FindNextFile(handle, &data));
      FindClose(handle);
    }
		write_dsw_end(dsw);
	}
/*	else if (nResponse == IDCANCEL)
	{
	}*/

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Visual C++ workspace write routine.

void CConfigureApp::write_dsw_start(ofstream &dsw)
{
	dsw << "Microsoft Developer Studio Workspace File, Format Version 6.00" << endl;
	dsw << "# WARNING: DO NOT EDIT OR DELETE THIS WORKSPACE FILE!" << endl;
	dsw << endl;
	dsw << "###############################################################################" << endl;
	dsw << endl;

	begin_project(dsw, "iptcutil", ".\\iptcutils\\iptcutil.dsp");
	end_project(dsw);
}

void CConfigureApp::write_dsw_end(ofstream &dsw)
{
	dsw << "Global:" << endl;
	dsw << endl;
	dsw << "Package=<5>" << endl;
	dsw << "{{{" << endl;
	dsw << "}}}" << endl;
	dsw << endl;
	dsw << "Package=<3>" << endl;
	dsw << "{{{" << endl;
	dsw << "}}}" << endl;
	dsw << endl;
	dsw << "###############################################################################" << endl;
	dsw << endl;
}

void CConfigureApp::begin_project(ofstream &dsw, const char *name, const char *filename)
{
	dsw << "Project: \"" << name << "\"=\"" << filename << "\" - Package Owner=<4>" << endl;
	dsw << endl;
	dsw << "Package=<5>" << endl;
	dsw << "{{{" << endl;
	dsw << "}}}" << endl;
	dsw << endl;
	dsw << "Package=<4>" << endl;
	dsw << "{{{" << endl;
}

void CConfigureApp::end_project(ofstream &dsw)
{
	dsw << "}}}" << endl;
	dsw << endl;
	dsw << "###############################################################################" << endl;
	dsw << endl;
}

void CConfigureApp::add_project_dependency(ofstream &dsw, const char *dep_name)
{
	dsw << "    Begin Project Dependency" << endl;
	dsw << "    Project_Dep_Name " << dep_name << endl;
	dsw << "    End Project Dependency" << endl;
}

void CConfigureApp::write_lib_dsp(
  bool dll,
  bool mt,
	std::string directory,
	std::string search,
	std::string dspname,
	std::string prefix,
	std::list<std::string> &libs_list_shared,
	std::list<std::string> &libs_list_release,
	std::list<std::string> &libs_list_debug,
	std::list<std::string> &defines_list,
	std::list<std::string> &includes_list)
{
	CString filename = "..\\";
  filename += directory.c_str();
  filename += "\\";
	filename += prefix.c_str();
	filename += dspname.c_str();
  if (dll)
    filename += "_mt_dll.dsp";
  else
    filename += "_st_lib.dsp";

  CString outname;

  CString libname = prefix.c_str();
  libname += dspname.c_str();

	ofstream dsp(filename);

	dsp << "# Microsoft Developer Studio Project File - Name=\"" << libname << "\" - Package Owner=<4>" << endl;
	dsp << "# Microsoft Developer Studio Generated Build File, Format Version 6.00" << endl;
	dsp << "# ** DO NOT EDIT **" << endl;
	dsp << endl;
  if (dll)
	  dsp << "# TARGTYPE \"Win32 (x86) Dynamic-Link Library\" 0x0102" << endl;
  else
	  dsp << "# TARGTYPE \"Win32 (x86) Static Library\" 0x0104" << endl;
	dsp << endl;
	dsp << "CFG=" << libname << " - Win32 Release" << endl;
	dsp << "!MESSAGE This is not a valid makefile. To build this project using NMAKE," << endl;
	dsp << "!MESSAGE use the Export Makefile command and run" << endl;
	dsp << "!MESSAGE " << endl;
	dsp << "!MESSAGE NMAKE /f \"" << libname << ".mak\"." << endl;
	dsp << "!MESSAGE " << endl;
	dsp << "!MESSAGE You can specify a configuration when running NMAKE" << endl;
	dsp << "!MESSAGE by defining the macro CFG on the command line. For example:" << endl;
	dsp << "!MESSAGE " << endl;
	dsp << "!MESSAGE NMAKE /f \"" << libname << ".mak\" CFG=\"" << libname << " - Win32 Release\"" << endl;
	dsp << "!MESSAGE " << endl;
	dsp << "!MESSAGE Possible choices for configuration are:" << endl;
	dsp << "!MESSAGE " << endl;
  if (dll)
  {
	  dsp << "!MESSAGE \"" << libname << " - Win32 Release\" (based on \"Win32 (x86) Dynamic-Link Library\")" << endl;
	  dsp << "!MESSAGE \"" << libname << " - Win32 Debug\" (based on \"Win32 (x86) Dynamic-Link Library\")" << endl;
  }
  else
  {
	  dsp << "!MESSAGE \"" << libname << " - Win32 Release\" (based on \"Win32 (x86) Static Library\")" << endl;
	  dsp << "!MESSAGE \"" << libname << " - Win32 Debug\" (based on \"Win32 (x86) Static Library\")" << endl;
  }
	dsp << "!MESSAGE " << endl;
	dsp << endl;
	dsp << "# Begin Project" << endl;
	dsp << "# PROP AllowPerConfigDependencies 0" << endl;
	dsp << "# PROP Scc_ProjName \"\"" << endl;
	dsp << "# PROP Scc_LocalPath \"\"" << endl;
	dsp << "CPP=cl.exe" << endl;
	dsp << "MTL=midl.exe" << endl;
	dsp << "RSC=rc.exe" << endl;
	dsp << endl;

	dsp << "!IF  \"$(CFG)\" == \"" << libname << " - Win32 Release\"" << endl;
	dsp << endl;

	dsp << "# PROP Use_MFC 0" << endl;
	dsp << "# PROP Use_Debug_Libraries 0" << endl;
	dsp << "# PROP Output_Dir \"" << lib_loc << "\"" << endl;
	dsp << "# PROP Intermediate_Dir \"" << release_loc << libname << "\"" << endl;
	dsp << "# PROP Target_Dir \"\"" << endl;
  if (dll)
	  dsp << "LIB32=link.exe -lib" << endl;
	dsp << "# ADD CPP /nologo";
  if (mt)
    dsp << " /MD";
  dsp << " /W3 /GX /O2";
  dsp << standard_include;
	{
		for (
			std::list<std::string>::iterator it = includes_list.begin();
			it != includes_list.end();
			it++)
		{
			dsp << " /I \"" << (*it).c_str() << "\"";
		}
	}
  dsp << " /D \"NDEBUG\" /D \"WIN32\" /D \"_WINDOWS\" /D \"_VISUALC_\" /D \"NeedFunctionPrototypes\"";
	{
		for (
			std::list<std::string>::iterator it = defines_list.begin();
			it != defines_list.end();
			it++)
		{
			dsp << " /D \"" << (*it).c_str() << "\"";
		}
	}
	dsp << " /YX /FD /c" << endl;

  dsp << "# ADD MTL /nologo /D \"NDEBUG\" /mktyplib203 /win32" << endl;
	dsp << "# ADD RSC /l 0x409 /d \"NDEBUG\"" << endl;
	dsp << "BSC32=bscmake.exe" << endl;
	dsp << "# ADD BSC32 /nologo" << endl;
  if (dll)
  {
	  dsp << "LINK32=link.exe" << endl;
	  dsp << "# ADD LINK32";
  }
  else
  {
	  dsp << "LIB32=link.exe -lib" << endl;
	  dsp << "# ADD LIB32";
  }

  if (dll)
  {
	  {
		  for (
			  std::list<std::string>::iterator it = libs_list_shared.begin();
			  it != libs_list_shared.end();
			  it++)
		  {
			  dsp << " " << (*it).c_str() << "";
		  }
	  }
  }
	{
		for (
			std::list<std::string>::iterator it = libs_list_release.begin();
			it != libs_list_release.end();
			it++)
		{
			dsp << " " << (*it).c_str() << "";
		}
	}
  if (dll)
  {
	  dsp << " /nologo /subsystem:windows /dll";
    if (search.compare("*") != 0)
    {
      dsp.setf( ios::hex );
      dsp << " /base:\"0x" << dllbaselocation << "\"";
      dllbaselocation += 0x00010000L;
    }
    dsp << " /incremental:no /machine:I386 ";
  }
  else
	  dsp << " /nologo /machine:I386 ";
  outname = prefix.c_str();
  outname += "RL_";
  outname += dspname.c_str();
  if (dll)
  {
    dsp << "/pdb:\"" << bin_loc << outname << "_.pdb\"";
    dsp << " /out:\"" << bin_loc << outname << "_.dll\"";
  }
  else
  {
    dsp << " /out:\"" << lib_loc << outname << "_.lib\"";
  }
	dsp << endl;
	dsp << endl;

	dsp << "!ELSEIF  \"$(CFG)\" == \"" << libname << " - Win32 Debug\"" << endl;
	dsp << endl;

	dsp << "# PROP Use_MFC 0" << endl;
	dsp << "# PROP Use_Debug_Libraries 1" << endl;
	dsp << "# PROP Output_Dir \"" << lib_loc << "\"" << endl;
	dsp << "# PROP Intermediate_Dir \"" << debug_loc << libname << "\"" << endl;
	dsp << "# PROP Target_Dir \"\"" << endl;
  if (dll)
	  dsp << "LIB32=link.exe -lib" << endl;
	dsp << "# ADD CPP /nologo";
  if (mt)
    dsp << " /MDd";
  dsp << " /W3 /Gm /GX /Zi /Od";
  dsp << standard_include;
	{
		for (
			std::list<std::string>::iterator it = includes_list.begin();
			it != includes_list.end();
			it++)
		{
			dsp << " /I \"" << (*it).c_str() << "\"";
		}
	}
  dsp << " /D \"_DEBUG\" /D \"WIN32\" /D \"_WINDOWS\" /D \"_VISUALC_\" /D \"NeedFunctionPrototypes\"";
	{
		for (
			std::list<std::string>::iterator it = defines_list.begin();
			it != defines_list.end();
			it++)
		{
			dsp << " /D \"" << (*it).c_str() << "\"";
		}
	}
	dsp << " /YX /FD /c" << endl;
	
	dsp << "# ADD MTL /nologo /D \"_DEBUG\" /mktyplib203 /win32" << endl;
	dsp << "# ADD RSC /l 0x409 /d \"_DEBUG\"" << endl;
	dsp << "BSC32=bscmake.exe" << endl;
	dsp << "# ADD BSC32 /nologo" << endl;
  if (dll)
  {
	  dsp << "LINK32=link.exe" << endl;
	  dsp << "# ADD LINK32";
  }
  else
  {
	  dsp << "LIB32=link.exe -lib" << endl;
	  dsp << "# ADD LIB32";
  }

  if (dll)
  {
	  {
		  for (
			  std::list<std::string>::iterator it = libs_list_shared.begin();
			  it != libs_list_shared.end();
			  it++)
		  {
			  dsp << " " << (*it).c_str() << "";
		  }
	  }
  }
	{
		for (
			std::list<std::string>::iterator it = libs_list_debug.begin();
			it != libs_list_debug.end();
			it++)
		{
			dsp << " " << (*it).c_str() << "";
		}
	}
  outname = prefix.c_str();
  outname += "DB_";
  outname += dspname.c_str();
  if (dll)
  {
	  dsp << " /nologo /subsystem:windows /dll";
    if (search.compare("*") != 0)
    {
      dsp.setf( ios::hex );
      dsp << " /base:\"0x" << dllbaselocation << "\"";
      dllbaselocation += 0x00010000L;
    }
    dsp << " /incremental:no /debug /machine:I386 ";
  }
  else
	  dsp << " /nologo /debug /machine:I386 ";
  if (dll)
  {
    dsp << "/pdb:\"" << bin_loc << outname << "_.pdb\"";
    dsp << " /out:\"" << bin_loc << outname << "_.dll\"";
  }
  else
  {
    dsp << " /out:\"" << lib_loc << outname << "_.lib\"";
  }
	dsp << endl;

	dsp << "!ENDIF " << endl;
	dsp << endl;
	dsp << "# Begin Target" << endl;
	dsp << endl;
	dsp << "# Name \"" << libname << " - Win32 Release\"" << endl;
	dsp << "# Name \"" << libname << " - Win32 Debug\"" << endl;

  CString dir;
  CString spec;

	begin_group(dsp, "src");
	dir = "..\\..\\";
  dir += directory.c_str();
  dir += "\\";
  if (search.length() > 0)
	  spec = search.c_str();
  else
	  spec = dspname.c_str();
  spec += ".c";
	generate_dir(dsp, dir, spec);

	dir = "..\\..\\";
  dir += directory.c_str();
  dir += "\\lib\\";
  if (search.length() > 0)
	  spec = search.c_str();
  else
	  spec = dspname.c_str();
  spec += ".c";
	generate_dir(dsp, dir, spec);

	dir = "..\\..\\";
  dir += directory.c_str();
  dir += "\\";
  if (search.length() > 0)
	  spec = search.c_str();
  else
	  spec = dspname.c_str();
  spec += ".cpp";
	generate_dir(dsp, dir, spec);

	dir = "..\\..\\";
  dir += directory.c_str();
  dir += "\\lib\\";
  if (search.length() > 0)
	  spec = search.c_str();
  else
	  spec = dspname.c_str();
  spec += ".cpp";
	generate_dir(dsp, dir, spec);
	end_group(dsp);

	begin_group(dsp, "include");
	dir = "..\\..\\";
  dir += directory.c_str();
  dir += "\\";
  if (search.length() > 0)
	  spec = search.c_str();
  else
	  spec = dspname.c_str();
  spec += ".h";
	generate_dir(dsp, dir, spec);

	dir = "..\\..\\";
  dir += directory.c_str();
  dir += "\\lib\\";
  if (search.length() > 0)
	  spec = search.c_str();
  else
	  spec = dspname.c_str();
  spec += ".h";
	generate_dir(dsp, dir, spec);
	end_group(dsp);

	// End .dsp file:
	dsp << "# End Target" << endl;
	dsp << "# End Project" << endl;
}

void CConfigureApp::write_exe_dsp(
  bool mt,
	std::string directory,
	std::string search,
	std::string dspname,
	std::string prefix,
	std::list<std::string> &libs_list_shared,
	std::list<std::string> &libs_list_release,
	std::list<std::string> &libs_list_debug,
	std::list<std::string> &defines_list,
	std::list<std::string> &includes_list)
{
	CString filename = "..\\";
  filename += directory.c_str();
  filename += "\\";
	filename += prefix.c_str();
	filename += dspname.c_str();
  if (mt)
	  filename += "_mt_exe.dsp";
  else
	  filename += "_st_exe.dsp";

  CString outname;

  CString libname = prefix.c_str();
  libname += dspname.c_str();

	ofstream dsp(filename);

	dsp << "# Microsoft Developer Studio Project File - Name=\"" << libname << "\" - Package Owner=<4>" << endl;
	dsp << "# Microsoft Developer Studio Generated Build File, Format Version 6.00" << endl;
	dsp << "# ** DO NOT EDIT **" << endl;
	dsp << endl;
	dsp << "# TARGTYPE \"Win32 (x86) Console Application\" 0x0103" << endl;
	dsp << endl;
	dsp << "CFG=" << libname << " - Win32 Release" << endl;
	dsp << "!MESSAGE This is not a valid makefile. To build this project using NMAKE," << endl;
	dsp << "!MESSAGE use the Export Makefile command and run" << endl;
	dsp << "!MESSAGE " << endl;
	dsp << "!MESSAGE NMAKE /f \"" << libname << ".mak\"." << endl;
	dsp << "!MESSAGE " << endl;
	dsp << "!MESSAGE You can specify a configuration when running NMAKE" << endl;
	dsp << "!MESSAGE by defining the macro CFG on the command line. For example:" << endl;
	dsp << "!MESSAGE " << endl;
	dsp << "!MESSAGE NMAKE /f \"" << libname << ".mak\" CFG=\"" << libname << " - Win32 Release\"" << endl;
	dsp << "!MESSAGE " << endl;
	dsp << "!MESSAGE Possible choices for configuration are:" << endl;
	dsp << "!MESSAGE " << endl;
	dsp << "!MESSAGE \"" << libname << " - Win32 Release\" (based on \"Win32 (x86) Console Application\")" << endl;
	dsp << "!MESSAGE \"" << libname << " - Win32 Debug\" (based on \"Win32 (x86) Console Application\")" << endl;
	dsp << "!MESSAGE " << endl;
	dsp << endl;
	dsp << "# Begin Project" << endl;
	dsp << "# PROP AllowPerConfigDependencies 0" << endl;
	dsp << "# PROP Scc_ProjName \"\"" << endl;
	dsp << "# PROP Scc_LocalPath \"\"" << endl;
	dsp << "CPP=cl.exe" << endl;
	dsp << "MTL=midl.exe" << endl;
	dsp << "RSC=rc.exe" << endl;
	dsp << endl;

	dsp << "!IF  \"$(CFG)\" == \"" << libname << " - Win32 Release\"" << endl;
	dsp << endl;

	dsp << "# PROP Use_MFC 0" << endl;
	dsp << "# PROP Use_Debug_Libraries 0" << endl;
	dsp << "# PROP Output_Dir \"" << bin_loc << "\"" << endl;
	dsp << "# PROP Intermediate_Dir \"" << release_loc << libname << "\"" << endl;
	dsp << "# PROP Target_Dir \"\"" << endl;
	dsp << "LIB32=link.exe -lib" << endl;
	dsp << "# ADD CPP /nologo";
  if (mt)
    dsp << " /MD";
  dsp << " /W3 /GX /O2";
  dsp << standard_include;
	{
		for (
			std::list<std::string>::iterator it = includes_list.begin();
			it != includes_list.end();
			it++)
		{
			dsp << " /I \"" << (*it).c_str() << "\"";
		}
	}
  dsp << " /D \"NDEBUG\" /D \"WIN32\" /D \"_CONSOLE\" /D \"_WINDOWS\" /D \"_VISUALC_\" /D \"NeedFunctionPrototypes\"";
	{
		for (
			std::list<std::string>::iterator it = defines_list.begin();
			it != defines_list.end();
			it++)
		{
			dsp << " /D \"" << (*it).c_str() << "\"";
		}
	}
	dsp << " /YX /FD /c" << endl;

  dsp << "# ADD MTL /nologo /D \"NDEBUG\" /mktyplib203 /win32" << endl;
	dsp << "# ADD RSC /l 0x409 /d \"NDEBUG\"" << endl;
	dsp << "BSC32=bscmake.exe" << endl;
	dsp << "# ADD BSC32 /nologo" << endl;
	dsp << "LINK32=link.exe" << endl;

	dsp << "# ADD LINK32";
	{
		for (
			std::list<std::string>::iterator it = libs_list_shared.begin();
			it != libs_list_shared.end();
			it++)
		{
			dsp << " " << (*it).c_str() << "";
		}
	}
	{
		for (
			std::list<std::string>::iterator it = libs_list_release.begin();
			it != libs_list_release.end();
			it++)
		{
			dsp << " " << (*it).c_str() << "";
		}
	}
	dsp << " /nologo /subsystem:console /incremental:no /machine:I386 ";
  if (decorateFilenames)
    outname = prefix.c_str();
  else
    outname = "";
  if (decorateFilenames)
    outname += "RL_";
  outname += dspname.c_str();
  dsp << "/pdb:\"" << bin_loc << outname;
  if (decorateFilenames)
    dsp << "_";
  dsp << ".pdb\"";
  dsp << " /out:\"" << bin_loc << outname;
  if (decorateFilenames)
    dsp << "_";
  dsp << ".exe\"";
	dsp << endl;
	dsp << endl;

	dsp << "!ELSEIF  \"$(CFG)\" == \"" << libname << " - Win32 Debug\"" << endl;
	dsp << endl;

	dsp << "# PROP Use_MFC 0" << endl;
	dsp << "# PROP Use_Debug_Libraries 1" << endl;
	dsp << "# PROP Output_Dir \"" << bin_loc << "\"" << endl;
	dsp << "# PROP Intermediate_Dir \"" << debug_loc << libname << "\"" << endl;
	dsp << "# PROP Target_Dir \"\"" << endl;
	dsp << "LIB32=link.exe -lib" << endl;
	dsp << "# ADD CPP /nologo";
  if (mt)
    dsp << " /MDd";
  dsp << " /W3 /Gm /GX /Zi /Od";
  dsp << standard_include;
	{
		for (
			std::list<std::string>::iterator it = includes_list.begin();
			it != includes_list.end();
			it++)
		{
			dsp << " /I \"" << (*it).c_str() << "\"";
		}
	}
  dsp << " /D \"_DEBUG\" /D \"WIN32\" /D \"_CONSOLE\" /D \"_WINDOWS\" /D \"_VISUALC_\" /D \"NeedFunctionPrototypes\"";
	{
		for (
			std::list<std::string>::iterator it = defines_list.begin();
			it != defines_list.end();
			it++)
		{
			dsp << " /D \"" << (*it).c_str() << "\"";
		}
	}
	dsp << " /YX /FD /c" << endl;
	
	dsp << "# ADD MTL /nologo /D \"_DEBUG\" /mktyplib203 /win32" << endl;
	dsp << "# ADD RSC /l 0x409 /d \"_DEBUG\"" << endl;
	dsp << "BSC32=bscmake.exe" << endl;
	dsp << "# ADD BSC32 /nologo" << endl;
	dsp << "LINK32=link.exe" << endl;

	dsp << "# ADD LINK32";
	{
		for (
			std::list<std::string>::iterator it = libs_list_shared.begin();
			it != libs_list_shared.end();
			it++)
		{
			dsp << " " << (*it).c_str() << "";
		}
	}
	{
		for (
			std::list<std::string>::iterator it = libs_list_debug.begin();
			it != libs_list_debug.end();
			it++)
		{
			dsp << " " << (*it).c_str() << "";
		}
	}
  if (decorateFilenames)
    outname = prefix.c_str();
  else
    outname = "";
  if (decorateFilenames)
    outname += "DB_";
  outname += dspname.c_str();
	dsp << " /nologo /subsystem:console /incremental:no /debug /machine:I386 ";
  dsp << "/pdb:\"" << bin_loc << outname;
  if (decorateFilenames)
    dsp << "_";
  dsp << ".pdb\"";
  dsp << " /out:\"" << bin_loc << outname;
  if (decorateFilenames)
    dsp << "_";
  dsp << ".exe\"";
	dsp << endl;

	dsp << "!ENDIF " << endl;
	dsp << endl;
	dsp << "# Begin Target" << endl;
	dsp << endl;
	dsp << "# Name \"" << libname << " - Win32 Release\"" << endl;
	dsp << "# Name \"" << libname << " - Win32 Debug\"" << endl;

  CString dir;
  CString spec;
	begin_group(dsp, "src");
	dir = "..\\..\\";
	//if (stricmp(directory.c_str(), "utilities") != 0)
  {
    dir += directory.c_str();
    dir += "\\";
  }
  if (search.length() > 0)
	  spec = search.c_str();
  else
	  spec = dspname.c_str();
  spec += ".c";
	generate_dir(dsp, dir, spec);

	dir = "..\\..\\";
  dir += directory.c_str();
  dir += "\\";
  if (search.length() > 0)
	  spec = search.c_str();
  else
	  spec = dspname.c_str();
  spec += ".cpp";
	generate_dir(dsp, dir, spec);
	end_group(dsp);

	begin_group(dsp, "include");
	dir = "..\\..\\";
  dir += directory.c_str();
  dir += "\\";
  if (search.length() > 0)
	  spec = search.c_str();
  else
	  spec = dspname.c_str();
  spec += ".h";
	generate_dir(dsp, dir, spec);
	end_group(dsp);

	// End .dsp file:
	dsp << "# End Target" << endl;
	dsp << "# End Project" << endl;
}

void CConfigureApp::generate_dir(ofstream &dsp, const char *dir, const char *spec)
{
	CString path = dir;
	if (path.GetAt(path.GetLength() - 1) != '\\') {
    path += "\\";
  }

	WIN32_FIND_DATA	data;
	HANDLE handle = FindFirstFile(path + spec, &data);
	if (handle == INVALID_HANDLE_VALUE) return;

	static const char *exclude_from_build[] =
	{
		".",
		"..",
		NULL
	};

	do
	{
		bool skip = false;

		for (int i=0; exclude_from_build[i] != NULL; i++)
			if (stricmp(data.cFileName, exclude_from_build[i]) == 0) skip = true;

		if (skip) continue;

		if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
		{
			//begin_group(dsp, data.cFileName + postfix);
			generate_dir(dsp, path + data.cFileName, spec);
			//end_group(dsp);
		}
		else
		{
			add_file(dsp, path + data.cFileName);
		}

	} while (FindNextFile(handle, &data));
  FindClose(handle);
}

void CConfigureApp::begin_group(ofstream &dsp, const char *group_name)
{
	dsp << "# Begin Group \"" << group_name << "\"" << endl;
	dsp << endl;
	dsp << "# PROP Default_Filter \"\"" << endl;
}

void CConfigureApp::end_group(ofstream &dsp)
{
	dsp << "# End Group" << endl;
}

void CConfigureApp::add_file(ofstream &dsp, const char *filename)
{
	dsp << "# Begin Source File" << endl;
	dsp << endl;
	dsp << "SOURCE=" << filename << endl;
	dsp << "# End Source File" << endl;
}

/////////////////////////////////////////////////////////////////////////////
// Browse helper function:

BOOL BrowseForFolder(HWND hOwner, char* szTitle, char* szRetval)
{
	BROWSEINFO		info;
	LPITEMIDLIST	itemidlist;
	char			szDirectory[_MAX_PATH];
	LPMALLOC		pMalloc;

	memset(szDirectory, '\0', _MAX_PATH);

	if (::SHGetMalloc(&pMalloc) == NOERROR)
	{
		info.hwndOwner = hOwner;
		info.pidlRoot = NULL;
		info.pszDisplayName = szDirectory;
		info.lpszTitle = szTitle;
		info.ulFlags = 0;
		info.lpfn = NULL;

		itemidlist = SHBrowseForFolder(&info);
		if (itemidlist != NULL)
		{
			SHGetPathFromIDList(itemidlist, szRetval);
			pMalloc->Free(itemidlist);
			pMalloc->Release();
			return TRUE;
		}
		else // User clicked Cancel
		{
			pMalloc->Release();
			return FALSE;
		}
	}
	else
		return FALSE;
}

int Replace( LPCTSTR str, LPCTSTR pcszKey, 
              LPCTSTR pcszReplacement,
                BOOL bIgnoreCase = FALSE )
{
	ASSERT( pcszKey != NULL );
	ASSERT( *pcszKey != _T('\0') );
	ASSERT( pcszReplacement != NULL );
	
	if ( pcszKey == NULL ||
        *pcszKey == _T('\0') ||
          pcszReplacement == NULL )
		return -1;
	
	int iStrLength = lstrlen( str );
	int iKeyLength = lstrlen( pcszKey );
	
	// A quick optimization; if the key is larger than our current string,
	// it cannot exist within -- get out of here.
	
	if ( iKeyLength > iStrLength )
		return 0;
	
	// First, figure out how large a buffer we need. If the replacement
	// is smaller or equal to the key, we can simply allocate the same
	// size as the number of characters currently in the string.  If not,
	// we can figure out the maximum number of keys that will be found,
	// multiply by the size differential and allocate that much more.
	// Once we have a number, we can allocate the buffer.
	
	int iBufLength;
	int iRepLength = lstrlen( pcszReplacement );
	
	if ( iRepLength <= iKeyLength )
	{
		iBufLength = iStrLength + 1;
	}
	else
	{
		int iMaxKeys = iStrLength / iKeyLength + 1;
		int iDelta = iRepLength - iKeyLength;
		iBufLength = iStrLength + iMaxKeys * iDelta + 1;
	}
	
	LPTSTR p = new TCHAR[iBufLength];
	memset( p, 0, iBufLength );
	
	if ( p == NULL )
		return -1;
	
	// declare some strings to handle case insensitivity
	CString strStr( str );
	CString strKey( pcszKey );
	
	if ( bIgnoreCase )
	{
		strStr.MakeUpper();
		strKey.MakeUpper();
	}
	
	// set up some loop controls and get to work!
	LPCTSTR pSource = str;
	LPCTSTR pCurr = strStr;
	LPTSTR pOut = p;
	int iReplacements = 0;
	
	while ( *pCurr != _T('\0') )
	{
		// if we find a match...
		if ( _tcsnccmp( pCurr, strKey, iKeyLength ) == 0 )
		{
			// copy the replacement string into the output buffer
			lstrcpy( pOut, pcszReplacement );
			
			// Increment the output pointer by the string just copied.  
			// Increment the source and current pointers by the key length.
			// Increment the replacement count.
			
			pOut += iRepLength;
			pSource += iKeyLength;
			pCurr += iKeyLength;			
			iReplacements++;
		}
		else
		{
			// otherwise, copy the character across and increment pointers
			*( pOut++ ) = *( pSource++ );
			pCurr++;
		}
	}
	
	str = p;
	delete [] p;
	return iReplacements;
}

BOOL ParseReplacement( LPCTSTR p, CString& strKey, CString& strData )
{
	CString strTmp( p );
	int iIndex = strTmp.Find( _T('=') );
	
	if ( iIndex > -1 )
	{
		strKey = strTmp.Left( iIndex );
		strData = strTmp.Right( strTmp.GetLength() - iIndex - 1 );
		return !strKey.IsEmpty() && !strData.IsEmpty();
	}
	
	return FALSE;
}

BOOL ConvertTemplate( char *input, CStringList* pReplacements )
{
	if ( ( pReplacements != NULL ) && !pReplacements->IsEmpty() )
	{
		CString strKey, strData;
		
		for ( POSITION pos = pReplacements->GetHeadPosition();
		pos != NULL;
		pReplacements->GetNext( pos ) )
		{
			CString& str = pReplacements->GetAt( pos );			
			if ( ParseReplacement( str, strKey, strData ) )
			{
				Replace( input, strKey, strData );
			}
		}
	}
  return TRUE;
}

int ReadTemplateFile( CString inputfile, CString outputfile )
{
  CStringList Replacements;
  char szBuf[2048];
  int nLinesRead = 0;
	ofstream dsp(outputfile);
  ifstream inpStream;
  inpStream.open( inputfile, ios::in | ios::nocreate );
  if ( inpStream ) {
    for (;;) {
      inpStream.getline( szBuf, 2047, '\n' );
      if ( inpStream.eof() ) {
        // last line may contain text also
        // (if it's not terminated with '\n' EOF is returned)
        return ConvertTemplate( szBuf, &Replacements );
      }
      if ( !inpStream.good() || !ConvertTemplate( szBuf, &Replacements ) )
        return FALSE;      
      dsp << szBuf << endl;
      nLinesRead++;
    }
  }
  inpStream.close();
  inpStream.clear();
  return nLinesRead;
}
