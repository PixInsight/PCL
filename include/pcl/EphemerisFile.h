//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// pcl/EphemerisFile.h - Released 2018-12-12T09:24:21Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_EphemerisFile_h
#define __PCL_EphemerisFile_h

/// \file pcl/EphemerisFile.h

#include <pcl/Defs.h>

#include <pcl/Atomic.h>
#include <pcl/ChebyshevFit.h>
#include <pcl/File.h>
#include <pcl/Mutex.h>
#include <pcl/Optional.h>
#include <pcl/TimePoint.h>

#ifdef __PCL_BUILDING_PIXINSIGHT_APPLICATION
namespace pi
{
   class JSEphemerisFileObject;
   class JSEphemerisHandleObject;
}
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \struct EphemerisMetadata
 * \brief %Metadata items available in ephemeris files (XEPH format).
 *
 * This structure holds metadata items that can be stored in ephemeris files
 * (current XEPH format version 1.0). For an existing ephemeris file, available
 * metadata are extracted directly from %XML file headers. Currently all items
 * are optional, so all data members of this structure can be empty strings.
 *
 * For generation of new XEPH files, the creationTime and creatorOS members of
 * this structure will be ignored, since the corresponding metadata items will
 * always be defined automatically by the EphemerisFile::Serialize() routine.
 * The specified creatorApplication member, if empty, will be replaced in the
 * same routine with a default value identifying the current PCL version.
 *
 * \ingroup solar_system_ephemerides
 */
struct PCL_CLASS EphemerisMetadata
{
   TimePoint creationTime;       //!< The date this file was created.
   String    creatorOS;          //!< The operating system on which this file was created.
   String    creatorApplication; //!< The software application or program that created this file.
   String    title;              //!< A title that represents or identifies this XEPH file.
   String    briefDescription;   //!< A brief (single-line) description of this XEPH file.
   String    description;        //!< A full description of the data stored in this XEPH file.
   String    organizationName;   //!< The name of the organization responsible for this file.
   String    authors;            //!< The names of one or more persons or groups that have created the data in this file.
   String    copyright;          //!< Copyright information applicable to the data stored in this XEPH file.
};

// ----------------------------------------------------------------------------

/*!
 * \struct EphemerisConstant
 * \brief A numerical constant defined in an ephemeris file (XEPH format).
 *
 * This structure implements a simple name/value pair used to store and
 * manipulate numerical constants defined in ephemeris files (current XEPH
 * format version 1.0). These constants are primarily intended to support JPL
 * DE/LE fundamental ephemerides, where we need access to the set of numerical
 * integration constants defined by JPL.
 *
 * Comparisons between %EphemerisConstant instances are performed on the their
 * names and are case-insensitive.
 *
 * \ingroup solar_system_ephemerides
 */
struct PCL_CLASS EphemerisConstant
{
   IsoString name;   //!< The constant name (case-insensitive).
   double    value;  //!< The constant value.

   /*!
    * Memberwise constructor.
    */
   EphemerisConstant( const IsoString& n = IsoString(), double v = 0 ) :
      name( n ), value( v )
   {
   }

   /*!
    * Copy constructor.
    */
   EphemerisConstant( const EphemerisConstant& ) = default;

