// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999
//
// Test reading/writing BLOBs using Magick++
//

#include <string>
#include <iostream>
#include <fstream>

#include <Magick++.h>

using namespace std;

using namespace Magick;

// A derived Blob class to exercise updateNoCopy()
class myBlob : public Blob
{
public:
  // Construct from open binary stream
  myBlob( std::ifstream &stream_ )
    : Blob::Blob()
    {
      unsigned char* blobData = new unsigned char[100000];
      stream_.read( (char*)blobData, 100000 );
      size_t blobLen =  stream_.gcount();
      // Insert data into blob
      updateNoCopy( (void*)blobData, blobLen );
    }
};


int main( int /*argc*/, char **/*argv*/)
{
  int failures=0;

  try {
    
    //
    // Test reading BLOBs
    //
    {
      string signature("");
      {
	Image image("test_image.miff");
	signature = image.signature();
      }

      // Read raw data from file into BLOB
      ifstream in( "test_image.miff", ios::in | ios::binary );
      if( !in )
	{
	  cout << "Failed to open file for input!" << endl;
	  exit(1);
	}
      unsigned char* blobData = new unsigned char[100000];
      in.read( (char*)blobData, 100000 );
      size_t blobLen =  in.gcount();
      in.close();

      // Construct Magick++ Blob
      Blob blob((const void*)blobData, blobLen);
      delete blobData;

      // If construction of image fails, an exception should be thrown
      {
	// Construct with blob data only
	Image image( blob );
	if ( image.signature() != signature )
	  {
	    ++failures;
	    cout << "Line: " << __LINE__
		 << "  Image signature "
		 << image.signature()
		 << " != "
		 << signature << endl;
	  }
      }

      {
	// Construct with image geometry and blob data
	Image image( Geometry(148,99), blob );
	if ( image.signature() != signature )
	  {
	    ++failures;
	    cout << "Line: " << __LINE__
		 << "  Image signature "
		 << image.signature()
		 << " != "
		 << signature << endl;
	  }
      }

      {
	// Construct default image, and then read in blob data
	Image image;
	image.read( blob );
	if ( image.signature() != signature )
	  {
	    ++failures;
	    cout << "Line: " << __LINE__
		 << "  Image signature "
		 << image.signature()
		 << " != "
		 << signature << endl;
	  }
      }

      {
	// Construct default image, and then read in blob data with
	// image geometry
	Image image;
	image.read( Geometry(148,99), blob );
	if ( image.signature() != signature )
	  {
	    ++failures;
	    cout << "Line: " << __LINE__
		 << "  Image signature "
		 << image.signature()
		 << " != "
		 << signature << endl;
	  }
      }

    }

    // Test writing BLOBs
    {
      Blob blob;
      string signature("");
      {
	Image image("test_image.miff");
	image.magick("MIFF");
	image.write( &blob, 40000 );
	signature = image.signature();
      }
      {
	Image image(blob);
	if ( image.signature() != signature )
	  {
	    ++failures;
	    cout << "Line: " << __LINE__
		 << "  Image signature "
		 << image.signature()
		 << " != "
		 << signature << endl;
	    image.display();
	  }
      }
      
    }
    // Test writing BLOBs via STL writeImages
    {
      Blob blob;

      list<Image> first;
      readImages( &first, "test_image_anim.miff" );
      writeImages( first.begin(), first.end(), &blob, true );
    }

    // Test constructing a BLOB from a derived class
    {

      string signature("");
      {
	Image image("test_image.miff");
	signature = image.signature();
      }

      // Read raw data from file into BLOB
      ifstream in( "test_image.miff", ios::in | ios::binary );
      if( !in )
	{
	  cout << "Failed to open file for input!" << endl;
	  exit(1);
	}

      myBlob blob( in );
      in.close();

      Image image( blob );
      if ( image.signature() != signature )
	{
	  ++failures;
	  cout << "Line: " << __LINE__
	       << "  Image signature "
	       << image.signature()
	       << " != "
	       << signature << endl;
	}
    }
  }
  
  catch( Exception error_ )
    {
      cout << "Caught exception: " << error_ << endl;
      return 1;
    }
  catch( exception error_ )
    {
      cout << "Caught exception: " << error_.what() << endl;
      return 1;
    }

  if ( failures )
    {
      cout << failures << " failures" << endl;
      return 1;
    }
  
  return 0;
}


