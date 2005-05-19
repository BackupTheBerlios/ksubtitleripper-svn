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

#include <klineedit.h>
#include <kprogress.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <kprocess.h>
#include <kdebug.h>
#include <klocale.h>
#include <qregexp.h>
#include <qdir.h>
#include <qsizepolicy.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qvalidator.h>

#include "configuration.h"
#include "subtitleview.h"
#include "project.h"
#include "convertdialog.h"

class CorrectValidator : public QValidator
{
	private:
		QRadioButton *m_butString;
		QRegExpValidator *m_hexCode;
	public:
		CorrectValidator( QRadioButton* butString, QObject* parent, const char* name = 0 ) : QValidator( parent, name ), m_butString( butString )
		{
			m_hexCode = new QRegExpValidator( QRegExp( "[0-9a-fA-F]{4}([0-9a-fA-F]{2}([0-9a-fA-F]{2})?)?" ), this );
		}
		virtual State validate( QString& input, int& pos ) const
		{
			if ( m_butString->isChecked() )
				return Acceptable;
			else return m_hexCode->validate( input, pos );
		}
};

ConvertDialog::ConvertDialog( Project *prj, QWidget *parent, const char* name )
 : KDialogBase( parent, name, true, i18n( "Converting images to text" ), Ok|Cancel|Help|User1,
 		Ok, false, KStdGuiItem::clear() ), project( prj ), sending( false ) {
	// prj mustn't be 0
	if ( !prj ) kdFatal() << "ConvertDialog constructor: prj is null\n";

	QFrame *top = makeMainWidget();
	QVBoxLayout *layoutGeneral = new QVBoxLayout( top, marginGeneral, 6 );

	QHBoxLayout *layoutSub = new QHBoxLayout( layoutGeneral );
    layoutSub->addItem( new QSpacerItem( 40, 20, QSizePolicy::Expanding,
						QSizePolicy::Minimum ) );
	m_subtitle = new QLabel( top );
	layoutSub->addWidget( m_subtitle );
    layoutSub->addItem( new QSpacerItem( 40, 20, QSizePolicy::Expanding,
						QSizePolicy::Minimum ) );

	m_image = new SubtitleView( top );
	m_image->setAutoCenterEnabled( Config().autoCenter() );
	layoutGeneral->addWidget( m_image );

	m_progress = new KProgress( project->numSub(), top );
	layoutGeneral->addWidget( m_progress );

	QLabel *text = new QLabel( top );
	text->setAlignment( text->alignment() | Qt::WordBreak );
	text->setText( i18n( "The marked chars were not recognized. Enter correct ASCII char or 4 to 8 digit hex unicode." ) );
	layoutGeneral->addWidget( text );

	m_correctString = new QButtonGroup( 0, Qt::Vertical, "Correct String", top );
	QGridLayout *correctLayout = new QGridLayout( m_correctString->layout(), 3, 2, 6 );

	m_butString = new QRadioButton( "String", m_correctString );
	m_butString->setChecked( true );
	correctLayout->addWidget( m_butString, 0, 0 );

	QRadioButton *butHexCode = new QRadioButton( "Hex code", m_correctString );
	correctLayout->addWidget( butHexCode, 0, 1 );
	
	m_line = new KLineEdit( m_correctString );
	m_line->setValidator( new CorrectValidator( m_butString, m_line ) );
	correctLayout->addMultiCellWidget( m_line, 1, 1, 0, 1 );

	QHBoxLayout *layoutCheckBox = new QHBoxLayout();
    layoutCheckBox->addItem( new QSpacerItem( 40, 20, QSizePolicy::Expanding,
						QSizePolicy::Minimum ) );
	m_checkbox = new QCheckBox( i18n( "Save to database" ), m_correctString );
	m_checkbox->setChecked( true );
	layoutCheckBox->addWidget( m_checkbox );
    layoutCheckBox->addItem( new QSpacerItem( 40, 20, QSizePolicy::Expanding,
						QSizePolicy::Minimum ) );
	correctLayout->addMultiCellLayout( layoutCheckBox, 2, 2, 0, 1 );

	layoutGeneral->addWidget( m_correctString );

	m_correctString->setEnabled( false );
	enableButton( User1, false );
	enableButtonOK( false );

	connect( this, SIGNAL( user1Clicked() ), m_line, SLOT( clear() ) );
	connect( m_line, SIGNAL( textChanged( const QString& ) ),
			this, SLOT( editChanged( const QString& ) ) );
	connect( m_correctString, SIGNAL(clicked(int )), this, SLOT( radioButtonClicked(int) ) );
}

