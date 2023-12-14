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

SvgWriter::SvgWriter(const QRectF& rect, double scale) : mRect(rect), mScale(scale), mMarkers()
{
    // Nothing more to do here.
}

//======================================================================================================================

bool SvgWriter::write(const QString& path, const QColor& backgroundColor, const QList<OdgItem*>& items)
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

    xml.writeStartElement("defs");
    mMarkers.clear();
    writeMarkers(xml, items);
    xml.writeEndElement();

    writeBackgroundColor(xml, backgroundColor);
    writeItems(xml, items);

    xml.writeEndElement();
    xml.writeEndDocument();

    svgFile.close();
    return true;
}

//======================================================================================================================

void SvgWriter::writeMarkers(QXmlStreamWriter& xml, const QList<OdgItem*>& items)
{
    OdgCurveItem* curveItem = nullptr;
    OdgGroupItem* groupItem = nullptr;
    OdgLineItem* lineItem = nullptr;
    OdgPolylineItem* polylineItem = nullptr;

    for(auto& item : items)
    {
        lineItem = dynamic_cast<OdgLineItem*>(item);
        if (lineItem)
        {
            if (lineItem->pen().style() != Qt::NoPen && lineItem->pen().brush().color().alpha() != 0)
            {
                writeMarkerDef(xml, lineItem->startMarker(), lineItem->pen());
                writeMarkerDef(xml, lineItem->endMarker(), lineItem->pen());
            }
            continue;
        }

        curveItem = dynamic_cast<OdgCurveItem*>(item);
        if (curveItem)
        {
            if (curveItem->pen().style() != Qt::NoPen && curveItem->pen().brush().color().alpha() != 0)
            {
                writeMarkerDef(xml, curveItem->startMarker(), curveItem->pen());
                writeMarkerDef(xml, curveItem->endMarker(), curveItem->pen());
            }
            continue;
        }

        polylineItem = dynamic_cast<OdgPolylineItem*>(item);
        if (polylineItem)
        {
            if (polylineItem->pen().style() != Qt::NoPen && polylineItem->pen().brush().color().alpha() != 0)
            {
                writeMarkerDef(xml, polylineItem->startMarker(), polylineItem->pen());
                writeMarkerDef(xml, polylineItem->endMarker(), polylineItem->pen());
            }
            continue;
        }

        groupItem = dynamic_cast<OdgGroupItem*>(item);
        if (groupItem)
        {
            writeMarkers(xml, groupItem->items());
            continue;
        }
    }
}

