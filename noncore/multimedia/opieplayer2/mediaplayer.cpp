#include <qpe/qpeapplication.h>
#include <qpe/qlibrary.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <qpe/qcopenvelope_qws.h>
#include <qfileinfo.h>

#include <qmainwindow.h>
#include <qmessagebox.h>
#include <qwidgetstack.h>
#include <qfile.h>

#include "mediaplayer.h"
#include "playlistwidget.h"
#include "audiowidget.h"
#include "videowidget.h"
#include "volumecontrol.h"

#include "mediaplayerstate.h"

// for setBacklight()
#include <linux/fb.h>
#include <sys/file.h>
#include <sys/ioctl.h>


extern AudioWidget *audioUI;
extern VideoWidget *videoUI;
extern PlayListWidget *playList;
extern MediaPlayerState *mediaPlayerState;


#define FBIOBLANK             0x4611

MediaPlayer::MediaPlayer( QObject *parent, const char *name )
    : QObject( parent, name ), volumeDirection( 0 ), currentFile( NULL ) {

    fd=-1;fl=-1;
    playList->setCaption( tr( "OpiePlayer: Initializating" ) );

    qApp->processEvents();
    //    QPEApplication::grabKeyboard(); // EVIL
    connect( qApp,SIGNAL( aboutToQuit()),SLOT( cleanUp()) );

    connect( mediaPlayerState, SIGNAL( playingToggled( bool ) ), this, SLOT( setPlaying( bool ) ) );

    connect( mediaPlayerState, SIGNAL( pausedToggled( bool ) ),  this, SLOT( pauseCheck( bool ) ) );

    connect( mediaPlayerState, SIGNAL( next() ), this, SLOT( next() ) );
    connect( mediaPlayerState, SIGNAL( prev() ), this, SLOT( prev() ) );
    connect( mediaPlayerState, SIGNAL( blankToggled( bool ) ), this, SLOT ( blank( bool ) ) );

    connect( audioUI,  SIGNAL( moreClicked() ), this, SLOT( startIncreasingVolume() ) );
    connect( audioUI,  SIGNAL( lessClicked() ),  this, SLOT( startDecreasingVolume() ) );
    connect( audioUI,  SIGNAL( moreReleased() ), this, SLOT( stopChangingVolume() ) );
    connect( audioUI,  SIGNAL( lessReleased() ), this, SLOT( stopChangingVolume() ) );

    connect( videoUI,  SIGNAL( moreClicked() ), this, SLOT( startIncreasingVolume() ) );
    connect( videoUI,  SIGNAL( lessClicked() ),  this, SLOT( startDecreasingVolume() ) );
    connect( videoUI,  SIGNAL( moreReleased() ), this, SLOT( stopChangingVolume() ) );
    connect( videoUI,  SIGNAL( lessReleased() ), this, SLOT( stopChangingVolume() ) );

    volControl = new VolumeControl;
    xineControl = new XineControl();
    playList->setCaption(tr("OpiePlayer"));
}

MediaPlayer::~MediaPlayer() {
    delete xineControl;
    delete volControl;
}

void MediaPlayer::pauseCheck( bool b ) {
     if ( b && !mediaPlayerState->playing() ) {
         mediaPlayerState->setPaused( FALSE );
     }
}

void MediaPlayer::play() {
    mediaPlayerState->setPlaying( FALSE );
    mediaPlayerState->setPlaying( TRUE );
}

