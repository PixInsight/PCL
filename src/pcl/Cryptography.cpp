//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// pcl/Cryptography.cpp - Released 2015/10/08 11:24:19 UTC
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

#include <pcl/Cryptography.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

void CryptographicHash::Initialize()
{
   if ( (*API->Cryptography->InitializeCryptographicHash)( handle ) == api_false )
      throw APIFunctionError( "InitializeCryptographicHash" );
}

void CryptographicHash::Update( const void* data, size_type length )
{
   if ( (*API->Cryptography->UpdateCryptographicHash)( handle, data, length ) == api_false )
      throw APIFunctionError( "UpdateCryptographicHash" );
}

void CryptographicHash::Finalize( void* hash )
{
   if ( (*API->Cryptography->FinalizeCryptographicHash)( handle, hash ) == api_false )
      throw APIFunctionError( "FinalizeCryptographicHash" );
}

void* CryptographicHash::CloneHandle() const
{
   throw Error( "Cannot clone a CryptographicHash handle" );
}

// ----------------------------------------------------------------------------

MD5::MD5() :
CryptographicHash( (*API->Cryptography->CreateCryptographicHash)( ModuleHandle(), CryptographyContext::MD5 ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateCryptographicHash" );
}

// ----------------------------------------------------------------------------

SHA1::SHA1() :
CryptographicHash( (*API->Cryptography->CreateCryptographicHash)( ModuleHandle(), CryptographyContext::SHA1 ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateCryptographicHash" );
}

// ----------------------------------------------------------------------------

void* Cipher::CloneHandle() const
{
   throw Error( "Cannot clone a Cipher handle" );
}

// ----------------------------------------------------------------------------

AES256::AES256( void* key ) :
Cipher( (*API->Cryptography->CreateAES256Cipher)( ModuleHandle(), key ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateAES256Cipher" );
}

AES256::AES256( IsoString& key ) : Cipher( (void*)0 )
{
   ByteArray md5 = MD5().Hash( key );
   key.SecureFill();
   IsoString hash = IsoString::ToHex( md5 );
   if ( hash.Length() != 32 )
      throw Error( "AES256: Internal error" );
   void* h = (*API->Cryptography->CreateAES256Cipher)( ModuleHandle(), hash.c_str() );
   if ( h == 0 )
      throw APIFunctionError( "CreateAES256Cipher" );
   SetHandle( h );
}

void AES256::Encrypt( void* a_output, const void* a_input, size_type length ) const
{
   if ( length % 16 )
      throw Error( "AES256::Encrypt(): Invalid buffer length - must be a multiple of 16 bytes" );

   uint8* output = reinterpret_cast<uint8*>( a_output );
   const uint8* input = reinterpret_cast<const uint8*>( a_input );

   uint8 iv[ 16 ];
   memset( iv, 0, 16 );

   while( length > 0 )
   {
      for ( int i = 0; i < 16; ++i )
         output[i] = input[i] ^ iv[i];

      EncryptBlock( output, output );

      memcpy( iv, output, 16 );

      input  += 16;
      output += 16;
      length -= 16;
   }
}

void AES256::Decrypt( void* a_output, const void* a_input, size_type length ) const
{
   if ( length % 16 )
      throw Error( "AES256::Decrypt(): Invalid buffer length - must be a multiple of 16 bytes" );

   uint8* output = reinterpret_cast<uint8*>( a_output );
   const uint8* input = reinterpret_cast<const uint8*>( a_input );

   uint8 iv[ 16 ];
   memset( iv, 0, 16 );

   while( length > 0 )
   {
      uint8 temp[ 16 ];
      memcpy( temp, input, 16 );

      DecryptBlock( output, input );

      for( int i = 0; i < 16; ++i )
         output[i] = output[i] ^ iv[i];

      memcpy( iv, temp, 16 );

      input  += 16;
      output += 16;
      length -= 16;
   }
}

void AES256::EncryptBlock( block_type output, const block_type input ) const
{
   if ( (*API->Cryptography->AES256EncryptBlock)( handle, output, input ) == api_false )
      throw APIFunctionError( "AES256EncryptBlock" );
}

void AES256::DecryptBlock( block_type output, const block_type input ) const
{
   if ( (*API->Cryptography->AES256DecryptBlock)( handle, output, input ) == api_false )
      throw APIFunctionError( "AES256DecryptBlock" );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Cryptography.cpp - Released 2015/10/08 11:24:19 UTC
