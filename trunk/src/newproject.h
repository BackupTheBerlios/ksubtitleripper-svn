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

#ifndef NEWPROJECT_H
#define NEWPROJECT_H

#include <kdialogbase.h>
#include <kurl.h>

class Project;
class QLabel;
class QGridLayout;
class QVBoxLayout;
class KLineEdit;
class KListBox;
class KPushButton;

class NewProject : public KDialogBase {
	Q_OBJECT
public:
	NewProject(  QString prefix = "sub"  );
	~NewProject();
	Project* getProject();
	
private slots:
	void selectVobs();
	void prefixChanged( const QString& text );
	
private:
	QGridLayout *layoutGeneral;
	QVBoxLayout *layoutFilesLabel, *layoutFilesButton;
	KLineEdit *prefixEdit;
	KListBox *vobFilesList;
	KPushButton *vobFilesButton;
	QLabel *vobFilesLabel, *prefixLabel;
	
	KURL::List files;
};

#endif // NEWPROJECT_H
