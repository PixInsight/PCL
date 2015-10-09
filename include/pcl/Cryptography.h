//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// pcl/Cryptography.h - Released 2015/10/08 11:24:12 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Redistribution and use in both source and binary forms, with or without
// modification, is permitted provided that the following conditions are met:
//
// 1. All redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
// 2. All redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// 3. Neither the names "PixInsight" and "Pleiades Astrophoto", nor the names
//    of their contributors, may be used to endorse or promote products derived
//    from this software without specific prior written permission. For written
//    permission, please contact info@pixinsight.com.
//
// 4. All products derived from this software, in any form whatsoever, must
//    reproduce the following acknowledgment in the end-user documentation
//    and/or other materials provided with the product:
//
//    "This product is based on software from the PixInsight project, developed
//    by Pleiades Astrophoto and its contributors (http://pixinsight.com/)."
//
//    Alternatively, if that is where third-party acknowledgments normally
//    appear, this acknowledgment must be reproduced in the product itself.
//
// THIS SOFTWARE IS PROVIDED BY PLEIADES ASTROPHOTO AND ITS CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL PLEIADES ASTROPHOTO OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, BUSINESS
// INTERRUPTION; PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; AND LOSS OF USE,
// DATA OR PROFITS) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
// ----------------------------------------------------------------------------

#ifndef __PCL_Cryptography_h
#define __PCL_Cryptography_h

/// \file pcl/Cryptography.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_UIObject_h
#include <pcl/UIObject.h>
#endif

#ifndef __PCL_ByteArray_h
#include <pcl/ByteArray.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup cryptography_classes Cryptographic Hash and Encryption Classes
 */

/*!
 * \class CryptographicHash
 * \brief Abstract base class for cryptographic hashing algorithm
 * implementations
 *
 * %CryptographicHash defines a common interface for implementations of
 * cryptographic hashing algorithms such as MD-5, SHA-1 or SHA-256.
 *
 * \ingroup cryptography_classes
 * \sa MD5, SHA1, SHA256
 */
class PCL_CLASS CryptographicHash : public UIObject
{
public:

   /*!
    * Destroys a %CryptographicHash object.
    */
   virtual ~CryptographicHash()
   {
   }

   /*!
    * Ensures that the server-side object managed by this instance is uniquely
    * referenced.
    *
    * Since cryptographic hashes are unique objects by definition, calling this
    * member function has no effect.
    */
   virtual void EnsureUnique()
   {
      // Unique by definition
   }

   /*!
    * Returns the name of this cryptographic hashing algorithm.
    */
   virtual String AlgorithmName() const = 0;

   /*!
    * Returns the length in bytes of a hash sequence (or message digest)
    * calculated with this cryptographic hashing algorithm.
    */
   virtual size_type HashLength() const = 0;

   /*!
    * Initializes this cryptographic hash generator.
    */
   virtual void Initialize();

   /*!
    * Updates the hash generator with a new \a data chunk of the specified
    * \a length in bytes.
    */
   virtual void Update( const void* data, size_type length );

   /*!
    * Updates the hash generator with data from a container with contiguous
    * storage.
    *
    * The \c C template argument must be a class with PCL container semantics
    * (the Begin() and Size() member functions are required). Typically the
    * ByteArray class is used, although any array-like class with contiguous
    * storage is appropriate, including Array, String, Vector and Matrix.
    */
   template <class C>
   void Update( const C& data )
   {
      Update( data.Begin(), data.Size() );
   }

   /*!
    * Terminates hash generation and returns the computed message digest
    * as a ByteArray instance.
    */
   ByteArray Finalize()
   {
      ByteArray hash( HashLength() );
      Finalize( hash.Begin() );
      return hash;
   }

   /*!
    * Returns the hash sequence (message digest) corresponding to a \a data
    * block of the specified \a length in bytes, stored in the specified
    * \a hash array. The specified array must provide room for at least
    * HashLength() bytes, that is, the length in bytes of the message digest
    * produced by this hashing algorithm.
    */
   void Hash( uint8* hash, const void* data, size_type length )
   {
      Initialize();
      Update( data, length );
      Finalize( hash );
   }

   /*!
    * Returns the hash sequence (message digest) corresponding to a \a data
    * block of the specified \a length in bytes, as a ByteArray instance.
    */
   ByteArray Hash( const void* data, size_type length )
   {
      ByteArray hash( HashLength() );
      Hash( hash.Begin(), data, length );
      return hash;
   }

