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
#ifndef PREFDIALOG_H
#define PREFDIALOG_H

#include <kdialogbase.h>

/**
@author Sergio Cambra
*/

class PrefGeneral;

class PrefDialog : public KDialogBase {
Q_OBJECT
public:
    PrefDialog(QWidget *parent = 0, const char *name = 0, WFlags f = 0);

	void updateDialog();
	void updateConfiguration();
	
public slots:
	void slotDefault();
	void slotApply();
	void enableApply();

signals:
	void settingsChanged();
	
private:
	PrefGeneral *m_prefGeneral;
};

#endif
