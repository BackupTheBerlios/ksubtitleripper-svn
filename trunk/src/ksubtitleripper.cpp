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

#include "ksubtitleripper.h"
//#include "pref.h"
#include "newproject.h"

#include <kglobal.h>
#include <klocale.h>
#include <kdeversion.h>
#include <kmenubar.h>
#include <kstatusbar.h>
#include <kkeydialog.h>
#include <kfiledialog.h>
#include <kconfig.h>
#include <kurl.h>
#include <kurldrag.h>
#include <kedittoolbar.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kmessagebox.h>

namespace {
	QString untitled( i18n ( "Untitled" ) );
//	QString projectFilter( "*.srip|" + i18n( "KSubtitleRipper Project Files" ) );
}

KSubtitleRipper::KSubtitleRipper() : KMainWindow( 0, "KSubtitleRipper" ),
			m_view( new KSubtitleRipperView( this ) ) {
	// accept dnd
	setAcceptDrops( true );

	// tell the KMainWindow that this is indeed the main widget
	setCentralWidget( m_view );

	// then, setup our actions
	setupActions();

	// and a status bar
	statusBar() ->show();

	// apply the saved mainwindow settings, if any, and ask the mainwindow
	// to automatically save settings if changed: window size, toolbar
	// position, icon size, etc.
	setAutoSaveSettings();

	// allow the view to change the statusbar and caption
	connect( m_view, SIGNAL( signalChangeStatusbar( const QString& ) ),
	        this, SLOT( changeStatusbar( const QString& ) ) );
	connect( m_view, SIGNAL( signalChangeCaption( const QString& ) ),
	        this, SLOT( changeCaption( const QString& ) ) );
}

KSubtitleRipper::~KSubtitleRipper() {}

void KSubtitleRipper::load( const KURL& url ) {
	if ( canCloseProject() && m_view->loadProject( url ) )
		setProject( url );
}

void KSubtitleRipper::setupActions() {
	KAction *saveSub, *prevSub, *nextSub;
	KAction *extractSub, *convertSub, *createSRT;
	
	KStdAction::openNew( this, SLOT( fileNew() ), actionCollection() );
	KStdAction::open( this, SLOT( fileOpen() ), actionCollection() );
	KStdAction::save( this, SLOT( fileSave() ), actionCollection() );
	KStdAction::saveAs( this, SLOT( fileSaveAs() ), actionCollection() );
	KStdAction::quit( kapp, SLOT( quit() ), actionCollection() );

	m_toolbarAction = KStdAction::showToolbar( this, SLOT( optionsShowToolbar() ), actionCollection() );
	m_statusbarAction = KStdAction::showStatusbar( this, SLOT( optionsShowStatusbar() ), actionCollection() );

	KStdAction::keyBindings( this, SLOT( optionsConfigureKeys() ), actionCollection() );
	KStdAction::configureToolbars( this, SLOT( optionsConfigureToolbars() ), actionCollection() );
	//KStdAction::preferences( this, SLOT( optionsPreferences() ), actionCollection() );

	saveSub = new KAction( i18n( "Save subtitle" ), CTRL+Key_Return,
			m_view, SLOT( saveSubtitle() ), actionCollection(), "saveSubtitle" );
	prevSub = new KAction( i18n( "Previous subtitle" ), "previous", ALT+Key_Left, m_view,
			SLOT( prevSubtitle() ), actionCollection(), "prevSubtitle" );
	nextSub = new KAction( i18n( "Next subtitle" ), "next", ALT+Key_Right, m_view,
			SLOT( nextSubtitle() ), actionCollection(), "nextSubtitle" );
	extractSub = new KAction( i18n( "Extract subtitles" ), "extract_subtitles", 0,
			m_view,SLOT( extractSub() ), actionCollection(), "extractSubtitle" );
	convertSub = new KAction( i18n( "Convert subtitles to text" ), "convert_subtitles", 0,
			m_view, SLOT( convertSub() ), actionCollection(), "convertSubtitle" );
	createSRT = new KAction( i18n( "Generate SRT file" ), "create_srt", 0,
			m_view, SLOT( createSRT() ), actionCollection(), "createSRT" );

	// allow the view to enable and disable some actions
	connect( m_view, SIGNAL( setEnabledSaveSub( bool ) ),
	        saveSub, SLOT( setEnabled( bool ) ) );
	connect( m_view, SIGNAL( setEnabledPrevSub( bool ) ),
	        prevSub, SLOT( setEnabled( bool ) ) );
	connect( m_view, SIGNAL( setEnabledNextSub( bool ) ),
	        nextSub, SLOT( setEnabled( bool ) ) );
	connect( m_view, SIGNAL( setEnabledExtractSub( bool ) ),
	        extractSub, SLOT( setEnabled( bool ) ) );
	connect( m_view, SIGNAL( setEnabledConvertSub( bool ) ),
	        convertSub, SLOT( setEnabled( bool ) ) );
	connect( m_view, SIGNAL( setEnabledCreateSRT( bool ) ),
	        createSRT, SLOT( setEnabled( bool ) ) );
	
	createGUI();
	stateChanged( "initial" );
}

