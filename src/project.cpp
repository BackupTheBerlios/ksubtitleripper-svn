/***************************************************************************
*   Copyright (C) 2004 by Sergio Cambra García                            *
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
		: files( list ), basename( base ) {
	numSub = currentSub = 0;
	extracted = converted = false;
	if ( list[0].isLocalFile() ) directory = list[0].directory( false );
	else directory = KTempDir().name();
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
	
	files.empty();
	directory = QString::null;
	basename = "";
	numSub = currentSub = 0;
	extracted = converted = false;
	
	while ( !readField( in, field, value ) ) {
		if ( field == "File" ) {
			url = value;
			if ( url.isEmpty() || !url.isValid() ) return false;
			files.append(url);
		} else if ( field == "Directory" ) {
			if ( value.isEmpty() ) return false;
			directory = value;
		} else if ( field == "Basename" ) {
			if ( value.isEmpty() ) return false;
			basename = value;
		} else if ( field == "NumberSubtitles" ) {
			numSub = value.toUInt( &success );
			if ( !success ) return false;
		} else if ( field == "CurrentSubtitle" ) {
			currentSub = value.toUInt( &success );
			if ( !success ) return false;
		} else if ( field == "Extracted" ) {
			extracted = value.toInt( &success );
			if ( !success ) return false;
		} else if ( field == "Converted" ) {
			converted = value.toInt( &success );
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

QString Project::getSubFilename( int sub ) {
	QString number = QString::number( sub );
	
	if ( number.length() < 4 )
		return basename + QString().fill( '0', 4-number.length() ) + number;
	else return basename + number;
}

QString Project::getSubFilename() {
	return getSubFilename( currentSub );
}

void Project::goFirst() {
	currentSub = 1;
}

unsigned int Project::getNumSub() const {
	return numSub;
}

unsigned int Project::getCurrentSub() const {
	return currentSub;
}

bool Project::getExtracted() const {
	return extracted;
}

bool Project::getConverted() const {
	return converted;
}

QString Project::getBaseName() const {
	return basename;
}

const KURL::List& Project::getFiles() const {
	return files;
}

QString Project::getDirectory() const {
	return directory;
}

void Project::nextSub() {
	currentSub++;
}

void Project::prevSub() {
	currentSub--;
}

bool Project::atFirst() const {
	return currentSub == 1;
}

bool Project::atLast() const {
	return currentSub == numSub;
}

bool Project::save( const QString& path ) const {
	QFile f( path );
	QTextStream out( &f );
	if ( !f.open ( IO_WriteOnly ) ) return false;
	
	for ( uint i = 0; i < files.count(); i++ ) {
		out << "File=" << files[i].url() << endl;
	}
	out << "Directory=" << directory << endl;
	out << "Basename=" << basename << endl;
	out << "NumberSubtitles=" << numSub << endl;
	out << "CurrentSubtitle=" << currentSub << endl;
	out << "Extracted=" << extracted << endl;
	out << "Converted=" << converted << endl;
	
	f.close();
	return true;
}

void Project::setExtracted( bool value ) {
	extracted = value;
}

void Project::setConverted( bool value ) {
	converted = value;
}

void Project::setNumSub( uint num ) {
	numSub = num;
	if ( currentSub > numSub ) currentSub = numSub;
}
