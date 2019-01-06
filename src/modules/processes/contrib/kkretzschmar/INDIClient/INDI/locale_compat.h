//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.01.00.0232
// ----------------------------------------------------------------------------
// locale_compat.h - Released 2018-12-12T09:25:25Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2018 Klaus Kretzschmar
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

/*
    INDI LIB
    Utility routines for saving and restoring the current locale, handling platform differences
    Copyright (C) 2017 Andy Galasso

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/

#pragma once

#include <locale.h>

#ifdef __cplusplus
extern "C" {
#endif

// C interface
//
// usage:
//
//     locale_char_t *save = indi_locale_C_numeric_push();
//     ...
//     indi_locale_C_numeric_pop(save);
//

#if defined(_MSC_VER)

#include <string.h>
#include <malloc.h>

typedef wchar_t locale_char_t;
#define INDI_LOCALE(s) L#s

__inline static locale_char_t *indi_setlocale(int category, const locale_char_t *locale)
{
    return _wcsdup(_wsetlocale(category, locale));
}

__inline static void indi_restore_locale(int category, locale_char_t *prev)
{
    _wsetlocale(category, prev);
    free(prev);
}

# define _INDI_C_INLINE __inline

#else // _MSC_VER

typedef char locale_char_t;
#define INDI_LOCALE(s) s

inline static locale_char_t *indi_setlocale(int category, const locale_char_t *locale)
{
    return setlocale(category, locale);
}

inline static void indi_restore_locale(int category, locale_char_t *prev)
{
    setlocale(category, prev);
}

# define _INDI_C_INLINE inline

#endif // _MSC_VER

_INDI_C_INLINE static locale_char_t *indi_locale_C_numeric_push()
{
    return indi_setlocale(LC_NUMERIC, INDI_LOCALE("C"));
}

_INDI_C_INLINE static void indi_locale_C_numeric_pop(locale_char_t *prev)
{
    indi_restore_locale(LC_NUMERIC, prev);
}

#undef _INDI_C_INLINE

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

// C++ interface
//
// usage:
//
//     AutoCNumeric locale; // LC_NUMERIC locale set to "C" for object scope
//     ...
//

class AutoLocale
{
    int m_category;
    locale_char_t *m_orig;

public:

    AutoLocale(int category, const locale_char_t *locale)
        : m_category(category)
    {
        m_orig = indi_setlocale(category, locale);
    }

    // method Restore can be used to restore the original locale
    // before the object goes out of scope
    void Restore()
    {
        if (m_orig)
        {
            indi_restore_locale(m_category, m_orig);
            m_orig = nullptr;
        }
    }

    ~AutoLocale()
    {
        Restore();
    }
};

class AutoCNumeric : public AutoLocale
{
public:
    AutoCNumeric() : AutoLocale(LC_NUMERIC, INDI_LOCALE("C")) { }
};

#endif // __cplusplus

// ----------------------------------------------------------------------------
// EOF locale_compat.h - Released 2018-12-12T09:25:25Z
