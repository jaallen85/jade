// File: SvgWriter.cpp
// Copyright (C) 2023  Jason Allen
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "SvgWriter.h"
#include "OdgCurveItem.h"
#include "OdgEllipseItem.h"
#include "OdgGroupItem.h"
#include "OdgLineItem.h"
#include "OdgPathItem.h"
#include "OdgPolygonItem.h"
#include "OdgPolylineItem.h"
#include "OdgRoundedRectItem.h"
#include "OdgTextItem.h"
#include "OdgTextEllipseItem.h"
#include "OdgTextRoundedRectItem.h"
#include <QFile>
#include <QXmlStreamWriter>

SvgWriter::SvgWriter(const QRectF& rect, double scale) : mRect(rect), mScale(scale)
{
    // Nothing more to do here.
}

//======================================================================================================================

bool SvgWriter::write(const QString& path, const QList<OdgItem*>& items)
{
    if (mRect.width() <= 0 || mRect.height() <= 0 || mScale <= 0) return false;

    QFile svgFile(path);
    if (!svgFile.open(QFile::WriteOnly)) return false;

    QXmlStreamWriter xml(&svgFile);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);

    xml.writeStartDocument();
    xml.writeStartElement("svg");
    xml.writeAttribute("width", QString::number(qRound(mRect.width() * mScale)));
    xml.writeAttribute("height", QString::number(qRound(mRect.height() * mScale)));
    xml.writeAttribute("viewBox", viewBoxToString(mRect));
    xml.writeAttribute("version", "1.1");
    xml.writeAttribute("xmlns", "http://www.w3.org/2000/svg");

    // write background color
    // write items

    xml.writeEndElement();
    xml.writeEndDocument();

    svgFile.close();
    return true;
}

//======================================================================================================================

QString SvgWriter::lengthToString(double length) const
{
    return QString::number(length, 'g', 8);
}

QString SvgWriter::colorToString(const QColor& color) const
{
    return color.name(QColor::HexRgb);
}

//======================================================================================================================

QString SvgWriter::transformToString(const QPointF& position, bool flipped, int rotation) const
{
    QString transformStr;
    if (rotation != 0)
        transformStr += "rotate(" + QString::number(qDegreesToRadians(rotation * -90), 'f', 6) + ") ";
    if (flipped)
        transformStr += "scale(-1, 1) ";
    if (position.x() != 0 || position.y() != 0)
        transformStr += "translate(" + lengthToString(position.x()) + ", " + lengthToString(position.y()) + ") ";
    return transformStr.trimmed();
}

QString SvgWriter::viewBoxToString(const QRectF& viewBox) const
{
    QString leftStr = QString::number(viewBox.left(), 'g', 8);
    QString topStr = QString::number(viewBox.top(), 'g', 8);
    QString widthStr = QString::number(viewBox.width(), 'g', 8);
    QString heightStr = QString::number(viewBox.height(), 'g', 8);
    return leftStr + " " + topStr + " " + widthStr + " " + heightStr;
}

QString SvgWriter::pointsToString(const QPolygonF& points) const
{
    QString pointsStr;
    for(auto& point : points)
        pointsStr += QString::number(point.x(), 'g', 8) + "," + QString::number(point.y(), 'g', 8) + " ";
    return pointsStr.trimmed();
}

QString SvgWriter::pathToString(const QPainterPath& path) const
{
    QString pathStr;

    const int elementCount = path.elementCount();
    for(int elementIndex = 0; elementIndex < elementCount; elementIndex++)
    {
        const QPainterPath::Element element = path.elementAt(elementIndex);
        switch (element.type)
        {
        case QPainterPath::MoveToElement:
            pathStr += "M " + QString::number(element.x, 'g', 8) + " " + QString::number(element.y, 'g', 8) + " ";
            break;
        case QPainterPath::LineToElement:
            pathStr += "L " + QString::number(element.x, 'g', 8) + " " + QString::number(element.y, 'g', 8) + " ";
            break;
        case QPainterPath::CurveToElement:
            pathStr += "C " + QString::number(element.x, 'g', 8) + " " + QString::number(element.y, 'g', 8) + " ";
            break;
        case QPainterPath::CurveToDataElement:
            pathStr += QString::number(element.x, 'g', 8) + " " + QString::number(element.y, 'g', 8) + " ";
            break;
        default:
            pathStr += QString::number(element.x, 'g', 8) + " " + QString::number(element.y, 'g', 8) + " ";
            break;
        }
    }

    return pathStr.trimmed();
}
