//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.15.0225
// ----------------------------------------------------------------------------
// indibasetypes.h - Released 2018-11-01T11:07:21Z
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

/*******************************************************************************
  Copyright(c) 2011 Jasem Mutlaq. All rights reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Library General Public
 License version 2 as published by the Free Software Foundation.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Library General Public License for more details.

 You should have received a copy of the GNU Library General Public License
 along with this library; see the file COPYING.LIB.  If not, write to
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 Boston, MA 02110-1301, USA.
*******************************************************************************/

#pragma once

/*! INDI property type */
typedef enum {
    INDI_NUMBER, /*!< INumberVectorProperty. */
    INDI_SWITCH, /*!< ISwitchVectorProperty. */
    INDI_TEXT,   /*!< ITextVectorProperty. */
    INDI_LIGHT,  /*!< ILightVectorProperty. */
    INDI_BLOB,   /*!< IBLOBVectorProperty. */
    INDI_UNKNOWN
} INDI_PROPERTY_TYPE;

/*! INDI Equatorial Axis type */
typedef enum {
    AXIS_RA, /*!< Right Ascension Axis. */
    AXIS_DE  /*!< Declination Axis. */
} INDI_EQ_AXIS;

/*! INDI Horizontal Axis type */
typedef enum {
    AXIS_AZ, /*!< Azimuth Axis. */
    AXIS_ALT /*!< Altitude Axis. */
} INDI_HO_AXIS;

/*! North/South Direction type */
typedef enum {
    DIRECTION_NORTH = 0, /*!< North direction */
    DIRECTION_SOUTH      /*!< South direction */
} INDI_DIR_NS;

/*! West/East Direction type */
typedef enum {
    DIRECTION_WEST = 0, /*!< West direction */
    DIRECTION_EAST      /*!< East direction */
} INDI_DIR_WE;

/*! INDI Error Types */
typedef enum {
    INDI_DEVICE_NOT_FOUND    = -1, /*!< Device not found error */
    INDI_PROPERTY_INVALID    = -2, /*!< Property invalid error */
    INDI_PROPERTY_DUPLICATED = -3, /*!< Property duplicated error */
    INDI_DISPATCH_ERROR      = -4  /*!< Dispatch error */
} INDI_ERROR_TYPE;

// ----------------------------------------------------------------------------
// EOF indibasetypes.h - Released 2018-11-01T11:07:21Z
