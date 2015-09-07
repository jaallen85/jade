/* DrawingItemPoint.cpp
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

#ifndef DRAWINGITEMPOINT_H
#define DRAWINGITEMPOINT_H

#include <DrawingTypes.h>

class DrawingItemPoint
{
	friend class DrawingItem;

public:
	enum Flag
	{
		NoFlags = 0x0000,
		Control = 0x0001,
		Connection = 0x0002,
		Free = 0x0004
	};
	Q_DECLARE_FLAGS(Flags, Flag)

private:
	DrawingItem* mItem;

	QPointF mPosition;
	QSize mSize;
	Flags mFlags;
	int mCategory;

	QList<DrawingItemPoint*> mConnections;

public:
	DrawingItemPoint(const QPointF& pos = QPointF(), Flags flags = Control);
	DrawingItemPoint(const DrawingItemPoint& point);
	~DrawingItemPoint();

	virtual DrawingItemPoint* copy() const;

	DrawingItem* item() const;

	// Selectors
	void setPos(const QPointF& pos);
	void setPos(qreal x, qreal y);
	void setX(qreal x);
	void setY(qreal y);
	QPointF pos() const;
	qreal x() const;
	qreal y() const;

	void setSize(const QSize& size);
	void setSize(int width, int height);
	void setWidth(int width);
	void setHeight(int height);
	QSize size() const;
	int width() const;
	int height() const;

	void setFlags(Flags flags);
	Flags flags() const;
	bool isControlPoint() const;
	bool isConnectionPoint() const;
	bool isFree() const;

	void setCategory(int category);
	int category() const;

	// Connections
	void addConnection(DrawingItemPoint* itemPoint);
	void removeConnection(DrawingItemPoint* itemPoint);
	void clearConnections();
	QList<DrawingItemPoint*> connections() const;

	bool isConnected(DrawingItemPoint* itemPoint) const;
	bool isConnected(DrawingItem* item) const;

	// Convenience functions
	QRect screenRect() const;
	QRectF sceneRect() const;
	QRectF itemRect() const;

	bool shouldConnect(DrawingItemPoint* otherPoint) const;
	bool shouldDisconnect(DrawingItemPoint* otherPoint) const;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DrawingItemPoint::Flags)

#endif
