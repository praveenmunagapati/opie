/****************************************************************************
** $Id: quuid.cpp,v 1.1 2002-11-01 00:10:45 kergoth Exp $
**
** Implementation of QUuid class
**
** Copyright (C) 2000-2001 Trolltech AS.  All rights reserved.
**
** This file is part of the tools module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "quuid.h"

#include <qdatastream.h>

/*!
  \class QUuid quuid.h
  \reentrant
  \brief The QUuid class defines a Universally Unique Identifier (UUID).

  \internal

  For objects or declarations that need to be identified uniquely, UUIDs (also known as GUIDs) are widely
  used in order to assign a fixed and easy to compare value to this object or declaration. The 128bit value
  of an UUID is generated by an algorithm that guarantees a value that is unique in time and space.

  In Qt, UUIDs are wrapped by the QUuid struct which provides convenience functions for comparing and coping
  this value. The QUuid struct is used in Qt's component model to identify interfaces. Most platforms provide a tool to
  generate new UUIDs (uuidgen, guidgen), and the Qt distribution includes a graphical tool \e quuidgen that generates
  the UUIDs in a programmer friendly format.

  \sa QUnknownInterface
*/

/*!
  \fn QUuid::QUuid()

  Creates the null UUID {00000000-0000-0000-0000-000000000000}.
*/

/*!
  \fn QUuid::QUuid( uint l, ushort w1, ushort w2, uchar b1, uchar b2, uchar b3, uchar b4, uchar b5, uchar b6, uchar b7, uchar b8 )

  Creates an UUID with the value specified by the parameters, \a l, \a
  w1, \a w2, \a b1, \a b2, \a b3, \a b4, \a b5, \a b6, \a b7, \a b8.

  Example:
  \code
  // {67C8770B-44F1-410A-AB9A-F9B5446F13EE}
  QUuid IID_MyInterface( 0x67c8770b, 0x44f1, 0x410a, 0xab, 0x9a, 0xf9, 0xb5, 0x44, 0x6f, 0x13, 0xee )
  \endcode
*/

/*!
  \fn QUuid::QUuid( const QUuid &orig )

  Creates a copy of the QUuid \a orig.
*/
#ifndef QT_NO_QUUID_STRING
/*!
  Creates a QUuid object from the string \a text. Right now, the function
  can only convert the format {12345678-1234-1234-1234-123456789ABC} and
  will create the null UUID when the conversion fails.
*/
QUuid::QUuid( const QString &text )
{
    bool ok;
    QString temp = text.upper();

    data1 = temp.mid( 1, 8 ).toULong( &ok, 16 );
    if ( !ok ) {
	*this = QUuid();
	return;
    }

    data2 = temp.mid( 10, 4 ).toUInt( &ok, 16 );
    if ( !ok ) {
	*this = QUuid();
	return;
    }
    data3 = temp.mid( 15, 4 ).toUInt( &ok, 16 );
    if ( !ok ) {
	*this = QUuid();
	return;
    }
    data4[0] = temp.mid( 20, 2 ).toUInt( &ok, 16 );
    if ( !ok ) {
	*this = QUuid();
	return;
    }
    data4[1] = temp.mid( 22, 2 ).toUInt( &ok, 16 );
    if ( !ok ) {
	*this = QUuid();
	return;
    }
    for ( int i = 2; i<8; i++ ) {
	data4[i] = temp.mid( 25 + (i-2)*2, 2 ).toUShort( &ok, 16 );
	if ( !ok ) {
	    *this = QUuid();
	    return;
	}
    }
}

/*!
    \overload
*/
QUuid::QUuid( const char *text )
{
    *this = QUuid( QString(text) );
}
#endif
/*!
  \fn QUuid QUuid::operator=(const QUuid &uuid )

  Assigns the value of \a uuid to this QUuid object.
*/

/*!
  \fn bool QUuid::operator==(const QUuid &other) const

  Returns TRUE if this QUuid and the \a other QUuid are identical, otherwise returns FALSE.
*/

/*!
  \fn bool QUuid::operator!=(const QUuid &other) const

  Returns TRUE if this QUuid and the \a other QUuid are different, otherwise returns FALSE.
*/
#ifndef QT_NO_QUUID_STRING
/*!
  \fn QUuid::operator QString() const

  Returns the string representation of the uuid.

  \sa toString()
*/

/*!
  QString QUuid::toString() const

  Returns the string representation of the uuid.
*/
QString QUuid::toString() const
{
    QString result;

    result = "{" + QString::number( data1, 16 ).rightJustify( 8, '0' ) + "-";
    result += QString::number( (int)data2, 16 ).rightJustify( 4, '0' ) + "-";
    result += QString::number( (int)data3, 16 ).rightJustify( 4, '0' ) + "-";
    result += QString::number( (int)data4[0], 16 ).rightJustify( 2, '0' );
    result += QString::number( (int)data4[1], 16 ).rightJustify( 2, '0' ) + "-";
    for ( int i = 2; i < 8; i++ )
	result += QString::number( (int)data4[i], 16 ).rightJustify( 2, '0' );

    return result + "}";
}
#endif

#ifndef QT_NO_DATASTREAM
/*!
  \relates QUuid
  Writes the \a id to the datastream \a s.
*/
QDataStream &operator<<( QDataStream &s, const QUuid &id )
{
    s << (Q_UINT32)id.data1;
    s << (Q_UINT16)id.data2;
    s << (Q_UINT16)id.data3;
    for (int i = 0; i < 8; i++ )
	s << (Q_UINT8)id.data4[i];
    return s;
}

/*!
  \relates QUuid
  Reads a universally unique id from from the stream \a s into \a id.
*/
QDataStream &operator>>( QDataStream &s, QUuid &id )
{
    Q_UINT32 u32;
    Q_UINT16 u16;
    Q_UINT8 u8;
    s >> u32;
    id.data1 = u32;
    s >> u16;
    id.data2 = u16;
    s >> u16;
    id.data3 = u16;
    for (int i = 0; i < 8; i++ ) {
	s >> u8;
	id.data4[i] = u8;
    }
    return s;
}
#endif

/*!
  Returns TRUE if this is the null UUID {00000000-0000-0000-0000-000000000000}, otherwise returns FALSE.
*/
bool QUuid::isNull() const
{
    return data4[0] == 0 && data4[1] == 0 && data4[2] == 0 && data4[3] == 0 &&
	   data4[4] == 0 && data4[5] == 0 && data4[6] == 0 && data4[7] == 0 &&
	   data1 == 0 && data2 == 0 && data3 == 0;
}
