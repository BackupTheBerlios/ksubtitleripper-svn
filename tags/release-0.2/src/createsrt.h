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
#ifndef CREATESRT_H
#define CREATESRT_H

#include <qobject.h>

class KProcess;
class KProcIO;
class Project;

/**
@author Sergio Cambra
*/
class CreateSRT : public QObject {
Q_OBJECT
public:
	CreateSRT( const Project *prj, const QString& path, QObject *parent=0, const char *name=0);
	void saveSRT();

signals:
	void failed( CreateSRT *createSRT, const QString& error );
	void success( CreateSRT *createSRT );

private slots:
	void createFinished( KProcess* proc );
	void convertFinished( KProcess* proc );
	void readOutput( KProcIO *proc );

private:
	void srtUnix2Dos( const QString& path );

	QString path;
	const Project *project;
};

#endif
