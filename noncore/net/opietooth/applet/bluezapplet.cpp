/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 Maximilian Reiss <max.reiss@gmx.de>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <,   >  .   <=           redistribute it and/or  modify it under
:=1 )Y*s>-.--   :            the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; version 2 of the License.
     ._= =}       :
    .%+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|     MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>:      PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .     .:        details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=            this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/


#include "bluezapplet.h"

/* OPIE */
#include <opie2/otaskbarapplet.h>
#include <opie2/odevice.h>
#include <opie2/odebug.h>
#include <opie2/oresource.h>
#include <opie2/oprocess.h>
#include <qpe/version.h>
#include <qpe/applnk.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/config.h>
using namespace Opie::Core;

/* QT */
#include <qapplication.h>
#include <qpoint.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qtimer.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>
#include <qfile.h>
#include <qtextstream.h>

/* STD */
#include <device.h>

namespace OpieTooth {
    BluezApplet::BluezApplet( QWidget *parent, const char *name ) : QWidget( parent, name ) {
        setFixedHeight( AppLnk::smallIconSize() );
        setFixedWidth( AppLnk::smallIconSize() );

        bluezOnPixmap = OResource::loadImage( "bluetoothapplet/bluezon", OResource::SmallIcon );
        bluezOffPixmap = OResource::loadImage( "bluetoothapplet/bluezoff", Opie::Core::OResource::SmallIcon );
        bluezDiscoveryOnPixmap = OResource::loadImage( "bluetoothapplet/bluezondiscovery", Opie::Core::OResource::SmallIcon );
        bluezReceiveOnPixmap = OResource::loadImage( "bluetoothapplet/bluezonreceive", Opie::Core::OResource::SmallIcon );

        startTimer(2000);
        btDevice = 0;
        btManager = 0;
        bluezactive = false;
        bluezDiscoveryActive = false;
        bluezReceiveActive = false;
        bluezReceiveChanged = false;
        doListDevice = false;
        isScanning = false;
        m_wasOn = false;
        m_sync = false;

	// TODO: determine whether this channel has to be closed at destruction time.
        QCopChannel* chan = new QCopChannel("QPE/Bluetooth", this );
        connect(chan, SIGNAL(received(const QCString&,const QByteArray&) ),
                this, SLOT(slotMessage(const QCString&,const QByteArray&) ) );
        ::system("/etc/init.d/bluetooth stop >/dev/null 2>/dev/null");
    }

    BluezApplet::~BluezApplet() {
        if ( btDevice ) {
            ::system("/etc/init.d/bluetooth stop >/dev/null 2>/dev/null");
            delete btDevice;
        }
        if ( btManager ) {
            delete btManager;
        }
    }

    int BluezApplet::position()
    {
        return 6;
    }


    bool BluezApplet::checkBluezStatus() {
        if (btDevice) {
            if (btDevice->isLoaded() ) {
                odebug << "btDevice isLoaded" << oendl;
                return true;
            } else {
                odebug << "btDevice is NOT loaded" << oendl;
                return false;
            }
        } else {
            odebug << "btDevice is ZERO" << oendl;
            return false;
        }
    }

