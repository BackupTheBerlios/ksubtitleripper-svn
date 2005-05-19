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
#include <kapplication.h>
#include <dcopclient.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

static const char description[] =
    I18N_NOOP( "A KDE GUI for subtitleripper package." );

static const char version[] = "0.3";

static KCmdLineOptions options[] =
    {
        { "+[URL]", I18N_NOOP( "Project to open." ), 0 },
        KCmdLineLastOption
    };

int main( int argc, char **argv ) {
	KAboutData about( "ksubtitleripper", I18N_NOOP( "KSubtitleRipper" ), version, description,
	                  KAboutData::License_GPL, "(C) 2004 Sergio Cambra", 0, 0, "runico@users.berlios.de" );
	about.addAuthor( "Sergio Cambra", 0, "runico@users.berlios.de" );
	KCmdLineArgs::init( argc, argv, &about );
	KCmdLineArgs::addCmdLineOptions( options );
	KApplication app;

	// see if we are starting with session management
	if ( app.isRestored() ) {
		RESTORE( KSubtitleRipper );
	} else {
		// no session.. just start up normally
		KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
		KSubtitleRipper *widget = new KSubtitleRipper;
		widget->show();

		if ( args->count() > 0 ) widget->load( args->url( 0 ) );

		args->clear();
	}

	return app.exec();
}
