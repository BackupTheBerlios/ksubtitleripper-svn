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
#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>

#include "extractprocess.h"
#include "project.h"

ExtractProcess::ExtractProcess( Project *prj, bool& success, QTextCodec *codec )
 : KProcIO( codec )
{
	success = false;
	// prj mustn't be 0
	if ( !prj ) kdFatal() << "ExtractProcess constructor: prj is null\n";

	setUseShell( true );
	setWorkingDirectory( prj->directory() );
	setComm( KProcess::Stderr );
	enableReadSignals( true );

	// TODO check if cat, tcextract and subtitle2pgm are executable

	*this << "cat";

	uint n = prj->files().count();
	QString file;
	for (uint i = 0; i < n; ++i ) {
		if ( !prj->downloadVob( i, file ) ) return;
		else *this << file;
	}

	*this << "|" << "tcextract" << "-x" << "ps1" << "-t" << "vob" << "-a" << prj->codeLangSelected();
	*this << "|" << "subtitle2pgm" << "-v" << "-P" << "-C" << "1";
	*this << "-c" << prj->coloursString() << "-o" << prj->baseName();

	success = true;
}
