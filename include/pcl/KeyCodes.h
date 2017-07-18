//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// pcl/KeyCodes.h - Released 2017-07-18T16:13:52Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2017 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_KeyCodes_h
#define __PCL_KeyCodes_h

/// \file pcl/KeyCodes.h

#include <pcl/Defs.h>

#ifndef __PCL_NO_KEY_QUERY_UTILITIES
# ifdef __PCL_WINDOWS
#  include <windows.h> // for virtual key codes
# endif
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::KeyCode
 * \brief Defines PCL key codes.
 *
 * The %KeyCode namespace defines platform-independent keyboard key identifiers
 * on the PixInsight/PCL framework.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>KeyCode::Backspace</td>         <td>0x00000008</td></tr>
 * <tr><td>KeyCode::Tab</td>               <td>0x00000009</td></tr>
 * <tr><td>KeyCode::Clear</td>             <td>0x0000000c</td></tr>
 * <tr><td>KeyCode::Return</td>            <td>0x0000000d</td></tr>
 * <tr><td>KeyCode::Enter</td>             <td>0x0000000d</td></tr>
 * <tr><td>KeyCode::Escape</td>            <td>0x0000001b</td></tr>
 * <tr><td>KeyCode::Shift</td>             <td>0x70000001</td></tr>
 * <tr><td>KeyCode::Control</td>           <td>0x70000002</td></tr>
 * <tr><td>KeyCode::Alt</td>               <td>0x70000003</td></tr>
 * <tr><td>KeyCode::Meta</td>              <td>0x70000004</td></tr>
 * <tr><td>KeyCode::CapsLock</td>          <td>0x70000010</td></tr>
 * <tr><td>KeyCode::NumLock</td>           <td>0x70000020</td></tr>
 * <tr><td>KeyCode::ScrollLock</td>        <td>0x70000030</td></tr>
 * <tr><td>KeyCode::Pause</td>             <td>0x70000100</td></tr>
 * <tr><td>KeyCode::Print</td>             <td>0x70000200</td></tr>
 * <tr><td>KeyCode::Help</td>              <td>0x70000300</td></tr>
 * <tr><td>KeyCode::SysReq</td>            <td>0x70000400</td></tr>
 * <tr><td>KeyCode::Left</td>              <td>0x10000001</td></tr>
 * <tr><td>KeyCode::Up</td>                <td>0x10000002</td></tr>
 * <tr><td>KeyCode::Right</td>             <td>0x10000003</td></tr>
 * <tr><td>KeyCode::Down</td>              <td>0x10000004</td></tr>
 * <tr><td>KeyCode::Insert</td>            <td>0x10000010</td></tr>
 * <tr><td>KeyCode::Delete</td>            <td>0x10000020</td></tr>
 * <tr><td>KeyCode::Home</td>              <td>0x10000100</td></tr>
 * <tr><td>KeyCode::End</td>               <td>0x10000200</td></tr>
 * <tr><td>KeyCode::PageUp</td>            <td>0x10000300</td></tr>
 * <tr><td>KeyCode::PageDown</td>          <td>0x10000400</td></tr>
 * <tr><td>KeyCode::F1</td>                <td>0x08000001</td></tr>
 * <tr><td>KeyCode::F2</td>                <td>0x08000002</td></tr>
 * <tr><td>KeyCode::F3</td>                <td>0x08000003</td></tr>
 * <tr><td>KeyCode::F4</td>                <td>0x08000004</td></tr>
 * <tr><td>KeyCode::F5</td>                <td>0x08000005</td></tr>
 * <tr><td>KeyCode::F6</td>                <td>0x08000006</td></tr>
 * <tr><td>KeyCode::F7</td>                <td>0x08000007</td></tr>
 * <tr><td>KeyCode::F8</td>                <td>0x08000008</td></tr>
 * <tr><td>KeyCode::F9</td>                <td>0x08000009</td></tr>
 * <tr><td>KeyCode::F10</td>               <td>0x0800000a</td></tr>
 * <tr><td>KeyCode::F11</td>               <td>0x0800000b</td></tr>
 * <tr><td>KeyCode::F12</td>               <td>0x0800000c</td></tr>
 * <tr><td>KeyCode::F13</td>               <td>0x0800000d</td></tr>
 * <tr><td>KeyCode::F14</td>               <td>0x0800000e</td></tr>
 * <tr><td>KeyCode::F15</td>               <td>0x0800000f</td></tr>
 * <tr><td>KeyCode::F16</td>               <td>0x08000010</td></tr>
 * <tr><td>KeyCode::F17</td>               <td>0x08000020</td></tr>
 * <tr><td>KeyCode::F18</td>               <td>0x08000030</td></tr>
 * <tr><td>KeyCode::F19</td>               <td>0x08000040</td></tr>
 * <tr><td>KeyCode::F20</td>               <td>0x08000050</td></tr>
 * <tr><td>KeyCode::F21</td>               <td>0x08000060</td></tr>
 * <tr><td>KeyCode::F22</td>               <td>0x08000070</td></tr>
 * <tr><td>KeyCode::F23</td>               <td>0x08000080</td></tr>
 * <tr><td>KeyCode::F24</td>               <td>0x08000090</td></tr>
 * <tr><td>KeyCode::Space</td>             <td>0x00000020</td></tr>
 * <tr><td>KeyCode::Exclamation</td>       <td>0x00000021</td></tr>
 * <tr><td>KeyCode::DoubleQuote</td>       <td>0x00000022</td></tr>
 * <tr><td>KeyCode::NumberSign</td>        <td>0x00000023</td></tr>
 * <tr><td>KeyCode::Dollar</td>            <td>0x00000024</td></tr>
 * <tr><td>KeyCode::Percent</td>           <td>0x00000025</td></tr>
 * <tr><td>KeyCode::Ampersand</td>         <td>0x00000026</td></tr>
 * <tr><td>KeyCode::Apostrophe</td>        <td>0x00000027</td></tr>
 * <tr><td>KeyCode::LeftParenthesis</td>   <td>0x00000028</td></tr>
 * <tr><td>KeyCode::RightParenthesis</td>  <td>0x00000029</td></tr>
 * <tr><td>KeyCode::Asterisk</td>          <td>0x0000002a</td></tr>
 * <tr><td>KeyCode::Plus</td>              <td>0x0000002b</td></tr>
 * <tr><td>KeyCode::Comma</td>             <td>0x0000002c</td></tr>
 * <tr><td>KeyCode::Minus</td>             <td>0x0000002d</td></tr>
 * <tr><td>KeyCode::Period</td>            <td>0x0000002e</td></tr>
 * <tr><td>KeyCode::Slash</td>             <td>0x0000002f</td></tr>
 * <tr><td>KeyCode::Zero</td>              <td>0x00000030</td></tr>
 * <tr><td>KeyCode::One</td>               <td>0x00000031</td></tr>
 * <tr><td>KeyCode::Two</td>               <td>0x00000032</td></tr>
 * <tr><td>KeyCode::Three</td>             <td>0x00000033</td></tr>
 * <tr><td>KeyCode::Four</td>              <td>0x00000034</td></tr>
 * <tr><td>KeyCode::Five</td>              <td>0x00000035</td></tr>
 * <tr><td>KeyCode::Six</td>               <td>0x00000036</td></tr>
 * <tr><td>KeyCode::Seven</td>             <td>0x00000037</td></tr>
 * <tr><td>KeyCode::Eight</td>             <td>0x00000038</td></tr>
 * <tr><td>KeyCode::Nine</td>              <td>0x00000039</td></tr>
 * <tr><td>KeyCode::Colon</td>             <td>0x0000003a</td></tr>
 * <tr><td>KeyCode::Semicolon</td>         <td>0x0000003b</td></tr>
 * <tr><td>KeyCode::Less</td>              <td>0x0000003c</td></tr>
 * <tr><td>KeyCode::Equal</td>             <td>0x0000003d</td></tr>
 * <tr><td>KeyCode::Greater</td>           <td>0x0000003e</td></tr>
 * <tr><td>KeyCode::Question</td>          <td>0x0000003f</td></tr>
 * <tr><td>KeyCode::At</td>                <td>0x00000040</td></tr>
 * <tr><td>KeyCode::A</td>                 <td>0x00000041</td></tr>
 * <tr><td>KeyCode::B</td>                 <td>0x00000042</td></tr>
 * <tr><td>KeyCode::C</td>                 <td>0x00000043</td></tr>
 * <tr><td>KeyCode::D</td>                 <td>0x00000044</td></tr>
 * <tr><td>KeyCode::E</td>                 <td>0x00000045</td></tr>
 * <tr><td>KeyCode::F</td>                 <td>0x00000046</td></tr>
 * <tr><td>KeyCode::G</td>                 <td>0x00000047</td></tr>
 * <tr><td>KeyCode::H</td>                 <td>0x00000048</td></tr>
 * <tr><td>KeyCode::I</td>                 <td>0x00000049</td></tr>
 * <tr><td>KeyCode::J</td>                 <td>0x0000004a</td></tr>
 * <tr><td>KeyCode::K</td>                 <td>0x0000004b</td></tr>
 * <tr><td>KeyCode::L</td>                 <td>0x0000004c</td></tr>
 * <tr><td>KeyCode::M</td>                 <td>0x0000004d</td></tr>
 * <tr><td>KeyCode::N</td>                 <td>0x0000004e</td></tr>
 * <tr><td>KeyCode::O</td>                 <td>0x0000004f</td></tr>
 * <tr><td>KeyCode::P</td>                 <td>0x00000050</td></tr>
 * <tr><td>KeyCode::Q</td>                 <td>0x00000051</td></tr>
 * <tr><td>KeyCode::R</td>                 <td>0x00000052</td></tr>
 * <tr><td>KeyCode::S</td>                 <td>0x00000053</td></tr>
 * <tr><td>KeyCode::T</td>                 <td>0x00000054</td></tr>
 * <tr><td>KeyCode::U</td>                 <td>0x00000055</td></tr>
 * <tr><td>KeyCode::V</td>                 <td>0x00000056</td></tr>
 * <tr><td>KeyCode::W</td>                 <td>0x00000057</td></tr>
 * <tr><td>KeyCode::X</td>                 <td>0x00000058</td></tr>
 * <tr><td>KeyCode::Y</td>                 <td>0x00000059</td></tr>
 * <tr><td>KeyCode::Z</td>                 <td>0x0000005a</td></tr>
 * <tr><td>KeyCode::LeftBracket</td>       <td>0x0000005b</td></tr>
 * <tr><td>KeyCode::Backslash</td>         <td>0x0000005c</td></tr>
 * <tr><td>KeyCode::RightBracket</td>      <td>0x0000005d</td></tr>
 * <tr><td>KeyCode::Circumflex</td>        <td>0x0000005e</td></tr>
 * <tr><td>KeyCode::Underscore</td>        <td>0x0000005f</td></tr>
 * <tr><td>KeyCode::LeftQuote</td>         <td>0x00000060</td></tr>
 * <tr><td>KeyCode::LeftBrace</td>         <td>0x0000007b</td></tr>
 * <tr><td>KeyCode::Bar</td>               <td>0x0000007c</td></tr>
 * <tr><td>KeyCode::RightBrace</td>        <td>0x0000007d</td></tr>
 * <tr><td>KeyCode::Tilde</td>             <td>0x0000007e</td></tr>
 * <tr><td>KeyCode::Unknown</td>           <td>0x7fffffff</td></tr>
 * </table>
 */
