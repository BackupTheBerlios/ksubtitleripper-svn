/***************************************************************************
*   Copyright (C) 2004 by Sergio Cambra                                   *
*   runico@users.berlios.de                                               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

#include "project.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <ktempdir.h>
#include <kdebug.h>
#include "xmlwriter.h"
#include <qvariant.h>

Colours::Colours() {
	m_colours[0] = 255;
	m_colours[1] = 255;
	m_colours[2] = 0;
	m_colours[3] = 255;
}

Colours::~Colours() {}

uchar& Colours::operator[](uint index) {
	if ( index > 3 )
		kdFatal() << "Colours: Index out of bounds\n";

	return m_colours[index];
}
const uchar& Colours::operator[](uint index) const {
	if ( index > 3 )
		kdFatal() << "Colours: Index out of bounds\n";

	return m_colours[index];
}

Project::Project( const KURL::List& list, const QString& base )
		: m_files( list ), m_baseName( base ) {
	m_numSub = m_currentSub = 0;
	m_extracted = m_converted = false;
	if ( list[0].isLocalFile() ) m_directory = list[0].directory( false );
	else m_directory = KTempDir().name();
}

Project::Project( const QString& path, bool& success ) {
	QFile f( path );

	if ( !f.open ( IO_ReadOnly ) ){
		success = false;
		return;
	}

	QTextStream in( &f );
	success = load( in );
	f.close();
}

Project::~Project() {}

bool Project::load( QTextStream& in ) {
	QString field, value;
	KURL url;
	bool success;

	m_files.empty();
	m_directory = QString::null;
	m_baseName = "";
	m_numSub = m_currentSub = 0;
	m_extracted = m_converted = false;

	while ( !readField( in, field, value ) ) {
		if ( field == "File" ) {
			url = value;
			if ( url.isEmpty() || !url.isValid() ) return false;
			m_files.append(url);
		} else if ( field == "Directory" ) {
			if ( value.isEmpty() ) return false;
			m_directory = value;
		} else if ( field == "Basename" ) {
			if ( value.isEmpty() ) return false;
			m_baseName = value;
		} else if ( field == "NumberSubtitles" ) {
			m_numSub = value.toUInt( &success );
			if ( !success ) return false;
		} else if ( field == "CurrentSubtitle" ) {
			m_currentSub = value.toUInt( &success );
			if ( !success ) return false;
		} else if ( field == "Extracted" ) {
			m_extracted = value.toInt( &success );
			if ( !success ) return false;
		} else if ( field == "Converted" ) {
			m_converted = value.toInt( &success );
			if ( !success ) return false;
		} else if ( field == "Colours" ) {
			if ( !setColours( value ) ) return false;
		}
	}

	return true;
}

bool Project::readField( QTextStream& stream, QString& field, QString& value ) const {
	// return true on end of file
	QString string;

	string = stream.readLine();
	if ( string.isNull() ) return true; // EOF

	string = string.stripWhiteSpace();
	field = string.section( '=', 0, 0 );
	value = string.section( '=', 1 );
	return false;
}

QString Project::subFilename( int sub ) {
	QString number = QString::number( sub );

	if ( number.length() < 4 )
		return m_baseName + QString().fill( '0', 4-number.length() ) + number;
	else return m_baseName + number;
}

QString Project::subFilename() {
	return subFilename( m_currentSub );
}

void Project::goFirst() {
	m_currentSub = 1;
}

unsigned int Project::numSub() const {
	return m_numSub;
}

unsigned int Project::currentSub() const {
	return m_currentSub;
}

bool Project::isExtracted() const {
	return m_extracted;
}

bool Project::isConverted() const {
	return m_converted;
}

QString Project::baseName() const {
	return m_baseName;
}

const KURL::List& Project::files() const {
	return m_files;
}

QString Project::directory() const {
	return m_directory;
}

void Project::nextSub() {
	m_currentSub++;
}

void Project::prevSub() {
	m_currentSub--;
}

bool Project::atFirst() const {
	return m_currentSub == 1;
}

bool Project::atLast() const {
	return m_currentSub == m_numSub;
}

bool Project::save( const QString& path ) const {
	QFile f( path );
	QTextStream out( &f );
	if ( !f.open ( IO_WriteOnly ) ) return false;
	XmlWriter xml( &f );
	xml.setAutoNewLine( true );
	xml.writeOpenTag( "ksubtitleripper_project" );

	xml.writeOpenTag( "files" );
	for ( uint i = 0; i < m_files.count(); i++ )
		xml.writeTaggedString( "url", m_files[i].url() );
	xml.writeCloseTag( "files" );

	xml.writeTaggedString( "directory", m_directory );
	xml.writeTaggedString( "basename", m_baseName );
	xml.writeTaggedString( "numbersubtitles", m_numSub );
	xml.writeTaggedString( "currentsubtitle", m_currentSub );
	xml.writeTaggedString( "extracted", m_extracted );
	xml.writeTaggedString( "converted", m_converted );

	AttrMap attrs;
	for ( uint i = 0; i < m_files.count(); i++ )
		attrs.insert( "colour"+(i+1), QString::number( colours[i] ) );
	xml.writeAtomTag( "colours", attrs );

	xml.writeCloseTag( "ksubtitleripper_project" );
	f.close();
	return true;
}

void Project::setExtracted( bool value ) {
	m_extracted = value;
}

void Project::setConverted( bool value ) {
	m_converted = value;
}

void Project::setNumSub( uint num ) {
	m_numSub = num;
	if ( m_currentSub > m_numSub ) m_currentSub = m_numSub;
}

bool Project::setColours( const QString& col ) {
	QRegExp re( "(\\d{1,3}),(\\d{1,3}),(\\d{1,3}),(\\d{1,3})" );
	if ( re.exactMatch( col ) ) {
		uint v[4];
		bool success;

		for (uint i = 0; i < 4; ++i) {
			v[i] = re.cap( i+1 ).toUInt( &success );
			if ( !success || v[i] > 255 ) return false;
		}

		// String is valid
		for (uint i = 0; i < 4; ++i)
			colours[i] = uchar( v[i] );
		return true;
	} else return false;
}

QString Project::coloursString() const {
	return QString::number(colours[0]) + "," + QString::number(colours[1]) + "," +
			QString::number(colours[2]) + "," + QString::number(colours[3]);
}
