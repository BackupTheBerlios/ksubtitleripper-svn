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

#ifndef KSUBTITLERIPPERVIEW_H
#define KSUBTITLERIPPERVIEW_H

#include <kurl.h>
#include <kprocio.h>
#include "ksubtitleripperviewdlg.h"
#include "project.h"

class KSubtitleRipperView : public KSubtitleRipperViewDlg {
	Q_OBJECT

public:
	KSubtitleRipperView( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
	~KSubtitleRipperView();
	
	void newProject( Project* prj );
	bool loadProject( const KURL& url );
	bool saveProject( const KURL& url );
	bool isModified() const;
	bool askIfModified();

public slots:
	virtual void saveSubtitle();
	virtual void prevSubtitle();
	virtual void nextSubtitle();
	virtual void extractSub();
	virtual void convertSub();
	virtual void createSRT();

private:
	QString subFileName();
	void writeSubtitle();
	void loadSubtitle();
	void emptySubtitle();
	void beforeExtracting();
	void beforeConverting();

	bool modified;
	Project *project;
};

#endif
