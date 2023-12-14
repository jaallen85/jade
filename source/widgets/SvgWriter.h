// File: SvgWriter.h
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

#ifndef SVGWRITER_H
#define SVGWRITER_H

#include <QPainterPath>

class QXmlStreamWriter;
class OdgCurveItem;
class OdgEllipseItem;
class OdgGroupItem;
class OdgItem;
class OdgLineItem;
class OdgMarker;
class OdgPathItem;
class OdgPolygonItem;
class OdgPolylineItem;
class OdgRectItem;
class OdgRoundedRectItem;
class OdgTextEllipseItem;
class OdgTextItem;
class OdgTextRoundedRectItem;


class SvgWriter
{
private:
    QRectF mRect;
    double mScale;

    QStringList mMarkers;

public:
    SvgWriter(const QRectF& rect, double scale);

    bool write(const QString& path, const QColor& backgroundColor, const QList<OdgItem*>& items);

private:
    void writeMarkers(QXmlStreamWriter& xml, const QList<OdgItem*>& items);
    void writeMarkerDef(QXmlStreamWriter& xml, const OdgMarker& marker, const QPen& pen);
    QString createMarkerName(const OdgMarker& marker, const QPen& pen) const;

    void writeBackgroundColor(QXmlStreamWriter& xml, const QColor& color);
    void writeItems(QXmlStreamWriter& xml, const QList<OdgItem*>& items);

    void writeLineItem(QXmlStreamWriter& xml, OdgLineItem* item);
    void writeCurveItem(QXmlStreamWriter& xml, OdgCurveItem* item);
    void writeRectItem(QXmlStreamWriter& xml, OdgRectItem* item);
    void writeRoundedRectItem(QXmlStreamWriter& xml, OdgRoundedRectItem* item);
    void writeEllipseItem(QXmlStreamWriter& xml, OdgEllipseItem* item);
    void writeTextItem(QXmlStreamWriter& xml, OdgTextItem* item);
    void writeTextRoundedRectItem(QXmlStreamWriter& xml, OdgTextRoundedRectItem* item);
    void writeTextEllipseItem(QXmlStreamWriter& xml, OdgTextEllipseItem* item);
    void writePolylineItem(QXmlStreamWriter& xml, OdgPolylineItem* item);
    void writePolygonItem(QXmlStreamWriter& xml, OdgPolygonItem* item);
    void writePathItem(QXmlStreamWriter& xml, OdgPathItem* item);
    void writeGroupItem(QXmlStreamWriter& xml, OdgGroupItem* item);

    void writeCaption(QXmlStreamWriter& xml, const QString& caption, const QPointF& anchorPoint,
                      Qt::Alignment textAlignment, const QSizeF textPadding);
    QPointF calculateAnchorPointWithPadding(const QPointF& anchorPoint, Qt::Alignment textAlignment,
                                            const QSizeF textPadding);

    void writeBrush(QXmlStreamWriter& xml, const QBrush& brush);
    void writePen(QXmlStreamWriter& xml, const QPen& pen);
    void writeStartMarker(QXmlStreamWriter& xml, const OdgMarker& marker, const QPen& pen);
    void writeEndMarker(QXmlStreamWriter& xml, const OdgMarker& marker, const QPen& pen);
    void writeFont(QXmlStreamWriter& xml, const QFont& font);
    void writeTextAlignment(QXmlStreamWriter& xml, Qt::Alignment alignment);
    void writeTextBrush(QXmlStreamWriter& xml, const QBrush& brush);

    QString lengthToString(double length) const;
    QString percentToString(double value) const;
    QString colorToString(const QColor& color) const;

    QString transformToString(const QPointF& position, bool flipped, int rotation) const;
    QString viewBoxToString(const QRectF& viewBox) const;
    QString pointsToString(const QPolygonF& points) const;
    QString pathToString(const QPainterPath& path) const;

};

#endif
