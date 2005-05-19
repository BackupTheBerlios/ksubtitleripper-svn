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
#include "subtitleview.h"
#include <qlabel.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <qimage.h>
#include <qpainter.h>
#include <qpoint.h>

SubtitleView::SubtitleView( QWidget *parent, const char *name )
	: QScrollView( parent, name ), m_center( 0 ), m_autoCenter( false )
{
	m_image = new QLabel( this );
	m_image->setPixmap( QPixmap() );
	addChild( m_image );
}

void SubtitleView::loadSubtitle( const QString& filename )
{
	if ( !m_image->pixmap()->load( filename ) || m_image->pixmap()->isNull() ) {
		m_image->pixmap()->resize( 0, 0 );
		KMessageBox::error( this, i18n( "Couldn't load file %1" ).arg( filename ) );
	} else m_image->resize( m_image->pixmap()->size() );

	delete m_center;
	m_center = 0;
}

void SubtitleView::load( const QString& filename )
{
	loadSubtitle( filename );
	m_image->update();
	if ( m_autoCenter ) centerSubtitle();
}

void SubtitleView::load( const QString& filename, const QRect& rect )
{
	loadSubtitle( filename );

	// mark unknown characters
	QPainter painter( m_image->pixmap() );
	painter.setPen( Qt::red );
	painter.drawRect( rect );
	m_image->update();
	m_center = new QPoint( rect.center() );

	if ( m_autoCenter ) centerSubtitle();
}

void SubtitleView::searchCenter()
{
	const QImage& img = m_image->pixmap()->convertToImage();
	QRgb black = Qt::black.rgb();
	int top = img.height(), bottom = 0, left = img.width(), right = 0;
	for ( int y = 0; y < img.height(); ++y ) {
		bool inRow = false;
		for ( int x = 0; x < img.width(); ++x ) {
			if ( img.pixel( x, y ) == black ) {
				inRow = true;
				if ( left > x ) left = x;
				if ( right < x ) right = x;
			}
		}
		if ( inRow ) {
			if ( top > y ) top = y;
			if ( bottom < y ) bottom = y;
		}
	}

	m_center = new QPoint( ( left + right ) / 2, ( top + bottom ) / 2 );
}

void SubtitleView::clearSubtitle()
{
	m_image->pixmap()->resize( 0, 0 );
	m_image->update();
}

void SubtitleView::centerSubtitle()
{
	if ( !m_center ) searchCenter();
	center( m_center->x(), m_center->y() );
}

#include "subtitleview.moc"
