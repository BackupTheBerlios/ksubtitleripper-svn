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

#include <kpushbutton.h>
#include <qlabel.h>
#include <qevent.h>
#include <kdebug.h>
#include <kprocio.h>
#include <klocale.h>
#include <qlayout.h>
#include <kmessagebox.h>
#include "extractdialog.h"
#include "extractprocess.h"

ExtractDialog::ExtractDialog( Project *prj, QWidget *parent, const char *name )
	: WaitingDialog( parent, name, i18n( "Extracting subtitles" ) ), m_project( prj ), m_extracted( false ) {
	// prj mustn't be 0
	if ( !prj ) kdFatal() << "ExtractDialog constructor: prj is null\n";

	setAllowCancel( true );
}

ExtractDialog::~ExtractDialog() {}

void ExtractDialog::keyPressEvent( QKeyEvent *e ) {
	if ( e->key() == Qt::Key_Escape ) slotCancel();
	else KDialogBase::keyPressEvent( e );
}

void ExtractDialog::slotCancel() {
	m_process->kill();
}

void ExtractDialog::show() {
	if ( m_extracted ) return; // fix a strange bug, sometimes is executed twice
	extractSub();
	WaitingDialog::show();
}

void ExtractDialog::extractSub() {
	m_extracted = true;
	bool success;
	m_process = new ExtractProcess( m_project, success );
	if ( !success ) reject();

	connect( m_process, SIGNAL( processExited( KProcess* ) ),
			this, SLOT( extractFinish( KProcess* ) ) );
	connect( m_process, SIGNAL( readReady( KProcIO* ) ),
			this, SLOT( extractOutput( KProcIO* ) ) );

	m_project->setNumSub( 0 );
	if ( !m_process->start( KProcess::NotifyOnExit, true ) )
		kdError() << "error executing process\n";
}

void ExtractDialog::extractFinish( KProcess *proc ) {
	stop();
	bool signalled = proc->signalled();
	bool goodExit = proc->exitStatus() == 0;
	delete proc;

	if ( signalled ) reject();
	else if ( goodExit ) accept();
	else {
		KMessageBox::error( this, i18n( "Error extracting subtitles" ) );
		reject();
	}
}

void ExtractDialog::extractOutput( KProcIO *proc ) {
	QString line, word;

	while ( proc->readln( line, false ) != -1 ) {
		word = line.section( ' ', 0, 0 );
		if ( word == "Generating" )
			setLabel( i18n( "Generating image %1" ).arg( line.section( ": ", 1, 1 ) ) );
		else if ( word == "Wrote" )
			m_project->setNumSub( line.section( ' ', 1, 1 ).toUInt() );
		else if ( line != "Conversion finished" ) kdWarning() << line << endl;
	}

	proc->ackRead();
}

#include "extractdialog.moc"
