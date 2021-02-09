// DrawingLineItem.h
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

#ifndef DRAWINGLINEITEM_H
#define DRAWINGLINEITEM_H

#include <DrawingItem.h>

class DrawingLineItem : public DrawingItem
{
private:
	enum PointIndex { StartPoint, EndPoint, MidPoint };

private:
	QLineF mLine;
	QPen mPen;
	DrawingArrow mStartArrow;
	DrawingArrow mEndArrow;

	QRectF mBoundingRect;
	QPainterPath mShape;

public:
	DrawingLineItem();
	DrawingLineItem(const DrawingLineItem& item);
	virtual ~DrawingLineItem();

	virtual QString uniqueKey() const override;
	virtual DrawingItem* copy() const override;

	void setLine(const QLineF& line);
	QLineF line() const;

	void setPen(const QPen& pen);
	QPen pen() const;

	void setStartArrow(const DrawingArrow& arrow);
	void setEndArrow(const DrawingArrow& arrow);
	DrawingArrow startArrow() const;
	DrawingArrow endArrow() const;

	virtual void setProperties(const QHash<QString,QVariant>& properties) override;
	virtual QHash<QString,QVariant> properties() const override;

	virtual QRectF boundingRect() const override;
	virtual QPainterPath shape() const override;
	virtual bool isValid() const override;

	virtual void render(QPainter* painter) override;

	virtual void resize(DrawingItemPoint* point, const QPointF& position) override;

	virtual void writeToXml(QXmlStreamWriter* xml) override;
	virtual void readFromXml(QXmlStreamReader* xml) override;

private:
	void updateItemGeometry();
};

#endif
