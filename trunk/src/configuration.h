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
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <qfont.h>

/**
@author Sergio Cambra
*/
class Configuration{
public:
	void write() const;

	bool doUnix2Dos() const { return m_doUnix2Dos; }
	bool checkSpelling() const { return m_checkSpelling; }
	QFont editorFont() const { return m_editorFont; }

	void setDoUnix2Dos( bool value ) { m_doUnix2Dos = value; }
	void setCheckSpelling( bool value ) { m_checkSpelling = value; }
	void setEditorFont( const QFont& font ) { m_editorFont = font; }

	static const bool defaultDoUnix2Dos;
	static const bool defaultCheckSpelling;
	static const QFont& defaultEditorFont();

private:
	Configuration();
	void read();

	friend Configuration& Config();

	bool m_doUnix2Dos;
	bool m_checkSpelling;
	QFont m_editorFont;
};

Configuration& Config();

#endif
