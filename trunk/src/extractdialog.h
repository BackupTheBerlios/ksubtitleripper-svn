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
#ifndef EXTRACTDIALOG_H
#define EXTRACTDIALOG_H

#include <kdialogbase.h>
#include "project.h"

class KPushButton;
class QLabel;
class QKeyEvent;
class KProcess;
class KProcIO;
class ExtractProcess;

/**
@author Sergio Cambra
*/
class ExtractDialog : public KDialogBase {
Q_OBJECT
public:
    ExtractDialog( Project *prj, QWidget *parent = 0, const char *name = 0 );

    ~ExtractDialog();

public slots:
	virtual void show();

protected:
	virtual void keyPressEvent( QKeyEvent *e );

protected slots:
	virtual void slotCancel();
	virtual void extractFinish( KProcess *proc );
	virtual void extractOutput( KProcIO *proc );

private:
	void extractSub();

	QLabel *m_subtitle;
	KPushButton *m_cancel;

	Project *m_project;
	ExtractProcess *m_process;
};

#endif
