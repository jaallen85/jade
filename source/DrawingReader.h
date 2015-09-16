/* DrawingReader.cpp
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

#ifndef DRAWINGREADER_H
#define DRAWINGREADER_H

#include <DrawingTypes.h>

class DrawingReader : public QXmlStreamReader
{
public:
	DrawingReader(QIODevice* device);
	~DrawingReader();

	void read(DrawingWidget* drawing);
	void readItems(QList<DrawingItem*>& items);

private:
	QList<DrawingItem*> readItemElements();

	DrawingLineItem* readLineItem();
	DrawingArcItem* readArcItem();
	DrawingPolylineItem* readPolylineItem();
	DrawingCurveItem* readCurveItem();
	DrawingRectItem* readRectItem();
	DrawingEllipseItem* readEllipseItem();
	DrawingPolygonItem* readPolygonItem();
	DrawingTextItem* readTextItem();
	DrawingPathItem* readPathItem();
	DrawingItemGroup* readItemGroup();

	void readItemProperties(DrawingItem* item);

	Qt::Alignment alignmentFromString(const QString& str) const;
	DrawingArrowStyle arrowStyleFromString(const QString& str) const;
	QColor colorFromString(const QString& str) const;
	DrawingGridStyle gridStyleFromString(const QString& str) const;
	QPainterPath pathFromString(const QString& str) const;
	Qt::PenStyle penStyleFromString(const QString& str) const;
	Qt::PenCapStyle penCapStyleFromString(const QString& str) const;
	Qt::PenJoinStyle penJoinStyleFromString(const QString& str) const;
	QPolygonF pointsFromString(const QString& str) const;
	void transformFromString(const QString& str, DrawingItem* item);
};

#endif