    int BluezApplet::setBluezStatus(int c, bool sync) {

        if ( c == 1 ) {
            QFile cfg("/etc/sysconfig/bluetooth");
            if ( cfg.open( IO_ReadOnly ) ) {
                QTextStream stream( &cfg );
                QString streamIn = stream.read();
                QStringList list = QStringList::split( "\n", streamIn );
                cfg.close();
                if ( list.grep( "BLUETOOTH_PORT=" ).count() > 0 &&
                     list.grep( "BLUETOOTH_PROTOCOL=" ).count() > 0 &&
                     list.grep( "BLUETOOTH_SPEED=" ).count() > 0)
                {
                    btDevice =
                        new Device( list.grep( "BLUETOOTH_PORT=" )[0].replace( QString( "BLUETOOTH_PORT=" ), ""),
                                    list.grep( "BLUETOOTH_PROTOCOL=" )[0].replace( QString( "BLUETOOTH_PROTOCOL=" ), ""),
                                    list.grep( "BLUETOOTH_SPEED=" )[0].replace( QString( "BLUETOOTH_SPEED=" ), "" ) );
                    return 0;
               }
            }

            // Device-specific stuff - should be removed
            switch ( ODevice::inst()->model() ) {
            case Model_iPAQ_H39xx:
                btDevice = new Device( "/dev/tts/1", "bcsp", "921600" );
                break;

            case Model_iPAQ_H5xxx:
                btDevice = new Device( "/dev/tts/1", "any", "921600" );
                break;

            case Model_MyPal_716:
                btDevice = new Device( "/dev/ttyS1", "bcsp", "921600" );
                break;

            case Model_HTC_Universal:
                btDevice = new Device( "/dev/ttyS1", "texas", "115200" );
                break;

            case Model_iPAQ_H22xx:
                btDevice = new Device( "/dev/tts/3", "any", "921600" );
                break;

            default:
                btDevice = new Device( "/dev/ttySB0", "bcsp", "230400" );
                break;
            }
            m_sync = sync;
            connect(btDevice, SIGNAL(device(const QString&, bool)),
                this, SLOT(slotDevice(const QString&, bool)));
            
        } else {
            setObexRecieveStatus(0);
            ::system("/etc/init.d/bluetooth stop >/dev/null 2>/dev/null");
            if ( btManager ) {
                delete btManager;
                btManager = 0;
            }
            if ( btDevice ) {
                delete btDevice;
                btDevice = 0;
            }
       }
        return 0;
    }

    int BluezApplet::checkBluezDiscoveryStatus() {
      return isScanning;
    }

    int BluezApplet::setBluezDiscoveryStatus(int d) {
      return bluezDiscoveryActive = d;
    }
	
    int BluezApplet::setObexRecieveStatus(int d) {
        {
    	    QCopEnvelope e ( "QPE/Obex", "btreceive(int)" );
	        e << ( d ? 1 : 0 );
        }
		bluezReceiveActive = (bool)(d != 0);
		bluezReceiveChanged = true;
		return d;
    }
    
	// FIXME mbhaynie
    // receiver for QCopChannel("QPE/Bluetooth") messages.
    void BluezApplet::slotMessage( const QCString& str, const QByteArray& )
    {
        if ( str == "enableBluetooth()") {
            m_wasOn = checkBluezStatus();
            if (!m_wasOn) {
                setBluezStatus(1, true);
                sleep(2);
            }
        }
        else if ( str == "disableBluetooth()") {
            /*
             * We can down BT only if it was started by qcop. We don't want 
             * to down BT started from menu an break our networking connection
             */
            if (checkBluezStatus() && !m_wasOn)
                setBluezStatus(0);
            doListDevice = false;
        }
        else if ( str == "listDevices()") {
            if (checkBluezStatus()) {
                doListDevice = false;
                timerEvent(0);
                if (!btManager) {
                    btManager = new Manager("hci0");
                    connect( btManager, 
                        SIGNAL( foundDevices(const QString&, RemoteDevice::ValueList) ),
                        this, SLOT( fillList(const QString&, RemoteDevice::ValueList) ) ) ;
                }
                btManager->searchDevices();
                isScanning = true;
            } else
                doListDevice = true;
        }
    }

    // Once the hcitool scan is complete, report back.
    void BluezApplet::fillList(const QString&, RemoteDevice::ValueList deviceList)
    {
        QCopEnvelope e("QPE/BluetoothBack", "devices(QStringMap)");

        QMap<QString, QString> btmap;

        RemoteDevice::ValueList::Iterator it;
        for( it = deviceList.begin(); it != deviceList.end(); ++it )
            btmap[(*it).name()] = (*it).mac();

        e << btmap;
        isScanning = false;
        timerEvent( 0 );
    }

