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
#include <kio/netaccess.h>
#include <klocale.h>
#include <ktempfile.h>
#include <kmessagebox.h>
#include <ktextedit.h>
#include <qlabel.h>
#include <qfile.h>
#include "ksubtitleripperview.h"
#include "convertdialog.h"
#include "extractdialog.h"
#include "previewdialog.h"

KSubtitleRipperView::KSubtitleRipperView( QWidget* parent, const char* name, WFlags fl )
		: KSubtitleRipperViewDlg( parent, name, fl ), project( 0 ) {
	modified = false;
	text->setCheckSpellingEnabled( true );
	image->setPixmap( QPixmap() );
}

KSubtitleRipperView::~KSubtitleRipperView() {
	delete project;
}

/*$SPECIALIZATION$*/
void KSubtitleRipperView::writeSubtitle() {
	QString filename = project->directory() + project->subFilename() + ".pgm.txt";
	
	QFile f( filename );
	if ( !f.open ( IO_WriteOnly ) ){
		KMessageBox::error( this, i18n( "Couldn't write to file %1" ).arg( filename ) );
		return;
	}
	QTextStream out( &f );
	out << text->text();
	f.close();
	
	text->setModified( false );
}

void KSubtitleRipperView::saveSubtitle() {
	// this slot is called whenever the Subtitles->Save subtitle menu is selected,
	// the Save subtitle shortcut is pressed (usually CTRL+Return) or the Save subtitle
	// toolbar button is clicked
	
	writeSubtitle();
	if ( !project->atLast() ) nextSubtitle();
}

void KSubtitleRipperView::loadSubtitle() {
	QString filename = project->directory() + project->subFilename() + ".pgm";
	
	// set label
	subtitle->setText( i18n( "Subtitle %1" ).arg( project->currentSub() ) );
	
	// load image
	if ( !image->pixmap()->load( filename ) || image->pixmap()->isNull() ) {
		image->pixmap()->resize( 0, 0 );
		KMessageBox::error( this, i18n( "Couldn't load file %1" ).arg( filename ) );
	}
	image->update();
	
	// load text
	if ( project->isConverted() ) {
		filename += ".txt";
		QFile f( filename );
		if ( !f.open ( IO_ReadOnly ) ){
			text->setText( QString::null );
			KMessageBox::error( this, i18n( "Couldn't load file %1" ).arg( filename ) );
			return;
		}
		QTextStream in( &f );
		text->setText( in.read() );
		f.close();
		
		progress->setValue( project->currentSub() );
	} else {
		text->setText( QString::null );
		progress->setValue( 0 );
	}
}

void KSubtitleRipperView::emptySubtitle() {
	subtitle->setText( QString::null );
	if ( image->pixmap() ) image->pixmap()->resize( 0, 0 );
	text->setText( QString::null );
	progress->setValue( 0 );
}

bool KSubtitleRipperView::askIfModified() {
	// Ask for saving subtitle if has been modified
	// and return if can load other subtitle
	
	if ( text->isModified() && project != 0 && project->isConverted() ) {
		int answer = KMessageBox::warningYesNoCancel( this,
			i18n( "The subtitle has been modified.\n\nDo you want to save it?" ),
			i18n( "Save Subtitle?" ), KStdGuiItem::save(), KStdGuiItem::discard() );
	
		switch ( answer ) {
		case KMessageBox::Yes:
			writeSubtitle();
			return true;
			break;
		case KMessageBox::No:
			return true;
			break;
		default: // Cancel
			return false;
			break;
		}
	} else return true;
}

void KSubtitleRipperView::prevSubtitle() {
	// this slot is called whenever the Subtitles->Previous subtitle menu is selected,
	// or the Previous subtitle toolbar button is clicked
	
	if ( !askIfModified() ) return;
	
	if ( project->atLast() ) emit setEnabledNextSub( true );
	project->prevSub();
	if ( project->atFirst() ) emit setEnabledPrevSub( false );
	modified = true;
	loadSubtitle();
}

void KSubtitleRipperView::nextSubtitle() {
	// this slot is called whenever the Subtitles->Next subtitle menu is selected,
	// or the Next subtitle toolbar button is clicked
	
	if ( !askIfModified() ) return;
	
	if ( project->atFirst() ) emit setEnabledPrevSub( true );
	project->nextSub();
	if ( project->atLast() ) emit setEnabledNextSub( false );
	modified = true;
	loadSubtitle();
}

