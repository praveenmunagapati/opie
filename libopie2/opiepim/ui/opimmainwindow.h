/*
                             This file is part of the Opie Project
                             Copyright (C) Stefan Eilers (Eilers.Stefan@epost.de)
              =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/
#ifndef OPIE_PIM_MAINWINDOW_H
#define OPIE_PIM_MAINWINDOW_H

#include <qmainwindow.h>

#include <opie2/opimrecord.h>
class QCopChannel;
class QDateTime;

namespace Opie {
/**
 * This is a common Opie PIM MainWindow
 * it takes care of the QCOP internals
 * and implements some  functions
 * for the URL scripting schema
 */
/*
 * due Qt and Templates with signal and slots
 * do not work that good :(
 * (Ok how to moc a template ;) )
 * We will have the mainwindow which calls a struct which
 * is normally reimplemented as a template ;)
 */

class OPimMainWindow : public QMainWindow {
    Q_OBJECT
public:
    enum TransPort { BlueTooth=0,
                     IrDa };

    OPimMainWindow(  const QString& service, QWidget *parent = 0,  const char* name = 0,
                     WFlags f = WType_TopLevel);
    virtual ~OPimMainWindow();


protected slots:
    /*
     * called when a setDocument
     * couldn't be handled by this window
     */
    virtual void doSetDocument( const QString& );
    /* for syncing */
    virtual void flush()  = 0;
    virtual void reload() = 0;

    /** create a new Records and return the uid */
    virtual int create() = 0;
    /** remove a record with UID == uid */
    virtual bool remove( int uid ) = 0;
    /** beam the record with UID = uid */
    virtual void beam( int uid ) = 0;

    /** show the record with UID == uid */
    virtual void show( int uid ) = 0;
    /** edit the record */
    virtual void edit( int uid ) = 0;

    /** make a copy of it! */
    virtual void add( const OPimRecord& ) = 0;

    virtual void doAlarm( const QDateTime&, int uid );

    QCopChannel* channel();

protected:
    /**
     * start to play soundAlarm()
     * @param count How many times the alarm is played
     */
    void startAlarm(int count = 10);
    void killAlarm();
    void timerEvent( QTimerEvent* );

private slots:
    void appMessage( const QCString&, const QByteArray& );
    void setDocument( const QString& );


private:
    class Private;
    Private* d;

    int m_rtti;
    QCopChannel* m_channel;
    QString m_service;
    QCString m_str;
    OPimRecord* m_fallBack;
    int m_alarmCount;
    int m_playedCount;
    int m_timerId;
    /* I would love to do this as a template
     * but can't think of a right way
     * because I need signal and slots -zecke
     */
    virtual OPimRecord* record( int rtti, const QByteArray& ) ;
    int service();
};

}

#endif
