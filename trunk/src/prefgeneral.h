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
#ifndef PREFGENERAL_H
#define PREFGENERAL_H

#include <prefgenerallayout.h>
#include <qcheckbox.h>
#include <kfontrequester.h>

/**
@author Sergio Cambra
*/
class PrefGeneral : protected PrefGeneralLayout
{
Q_OBJECT
public:
	PrefGeneral( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );

	void setDoUnix2DosChecked( bool value ) { m_doUnix2DosChk->setChecked( value ); }
	void setEnableAutoCenterChecked( bool value ) { m_enableAutoCenter->setChecked( value ); }
	void setEditorFont( const QFont& font ) { m_editorFont->setFont( font ); }

	bool isDoUnix2DosChecked() const { return m_doUnix2DosChk->isChecked(); }
	bool isEnableAutoCenterChecked() const { return m_enableAutoCenter->isChecked(); }
	QFont editorFont() const { return m_editorFont->font(); }

protected slots:
	void optionsChangedSlot();
};

#endif
