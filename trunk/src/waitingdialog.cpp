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
#include <kdebug.h>
#include <qtimer.h>

#include "waitingdialog.h"

WaitingDialog::WaitingDialog( QWidget *parent, const char *name, const QString &caption,
	const QString &text, bool modal)
	: KProgressDialog( parent, name, caption, text, modal ), m_timer( 0 )
{
	progressBar()->setTotalSteps( 0 );
	progressBar()->setPercentageVisible( 0 );
}

void WaitingDialog::stop()
{
	if ( m_timer ) {
		disconnect( m_timer, SIGNAL( timeout() ), this, SLOT( slotProgress() ) );
		m_timer->stop();
		delete m_timer;
		m_timer = 0;
	}
	accept();
}

void WaitingDialog::show()
{
	if ( m_timer ) return;
	m_timer = new QTimer( this );
	connect( m_timer, SIGNAL( timeout() ), this, SLOT( slotProgress() ) );

	KProgressDialog::show();
	if ( !m_timer->start( 150, false ) )
		kdError() << "Error starting timer\n";
}

void WaitingDialog::slotProgress()
{
	progressBar()->setProgress( progressBar()->progress() + 4 );
}