void SvgWriter::writeMarkerDef(QXmlStreamWriter& xml, const OdgMarker& marker, const QPen& pen)
{
    QString name;
    bool writeElement = false;

    switch (marker.style())
    {
    case Odg::TriangleMarker:
        name = createMarkerName(marker, pen);
        writeElement = !mMarkers.contains(name);
        if (writeElement)
        {
            mMarkers.append(name);

            const double arrowWidth = marker.size() / pen.widthF();
            const double arrowHeight = 2 * arrowWidth / qSqrt(3);
            QString xStr = lengthToString(-arrowWidth);
            QString y1Str = lengthToString(-arrowHeight / 2);
            QString y2Str = lengthToString(arrowHeight / 2);

            xml.writeStartElement("marker");
            xml.writeAttribute("id", name);
            xml.writeAttribute("viewBox", viewBoxToString(QRectF(-arrowWidth - 0.5, -arrowHeight / 2 - 0.5,
                                                                 arrowWidth + 1.0, arrowHeight + 1.0)));
            xml.writeAttribute("markerWidth", lengthToString(arrowWidth + 1.0));
            xml.writeAttribute("markerHeight", lengthToString(arrowHeight + 1.0));
            xml.writeAttribute("orient", "auto-start-reverse");

            xml.writeStartElement("path");
            xml.writeAttribute("d", "M 0,0 L " + xStr + "," + y1Str + " L " + xStr + "," + y2Str + " Z");
        }
        break;
    case Odg::CircleMarker:
        name = createMarkerName(marker, pen);
        writeElement = !mMarkers.contains(name);
        if (writeElement)
        {
            mMarkers.append(name);

            const double diameter = marker.size() / pen.widthF();

            xml.writeStartElement("marker");
            xml.writeAttribute("id", name);
            xml.writeAttribute("viewBox", viewBoxToString(QRectF(-diameter / 2 - 0.5, -diameter / 2 - 0.5,
                                                                 diameter + 1.0, diameter + 1.0)));
            xml.writeAttribute("markerWidth", lengthToString(diameter + 1.0));
            xml.writeAttribute("markerHeight", lengthToString(diameter + 1.0));
            xml.writeAttribute("orient", "auto-start-reverse");

            xml.writeStartElement("circle");
            xml.writeAttribute("cx", "0");
            xml.writeAttribute("cy", "0");
            xml.writeAttribute("r", lengthToString(diameter / 2));
        }
        break;
    default:
        break;
    }

    if (writeElement)
    {
        const QColor penColor = pen.brush().color();

        // Write fill and stroke attributes
        xml.writeAttribute("fill", colorToString(penColor));
        xml.writeAttribute("stroke", colorToString(penColor));
        if (penColor.alpha() != 255)
            xml.writeAttribute("stroke-opacity", percentToString(penColor.alphaF()));
        switch (pen.capStyle())
        {
        case Qt::FlatCap:
            xml.writeAttribute("stroke-linecap", "butt");
            break;
        case Qt::SquareCap:
            xml.writeAttribute("stroke-linecap", "square");
            break;
        default:
            xml.writeAttribute("stroke-linecap", "round");
            break;
        }

        switch (pen.joinStyle())
        {
        case Qt::MiterJoin:
        case Qt::SvgMiterJoin:
            xml.writeAttribute("stroke-linejoin", "miter");
            break;
        case Qt::BevelJoin:
            xml.writeAttribute("stroke-linejoin", "bevel");
            break;
        default:
            xml.writeAttribute("stroke-linejoin", "round");
            break;
        }

        // Finish marker element and sub-element
        xml.writeEndElement();
        xml.writeEndElement();
    }
}

QString SvgWriter::createMarkerName(const OdgMarker& marker, const QPen& pen) const
{
    switch (marker.style())
    {
    case Odg::TriangleMarker:
        return "Triangle_" + lengthToString(marker.size()) + "_" + lengthToString(pen.widthF());
        break;
    case Odg::CircleMarker:
        return "Circle_" + lengthToString(marker.size()) + "_" + lengthToString(pen.widthF());
        break;
    default:
        break;
    }

    return "none";
}

//======================================================================================================================

void SvgWriter::writeBackgroundColor(QXmlStreamWriter& xml, const QColor& backgroundColor)
{
    xml.writeStartElement("rect");
    xml.writeAttribute("x", lengthToString(mRect.left()));
    xml.writeAttribute("y", lengthToString(mRect.top()));
    xml.writeAttribute("width", lengthToString(mRect.width()));
    xml.writeAttribute("height", lengthToString(mRect.height()));

    xml.writeAttribute("fill", colorToString(backgroundColor));
    xml.writeAttribute("stroke", "none");

    xml.writeEndElement();
}