   /*!
    * Move constructor.
    */
   EphemerisConstant( EphemerisConstant&& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   EphemerisConstant& operator =( const EphemerisConstant& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   EphemerisConstant& operator =( EphemerisConstant&& ) = default;

   /*!
    * Equality operator. Returns the result of the case-insensitive equality
    * operator applied to the \a name members.
    */
   bool operator ==( const EphemerisConstant& x ) const
   {
      return name.CompareIC( x.name ) == 0;
   }

   /*!
    * Less-than relational operator. Returns the result of the case-insensitive
    * less-than operator applied to the \a name members of this object and the
    * specified instance \a x.
    */
   bool operator <( const EphemerisConstant& x ) const
   {
      return name.CompareIC( x.name ) < 0;
   }
};

/*!
 * \class pcl::EphemerisConstantList
 * \brief Dynamic list of ephemeris numerical constants.
 *
 * %EphemerisConstantList is used as the internal implementation of numerical
 * constants associated with an %EphemerisFile instance, that is, with an
 * ephemeris file in the XEPH format. %EphemerisConstantList is a template
 * instantiation of Array\<\> for the EphemerisConstant class.
 *
 * \ingroup solar_system_ephemerides
 */
typedef Array<EphemerisConstant> EphemerisConstantList;

// ----------------------------------------------------------------------------

/*!
 * \struct SerializableEphemerisData
 * \brief Chebyshev polynomial expansion coefficients for ephemeris serialization.
 *
 * This structure holds (possibly truncated) Chebyshev polynomial expansions
 * suitable for generation of an ephemeris file in XEPH format with the
 * EphemerisFile::Serialize() static member function.
 *
 * In an ephemeris file, the complete time span covered by the file is usually
 * subdivided into many small chunks or subspans, each of them with a
 * relatively short polynomial expansion. The duration of each subspan is
 * defined in a way such that the movement of the object for which positions
 * are being calculated within the time subspan is sufficiently smooth to be
 * fitted by truncated Chebyshev polynomials with relatively few coefficients
 * (typically in the range of 15 to 30 coefficients) to achieve a prescribed
 * accuracy.
 *
 * \sa EphemerisFile::Serialize()
 * \ingroup solar_system_ephemerides
 */
struct PCL_CLASS SerializableEphemerisData
{
   /*!
    * Starting point of the time span covered by this Chebyshev polynomial
    * expansion.
    */
   TimePoint    startTime;

   /*!
    * Chebyshev polynomial expansion. If the coefficient series has been
    * truncated, only the truncated series will be stored in the XEPH file,
    * that is, truncated coefficients will be ignored.
    *
    * \sa ChebyshevFit::Truncate()
    */
   ChebyshevFit expansion;

   /*!
    * Memberwise constructor.
    */
   SerializableEphemerisData( TimePoint t, const ChebyshevFit& T ) :
      startTime( t ),
      expansion( T )
   {
   }

   /*!
    * Copy constructor.
    */
   SerializableEphemerisData( const SerializableEphemerisData& ) = default;

   /*!
    * Move constructor.
    */
   SerializableEphemerisData( SerializableEphemerisData&& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   SerializableEphemerisData& operator =( const SerializableEphemerisData& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   SerializableEphemerisData& operator =( SerializableEphemerisData&& ) = default;
};

/*!
 * \class pcl::SerializableEphemerisDataList
 * \brief Dynamic list of Chebyshev polynomial expansions for ephemeris serialization.
 *
 * %SerializableEphemerisDataList is a dynamic list of
 * SerializableEphemerisData instances to cover the entire time span of an
 * ephemeris file being serialized. Each instance in the list must cover a time
 * subspan in ascending time order without gaps. The value of the \a startTime
 * member of the first instance in the list must be equal to the lower bound of
 * the time span covered by the file, and the \a startTime member of each
 * subsequent instance in the list is equal to the upper bound of the subspan
 * covered by the previous instance. These conditions are enforced by the
 * EphemerisFile::Serialize() static member function, which is the standard
 * resource for generation of ephemeris files in XEPH format.
 *
 * \ingroup solar_system_ephemerides
 */
typedef Array<SerializableEphemerisData> SerializableEphemerisDataList;

// ----------------------------------------------------------------------------

/*!
 * \struct SerializableEphemerisObjectData
 * \brief A set of Chebyshev polynomial expansions and associated ancillary
 * data for ephemeris serialization.
 *
 * This structure associates a unique pair of object and origin identifiers, an
 * optional object name, an optional descriptive text, and one or two ordered
 * sequences of Chebyshev polynomial expansions for calculation of ephemerides
 * within a prescribed time span. These data are suitable for generation of an
 * ephemeris file in XEPH format with the EphemerisFile::Serialize() static
 * member function.
 *
 * The first sequence of Chebyshev polynomial expansions is mandatory for
 * calculation of ephemeris function values (typically, the components of a
 * position vector as a function of time). An additional sequence of expansions
 * for the first derivative (typically, the components of a velocity vector) is
 * optional. Derivative expansions should be used when the velocity of an
 * object is known very accurately upon ephemeris generation, and this high
 * accuracy must be preserved.
 *
 * All ephemeris data stored in XEPH files must be referred to the
 * International Celestial Reference System (ICRS/J2000.0).
 *
 * \sa EphemerisFile::Serialize()
 * \ingroup solar_system_ephemerides
 */
struct PCL_CLASS SerializableEphemerisObjectData
{
   /*!
    * Unique identifier of the object or item for which ephemeris data have
    * been generated. This should be a short string to uniquely identify the
    * object in the context of the ephemeris file being serialized. For
    * example, standard XEPH files serializing JPL DE/LE ephemerides use a
    * two/three letter convention for object identifiers: 'Me', 'Ve', 'EMB',
    * 'Ea', 'Ma', etc, respectively for Mercury, Venus, Earth-Moon barycenter,
    * Earth, Mars, etc. Object identifiers are considered as case-sensitive in
    * XEPH files.
    */
   IsoString                     objectId;

   /*!
    * Identifier of the origin of coordinates (case-sensitive). This can be the
    * unique identifier of a solar system object, or the identifier of an
    * arbitrary object or location, which will be treated as
    * implementation-specific, that is, specific to the data being serialized
    * and to the procedures employed to generate them.
    *
    * For example, if the ephemeris data being serialized are geocentric, this
    * member should be "Ea", which is the unique identifier of the Earth's
    * center in standard fundamental ephemerides files. Similarly, if the
    * ephemerides are heliocentric, this member should be "Sn". If the
    * ephemeris data are with respect to the solar system barycenter, this
    * member must be "SSB", also for coherence with fundamental ephemerides.
    */
   IsoString                     originId;

   /*!
    * Optional Name of the object or item for which ephemeris data have been
    * generated. This can be an arbitrary string, such as 'Saturn', 'Iris', or
    * 'C/1996 B2'. However, bear in mind that ephemeris data can be accessed
    * both by object identifiers and object names, so there should be no
    * duplicate names. Object names are considered as case-insensitive in XEPH
    * files, so all of 'Jupiter', 'jupiter' and 'JUPITER' are equivalent.
    */
   String                        objectName;

   /*!
    * Optional information about the object and/or the ephemeris data being
    * serialized.
    */
   String                        description;

   /*!
    * Two dynamic lists of Chebyshev polynomial expansions. The first list is
    * mandatory for approximation of function values (typically, the components
    * of a position vector). The second list is optional for approximation of
    * first derivatives (typically, the components of a velocity vector). Both
    * sequences must cover the entire ephemeris time span with arbitrary
    * subspans in ascending time order without any gaps.
    */
   SerializableEphemerisDataList data[ 2 ];

   /*
    * Absolute magnitude. H is the visual magnitude of the object as seen at 1
    * au of the Earth, 1 au from the Sun, and with a phase angle of 0 degrees.
    *
    * <b>References</b>
    *
    * E. Bowell et al., <em>Asteroids II</em>, R. P. Binzel et al. (eds.), The
    * University of Arizona Press, Tucson, 1989, pp. 549-554.
    *
    * Urban, Sean E., Kenneth Seidelmann, P., ed. (2013), <em>The Explanatory
    * Supplement to the Astronomical Almanac</em> 3rd Edition, Section 10.4.3.
    */
   Optional<double>              H;

   /*
    * Slope parameter. See the H data member for references.
    */
   Optional<double>              G;

   /*
    * Color index B-V in magnitudes.
    */
   Optional<double>              B_V;

   /*
    * Diameter of the object in km.
    */
   Optional<double>              D;

   /*!
    * Memberwise constructor.
    */
   SerializableEphemerisObjectData( const IsoString& id,
                                    const IsoString& origin,
                                    const String& name = String(),
                                    const String& desc = String() ) :
      objectId( id.Trimmed() ),
      originId( origin.Trimmed() ),
      objectName( name.Trimmed() ),
      description( desc )
   {
   }

   /*!
    * Copy constructor.
    */
   SerializableEphemerisObjectData( const SerializableEphemerisObjectData& ) = default;

   /*!
    * Move constructor.
    */
   SerializableEphemerisObjectData( SerializableEphemerisObjectData&& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   SerializableEphemerisObjectData& operator =( const SerializableEphemerisObjectData& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   SerializableEphemerisObjectData& operator =( SerializableEphemerisObjectData&& ) = default;
};

/*!
 * \class pcl::SerializableEphemerisObjectDataList
 * \brief Dynamic list of per-object data for ephemeris serialization.
 * \ingroup solar_system_ephemerides
 */
typedef Array<SerializableEphemerisObjectData> SerializableEphemerisObjectDataList;

// ----------------------------------------------------------------------------

/*!
 * \struct EphemerisObject
 * \brief Identifiers and descriptive data of an object available in an
 * ephemeris file.
 *
 * This structure associates the unique identifier, origin identifier, name and
 * description of an object or item available for ephemeris calculations. All
 * of the data members in this structure are specific to a particular ephemeris
 * file. A dynamic list of %EphemerisObject instances (EphemerisObjectList) is
 * returned by the EphemerisFile::Objects() member function.
 *
 * \sa EphemerisFile::Objects()
 * \ingroup solar_system_ephemerides
 */
struct PCL_CLASS EphemerisObject
{
   /*!
    * Unique identifier of an object or item for which ephemeris data are
    * available. This is a short string that uniquely identifies the object in
    * the context of an ephemeris file.
    *
    * For example, standard XEPH files serializing JPL DE/LE ephemerides use a
    * two/three letter convention for object identifiers: 'Me', 'Ve', 'EMB',
    * 'Ea', 'Ma', etc, respectively for Mercury, Venus, Earth-Moon barycenter,
    * Earth, Mars, etc. Object identifiers are considered as case-sensitive in
    * XEPH files.
    */
   IsoString                     objectId;

   /*!
    * Identifier of the origin of coordinates (case-sensitive). This can be the
    * unique identifier of a solar system object, or the identifier of an
    * arbitrary object or location, specific to a particular ephemeris file.
    *
    * For example, if the object provides geocentric coordinates, this member
    * should be "Ea", which is the unique identifier of the Earth's center in
    * standard fundamental ephemerides files. Similarly, if the ephemerides are
    * heliocentric, this member should be "Sn". If the ephemeris data are with
    * respect to the solar system barycenter, this member must be "SSB", also
    * for coherence with fundamental ephemerides.
    */
   IsoString                     originId;

   /*!
    * Optional Name of the object or item for which ephemeris data are
    * available. This can be an arbitrary string, such as 'Saturn', 'Iris', or
    * 'C/1996 B2'. Object names are considered as case-insensitive in XEPH
    * files, so all of 'Jupiter', 'jupiter' and 'JUPITER' are equivalent.
    */
   String                        objectName;

   /*!
    * Optional information about the object and/or the ephemeris data.
    */
   String                        objectDescription;

   /*!
    * Absolute magnitude. H is the visual magnitude of the object as seen at 1
    * au of the Earth, 1 au from the Sun, and with a phase angle of 0 degrees.
    *
    * <b>References</b>
    *
    * E. Bowell et al., <em>Asteroids II</em>, R. P. Binzel et al. (eds.), The
    * University of Arizona Press, Tucson, 1989, pp. 549-554.
    *
    * Urban, Sean E., Kenneth Seidelmann, P., ed. (2013), <em>The Explanatory
    * Supplement to the Astronomical Almanac</em> 3rd Edition, Section 10.4.3.
    */
   Optional<double>              H;

   /*!
    * Slope parameter. See the H data member for references.
    */
   Optional<double>              G;

   /*!
    * Color index B-V in magnitudes.
    */
   Optional<double>              B_V;

   /*!
    * Diameter of the object in km.
    */
   Optional<double>              D;

   /*!
    * Memberwise constructor.
    */
   EphemerisObject( const IsoString& objectId_,
                    const IsoString& originId_,
                    const String&    objectName_ = String(),
                    const String&    objectDescription_ = String(),
                    Optional<double> H_ = Optional<double>(),
                    Optional<double> G_ = Optional<double>(),
                    Optional<double> B_V_ = Optional<double>(),
                    Optional<double> D_ = Optional<double>() ) :
      objectId( objectId_ ),
      originId( originId_ ),
      objectName( objectName_ ),
      objectDescription( objectDescription_ ),
      H( H_ ),
      G( G_ ),
      B_V( B_V_ ),
      D( D_ )
   {
   }

   /*!
    * Copy constructor.
    */
   EphemerisObject( const EphemerisObject& ) = default;

   /*!
    * Move constructor.
    */
   EphemerisObject( EphemerisObject&& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   EphemerisObject& operator =( const EphemerisObject& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   EphemerisObject& operator =( EphemerisObject&& ) = default;
};

/*!
 * \class pcl::EphemerisObjectList
 * \brief Dynamic list of object identifiers and descriptions.
 * \ingroup solar_system_ephemerides
 */
typedef Array<EphemerisObject> EphemerisObjectList;

// ----------------------------------------------------------------------------

/*!
 * \class EphemerisFile
 * \brief Solar system ephemerides from XEPH files.
 *
 * This class implements ephemerides of solar system bodies computed from data
 * stored in XEPH (Extensible Ephemeris Data format) files. It also implements
 * serialization of ephemeris data in the XEPH file format.
 *
 * On the PixInsight/PCL platform, the XEPH file format allows for efficient
 * ephemeris calculations through Chebyshev polynomial expansions stored as raw
 * binary data. An XEPH file stores multiple arrays of Chebyshev coefficient
 * series accessible by means of fast indexed file search algorithms and
 * structures, along with auxiliary data and metadata required for ephemeris
 * calculations.
 *
 * Calculation of state vectors (such as position and velocity vectors) for
 * specific objects is performed through a dedicated client subclass, namely
 * EphemerisFile::Handle. This class implements transparent file seek and read
 * operations, as well as fast, lock-free multithreaded evaluation of Chebyshev
 * polynomials.
 *
 * XEPH ephemeris files allow for calculation of rectangular coordinates
 * referred to the axes of the International Celestial Reference System
 * (ICRS/J2000.0). Positions are given in au and velocities in au/day for all
 * solar system objects, except planetocentric coordinates of natural
 * satellites, including the geocentric Moon, for which positions and
 * velocities are given in kilometers and km/day, respectively. Angles
 * (nutations and librations) are given in radians, and time differences (such
 * as TT-TDB) in seconds.
 *
 * An XEPH file storing up-to-date JPL DE/LE ephemeris data is part of all
 * standard PixInsight distributions since 1.8.5 versions released Fall 2018.
 * As of writing this documentation, the standard XEPH file provides the
 * complete JPL DE438/LE438 ephemerides. See the
 * EphemerisFile::FundamentalEphemerides() static member function for detailed
 * information.
 *
 * \ingroup solar_system_ephemerides
 */
class PCL_CLASS EphemerisFile
{
public:

   /*!
    * Default constructor.
    *
    * Constructs an invalid instance that cannot be used until initialized by
    * calling the Open() member function.
    */
   EphemerisFile()
   {
   }

   /*!
    * Constructs an &EphemerisFile instance initialized from the specified
    * ephemeris data file in XEPH format.
    *
    * In the event of errors or invalid data, this constructor will throw the
    * appropriate Error exception.
    */
   EphemerisFile( const String& filePath )
   {
      Open( filePath );
   }

   /*!
    * Move constructor.
    */
   EphemerisFile( EphemerisFile&& x ) :
      m_file( std::move( x.m_file ) ),
      m_startTime( x.m_startTime ),
      m_endTime( x.m_endTime ),
      m_constants( std::move( x.m_constants ) ),
      m_index( std::move( x.m_index ) )
   {
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   EphemerisFile& operator =( EphemerisFile&& x )
   {
      m_file = std::move( x.m_file );
      m_startTime = x.m_startTime;
      m_endTime = x.m_endTime;
      m_constants = std::move( x.m_constants );
      m_index = std::move( x.m_index );
      return *this;
   }

   /*!
    * Deleted copy constructor. %EphemerisFile instances are unique, hence
    * cannot be copied.
    */
   EphemerisFile( const EphemerisFile& ) = delete;

   /*!
    * Deleted copy assignment operator. %EphemerisFile instances are unique,
    * hence cannot be copied.
    */
   EphemerisFile& operator =( const EphemerisFile& ) = delete;

   /*!
    * Virtual destructor.
    */
   virtual ~EphemerisFile() noexcept( false );

   /*!
    * Initializes this object to give access to the specified ephemeris data
    * file in XEPH format.
    *
    * This member function opens an existing file at the specified \a filePath,
    * loads and parses its XML header, and loads the file indexes ready for
    * fast access to ephemeris data. The file will remain open until this
    * object is destroyed, or until a new call to this function.
    *
    * If a previous file was already opened by this instance, it will be closed
    * and all associated control and file indexing structures will be destroyed
    * and deallocated, before accessing the new file.
    *
    * \warning If this object has active ephemeris calculation handles, no
    * action will be taken and an Error exception will be thrown. See
    * EphemerisFile::Handle and NumberOfHandles().
    */
   void Open( const String& filePath );

   /*!
    * Closes the ephemeris file represented by this object and resets all
    * internal structures to a default, uninitialized state.
    *
    * If a previous file was already opened by this instance, it will be closed
    * and all associated control and file indexing structures will be destroyed
    * and deallocated. If no file is currently open, calling this member has no
    * effect.
    *
    * \warning If this object has active ephemeris calculation handles, no
    * action will be taken and an Error exception will be thrown. See
    * EphemerisFile::Handle and NumberOfHandles().
    */
   void Close();

   /*!
    * Returns true iff this object has an open ephemeris file and is ready for
    * ephemeris data retrieval.
    */
   bool IsOpen() const
   {
      return m_file.IsOpen();
   }

   /*!
    * Returns the path of the ephemeris file represented by this object.
    * Returned file paths are always absolute, full file paths.
    */
   const String& FilePath() const
   {
      return m_file.FilePath();
   }

   /*!
    * Starting point of the time span covered by this ephemeris, or the
    * earliest time point for which ephemerides can be calculated using this
    * object.
    */
   TimePoint StartTime() const
   {
      return m_startTime;
   }

   /*!
    * Ending point of the time span covered by this ephemeris, or the latest
    * time point for which ephemerides can be calculated using this object.
    */
   TimePoint EndTime() const
   {
      return m_endTime;
   }

   /*!
    * Returns a reference to the dynamic list of constants stored in the
    * ephemeris data file represented by this object. These constants are
    * name/value pairs, where keys are 8-bit strings and values are double
    * precision floating point numbers.
    *
    * Typically, for XEPH files that store fundamental JPL DE/LE ephemerides
    * this member function returns the list of constants used by the
    * corresponding DE/LE numerical integration.
    *
    * The returned list is sorted by constant name in ascending order.
    */
   const EphemerisConstantList& Constants() const
   {
      return m_constants;
   }

   /*!
    * Returns true iff this instance knows a numerical constant with the
    * specified \a name.
    */
   bool IsConstantAvailable( const IsoString& name ) const
   {
      return BinarySearch( m_constants.Begin(), m_constants.End(),
                           EphemerisConstant( name ) ) != m_constants.End();
   }

   /*!
    * Returns the value of an ephemeris constant given by its \a name.
    *
    * If no constant is available with the specified \a name
    * (case-insensitive) in the XEPH file represented by this object, this
    * function throws an Error exception.
    */
   double ConstantValue( const IsoString& name ) const
   {
      EphemerisConstantList::const_iterator i =
         BinarySearch( m_constants.Begin(), m_constants.End(), EphemerisConstant( name ) );
      if ( i == m_constants.End() )
         throw Error( "Undefined ephemeris constant '" + name + '\'' );
      return i->value;
   }

   /*!
    * Returns a dynamic list of EphemerisObject instances describing all of the
    * objects available in this file for ephemeris calculations.
    *
    * The returned list is sorted by object and origin identifiers (in that
    * order of precedence) in ascending order.
    */
   EphemerisObjectList Objects() const
   {
      EphemerisObjectList objects;
      for ( const Index& ix : m_index )
         objects << EphemerisObject( ix.objectId, ix.originId,
                                     ix.objectName, ix.objectDescription,
                                     ix.H, ix.G, ix.B_V, ix.D );
      return objects;
   }

   /*!
    * Returns true iff this instance contains ephemeris data for the specified
    * \a object, given by its identifier or name, with respect to the specified
    * \a origin.
    *
    * The specified \a object string can be either an object identifier
    * (case-sensitive), or an object name (case-insensitive) encoded as UTF-8.
    * For example, all of 'Ju', 'Jupiter', 'jupiter' and 'JUPITER' refer to
    * Jupiter in a standard XEPH file storing fundamental JPL DE/LE
    * ephemerides. In this example, 'Ju' is a case-sensitive object identifier,
    * thus 'ju' and 'JU' are not valid and this function would return false for
    * both of them.
    *
    * \a origin is the identifier of an origin of coordinates. If an empty
    * string is specified (which is the default parameter value), this function
    * will return true if the file contains any ephemeris data for the
    * specified \a object, irrespective of the origin. Otherwise an exact match
    * of the \a origin identifier will be required.
    *
    * See the FundamentalEphemerides() static member function for information
    * about the objects supported by standard XEPH files storing fundamental
    * ephemerides.
    */
   bool IsObjectAvailable( const IsoString& object, const IsoString& origin = IsoString() ) const
   {
      Array<Index>::const_iterator i = BinarySearch( m_index.Begin(), m_index.End(), Index( object, origin ) );
      if ( i != m_index.End() )
         return true;
      String name = object.UTF8ToUTF16();
      for ( const Index& index : m_index )
         if ( index.objectName.CompareIC( name ) == 0 )
            if ( origin.IsEmpty() || index.originId == origin )
               return true;
      return false;
   }

   /*!
    * Returns the name of the specified \a object, with respect to the
    * specified \a origin.
    *
    * Both \a object and \a origin must be object identifiers. If \a origin is
    * an empty string (which is the default parameter value), this function
    * will return the name of the first object found with the specified
    * identifier, irrespective of the origin. Otherwise an exact match of the
    * \a origin identifier will be required.
    *
    * If no object with the required conditions is available in this ephemeris
    * file, this function returns an empty string.
    */
   String ObjectName( const IsoString& object, const IsoString& origin = IsoString() ) const
   {
      Array<Index>::const_iterator i = BinarySearch( m_index.Begin(), m_index.End(), Index( object, origin ) );
      if ( i != m_index.End() )
         return i->objectName;
      return String();
   }

   /*!
    * Returns a reference to the (immutable) metadata items available in the
    * ephemeris data file loaded by this object.
    */
   const EphemerisMetadata& Metadata() const
   {
      return m_metadata;
   }

   /*!
    * Returns the number of handles currently active for this ephemeris file.
    *
    * If an %EphemerisFile object has active handles, also known as \e child
    * \e handles, destroying it will most likely lead to a crash, since any
    * activity performed by a child handle will make reference to a nonexistent
    * parent object.
    *
    * This function is thread-safe.
    *
    * \sa pcl::EphemerisFile::Handle
    */
   int NumberOfHandles() const
   {
      return m_handleCount.Load();
   }

   /*!
    * Generates a file to store solar system ephemeris data in XEPH format.
    *
    * \param filePath      Path to the file that will be generated in the local
    *                      filesystem. The file name should carry the '.xeph'
    *                      suffix.
    *
    * \param startTime     Lower bound of the entire time span for which the
    *                      ephemeris data being serialized is valid.
    *
    * \param endTime       Upper bound of the entire time span for which the
    *                      ephemeris data being serialized is valid.
    *
    * \param data          Reference to an array of per-object ephemeris data,
    *                      including Chebyshev polynomial expansions for the
    *                      ephemeris function (such as position) and,
    *                      optionally, for its first derivative (such as
    *                      velocity), as well as object identifiers and object
    *                      names.
    *
    * \param metadata      Reference to an EphemerisMetadata structure with
    *                      optional metadata information that will be included
    *                      in the generated XEPH file.
    *
    * \param constants     Reference to an array of name/value pairs used to
    *                      represent a set of numerical constants relevant to
    *                      the ephemerides being serialized If an empty array
    *                      is specified (as the default parameter value), no
    *                      numerical constant will be included in the generated
    *                      XEPH file.
    *
    * The entire time span covered by an ephemeris file, from \a startTime to
    * \a endTime, is usually subdivided into many small chunks or subspans,
    * each of them with a relatively short polynomial expansion. The duration
    * of each subspan is defined in a way such that the motion of the object
    * for which positions are being calculated is sufficiently smooth to be
    * fitted within the time subspan by truncated Chebyshev polynomials with
    * relatively few coefficients (typically in the range of 15 to 30
    * coefficients) to achieve a prescribed accuracy. The faster and more
    * perturbed the object's motion is, the shorter subspans are necessary to
    * fit an accurate representation of the object's orbit.
    *
    * In the event of invalid, incongruent or malformed data, or if an I/O
    * error occurs, this function will throw an Error exception.
    *
    * \warning If a file already exists at the specified path, its previous
    * contents will be lost after calling this function.
    */
   static void Serialize( const String& filePath,
                          TimePoint startTime, TimePoint endTime,
                          const SerializableEphemerisObjectDataList& data,
                          const EphemerisMetadata& metadata = EphemerisMetadata(),
                          const EphemerisConstantList& constants = EphemerisConstantList() );

   /*!
    * Returns a reference to the global fundamental ephemerides file currently
    * defined by the running PixInsight platform.
    *
    * Under normal running conditions, the returned object provides ephemeris
    * data for at least the following objects:
    *
    * <table border="1" cellpadding="4" cellspacing="0">
    * <tr><td>Identifier</td><td>Name</td></tr>
    * <tr><td>Me</td><td>Mercury</td></tr>
    * <tr><td>Ve</td><td>Venus</td></tr>
    * <tr><td>EMB</td><td>Earth-Moon barycenter</td></tr>
    * <tr><td>Ma</td><td>Mars' barycenter</td></tr>
    * <tr><td>Ju</td><td>Jupiter's barycenter</td></tr>
    * <tr><td>Sa</td><td>Saturn's barycenter</td></tr>
    * <tr><td>Ur</td><td>Uranus' barycenter</td></tr>
    * <tr><td>Ne</td><td>Neptune's barycenter</td></tr>
    * <tr><td>Pl</td><td>Pluto's barycenter</td></tr>
    * <tr><td>Mn</td><td>Moon's geometric center with respect to Earth's center.</td></tr>
    * <tr><td>Sn</td><td>Sun's geometric center</td></tr>
    * <tr><td>Ea</td><td>Earth's geometric center</td></tr>
    * </table>
    *
    * With the only exception of the Moon ("Mn" identifier), ephemeris data for
    * all of the objects above are provided relative to the solar system
    * barycenter ("SSB" identifier).
    *
    * Additional items may also be available, depending on specific file
    * versions and compilations:
    *
    * <table border="1" cellpadding="4" cellspacing="0">
    * <tr><td>Identifier</td><td>Name</td></tr>
    * <tr><td>Lbr</td><td>Lunar librations (Euler angles) in radians</td></tr>
    * <tr><td>Nut</td><td>Nutation angles in radians</td></tr>
    * <tr><td>TT_TDB</td><td>TT-TDB difference at the geocenter in seconds.</td></tr>
    * </table>
    *
    * As of writing this documentation, the standard fundamental ephemeris file
    * provides the complete JPL's DE438/LE438 ephemerides, but nutations,
    * librations and time differences are not included.
    *
    * The fundamental ephemeris file can be overridden by the caller module.
    * See the OverrideFundamentalEphemerides() member function for more
    * information.
    *
    * This static member function is thread-safe. It can be called safely from
    * multiple execution threads running concurrently.
    */
   static const EphemerisFile& FundamentalEphemerides();

   /*!
    * Returns a reference to the global short-term fundamental ephemerides file
    * currently defined by the running PixInsight platform.
    *
    * See the FundamentalEphemerides() static member function for information
    * on fundamental ephemerides and their status in current versions of
    * PixInsight.
    *
    * Under normal running conditions, the returned object should be a
    * shortened version (that is, covering a shorter time span) of the standard
    * fundamental ephemerides file. As of writing this documentation, the
    * standard short-term fundamental ephemeris file provides DE438/LE438
    * ephemerides for the period from 1850 January 1.0 to 2150 December 32.0.
    *
    * The short-term fundamental ephemeris file can be overridden by the caller
    * module. See the OverrideShortTermFundamentalEphemerides() member function
    * for more information.
    *
    * This static member function is thread-safe. It can be called safely from
    * multiple execution threads running concurrently.
    */
   static const EphemerisFile& ShortTermFundamentalEphemerides();

   /*!
    * Returns a reference to the global asteroid ephemerides file currently
    * defined by the running PixInsight platform.
    *
    * Under normal running conditions, the returned object provides ephemeris
    * data for a set of asteroids with relevant masses. In a standard asteroid
    * ephemeris file, object identifiers are asteroid numbers and object names
    * are asteroid designations; for example:
    *
    * <table border="1" cellpadding="4" cellspacing="0">
    * <tr><td>Identifier</td><td>Name</td></tr>
    * <tr><td>1</td><td>Ceres</td></tr>
    * <tr><td>2</td><td>Pallas</td></tr>
    * <tr><td>3</td><td>Juno</td></tr>
    * <tr><td>4</td><td>Vesta</td></tr>
    * <tr><td>5</td><td>Astraea</td></tr>
    * <tr><td>...</td><td>...</td></tr>
    * <tr><td>702</td><td>Alauda</td></tr>
    * <tr><td>703</td><td>Noemi</td></tr>
    * <tr><td>704</td><td>Interamnia</td></tr>
    * <tr><td>...</td><td>...</td></tr>
    * </table>
    *
    * Asteroid ephemeris data are provided relative to the solar system
    * barycenter ("SSB" identifier), with position and velocity coordinates
    * coherent with global fundamental ephemerides.
    *
    * As of writing this documentation, the standard asteroid ephemeris file
    * provides the complete set of 343 asteroids used for the numerical
    * integration of DE430 ephemerides, with barycentric coordinates coherent
    * with DE438.
    *
    * The asteroid ephemeris file can be overridden by the caller module. See
    * the OverrideAsteroidEphemerides() member function for more information.
    *
    * This static member function is thread-safe. It can be called safely from
    * multiple execution threads running concurrently.
    */
   static const EphemerisFile& AsteroidEphemerides();

   /*!
    * Returns a reference to the global short-term asteroid ephemerides file
    * currently defined by the running PixInsight platform.
    *
    * See the AsteroidEphemerides() static member function for information
    * on asteroid ephemerides and their status in current versions of
    * PixInsight.
    *
    * Under normal running conditions, the returned object should be a
    * shortened version (that is, covering a shorter time span) of the standard
    * asteroid ephemerides file. As of writing this documentation, the
    * standard short-term asteroid ephemeris file covers the period from 1950
    * January 1.0 to 2100 January 32.0.
    *
    * The short-term asteroid ephemeris file can be overridden by the caller
    * module. See the OverrideShortTermAsteroidEphemerides() member function
    * for more information.
    *
    * This static member function is thread-safe. It can be called safely from
    * multiple execution threads running concurrently.
    */
   static const EphemerisFile& ShortTermAsteroidEphemerides();

   /*!
    * Returns a reference to the global nutation model ephemeris file currently
    * defined by the running PixInsight platform.
    *
    * Under normal running conditions, the returned object provides Chebyshev
    * polynomial expansions for the current IAU nutation model. As of writing
    * this documentation, the standard nutation model file provides the IAU
    * 2006/2000A_R nutation model (MHB2000 luni-solar and planetary nutation
    * with adjustments to match the IAU 2006 precession).
    *
    * The returned object should provide at least one object with the "IAUNut"
    * identifier, which can be used to approximate the implemented nutation
    * theory with a child EphemerisFile::Handle object.
    *
    * The nutation model ephemeris file can be overridden by the caller module.
    * See the OverrideNutationModel() member function for more information.
    *
    * This static member function is thread-safe. It can be called safely from
    * multiple execution threads running concurrently.
    */
   static const EphemerisFile& NutationModel();

   /*!
    * Returns a reference to the global short-term nutation model ephemeris
    * file currently defined by the running PixInsight platform.
    *
    * See the NutationModel() static member function for information on
    * nutation model ephemerides and their status in current versions of
    * PixInsight.
    *
    * Under normal running conditions, the returned object should be a
    * shortened version (that is, covering a shorter time span) of the standard
    * nutation model ephemerides file. As of writing this documentation, the
    * standard short-term nutation model file provides the IAU 2006/2000A_R
    * nutation model for the period from 1850 January 1.0 to 2150 December
    * 32.0.
    *
    * The returned object should provide at least one object with the "IAUNut"
    * identifier, which can be used to approximate the implemented nutation
    * theory with a child EphemerisFile::Handle object.
    *
    * The short-term nutation model ephemeris file can be overridden by the
    * caller module. See the OverrideShortTermNutationModel() member function
    * for more information.
    *
    * This static member function is thread-safe. It can be called safely from
    * multiple execution threads running concurrently.
    */
   static const EphemerisFile& ShortTermNutationModel();

   /*!
    * Returns the path to the global database file of observed Delta T values.
    *
    * Delta T is the difference TT-UT1 in seconds. In current versions of
    * PixInsight the Delta T database is a plain text file generated with values
    * taken from the following online references:
    *
    * For the period 1657-1973.0:\n
    * http://maia.usno.navy.mil/ser7/historic_deltat.data
    *
    * For the period 1973 Feb 1 to the beginning of the current year:\n
    * http://maia.usno.navy.mil/ser7/deltat.data
    *
    * For predicted Delta T values until 2025 approximately (as of writing this
    * documentation, October 2018):\n
    * http://maia.usno.navy.mil/ser7/deltat.preds
    *
    * Delta T data files are expected to follow a simple format where each text
    * line provides a TT/DeltaT pair. The exact format is described at the top
    * of the corresponding file included in the current PixInsight
    * distribution.
    *
    * Outside of the period from 1657 to the current year, the current
    * PixInsight/PCL implementation uses polynomial expressions taken from
    * <em>Five Millennium Canon of Solar Eclipses</em>, by Fred Espenak and
    * Jean Meeus (NASA/TP–2006–214141, Revision 1.0, 2007).
    *
    * The Delta T database file can be overridden by the caller module. See the
    * OverrideDeltaTDataFilePath() member function for more information.
    *
    * This static member function is thread-safe. It can be called safely from
    * multiple execution threads running concurrently.
    *
    * \sa TimePoint::DeltaT()
    */
   static String DeltaTDataFilePath();

   /*!
    * Returns the path to the global database file of Delta AT values.
    *
    * Delta AT is the difference TAI-UTC in seconds. In current versions of
    * PixInsight the Delta AT database is a plain text file generated with
    * values taken from the following online reference:
    *
    * http://maia.usno.navy.mil/ser7/tai-utc.dat
    *
    * Delta AT data files are expected to follow a simple format where each
    * text line provides a UTC/DeltaAT pair. The exact format is described at
    * the top of the corresponding file included in the current PixInsight
    * distribution.
    *
    * The Delta AT database file can be overridden by the caller module. See
    * the OverrideDeltaATDataFilePath() member function for more information.
    *
    * This static member function is thread-safe. It can be called safely from
    * multiple execution threads running concurrently.
    *
    * \sa TimePoint::DeltaAT()
    */
   static String DeltaATDataFilePath();

   /*!
    * Returns the path to the global database file of CIP positions referred to
    * the ITRS.
    *
    * The position of the Celestial Intermediate Pole (CIP) in the
    * International Terrestrial Reference System (ITRS) is necessary to compute
    * polar motion corrections applied to topocentric coordinates of solar
    * system bodies. These corrections are relevant for the topocentric
    * position of the Moon at the milliarcsecond level.
    *
    * In current versions of PixInsight the CIP_ITRS database is a plain text
    * file generated with values provided by the IERS Rapid Service/Prediction
    * Center. As of writing this documentation, the main online reference is:
    *
    * http://hpiers.obspm.fr/iers/eop/eopc01/eopc01.iau2000.1846-now
    */
   static String CIP_ITRSDataFilePath();

   /*!
    * Override the default fundamental ephemerides file.
    *
    * The specified \a filePath must be a valid path to an existing file in
    * XEPH format, which must provide fundamental solar system ephemerides. See
    * the FundamentalEphemerides() member function for a comprehensive
    * description.
    *
    * After calling this member function, all fundamental ephemerides will be
    * calculated using the specified XEPH file, which will be installed
    * automatically upon the first call to FundamentalEphemerides().
    *
    * This function is useful to build standalone applications that don't
    * depend on a running PixInsight core application, which is necessary to
    * retrieve the default file names and directories from global settings.
    */
   static void OverrideFundamentalEphemerides( const String& filePath );

   /*!
    * Override the default short-term fundamental ephemerides file.
    *
    * The specified \a filePath must be a valid path to an existing file in
    * XEPH format, which must provide fundamental solar system ephemerides. See
    * the FundamentalEphemerides() member function for a comprehensive
    * description.
    *
    * After calling this member function, all short-term fundamental
    * ephemerides will be calculated using the specified XEPH file, which will
    * be installed automatically upon the first call to
    * ShortTermFundamentalEphemerides().
    *
    * This function is useful to build standalone applications that don't
    * depend on a running PixInsight core application, which is necessary to
    * retrieve the default file names and directories from global settings.
    */
   static void OverrideShortTermFundamentalEphemerides( const String& filePath );

   /*!
    * Override the default asteroid ephemerides file.
    *
    * The specified \a filePath must be a valid path to an existing file in
    * XEPH format, which must provide asteroid ephemerides coherent with the
    * global fundamental ephemerides being used. See the AsteroidEphemerides()
    * member function for a more comprehensive description.
    *
    * After calling this member function, all asteroid ephemerides will be
    * calculated using the specified XEPH file, which will be installed
    * automatically upon the first call to AsteroidEphemerides().
    *
    * This function is useful to build standalone applications that don't
    * depend on a running PixInsight core application, which is necessary to
    * retrieve the default file names and directories from global settings.
    */
   static void OverrideAsteroidEphemerides( const String& filePath );

   /*!
    * Override the default short-term asteroid ephemerides file.
    *
    * The specified \a filePath must be a valid path to an existing file in
    * XEPH format, which must provide asteroid ephemerides coherent with the
    * global fundamental ephemerides being used. See the AsteroidEphemerides()
    * member function for a more comprehensive description.
    *
    * After calling this member function, all short-term asteroid ephemerides
    * will be calculated using the specified XEPH file, which will be installed
    * automatically upon the first call to ShortTermAsteroidEphemerides().
    *
    * This function is useful to build standalone applications that don't
    * depend on a running PixInsight core application, which is necessary to
    * retrieve the default file names and directories from global settings.
    */
   static void OverrideShortTermAsteroidEphemerides( const String& filePath );

   /*!
    * Override the global nutation model ephemeris file.
    *
    * The specified \a filePath must be a valid path to an existing file in
    * XEPH format, which must provide estimates of nutation angles compliant
    * with the current IAU nutation model. See the NutationModel() member
    * function for more details.
    *
    * After calling this member function, the nutation model used for ephemeris
    * calculations will be provided by the specified XEPH file, which will
    * be installed automatically upon the first call to NutationModel().
    *
    * This function is useful to build standalone applications that don't
    * depend on a running PixInsight core application, which is necessary to
    * retrieve the default file names and directories from global settings.
    */
   static void OverrideNutationModel( const String& filePath );

   /*!
    * Override the global short-term nutation model ephemeris file.
    *
    * The specified \a filePath must be a valid path to an existing file in
    * XEPH format, which must provide estimates of nutation angles compliant
    * with the current IAU nutation model. See the NutationModel() member
    * function for more details.
    *
    * After calling this member function, the nutation model used for
    * short-term ephemeris calculations will be provided by the specified XEPH
    * file, which will be installed automatically upon the first call to
    * ShortTermNutationModel().
    *
    * This function is useful to build standalone applications that don't
    * depend on a running PixInsight core application, which is necessary to
    * retrieve the default file names and directories from global settings.
    */
   static void OverrideShortTermNutationModel( const String& filePath );

   /*!
    * Override the global Delta T database.
    *
    * The specified \a filePath must be a valid path to an existing file in a
    * plain text database format compatible with the standard Delta T database
    * included in PixInsight distributions. See the DeltaTDataFilePath() member
    * function for more details.
    *
    * After calling this member function, Delta T values will be calculated by
    * interpolation from the data provided by the specified file, which will be
    * loaded and parsed automatically upon the first call (explicit or
    * implicit) to TimePoint::DeltaT(). However, for performance and
    * modularization reasons, once a Delta T database has been loaded there is
    * no way to change it, so calling this function again will have no effect.
    *
    * This function is useful to build standalone applications that don't
    * depend on a running PixInsight core application, which is necessary to
    * retrieve the default file names and directories from global settings.
    */
   static void OverrideDeltaTDataFilePath( const String& filePath );

   /*!
    * Override the global Delta AT database.
    *
    * The specified \a filePath must be a valid path to an existing file in a
    * plain text database format compatible with the standard Delta AT database
    * included in PixInsight distributions. See the DeltaATDataFilePath()
    * member function for more details.
    *
    * After calling this member function, Delta AT values will be calculated by
    * interpolation from the data provided by the specified file, which will be
    * loaded and parsed automatically upon the first call (explicit or
    * implicit) to TimePoint::DeltaAT(). However, for performance and
    * modularization reasons, once a Delta AT database has been loaded there is
    * no way to change it, so calling this function again will have no effect.
    *
    * This function is useful to build standalone applications that don't
    * depend on a running PixInsight core application, which is necessary to
    * retrieve the default file names and directories from global settings.
    */
   static void OverrideDeltaATDataFilePath( const String& filePath );

   /*!
    * Override the global database of CIP positions in the ITRS.
    *
    * The specified \a filePath must be a valid path to an existing file in a
    * plain text database format compatible with the standard CIP_ITRS database
    * included in PixInsight distributions. See the CIP_ITRSDataFilePath()
    * member function for more details.
    *
    * After calling this member function, CIP coordinates in the ITRS will be
    * calculated by interpolation from the data provided by the specified file,
    * which will be loaded and parsed automatically upon the first call
    * (explicit or implicit) to Position::CIP_ITRS(). However, for performance
    * and modularization reasons, once a CIP_ITRS database has been loaded
    * there is no way to change it, so calling this function again will have no
    * effect.
    *
    * This function is useful to build standalone applications that don't
    * depend on a running PixInsight core application, which is necessary to
    * retrieve the default file names and directories from global settings.
    */
   static void OverrideCIP_ITRSDataFilePath( const String& filePath );

private:

   /*!
    * \internal
    * Expansion index node structure.
    */
   struct IndexNode
   {
      int32 jdi = 0;      // Starting point of time span covered by this expansion, int( JD ).
      float jdf = 0;      // Ditto, frac( JD ).
      uint8 n[ 4 ] = {};  // For each component: number of Chebyshev coefficients in this expansion.
      int64 position = 0; // File byte position of first Chebyshev coefficient.

      constexpr bool operator ==( const IndexNode& n ) const
      {
         return jdi == n.jdi && jdf == n.jdf;
      }

      constexpr bool operator <( const IndexNode& n ) const
      {
         return jdi < n.jdi || jdi == n.jdi && jdf < n.jdf;
      }

      constexpr int NumberOfComponents() const
      {
         return (n[0] > 0) ? ((n[1] > 0) ? ((n[2] > 0) ? ((n[3] > 0) ? 4 : 3) : 2) : 1) : 0;
      }

      constexpr int NumberOfCoefficients() const
      {
         return n[0] + n[1] + n[2] + n[3];
      }

      TimePoint StartTime() const
      {
         return TimePoint( jdi, jdf );
      }
   };

   /*!
    * \internal
    * Expansion index structure.
    */
   struct Index
   {
      IsoString        objectId;          // Object identifier (mandatory, case-sensitive).
      IsoString        originId;          // Identifier of the origin of coordinates (mandatory, case-sensitive).
      String           objectName;        // Object name (optional, case-insensitive).
      String           objectDescription; // Object description (optional, arbitrary)
      Optional<double> H;                 // Absolute magnitude.
      Optional<double> G;                 // Slope parameter.
      Optional<double> B_V;               // Color index B-V in magnitudes.
      Optional<double> D;                 // Diameter of the object in km.
      Array<IndexNode> nodes[ 2 ];        // Expansion indexes: 0=function 1=derivative.

      Index( const IsoString& objectId_,
             const IsoString& originId_ = IsoString(),
             const String& objectName_ = String(),
             const String& objectDescription_ = String() ) :
         objectId( objectId_.Trimmed() ),
         originId( originId_.Trimmed() ),
         objectName( objectName_.Trimmed() ),
         objectDescription( objectDescription_.Trimmed() )
      {
      }

      bool operator ==( const Index& x ) const
      {
         return objectId == x.objectId && originId == x.originId;
      }

      bool operator <( const Index& x ) const
      {
         return (objectId != x.objectId) ? objectId < x.objectId : originId < x.originId;
      }

      bool HasDerivative() const
      {
         return !nodes[1].IsEmpty();
      }
   };

   mutable File                  m_file;
   mutable AtomicInt             m_handleCount;
   mutable Mutex                 m_mutex;
           TimePoint             m_startTime;
           TimePoint             m_endTime;
           EphemerisMetadata     m_metadata;
           EphemerisConstantList m_constants;
           Array<Index>          m_index;

   // Current data files.
   // If empty, i.e. if not overridden, use platform defaults.
   static String s_ephFilePath;
   static String s_ephFilePath_s;
   static String s_astFilePath;
   static String s_astFilePath_s;
   static String s_nutFilePath;
   static String s_nutFilePath_s;
   static String s_deltaTFilePath;
   static String s_deltaATFilePath;
   static String s_cipITRSFilePath;

   /*!
    * \internal
    * Returns a pointer to an Index structure for the specified object
    * identifier or name, with respect to the specified origin of coordinates.
    */
   const Index* FindObject( const IsoString& object, const IsoString& origin ) const
   {
      if ( origin.IsEmpty() )
      {
         for ( const Index& index : m_index )
            if ( index.objectId == object )
               return &index;
      }
      else
      {
         Array<Index>::const_iterator i = BinarySearch( m_index.Begin(), m_index.End(), Index( object, origin ) );
         if ( i != m_index.End() )
            return i;
      }
      String name = object.UTF8ToUTF16();
      for ( const Index& index : m_index )
         if ( index.objectName.CompareIC( name ) == 0 )
            if ( origin.IsEmpty() || index.originId == origin )
               return &index;
      throw Error( "Unavailable object '" + name + "\' with origin '" + String( origin ) + "'." );
   }

public:

   /*!
    * \class pcl::EphemerisFile::Handle
    * \brief Calculation of ephemerides from data stored in XEPH files.
    *
    * This subclass provides access to ephemeris data for a specific object
    * available in an ephemeris file. It can perform basic ephemeris
    * calculations, including state vectors and its first derivatives (such as
    * position and velocity), and performs all the low-level file seek and read
    * operations transparently and efficiently.
    *
    * Data stored in an XEPH ephemeris file generates rectangular coordinates
    * referred to the axes of the International Celestial Reference System
    * (ICRS/J2000.0). Positions are given in au and velocities in au/day for
    * all solar system objects, except planetocentric coordinates of natural
    * satellites, including the geocentric Moon, for which positions and
    * velocities are given in kilometers and km/day, respectively. Angles
    * (nutations and librations) are given in radians, and time differences
    * (such as TT-TDB) in seconds.
    *
    * For performance reasons, this class is not thread-safe. However, as far
    * as a given instance is never used concurrently from two or more threads,
    * multiple instances of this class can be used from different running
    * threads, including instances constructed to calculate ephemerides for the
    * same object. This allows for the implementation of performance-intensive,
    * lock-free multithreaded ephemeris calculation tasks.
    */
   class PCL_CLASS Handle
   {
   public:

      /*!
       * Constructs a new %Handle object.
       *
       * \param parent  Reference to an open EphemerisFile object providing
       *                access to an ephemeris data file in XEPH format.
       *
       * \param object  The identifier or name of the object for which this
       *                instance will compute ephemerides.
       *
       * \param origin  The identifier of the origin of coordinates. If an
       *                empty string is specified (which is the default
       *                parameter value), this object will be created for the
       *                first occurrence of ephemeris data available for
       *                \a object in the \a parent file, irrespective of the
       *                origin. In such case, if \a parent is a standard
       *                fundamental ephemerides file, the origin is the solar
       *                system barycenter (identified as "SSB").
       *
       * For a detailed description of object names and identifiers, see
       * EphemerisFile::IsObjectAvailable().
       *
       * If the specified \a parent ephemeris file is not open, or if no
       * ephemeris data are available for the specified \a object and \a origin
       * in the \a parent ephemeris file, this constructor will throw an Error
       * exception.
       */
      Handle( const EphemerisFile& parent, const IsoString& object, const IsoString& origin = IsoString() )
      {
         if ( !parent.IsOpen() )
            throw Error( "Cannot create a handle to a closed ephemeris file." );
         m_parent = &parent;
         m_parent->m_handleCount.Increment();
         m_index = m_parent->FindObject( object, origin );
      }

      /*!
       * Copy constructor.
       */
      Handle( const Handle& x )
      {
         m_parent = x.m_parent;
         m_index = x.m_index;
         m_node[0] = x.m_node[0];
         m_node[1] = x.m_node[1];
         if ( m_parent != nullptr )
            m_parent->m_handleCount.Increment();
      }

      /*!
       * Move constructor.
       */
      Handle( Handle&& x )
      {
         m_parent = x.m_parent;
         x.m_parent = nullptr;
         m_index = x.m_index;
         m_node[0] = x.m_node[0];
         m_node[1] = x.m_node[1];
      }

      /*!
       * virtual destructor.
       */
      virtual ~Handle()
      {
         if ( m_parent != nullptr )
            m_parent->m_handleCount.Decrement();
      }

      /*!
       * Copy assignment operator. Returns a reference to this object.
       */
      Handle& operator =( const Handle& x )
      {
         if ( m_parent != nullptr )
            m_parent->m_handleCount.Decrement();
         m_parent = x.m_parent;
         m_index = x.m_index;
         m_node[0] = x.m_node[0];
         m_node[1] = x.m_node[1];
         if ( m_parent != nullptr )
            m_parent->m_handleCount.Increment();
         return *this;
      }

      /*!
       * Move assignment operator. Returns a reference to this object.
       */
      Handle& operator =( Handle&& x )
      {
         if ( m_parent != nullptr )
            m_parent->m_handleCount.Decrement();
         m_parent = x.m_parent;
         x.m_parent = nullptr;
         m_index = x.m_index;
         m_node[0] = x.m_node[0];
         m_node[1] = x.m_node[1];
         return *this;
      }

      /*!
       * Computes a state vector for the specified time point \a t.
       *
       * \param[out] p     Reference to a vector where the components of the
       *                   computed state will be stored.
       *
       * \param t          The requested time point in the TDB time scale
       *                   (rigorously, this is Teph as defined by JPL, but is
       *                   equivalent to TDB or TT for all practical purposes).
       *
       * Rectangular coordinates for position are provided in au, except for
       * the geocentric Moon, for which positions are provided in km.
       *
       * Angles are provided in radians.
       *
       * TT-TDB differences are provided in seconds.
       *
       * The reference system is ICRS/J2000.0.
       *
       * If \a t is either an invalid (uninitialized) TimePoint instance, or a
       * time point outside the time span available from the parent ephemeris
       * file, this member function throws an Error exception.
       */
      void ComputeState( Vector& p, TimePoint t )
      {
         Update( t, 0 );
         p = m_node[0].expansion( t - m_node[0].startTime );
      }

      /*!
       * Computes a state vector and its first derivative for the specified
       * time point \a t.
       *
       * \param[out] p     Reference to a vector where the components of the
       *                   computed state will be stored.
       *
       * \param[out] v     Reference to a vector where the components of the
       *                   computed first derivative will be stored.
       *
       * \param t          The requested time point in the TDB time scale
       *                   (rigorously, this is Teph as defined by JPL, but is
       *                   equivalent to TDB or TT for all practical purposes).
       *
       * Rectangular coordinates for position and velocity are provided in au
       * and au/day, respectively, except for the geocentric Moon, for which
       * position and velocity are provided in km and km/day, respectively.
       *
       * Angles and their variations are provided in radians and radians/day.
       *
       * Time differences (such as TT-TDB) and their variations are provided in
       * seconds and seconds/day.
       *
       * The reference system is ICRS/J2000.0.
       *
       * If the parent ephemeris file provides Chebyshev expansions of state
       * vector derivatives for the object being calculated, the components of
       * \a v will be calculated directly from these expansions. Otherwise the
       * components of \a v will be approximated by numerical differentiation
       * of the Chebyshev expansions for state vectors.
       *
       * If \a t is either an invalid (uninitialized) TimePoint instance, or a
       * time point outside the time span available from the parent ephemeris
       * file, this member function throws an Error exception.
       */
      void ComputeState( Vector& p, Vector& v, TimePoint t )
      {
         ComputeState( p, t );

         if ( HasDerivative() )
            Update( t, 1 );
         else if ( m_node[1].current != m_node[0].current )
         {
            m_node[1].current = m_node[0].current;
            m_node[1].startTime = m_node[0].startTime;
            m_node[1].endTime = m_node[0].endTime;
            m_node[1].expansion = m_node[0].expansion.Derivative();
         }
         v = m_node[1].expansion( t - m_node[1].startTime );
      }

      /*!
       * Computes a state vector for the specified time point \a t.
       *
       * \param t          The requested time point in the TDB time scale
       *                   (rigorously, this is Teph as defined by JPL, but is
       *                   equivalent to TDB or TT for all practical purposes).
       *
       * Returns the computed state function value.
       *
       * See ComputeState( Vector&, TimePoint ) for complete information.
       */
      Vector StateVector( TimePoint t )
      {
         Vector p;
         ComputeState( p, t );
         return p;
      }

      /*!
       * Computes a state vector and its first derivative for the specified
       * time point \a t.
       *
       * \param t          The requested time point in the TDB time scale
       *                   (rigorously, this is Teph as defined by JPL, but is
       *                   equivalent to TDB or TT for all practical purposes).
       *
       * Returns a two-component multivector, where the first component vector
       * is the state function's value and the second is its first derivative.
       *
       * See ComputeState( Vector&, Vector&, TimePoint ) for complete
       * information.
       */
      MultiVector StateVectors( TimePoint t )
      {
         Vector p, v;
         ComputeState( p, v, t );
         return MultiVector( p, v );
      }

      /*!
       * Returns a reference to the parent ephemeris file object. The parent
       * object was specified upon construction.
       */
      const EphemerisFile& ParentFile() const
      {
         return *m_parent;
      }

      /*!
       * Returns the unique identifier of the object for which this handle can
       * compute ephemerides.
       */
      const IsoString& ObjectId() const
      {
         return m_index->objectId;
      }

      /*!
       * Returns the unique identifier of the origin of the coordinates
       * computed by this object. For example, if this member function returns
       * "Ea" or "Sn", the computed coordinates are geocentric or heliocentric,
       * respectively. If this function returns "SSB", the origin of
       * coordinates is the solar system barycenter. This happens, for example,
       * with all standard XEPH files providing fundamental ephemerides.
       */
      const IsoString& OriginId() const
      {
         return m_index->originId;
      }

      /*!
       * Returns the name of the object for which this handle can compute
       * ephemerides. The returned value can be an empty string if no
       * information about the object name is available in the parent ephemeris
       * file.
       */
      const String& ObjectName() const
      {
         return m_index->objectName;
      }

      /*!
       * Returns the lower bound of the time span for which this instance can
       * calculate ephemerides using the ephemeris data already available. If
       * a time point outside this span is requested, new file seek and read
       * operations must be performed.
       *
       * \param i    Expansion index: 0 to select function values (such as
       *             position), 1 to select function derivatives (such as
       *             velocity).
       */
      TimePoint StartTime( int i = 0 ) const
      {
         return m_node[Range( i, 0, 1 )].startTime;
      }

      /*!
       * Returns the upper bound of the time span for which this instance can
       * calculate ephemerides using the ephemeris data already available. If
       * a time point outside this span is requested, new file seek and read
       * operations must be performed.
       *
       * \param i    Expansion index: 0 to select function values (such as
       *             position), 1 to select function derivatives (such as
       *             velocity).
       */
      TimePoint EndTime( int i = 0 ) const
      {
         return m_node[Range( i, 0, 1 )].endTime;
      }

      /*!
       * Returns true iff the parent ephemeris file provides Chebyshev
       * expansions of state vector derivatives for the object for which this
       * handle computes ephemerides.
       *
       * When no expansions for derivatives are available, derivatives are
       * approximated by numerical differentiation of the Chebyshev expansions
       * for state vectors.
       */
      bool HasDerivative() const
      {
         return m_index->HasDerivative();
      }

      /*!
       * Returns the absolute magnitude of the object. H is the visual
       * magnitude of the object as seen at 1 au of the Earth, 1 au from the
       * Sun, and with a phase angle of 0 degrees.
       *
       * <b>References</b>
       *
       * E. Bowell et al., <em>Asteroids II</em>, R. P. Binzel et al. (eds.),
       * The University of Arizona Press, Tucson, 1989, pp. 549-554.
       *
       * Urban, Sean E., Kenneth Seidelmann, P., ed. (2013), <em>The
       * Explanatory Supplement to the Astronomical Almanac</em> 3rd Edition,
       * Section 10.4.3.
       */
      const Optional<double>& H() const
      {
         return m_index->H;
      }

      /*!
       * Slope parameter. See H() for references.
       */
      const Optional<double>& G() const
      {
         return m_index->G;
      }

      /*!
       * Color index B-V, in magnitudes.
       */
      const Optional<double>& B_V() const
      {
         return m_index->B_V;
      }

      /*!
       * Diameter of the object in km. When available, this is normally an IRAS
       * diameter for an asteroid.
       */
      const Optional<double>& D() const
      {
         return m_index->D;
      }

   private:

      struct NodeInfo
      {
         int          current = -1;
         TimePoint    startTime;
         TimePoint    endTime;
         ChebyshevFit expansion;
      };

      const EphemerisFile*        m_parent = nullptr;
      const EphemerisFile::Index* m_index = nullptr;
            NodeInfo              m_node[ 2 ];

      /*!
       * \internal
       * Updates internal data for the specified time point \a t and function
       * index \a i. Performs file seek and read operations only when required
       * to load a new set of Chebyshev polynomial coefficients.
       */
      void Update( TimePoint t, int index )
      {
         if ( !t.IsValid() )
            throw Error( "Invalid time point." );
         if ( t < m_parent->StartTime() || t > m_parent->EndTime() )
            throw Error( "Time point out of range." );

         const Array<IndexNode>& nodes = m_index->nodes[index];
         NodeInfo& info = m_node[index];

         for ( int N = int( nodes.Length() ), l = 0, r = N-1; ; )
         {
            int m = (l + r) >> 1;
            const IndexNode& node = nodes[m];
            TimePoint t0 = node.StartTime();
            if ( t < t0 )
               r = m;
            else
            {
               if ( m == N-1 || t < nodes[m+1].StartTime() )
               {
                  if ( m != info.current )
                  {
                     m_parent->m_file.SetPosition( node.position );
                     ChebyshevFit::coefficient_series coefficients;
                     for ( int i = 0, n = node.NumberOfComponents(); i < n; ++i )
                     {
                        Vector c( node.n[i] );
                        m_parent->m_file.Read( reinterpret_cast<void*>( c.Begin() ), c.Size() );
                        coefficients << c;
                     }
                     info.current = m;
                     info.startTime = t0;
                     info.endTime = (m < N-1) ? nodes[m+1].StartTime() : m_parent->EndTime();
                     info.expansion = ChebyshevFit( coefficients, 0, info.endTime - info.startTime );
                  }
                  break;
               }

               l = m + 1;
            }
         }
      }
   };

private:

#if defined( __clang__ )
# pragma GCC diagnostic ignored "-Wunused-private-field"
#endif

   /*
    * For the core JavaScript engine we need garbage-collector-safe
    * asynchronous destruction of these objects.
    */
   mutable bool m_finalized = false;

   /*
    * In the core JavaScript engine we must identity static objects that can be
    * shared and must never be destroyed. This includes fundamental
    * ephemerides, nutation, etc.
    */
   mutable bool m_internal = false;

#if defined( __clang__ )
# pragma GCC diagnostic warning "-Wunused-private-field"
#endif

#ifdef __PCL_BUILDING_PIXINSIGHT_APPLICATION
   friend class pi::JSEphemerisFileObject;
   friend class pi::JSEphemerisHandleObject;
#endif
};

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_EphemerisFile_h

// ----------------------------------------------------------------------------
// EOF pcl/EphemerisFile.h - Released 2018-12-12T09:24:21Z
