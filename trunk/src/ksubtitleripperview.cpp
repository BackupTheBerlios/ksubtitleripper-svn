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
#include <kfiledialog.h>
#include <qlabel.h>
#include <kprogress.h>
#include <kurl.h>

#include "ksubtitleripperview.h"
#include "convertdialog.h"
#include "extractdialog.h"
#include "createsrt.h"
#include "project.h"
//#include "configuration.h"

KSubtitleRipperView::KSubtitleRipperView( QWidget* parent, const char* name, WFlags fl )
		: KSubtitleRipperViewDlg( parent, name, fl ), m_project( 0 ), m_newSrt( 0 ) {
	m_modified = false;
	text->setCheckSpellingEnabled( true );
	image->setPixmap( QPixmap() );
	connect( text, SIGNAL( modificationChanged( bool ) ), this, SLOT( modify( bool ) ) );
}

KSubtitleRipperView::~KSubtitleRipperView() {
	delete m_project;
}

/*$SPECIALIZATION$*/
void KSubtitleRipperView::writeSubtitle() {
	QString filename = m_project->directory() + m_project->subFilename() + ".pgm.txt";

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
	if ( !m_project->atLast() ) nextSubtitle();
}

void KSubtitleRipperView::loadSubtitle() {
	QString filename = m_project->directory() + m_project->subFilename() + ".pgm";

	// set label
	subtitle->setText( i18n( "Subtitle %1" ).arg( m_project->currentSub() ) );

	// load image
	if ( !image->pixmap()->load( filename ) || image->pixmap()->isNull() ) {
		image->pixmap()->resize( 0, 0 );
		KMessageBox::error( this, i18n( "Couldn't load file %1" ).arg( filename ) );
	}
	image->update();

	// load text
	if ( m_project->isConverted() ) {
		progress->setValue( m_project->currentSub() );

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
	}
}

void KSubtitleRipperView::modify( bool m ) const {
	kdDebug() << m << endl;
	kdDebug() << kdBacktrace( 10 ) << endl;
}

bool KSubtitleRipperView::askIfModified() {
	// Ask for saving subtitle if has been modified
	// and return if can load other subtitle

	if ( text->isModified() && m_project != 0 && m_project->isConverted() ) {
		int answer = KMessageBox::warningYesNoCancel( this,
			i18n( "The subtitle has been modified.\n\nDo you want to save it?" ),
			i18n( "Save Subtitle?" ), KStdGuiItem::save(), KStdGuiItem::discard() );

		if ( answer == KMessageBox::Yes ) writeSubtitle();
		return answer != KMessageBox::Cancel;
	} else return true;
}

void KSubtitleRipperView::prevSubtitle() {
	// this slot is called whenever the Subtitles->Previous subtitle menu is selected,
	// or the Previous subtitle toolbar button is clicked

	if ( !askIfModified() ) return;

	if ( m_project->atLast() ) emit setEnabledNextSub( true );
	m_project->prevSub();
	if ( m_project->atFirst() ) emit setEnabledPrevSub( false );
	m_modified = true;
	loadSubtitle();
}

void KSubtitleRipperView::nextSubtitle() {
	// this slot is called whenever the Subtitles->Next subtitle menu is selected,
	// or the Next subtitle toolbar button is clicked

	if ( !askIfModified() ) return;

	if ( m_project->atFirst() ) emit setEnabledPrevSub( true );
	m_project->nextSub();
	if ( m_project->atLast() ) emit setEnabledNextSub( false );
	m_modified = true;
	loadSubtitle();
}

void KSubtitleRipperView::extractSub() {
	cleanBeforeExtracting();
	emit setState( "newProject" );

	if ( ExtractDialog( m_project, this ).exec() == QDialog::Accepted ) {
		m_project->setExtracted( true );
		progress->setTotalSteps( m_project->numSub() );

		if ( m_project->numSub() > 0 ) {
			KMessageBox::information( this, i18n( "%n subtitle has been extracted",
									  "%n subtitles have been extracted", m_project->numSub() ),
									  i18n("Extraction completed") );

			m_project->goFirst();
			loadSubtitle();
			emit setEnabledConvertSub( true );
			if ( !m_project->atLast() ) emit setEnabledNextSub( true );

		} else KMessageBox::information( this, i18n( "No subtitles have been extracted" ),
						i18n("Extraction completed") );
	} else {
		m_project->setExtracted( false );
		m_project->setConverted( false );
	}
	m_modified = true;
}

void KSubtitleRipperView::convertSub() {
	cleanBeforeConverting();
	emit setEnabledCreateSRT( false );
	emit setEnabledSaveSub( false );

	if ( ConvertDialog( m_project, this ).exec() == QDialog::Accepted ) {
		m_project->setConverted( true );
		m_project->goFirst();
		loadSubtitle();

		emit setEnabledCreateSRT( true );
		emit setEnabledSaveSub( true );
		emit setEnabledPrevSub( false );
		if ( !m_project->atLast() ) emit setEnabledNextSub( true );

	} else m_project->setConverted( false );
	m_modified = true;
}

