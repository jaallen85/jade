/* DrawingItem.cpp
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

#include "DrawingItem.h"
#include "DrawingWidget.h"
#include "DrawingItemPoint.h"

DrawingItem::DrawingItem()
{
	mDrawing = nullptr;

	mRotation = 0;
	mFlipped = false;

	mSelected = false;
	mVisible = true;

	mSelectedPoint = nullptr;

	recalculateTransform();
}

DrawingItem::DrawingItem(const DrawingItem& item)
{
	mDrawing = nullptr;

	mPos = item.mPos;
	mRotation = item.mRotation;
	mFlipped = item.mFlipped;

	mProperties = item.mProperties;

	mSelected = false;
	mVisible = true;

	for(auto pointIter = item.mPoints.begin(); pointIter != item.mPoints.end(); pointIter++)
		addPoint((*pointIter)->copy());
	mSelectedPoint = nullptr;

	recalculateTransform();
}

DrawingItem::~DrawingItem()
{
	clearPoints();
	mSelectedPoint = nullptr;
	mDrawing = nullptr;
}

//==================================================================================================

DrawingWidget* DrawingItem::drawing() const
{
	return mDrawing;
}

//==================================================================================================

void DrawingItem::setPos(const QPointF& pos)
{
	mPos = pos;
}

void DrawingItem::setPos(qreal x, qreal y)
{
	setPos(QPointF(x, y));
}

void DrawingItem::setX(qreal x)
{
	setPos(QPointF(x, y()));
}

void DrawingItem::setY(qreal y)
{
	setPos(QPointF(x(), y));
}

QPointF DrawingItem::pos() const
{
	return mPos;
}

qreal DrawingItem::x() const
{
	return mPos.x();
}

qreal DrawingItem::y() const
{
	return mPos.y();
}

//==================================================================================================

void DrawingItem::setRotation(qreal angle)
{
	mRotation = angle;
	while (mRotation >= 360.0) mRotation -= 360.0;
	while (mRotation < 0.0) mRotation += 360.0;
}

void DrawingItem::setFlipped(bool flipped)
{
	mFlipped = flipped;
}

qreal DrawingItem::rotation() const
{
	return mRotation;
}

bool DrawingItem::isFlipped() const
{
	return mFlipped;
}

//==================================================================================================

void DrawingItem::setSelected(bool selected)
{
	mSelected = selected;
}

void DrawingItem::setVisible(bool visible)
{
	mVisible = visible;
}

bool DrawingItem::isSelected() const
{
	return mSelected;
}

bool DrawingItem::isVisible() const
{
	return mVisible;
}

//==================================================================================================

void DrawingItem::setProperty(const QString& name, const QVariant& value)
{
	mProperties[name] = value;
}

QVariant DrawingItem::property(const QString& name) const
{
	return mProperties.contains(name) ? mProperties[name] : QVariant();
}

bool DrawingItem::hasProperty(const QString& name) const
{
	return mProperties.contains(name);
}

//==================================================================================================

void DrawingItem::addPoint(DrawingItemPoint* itemPoint)
{
	if (itemPoint && !mPoints.contains(itemPoint))
	{
		mPoints.append(itemPoint);
		itemPoint->mItem = this;
	}
}

void DrawingItem::insertPoint(int index, DrawingItemPoint* itemPoint)
{
	if (itemPoint && !mPoints.contains(itemPoint))
	{
		mPoints.insert(index, itemPoint);
		itemPoint->mItem = this;
	}
}

void DrawingItem::removePoint(DrawingItemPoint* itemPoint)
{
	if (itemPoint && mPoints.contains(itemPoint))
	{
		mPoints.removeAll(itemPoint);
		itemPoint->mItem = nullptr;
	}
}

void DrawingItem::clearPoints()
{
	DrawingItemPoint* itemPoint = nullptr;

	while (!mPoints.empty())
	{
		itemPoint = mPoints.first();
		removePoint(itemPoint);
		delete itemPoint;
		itemPoint = nullptr;
	}
}

QList<DrawingItemPoint*> DrawingItem::points() const
{
	return mPoints;
}

DrawingItemPoint* DrawingItem::selectedPoint() const
{
	return mSelectedPoint;
}

//==================================================================================================

DrawingItemPoint* DrawingItem::pointAt(const QPointF& itemPos) const
{
	DrawingItemPoint* itemPoint = nullptr;

	for(auto pointIter = mPoints.begin(); itemPoint == nullptr && pointIter != mPoints.end(); pointIter++)
	{
		if ((*pointIter)->itemRect().contains(itemPos))
			itemPoint = *pointIter;
	}

	return itemPoint;
}

DrawingItemPoint* DrawingItem::pointNearest(const QPointF& itemPos) const
{
	DrawingItemPoint *itemPoint = nullptr;

	if (!mPoints.isEmpty())
	{
		itemPoint = mPoints.first();

		QPointF vec = (itemPoint->pos() - itemPos);
		qreal minimumDistanceSq = vec.x() * vec.x() + vec.y() * vec.y();
		qreal distanceSq;

		for(auto pointIter = mPoints.begin() + 1; pointIter != mPoints.end(); pointIter++)
		{
			vec = ((*pointIter)->pos() - itemPos);
			distanceSq = vec.x() * vec.x() + vec.y() * vec.y();

			if (distanceSq < minimumDistanceSq)
			{
				itemPoint = *pointIter;
				minimumDistanceSq = distanceSq;
			}
		}
	}

	return itemPoint;
}

//==================================================================================================

QPointF DrawingItem::mapFromScene(const QPointF& point) const
{
	return mTransform.map(point - mPos);
}

QRectF DrawingItem::mapFromScene(const QRectF& rect) const
{
	return mapFromScene(QPolygonF(rect)).boundingRect();
}

QPolygonF DrawingItem::mapFromScene(const QPolygonF& polygon) const
{
	QPolygonF poly = polygon;
	poly.translate(-mPos);
	return mTransform.map(poly);
}

QPainterPath DrawingItem::mapFromScene(const QPainterPath& path) const
{
	QPainterPath painterPath = path;
	painterPath.translate(-mPos);
	return mTransform.map(painterPath);
}

QPointF DrawingItem::mapToScene(const QPointF& point) const
{
	return mTransformInverse.map(point) + mPos;
}

QRectF DrawingItem::mapToScene(const QRectF& rect) const
{
	return mapToScene(QPolygonF(rect)).boundingRect();
}

QPolygonF DrawingItem::mapToScene(const QPolygonF& polygon) const
{
	QPolygonF poly = mTransformInverse.map(polygon);
	poly.translate(mPos);
	return poly;
}

QPainterPath DrawingItem::mapToScene(const QPainterPath& path) const
{
	QPainterPath painterPath = mTransformInverse.map(path);
	painterPath.translate(mPos);
	return painterPath;
}

//==================================================================================================

void DrawingItem::recalculateTransform()
{
	mTransform = QTransform();
	mTransform = mTransform.rotate(-mRotation);
	if (mFlipped) mTransform = mTransform.scale(-1.0, 1.0);

	mTransformInverse = mTransform.inverted();
}

QTransform DrawingItem::transform() const
{
	return mTransform;
}

//==================================================================================================

QPainterPath DrawingItem::shape() const
{
	QPainterPath path;
	path.addRect(boundingRect());
	return path;
}

QPointF DrawingItem::centerPos() const
{
	return boundingRect().center();
}

bool DrawingItem::isValid() const
{
	return boundingRect().isValid();
}

//==================================================================================================

void DrawingItem::moveItem(const QPointF& scenePos)
{
	setPos(scenePos);
}

void DrawingItem::resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos)
{
	if (itemPoint) itemPoint->setPos(mapFromScene(scenePos));
}

void DrawingItem::rotateItem(const QPointF& scenePos)
{
	QPointF difference(mPos - scenePos);

	// Calculate new position of reference point
	setPos(scenePos.x() - difference.y(), scenePos.y() + difference.x());

	// Update orientation
	if (isFlipped()) setRotation(rotation() - 90.0);
	else setRotation(rotation() + 90.0);

	recalculateTransform();
}

void DrawingItem::rotateBackItem(const QPointF& scenePos)
{
	QPointF difference(mPos - scenePos);

	// Calculate new position of reference point
	setPos(scenePos.x() + difference.y(), scenePos.y() - difference.x());

	// Update orientation
	if (isFlipped()) setRotation(rotation() + 90.0);
	else setRotation(rotation() - 90.0);

	recalculateTransform();
}

void DrawingItem::flipItem(const QPointF& scenePos)
{
	// Calculate new position of reference point
	setPos(2 * scenePos.x() - x(), y());

	// Update orientation
	mFlipped = !mFlipped;

	recalculateTransform();
}

int DrawingItem::insertItemPoint(DrawingItemPoint* itemPoint)
{
	Q_UNUSED(itemPoint);
	return -1;
}

bool DrawingItem::removeItemPoint(DrawingItemPoint* itemPoint)
{
	Q_UNUSED(itemPoint);
	return false;
}

void DrawingItem::updateProperties(const QMap<QString,QVariant>& properties)
{
	QStringList keys = properties.keys();
	for(auto keyIter = keys.begin(); keyIter != keys.end(); keyIter++)
	{
		if (mProperties.contains(*keyIter)) mProperties[*keyIter] = properties[*keyIter];
	}
}

//==================================================================================================

void DrawingItem::mousePressEvent(DrawingMouseEvent* event)
{
	mSelectedPoint = pointAt(mapFromScene(event->scenePos()));
	if (mSelectedPoint && !mSelectedPoint->isControlPoint()) mSelectedPoint = nullptr;
}

void DrawingItem::mouseMoveEvent(DrawingMouseEvent* event)
{
	Q_UNUSED(event);
}

void DrawingItem::mouseReleaseEvent(DrawingMouseEvent* event)
{
	mSelectedPoint = nullptr;
	Q_UNUSED(event);
}

//==================================================================================================

void DrawingItem::keyPressEvent(QKeyEvent* event)
{
	Q_UNUSED(event);
}

void DrawingItem::keyReleaseEvent(QKeyEvent* event)
{
	Q_UNUSED(event);
}

//==================================================================================================

bool DrawingItem::newItemCreateEvent(QWidget* widget)
{
	Q_UNUSED(widget);
	return true;
}

bool DrawingItem::newItemCopyEvent()
{
	return true;
}

void DrawingItem::newMousePressEvent(DrawingMouseEvent* event)
{
	if (mDrawing) setPos(mDrawing->roundToGrid(event->scenePos()));
	else setPos(event->scenePos());
}

void DrawingItem::newMouseMoveEvent(DrawingMouseEvent* event)
{
	if (mDrawing) setPos(mDrawing->roundToGrid(event->scenePos()));
	else setPos(event->scenePos());
}

bool DrawingItem::newMouseReleaseEvent(DrawingMouseEvent* event)
{
	if (mDrawing) setPos(mDrawing->roundToGrid(event->scenePos()));
	else setPos(event->scenePos());

	return true;
}

bool DrawingItem::newMouseDoubleClickEvent(DrawingMouseEvent* event)
{
	Q_UNUSED(event);
	return false;
}

//==================================================================================================

QPainterPath DrawingItem::strokePath(const QPainterPath& path, const QPen& pen) const
{
	if (path == QPainterPath()) return path;

	QPainterPathStroker ps;
	const qreal penWidthZero = qreal(0.00000001);

	if (pen.widthF() <= 0.0)
		ps.setWidth(penWidthZero);
	else
		ps.setWidth(pen.widthF());

	//ps.setCapStyle(pen.capStyle());
	//ps.setJoinStyle(pen.joinStyle());
	//ps.setMiterLimit(pen.miterLimit());

	ps.setCapStyle(Qt::SquareCap);
	ps.setJoinStyle(Qt::BevelJoin);

	return ps.createStroke(path);
}

//==================================================================================================

QList<DrawingItem*> DrawingItem::copyItems(const QList<DrawingItem*>& items)
{
	QList<DrawingItem*> copiedItems;
	QList<DrawingItemPoint*> itemPoints;
	QList<DrawingItemPoint*> targetPoints;
	DrawingItem* targetItem;
	DrawingItem* copiedTargetItem;
	DrawingItemPoint* copiedTargetPoint;
	DrawingItemPoint* copiedPoint;

	// Copy items
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		copiedItems.append((*itemIter)->copy());

	// Maintain connections to other items in this list
	for(int itemIndex = 0; itemIndex < items.size(); itemIndex++)
	{
		itemPoints = items[itemIndex]->points();
		for(int pointIndex = 0; pointIndex < itemPoints.size(); pointIndex++)
		{
			targetPoints = itemPoints[pointIndex]->connections();
			for(auto targetIter = targetPoints.begin(); targetIter != targetPoints.end(); targetIter++)
			{
				targetItem = (*targetIter)->item();
				if (items.contains(targetItem))
				{
					// There is a connection here that must be maintained in the copied items
					copiedPoint = copiedItems[itemIndex]->points().at(pointIndex);

					copiedTargetItem = copiedItems[items.indexOf(targetItem)];
					copiedTargetPoint =
						copiedTargetItem->points().at(targetItem->points().indexOf(*targetIter));

					if (copiedPoint && copiedTargetPoint)
					{
						copiedPoint->addConnection(copiedTargetPoint);
						copiedTargetPoint->addConnection(copiedPoint);
					}
				}
			}
		}
	}

	return copiedItems;
}
