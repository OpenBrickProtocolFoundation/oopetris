// //////////////////////////////////////////////////////////
// sha256.h
// Copyright (c) 2014,2015 Stephan Brumme. All rights reserved.
// see http://create.stephan-brumme.com/disclaimer.html
//

// Altered by Totto16 to fix some bugs and compiler warnings / errors

#pragma once

//#include "hash.h"
#include <string>

// define fixed size integer types
#if defined(_MSC_VER) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
// Windows
typedef unsigned __int8 uint8_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;

#if defined(OOPETRIS_LIBRARY_CORE_TYPE) && OOPETRIS_LIBRARY_CORE_TYPE == 0
#if defined(OOPETRIS_LIBRARY_CORE_EXPORT)
#if defined(__GNUC__)
#define HASH_LIBRARY_EXPORTED __attribute__((dllexport))
#else
#define HASH_LIBRARY_EXPORTED __declspec(dllexport)
#endif
#else
#if defined(__GNUC__)
#define HASH_LIBRARY_EXPORTED __attribute__((dllimport))
#else
#define HASH_LIBRARY_EXPORTED __declspec(dllimport)
#endif
#endif
#else
#define HASH_LIBRARY_EXPORTED  __attribute__((visibility("default")))
#endif

#else
// GCC
#include <stdint.h>

#define HASH_LIBRARY_EXPORTED
#endif

namespace hash_library {

    /// compute SHA256 hash
    /** Usage:
    SHA256 sha256;
    std::string myHash  = sha256("Hello World");     // std::string
    std::string myHash2 = sha256("How are you", 11); // arbitrary data, 11 bytes

    // or in a streaming fashion:

    SHA256 sha256;
    while (more data available)
      sha256.add(pointer to fresh data, number of new bytes);
    std::string myHash3 = sha256.getHash();
  */
    class SHA256 //: public Hash
    {
    public:
        /// split into 64 byte blocks (=> 512 bits), hash is 32 bytes long
        enum { BlockSize = 512 / 8, HashBytes = 32 };

        /// same as reset()
        HASH_LIBRARY_EXPORTED SHA256();

        /// compute SHA256 of a memory block
        HASH_LIBRARY_EXPORTED std::string operator()(const void* data, size_t numBytes);
        /// compute SHA256 of a string, excluding final zero
        HASH_LIBRARY_EXPORTED std::string operator()(const std::string& text);

        /// add arbitrary number of bytes
        HASH_LIBRARY_EXPORTED void add(const void* data, size_t numBytes);

        /// return latest hash as 64 hex characters
        HASH_LIBRARY_EXPORTED std::string getHash();
        /// return latest hash as bytes
        HASH_LIBRARY_EXPORTED void getHash(unsigned char buffer[HashBytes]);

        /// restart
        HASH_LIBRARY_EXPORTED void reset();

    private:
        /// process 64 bytes
        void processBlock(const void* data);
        /// process everything left in the internal buffer
        void processBuffer();

        /// size of processed data in bytes
        uint64_t m_numBytes;
        /// valid bytes in m_buffer
        size_t m_bufferSize;
        /// bytes not processed yet
        uint8_t m_buffer[BlockSize];

        enum { HashValues = HashBytes / 4 };
        /// hash, stored as integers
        uint32_t m_hash[HashValues];
    };

} // namespace hash_library
