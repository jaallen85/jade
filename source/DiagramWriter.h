/* DiagramWriter.h
 *
 * Copyright (C) 2013-2017 Jason Allen
 *
 * This file is part of the jade application.
 *
 * jade is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * jade is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with jade.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef DIAGRAMWRITER_H
#define DIAGRAMWRITER_H

#include <DiagramWidget.h>

class DiagramWriter : public QXmlStreamWriter
{
public:
	DiagramWriter(QIODevice* device);
	DiagramWriter(QString* string);
	~DiagramWriter();

	void write(DiagramWidget* diagram);
	void writeItems(const QList<DrawingItem*>& items);

private:
	void writeItemElements(const QList<DrawingItem*>& items);

	void writeLineItem(DrawingLineItem* item);
	void writeArcItem(DrawingArcItem* item);
	void writePolylineItem(DrawingPolylineItem* item);
	void writeCurveItem(DrawingCurveItem* item);
	void writeRectItem(DrawingRectItem* item);
	void writeEllipseItem(DrawingEllipseItem* item);
	void writePolygonItem(DrawingPolygonItem* item);
	void writeTextItem(DrawingTextItem* item);
	void writeTextRectItem(DrawingTextRectItem* item);
	void writeTextEllipseItem(DrawingTextEllipseItem* item);
	void writeTextPolygonItem(DrawingTextPolygonItem* item);
	void writePathItem(DrawingPathItem* item);
	void writeItemGroup(DrawingItemGroup* item);

	void writeItemStyle(DrawingItemStyle* style);

	QString alignmentToString(Qt::Alignment align) const;
	QString arrowStyleToString(DrawingItemStyle::ArrowStyle style) const;
	QString colorToString(const QColor& color) const;
	QString gridStyleToString(DiagramWidget::GridRenderStyle gridStyle) const;
	QString pathToString(const QPainterPath& path) const;
	QString penStyleToString(Qt::PenStyle style) const;
	QString penCapStyleToString(Qt::PenCapStyle style) const;
	QString penJoinStyleToString(Qt::PenJoinStyle style) const;
	QString pointsToString(const QPolygonF& points) const;
	QString transformToString(DrawingItem* item) const;
};

#endif
