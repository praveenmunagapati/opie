/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include <qglobal.h>
#include <qtopia/timeconversion.h>
#include <qregexp.h>
#include <stdlib.h>

QString TimeConversion::toString( const QDate &d )
{
// 	QString empty;
// 	if ( d.isNull() )
// 		return empty;

	QString r = QString::number( d.day() ) + "." +
		QString::number( d.month() ) + "." +
		QString::number( d.year() );
	//qDebug("TimeConversion::toString %s", r.latin1());

	return r;
}

QDate TimeConversion::fromString( const QString &datestr )
{
//     QDate empty;
//     if ( datestr.isEmpty() )
// 	    return empty;

    int monthPos = datestr.find('.');
    int yearPos = datestr.find('.', monthPos+1 );
    if ( monthPos == -1 || yearPos == -1 ) {
	qDebug("fromString didn't find . in str = %s; mpos = %d ypos = %d", datestr.latin1(), monthPos, yearPos );
	return QDate();
    }
    int d = datestr.left( monthPos ).toInt();
    int m = datestr.mid( monthPos+1, yearPos - monthPos - 1 ).toInt();
    int y = datestr.mid( yearPos+1 ).toInt();
    QDate date ( y,m,d );
    //qDebug("TimeConversion::fromString ymd = %s => %d %d %d; mpos = %d ypos = %d", datestr.latin1(), y, m, d, monthPos, yearPos);
    return date;
}

time_t TimeConversion::toUTC( const QDateTime& dt )
{
    time_t tmp;
    struct tm *lt;

#if defined(_OS_WIN32) || defined (Q_OS_WIN32) || defined (Q_OS_WIN64)
    _tzset();
#else
    tzset();
#endif

    // get a tm structure from the system to get the correct tz_name
    tmp = time( 0 );
    lt = localtime( &tmp );

    lt->tm_sec = dt.time().second();
    lt->tm_min = dt.time().minute();
    lt->tm_hour = dt.time().hour();
    lt->tm_mday = dt.date().day();
    lt->tm_mon = dt.date().month() - 1; // 0-11 instead of 1-12
    lt->tm_year = dt.date().year() - 1900; // year - 1900
    //lt->tm_wday = dt.date().dayOfWeek(); ignored anyway
    //lt->tm_yday = dt.date().dayOfYear(); ignored anyway
    lt->tm_wday = -1;
    lt->tm_yday = -1;
    // tm_isdst negative -> mktime will find out about DST
    lt->tm_isdst = -1;
    // keep tm_zone and tm_gmtoff
    tmp = mktime( lt );
    return tmp;
}

QDateTime TimeConversion::fromUTC( time_t time )
{
    struct tm *lt;

#if defined(_OS_WIN32) || defined (Q_OS_WIN32) || defined (Q_OS_WIN64)
    _tzset();
#else
    tzset();
#endif
    lt = localtime( &time );
    QDateTime dt;
    dt.setDate( QDate( lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday ) );
    dt.setTime( QTime( lt->tm_hour, lt->tm_min, lt->tm_sec ) );
    return dt;
}


int TimeConversion::secsTo( const QDateTime &from, const QDateTime &to )
{
    return toUTC( to ) - toUTC( from );
}

QCString TimeConversion::toISO8601( const QDate &d )
{
    time_t tmp = toUTC( d );
    struct tm *utc = gmtime( &tmp );

    QCString str;
    str.sprintf("%04d%02d%02d", (utc->tm_year + 1900), utc->tm_mon+1, utc->tm_mday );
    return str;
}

QCString TimeConversion::toISO8601( const QDateTime &dt )
{
    time_t tmp = toUTC( dt );
    struct tm *utc = gmtime( &tmp );

    QCString str;
    str.sprintf("%04d%02d%02dT%02d%02d%02dZ",
		(utc->tm_year + 1900), utc->tm_mon+1, utc->tm_mday,
		utc->tm_hour, utc->tm_min, utc->tm_sec );
    return str;
}

QDateTime TimeConversion::fromISO8601( const QCString &s )
{

#if defined(_OS_WIN32) || defined (Q_OS_WIN32) || defined (Q_OS_WIN64)
    _tzset();
#else
    tzset();
#endif

    struct tm thetime;

    QCString str = s.copy();
    str.replace(QRegExp("-"), "" );
    str.replace(QRegExp(":"), "" );
    str.stripWhiteSpace();
    str = str.lower();

    int i = str.find( "t" );
    QCString date;
    QCString timestr;
    if ( i != -1 ) {
	date = str.left( i );
	timestr = str.mid( i+1 );
    } else {
	date = str;
    }

//     qDebug("--- parsing ISO time---");
    memset( &thetime, 0, sizeof(tm) );
    thetime.tm_year = 100;
    thetime.tm_mon = 0;
    thetime.tm_mday = 0;
    thetime.tm_hour = 0;
    thetime.tm_min = 0;
    thetime.tm_sec = 0;

//     qDebug("date = %s", date.data() );

    switch( date.length() ) {
	case 8:
	case 6:
	case 4:
	    if( date.length() == 8 )
	        thetime.tm_mday = date.right( 2 ).toInt();

	    if( date.length() >= 6 )
	        thetime.tm_mon = date.mid( 4, 2 ).toInt() - 1;

	    thetime.tm_year = date.left( 4 ).toInt();
	    thetime.tm_year -= 1900;
	    break;
	default:
	    break;
    }

    int tzoff = 0;
    bool inLocalTime = FALSE;
    if ( timestr.find( 'z' ) == (int)timestr.length() - 1 )
	// UTC
	timestr = timestr.left( timestr.length() -1 );
    else {
	int plus = timestr.find( "+" );
	int minus = timestr.find( "-" );
	if ( plus != -1 || minus != -1 ) {
	    // have a timezone offset
	    plus = (plus != -1) ? plus : minus;
	    QCString off = timestr.mid( plus );
	    timestr = timestr.left( plus );

	    int tzoffhour = 0;
	    int tzoffmin = 0;
	    switch( off.length() ) {
		case 5:
		case 3:
		    if ( off.length() == 5)
		        tzoffmin = off.mid(3).toInt();

		    tzoffhour = off.left(3).toInt();
		default:
		    break;
	    }
	    tzoff = 60*tzoffhour + tzoffmin;
	} else
	    inLocalTime = TRUE;
    }

    // get the time:
    switch( timestr.length() ) {
	case 6:
	case 4:
	case 2:
	    if( timestr.length() == 6 )
	        thetime.tm_sec = timestr.mid( 4 ).toInt();

	    if( timestr.length() >= 4 )
	        thetime.tm_min = timestr.mid( 2, 2 ).toInt();

	    thetime.tm_hour = timestr.left( 2 ).toInt();
	default:
	    break;
    }

    int tzloc = 0;
    time_t tmp = time( 0 );
    if ( !inLocalTime ) {
	// have to get the offset between gmt and local time
	struct tm *lt = localtime( &tmp );
	tzloc = mktime( lt );
	struct tm *ut = gmtime( &tmp );
	tzloc -= mktime( ut );
    }
//     qDebug("time: %d %d %d, tzloc=%d, tzoff=%d", thetime->tm_hour, thetime->tm_min, thetime->tm_sec,
//  	   tzloc, tzoff );

    tmp = mktime( &thetime );
    tmp += 60*(-tzloc + tzoff);



    return fromUTC( tmp );
}