void SvgWriter::writeItems(QXmlStreamWriter& xml, const QList<OdgItem*>& items)
{
    OdgCurveItem* curveItem = nullptr;
    OdgEllipseItem* ellipseItem = nullptr;
    OdgGroupItem* groupItem = nullptr;
    OdgLineItem* lineItem = nullptr;
    OdgPathItem* pathItem = nullptr;
    OdgPolygonItem* polygonItem = nullptr;
    OdgPolylineItem* polylineItem = nullptr;
    OdgRectItem* rectItem = nullptr;
    OdgRoundedRectItem* roundedRectItem = nullptr;
    OdgTextEllipseItem* textEllipseItem = nullptr;
    OdgTextItem* textItem = nullptr;
    OdgTextRoundedRectItem* textRoundedRectItem = nullptr;

    for(auto& item : items)
    {
        lineItem = dynamic_cast<OdgLineItem*>(item);
        if (lineItem)
        {
            writeLineItem(xml, lineItem);
            continue;
        }

        curveItem = dynamic_cast<OdgCurveItem*>(item);
        if (curveItem)
        {
            writeCurveItem(xml, curveItem);
            continue;
        }

        polylineItem = dynamic_cast<OdgPolylineItem*>(item);
        if (polylineItem)
        {
            writePolylineItem(xml, polylineItem);
            continue;
        }

        textItem = dynamic_cast<OdgTextItem*>(item);
        if (textItem)
        {
            writeTextItem(xml, textItem);
            continue;
        }

        textRoundedRectItem = dynamic_cast<OdgTextRoundedRectItem*>(item);
        if (textRoundedRectItem)
        {
            writeTextRoundedRectItem(xml, textRoundedRectItem);
            continue;
        }

        roundedRectItem = dynamic_cast<OdgRoundedRectItem*>(item);
        if (roundedRectItem)
        {
            writeRoundedRectItem(xml, roundedRectItem);
            continue;
        }

        textEllipseItem = dynamic_cast<OdgTextEllipseItem*>(item);
        if (textEllipseItem)
        {
            writeTextEllipseItem(xml, textEllipseItem);
            continue;
        }

        ellipseItem = dynamic_cast<OdgEllipseItem*>(item);
        if (ellipseItem)
        {
            writeEllipseItem(xml, ellipseItem);
            continue;
        }

        pathItem = dynamic_cast<OdgPathItem*>(item);
        if (pathItem)
        {
            writePathItem(xml, pathItem);
            continue;
        }

        rectItem = dynamic_cast<OdgRectItem*>(item);
        if (rectItem)
        {
            writeRectItem(xml, rectItem);
            continue;
        }

        polygonItem = dynamic_cast<OdgPolygonItem*>(item);
        if (polygonItem)
        {
            writePolygonItem(xml, polygonItem);
            continue;
        }

        groupItem = dynamic_cast<OdgGroupItem*>(item);
        if (groupItem)
        {
            writeGroupItem(xml, groupItem);
            continue;
        }
    }
}

//======================================================================================================================

void SvgWriter::writeLineItem(QXmlStreamWriter& xml, OdgLineItem* item)
{
    xml.writeStartElement("line");

    const QString transform = transformToString(item->position(), item->rotation(), item->isFlipped());
    if (!transform.isEmpty()) xml.writeAttribute("transform", transform);

    const QLineF line = item->line();
    xml.writeAttribute("x1", lengthToString(line.x1()));
    xml.writeAttribute("y1", lengthToString(line.y1()));
    xml.writeAttribute("x2", lengthToString(line.x2()));
    xml.writeAttribute("y2", lengthToString(line.y2()));

    xml.writeAttribute("fill", "none");
    writePen(xml, item->pen());
    writeStartMarker(xml, item->startMarker(), item->pen());
    writeEndMarker(xml, item->endMarker(), item->pen());

    xml.writeEndElement();
}

void SvgWriter::writeCurveItem(QXmlStreamWriter& xml, OdgCurveItem* item)
{
    xml.writeStartElement("path");

    const QString transform = transformToString(item->position(), item->rotation(), item->isFlipped());
    if (!transform.isEmpty()) xml.writeAttribute("transform", transform);

    OdgCurve curve = item->curve();
    QPainterPath curvePath;
    curvePath.moveTo(curve.p1());
    curvePath.cubicTo(curve.cp1(), curve.cp2(), curve.p2());
    xml.writeAttribute("d", pathToString(curvePath));

    xml.writeAttribute("fill", "none");
    writePen(xml, item->pen());
    writeStartMarker(xml, item->startMarker(), item->pen());
    writeEndMarker(xml, item->endMarker(), item->pen());

    xml.writeEndElement();
}

