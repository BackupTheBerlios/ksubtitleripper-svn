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
#include <klistbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <klineedit.h>
#include <kpushbutton.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kdirselectdialog.h>
#include <kurlcompletion.h>
#include <qdir.h>
#include <qvalidator.h>
#include "project.h"
#include "newproject.h"

class DirValidator : public QValidator
{
public:
	DirValidator( QObject* parent, const char* name = 0 ) : QValidator( parent, name ) {}
	virtual State validate( QString& input, int& ) const
	{
		if ( input.isEmpty() ) return QValidator::Intermediate;

		if ( QDir( input ).exists() )
			return QValidator::Acceptable;

		KURL url( input );
		if ( url.isLocalFile() && QDir( url.path() ).exists() )
			return QValidator::Acceptable;

		return QValidator::Intermediate;
	}
};

NewProject::NewProject( QString prefix ) : KDialogBase( Plain,
			i18n( "Create New Project" ), Help | Ok | Cancel, Ok ) {
	QFrame* top = plainPage();

	QGridLayout* layoutGeneral = new QGridLayout( top, 2, 3, 5, 6 );

	int row = 0;
	QLabel* vobFilesLabel = new QLabel( i18n( "Vob or Sub files" ), top );
	vobFilesLabel->setAlignment( Qt::AlignTop );
	layoutGeneral->addWidget( vobFilesLabel, row, 0 );
	vobFilesList = new KListBox( top );
	vobFilesList->setMinimumSize( QSize( 200, 120 ) );
	layoutGeneral->addWidget( vobFilesList, row, 1 );

	QVBoxLayout* layoutFilesButton = new QVBoxLayout( 0, 0, 6 );
	KPushButton* vobFilesButton = new KPushButton ( top );
	layoutFilesButton->addWidget( vobFilesButton );
	layoutFilesButton->addItem( new QSpacerItem( 20, 40, QSizePolicy::Minimum,
								QSizePolicy::Expanding ) );
	layoutGeneral->addLayout( layoutFilesButton, row, 2 );

	++row;
	QLabel* dirLabel = new QLabel( i18n( "Directory" ), top );
	layoutGeneral->addWidget( dirLabel, row, 0 );

	dirEdit = new KLineEdit( top );
	dirEdit->setValidator( new DirValidator( dirEdit ) );
	dirEdit->setURLDropsEnabled( true );
	KURLCompletion* complet = new KURLCompletion( KURLCompletion::DirCompletion );
	dirEdit->setCompletionObject( complet );
	layoutGeneral->addWidget( dirEdit, row, 1 );

	KPushButton* dirButton = new KPushButton ( top );
	layoutGeneral->addWidget( dirButton, row, 2 );

	++row;
	QLabel* prefixLabel = new QLabel( i18n( "Prefix" ), top );
	layoutGeneral->addWidget( prefixLabel, row, 0 );
	prefixEdit = new KLineEdit( prefix, top );
	prefixEdit->setValidator( new QRegExpValidator( QRegExp( "\\S+" ), prefixEdit ) );
	layoutGeneral->addWidget( prefixEdit, row, 1 );

	enableButtonOK( false );
	vobFilesButton->setPixmap( KGlobal::iconLoader()->loadIcon( "fileopen", KIcon::Small ) );
	dirButton->setPixmap( KGlobal::iconLoader()->loadIcon( "fileopen", KIcon::Small ) );

	connect( vobFilesButton, SIGNAL( clicked() ), this, SLOT( selectVobs() ) );
	connect( dirButton, SIGNAL( clicked() ), this, SLOT( selectDir() ) );
	connect( prefixEdit, SIGNAL( textChanged( const QString& ) ),
			 this, SLOT( tryEnableButtonOk() ) );
	connect( dirEdit, SIGNAL( textChanged( const QString& ) ),
			 this, SLOT( tryEnableButtonOk() ) );
}

Project* NewProject::getProject() {
	Project *prj = new Project();
	prj->setDirectory( dirEdit->text() );
	prj->setFiles( files );
	prj->setBaseName( prefixEdit->text() );
	return prj;
}

void NewProject::selectVobs() {
	files = KFileDialog::getOpenURLs( QString::null,
				"*.vob *.sub|" + i18n( "VOB and SUB files" ), this, i18n( "Select VOB or SUB files" ) );
	if ( files.isEmpty() ) return;

	vobFilesList->clear();
	vobFilesList->insertStringList( files.toStringList() );
	if ( dirEdit->text().isEmpty() && files[0].isLocalFile() )
		dirEdit->setText( files[0].directory( false ) );
	tryEnableButtonOk();
}

void NewProject::selectDir() {
	KURL dir = KDirSelectDialog::selectDirectory( "/", true, this );
	if ( dir.isEmpty() ) return;
	dirEdit->setText( dir.path( 1 ) );
	tryEnableButtonOk();
}

void NewProject::tryEnableButtonOk()
{
	enableButtonOK( vobFilesList->count() > 0 && dirEdit->hasAcceptableInput() &&
			prefixEdit->hasAcceptableInput() );
}

void NewProject::slotOk()
{
	if ( QDir( dirEdit->text() ).exists() ) {
		if ( dirEdit->text().right( 1 ) != "/" )
			dirEdit->setText( dirEdit->text() + '/' );
	} else dirEdit->setText( KURL( dirEdit->text() ).path( 1 ) );

	KDialogBase::slotOk();
}

#include "newproject.moc"
