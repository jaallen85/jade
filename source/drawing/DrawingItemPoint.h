// DrawingItemPoint.h
// Copyright (C) 2020  Jason Allen
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

#ifndef DRAWINGITEMPOINT_H
#define DRAWINGITEMPOINT_H

#include <QList>
#include <QPointF>

class DrawingItem;

class DrawingItemPoint
{
	friend class DrawingItem;

public:
	enum Type { None, Control, Connection, ControlAndConnection, FreeControlAndConnection };

private:
	DrawingItem* mItem;

	QPointF mPosition;
	Type mType;

	QList<DrawingItemPoint*> mConnections;

public:
	DrawingItemPoint(const QPointF& position = QPointF(), Type type = None);
	DrawingItemPoint(const DrawingItemPoint& point);
	~DrawingItemPoint();

	DrawingItem* item() const;

	void setPosition(const QPointF& position);
	void setPosition(qreal x, qreal y);
	void setX(qreal x);
	void setY(qreal y);
	QPointF position() const;
	qreal x() const;
	qreal y() const;

	void setType(Type type);
	Type type() const;
	bool isControlPoint() const;
	bool isConnectionPoint() const;
	bool isFree() const;

	void addConnection(DrawingItemPoint* itemPoint);
	void removeConnection(DrawingItemPoint* itemPoint);
	void clearConnections();
	QList<DrawingItemPoint*> connections() const;
	bool isConnected(DrawingItemPoint* itemPoint) const;
	bool isConnected(DrawingItem* item) const;
};

#endif