void SvgWriter::writeRectItem(QXmlStreamWriter& xml, OdgRectItem* item)
{
    xml.writeStartElement("rect");

    const QString transform = transformToString(item->position(), item->rotation(), item->isFlipped());
    if (!transform.isEmpty()) xml.writeAttribute("transform", transform);

    const QRectF rect = item->rect();
    xml.writeAttribute("x", lengthToString(rect.left()));
    xml.writeAttribute("y", lengthToString(rect.top()));
    xml.writeAttribute("width", lengthToString(rect.width()));
    xml.writeAttribute("height", lengthToString(rect.height()));

    writeBrush(xml, item->brush());
    writePen(xml, item->pen());

    xml.writeEndElement();
}

void SvgWriter::writeRoundedRectItem(QXmlStreamWriter& xml, OdgRoundedRectItem* item)
{
    xml.writeStartElement("rect");

    const QString transform = transformToString(item->position(), item->rotation(), item->isFlipped());
    if (!transform.isEmpty()) xml.writeAttribute("transform", transform);

    const QRectF rect = item->rect();
    xml.writeAttribute("x", lengthToString(rect.left()));
    xml.writeAttribute("y", lengthToString(rect.top()));
    xml.writeAttribute("width", lengthToString(rect.width()));
    xml.writeAttribute("height", lengthToString(rect.height()));

    const double cornerRadius = item->cornerRadius();
    if (cornerRadius > 0)
    {
        xml.writeAttribute("rx", lengthToString(cornerRadius));
        xml.writeAttribute("ry", lengthToString(cornerRadius));
    }

    writeBrush(xml, item->brush());
    writePen(xml, item->pen());

    xml.writeEndElement();
}

void SvgWriter::writeEllipseItem(QXmlStreamWriter& xml, OdgEllipseItem* item)
{
    xml.writeStartElement("ellipse");

    const QString transform = transformToString(item->position(), item->rotation(), item->isFlipped());
    if (!transform.isEmpty()) xml.writeAttribute("transform", transform);

    const QRectF ellipse = item->ellipse();
    xml.writeAttribute("cx", lengthToString(ellipse.center().x()));
    xml.writeAttribute("cy", lengthToString(ellipse.center().y()));
    xml.writeAttribute("rx", lengthToString(ellipse.width() / 2));
    xml.writeAttribute("ry", lengthToString(ellipse.height() / 2));

    writeBrush(xml, item->brush());
    writePen(xml, item->pen());

    xml.writeEndElement();
}

void SvgWriter::writeTextItem(QXmlStreamWriter& xml, OdgTextItem* item)
{
    xml.writeStartElement("text");

    const QString transform = transformToString(item->position(), item->rotation(), item->isFlipped());
    if (!transform.isEmpty()) xml.writeAttribute("transform", transform);

    writeFont(xml, item->font());
    writeTextAlignment(xml, item->textAlignment());
    writeTextBrush(xml, item->textBrush());

    writeCaption(xml, item->caption(), QPointF(0, 0), item->textAlignment(), item->textPadding());

    xml.writeEndElement();
}

void SvgWriter::writeTextRoundedRectItem(QXmlStreamWriter& xml, OdgTextRoundedRectItem* item)
{
    xml.writeStartElement("g");

    const QString transform = transformToString(item->position(), item->rotation(), item->isFlipped());
    if (!transform.isEmpty()) xml.writeAttribute("transform", transform);

    // rect sub-element
    xml.writeStartElement("rect");

    const QRectF rect = item->rect();
    xml.writeAttribute("x", lengthToString(rect.left()));
    xml.writeAttribute("y", lengthToString(rect.top()));
    xml.writeAttribute("width", lengthToString(rect.width()));
    xml.writeAttribute("height", lengthToString(rect.height()));

    const double cornerRadius = item->cornerRadius();
    if (cornerRadius > 0)
    {
        xml.writeAttribute("rx", lengthToString(cornerRadius));
        xml.writeAttribute("ry", lengthToString(cornerRadius));
    }

    writeBrush(xml, item->brush());
    writePen(xml, item->pen());

    xml.writeEndElement();

    // text sub-element
    xml.writeStartElement("text");

    writeFont(xml, item->font());
    writeTextAlignment(xml, item->textAlignment());
    writeTextBrush(xml, item->textBrush());

    writeCaption(xml, item->caption(), item->calculateAnchorPoint(item->textAlignment()),
                 item->textAlignment(), item->textPadding());

    xml.writeEndElement();

    xml.writeEndElement();
}

