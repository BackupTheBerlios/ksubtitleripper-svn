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

/**
@author Sergio Cambra
*/

class Colours {
public:
	Colours();
	~Colours();
	uchar& operator[](uint index);
	const uchar& operator[](uint index) const;
private:
	uchar m_colours[4];
};

class Project {
public:
	Project( const KURL::List& list, const QString& base );
	Project( const QString& path, bool& success );

	~Project();
	
	QString subFilename( int sub );
	QString subFilename();
	void goFirst();
	unsigned int numSub() const;
	unsigned int currentSub() const;
	bool isExtracted() const;
	bool isConverted() const;
	QString baseName() const;
	const KURL::List& files() const;
	QString directory() const;
	
	void setExtracted( bool value );
	void setConverted( bool value );
	void setNumSub( uint num );
	
	QString coloursString() const;
	bool setColours( const QString& col );

	void nextSub();
	void prevSub();
	bool atFirst() const;
	bool atLast() const;
	
	bool save( const QString& path ) const;
	
	Colours colours;
	
private:
	bool readField( QTextStream& stream, QString& field, QString& value ) const;
	bool load ( QTextStream& stream );

	KURL::List m_files;
	QString m_directory, m_baseName;
	uint m_numSub, m_currentSub;
	bool m_extracted, m_converted;
};

#endif
