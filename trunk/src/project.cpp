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
#include <ktempdir.h>

Project::Project( const KURL::List& list, const QString& base )
		: v_files( list ), v_baseName( base ) {
	v_numSub = v_currentSub = 0;
	v_extracted = v_converted = false;
	if ( list[0].isLocalFile() ) v_directory = list[0].directory( false );
	else v_directory = KTempDir().name();
}

Project::Project( const QString& path, bool& success ) {
	QFile f( path );
	
	if ( !f.open ( IO_ReadOnly ) ){
		success = false;
		return;
	}
	
	QTextStream in( &f );
	if ( !load( in ) ) {
		f.close();
		success = false;
		return;
	}
	
	success = true;
}

Project::~Project() {}

bool Project::load( QTextStream& in ) {
	QString field, value;
	KURL url;
	bool success;
	
	v_files.empty();
	v_directory = QString::null;
	v_baseName = "";
	v_numSub = v_currentSub = 0;
	v_extracted = v_converted = false;
	
	while ( !readField( in, field, value ) ) {
		if ( field == "File" ) {
			url = value;
			if ( url.isEmpty() || !url.isValid() ) return false;
			v_files.append(url);
		} else if ( field == "Directory" ) {
			if ( value.isEmpty() ) return false;
			v_directory = value;
		} else if ( field == "Basename" ) {
			if ( value.isEmpty() ) return false;
			v_baseName = value;
		} else if ( field == "NumberSubtitles" ) {
			v_numSub = value.toUInt( &success );
			if ( !success ) return false;
		} else if ( field == "CurrentSubtitle" ) {
			v_currentSub = value.toUInt( &success );
			if ( !success ) return false;
		} else if ( field == "Extracted" ) {
			v_extracted = value.toInt( &success );
			if ( !success ) return false;
		} else if ( field == "Converted" ) {
			v_converted = value.toInt( &success );
			if ( !success ) return false;
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
		return v_baseName + QString().fill( '0', 4-number.length() ) + number;
	else return v_baseName + number;
}

QString Project::subFilename() {
	return subFilename( v_currentSub );
}

void Project::goFirst() {
	v_currentSub = 1;
}

unsigned int Project::numSub() const {
	return v_numSub;
}

unsigned int Project::currentSub() const {
	return v_currentSub;
}

bool Project::isExtracted() const {
	return v_extracted;
}

bool Project::isConverted() const {
	return v_converted;
}

QString Project::baseName() const {
	return v_baseName;
}

const KURL::List& Project::files() const {
	return v_files;
}

QString Project::directory() const {
	return v_directory;
}

void Project::nextSub() {
	v_currentSub++;
}

void Project::prevSub() {
	v_currentSub--;
}

bool Project::atFirst() const {
	return v_currentSub == 1;
}

bool Project::atLast() const {
	return v_currentSub == v_numSub;
}

bool Project::save( const QString& path ) const {
	QFile f( path );
	QTextStream out( &f );
	if ( !f.open ( IO_WriteOnly ) ) return false;
	
	for ( uint i = 0; i < v_files.count(); i++ ) {
		out << "File=" << v_files[i].url() << endl;
	}
	out << "Directory=" << v_directory << endl;
	out << "Basename=" << v_baseName << endl;
	out << "NumberSubtitles=" << v_numSub << endl;
	out << "CurrentSubtitle=" << v_currentSub << endl;
	out << "Extracted=" << v_extracted << endl;
	out << "Converted=" << v_converted << endl;
	
	f.close();
	return true;
}

void Project::setExtracted( bool value ) {
	v_extracted = value;
}

void Project::setConverted( bool value ) {
	v_converted = value;
}

void Project::setNumSub( uint num ) {
	v_numSub = num;
	if ( v_currentSub > v_numSub ) v_currentSub = v_numSub;
}