   /*!
    * Returns the hash sequence (message digest) corresponding to a container
    * \a data with contiguous storage as a ByteArray instance.
    *
    * The \c C template argument must be a class with PCL container semantics
    * (the Begin() and Size() member functions are required). Typically the
    * ByteArray class is used, although any array-like class with contiguous
    * storage is appropriate, including Array, String, Vector and Matrix.
    */
   template <class C>
   ByteArray Hash( const C& data )
   {
      return Hash( data.Begin(), data.Size() );
   }

protected:

   /*!
    * Terminates hash generation and returns the computed message digest
    * (hash value) stored in the specified \a hash array. The specified array
    * must provide room for at least HashLength() bytes, that is, the length in
    * bytes of the message digest produced by this hashing algorithm.
    */
   virtual void Finalize( void* hash );

   /*!
    * \internal
    */
   CryptographicHash( void* h ) : UIObject( h )
   {
   }

private:

   virtual void* CloneHandle() const;

   CryptographicHash( const CryptographicHash& ) = delete;
   void operator =( const CryptographicHash& ) = delete;
};

// ----------------------------------------------------------------------------

/*!
 * \class MD5
 * \brief Implementation of the %MD5 cryptographic hashing algorithm
 *
 * \b References
 *
 * Message Digest algorithm 5 (%MD5) was designed by Ronald Rivest in 1991.
 * %MD5 is a widely-used Internet standard published as RFC-1321 by RSA:
 *
 * http://tools.ietf.org/html/rfc1321
 *
 * An %MD5 message digest (or %MD5 hash value) is 128 bits (16 bytes) long.
 *
 * \ingroup cryptography_classes
 * \sa CryptographicHash, SHA1, SHA256
 */
class PCL_CLASS MD5 : public CryptographicHash
{
public:

   /*!
    * Constructs an %MD5 hash generator.
    */
   MD5();

   /*!
    * Destroys an %MD5 hash generator.
    */
   virtual ~MD5()
   {
   }

   /*!
    * Returns the name of this cryptographic hashing algorithm: "MD5".
    */
   virtual String AlgorithmName() const
   {
      return "MD5";
   }