void SvgWriter::writeTextEllipseItem(QXmlStreamWriter& xml, OdgTextEllipseItem* item)
{
    xml.writeStartElement("ellipse");

    const QString transform = transformToString(item->position(), item->rotation(), item->isFlipped());
    if (!transform.isEmpty()) xml.writeAttribute("transform", transform);

    const QRectF ellipse = item->ellipse();
    xml.writeAttribute("cx", lengthToString(ellipse.center().x()));
    xml.writeAttribute("cy", lengthToString(ellipse.center().y()));
    xml.writeAttribute("rx", lengthToString(ellipse.width() / 2));
    xml.writeAttribute("ry", lengthToString(ellipse.height() / 2));

    writeBrush(xml, item->brush());
    writePen(xml, item->pen());

    xml.writeStartElement("text");

    writeFont(xml, item->font());
    writeTextAlignment(xml, item->textAlignment());
    writeTextBrush(xml, item->textBrush());

    writeCaption(xml, item->caption(), item->calculateAnchorPoint(item->textAlignment()),
                 item->textAlignment(), item->textPadding());

    xml.writeEndElement();

    xml.writeEndElement();
}

void SvgWriter::writePolylineItem(QXmlStreamWriter& xml, OdgPolylineItem* item)
{
    xml.writeStartElement("polyline");

    const QString transform = transformToString(item->position(), item->rotation(), item->isFlipped());
    if (!transform.isEmpty()) xml.writeAttribute("transform", transform);

    xml.writeAttribute("points", pointsToString(item->polyline()));

    xml.writeAttribute("fill", "none");
    writePen(xml, item->pen());
    writeStartMarker(xml, item->startMarker(), item->pen());
    writeEndMarker(xml, item->endMarker(), item->pen());

    xml.writeEndElement();
}

void SvgWriter::writePolygonItem(QXmlStreamWriter& xml, OdgPolygonItem* item)
{
    xml.writeStartElement("polygon");

    const QString transform = transformToString(item->position(), item->rotation(), item->isFlipped());
    if (!transform.isEmpty()) xml.writeAttribute("transform", transform);

    xml.writeAttribute("points", pointsToString(item->polygon()));

    writeBrush(xml, item->brush());
    writePen(xml, item->pen());

    xml.writeEndElement();
}

void SvgWriter::writePathItem(QXmlStreamWriter& xml, OdgPathItem* item)
{
    const QRectF pathRect = item->pathRect();

    if (pathRect.width() != 0 && pathRect.height() != 0)
    {
        xml.writeStartElement("path");

        const QString transform = transformToString(item->position(), item->rotation(), item->isFlipped());
        if (!transform.isEmpty()) xml.writeAttribute("transform", transform);

        const QRectF rect = item->rect();
        const double xScale = rect.width() / pathRect.width(), yScale = rect.height() / pathRect.height();
        QTransform pathTransform;
        pathTransform.translate(-pathRect.left() * xScale, -pathRect.top() * yScale);
        pathTransform.translate(rect.left(), rect.top());
        pathTransform.scale(xScale, yScale);
        xml.writeAttribute("d", pathToString(pathTransform.map(item->path())));

        writeBrush(xml, item->brush());
        writePen(xml, item->pen());

        xml.writeEndElement();
    }
}

void SvgWriter::writeGroupItem(QXmlStreamWriter& xml, OdgGroupItem* item)
{
    xml.writeStartElement("g");

    const QString transform = transformToString(item->position(), item->rotation(), item->isFlipped());
    if (!transform.isEmpty()) xml.writeAttribute("transform", transform);

    writeItems(xml, item->items());

    xml.writeEndElement();
}

