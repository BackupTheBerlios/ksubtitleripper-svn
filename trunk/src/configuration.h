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

/**
@author Sergio Cambra
*/
class Configuration{
public:
	void write() const;

	bool doUnix2Dos() const;
	bool checkSpelling() const;

	void setDoUnix2Dos( bool value );
	void setCheckSpelling( bool value );

	static const bool m_defaultDoUnix2Dos;
	static const bool m_defaultCheckSpelling;

private:
	Configuration();
	void read();

	friend Configuration& Config();

	bool m_doUnix2Dos;
	bool m_checkSpelling;
};

Configuration& Config();

#endif