void KSubtitleRipper::saveProperties( KConfig *config ) {
	// the 'config' object points to the session managed
	// config file.  anything you write here will be available
	// later when this app is restored

	if ( hasName() ) {
#if KDE_IS_VERSION(3,1,3)
		config->writePathEntry( "lastURL", project.url() );
#else
		config->writeEntry( "lastURL", project.url() );
#endif

	}
}

void KSubtitleRipper::readProperties( KConfig *config ) {
	// the 'config' object points to the session managed
	// config file.  this function is automatically called whenever
	// the app is being restored.  read in here whatever you wrote
	// in 'saveProperties'

	QString url = config->readPathEntry( "lastURL" );

	if ( !url.isEmpty() )
		load( KURL( url ) );
}

void KSubtitleRipper::dragEnterEvent( QDragEnterEvent *event ) {
	// accept uri drops only
	event->accept( KURLDrag::canDecode( event ) );
}

void KSubtitleRipper::dropEvent( QDropEvent *event ) {
	// this is a very simplistic implementation of a drop event.  we
	// will only accept a dropped URL.  the Qt dnd code can do *much*
	// much more, so please read the docs there
	KURL::List urls;

	// see if we can decode a URI.. if not, just ignore it
	if ( KURLDrag::decode( event, urls ) && !urls.isEmpty() ) {
		// okay, we have a URI... process it
		load( urls.first() );
	}
}

void KSubtitleRipper::setProject( const KURL& url ) {
	stateChanged( "withProject" );
	project = url;
}

void KSubtitleRipper::fileNew() {
	// this slot is called whenever the File->New menu is selected,
	// the New shortcut is pressed (usually CTRL+N) or the New toolbar
	// button is clicked

	if ( canCloseProject() ) {
		NewProject dialog;
		if ( dialog.exec() == QDialog::Accepted ) {
			m_view->newProject( dialog.getProject() );
			setProject( KURL() );
		}
	}
}

void KSubtitleRipper::fileOpen() {
	// this slot is called whenever the File->Open menu is selected,
	// the Open shortcut is pressed (usually CTRL+O) or the Open toolbar
	// button is clicked

	// standard filedialog
	KURL url = KFileDialog::getOpenURL( QString::null, "*.srip|" + i18n("KSubtitleRipper Project Files"), this, i18n( "Open Project" ) );
	if ( !url.isEmpty() && url.isValid() )
		load( url );
}

void KSubtitleRipper::fileSave() {
	// this slot is called whenever the File->Save menu is selected,
	// the Save shortcut is pressed (usually CTRL+S) or the Save toolbar
	// button is clicked

	if ( hasName() ) m_view->saveProject( project );
	else fileSaveAs();
}

