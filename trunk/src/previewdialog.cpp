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

#include <qvbuttongroup.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qlistbox.h>
#include <qpixmap.h>
#include <kmessagebox.h>
#include <kcombobox.h>
#include <qlabel.h>
#include <kapplication.h>
#include <kdebug.h>
#include <klocale.h>
#include "previewdialog.h"
#include "extractprocess.h"
#include "seeklanguagesinvob.h"
#include "waitingdialog.h"
#include "project.h"

const int numSubs = 8;

PreviewDialog::PreviewDialog( Project *prj, QWidget *parent, const char* name )
 : KDialogBase( parent, name, true, i18n( "Converting images to text" ), Ok|Cancel|Help|User1,
 		Ok, false, KGuiItem( i18n( "Preview" ), "thumbnail" ) ), m_project( prj )
{
	// prj mustn't be 0
	if ( !prj ) kdFatal() << "PreviewDialog constructor: prj is null\n";

	QFrame *top = makeMainWidget();

	QVBoxLayout* layoutGeneral = new QVBoxLayout( top, 5, 6 );

	QLabel* languageLabel = new QLabel( i18n( "Languages" ), top );
	languageLabel->setAlignment( Qt::AlignHCenter );
	layoutGeneral->addWidget( languageLabel );

	languageList = new KComboBox( top );
	layoutGeneral->addWidget( languageList );
	fillLanguages();

	QHBoxLayout* layoutIndex = new QHBoxLayout( layoutGeneral );
    layoutIndex->addItem( new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );

	groupIndex = new QVButtonGroup( i18n( "Choose filling colour index" ), top );

	radioButton[0] = new QRadioButton( groupIndex );
	radioButton[0]->setText( i18n( "Colour %1" ).arg(1) );
	groupIndex->insert( radioButton[0] );

	radioButton[1] = new QRadioButton( groupIndex );
	radioButton[1]->setText( i18n( "Colour %1" ).arg(2) );
	groupIndex->insert( radioButton[1] );

	radioButton[2] = new QRadioButton( groupIndex );
	radioButton[2]->setText( i18n( "Colour %1" ).arg(3) );
	groupIndex->insert( radioButton[2] );

	radioButton[3] = new QRadioButton( groupIndex );
	radioButton[3]->setText( i18n( "Colour %1" ).arg(4) );
	groupIndex->insert( radioButton[3] );

	layoutIndex->addWidget( groupIndex );
	layoutIndex->addItem( new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );

	subtitleList = new QListBox( top );
	subtitleList->setSelectionMode( QListBox::NoSelection );
	subtitleList->setMinimumHeight( 200 );
	layoutGeneral->addWidget( subtitleList );

	for (uint i = 0; i < 4; ++i)
		radioButton[i]->setChecked( prj->colours[i] == 0 );

	connect( this, SIGNAL( user1Clicked() ), this, SLOT( preview() ) );
	connect( this, SIGNAL( okClicked() ), this, SLOT( setColours() ) );
}

PreviewDialog::~PreviewDialog()
{
}

void PreviewDialog::preview()
{
	m_numSub = 0;
	setColours();

	m_process = new ExtractProcess( m_project, this );
	*m_process << "-e" << QString( "00:00:00,%1" ).arg( numSubs );

	connect( m_process, SIGNAL( processExited( KProcess* ) ),
			this, SLOT( extractFinish( KProcess* ) ) );
	connect( m_process, SIGNAL( readReady( KProcIO* ) ),
			this, SLOT( extractOutput( KProcIO* ) ) );

	progress = new KProgressDialog( this, 0, i18n("Extracting..."), QString::null, true );
	progress->setAllowCancel( false );
	progress->setAutoClose( true );
	progress->setMinimumDuration( 1000 );
	progress->progressBar()->setTotalSteps( numSubs );

	if ( !m_process->start( KProcess::NotifyOnExit, true ) )
		kdError() << "error executing process\n";

	progress->exec();
	delete progress;
	progress = 0;
}

void PreviewDialog::extractFinish( KProcess *proc ) {
	bool goodExit = proc->exitStatus() == 0;
	delete proc;

	if ( goodExit ) {
		subtitleList->clear();
		QPixmap blank(1, 20);
		blank.fill();

		for (uint i = 1; i <= m_numSub; ++i) {
			QString filename = m_project->directory() + m_project->subFilename( i ) + ".pgm";
			QPixmap image( filename );

			if ( image.isNull() )
				KMessageBox::error( this, i18n( "Couldn't load file %1" ).arg( filename ) );
			else {
				subtitleList->insertItem( image );
				if ( i != m_numSub ) subtitleList->insertItem( blank );
			}
		}
	} else KMessageBox::error( this, i18n( "Error extracting subtitles" ) );
}

void PreviewDialog::extractOutput( KProcIO *proc ) {
	QString line, word;

	while ( proc->readln( line, false ) != -1 ) {
		word = line.section( ' ', 0, 0 );
		if ( word == "Generating" )
			progress->progressBar()->advance( 1 );
		else if ( word == "Wrote" )
			m_numSub = line.section( ' ', 1, 1 ).toUInt();
		else if ( line != "Conversion finished" ) kdWarning() << line << endl;
	}

	proc->ackRead();
}

void PreviewDialog::setColours() {
	for (uint i = 0; i < 4; ++i)
		m_project->colours[i] = ( radioButton[i]->isChecked() ) ? 0 : 255;
}

void PreviewDialog::fillLanguages()
{
	SeekLanguagesInVob proc( m_project->files()[0], m_project->directory() );
	WaitingDialog dlg( this, 0, QString::null, i18n( "Seeking Languages" ) );

	dlg.show();
	proc.start();
	do {
		kapp->processEvents();
	} while ( !proc.wait( 100 ) );
	dlg.stop();
	dlg.close();

	LanguageMap map = proc.languages();
	if ( map.count() == 0 ) map.insert( "1", "0x20" );
	for ( LanguageMap::iterator it = map.begin(); it != map.end(); ++it )
		languageList->insertItem( it.key() );
}

#include "previewdialog.moc"