    void BluezApplet::mousePressEvent( QMouseEvent *) {

        QPopupMenu *menu = new QPopupMenu();
        QPopupMenu *signal = new QPopupMenu();
        int ret=0;

        /* Refresh active state */
        timerEvent( 0 );


        if (bluezactive) {
            menu->insertItem( tr("Disable Bluetooth"), 0 );
        } else {
            menu->insertItem( tr("Enable Bluetooth"), 1 );
        }

        menu->insertItem( tr("Launch manager"), 2 );

        menu->insertSeparator(6);
        //menu->insertItem( tr("Signal strength"), signal,  5);
        //menu->insertSeparator(8);

        if (bluezactive) {
	        if (bluezReceiveActive) {
                menu->insertItem( tr("Disable receive"), 3 );
        	} else {
                menu->insertItem( tr("Enable receive"), 4 );
        	}
		}

        QPoint p = mapToGlobal( QPoint(1, -menu->sizeHint().height()-1) );
        ret = menu->exec(p, 0);
        menu->hide();

        switch(ret) {
        case 0:
            setBluezStatus(0);
            timerEvent( 0 );
            break;
        case 1:
            setBluezStatus(1);
            timerEvent( 0 );
            break;
        case 2:
            // start bluetoothmanager
            launchManager();
            timerEvent( 0 );
            break;
        case 3:
            setObexRecieveStatus(0);
            timerEvent( 0 );
            break;
        case 4:
            setObexRecieveStatus(1);
            timerEvent( 0 );
            break;
            //case 7:
            // With table of currently-detected devices.
        }

        delete signal;
        delete menu;
    }


/**
 * Launches the bluetooth manager
 */
    void BluezApplet::launchManager() {
        QCopEnvelope e("QPE/System", "execute(QString)");
        e << QString("bluetooth-manager");
    }

/**
 * Refresh timer
 * @param the timer event
 */
    void BluezApplet::timerEvent( QTimerEvent * ) {
        bool oldactive = bluezactive;
        int olddiscovery = bluezDiscoveryActive;

        bluezactive = checkBluezStatus();
        bluezDiscoveryActive = checkBluezDiscoveryStatus();

        if ((bluezactive != oldactive) || 
            (bluezDiscoveryActive != olddiscovery) ||
			bluezReceiveChanged)
            update();
        if (bluezactive && doListDevice) {
            const QByteArray arr;
            slotMessage("listDevices()", arr);
        }
		if (bluezReceiveChanged)
			bluezReceiveChanged = false;
    }

/**
 * Implementation of the paint event
 * @param the QPaintEvent
 */
    void BluezApplet::paintEvent( QPaintEvent* ) {
        QPainter p(this);
        odebug << "paint bluetooth pixmap" << oendl; 

        if (bluezactive) {
            p.drawPixmap( 0, 0,  bluezOnPixmap );
        } else {
            p.drawPixmap( 0, 0,  bluezOffPixmap );
        }

        if (bluezDiscoveryActive) {
            p.drawPixmap( 0, 0,  bluezDiscoveryOnPixmap );
        }
        if (bluezReceiveActive) {
            p.drawPixmap( 0, 0,  bluezReceiveOnPixmap );
        }
    }

    /**
    * Reacts on device up
    * @param name device name
    * @param up if device was brought up
    */
    void BluezApplet::slotDevice(const QString& name, bool up)
    {
        if (!up)
            return;
        odebug << name << " is up" << oendl;
        if (m_sync) {
            ::system("/etc/init.d/bluetooth start >/dev/null 2>/dev/null");
        } else {
            QCopEnvelope e("QPE/System", "execute(QString)");
            e << QString("/etc/init.d/bluetooth start");
        }
    }
};


EXPORT_OPIE_APPLET_v1( OpieTooth::BluezApplet )

