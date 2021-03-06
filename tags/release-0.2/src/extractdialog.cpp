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
 : KDialogBase( parent, name, true, i18n( "Extracting subtitles" ), 0 ), project( prj ) {
	// prj mustn't be 0
	if ( !prj ) kdFatal() << "ExtractDialog constructor: prj is null\n";
	
	QFrame *top = makeMainWidget();
	QVBoxLayout *layoutGeneral;
	layoutGeneral = new QVBoxLayout( top, 5, 6 );
	
	subtitle = new QLabel( top );
	layoutGeneral->addWidget( subtitle );
	
	QHBoxLayout *layoutButton;
	layoutButton = new QHBoxLayout( layoutGeneral ); 
    layoutButton->addItem( new QSpacerItem( 40, 20, QSizePolicy::Expanding,
						QSizePolicy::Minimum ) );
    cancel = new KPushButton( KStdGuiItem::cancel(), top );
    layoutButton->addWidget( cancel );
    layoutButton->addItem( new QSpacerItem( 40, 20, QSizePolicy::Expanding,
						QSizePolicy::Minimum ) );

	connect( cancel, SIGNAL( clicked() ), this, SLOT( slotCancel() ) );
}

ExtractDialog::~ExtractDialog() {}

void ExtractDialog::keyPressEvent( QKeyEvent *e ) {
	if ( e->key() == Qt::Key_Escape ) cancel->animateClick();
	else KDialogBase::keyPressEvent( e );
}

void ExtractDialog::slotCancel() {
	process->kill();
}

void ExtractDialog::show() {
	extractSub();
	KDialogBase::show();
}

void ExtractDialog::extractSub() {
	process = new ExtractProcess( project, this );
	
	connect( process, SIGNAL( processExited( KProcess* ) ),
			this, SLOT( extractFinish( KProcess* ) ) );
	connect( process, SIGNAL( readReady( KProcIO* ) ),
			this, SLOT( extractOutput( KProcIO* ) ) );
	
	if ( !process->start( KProcess::NotifyOnExit, true ) )
		kdError() << "error executing process\n";
}

void ExtractDialog::extractFinish( KProcess *proc ) {
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
			subtitle->setText( i18n( "Generating image %1" ).arg( line.section( ": ", 1, 1 ) ) );
		else if ( word == "Wrote" )
			project->setNumSub( line.section( ' ', 1, 1 ).toUInt() );
		else kdWarning() << line << endl;
	}
	
	proc->ackRead();
}

#include "extractdialog.moc"
