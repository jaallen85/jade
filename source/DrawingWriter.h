/* DrawingWriter.cpp
 *
 * Copyright (C) 2013-2015 Jason Allen
 *
 * This file is part of the libdrawing library.
 *
 * libdrawing is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libdrawing is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libdrawing.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef DRAWINGWRITER_H
#define DRAWINGWRITER_H

#include <DrawingTypes.h>

class DrawingWriter : public QXmlStreamWriter
{
private:


public:
	DrawingWriter(QIODevice* device);
	~DrawingWriter();

public slots:
	void write(DrawingWidget* drawing);
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
	void writePathItem(DrawingPathItem* item);
	void writeItemGroup(DrawingItemGroup* item);

	void writeItemProperties(DrawingItem* item);

	QString transformToString(const QPointF& pos, qreal rotation, bool flipped);
	QString colorToString(const QColor& color) const;
	QString gridStyleToString(DrawingGridStyle gridStyle) const;
	QString pathToString(const QPainterPath& path) const;
	QString pointsToString(const QPolygonF& points) const;
};

#endif
