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

#ifndef CONVERTDIALOG_H
#define CONVERTDIALOG_H

#include <kdialogbase.h>
#include <qstringlist.h>

class Project;
class KProcess;
class KLineEdit;
class KProgress;
class QLabel;
class QCheckBox;
class QVBoxLayout;
class QHBoxLayout;

/**
@author Sergio Cambra
*/
class ConvertDialog : public KDialogBase {
	Q_OBJECT
public:
    ConvertDialog( Project *prj, QWidget *parent = 0, const char* name = 0 );

    ~ConvertDialog();
	
public slots:
	virtual void show();

protected slots:
	virtual void slotOk();

private slots:
	void editChanged( const QString& text );
	virtual void convertQuestion( KProcess *proc, char *buffer, int buflen );
	virtual void gocrFinish( KProcess *proc );
	virtual void sent( KProcess *proc );
	virtual void killProcess();

private:
	void convertSub();
	void startGocr( KProcess *proc );
	void writeStdin( KProcess *proc, QString data );
	void writeStdin( KProcess *proc, int data );
	
	void loadSubtitle( QRect rect );
	void setEnabledWidgetsInput( bool enable );
	
	KLineEdit *line;
	QLabel *image, *subtitle, *text;
    KProgress *progress;
	QCheckBox *checkbox;
	
	QVBoxLayout *layoutGeneral;
	QHBoxLayout *layoutSub, *layoutCheckBox;
	
	Project *project;
	uint sub;
	QString databasePath;
	QValueList<char*> toSent;
	KProcess *process;
	bool sending;
	
	static const int marginGeneral = 5;
};

#endif
