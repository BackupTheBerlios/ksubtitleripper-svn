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

#include "ksubtitleripperviewdlg.h"

class KURL;
class CreateSRT;
class Project;

class KSubtitleRipperView : public KSubtitleRipperViewDlg
{
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
	void saveSubtitle();
	void prevSubtitle();
	void nextSubtitle();
	void extractSub();
	void convertSub();
	void createSRT();
	void createSrtFailed( CreateSRT *createSRT, const QString& error );
	void createSrtSuccess( CreateSRT *createSRT );
	void modify( bool m ) const;
	void setCheckSpellingEnabled( bool enabled );
	void setEditorFont( const QFont& font );

signals:
	void signalProjectModified();

private:
	QString subFileName();
	void writeSubtitle();
	void loadSubtitle();
	void emptySubtitle();
	void cleanBeforeExtracting();
	void cleanBeforeConverting();
	bool saveSRT( const QString& path );
	bool srtUnix2Dos( const QString& path );
	void setSrtName( const KURL& url );
	void setModified( bool modif );

	bool m_modified;
	Project *m_project;
	QString m_srtName, m_tmpSrt;
	KURL *m_newSrt;
};

#endif
