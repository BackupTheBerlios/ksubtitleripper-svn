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
#include <qvbox.h>
#include <qscrollview.h>

#include <kdebug.h>
#include <klocale.h>
#include "previewdialog.h"
#include "project.h"

PreviewDialog::PreviewDialog( Project *prj, QWidget *parent, const char* name )
 : KDialogBase( parent, name, true, i18n( "Converting images to text" ), Ok|Cancel|Help|User1,
 		Ok, false, KGuiItem( i18n( "Preview" ), "thumbnail" ) ), project( prj )
{
	// prj mustn't be 0
	if ( !prj ) kdFatal() << "ConvertDialog constructor: prj is null\n";
	
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
	
	scrollPreview = new QScrollView( top );
	previewArea = new QVBox( scrollPreview->viewport() );
	scrollPreview->addChild( previewArea );
	layoutGeneral->addWidget( scrollPreview );
	
	for (uint i = 0; i < 4; ++i)
		radioButton[i]->setChecked( prj->colours[i] == 0 );

	connect( this, SIGNAL( user1Clicked() ), this, SLOT( preview() ) );
	connect( this, SIGNAL( okClicked() ), this, SLOT( ok() ) );
}

PreviewDialog::~PreviewDialog()
{
}

void PreviewDialog::preview()
{

}

void PreviewDialog::ok() {
	for (uint i = 0; i < 4; ++i)
		project->colours[i] = ( radioButton[i]->isChecked() ) ? 0 : 255;
}

#include "previewdialog.moc"