//======================================================================================================================

void SvgWriter::writeCaption(QXmlStreamWriter& xml, const QString& caption, const QPointF& anchorPoint, Qt::Alignment textAlignment,
                             const QSizeF textPadding)
{
    const QStringList lines = caption.split("\n");

    const QPointF anchorPointWithPadding = calculateAnchorPointWithPadding(anchorPoint, textAlignment, textPadding);

    QString startDyStr;
    const Qt::Alignment vertical = (textAlignment & Qt::AlignVertical_Mask);
    if (vertical & Qt::AlignVCenter)
        startDyStr = QString::number(-(lines.size() - 1) / 2.0, 'f', 1) + "em";
    else if (vertical & Qt::AlignBottom)
        startDyStr = QString::number(-(lines.size() - 1)) + "em";
    else
        startDyStr = "0em";

    bool first = true;
    for(auto& line : lines)
    {
        xml.writeStartElement("tspan");

        if (first)
        {
            xml.writeAttribute("x", lengthToString(anchorPointWithPadding.x()));
            xml.writeAttribute("y", lengthToString(anchorPointWithPadding.y()));
            xml.writeAttribute("dy", startDyStr);
        }
        else
        {
            xml.writeAttribute("x", lengthToString(anchorPointWithPadding.x()));
            xml.writeAttribute("dy", "1em");
        }

        xml.writeCharacters(line);
        xml.writeEndElement();

        first = false;
    }
}

QPointF SvgWriter::calculateAnchorPointWithPadding(const QPointF& anchorPoint, Qt::Alignment textAlignment,
                                                   const QSizeF textPadding)
{
    QPointF anchorPointWithPadding = anchorPoint;

    Qt::Alignment horizontal = (textAlignment & Qt::AlignHorizontal_Mask);
    if (horizontal & Qt::AlignLeft)
        anchorPointWithPadding.setX(anchorPointWithPadding.x() + textPadding.width());
    else if (horizontal & Qt::AlignRight)
        anchorPointWithPadding.setX(anchorPointWithPadding.x() - textPadding.width());

    Qt::Alignment vertical = (textAlignment & Qt::AlignVertical_Mask);
    if (vertical & Qt::AlignTop)
        anchorPointWithPadding.setY(anchorPointWithPadding.y() + textPadding.height());
    else if (vertical & Qt::AlignBottom)
        anchorPointWithPadding.setY(anchorPointWithPadding.y() - textPadding.height());

    return anchorPointWithPadding;
}

//======================================================================================================================

void SvgWriter::writeBrush(QXmlStreamWriter& xml, const QBrush& brush)
{
    QColor brushColor = brush.color();
    if (brushColor.alpha() == 0)
    {
        xml.writeAttribute("fill", "none");
    }
    else
    {
        xml.writeAttribute("fill", colorToString(brushColor));
        if (brushColor.alpha() != 255)
            xml.writeAttribute("fill-opacity", percentToString(brushColor.alphaF()));
    }
}

