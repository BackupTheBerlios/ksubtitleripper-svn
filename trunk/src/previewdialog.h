/***************************************************************************
 *   Copyright (C) 2004 by Sergio Cambra Garc�a                            *
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
#ifndef PREVIEWDIALOG_H
#define PREVIEWDIALOG_H

#include <kdialogbase.h>

class QVButtonGroup;
class QRadioButton;
class ExtractProcess;
class KProcess;
class KProcIO;
class KProgressDialog;
class KListBox;
class KComboBox;
class Project;

/**
@author Sergio Cambra
*/
class PreviewDialog : public KDialogBase {
	Q_OBJECT
public:
    PreviewDialog( Project *prj, QWidget *parent = 0, const char* name = 0 );
    ~PreviewDialog();

public slots:
	virtual void preview();

protected slots:
	virtual void extractFinish( KProcess *proc );
	virtual void extractOutput( KProcIO *proc );
	virtual void setColours();
	virtual void setLanguage();

private:
	void fillLanguages();

	QVButtonGroup *groupIndex;
	QRadioButton *radioButton[4];
	QListBox *subtitleList;
	KProgressDialog *progress;
	KComboBox *languageList;

	Project *m_project;
	ExtractProcess *m_process;
	uint m_numSub;
};

#endif