namespace KeyCode
{
   enum value_type
   {
      Backspace         = 0x00000008,
      Tab               = 0x00000009,
      Clear             = 0x0000000c,
      Return            = 0x0000000d,
      Enter             = 0x0000000d,
      Escape            = 0x0000001b,
      Shift             = 0x70000001,
      Control           = 0x70000002,
      Alt               = 0x70000003,
      Meta              = 0x70000004,
      CapsLock          = 0x70000010,
      NumLock           = 0x70000020,
      ScrollLock        = 0x70000030,
      Pause             = 0x70000100,
      Print             = 0x70000200,
      Help              = 0x70000300,
      SysReq            = 0x70000400,
      Left              = 0x10000001,
      Up                = 0x10000002,
      Right             = 0x10000003,
      Down              = 0x10000004,
      Insert            = 0x10000010,
      Delete            = 0x10000020,
      Home              = 0x10000100,
      End               = 0x10000200,
      PageUp            = 0x10000300,
      PageDown          = 0x10000400,
      F1                = 0x08000001,
      F2                = 0x08000002,
      F3                = 0x08000003,
      F4                = 0x08000004,
      F5                = 0x08000005,
      F6                = 0x08000006,
      F7                = 0x08000007,
      F8                = 0x08000008,
      F9                = 0x08000009,
      F10               = 0x0800000a,
      F11               = 0x0800000b,
      F12               = 0x0800000c,
      F13               = 0x0800000d,
      F14               = 0x0800000e,
      F15               = 0x0800000f,
      F16               = 0x08000010,
      F17               = 0x08000020,
      F18               = 0x08000030,
      F19               = 0x08000040,
      F20               = 0x08000050,
      F21               = 0x08000060,
      F22               = 0x08000070,
      F23               = 0x08000080,
      F24               = 0x08000090,
      Space             = 0x00000020,
      Exclamation       = 0x00000021,
      DoubleQuote       = 0x00000022,
      NumberSign        = 0x00000023,
      Dollar            = 0x00000024,
      Percent           = 0x00000025,
      Ampersand         = 0x00000026,
      Apostrophe        = 0x00000027,
      LeftParenthesis   = 0x00000028,
      RightParenthesis  = 0x00000029,
      Asterisk          = 0x0000002a,
      Plus              = 0x0000002b,
      Comma             = 0x0000002c,
      Minus             = 0x0000002d,
      Period            = 0x0000002e,
      Slash             = 0x0000002f,
      Zero              = 0x00000030,
      One               = 0x00000031,
      Two               = 0x00000032,
      Three             = 0x00000033,
      Four              = 0x00000034,
      Five              = 0x00000035,
      Six               = 0x00000036,
      Seven             = 0x00000037,
      Eight             = 0x00000038,
      Nine              = 0x00000039,
      Colon             = 0x0000003a,
      Semicolon         = 0x0000003b,
      Less              = 0x0000003c,
      Equal             = 0x0000003d,
      Greater           = 0x0000003e,
      Question          = 0x0000003f,
      At                = 0x00000040,
      A                 = 0x00000041,
      B                 = 0x00000042,
      C                 = 0x00000043,
      D                 = 0x00000044,
      E                 = 0x00000045,
      F                 = 0x00000046,
      G                 = 0x00000047,
      H                 = 0x00000048,
      I                 = 0x00000049,
      J                 = 0x0000004a,
      K                 = 0x0000004b,
      L                 = 0x0000004c,
      M                 = 0x0000004d,
      N                 = 0x0000004e,
      O                 = 0x0000004f,
      P                 = 0x00000050,
      Q                 = 0x00000051,
      R                 = 0x00000052,
      S                 = 0x00000053,
      T                 = 0x00000054,
      U                 = 0x00000055,
      V                 = 0x00000056,
      W                 = 0x00000057,
      X                 = 0x00000058,
      Y                 = 0x00000059,
      Z                 = 0x0000005a,
      LeftBracket       = 0x0000005b,
      Backslash         = 0x0000005c,
      RightBracket      = 0x0000005d,
      Circumflex        = 0x0000005e,
      Underscore        = 0x0000005f,
      LeftQuote         = 0x00000060,
      LeftBrace         = 0x0000007b,
      Bar               = 0x0000007c,
      RightBrace        = 0x0000007d,
      Tilde             = 0x0000007e,

