// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000
//
// Definition of types and classes to support threads
//
// This class is a Magick++ implementation class and is not intended
// for use by end-users.
// 
#if !defined (Thread_header)
#define Thread_header

#include "Magick++/Include.h"

#if defined(HasPTHREADS)
# include <pthread.h>
#endif

namespace Magick
{
  // Mutex lock wrapper
  class MutexLock
  {
  public:
    // Default constructor
    MutexLock(void);

    // Destructor
    ~MutexLock(void);

    // Lock mutex
    void lock(void);

    // Unlock mutex
    void unlock(void);

  private:

    // Don't support copy constructor
    MutexLock ( const MutexLock& original_ );
    
    // Don't support assignment
    MutexLock& operator = ( const MutexLock& original_ );

#if defined(HasPTHREADS)
    pthread_mutex_t  _mutex;
#endif
  };

  // Lock mutex while object is in scope
  class Lock
  {
  public:
    // Construct with mutex lock (locks mutex)
    Lock(  MutexLock &mutexLock_ );

    // Destrutor (unlocks mutex)
    ~Lock( void );
  private:

    // Don't support copy constructor
    Lock ( const Lock& original_ );
    
    // Don't support assignment
    Lock& operator = ( const Lock& original_ );

    MutexLock* _mutexLock;
  };
}

// Construct with mutex lock (locks mutex)
inline Magick::Lock::Lock( MutexLock &mutexLock_ )
{
  _mutexLock = &mutexLock_;
  _mutexLock->lock();
}

// Destrutor (unlocks mutex)
inline Magick::Lock::~Lock( void )
{
  _mutexLock->unlock();
}

#endif Thread_header
