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

#include <kprocio.h>
#include <klocale.h>

#include "project.h"
#include "configuration.h"
#include "createsrt.h"

CreateSRT::CreateSRT( const Project *prj, const QString& path, QObject *parent, const char *name)
 : QObject(parent, name), path( path ), project( prj )
{
}

void CreateSRT::saveSRT() {
	KProcIO *process = new KProcIO();
	process->setWorkingDirectory( project->directory() );
	process->setUseShell( true );
	
	*process << "srttool" << "-s";
	*process << "-i" << project->baseName() + ".srtx";
	*process << "-o" << KProcess::quote( path );
	
	connect( process, SIGNAL( processExited( KProcess* ) ),
			this, SLOT( createFinished( KProcess* ) ) );
	
	if ( !process->start( KProcess::NotifyOnExit, false ) )
		emit failed( this, i18n( "Couldn't run srttool" ) );
}

void CreateSRT::createFinished( KProcess* proc ) {
	bool goodExit = proc->exitStatus() == 0;
	delete proc;
	
	if ( goodExit ) {
		if ( Config().doUnix2Dos() ) srtUnix2Dos( path );
		else emit success( this );
	} else emit failed( this, i18n( "Couldn't read from \"%1\" or couldn't write to \"%2\"" ).arg( project->directory() + project->baseName() + ".srtx" ).arg( path ) );
}

void CreateSRT::srtUnix2Dos( const QString& path ) {
	KProcIO *process = new KProcIO();
	process->setWorkingDirectory( project->directory() );
	process->setUseShell( true );
	
	*process << "dos2unix" << "--u2d" << KProcess::quote( path );
	
	connect( process, SIGNAL( processExited( KProcess* ) ),
			this, SLOT( convertFinished( KProcess* ) ) );
	
	if ( !process->start( KProcess::NotifyOnExit, false ) )
		emit failed( this, i18n( "Couldn't run dos2unix" ) );
}

void CreateSRT::convertFinished( KProcess* proc ) {
	bool goodExit = proc->exitStatus() == 0;
	delete proc;
	
	if ( goodExit ) emit success( this );
	else emit failed( this, i18n( "Couldn't read from or write to \"%1\"" ).arg( path ) );
}

#include "createsrt.moc"
