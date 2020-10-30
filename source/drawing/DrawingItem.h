// DrawingItem.h
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

#ifndef DRAWINGITEM_H
#define DRAWINGITEM_H

#include <DrawingArrow.h>
#include <DrawingItemFactory.h>
#include <DrawingItemPoint.h>
#include <QList>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPointF>
#include <QPolygonF>
#include <QRectF>
#include <QString>
#include <QTransform>
#include <QVariant>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QtMath>

class DrawingWidget;
class QPainter;

class DrawingItem
{
	friend class DrawingWidget;

public:
	enum PlaceType { PlaceByMouseRelease, PlaceByMousePressAndRelease };

private:
	DrawingWidget* mWidget;

	QPointF mPosition;
	QTransform mTransform;
	QTransform mTransformInverse;

	QList<DrawingItemPoint*> mPoints;

	PlaceType mPlaceType;

	bool mSelected;

public:
	DrawingItem();
	DrawingItem(const DrawingItem& item);
	virtual ~DrawingItem();

	virtual QString name() const = 0;
	virtual DrawingItem* copy() const = 0;

	DrawingWidget* widget() const;

	void setPosition(const QPointF& position);
	void setPosition(qreal x, qreal y);
	void setX(qreal x);
	void setY(qreal y);
	QPointF position() const;
	qreal x() const;
	qreal y() const;

	void setTransform(const QTransform& transform, bool combine = false);
	QTransform transform() const;
	QTransform transformInverted() const;

	void addPoint(DrawingItemPoint* point);
	void insertPoint(int index, DrawingItemPoint* point);
	void removePoint(DrawingItemPoint* point);
	void clearPoints();
	QList<DrawingItemPoint*> points() const;

	DrawingItemPoint* pointAt(const QPointF& position) const;
	DrawingItemPoint* pointNearest(const QPointF& position) const;

	void setPlaceType(PlaceType type);
	PlaceType placeType() const;

	QPointF mapFromScene(const QPointF& point) const;
	QPolygonF mapFromScene(const QRectF& rect) const;
	QPolygonF mapFromScene(const QPolygonF& polygon) const;
	QPainterPath mapFromScene(const QPainterPath& path) const;
	QPointF mapToScene(const QPointF& point) const;
	QPolygonF mapToScene(const QRectF& rect) const;
	QPolygonF mapToScene(const QPolygonF& polygon) const;
	QPainterPath mapToScene(const QPainterPath& path) const;

	void setSelected(bool selected);
	bool isSelected() const;

	virtual void setProperties(const QHash<QString,QVariant>& properties);
	virtual QHash<QString,QVariant> properties() const;

	virtual QRectF boundingRect() const = 0;
	virtual QPainterPath shape() const;
	virtual QPointF centerPosition() const;
	virtual bool isValid() const;

	virtual void render(QPainter* painter) = 0;

	virtual void move(const QPointF& position);
	virtual void resize(DrawingItemPoint* point, const QPointF& position);
	virtual void rotate(const QPointF& position);
	virtual void rotateBack(const QPointF& position);
	virtual void flipHorizontal(const QPointF& position);
	virtual void flipVertical(const QPointF& position);
	virtual DrawingItemPoint* insertNewPoint(const QPointF& position);
	virtual DrawingItemPoint* removeExistingPoint(const QPointF& position, int& pointIndex);

	virtual void writeToXml(QXmlStreamWriter* xml);
	virtual void readFromXml(QXmlStreamReader* xml);

	void writeTransformToXml(QXmlStreamWriter* xml, const QString& name);
	void writeBrushToXml(QXmlStreamWriter* xml, const QString& name, const QBrush& brush);
	void writePenToXml(QXmlStreamWriter* xml, const QString& name, const QPen& pen);
	void writeArrowToXml(QXmlStreamWriter* xml, const QString& name, const DrawingArrow& arrow);
	void writeFontToXml(QXmlStreamWriter* xml, const QString& name, const QFont& font);
	void writeAlignmentToXml(QXmlStreamWriter* xml, const QString& name, Qt::Alignment alignment);
	void readTransformFromXml(QXmlStreamReader* xml, const QString& name);
	void readBrushFromXml(QXmlStreamReader* xml, const QString& name, QBrush& brush);
	void readPenFromXml(QXmlStreamReader* xml, const QString& name, QPen& pen);
	void readArrowFromXml(QXmlStreamReader* xml, const QString& name, const QPen& pen, DrawingArrow& arrow);
	void readFontFromXml(QXmlStreamReader* xml, const QString& name, QFont& font);
	void readAlignmentFromXml(QXmlStreamReader* xml, const QString& name, Qt::Alignment& alignment);

private:
	QString colorToString(const QColor& color) const;
	QColor colorFromString(const QString& str) const;

public:
	QPainterPath strokePath(const QPainterPath& path, const QPen& pen) const;
	void adjustFirstPointToOrigin();

public:
	static DrawingItemFactory factory;

	static QList<DrawingItem*> copyItems(const QList<DrawingItem*>& items);
};

#endif
