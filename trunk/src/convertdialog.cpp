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

#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <qpainter.h>
#include <qregexp.h>
#include <qdir.h>
#include <qsizepolicy.h>
#include "convertdialog.h"

ConvertDialog::ConvertDialog( Project *prj, QWidget *parent, const char* name )
 : KDialogBase( parent, name, true, i18n( "Converting images to text" ), Ok|Cancel|Help|User1,
 		Ok, false, KStdGuiItem::clear() ), project( prj ), sending( false ) {
	// prj mustn't be 0
	if ( !prj ) kdFatal() << "ConvertDialog constructor: prj is null\n";
	
	QFrame *top = makeMainWidget();
	layoutGeneral = new QVBoxLayout( top, marginGeneral, 6 );
	
	layoutSub = new QHBoxLayout( layoutGeneral ); 
    layoutSub->addItem( new QSpacerItem( 40, 20, QSizePolicy::Expanding,
						QSizePolicy::Minimum ) );
    subtitle = new QLabel( top );
    layoutSub->addWidget( subtitle );
    layoutSub->addItem( new QSpacerItem( 40, 20, QSizePolicy::Expanding,
						QSizePolicy::Minimum ) );
	
	image = new QLabel( top );
	image->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ) );
	image->setAlignment( image->alignment() | Qt::AlignHCenter );
	image->setPixmap( QPixmap() );
	layoutGeneral->addWidget( image );
	
    progress = new KProgress( project->numSub(), top );
    layoutGeneral->addWidget( progress );
	
	text = new QLabel( top );
	text->setAlignment( text->alignment() | Qt::WordBreak );
	text->setText( i18n( "The marked chars were not recognized. Enter correct ASCII char, \"string\" or 4 to 8 digit hex unicode." ) );
	layoutGeneral->addWidget( text );
	
	line = new KLineEdit( top );
	layoutGeneral->addWidget( line );
	
	layoutCheckBox = new QHBoxLayout( layoutGeneral );
    layoutCheckBox->addItem( new QSpacerItem( 40, 20, QSizePolicy::Expanding,
						QSizePolicy::Minimum ) );
    checkbox = new QCheckBox( i18n( "Save to database" ), top );
	checkbox->setChecked( true );
    layoutCheckBox->addWidget( checkbox );
    layoutCheckBox->addItem( new QSpacerItem( 40, 20, QSizePolicy::Expanding,
						QSizePolicy::Minimum ) );
	
	setEnabledWidgetsInput( false );
	enableButton( User1, false );
	
	connect( this, SIGNAL( user1Clicked() ), line, SLOT( clear() ) );
	connect( line, SIGNAL( textChanged( const QString& ) ),
			this, SLOT( editChanged( const QString& ) ) );
}

ConvertDialog::~ConvertDialog() {}

void ConvertDialog::setEnabledWidgetsInput( bool enable ) {
	text->setEnabled( enable );
	checkbox->setEnabled( enable );
	enableButtonOK( enable );
}

void ConvertDialog::loadSubtitle( QRect rect ) {
	QString filename = project->directory() + project->subFilename( sub ) + ".pgm";
	
	// set label
	subtitle->setText( i18n( "Subtitle %1" ).arg( sub ) );
	
	// load image
	if ( !image->pixmap()->load( filename ) || image->pixmap()->isNull() ) {
		image->pixmap()->resize( 0, 0 );
		KMessageBox::error( this, i18n( "Couldn't load file %1" ).arg( filename ) );
	}
	QSize inc = image->pixmap()->size() - image->size();
	image->setMinimumSize( image->pixmap()->size() );
	
	// Don't shrink the dialog
	if ( inc.width() < 0 ) inc.setWidth( 0 );
	if ( inc.height() < 0 ) inc.setHeight( 0 );
	
	QSize newSize = size() + inc;
	if ( newSize.width() < sizeHint().width() ) newSize.setWidth( sizeHint().width() );
	resize( newSize );
	
	// mark unknown characters
	QPainter painter( image->pixmap() );
	painter.setPen( Qt::red );
	painter.drawRect( rect );
	image->update();
}

