
/*!
	\file
	\brief Stream in the compound file.

	\author Igor Mironchik (igor.mironchik at gmail dot com).

	Copyright (c) 2011-2017 Igor Mironchik

	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation
	files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef COMPOUNDFILE__STREAM_HPP__INCLUDED
#define COMPOUNDFILE__STREAM_HPP__INCLUDED

// CompoundFile include.
#include "header.hpp"
#include "sat.hpp"
#include "utils.hpp"

// Excel include.
#include "../stream.hpp"

// C++ include.
#include <iostream>
#include <string>


namespace CompoundFile {

class Stream;


//
// Directory
//

//! Directory in a compound file.
class Directory {
public:
	Directory();

	//! Type of the entry.
	enum Type {
		//! Empty.
		Empty = 0x00,
		//! User storage.
		UserStorage = 0x01,
		//! User stream.
		UserStream = 0x02,
		//! LockBytes.
		LockBytes = 0x03,
		//! Property.
		Property = 0x04,
		//! Root storage.
		RootStorage = 0x05
	}; // enum Type

	//! \return Name of the directory.
	const std::wstring & name() const;

	//! \return Type of the directory.
	Type type() const;

	//! \return SecID of the first sector.
	SecID streamSecID() const;

	//! \return Stream size.
	int32_t streamSize() const;

	//! \return DirID of the right child node.
	int32_t rightChild() const;

	//! \return DirID of the left child node.
	int32_t leftChild() const;

	//! \return DirID of the root node.
	int32_t rootNode() const;

	//! Load directory.
	void load( Stream & stream );

private:
	//! Name of the directory.
	std::wstring m_name;
	//! Type of the directory.
	Type m_type;
	//! SecID of the first sector.
	SecID m_secID;
	//! Stream size.
	int32_t m_streamSize;
	//! DirID of the right child node.
	int32_t m_rightChild;
	//! DirID of the left child node.
	int32_t m_leftChild;
	//! DirID of the root node.
	int32_t m_rootNode;
}; // class Directory


//
// Stream
//

//! Stream in a compound file.
class Stream
	:	public Excel::Stream
{

	friend class File;

protected:
	Stream( const Header & header,
		const SAT & sat,
		const SecID & secID,
		std::istream & stream );

public:
	Stream( const Header & header,
		const SAT & sat,
		const SAT & ssat,
		const Directory & dir,
		const SecID & shortStreamFirstSector,
		std::istream & cstream );

	//! Read one byte from the stream.
	char getByte() override;

	//! \return true if EOF reached.
	bool eof() const override;

	//! Seek stream to new position.
	void seek( int32_t pos, SeekType type = FromBeginning ) override;

	//! \return Position in the stream.
	int32_t pos() override;

private:
	//! Seek internal stream to the next sector.
	void seekToNextSector();
	//! \return Offset in sectors from the beginning of the large
	//! stream sector.
	int32_t whereIsShortSector( const SecID & shortSector,
		SecID & largeSector );

private:
	//! Header.
	const Header & m_header;
	//! Large stream sectors chain.
	std::vector< SecID > m_largeStreamChain;
	//! Short stream sectors chain.
	std::vector< SecID > m_shortStreamChain;
	//! File's stream.
	std::istream & m_stream;

	//! Mode of the stream.
	enum Mode {
		//! Large stream mode.
		LargeStream,
		//! Short stream mode.
		ShortStream
	}; // enum Mode

	//! Mode of the stream.
	Mode m_mode;
	//! Bytes readed from the stream.
	int32_t m_bytesReaded;
	//! Size of the sector for this stream.
	int32_t m_sectorSize;
	//! Bytes readed from the current sector.
	int32_t m_sectorBytesReaded;
	//! Current SecID in the short stream.
	int32_t m_shortSecIDIdx;
	//! Current SecID in the large stream.
	int32_t m_largeSecIDIdx;
	//! Size of the stream.
	int32_t m_streamSize;
}; // class Stream

inline
Stream::Stream( const Header & header,
	const SAT & sat,
	const SecID & secID,
	std::istream & stream )
	:	Excel::Stream( header.byteOrder() )
	,	m_header( header )
	,	m_stream( stream )
	,	m_mode( LargeStream )
	,	m_bytesReaded( 0 )
	,	m_sectorSize( m_header.sectorSize() )
	,	m_sectorBytesReaded( 0 )
	,	m_shortSecIDIdx( 0 )
	,	m_largeSecIDIdx( 0 )
	,	m_streamSize( 0 )
{
	m_largeStreamChain = sat.sectors( secID );

	m_stream.seekg( calcFileOffset( m_largeStreamChain.front(),
		m_sectorSize ) );

	m_streamSize = m_largeStreamChain.size() * m_sectorSize;
}

inline
Stream::Stream( const Header & header,
	const SAT & sat,
	const SAT & ssat,
	const Directory & dir,
	const SecID & shortStreamFirstSector,
	std::istream & cstream )
	:	Excel::Stream( header.byteOrder() )
	,	m_header( header )
	,	m_stream( cstream )
	,	m_mode(
		( static_cast< size_t > ( dir.streamSize() ) < m_header.streamMinSize() ?
			ShortStream : LargeStream ) )
	,	m_bytesReaded( 0 )
	,	m_sectorSize(
		( static_cast< size_t > ( dir.streamSize() ) < m_header.streamMinSize() ?
			m_header.shortSectorSize() : m_header.sectorSize() ) )
	,	m_sectorBytesReaded( 0 )
	,	m_shortSecIDIdx( 0 )
	,	m_largeSecIDIdx( 0 )
	,	m_streamSize( dir.streamSize() )
{
	if( m_mode == LargeStream )
	{
		m_largeStreamChain = sat.sectors( dir.streamSecID() );
		m_stream.seekg( calcFileOffset( m_largeStreamChain.front(),
			m_header.sectorSize() ) );
	}
	else
	{
		m_largeStreamChain = sat.sectors( shortStreamFirstSector );
		m_shortStreamChain = ssat.sectors( dir.streamSecID() );

		SecID largeSector;
		const int32_t offset = whereIsShortSector( dir.streamSecID(),
			largeSector );

		std::streampos pos = calcFileOffset( largeSector,
			m_header.sectorSize() );
		pos += offset * m_header.shortSectorSize();

		m_stream.seekg( pos );
	}
}

inline bool
Stream::eof() const
{
	return ( m_bytesReaded == m_streamSize );
}

inline void
Stream::seek( int32_t pos, SeekType type )
{
	if( type == FromCurrent )
	{
		pos += m_bytesReaded;

		if( pos < 0 )
			pos += m_streamSize;
	}
	else if( type == FromEnd && pos > 0 )
		pos = m_streamSize - pos;
	else if( type == FromEnd && pos < 0 )
		pos = std::abs( pos );
	else if( type == FromBeginning && pos < 0 )
		pos = m_streamSize - pos;

	if( pos >= m_streamSize )
	{
		m_bytesReaded = m_streamSize;
		return;
	}

	const int32_t offset = pos % m_sectorSize;
	const int32_t sectorIdx = pos / m_sectorSize;

	m_bytesReaded = pos;
	m_sectorBytesReaded = offset;

	if( m_mode == LargeStream )
	{
		m_largeSecIDIdx = sectorIdx;

		m_stream.seekg( calcFileOffset( m_largeStreamChain.at( m_largeSecIDIdx ),
			m_sectorSize ) + offset, std::ios::beg );
	}
	else
	{
		const SecID shortSector = m_shortStreamChain.at( sectorIdx );

		SecID largeSector;
		const int32_t offsetInLargeSector = whereIsShortSector( shortSector,
			largeSector );

		m_shortSecIDIdx = sectorIdx;

		std::streampos pos = calcFileOffset( largeSector,
			m_header.sectorSize() );
		pos += offsetInLargeSector * m_header.shortSectorSize();
		pos += offset;

		m_stream.seekg( pos, std::ios::beg );
	}
}

inline int32_t
Stream::pos()
{
	if( eof() )
		return -1;
	else
		return m_bytesReaded;
}

inline int32_t
Stream::whereIsShortSector( const SecID & shortSector,
	SecID & largeSector )
{
	const int32_t shortSectorsInLarge =
		m_header.sectorSize() / m_header.shortSectorSize();

	const int32_t idxOfTheShortSector =
		SAT::indexOfTheSecID( shortSector, m_shortStreamChain );

	const int32_t offset = idxOfTheShortSector % shortSectorsInLarge;

	const int32_t largeSectorIdx = idxOfTheShortSector / shortSectorsInLarge;

	largeSector = m_largeStreamChain.at( largeSectorIdx );

	return offset;
}

inline void
Stream::seekToNextSector()
{
	if( m_mode == LargeStream )
	{
		++m_largeSecIDIdx;

		m_stream.seekg( calcFileOffset( m_largeStreamChain.at( m_largeSecIDIdx ),
			m_sectorSize ), std::ios::beg );
	}
	else
	{
		++m_shortSecIDIdx;

		SecID largeSector;
		const int32_t offset =
			whereIsShortSector( m_shortStreamChain.at( m_shortSecIDIdx ),
				largeSector );

		std::streampos pos = calcFileOffset( largeSector,
			m_header.sectorSize() );
		pos += offset * m_header.shortSectorSize();

		m_stream.seekg( pos, std::ios::beg );
	}
}

inline char
Stream::getByte()
{
	if( eof() )
		return 0xFFu;

	if( m_sectorBytesReaded == m_sectorSize )
	{
		m_sectorBytesReaded = 0;

		seekToNextSector();
	}

	char ch;
	m_stream.get( ch );

	++m_sectorBytesReaded;
	++m_bytesReaded;

	return ch;
}


//
// Directory
//

inline
Directory::Directory()
	:	m_type( Empty )
	,	m_secID( SecID::EndOfChain )
	,	m_streamSize( 0 )
	,	m_rightChild( -1 )
	,	m_leftChild( -1 )
	,	m_rootNode( -1 )
{
}

inline const std::wstring &
Directory::name() const
{
	return m_name;
}

inline Directory::Type
Directory::type() const
{
	return m_type;
}

inline SecID
Directory::streamSecID() const
{
	return m_secID;
}

inline int32_t
Directory::streamSize() const
{
	return m_streamSize;
}

inline int32_t
Directory::rightChild() const
{
	return m_rightChild;
}

inline int32_t
Directory::leftChild() const
{
	return m_leftChild;
}

inline int32_t
Directory::rootNode() const
{
	return m_rootNode;
}

inline void
Directory::load( Stream & stream )
{
	{
		const int16_t maxSymbolsInName = 32;

		int16_t i = 1;

		std::vector< uint16_t > data;
		data.reserve( maxSymbolsInName - 1 );

		for( ; i <= maxSymbolsInName; ++i )
		{
			uint16_t symbol = 0;
			stream.read( symbol, 2 );

			if( symbol != 0 )
				data.push_back( symbol );
			else
				break;
		}

		m_name.assign( data.begin(), data.end() );

		if( i < maxSymbolsInName )
			stream.seek( ( maxSymbolsInName - i ) * 2, Stream::FromCurrent );
	}

	stream.seek( 2, Stream::FromCurrent );
	m_type = (Type)(int32_t) stream.getByte();
	stream.seek( 1, Stream::FromCurrent );
	stream.read( m_leftChild, 4 );
	stream.read( m_rightChild, 4 );
	stream.read( m_rootNode, 4 );
	stream.seek( 36, Stream::FromCurrent );

	{
		int32_t sec = 0;
		stream.read( sec, 4 );
		m_secID = sec;
	}

	stream.read( m_streamSize, 4 );
	stream.seek( 4, Stream::FromCurrent );
}

} /* namespace CompoundFile */

#endif // COMPOUNDFILE__STREAM_HPP__INCLUDED