      Unknown           = 0x7fffffff
   };
}

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

/*!
 * \defgroup keyboard_utilities Keyboard Utilities
 */

/*!
 * Returns true iff the specified keyboard key is currently pressed.
 *
 * The \a key1 and (optional) \a key2 parameters are platform-dependent
 * keyboard scan codes or virtual key identifiers.
 *
 * \ingroup keyboard_utilities
 */
bool IsKeyPressed( int key1, int key2 = 0 ); // implemented in pcl/Keyboard.cpp

#ifndef __PCL_NO_KEY_QUERY_UTILITIES

/*!
 * Returns true iff the Space Bar key is currently pressed.
 * \ingroup keyboard_utilities
 */
inline bool IsSpaceBarPressed()
{
   return IsKeyPressed(
#if defined( __PCL_X11 )
               65
#endif
#if defined( __PCL_MACOSX )
               0x31  // kVK_Space
#endif
#if defined( __PCL_WINDOWS )
               VK_SPACE
#endif
            );
}

/*!
 * Returns true iff a Control key (also known as Meta key on Mac OS X) is
 * currently pressed.
 * \ingroup keyboard_utilities
 */
inline bool IsControlPressed()
{
   return IsKeyPressed(
#if defined( __PCL_X11 )
               37, 109
#endif
#if defined( __PCL_MACOSX )
               0x3B  // kVK_Control
#endif
#if defined( __PCL_WINDOWS )
               VK_CONTROL
#endif
            );
}