   /*!
    * Returns the length in bytes of a hash sequence (or message digest)
    * calculated with this cryptographic hashing algorithm.
    *
    * As implemented in %MD5, this function returns 16, the length in bytes of
    * an %MD5 digest.
    */
   virtual size_type HashLength() const
   {
      return 16;
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class SHA1
 * \brief Implementation of the SHA-1 cryptographic hashing algorithm
 *
 * \b References
 *
 * The initial Secure Hash Algorithm (SHA) was published by NIST in 1993 as the
 * Secure Hash Standard, FIPS PUB 180. The SHA-1 algorithm was defined in a
 * revised version published in 1995, FIPS PUB 180-1:
 *
 * http://www.itl.nist.gov/fipspubs/fip180-1.htm
 *
 * An SHA-1 message digest (or SHA-1 hash value) is 160 bits (20 bytes) long.
 *
 * \ingroup cryptography_classes
 * \sa CryptographicHash, MD5, SHA256
 */
class PCL_CLASS SHA1 : public CryptographicHash
{
public:

   /*!
    * Constructs an SHA-1 hash generator.
    */
   SHA1();

   /*!
    * Destroys an SHA-1 hash generator.
    */
   virtual ~SHA1()
   {
   }

   /*!
    * Returns the name of this cryptographic hashing algorithm: "SHA1".
    */
   virtual String AlgorithmName() const
   {
      return "SHA1";
   }

   /*!
    * Returns the length in bytes of a hash sequence (or message digest)
    * calculated with this cryptographic hashing algorithm.
    *
    * As implemented in %SHA1, this function returns 20, the length in bytes of
    * an SHA-1 digest.
    */
   virtual size_type HashLength() const
   {
      return 20;
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class Cipher
 * \brief Abstract base class for cipher algorithm implementations
 *
 * %Cipher defines a common interface for implementations of algorithms
 * performing encryption and decryption of generic data.
 *
 * Currently we have implemented the AES-256 cipher with CBC encryption and
 * decryption on the PixInsight platform, available as the AES256 PCL class.
 *
 * \ingroup cryptography_classes
 * \sa AES256
 */
class PCL_CLASS Cipher : public UIObject
{
public:

   /*!
    * Destroys a %Cipher object.
    */
   virtual ~Cipher()
   {
   }

   /*!
    * Ensures that the server-side object managed by this instance is uniquely
    * referenced.
    *
    * Since cipher algorithms are unique objects by definition, calling this
    * member function has no effect.
    */
   virtual void EnsureUnique()
   {
   }

   /*!
    * Returns the name of this cipher algorithm.
    */
   virtual String AlgorithmName() const = 0;

   /*!
    * Returns the length in bytes of a key in this cipher algorithm.
    */
   virtual size_type KeyLength() const = 0;

   /*!
    * Buffer encryption.
    *
    * \param[out] output  Starting address of the output buffer, where the
    *                encrypted data (or <em>cipher text</em>) will be stored.
    *
    * \param input   Starting address of the input buffer, where the original
    *                data (or <em>plain text</em>) is stored.
    *
    * \param length  The buffer length in bytes.
    */
   virtual void Encrypt( void* output, const void* input, size_type length ) const = 0;

   /*!
    * Buffer decryption.
    *
    * \param[out] output  Starting address of the output buffer, where the
    *                decrypted data (or <em>plain text</em>) will be stored.
    *
    * \param input   Starting address of the input buffer, where the encrypted
    *                data (or <em>cipher text</em>) is stored.
    *
    * \param length  The buffer length in bytes.
    */
   virtual void Decrypt( void* output, const void* input, size_type length ) const = 0;

   /*!
    * Container encryption.
    *
    * \param[out] output  Starting address of the output buffer, where the
    *                encrypted data (or <em>cipher text</em>) will be stored.
    *
    * \param input   Reference to a container where the original data (or
    *                <em>plain text</em>) is stored as a contiguous block. The
    *                input container will be treated as a contiguous sequence
    *                of bytes, ignoring the data type of the items contained.
    *
    * The \c C template argument must be a class with PCL container semantics
    * (the Begin() and Size() member functions are required). Typically the
    * ByteArray class is used, although any array-like class with contiguous
    * storage is appropriate, including Array, String, Vector and Matrix.
    */
   template <class C>
   void Encrypt( void* output, const C& input ) const
   {
      Encrypt( output, input.Begin(), input.Size() );
   }

   /*!
    * Out-of-place container encryption.
    *
    * \param input   Reference to a container where the original data (or
    *                <em>plain text</em>) is stored as a contiguous block. The
    *                input container will be treated as a contiguous sequence
    *                of bytes, ignoring the data type of the items contained.
    *
    * The \c C template argument must be a class with PCL container semantics
    * (the Begin() and Size() member functions are required). Typically the
    * ByteArray class is used, although any array-like class with contiguous
    * storage is appropriate, including Array, String, Vector and Matrix.
    *
    * Returns a ByteArray object with the encrypted data, or <em>cipher
    * text</em>. The returned object can be used to decrypt the data with the
    * Decrypt() member function.
    */
   template <class C>
   ByteArray Encrypt( const C& input ) const
   {
      ByteArray output( input.Size() );
      Encrypt( output.Begin(), input.Begin(), output.Length() );
      return output;
   }

   /*!
    * Container decryption.
    *
    * \param[out] output  Starting address of the output buffer, where the
    *                decrypted data (or <em>plain text</em>) will be stored.
    *
    * \param input   Reference to a container where the encrypted data (or
    *                <em>cipher text</em>) is stored as a contiguous block. The
    *                input container will be treated as a contiguous sequence
    *                of bytes, ignoring the data type of the items contained.
    *
    * The \c C template argument must be a class with PCL container semantics
    * (the Begin() and Size() member functions are required). Typically the
    * ByteArray class is used, although any array-like class with contiguous
    * storage is appropriate, including Array, String, Vector and Matrix.
    */
   template <class C>
   void Decrypt( void* output, const C& input ) const
   {
      Decrypt( output, input.Begin(), input.Size() );
   }

   /*!
    * Out-of-place container decryption.
    *
    * \param input   Reference to a container where the encrypted data (or
    *                <em>cipher text</em>) is stored as a contiguous block. The
    *                input container will be treated as a contiguous sequence
    *                of bytes, ignoring the data type of the items contained.
    *
    * The \c C template argument must be a class with PCL container semantics
    * (the Begin() and Size() member functions are required). Typically the
    * ByteArray class is used, although any array-like class with contiguous
    * storage is appropriate, including Array, String, Vector and Matrix.
    *
    * Returns a ByteArray object with the decrypted data, or <em>plain
    * text</em>), as a contiguous sequence of bytes.
    */
   template <class C>
   ByteArray Decrypt( const C& input ) const
   {
      ByteArray output( input.Size() );
      Decrypt( output.Begin(), input.Begin(), output.Length() );
      return output;
   }

protected:

   /*!
    * \internal
    */
   Cipher( void* h ) : UIObject( h )
   {
   }

private:

   virtual void* CloneHandle() const;

   Cipher( const Cipher& ) = delete;
   void operator =( const Cipher& ) = delete;
};

// ----------------------------------------------------------------------------

/*!
 * \class AES256
 * \brief AES-256 cipher
 *
 * The %AES256 class is a FIPS-197 compliant implementation of the Advanced
 * Encryption Standard (AES) cipher using 256-bit keys.
 *
 * \b References
 *
 * The AES (Advanced Encryption Standard) block cipher algorithm (originally
 * called \e Rijndael) was designed by Vincent Rijmen and Joan Daemen:
 *
 * http://csrc.nist.gov/encryption/aes/rijndael/Rijndael.pdf
 * http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
 */
class PCL_CLASS AES256 : public Cipher
{
public:

   enum { key_length = 32, block_length = 16 };

   /*!
    * Represents a 256-bit cipher key for the AES-256 algorithm.
    */
   typedef uint8  key_type[ key_length ];

   /*!
    * Represents a 16-byte ECB encryption/decription block for the AES-256
    * algorithm.
    */
   typedef uint8  block_type[ block_length ];

   /*!
    * Constructs an AES-256 cipher object.
    *
    * \param key     Starting address of a contiguous block of 32 bytes, which
    *                will be treated as the 256-bit cipher key. The first 32
    *                bytes stored at this address will be wiped out (replaced
    *                with zeros) by this constructor.
    *
    * \note For enhanced security, this constructor wipes out the specified
    * cipher \a key, filling it with 32 zero bytes. If you want to preserve the
    * key (which in general is a <em>very bad idea</em> for security reasons),
    * make a copy before invoking this constructor.
    */
   AES256( void* key );

   /*!
    * Constructs an AES-256 cipher object.
    *
    * \param key     256-bit cipher key specified as an 8-bit string. The
    *                contents of this string Will be wiped out (filled with
    *                null characters) by this constructor.
    *
    * The AES cipher key will be initialized with the lowercase hexadecimal
    * representation of the MD5 checksum of the specified \a key string (note
    * that an %MD5 checksum is a sequence of 16 bytes, whose hex representation
    * has 32 characters, or 256 bits).
    *
    * \note For enhanced security, this constructor wipes out the specified
    * cipher \a key string, filling it with null characters. If you want to
    * preserve the original key (which in general is a <em>very bad idea</em>
    * for security reasons), make a copy before invoking this constructor.
    */
   AES256( IsoString& key );

   /*!
    * Destroys an AES-256 cipher object.
    */
   virtual ~AES256()
   {
   }

   /*!
    * Returns the name of this cipher algorithm: "AES256".
    */
   virtual String AlgorithmName() const
   {
      return "AES256";
   }

   /*!
    * Returns the length in bytes of a key in the AES-256 cipher algorithm: 32.
    */
   virtual size_type KeyLength() const
   {
      return sizeof( key_type );
   }

   /*!
    * AES-256 buffer encryption (CBC mode).
    *
    * \param[out] output  Starting address of the output buffer, where the
    *                encrypted data (or <em>cipher text</em>) will be stored.
    *
    * \param input   Starting address of the input buffer, where the original
    *                data (or <em>plain text</em>) is stored.
    *
    * \param length  The buffer length in bytes. Must be an integer multiple of
    *                16 bytes.
    *
    * If the specified \a length is not an integer multiple of 16 bytes, this
    * function throws an Error exception. If necessary, the source data must
    * be padded with zeros or spaces, as required to achieve a valid length.
    */
   virtual void Encrypt( void* output, const void* input, size_type length ) const;

   /*!
    * AES buffer decryption (CBC mode).
    *
    * \param[out] output  Starting address of the output buffer, where the
    *                decrypted data (or <em>plain text</em>) will be stored.
    *
    * \param input   Starting address of the input buffer, where the encrypted
    *                data (or <em>cipher text</em>) is stored.
    *
    * \param length  The buffer length in bytes. Must be an integer multiple of
    *                16 bytes.
    *
    * If the specified \a length is not an integer multiple of 16 bytes, this
    * function throws an Error exception.
    */
   virtual void Decrypt( void* output, const void* input, size_type length ) const;

   /*!
    * AES block encryption (ECB mode).
    *
    * \param[out] output   Starting address of the encrypted 16-byte block.
    *
    * \param input         Starting address of the input 16-byte block.
    */
   void EncryptBlock( block_type output, const block_type input ) const;

   /*!
    * AES block decryption (ECB mode).
    *
    * \param[out] output   Starting address of the decrypted 16-byte block.
    *
    * \param input         Starting address of the input 16-byte block.
    */
   void DecryptBlock( block_type output, const block_type input ) const;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Cryptography_h

// ----------------------------------------------------------------------------
// EOF pcl/Cryptography.h - Released 2015/10/08 11:24:12 UTC
