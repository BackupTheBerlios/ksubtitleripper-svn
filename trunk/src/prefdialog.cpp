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

#include "prefdialog.h"
#include "prefgeneral.h"
#include "configuration.h"

#include <klocale.h>
#include <qframe.h>
#include <qlayout.h>

PrefDialog::PrefDialog(QWidget *parent, const char *name, WFlags f)
 : KDialogBase(IconList, i18n("Configure"), Help | Default | Ok | Apply | Cancel, Ok, parent, name, f)
{
	QFrame *frame;
	frame = addPage( i18n( "General" ), i18n( "General options" ) );

	QVBoxLayout *layout = new QVBoxLayout( frame, 0, 0 );
	m_prefGeneral = new PrefGeneral( frame );
	layout->addWidget( m_prefGeneral );

	// connect interactive widgets and selfmade signals to the enableApply slotDefault
	connect( m_prefGeneral, SIGNAL( optionsChanged() ), this, SLOT( enableApply() )  );
}

void PrefDialog::updateDialog() {
	m_prefGeneral->setDoUnix2DosChecked( Config().doUnix2Dos() );
	m_prefGeneral->setEnableAutoCenterChecked( Config().autoCenter() );
	m_prefGeneral->setEditorFont( Config().editorFont() );
	enableButtonApply( false );
}

void PrefDialog::updateConfiguration() {
	Config().setDoUnix2Dos( m_prefGeneral->isDoUnix2DosChecked() );
	Config().setAutoCenter( m_prefGeneral->isEnableAutoCenterChecked() );
	Config().setEditorFont( m_prefGeneral->editorFont() );
	enableButtonApply( false );
}

void PrefDialog::slotDefault() {
	switch (activePageIndex()) {
	case 1: // General
		m_prefGeneral->setDoUnix2DosChecked( Configuration::defaultDoUnix2Dos );
		m_prefGeneral->setEnableAutoCenterChecked( Configuration::defaultAutoCenter );
		m_prefGeneral->setEditorFont( Configuration::defaultEditorFont() );
		break;
	default: return;
	}
	enableApply();
}

void PrefDialog::slotApply() {
	updateConfiguration();
	emit settingsChanged();
}

void PrefDialog::enableApply() {
	enableButtonApply( true );
}

#include "prefdialog.moc"