void SvgWriter::writePen(QXmlStreamWriter& xml, const QPen& pen)
{
    QColor penColor = pen.brush().color();
    if (pen.style() == Qt::NoPen || penColor.alpha() == 0)
    {
        xml.writeAttribute("stroke", "none");
    }
    else
    {
        xml.writeAttribute("stroke", colorToString(penColor));
        if (penColor.alpha() != 255)
            xml.writeAttribute("stroke-opacity", percentToString(penColor.alphaF()));
    }

    xml.writeAttribute("stroke-width", lengthToString(pen.widthF()));

    const QString dashStr = lengthToString(pen.widthF() * 3);
    const QString gapStr = lengthToString(pen.widthF() * 2);
    const QString dotStr = lengthToString(pen.widthF() * 1);
    switch (pen.style())
    {
    case Qt::DashLine:
        xml.writeAttribute("stroke-dasharray", dashStr + " " + gapStr);
        break;
    case Qt::DotLine:
        xml.writeAttribute("stroke-dasharray", dotStr + " " + gapStr);
        break;
    case Qt::DashDotLine:
        xml.writeAttribute("stroke-dasharray", dashStr + " " + gapStr + " " + dotStr + " " + gapStr);
        break;
    case Qt::DashDotDotLine:
        xml.writeAttribute(
            "stroke-dasharray", dashStr + " " + gapStr + " " + dotStr + " " + gapStr + " " + dotStr + " " + gapStr);
        break;
    default:
        break;
    }

    switch (pen.capStyle())
    {
    case Qt::FlatCap:
        xml.writeAttribute("stroke-linecap", "butt");
        break;
    case Qt::SquareCap:
        xml.writeAttribute("stroke-linecap", "square");
        break;
    default:
        xml.writeAttribute("stroke-linecap", "round");
        break;
    }

    switch (pen.joinStyle())
    {
    case Qt::MiterJoin:
    case Qt::SvgMiterJoin:
        xml.writeAttribute("stroke-linejoin", "miter");
        break;
    case Qt::BevelJoin:
        xml.writeAttribute("stroke-linejoin", "bevel");
        break;
    default:
        xml.writeAttribute("stroke-linejoin", "round");
        break;
    }
}

void SvgWriter::writeStartMarker(QXmlStreamWriter& xml, const OdgMarker& marker, const QPen& pen)
{
    if (marker.style() != Odg::NoMarker)
        xml.writeAttribute("marker-start", "url(#" + createMarkerName(marker, pen) + ")");
}

void SvgWriter::writeEndMarker(QXmlStreamWriter& xml, const OdgMarker& marker, const QPen& pen)
{
    if (marker.style() != Odg::NoMarker)
        xml.writeAttribute("marker-end", "url(#" + createMarkerName(marker, pen) + ")");
}

void SvgWriter::writeFont(QXmlStreamWriter& xml, const QFont& font)
{
    xml.writeAttribute("font-family", font.family());
    xml.writeAttribute("font-size", lengthToString(font.pointSizeF()));
    if (font.bold()) xml.writeAttribute("font-weight", "bold");
    if (font.italic()) xml.writeAttribute("font-style", "italic");
    if (font.underline()) xml.writeAttribute("text-decoration", "underline");
    if (font.strikeOut()) xml.writeAttribute("text-decoration", "line-through");
}

void SvgWriter::writeTextAlignment(QXmlStreamWriter& xml, Qt::Alignment alignment)
{
    Qt::Alignment horizontal = (alignment & Qt::AlignHorizontal_Mask);
    if (horizontal & Qt::AlignHCenter)
        xml.writeAttribute("text-anchor", "middle");
    else if (horizontal & Qt::AlignRight)
        xml.writeAttribute("text-anchor", "end");
    else
        xml.writeAttribute("text-anchor", "start");

    Qt::Alignment vertical = (alignment & Qt::AlignVertical_Mask);
    if (vertical & Qt::AlignVCenter)
        xml.writeAttribute("dominant-baseline", "central");
    else if (vertical & Qt::AlignBottom)
        xml.writeAttribute("dominant-baseline", "text-after-edge");
    else
        xml.writeAttribute("dominant-baseline", "text-before-edge");
}

void SvgWriter::writeTextBrush(QXmlStreamWriter& xml, const QBrush& brush)
{
    QColor brushColor = brush.color();
    if (brushColor.alpha() == 0)
    {
        xml.writeAttribute("fill", "none");
        xml.writeAttribute("stroke", "none");
    }
    else
    {
        xml.writeAttribute("fill", colorToString(brushColor));
        if (brushColor.alpha() != 255)
            xml.writeAttribute("fill-opacity", percentToString(brushColor.alphaF()));
        xml.writeAttribute("stroke", "none");
    }
}

//======================================================================================================================

QString SvgWriter::lengthToString(double length) const
{
    return QString::number(length, 'g', 8);
}

QString SvgWriter::percentToString(double value) const
{
    return QString::number(value * 100, 'f', 2) + "%";
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