/*!
 * Returns true iff a Shift key is currently pressed.
 * \ingroup keyboard_utilities
 */
inline bool IsShiftPressed()
{
   return IsKeyPressed(
#if defined( __PCL_X11 )
               50, 62
#endif
#if defined( __PCL_MACOSX )
               0x38  // kVK_Shift
#endif
#if defined( __PCL_WINDOWS )
               VK_SHIFT
#endif
            );
}

/*!
 * Returns true iff an Alt key (also known as Option key on Mac OS X) is
 * currently pressed.
 * \ingroup keyboard_utilities
 */
inline bool IsAltPressed()
{
   return IsKeyPressed(
#if defined( __PCL_X11 )
               64, 113
#endif
#if defined( __PCL_MACOSX )
               0x3A  // kVK_Option
#endif
#if defined( __PCL_WINDOWS )
               VK_MENU
#endif
            );
}

#if defined( __PCL_MACOSX )

/*!
 * Returns true iff the Command key (Mac keyboard only) is currently pressed.
 *
 * \note This function is only defined on Mac OS X.
 * \ingroup keyboard_utilities
 */
inline bool IsCmdPressed()
{
   return IsKeyPressed( 0x37 );  // kVK_Command
}

/*!
 * Returns true iff the Meta key is currently pressed on the Mac keyboard.
 *
 * This function is a convenient alias for IsControlPressed(), only available
 * on Mac OS X.
 *
 * \ingroup keyboard_utilities
 */
inline bool IsMetaPressed()
{
   return IsControlPressed();
}

#endif // __PCL_MACOSX

/*!
 * On UNIX, Linux and Windows platforms, returns true iff a Control key is
 * currently pressed. On macOS, returns true iff the Command key is currently
 * pressed.
 *
 * \ingroup keyboard_utilities
 */
inline bool IsControlOrCmdPressed()
{
#if defined( __PCL_MACOSX )
   return IsCmdPressed();
#else
   return IsControlPressed();
#endif
}

#endif // !__PCL_NO_KEY_QUERY_UTILITIES

#endif // !__PCL_BUILDING_PIXINSIGHT_APPLICATION

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_KeyCodes_h

// ----------------------------------------------------------------------------
// EOF pcl/KeyCodes.h - Released 2017-07-18T16:13:52Z