void KSubtitleRipperView::extractSub() {
	modified = true;
	beforeExtracting();
	
	if ( ExtractDialog( project, this ).exec() == QDialog::Accepted ) {
		project->setExtracted( true );
		progress->setTotalSteps( project->numSub() );
		
		if ( project->numSub() > 0 ) {
			KMessageBox::information( this, i18n( "%n subtitle has been extracted",
						"%n subtitles have been extracted", project->numSub() ),
						i18n("Extraction completed") );
						
			project->goFirst();
			loadSubtitle();
			emit setEnabledConvertSub( true );
			if ( !project->atLast() ) emit setEnabledNextSub( true );
			
		} else KMessageBox::information( this, i18n( "No subtitles have been extracted" ),
						i18n("Extraction completed") );
	}
}

void KSubtitleRipperView::convertSub() {
	modified = true;
	beforeConverting();

	if ( ConvertDialog( project, this ).exec() == QDialog::Accepted ) {
		project->setConverted( true );
		project->goFirst();
		loadSubtitle();
		
		emit setEnabledCreateSRT( true );
		emit setEnabledSaveSub( true );
		emit setEnabledPrevSub( false );
		if ( !project->atLast() ) emit setEnabledNextSub( true );
	}
}

void KSubtitleRipperView::createSRT() {
	if ( !askIfModified() ) return;

	KProcIO process;
	
	process.setWorkingDirectory( project->directory() );
	
	// TODO check if srttool is executable
	
	process << "srttool" << "-s";
	process << "-i" << project->baseName() + ".srtx";
	process << "-o" << project->baseName() + ".srt";
	
	if ( process.start( KProcess::DontCare, false ) ) process.detach();
	else kdError() << "error executing process" << endl;
}

void KSubtitleRipperView::beforeExtracting() {
	project->setExtracted( false );
	progress->setValue( 0 );
	emit setEnabledConvertSub( false );
	emit setEnabledPrevSub( false );
	emit setEnabledNextSub( false );
	subtitle->setText( QString::null );
	if ( image->pixmap() ) {
		image->pixmap()->resize( 0, 0 );
		image->update();
	}
	beforeConverting();
}

void KSubtitleRipperView::beforeConverting() {
	project->setConverted( false );
	emit setEnabledCreateSRT( false );
	emit setEnabledSaveSub( false );
	text->clear();
}

void KSubtitleRipperView::newProject( Project* prj ) {
	project = prj;
	modified = true;
	emit setEnabledExtractSub( true );
	emit signalChangeCaption( QString::null );
	
}

bool KSubtitleRipperView::loadProject( const KURL& url ) {
	QString target;
	bool success;
	Project *aux;
	
	if ( KIO::NetAccess::download( url, target, this ) ) {
		aux = new Project( target, success );
		if ( success ) {
			delete project;
			project = aux;
			progress->setTotalSteps( project->numSub() );
			modified = false;
			emit signalChangeCaption( url.prettyURL() );
			
			// enable and disable some actions
			emit setEnabledExtractSub( true );
			emit setEnabledConvertSub( project->isExtracted() );
			if ( project->isExtracted() ) {
				loadSubtitle();
				emit setEnabledPrevSub( !project->atFirst() );
				emit setEnabledNextSub( !project->atLast() );
			} else {
				emptySubtitle();
				emit setEnabledPrevSub( false );
				emit setEnabledNextSub( false );
			}
			emit setEnabledCreateSRT( project->isConverted() );
			emit setEnabledSaveSub( project->isConverted() );
		} else KMessageBox::error( this, i18n( "Couldn't open file %1" ).arg( target ) );
		
		KIO::NetAccess::removeTempFile( target );
		return success;
	} else {
		KMessageBox::error( this, i18n( "Couldn't download file %1" ).arg( url.prettyURL() ) );
		return false;
	}
}

bool KSubtitleRipperView::saveProject( const KURL& url ) {
	if ( url.isLocalFile() ) {
		if ( !project->save( url.path() ) ) {
			KMessageBox::error( this,
						i18n( "Couldn't save project to %1" ).arg( url.prettyURL() ) );
			return false;
		}
	} else {
		KTempFile tmp;
		tmp.setAutoDelete(true);
		if ( !project->save( tmp.name() ) ) {
			KMessageBox::error( this,
						i18n( "Couldn't save project to %1" ).arg( tmp.name() ) );
			return false;
		}
		if ( !KIO::NetAccess::upload( tmp.name(), url, this ) ) {
			KMessageBox::error(this,
						i18n( "Couldn't save remote file %1" ).arg( url.prettyURL() ) );
			return false;
		}
	}
	
	emit signalChangeCaption( url.prettyURL() );
	modified = false;
	return true;
}

bool KSubtitleRipperView::isModified() const {
	return modified;
}

#include "ksubtitleripperview.moc"




