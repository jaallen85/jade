// DrawingEllipseItem.h
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

#ifndef DRAWINGELLIPSEITEM_H
#define DRAWINGELLIPSEITEM_H

#include <DrawingItem.h>

class DrawingEllipseItem : public DrawingItem
{
private:
	enum PointIndex { TopLeft, BottomRight, TopRight, BottomLeft, TopMiddle, MiddleRight, BottomMiddle, MiddleLeft };

private:
	QRectF mEllipse;
	QPen mPen;
	QBrush mBrush;

	QRectF mBoundingRect;
	QPainterPath mShape;

public:
	DrawingEllipseItem();
	DrawingEllipseItem(const DrawingEllipseItem& item);
	virtual ~DrawingEllipseItem();

	virtual QString uniqueKey() const override;
	virtual DrawingItem* copy() const override;

	void setEllipse(const QRectF& rect);
	QRectF ellipse() const;

	void setPen(const QPen& pen);
	QPen pen() const;

	void setBrush(const QBrush& brush);
	QBrush brush() const;

	virtual void setProperties(const QHash<QString,QVariant>& properties) override;
	virtual QHash<QString,QVariant> properties() const override;

	virtual QRectF boundingRect() const override;
	virtual QPainterPath shape() const override;
	virtual bool isValid() const override;

	virtual void render(QPainter* painter) override;

	virtual void resize(DrawingItemPoint* point, const QPointF& position) override;

	virtual void writeToXml(QXmlStreamWriter* xml) override;
	virtual void readFromXml(QXmlStreamReader* xml) override;

protected:
	virtual void updateItemGeometry();
};

#endif
