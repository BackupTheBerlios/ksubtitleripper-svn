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

class Project {
public:
	Project( const KURL::List& list, const QString& base );
	Project( const QString& path, bool& success );

	~Project();
	
	QString getSubFilename( int sub );
	QString getSubFilename();
	void goFirst();
	unsigned int getNumSub() const;
	unsigned int getCurrentSub() const;
	bool getExtracted() const;
	bool getConverted() const;
	QString getBaseName() const;
	const KURL::List& getFiles() const;
	QString getDirectory() const;
	
	void setExtracted( bool value );
	void setConverted( bool value );
	void setNumSub( uint num );

	void nextSub();
	void prevSub();
	bool atFirst() const;
	bool atLast() const;
	
	bool save( const QString& path ) const;
	
private:
	bool readField( QTextStream& stream, QString& field, QString& value ) const;
	bool load ( QTextStream& stream );

	KURL::List files;
	QString directory, basename;
	uint numSub, currentSub;
	bool extracted, converted;
};

#endif
