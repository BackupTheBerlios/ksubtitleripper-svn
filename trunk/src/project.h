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
#ifndef PROJECT_H
#define PROJECT_H

#include <kurl.h>
#include "languagemap.h"

class QStringList;

/**
@author Sergio Cambra
*/

class Colours {
public:
	Colours();
	~Colours() {}
	uchar& operator[](uint index);
	const uchar& operator[](uint index) const;
private:
	uchar m_colours[4];
};

class Project {
public:
	Project();
	Project( const QString& path, bool& success );
	~Project() {}

	bool save( const QString& path ) const;

	QString subFilename( int sub );
	QString subFilename() { return subFilename( m_currentSub ); }
	bool isExtracted() const { return m_extracted; }
	bool isConverted() const { return m_converted; }
	QString baseName() const { return m_baseName; }
	QString directory() const { return m_directory; }
	const KURL::List& files() const { return m_files; }
	QString codeLangSelected() const { return m_code; }
	QString langSelected() const { return m_lang; }
	QStringList languages() const;

	void setLanguageMap( LanguageMap* map ) { m_langMap = map; }
	void setBaseName( const QString& base ) { m_baseName = base; }
	void setDirectory( const QString& dir ) { m_directory = dir; }
	void setExtracted( bool value ) { m_extracted = value; }
	void setConverted( bool value ) { m_converted = value; }
	void setNumSub( uint num ) {
		m_numSub = num;
		if ( m_currentSub > m_numSub ) m_currentSub = m_numSub;
	}

	bool setLanguage( const QString& language );
	void setFiles( const KURL::List& list );
	// set directory if it's empty

	QString error() const { return m_error; }
	QString coloursString() const;

	void nextSub() { m_currentSub++; }
	void prevSub() { m_currentSub--; }
	void goSub( uint i ) { m_currentSub = i; }
	void goFirst() { m_currentSub = 1; }
	bool atFirst() const { return m_currentSub == 1; }
	bool atLast() const { return m_currentSub == m_numSub; }
	uint numSub() const { return m_numSub; }
	uint currentSub() const { return m_currentSub; }

	Colours colours;

private:
	void init() {
		m_numSub = m_currentSub = 0;
		m_extracted = m_converted = false;
		m_lang = "1"; m_code = "0x20";
	}

	KURL::List m_files;
	QString m_directory, m_baseName, m_lang, m_code;
	uint m_numSub, m_currentSub;
	bool m_extracted, m_converted;
	LanguageMap* m_langMap;
	QString m_error;
};

#endif
