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
#ifndef SEEKLANGUAGESINVOB_H
#define SEEKLANGUAGESINVOB_H

#include <qmap.h>
#include <qstring.h>
#include <kprocio.h>
#include <qthread.h>

class KURL;

/**
@author Sergio Cambra
 */

typedef QMap<QString, QString> LanguageMap;

class SeekLanguagesInVob : public QObject, public QThread
{
	Q_OBJECT
public:
	SeekLanguagesInVob( const KURL& vob, const QString& dir );
	~SeekLanguagesInVob();
	LanguageMap* languages() { return m_languages; }

protected:
	virtual void run();

private slots:
	void vobFinish( KProcess *proc );
	void vobOutput( KProcIO *proc );

private:
	bool download( const KURL& vob );

	LanguageMap *m_languages;
	KProcIO *m_proc;
	uint m_numberSubs;
};

#endif
