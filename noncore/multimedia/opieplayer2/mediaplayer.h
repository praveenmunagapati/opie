 /*
                            This file is part of the Opie Project

                             Copyright (c)  2002 Max Reiss <harlekin@handhelds.org>
                             Copyright (c)  2002 LJP <>
                             Copyright (c)  2002 Holger Freyther <zecke@handhelds.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef MEDIA_PLAYER_H
#define MEDIA_PLAYER_H

#include "xinecontrol.h"

#include "playlistwidget.h"
#include "skin.h"

class DocLnk;
class VolumeControl;
class MediaPlayerState;
class AudioWidget;
class VideoWidget;

namespace XINE
{
    class Lib;
};

class MediaPlayer : public QObject {
    Q_OBJECT
public:
    MediaPlayer( PlayListWidget &_playList, MediaPlayerState &_mediaPlayerState, QObject *parent, const char *name );
    ~MediaPlayer();

public slots:
    void reloadSkins();

private slots:
    void setPlaying( bool );
    void pauseCheck( bool );
    void play();
    void next();
    void prev();
    void startIncreasingVolume();
    void startDecreasingVolume();
    void stopChangingVolume();
    void cleanUp();
    void blank( bool );

protected:
    void timerEvent( QTimerEvent *e );
    void keyReleaseEvent( QKeyEvent *e);

private:
    AudioWidget *audioUI() const;
    VideoWidget *videoUI() const;
    XineControl *xineControl() const;

    bool isBlanked, l, r;
    int fd, fl;
    int     volumeDirection;
    VolumeControl *volControl;
    MediaPlayerState &mediaPlayerState;
    PlayListWidget &playList;

    void recreateAudioAndVideoWidgets() const;

    mutable XineControl *m_xineControl;
    mutable AudioWidget *m_audioUI;
    mutable VideoWidget *m_videoUI;
    mutable XINE::Lib *xine;

    QGuardedPtr<SkinLoader> m_skinLoader;
};


#endif // MEDIA_PLAYER_H

