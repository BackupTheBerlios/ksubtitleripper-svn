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

#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kurl.h>
#include <kfiledialog.h>
#include "newproject.h"

NewProject::NewProject( QString prefix ) : KDialogBase( Plain,
			i18n( "Create New Project" ), Help | Ok | Cancel, Ok ) {
	QFrame* top = plainPage();
	
    layoutGeneral = new QGridLayout( top, 2, 3, 5, 6 );

    vobFilesList = new KListBox( top );
    layoutGeneral->addWidget( vobFilesList, 0, 1 );
	
    prefixLabel = new QLabel( i18n( "Prefix" ), top );
    layoutGeneral->addWidget( prefixLabel, 1, 0 );
    
    prefixEdit = new KLineEdit( prefix, top );
    layoutGeneral->addWidget( prefixEdit, 1, 1 );
	
    layoutFilesLabel = new QVBoxLayout( 0, 0, 6 ); 
    vobFilesLabel = new QLabel( i18n( "Vob files" ), top );
    layoutFilesLabel->addWidget( vobFilesLabel );
	layoutFilesLabel->addItem( new QSpacerItem( 20, 40, QSizePolicy::Minimum,
									QSizePolicy::Expanding ) );
    layoutGeneral->addLayout( layoutFilesLabel, 0, 0 );

	layoutFilesButton = new QVBoxLayout( 0, 0, 6 ); 
	vobFilesButton = new KPushButton ( top );
    layoutFilesButton->addWidget( vobFilesButton );
	layoutFilesButton->addItem( new QSpacerItem( 20, 40, QSizePolicy::Minimum,
									QSizePolicy::Expanding ) );
    layoutGeneral->addLayout( layoutFilesButton, 0, 2 );
    
	enableButtonOK( false );
	vobFilesButton->setPixmap( KGlobal::iconLoader()->loadIcon( "fileopen", KIcon::Small ) );
	connect( vobFilesButton, SIGNAL( clicked() ), this, SLOT( selectVobs() ) );
	connect( prefixEdit, SIGNAL( textChanged( const QString& ) ),
			this, SLOT( prefixChanged( const QString& ) ) );
}

NewProject::~NewProject() {}

Project* NewProject::getProject() {
	return new Project( files, prefixEdit->text() );
}

void NewProject::selectVobs() {
	files = KFileDialog::getOpenURLs( QString::null,
				"*.vob|" + i18n( "VOB files" ), this, i18n( "Select VOB files" ) );
	if ( files.isEmpty() ) return;
	
	enableButtonOK( prefixEdit->text().find( ' ' ) == -1 );
	vobFilesList->clear();
	vobFilesList->insertStringList( files.toStringList() );
}

void NewProject::prefixChanged( const QString& text ) {
	enableButtonOK( vobFilesList->count() > 0 && text.find( ' ' ) == -1 );
}

#include "newproject.moc"
