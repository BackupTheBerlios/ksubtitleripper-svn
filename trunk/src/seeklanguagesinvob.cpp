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
#include <kio/netaccess.h>
#include <kurl.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <qregexp.h>

#include "seeklanguagesinvob.h"

SeekLanguagesInVob::SeekLanguagesInVob( const KURL& vob, const QString& dir, bool& success )
	: m_numberSubs ( 0 )
{
	success = false;
	m_languages = new LanguageMap();
	m_proc = new KProcIO();

	m_proc->setUseShell( true );
	m_proc->setWorkingDirectory( dir );
	m_proc->setComm( KProcess::Stdout );
	m_proc->enableReadSignals( true );

	// TODO check if tcprobe is executable

	*m_proc << "tcprobe" << "-H" << "100" << "-i";
	if ( !download( vob ) ) return;

	connect( m_proc, SIGNAL( processExited( KProcess* ) ),
			 this, SLOT( vobFinish( KProcess* ) ) );
	connect( m_proc, SIGNAL( readReady( KProcIO* ) ),
			 this, SLOT( vobOutput( KProcIO* ) ) );

	success = true;
}

SeekLanguagesInVob::~SeekLanguagesInVob() {
	delete m_proc;
}

void SeekLanguagesInVob::run()
{
	if ( !m_proc->start( KProcess::Block, false ) )
		kdError() << "error executing process\n";
}

void SeekLanguagesInVob::vobFinish( KProcess *proc )
{
	bool goodExit = proc->exitStatus() == 0;

	if ( goodExit )
		for (uint i = 0; i < m_numberSubs; ++i)
			m_languages->insert( QString::number( i+1 ), QString( "0x%1" ).arg( 20+i ) );
	else KMessageBox::error( 0, i18n( "Error seeking subtitles in VOB file" ) );
}

void SeekLanguagesInVob::vobOutput( KProcIO *proc )
{
	QString line;
	QRegExp re( "detected \\((\\d+)\\) subtitle\\(s\\)" );

	while ( proc->readln( line, false ) != -1 )
		if ( re.exactMatch( line ) )
			m_numberSubs = re.cap( 1 ).toUInt();

	proc->ackRead();
}


bool SeekLanguagesInVob::download( const KURL& vob ) {
	QString target;

	if ( KIO::NetAccess::download( vob, target, 0 ) ) {
		*m_proc << KProcess::quote( target );
		KIO::NetAccess::removeTempFile( target );
		return true;
	} else {
		QString error = KIO::NetAccess::lastErrorString();
		if ( !error.isEmpty() )
			KMessageBox::error( 0, error );
		return false;
	}
}

#include "seeklanguagesinvob.moc"
