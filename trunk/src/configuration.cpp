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

#include "configuration.h"

#include <kapplication.h>
#include <kconfig.h>
#include <kglobalsettings.h>

const bool Configuration::defaultDoUnix2Dos = false;
const bool Configuration::defaultCheckSpelling = true;

Configuration::Configuration()
{
	read();
}

const QFont& Configuration::defaultEditorFont()
{
	static QFont *m_defaultEditorFont = new QFont( KGlobalSettings::fixedFont() );
	return *m_defaultEditorFont;
}

void Configuration::read() {
	KConfig *conf = kapp->config();

	conf->setGroup( "General" );
	m_doUnix2Dos = conf->readBoolEntry( "doDos2Unix", defaultDoUnix2Dos );
	m_checkSpelling = conf->readBoolEntry( "checkSpelling", defaultCheckSpelling );
	m_editorFont = conf->readFontEntry( "fontEditor", &defaultEditorFont() );
}

void Configuration::write() const {
	KConfig *conf = kapp->config();

	conf->setGroup( "General" );
	conf->writeEntry( "doDos2Unix", m_doUnix2Dos );
	conf->writeEntry( "checkSpelling", m_checkSpelling );
	conf->writeEntry( "fontEditor", m_editorFont );

	conf->sync();
}

Configuration& Config() {
	static Configuration conf;
	return conf;
}