void MediaPlayer::setPlaying( bool play ) {
    if ( !play ) {
        return;
    }

    if ( mediaPlayerState->paused() ) {
        mediaPlayerState->setPaused( FALSE );
        return;
    }

    QString tickerText, time, fileName;
    if( playList->whichList() == 0 ) { //check for filelist
        const DocLnk *playListCurrent = playList->current();
        if ( playListCurrent != NULL ) {
            currentFile = playListCurrent;
        }
        xineControl->play( currentFile->file() );
        fileName = currentFile->name();
        long seconds =  mediaPlayerState->length();//
        time.sprintf("%li:%02i", seconds/60, (int)seconds%60 );
        //qDebug(time);

    } else {
        //if playing in file list.. play in a different way
        // random and looping settings enabled causes problems here,
        // since there is no selected file in the playlist, but a selected file in the file list,
        // so we remember and shutoff
        l = mediaPlayerState->looping();
        if(l) {
            mediaPlayerState->setLooping( false );
        }
        r = mediaPlayerState->shuffled();
        mediaPlayerState->setShuffled( false );

        fileName = playList->currentFileListPathName();
        xineControl->play( fileName );
        long seconds =  mediaPlayerState->length();
        time.sprintf("%li:%02i", seconds/60, (int)seconds%60 );
        //qDebug(time);
        if( fileName.left(4) != "http" ) {
            fileName = QFileInfo( fileName ).baseName();
        }

    }

    if( fileName.left(4) == "http" ) {
       if ( xineControl->getMetaInfo().isEmpty() ) {
           tickerText = tr( " File: " ) + fileName;
       } else {
           tickerText = xineControl->getMetaInfo();
       }
    } else {
        if ( xineControl->getMetaInfo().isEmpty() ) {
            tickerText = tr( " File: " ) + fileName + tr( ", Length: " ) + time + "  ";
        } else {
            tickerText = xineControl->getMetaInfo() + " Length: " + time + "  ";
        }
    }
    audioUI->setTickerText( tickerText );
}


void MediaPlayer::prev() {
    if( playList->whichList() == 0 ) { //if using the playlist
        if ( playList->prev() ) {
            play();
        } else if ( mediaPlayerState->looping() ) {
            if ( playList->last() ) {
                play();
            }
        } else {
            mediaPlayerState->setList();
        }
    }
}


void MediaPlayer::next() {

    if(playList->whichList() == 0) { //if using the playlist
        if ( playList->next() ) {
            play();
        } else if ( mediaPlayerState->looping() ) {
            if ( playList->first() ) {
                play();
            }
        } else {
            mediaPlayerState->setList();
        }
    } else { //if playing from file list, let's just stop
        qDebug("<<<<<<<<<<<<<<<<<stop for filelists");
        mediaPlayerState->setPlaying(false);
        mediaPlayerState->setView('l');
        if(l) mediaPlayerState->setLooping(l);
        if(r) mediaPlayerState->setShuffled(r);
    }
    qApp->processEvents();
}


void MediaPlayer::startDecreasingVolume() {
    volumeDirection = -1;
    startTimer( 100 );
    volControl->decVol(2);
}


void MediaPlayer::startIncreasingVolume() {
    volumeDirection = +1;
    startTimer( 100 );
    volControl->incVol(2);
}


bool drawnOnScreenDisplay = FALSE;
unsigned int onScreenDisplayVolume = 0;
const int yoff = 110;

void MediaPlayer::stopChangingVolume() {
    killTimers();
    // Get rid of the on-screen display stuff
    drawnOnScreenDisplay = FALSE;
    onScreenDisplayVolume = 0;
    int w=0;
    int h=0;
    if( !xineControl->hasVideo() ) {
        w = audioUI->width();
        h = audioUI->height();
        audioUI->repaint( ( w - 200 ) / 2, h - yoff, 200 + 9, 70, FALSE );
    } else {
        w = videoUI->width();
        h = videoUI->height();
        videoUI->repaint( ( w - 200 ) / 2, h - yoff, 200 + 9, 70, FALSE );
    }
}


