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
/*
 * Copyright (C) 2004 Sergio Cambra <runico@users.berlios.de>
 */

#ifndef _KSUBTITLERIPPER_H_
#define _KSUBTITLERIPPER_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kmainwindow.h>
#include <kurl.h>

class KSubtitleRipperView;
class KToggleAction;
class PrefDialog;

/**
 * This class serves as the main window for KSubtitleRipper.  It handles the
 * menus, toolbars, and status bars.
 *
 * @short Main window class
 * @author $AUTHOR <$EMAIL>
 * @version $APP_VERSION
 */
class KSubtitleRipper : public KMainWindow {
	Q_OBJECT
public:
	/**
	 * Default Constructor
	 */
	KSubtitleRipper();

	/**
	 * Default Destructor
	 */
	virtual ~KSubtitleRipper();

	void load( const KURL& url );

protected:
	/**
	 * Overridden virtuals for Qt drag 'n drop (XDND)
	 */
	virtual void dragEnterEvent( QDragEnterEvent *event );
	virtual void dropEvent( QDropEvent *event );
	virtual bool queryClose();

protected:
	/**
	 * This function is called when it is time for the app to save its
	 * properties for session management purposes.
	 */
	void saveProperties( KConfig * );

	/**
	 * This function is called when this app is restored.  The KConfig
	 * object points to the session management config file that was saved
	 * with @ref saveProperties
	 */
	void readProperties( KConfig * );


private slots:
	void fileNew();
	void fileOpen();
	void fileSave();
	void fileSaveAs();
	void optionsConfigureKeys();
	void optionsConfigureToolbars();
	void optionsPreferences();
	void newToolbarConfig();
	void applyPreferences();
	void switchCheckSpelling();

	void changeStatusbar( const QString& text );
	void changeCaption( const QString& text );
	void setState( const QString& state );
	void updateCaption();

private:
	void setupActions();
	bool canCloseProject();
	bool hasName();

	KSubtitleRipperView *m_view;

	KToggleAction *m_enableCheckSpelling;
	KURL m_project;
	PrefDialog *m_prefDialog;
	QString nameProject;
};

#endif // _KSUBTITLERIPPER_H_
