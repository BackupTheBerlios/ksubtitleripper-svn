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
#ifndef WAITINGDIALOG_H
#define WAITINGDIALOG_H

#include <kprogress.h>

class QTimer;

/**
@author Sergio Cambra
 */

class WaitingDialog : private KProgressDialog
{
Q_OBJECT
public:
	WaitingDialog( QWidget *parent = 0, const char *name = 0,
				   const QString &caption = QString::null, const QString &text = QString::null,
				   bool modal = true );
	virtual void show();
	void stop();
	void close() { KProgressDialog::accept(); }

private slots:
	void slotProgress();

private:
	QTimer *m_timer;
};

#endif