void KSubtitleRipper::fileSaveAs() {
	// this slot is called whenever the File->Save As menu is selected
	QString text = "A file named \"%1\" already exists.\nAre you sure you want to overwrite it?";

	KURL url = KFileDialog::getSaveURL( QString::null, "*.srip|" + i18n("KSubtitleRipper Project Files"), this, i18n( "Save Project" ) );
	
	if ( !url.isEmpty() && url.isValid() ) {
		if ( !url.isLocalFile() || !QFile::exists( url.path() ) ) {
			if ( QFileInfo( url.path() ).extension( false ) != "srip" ) url = url.url() + ".srip";
		}
		
		if ( url.isLocalFile() && QFile::exists( url.path() ) &&
			KMessageBox::warningContinueCancel( this, i18n( text ).arg( url.filename() ),
			i18n( "Overwrite File?" ), i18n( "Overwrite" ) ) == KMessageBox::Cancel ) return;
		
		if ( m_view->saveProject( url ) ) project = url;
	}
}


void KSubtitleRipper::optionsShowToolbar() {
	if ( m_toolbarAction->isChecked() )
		toolBar() ->show();
	else
		toolBar() ->hide();
}

void KSubtitleRipper::optionsShowStatusbar() {
	if ( m_statusbarAction->isChecked() )
		statusBar() ->show();
	else
		statusBar() ->hide();
}

void KSubtitleRipper::optionsConfigureKeys() {
	KKeyDialog::configure( actionCollection() );
}

void KSubtitleRipper::optionsConfigureToolbars() {
	// use the standard toolbar editor
#if defined(KDE_MAKE_VERSION)
# if KDE_VERSION >= KDE_MAKE_VERSION(3,1,0)
	saveMainWindowSettings( KGlobal::config(), autoSaveGroup() );
# else
	saveMainWindowSettings( KGlobal::config() );
# endif
#else
	saveMainWindowSettings( KGlobal::config() );
#endif

	KEditToolbar dlg( actionCollection() );
	connect( &dlg, SIGNAL( newToolbarConfig() ), this, SLOT( newToolbarConfig() ) );
	dlg.exec();
}

void KSubtitleRipper::newToolbarConfig() {
	// this slot is called when user clicks "Ok" or "Apply" in the toolbar editor.
	// recreate our GUI, and re-apply the settings (e.g. "text under icons", etc.)
	createGUI();

#if defined(KDE_MAKE_VERSION)
# if KDE_VERSION >= KDE_MAKE_VERSION(3,1,0)
	applyMainWindowSettings( KGlobal::config(), autoSaveGroup() );
# else
	applyMainWindowSettings( KGlobal::config() );
# endif
#else
	applyMainWindowSettings( KGlobal::config() );
#endif
}

/*void KSubtitleRipper::optionsPreferences() {
	// popup some sort of preference dialog, here
	KSubtitleRipperPreferences dlg;
	if ( dlg.exec() ) {
		// TODO redo your settings
	}
}*/

void KSubtitleRipper::changeStatusbar( const QString& text ) {
	// display the text on the statusbar
	statusBar() ->message( text );
}

void KSubtitleRipper::changeCaption( const QString& text ) {
	// display the text on the caption
	if ( text.isEmpty() )
		setCaption( untitled + " - " + kapp->caption() );
	else setCaption( text + " - " + kapp->caption() );
}

bool KSubtitleRipper::canCloseProject() {
	if ( !m_view->askIfModified() ) return false;
	if ( !m_view->isModified() ) return true;

	QString file = hasName() ? project.prettyURL() : untitled;
	int answer = KMessageBox::warningYesNoCancel( this,
		i18n( "The project \"%1\" has been modified.\n\nDo you want to save it?" ).arg( file ), i18n( "Save Project?" ), KStdGuiItem::save(), KStdGuiItem::discard() );

	switch ( answer ) {
	case KMessageBox::Yes:
		fileSave();
		return true;
		break;
	case KMessageBox::No:
		return true;
		break;
	default: // Cancel
		return false;
	}
}

bool KSubtitleRipper::queryClose() {
	return canCloseProject();
}

bool KSubtitleRipper::hasName() {
	return !project.isEmpty();
}

#include "ksubtitleripper.moc"
