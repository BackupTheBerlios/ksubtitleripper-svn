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

#include <kdebug.h>
#include <klocale.h>
#include "previewdialog.h"
#include "project.h"
#include "extractprocess.h"
#include <iostream>

PreviewDialog::PreviewDialog( Project *prj, QWidget *parent, const char* name )
 : KDialogBase( parent, name, true, i18n( "Converting images to text" ), Ok|Cancel|Help|User1,
 		Ok, false, KGuiItem( i18n( "Preview" ), "thumbnail" ) ), project( prj )
{
	// prj mustn't be 0
	if ( !prj ) kdFatal() << "PreviewDialog constructor: prj is null\n";
	coloursOld = prj->coloursString();
	
	QFrame *top = makeMainWidget();
	
	QVBoxLayout* layoutGeneral = new QVBoxLayout( top, 5, 6 );
	
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
	layoutGeneral->addWidget( subtitleList );
	
	for (uint i = 0; i < 4; ++i)
		radioButton[i]->setChecked( prj->colours[i] == 0 );

	connect( this, SIGNAL( user1Clicked() ), this, SLOT( preview() ) );
	connect( this, SIGNAL( okClicked() ), this, SLOT( setColours() ) );
	connect( this, SIGNAL( cancelClicked() ), this, SLOT( restoreColours() ) );
}

PreviewDialog::~PreviewDialog()
{
}

void PreviewDialog::preview()
{
	numSub = 0;
	setColours();
	
	process = new ExtractProcess( project, this );
	*process << "-e" << QString( "00:00:00,%1" ).arg( 3 );
	
	connect( process, SIGNAL( processExited( KProcess* ) ),
			this, SLOT( extractFinish( KProcess* ) ) );
	connect( process, SIGNAL( readReady( KProcIO* ) ),
			this, SLOT( extractOutput( KProcIO* ) ) );
	
	if ( !process->start( KProcess::NotifyOnExit, true ) )
		kdError() << "error executing process\n";	
}

void PreviewDialog::extractFinish( KProcess *proc ) {
	bool goodExit = proc->exitStatus() == 0;
	delete proc;
	
	if ( goodExit ) {
		KMessageBox::information( this, "fin" );
		subtitleList->clear();
		for (uint i = 1; i <= numSub; ++i) {
			QString filename = project->directory() + project->subFilename( i ) + ".pgm";
			QPixmap image( filename );
			
			if ( image.isNull() )
				KMessageBox::error( this, i18n( "Couldn't load file %1" ).arg( filename ) );
			else {
				subtitleList->insertItem( image );
				if ( i != numSub ) subtitleList->insertItem( QPixmap(1, 20) );
			}
		}
	} else KMessageBox::error( this, i18n( "Error extracting subtitles" ) );
}

void PreviewDialog::extractOutput( KProcIO *proc ) {
	QString line, word;
	
	while ( proc->readln( line, false ) != -1 ) {
		word = line.section( ' ', 0, 0 );
		if ( word == "Generating" )
			;//subtitle->setText( i18n( "Generating image %1" ).arg( line.section( ": ", 1, 1 ) ) );
		else if ( word == "Wrote" )
			numSub = line.section( ' ', 1, 1 ).toUInt();
	}
	
	proc->ackRead();
}

void PreviewDialog::setColours() {
	for (uint i = 0; i < 4; ++i)
		project->colours[i] = ( radioButton[i]->isChecked() ) ? 0 : 255;
}

void PreviewDialog::restoreColours() {
	project->setColours( coloursOld );
}

#include "previewdialog.moc"
