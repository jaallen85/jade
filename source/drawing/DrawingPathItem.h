// DrawingPathItem.h
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

#ifndef DRAWINGPATHITEM_H
#define DRAWINGPATHITEM_H

#include <DrawingItem.h>

class DrawingPathItem : public DrawingItem
{
private:
	enum PointIndex { TopLeft, BottomRight, TopRight, BottomLeft, TopMiddle, MiddleRight, BottomMiddle, MiddleLeft };

private:
	QRectF mRect;
	QPen mPen;

	QString mPathName;
	QPainterPath mPath;
	QRectF mPathRect;
	QHash<DrawingItemPoint*,QPointF> mPathConnectionPoints;

	QRectF mBoundingRect;
	QPainterPath mShape;
	QPainterPath mTransformedPath;

public:
	DrawingPathItem();
	DrawingPathItem(const DrawingPathItem& item);
	virtual ~DrawingPathItem();

	virtual QString uniqueKey() const override;
	virtual DrawingItem* copy() const override;

	void setRect(const QRectF& rect);
	void setRect(qreal left, qreal top, qreal width, qreal height);
	QRectF rect() const;

	void setPen(const QPen& pen);
	QPen pen() const;

	void setPathName(const QString& name);
	QString pathName() const;

	void setPath(const QPainterPath& path, const QRectF& pathRect);
	QPainterPath path() const;
	QRectF pathRect() const;

	void addConnectionPoint(const QPointF& pathPos);
	void addConnectionPoints(const QPolygonF& pathPos);
	QPolygonF connectionPoints() const;

	QPointF mapToPath(const QPointF& itemPos) const;
	QRectF mapToPath(const QRectF& itemRect) const;
	QPointF mapFromPath(const QPointF& pathPos) const;
	QRectF mapFromPath(const QRectF& pathRect) const;

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

	QPainterPath transformedPath() const;

	QString pathToString(const QPainterPath& path) const;
	QPainterPath pathFromString(const QString& str) const;
};

#endif
