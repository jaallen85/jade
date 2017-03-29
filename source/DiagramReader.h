/* DiagramReader.h
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

#ifndef DIAGRAMREADER_H
#define DIAGRAMREADER_H

#include <DiagramWidget.h>

class DiagramReader : public QXmlStreamReader
{
public:
	DiagramReader(QIODevice* device);
	DiagramReader(const QString & data);
	~DiagramReader();

	void read(DiagramWidget* diagram);
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
	DrawingTextRectItem* readTextRectItem();
	DrawingTextEllipseItem* readTextEllipseItem();
	DrawingTextPolygonItem* readTextPolygonItem();
	DrawingPathItem* readPathItem();
	DrawingItemGroup* readItemGroup();

	void readItemStyle(DrawingItemStyle* style);

	Qt::Alignment alignmentFromString(const QString& str) const;
	DrawingItemStyle::ArrowStyle arrowStyleFromString(const QString& str) const;
	QColor colorFromString(const QString& str) const;
	DiagramWidget::GridRenderStyle gridStyleFromString(const QString& str) const;
	QPainterPath pathFromString(const QString& str) const;
	Qt::PenStyle penStyleFromString(const QString& str) const;
	Qt::PenCapStyle penCapStyleFromString(const QString& str) const;
	Qt::PenJoinStyle penJoinStyleFromString(const QString& str) const;
	QPolygonF pointsFromString(const QString& str) const;
	void transformFromString(const QString& str, DrawingItem* item);
};

#endif