void KSubtitleRipperView::createSRT() {
	// TODO check if srttool is executable

	if ( !askIfModified() ) return;

	KURL url = KFileDialog::getSaveURL( m_srtName, "*.srt|" + i18n("SRT Subtitles"), this, i18n( "Save Subtitles" ) );
	if ( url.isEmpty() || !url.isValid() ) return;

	/*QString extension = QFileInfo( url.path() ).extension( false ).lower();
	if ( extension != "srt" && ( !url.isLocalFile() || !QFile::exists( url.path() ) ) )
		url = url.url() + ".srt";
	*/
	QString text = "A file named \"%1\" already exists.\nAre you sure you want to overwrite it?";
	if ( url.isLocalFile() && QFile::exists( url.path() ) &&
		KMessageBox::warningContinueCancel( this, i18n( text ).arg( url.filename() ),
		i18n( "Overwrite File?" ), i18n( "Overwrite" ) ) == KMessageBox::Cancel ) return;

	m_newSrt = new KURL( url );

	if ( url.isLocalFile() ) {
		m_tmpSrt = QString::null;

		CreateSRT *createSrt = new CreateSRT( m_project, url.path() );

		connect(createSrt, SIGNAL(success( CreateSRT* )),
			this, SLOT(createSrtSuccess( CreateSRT* ) ) );
		connect(createSrt, SIGNAL(failed( CreateSRT*, const QString& )),
			this, SLOT(createSrtFailed( CreateSRT*, const QString& ) ) );

		createSrt->saveSRT();
	} else {
		KTempFile tmp;
		tmp.setAutoDelete(true);
		m_tmpSrt = tmp.name();

		CreateSRT *createSrt = new CreateSRT( m_project, tmp.name() );

		connect(createSrt, SIGNAL(success( CreateSRT* )),
			this, SLOT(createSrtSuccessRemote( CreateSRT* ) ) );
		connect(createSrt, SIGNAL(failed( CreateSRT*, const QString& )),
			this, SLOT(createSrtFailed( CreateSRT*, const QString& ) ) );

		createSrt->saveSRT();
	}
}

void KSubtitleRipperView::createSrtFailed( CreateSRT *createSRT, const QString& error ) {
	delete createSRT;
	KMessageBox::error( this, error );
}

void KSubtitleRipperView::createSrtSuccess( CreateSRT *createSRT ) {
	delete createSRT;
	bool success = true;

	if ( !m_tmpSrt.isNull() ) {
		if ( !KIO::NetAccess::upload( m_tmpSrt, *m_newSrt, this ) ) {
			success = false;
			KMessageBox::error(this, i18n( "Couldn't save remote file %1" ).arg( m_newSrt->prettyURL() ) );
		}
		m_tmpSrt = QString::null;
	}
	if ( success ) m_srtName = m_newSrt->url();
	delete m_newSrt;
	m_newSrt = 0;
}

void KSubtitleRipperView::cleanBeforeExtracting() {
	subtitle->clear();
	if ( image->pixmap() ) {
		image->pixmap()->resize( 0, 0 );
		image->update();
	}
	cleanBeforeConverting();
}

void KSubtitleRipperView::cleanBeforeConverting() {
	text->clear();
	progress->setValue( 0 );
}

void KSubtitleRipperView::newProject( Project* prj ) {
	delete m_project;
	m_project = prj;
	m_modified = true;
	cleanBeforeExtracting();
	emit signalChangeCaption( QString::null );
}

bool KSubtitleRipperView::loadProject( const KURL& url ) {
	QString target;
	bool success;
	Project *aux;

	if ( KIO::NetAccess::download( url, target, this ) ) {
		aux = new Project( target, success );
		if ( success ) {
			delete m_project;
			m_project = aux;
			progress->setTotalSteps( m_project->numSub() );
			m_modified = false;
			emit signalChangeCaption( url.prettyURL() );

			// setup some actions
			emit setState( "newProject" );
			emit setEnabledConvertSub( m_project->isExtracted() );
			if ( m_project->isExtracted() ) {
				loadSubtitle();
				if ( !m_project->atFirst() ) emit setEnabledPrevSub( true );
				if ( !m_project->atLast() ) emit setEnabledNextSub( true );
				if ( m_project->isConverted() ) {
					emit setEnabledCreateSRT( true );
					emit setEnabledSaveSub( true );
				} else cleanBeforeConverting();
			} else cleanBeforeExtracting();

			setSrtName( url );
		} else {
			delete aux;
			KMessageBox::error( this, i18n( "Couldn't open file %1" ).arg( target ) );
		}

		KIO::NetAccess::removeTempFile( target );
		return success;
	} else {
		KMessageBox::error( this, i18n( "Couldn't download file %1" ).arg( url.prettyURL() ) );
		return false;
	}
}

bool KSubtitleRipperView::saveProject( const KURL& url ) {
	if ( url.isLocalFile() ) {
		if ( !m_project->save( url.path() ) ) {
			KMessageBox::error( this,
						i18n( "Couldn't save project to %1" ).arg( url.prettyURL() ) );
			return false;
		}
	} else {
		KTempFile tmp;
		tmp.setAutoDelete(true);
		if ( !m_project->save( tmp.name() ) ) {
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

	setSrtName( url );
	emit signalChangeCaption( url.prettyURL() );
	m_modified = false;
	return true;
}

bool KSubtitleRipperView::isModified() const {
	return m_modified;
}

void KSubtitleRipperView::setSrtName( const KURL& url ) {
	m_srtName = url.url();
	int index = m_srtName.findRev( '.' );
	if ( index != -1 ) m_srtName.truncate( index );
	m_srtName += ".srt";
}

#include "ksubtitleripperview.moc"




