/***************************************************************************
 *   Copyright (C) 2004 by Sergio Cambra Garc�a                            *
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
#include <qtextcodec.h>

const bool Configuration::defaultDoUnix2Dos = false;
const bool Configuration::defaultCheckSpelling = true;
const bool Configuration::defaultAutoCenter = true;

Configuration::Configuration()
{
	read();
}

const QFont& Configuration::defaultEditorFont()
{
	static QFont *m_defaultEditorFont = new QFont( KGlobalSettings::fixedFont() );
	return *m_defaultEditorFont;
}

const QString& Configuration::defaultSubEncoding()
{
	static QString *m_defaultSubEncoding = new QString( QString( QTextCodec::codecForLocale()->name() ).find( "UTF-8" ) != -1 ? "UTF8" : "ISO8859_1" );
	return *m_defaultSubEncoding;
}

void Configuration::read() {
	KConfig *conf = kapp->config();

	conf->setGroup( "General" );
	m_doUnix2Dos = conf->readBoolEntry( "doDos2Unix", defaultDoUnix2Dos );
	m_checkSpelling = conf->readBoolEntry( "checkSpelling", defaultCheckSpelling );
	m_editorFont = conf->readFontEntry( "fontEditor", &defaultEditorFont() );
	m_autoCenter = conf->readBoolEntry( "autoCenter", defaultAutoCenter );
	m_subEncoding = conf->readEntry( "subEncoding", defaultSubEncoding() );
}

void Configuration::write() const {
	KConfig *conf = kapp->config();

	conf->setGroup( "General" );
	conf->writeEntry( "doDos2Unix", m_doUnix2Dos );
	conf->writeEntry( "checkSpelling", m_checkSpelling );
	conf->writeEntry( "fontEditor", m_editorFont );
	conf->writeEntry( "autoCenter", m_autoCenter );
	conf->writeEntry( "subEncoding", m_subEncoding );

	conf->sync();
}

Configuration& Config() {
	static Configuration conf;
	return conf;
}
