/* DrawingPathItem.cpp
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

#ifndef DRAWINGPATHITEM_H
#define DRAWINGPATHITEM_H

#include <DrawingShapeItems.h>

class DrawingPathItem : public DrawingShapeItem
{
private:
	QString mName;
	QPainterPath mPath;
	QRectF mPathRect;
	QHash<DrawingItemPoint*,QPointF> mPathConnectionPoints;

public:
	DrawingPathItem();
	DrawingPathItem(const DrawingPathItem& item);
	virtual ~DrawingPathItem();

	virtual DrawingItem* copy() const;

	// Selectors
	void setName(const QString& name);
	QString name() const;

	void setRect(const QRectF& rect);
	void setRect(qreal left, qreal top, qreal width, qreal height);
	QRectF rect() const;

	void setPath(const QPainterPath& path, const QRectF& rect);
	QPainterPath path() const;
	QRectF pathRect() const;

	void addConnectionPoint(const QPointF& pathPos);
	void addConnectionPoints(const QPolygonF& pathPos);
	QPolygonF connectionPoints() const;

	// Helper functions
	QPointF mapToPath(const QPointF& itemPos) const;
	QRectF mapToPath(const QRectF& itemRect) const;
	QPointF mapFromPath(const QPointF& pathPos) const;
	QRectF mapFromPath(const QRectF& pathRect) const;
	QPainterPath transformedPath() const;

	// Description
	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	virtual bool isValid() const;

	virtual void paint(QPainter* painter);

	// Slots
	virtual void resizeItem(DrawingItemPoint* itemPoint, const QPointF& pos);

private:
	void setItemRect(const QRectF& rect);
	QRectF itemRect() const;
};

#endif