void ConvertDialog::convertSub() {
	process = new KProcess();
	
	process->setUseShell( true );
	process->setWorkingDirectory( project->directory() );
	databasePath = KProcess::quote( project->directory() + "db/" );
	
	connect( process, SIGNAL( processExited( KProcess* ) ),
			this, SLOT( gocrFinish( KProcess* ) ) );
	connect( process, SIGNAL( receivedStderr( KProcess*, char*, int ) ),
			this, SLOT( convertQuestion( KProcess*, char*, int ) ) );
	connect( process, SIGNAL( wroteStdin( KProcess* ) ),
			this, SLOT( sent( KProcess* ) ) );
	connect( this, SIGNAL( cancelClicked() ), this, SLOT( killProcess() ) );
	
	// mkdir ./db/ and create an empty file called db.lst
	QDir( project->directory() ).mkdir( "db" );
	QFile( project->directory() + "db/db.lst" ).open( IO_ReadWrite );
	
	sub = 1;
	progress->setValue( 0 );
	startGocr( process );
}

void ConvertDialog::startGocr( KProcess *proc ) {
	QString filename = project->subFilename( sub );
	proc->clearArguments();
	
	// TODO check if gocr is executable
	
	*proc << "gocr" << "-p" << databasePath;
	*proc << "-s" << "10" << "-m" << "130";
	*proc << filename + ".pgm" << "-o" << filename + ".pgm.txt";
	
	if ( !proc->start( KProcess::NotifyOnExit, KProcess::All ) )
		kdError() << "error executing process\n";
}

void ConvertDialog::gocrFinish( KProcess *proc ) {
	disconnect( this, SIGNAL( cancelClicked() ), this, SLOT( killProcess() ) );
	if ( proc->exitStatus() == 0 ) {
		progress->advance( 1 );
		
		if ( sub == project->numSub() ) {
			delete proc;
			accept();
		} else {
			sub++;
			startGocr( proc );
		}
	} else {
		delete proc;
		reject();
	}
}

void ConvertDialog::convertQuestion( KProcess *proc, char *buffer, int buflen ) {
	QStringList buf = QStringList::split( '\n', QString::fromLatin1( buffer, buflen ) );
	
	for ( uint i = 0; i < buf.count(); i++ ) {
		if ( buf[i].startsWith( "# list pattern" ) ) {
			QRegExp regExp( "\\b\\d+\\b" );
			int pos, x, y, w, h;
			
			// get the unknown cluster's coordinates
			pos = regExp.search( buf[i] );
			x = regExp.cap( 0 ).toUInt();
			pos = regExp.search( buf[i], pos + regExp.matchedLength() );
			y = regExp.cap( 0 ).toUInt();
			pos = regExp.search( buf[i], pos + regExp.matchedLength() );
			w = regExp.cap( 0 ).toUInt();
			pos = regExp.search( buf[i], pos + regExp.matchedLength() );
			h = regExp.cap( 0 ).toUInt();
			
			// load the subtitle and mark the unknown cluster on the image
			loadSubtitle( QRect( x, y, w, h) );
			
		} else if ( buf[i].startsWith( "The upper char" ) ) {
			setEnabledWidgetsInput( true );
			line->setFocus();
			
		} else if ( buf[i].startsWith( " Store the pattern?" ) ) {
			int option;
			
			if ( checkbox->isChecked() ) option = 2;
			else option = 0;
				
			writeStdin( proc, option );
		}
	}
}

void ConvertDialog::sent( KProcess *proc ) {
	toSent.pop_front();
	
	if ( toSent.count() == 0 ) sending = false;
	else proc->writeStdin( toSent.first().latin1(), toSent.first().length() );
}

void ConvertDialog::writeStdin( KProcess *proc, QString data ) {
	QString aux = data + '\n';
	
	toSent.append( aux );
	if ( !sending ) {
		sending = true;
		proc->writeStdin( aux.latin1(), aux.length() );
	}
}

void ConvertDialog::writeStdin( KProcess *proc, int data ) {
	writeStdin( proc, QString::number( data ) );
}

void ConvertDialog::slotOk() {
	writeStdin( process, line->text() );
	
	setEnabledWidgetsInput( false );
	enableButton( User1, false );
	
	if ( image->pixmap() ) {
		image->pixmap()->resize( 0, 0 );
		image->update();
	}
	subtitle->setText( QString::null );
	line->clear();
}

void ConvertDialog::killProcess() {
	process->kill();
}

void ConvertDialog::show() {
	convertSub();
	KDialogBase::show();
}

void ConvertDialog::editChanged( const QString& text ) {
	enableButton( User1, !text.isEmpty() );
}

#include "convertdialog.moc"
