/*
 * cardmon.h
 *
 * ---------------------
 *
 * copyright   : (c) 2003 by Maximilian Reiss
 * email       : max.reiss@gmx.de
 * based on two apps by Devin Butterfield
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CARDMON_H
#define CARDMON_H

#include <qwidget.h>
#include <qpixmap.h>
#include <qpopupmenu.h>

namespace Opie {
    namespace Core {
        class OProcess;
    }
}

class CardMonitor : public QWidget {
    Q_OBJECT
public:
    CardMonitor( QWidget *parent = 0 );
    ~CardMonitor();
    bool getStatusPcmcia( int showPopUp = FALSE );
    bool getStatusSd( int showPopUp = FALSE );
    static int position();
private slots:
    void cardMessage( const QCString &msg, const QByteArray & );
    void slotExited( Opie::Core::OProcess* proc );
    void popupTimeout();

protected:
    void paintEvent( QPaintEvent* );
    void mousePressEvent( QMouseEvent * );

private:
    void execCommand( const QString &command );
    int m_commandOrig;
    QPixmap pm;
    // pcmcia socket 0
    bool cardInPcmcia0;
    QString cardInPcmcia0Name;
    QString cardInPcmcia0Type;
    // pcmcia socket 1
    bool cardInPcmcia1;
    QString cardInPcmcia1Name;
    QString cardInPcmcia1Type;
    bool cardInSd;
    QString cardSdName; // the device which is mounted
    void iconShow();
    QPopupMenu *popupMenu;
    void popUp(QString message, QString icon = QString::null );
    Opie::Core::OProcess* m_process;
};

#endif

