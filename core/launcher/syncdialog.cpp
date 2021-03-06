/*
                             This file is part of the Opie Project
              =.             (C) 2000-2002 Trolltech AS
            .=l.             (C) 2002-2005 The Opie Team <opie-devel@lists.sourceforge.net>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
    :`=1 )Y*s>-.--   :       the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; version 2 of the License.
     ._= =}       :
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
#include "syncdialog.h"

/* OPIE */
#include <opie2/oresource.h>
using namespace Opie::Core;

/* QT */
#include <qpainter.h>
#include <qapplication.h>
#include <qpushbutton.h>
#include <qfile.h>

SyncDialog::SyncDialog( QWidget *parent, const QString &w )
    : QDialog( parent, "SyncDialog", FALSE, WStyle_Tool | WStyle_Customize |
	WStyle_StaysOnTop ), what(w), nextPt(0), rev(FALSE), hideDot(TRUE)
{
    QFont f( font() );
    f.setPointSize( 16 );
    setFont(f);

    loadPath();

    QSize ds = qApp->desktop()->size();
    setGeometry( 0, 0, ds.width(), ds.height() );
    img = OResource::loadImage( "SyncScreen", OResource::NoScale );
    if ( img.width() > ds.width() || img.height() > ds.height() ) {
	path = scalePath( path, ds.width(), img.width(), ds.height(), img.height() );
	img = img.smoothScale( ds.width(), ds.height() );
    }
    dot = OResource::loadImage( "syncdot", OResource::NoScale );
    setBackgroundColor( white );

    QPushButton *pb = new QPushButton( tr("Abort"), this, "CancelSync" );
    QSize bs = pb->sizeHint();
    bs.rwidth() += 10;
    bs.rheight() += 5;
    pb->setGeometry( (ds.width()-bs.width())/2, 4*ds.height()/5,
			bs.width(), bs.height() );
    connect( pb, SIGNAL(clicked()), this, SIGNAL(cancel()) );

    if ( path.count() >= 2 ) {
	path = generatePath( path, 8 );
	startTimer( 200 );
    }
}

void SyncDialog::paintEvent( QPaintEvent *pe )
{
    QPainter p(this );
    p.setClipRect( pe->rect() );
    int ox = (width() - img.width())/2;
    int oy = (height() - img.height())/2;

    QRect ir = QRect(ox, oy, img.width(), img.height()) & pe->rect();

    if ( ir.isValid() )
	p.drawImage( ir.x(), ir.y(), img, ir.x()-ox, ir.y()-oy, ir.width(), ir.height() );

    QString syncMsg = tr("Syncing:");
    p.setPen( black );
    QRect r( 0, 0, width()/2-5, QMAX(oy,80) );
    p.drawText( r, AlignRight | AlignVCenter, syncMsg );
    r.moveBy( width()/2, 0 );
    QFont f( font() );
    f.setWeight( QFont::Bold );
    p.setFont( f );
    p.drawText( r, AlignLeft | AlignVCenter, what );

    if ( !hideDot )
	p.drawImage( ox+path[nextPt].x()-dot.width()/2, oy+path[nextPt].y()-dot.height()/2, dot );
}

void SyncDialog::timerEvent( QTimerEvent * )
{
    int ox = (width() - img.width())/2;
    int oy = (height() - img.height())/2;
    int oldPt = nextPt;

    if ( !rev ) {
	nextPt++;
	if ( nextPt == (int)path.count() ) {
	    nextPt -= 2;
	    rev = TRUE;
	}
    } else {
	nextPt--;
	if ( nextPt < 0 ) {
	    nextPt = 1;
	    rev = FALSE;
	}
    }

    hideDot = FALSE;
    repaint( ox+path[nextPt].x()-dot.width()/2, oy+path[nextPt].y()-dot.height()/2,
	    dot.width(), dot.height() );
    hideDot = TRUE;
    repaint( ox+path[oldPt].x()-dot.width()/2, oy+path[oldPt].y()-dot.height()/2,
	    dot.width(), dot.height() );
}

void SyncDialog::loadPath()
{
    QString pfile = OResource::findPixmap( "syncdot" );
    if ( pfile.isEmpty() )
	return;
    int dp = pfile.findRev('.');
    pfile.replace( dp, pfile.length()-dp, ".path" );

    int count = 0;
    QFile file( pfile );
    if ( file.open( IO_ReadOnly ) ) {
	QString line;
	while ( file.readLine( line, 256 ) > 0 ) {
	    int x, y;
	    if ( sscanf( line.latin1(), "%d %d", &x, &y ) == 2 ) {
		path.resize( count+1 );
		path[count++] = QPoint(x, y);
	    }
	}
    }
}

QPointArray SyncDialog::scalePath( const QPointArray &pa, int xn, int xd, int yn, int yd )
{
    QPointArray sa( pa.size() );

    for ( unsigned i = 0; i < pa.count(); i++ ) {
	int x = xn * pa[int(i)].x() / xd;
	int y = yn * pa[int(i)].y() / yd;
	sa[int(i)] = QPoint( x, y );
    }

    return sa;
}

QPointArray SyncDialog::generatePath( const QPointArray &pa, int dist )
{
    if ( pa.count() < 2 )
	return pa;

    QPointArray fa;
    int count = 0;
    fa.resize( count+1 );
    fa[count++] = pa[0];
    for ( unsigned i = 0; i < pa.count()-1; i++ ) {
	int x1 = pa[int(i)].x();
	int y1 = pa[int(i)].y();
	int x2 = pa[int(i+1)].x();
	int y2 = pa[int(i+1)].y();
	int dx = x2 - x1;
	int dy = y2 - y1;
	int pts = (QMAX(QABS(dx),QABS(dy)) + dist/2 )/dist;
	for ( int j = 1; j < pts; j++ ) {
	    int x = j * dx / pts;
	    int y = j * dy / pts;
	    fa.resize( count+1 );
	    fa[count++] = pa[int(i)] + QPoint( x, y );
	}
	fa.resize( count+1 );
	fa[count++] = pa[int(i+1)];
    }

    return fa;
}

