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
#include <kdebug.h>

/**
@author Sergio Cambra
*/
namespace {
	class Colours {
	public:
		Colours() { for (uint i=0; i<4; ++i) v_colours[i] = 255; v_colours[2] = 0; };
		~Colours() {};
		uchar& operator[](uint index) {
			if ( index > 3 )
				kdFatal() << "Colours: Index out of bounds\n";
			
			return v_colours[index];
		};
		const uchar& operator[](uint index) const {
			if ( index > 3 )
				kdFatal() << "Colours: Index out of bounds\n";
			
			return v_colours[index];
		};
	private:
		uchar v_colours[4];
	};
}

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

	void nextSub();
	void prevSub();
	bool atFirst() const;
	bool atLast() const;
	
	bool save( const QString& path ) const;
	
	Colours colours;
	
private:
	bool readField( QTextStream& stream, QString& field, QString& value ) const;
	bool load ( QTextStream& stream );
	bool setColours( const QString& col );

	KURL::List v_files;
	QString v_directory, v_baseName;
	uint v_numSub, v_currentSub;
	bool v_extracted, v_converted;
};

#endif
