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
#ifndef SUBTITLEVIEW_H
#define SUBTITLEVIEW_H

#include <qscrollview.h>

/**
@author Sergio Cambra
*/

class QLabel;

class SubtitleView : public QScrollView
{
	Q_OBJECT
public:
	SubtitleView( QWidget *parent = 0, const char *name = 0 );
	~SubtitleView() {}

	void load( const QString& filename );
	void load( const QString& filename, const QRect& rect );
    void clearSubtitle();

	bool isAutoCenterEnabled() const { return m_autoCenter; }
	void setAutoCenterEnabled( bool value ) { m_autoCenter = value; }

private:
    void searchCenter();
	void loadSubtitle( const QString& filename );

	QLabel* m_image;
	QPoint* m_center;
    bool m_autoCenter;

public slots:
    void centerSubtitle();
};

#endif
