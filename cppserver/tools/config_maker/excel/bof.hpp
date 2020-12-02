
/*!
	\file
	\brief Excel BOF record.

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

#ifndef EXCEL__BOF_HPP__INCLUDED
#define EXCEL__BOF_HPP__INCLUDED

// C++ include.
#include <cstdint>

// Excel include.
#include "record.hpp"


namespace Excel {

//
// BOF
//

//! BOF record in the Excel file.
class BOF {
public:
	BOF();

	//! Record's code for the BOF record.
	static const uint16_t RecordCode = 0x0809;

	//! BIFF version.
	enum BiffVersion {
		BIFF8 = 0x0600,
		BIFF7 = 0x0500,
		UnknownVersion = 0x0000
	}; // enum BiffVersion

	//! Types of the Excel's substream.
	enum SubstreamType {
		UnknownType = 0x0000,
		WorkBookGlobals = 0x0005,
		VisualBasicModule = 0x0006,
		WorkSheet = 0x0010,
		Chart = 0x0020,
		MacroSheet = 0x0040,
		WorkSpace = 0x0100
	}; // enum SubstreamType

	//! \return BIFF version.
	BiffVersion version() const;

	//! \return Substream type.
	SubstreamType type() const;

	//! Parse BOF record.
	void parse( Record & record );

private:
	//! BIFF version.
	BiffVersion m_version;
	//! Substream type.
	SubstreamType m_type;
}; // class BOF

inline
BOF::BOF()
	:	m_version( UnknownVersion )
	,	m_type( UnknownType )
{
}

inline BOF::BiffVersion
BOF::version() const
{
	return m_version;
}

inline BOF::SubstreamType
BOF::type() const
{
	return m_type;
}

inline void
BOF::parse( Record & record )
{
	int16_t version = 0;
	int16_t type = 0;

	record.dataStream().read( version, 2 );
	record.dataStream().read( type, 2 );

	switch( version )
	{
		case BIFF8 :
			m_version = BIFF8;
			break;

		case BIFF7 :
			m_version = BIFF7;
			break;

		default :
			m_version = UnknownVersion;
			break;
	}

	switch( type )
	{
		case WorkBookGlobals :
			m_type = WorkBookGlobals;
			break;

		case VisualBasicModule :
			m_type = VisualBasicModule;
			break;

		case WorkSheet :
			m_type = WorkSheet;
			break;

		case Chart :
			m_type = Chart;
			break;

		case MacroSheet :
			m_type = MacroSheet;
			break;

		case WorkSpace :
			m_type = WorkSpace;
			break;

		default :
			m_type = UnknownType;
			break;
	}
}

} /* namespace Excel */

#endif // EXCEL__BOF_HPP__INCLUDED