void MediaPlayer::timerEvent( QTimerEvent * ) {
    if ( volumeDirection == +1 ) {
        volControl->incVol( 2 );
    } else if ( volumeDirection == -1 ) {
        volControl->decVol( 2 );
    }


      // TODO FIXME
      // huh??
    unsigned int v= 0;
    v = volControl->volume();
    v = v / 10;

    if ( drawnOnScreenDisplay && onScreenDisplayVolume == v ) {
        return;
    }

    int w=0; int h=0;
    if( !xineControl->hasVideo() ) {
        w = audioUI->width();
        h = audioUI->height();

        if ( drawnOnScreenDisplay ) {
            if ( onScreenDisplayVolume > v ) {
                audioUI->repaint( ( w - 200 ) / 2 + v * 20 + 0, h - yoff + 40, ( onScreenDisplayVolume - v ) * 20 + 9, 30, FALSE );
            }
        }
        drawnOnScreenDisplay = TRUE;
        onScreenDisplayVolume = v;
        QPainter p( audioUI );
        p.setPen( QColor( 0x10, 0xD0, 0x10 ) );
        p.setBrush( QColor( 0x10, 0xD0, 0x10 ) );

        QFont f;
        f.setPixelSize( 20 );
        f.setBold( TRUE );
        p.setFont( f );
        p.drawText( (w - 200) / 2, h - yoff + 20, tr("Volume") );

        for ( unsigned int i = 0; i < 10; i++ ) {
            if ( v > i ) {
                p.drawRect( ( w - 200 ) / 2 + i * 20 + 0, h - yoff + 40, 9, 30 );
            } else {
                p.drawRect( ( w - 200 ) / 2 + i * 20 + 3, h - yoff + 50, 3, 10 );
            }
        }
    } else {
        w = videoUI->width();
        h = videoUI->height();

        if ( drawnOnScreenDisplay ) {
            if ( onScreenDisplayVolume > v ) {
                videoUI->repaint( (w - 200) / 2 + v * 20 + 0, h - yoff + 40, ( onScreenDisplayVolume - v ) * 20 + 9, 30, FALSE );
            }
        }
        drawnOnScreenDisplay = TRUE;
        onScreenDisplayVolume = v;
        QPainter p( videoUI );
        p.setPen( QColor( 0x10, 0xD0, 0x10 ) );
        p.setBrush( QColor( 0x10, 0xD0, 0x10 ) );

        QFont f;
        f.setPixelSize( 20 );
        f.setBold( TRUE );
        p.setFont( f );
        p.drawText( (w - 200) / 2, h - yoff + 20, tr( "Volume" ) );

        for ( unsigned int i = 0; i < 10; i++ ) {
            if ( v > i ) {
                p.drawRect( (w - 200) / 2 + i * 20 + 0, h - yoff + 40, 9, 30 );
            } else {
                p.drawRect( (w - 200) / 2 + i * 20 + 3, h - yoff + 50, 3, 10 );
            }
        }
    }
}


void MediaPlayer::blank( bool b ) {
    fd=open("/dev/fb0",O_RDWR);
#ifdef QT_QWS_EBX
    fl= open( "/dev/fl", O_RDWR );
#endif
    if (fd != -1) {
        if ( b ) {
            qDebug("do blanking");
#ifdef QT_QWS_EBX
            ioctl( fd, FBIOBLANK, 1 );
            if(fl !=-1) {
                ioctl( fl, 2 );
                ::close(fl);
            }
#else
            ioctl( fd, FBIOBLANK, 3 );
#endif
            isBlanked = TRUE;
        } else {
            qDebug("do unblanking");
            ioctl( fd, FBIOBLANK, 0);
#ifdef QT_QWS_EBX
            if(fl != -1) {
                ioctl( fl, 1);
                ::close(fl);
            }
#endif
            isBlanked = FALSE;
        }
        close( fd );
    } else {
        qDebug("<< /dev/fb0 could not be opened  >>");
    }
}

void MediaPlayer::keyReleaseEvent( QKeyEvent *e) {
    switch ( e->key() ) {
////////////////////////////// Zaurus keys
      case Key_Home:
          break;
      case Key_F9: //activity
          break;
      case Key_F10: //contacts
          break;
      case Key_F11: //menu
          break;
      case Key_F12: //home
          qDebug("Blank here");
//          mediaPlayerState->toggleBlank();
          break;
      case Key_F13: //mail
          qDebug("Blank here");
          //  mediaPlayerState->toggleBlank();
          break;
    }
}

void MediaPlayer::cleanUp() {// this happens on closing
     Config cfg( "OpiePlayer" );
     mediaPlayerState->writeConfig( cfg );
     playList->writeDefaultPlaylist( );

//     QPEApplication::grabKeyboard();
//     QPEApplication::ungrabKeyboard();
}
