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

#include "project.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <ktempdir.h>
#include <kdebug.h>
#include "xmlwriter.h"
#include <qvariant.h>
#include <qxml.h>

Colours::Colours() {
	m_colours[0] = 255;
	m_colours[1] = 255;
	m_colours[2] = 0;
	m_colours[3] = 255;
}

uchar& Colours::operator[](uint index) {
	if ( index > 3 )
		kdFatal() << "Colours: Index out of bounds\n";

	return m_colours[index];
}

const uchar& Colours::operator[](uint index) const {
	if ( index > 3 )
		kdFatal() << "Colours: Index out of bounds\n";

	return m_colours[index];
}

class ProjectParser : public QXmlDefaultHandler
{
	public:
		ProjectParser( Project* prj ) {
			if ( !prj ) kdFatal() << "ProjectParser constructor: prj is null\n";
			m_prj = prj;
		}

		bool startDocument() {
			m_inProject = m_inFiles = m_inUrl = false;
			return true;
		}

		bool endDocument() {
			m_prj->setFiles( m_files );
			return true;
		}

		bool endElement( const QString&, const QString&, const QString &name ) {
			if( name == "ksubtitleripper_project" )
				m_inProject = false;
			else if ( name == "files" )
				m_inFiles = false;
			else if ( name == "url" )
				m_inUrl = false;

			return true;
		}

		bool startElement( const QString&, const QString&, const QString &name, const QXmlAttributes &attrs ) {
			if( m_inProject ) {
				if ( m_inFiles ) {
					if ( name == "url" ) m_inUrl = true;
				} else {
					if ( name == "files" ) m_inFiles = true;
					else if ( name == "params" ) fillProjectParams( attrs );
					else if ( name == "colours" ) {
						uint n;
						bool success;
						for( uint i=0; i < 4; ++i ) {
							n = attrs.value( QString( "colour%1" ).arg( i+1 ) ).toUInt( &success );
							if ( success ) m_prj->colours[i] = n;
						}
					}
				}
			} else if( name == "ksubtitleripper_project" )
				m_inProject = true;

			return true;
		}

		bool characters ( const QString & ch ) {
			if ( m_inUrl ) {
				KURL url = ch;
				if ( !url.isEmpty() && url.isValid() ) m_files.append(url);
			}
			return true;
		}

	private:
		bool m_inProject, m_inFiles, m_inUrl;
		KURL::List m_files;
		Project* m_prj;

		void fillProjectParams( const QXmlAttributes& attrs ) {
			uint n;
			bool success;
			QString aux;

			for( int i=0; i < attrs.count(); ++i ) {
				if( attrs.localName( i ) == "extracted" ) {
					n = attrs.value( i ).toUInt( &success );
					if ( success ) m_prj->setExtracted( n );

				} else if( attrs.localName( i ) == "converted" ) {
					n = attrs.value( i ).toUInt( &success );
					if ( success ) m_prj->setConverted( n );

				} else if( attrs.localName( i ) == "currentsubtitle" ) {
					n = attrs.value( i ).toUInt( &success );
					if ( success ) m_prj->goSub( n );

				} else if( attrs.localName( i ) == "numbersubtitles" ) {
					n = attrs.value( i ).toUInt( &success );
					if ( success ) m_prj->setNumSub( n );

				} else if( attrs.localName( i ) == "directory" ) {
					aux = attrs.value( i );
					if ( !aux.isEmpty() ) m_prj->setDirectory( aux );

				} else if( attrs.localName( i ) == "basename" ) {
					aux = attrs.value( i );
					if ( !aux.isEmpty() ) m_prj->setBaseName( aux );
				}
			}
		}
};

Project::Project( const QString& path, bool& success ) {
	init();

	QFile f( path );
	QXmlInputSource source( f );
	QXmlSimpleReader reader;
	ProjectParser parser( this );

	reader.setContentHandler( &parser );
	success = reader.parse( source );
}

QString Project::subFilename( int sub ) {
	QString number = QString::number( sub );

	if ( number.length() < 4 )
		return m_baseName + QString().fill( '0', 4-number.length() ) + number;
	else return m_baseName + number;
}

bool Project::save( const QString& path ) const {
	QFile f( path );
	QTextStream out( &f );
	if ( !f.open ( IO_WriteOnly ) ) return false;
	XmlWriter xml( &f );
	xml.setAutoNewLine( true );
	xml.writeOpenTag( "ksubtitleripper_project" );

	AttrMap attrs;
	attrs.insert( "directory", m_directory );
	attrs.insert( "basename", m_baseName );
	attrs.insert( "numbersubtitles", m_numSub );
	attrs.insert( "currentsubtitle", m_currentSub );
	attrs.insert( "extracted", m_extracted );
	attrs.insert( "converted", m_converted );
	xml.writeAtomTag( "params", attrs );

	xml.writeOpenTag( "files" );
	for ( uint i = 0; i < m_files.count(); i++ )
		xml.writeTaggedString( "url", m_files[i].url() );
	xml.writeCloseTag( "files" );

	attrs.clear();
	for ( uint i = 0; i < 4; i++ )
		attrs.insert( QString( "colour%1" ).arg( i+1 ), uint( colours[i] ) );
	xml.writeAtomTag( "colours", attrs );

	xml.writeCloseTag( "ksubtitleripper_project" );
	f.close();
	return true;
}

void Project::setFiles( const KURL::List& list ) {
	m_files = list;
	if ( m_directory.isEmpty() ) {
		if ( list[0].isLocalFile() ) m_directory = list[0].directory( false );
		else m_directory = KTempDir().name();
	}
}

QString Project::coloursString() const {
	return QString::number(colours[0]) + "," + QString::number(colours[1]) + "," +
			QString::number(colours[2]) + "," + QString::number(colours[3]);
}