ConvertDialog::~ConvertDialog() {}

void ConvertDialog::loadSubtitle( QRect rect ) {
	QString filename = project->directory() + project->subFilename( sub ) + ".pgm";

	// set label
	m_subtitle->setText( i18n( "Subtitle %1" ).arg( sub ) );

	// load image
	m_image->load( filename, rect );
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
	m_progress->setValue( 0 );
	startGocr( process );
}

void ConvertDialog::startGocr( KProcess *proc ) {
	QString filename = project->subFilename( sub );
	proc->clearArguments();

	// TODO check if gocr is executable

	*proc << "gocr" << "-p" << databasePath;
	*proc << "-s" << "10" << "-m" << "130";
	*proc << "-f" << Config().subEncoding();
	*proc << filename + ".pgm" << "-o" << filename + ".pgm.txt";

	if ( !proc->start( KProcess::NotifyOnExit, KProcess::All ) )
		kdError() << "error executing process\n";
}

void ConvertDialog::gocrFinish( KProcess *proc ) {
	disconnect( this, SIGNAL( cancelClicked() ), this, SLOT( killProcess() ) );
	if ( proc->exitStatus() == 0 ) {
		m_progress->advance( 1 );

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
	kdDebug() << buffer << endl;

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
			m_correctString->setEnabled( true );
			editChanged( m_line->text() );
			m_line->setFocus();

		} else if ( buf[i].startsWith( " Store the pattern?" ) ) {
			int option;

			if ( m_checkbox->isChecked() ) option = 2;
			else option = 0;

			writeStdin( proc, option );
		}
	}
}

void ConvertDialog::sent( KProcess *proc ) {
	delete[] toSent.first();
	toSent.pop_front();

	if ( toSent.count() == 0 ) sending = false;
	else proc->writeStdin( toSent.first(), strlen( toSent.first() ) );
}

void ConvertDialog::writeStdin( KProcess *proc, QString data ) {
	QString aux = data + '\n';

	toSent.append( qstrdup( aux.local8Bit().data() ) );
	kdDebug() << toSent.last() << endl;
	if ( !sending ) {
		sending = true;
		proc->writeStdin( toSent.first(), strlen( toSent.first() ) );
	}
}

void ConvertDialog::writeStdin( KProcess *proc, int data ) {
	writeStdin( proc, QString::number( data ) );
}

void ConvertDialog::slotOk() {
	// if corrected string isn't empty and isn't hex code, send it quoted
	QString text = m_line->text();
	if ( !text.isEmpty() && m_butString->isChecked() ) text = '"' + text + '"';
	writeStdin( process, text );

	m_correctString->setEnabled( false );
	enableButton( User1, false );

	m_image->clearSubtitle();
	m_subtitle->setText( QString::null );
	m_line->clear();
}

void ConvertDialog::killProcess() {
	process->kill();
}

void ConvertDialog::show() {
	convertSub();
	KDialogBase::show();
}

void ConvertDialog::radioButtonClicked( int )
{
	int cursor = 0;
	QString text = m_line->text();
	if ( m_line->validator()->validate( text, cursor ) == QValidator::Invalid )
		m_line->clear();
	editChanged( m_line->text() );
}

void ConvertDialog::editChanged( const QString& text ) {
	enableButton( User1, !text.isEmpty() );
	enableButtonOK( m_line->hasAcceptableInput() );
}

#include "convertdialog.moc"
