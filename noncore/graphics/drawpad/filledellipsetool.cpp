/***************************************************************************
 *                                                                         *
 *   DrawPad - a drawing program for Opie Environment                      *
 *                                                                         *
 *   (C) 2002 by S. Prud'homme <prudhomme@laposte.net>                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "filledellipsetool.h"

#include "drawpad.h"
#include "drawpadcanvas.h"

#include <qimage.h>

FilledEllipseTool::FilledEllipseTool(DrawPad* drawPad, DrawPadCanvas* drawPadCanvas)
    : ShapeTool(drawPad, drawPadCanvas)
{
}

FilledEllipseTool::~FilledEllipseTool()
{
}

void FilledEllipseTool::drawFinalShape(QPainter& p)
{
    p.setRasterOp(Qt::NotROP);
    p.drawRect(QRect(m_polyline[2], m_polyline[0]));
    p.setRasterOp(Qt::CopyROP);

    if (m_pDrawPad->antiAliasing()) {
        QRect r = m_polyline.boundingRect();
        r = r.normalize();
        r.setLeft(r.left() - m_pDrawPad->pen().width());
        r.setTop(r.top() - m_pDrawPad->pen().width());
        r.setRight(r.right() + m_pDrawPad->pen().width());
        r.setBottom(r.bottom() + m_pDrawPad->pen().width());

        QPixmap areaPixmap(r.width(), r.height());
        bitBlt(&areaPixmap, QPoint(0, 0), p.device(), r);

        QImage areaImage = areaPixmap.convertToImage();
        QImage bigAreaImage = areaImage.smoothScale(areaImage.width() * 3, areaImage.height() * 3);

        QPixmap bigAreaPixmap;
        bigAreaPixmap.convertFromImage(bigAreaImage);

        QPen bigAreaPen = m_pDrawPad->pen();
        bigAreaPen.setWidth(bigAreaPen.width() * 3);

        QPainter bigAreaPainter;
        bigAreaPainter.begin(&bigAreaPixmap);
        bigAreaPainter.setPen(bigAreaPen);
        bigAreaPainter.setBrush(m_pDrawPad->brush());
        bigAreaPainter.drawEllipse(QRect(bigAreaPen.width() + 1, bigAreaPen.width() + 1,
                                        bigAreaPixmap.width() - 2 * (bigAreaPen.width() + 1),
                                        bigAreaPixmap.height() - 2 * (bigAreaPen.width() + 1)));
        bigAreaPainter.end();

        bigAreaImage = bigAreaPixmap.convertToImage();
        areaImage = bigAreaImage.smoothScale(bigAreaImage.width() / 3, bigAreaImage.height() / 3);
        areaPixmap.convertFromImage(areaImage);

        p.drawPixmap(r.x(), r.y(), areaPixmap);
    } else {
        p.setPen(m_pDrawPad->pen());
        p.setBrush(m_pDrawPad->brush());
        p.drawEllipse(QRect(m_polyline[2], m_polyline[0]));
    }
}

void FilledEllipseTool::drawTemporaryShape(QPainter& p)
{
    p.setRasterOp(Qt::NotROP);
    p.drawRect(QRect(m_polyline[2], m_polyline[1]));
    p.drawRect(QRect(m_polyline[2], m_polyline[0]));
}
