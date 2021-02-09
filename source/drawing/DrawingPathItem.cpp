// DrawingPathItem.cpp
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

#include "DrawingPathItem.h"

DrawingPathItem::DrawingPathItem() : DrawingItem()
{
	for(int i = 0; i < 8; i++)
		addPoint(new DrawingItemPoint(QPointF(0, 0), DrawingItemPoint::Control));

	mRect = QRectF(0, 0, 0, 0);
	mPen = QPen(Qt::black, 16, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

	mPathName = "Path";

	updateItemGeometry();
}

DrawingPathItem::DrawingPathItem(const DrawingPathItem& item) : DrawingItem(item)
{
	mRect = item.mRect;
	mPen = item.mPen;

	mPathName = item.mPathName;
	mPath = item.mPath;
	mPathRect = item.mPathRect;

	QList<DrawingItemPoint*> points = DrawingPathItem::points();
	QList<DrawingItemPoint*> otherItemPoints = item.points();
	for(int i = 8; i < points.size(); i++)
		mPathConnectionPoints[points[i]] = item.mPathConnectionPoints[otherItemPoints[i]];

	mBoundingRect = item.mBoundingRect;
	mShape = item.mShape;
	mTransformedPath = item.mTransformedPath;
}

DrawingPathItem::~DrawingPathItem() { }

//==================================================================================================

QString DrawingPathItem::uniqueKey() const
{
	return "path";
}

DrawingItem* DrawingPathItem::copy() const
{
	return new DrawingPathItem(*this);
}

//==================================================================================================

void DrawingPathItem::setRect(const QRectF& rect)
{
	mRect = rect;

	// Update points
	QList<DrawingItemPoint*> points = DrawingPathItem::points();
	if (points.size() >= 8)
	{
		points[TopLeft]->setPosition(rect.left(), rect.top());
		points[TopMiddle]->setPosition(rect.center().x(), rect.top());
		points[TopRight]->setPosition(rect.right(), rect.top());
		points[MiddleRight]->setPosition(rect.right(), rect.center().y());
		points[BottomRight]->setPosition(rect.right(), rect.bottom());
		points[BottomMiddle]->setPosition(rect.center().x(), rect.bottom());
		points[BottomLeft]->setPosition(rect.left(), rect.bottom());
		points[MiddleLeft]->setPosition(rect.left(), rect.center().y());
	}

	updateItemGeometry();
}

QRectF DrawingPathItem::rect() const
{
	return mRect;
}

//==================================================================================================

void DrawingPathItem::setPen(const QPen& pen)
{
	mPen = pen;
	updateItemGeometry();
}

QPen DrawingPathItem::pen() const
{
	return mPen;
}

//==================================================================================================

void DrawingPathItem::setPathName(const QString& name)
{
	mPathName = name;
}

QString DrawingPathItem::pathName() const
{
	return mPathName;
}

//==================================================================================================

void DrawingPathItem::setPath(const QPainterPath& path, const QRectF& pathRect)
{
	mPath = path;
	mPathRect = pathRect;
	updateItemGeometry();
}

QPainterPath DrawingPathItem::path() const
{
	return mPath;
}

QRectF DrawingPathItem::pathRect() const
{
	return mPathRect;
}

//==================================================================================================

void DrawingPathItem::addConnectionPoint(const QPointF& pathPos)
{
	bool existingPointFound = false;
	QPointF itemPos = mapFromPath(pathPos);
	QList<DrawingItemPoint*> points = DrawingPathItem::points();

	for(auto pointIter = points.begin(); !existingPointFound && pointIter != points.end(); pointIter++)
	{
		existingPointFound = ((*pointIter)->position() == itemPos);
		if (existingPointFound) (*pointIter)->setType(DrawingItemPoint::ControlAndConnection);
	}

	if (!existingPointFound)
	{
		DrawingItemPoint* newPoint = new DrawingItemPoint(itemPos, DrawingItemPoint::Connection);
		mPathConnectionPoints[newPoint] = pathPos;
		addPoint(newPoint);
	}
}

	void DrawingPathItem::addConnectionPoints(const QPolygonF& pathPos)
{
	for(auto posIter = pathPos.begin(); posIter != pathPos.end(); posIter++)
		addConnectionPoint(*posIter);
}

QPolygonF DrawingPathItem::connectionPoints() const
{
	QPolygonF pathPos;
	QList<DrawingItemPoint*> points = DrawingPathItem::points();

	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
	{
		if ((*pointIter)->isConnectionPoint())
			pathPos.append(mapToPath((*pointIter)->position()));
	}

	return pathPos;
}

//==================================================================================================

QPointF DrawingPathItem::mapToPath(const QPointF& itemPos) const
{
	QPointF pathPos;

	pathPos.setX((itemPos.x() - mRect.left()) / mRect.width() * mPathRect.width() + mPathRect.left());
	pathPos.setY((itemPos.y() - mRect.top()) / mRect.height() * mPathRect.height() + mPathRect.top());

	return pathPos;
}

QRectF DrawingPathItem::mapToPath(const QRectF& itemRect) const
{
	return QRectF(mapToPath(itemRect.topLeft()), mapToPath(itemRect.bottomRight()));
}

QPointF DrawingPathItem::mapFromPath(const QPointF& pathPos) const
{
	QPointF itemPos;

	itemPos.setX((pathPos.x() - mPathRect.left()) / mPathRect.width() * mRect.width() + mRect.left());
	itemPos.setY((pathPos.y() - mPathRect.top()) / mPathRect.height() * mRect.height() + mRect.top());

	return itemPos;
}

QRectF DrawingPathItem::mapFromPath(const QRectF& pathRect) const
{
	return QRectF(mapFromPath(pathRect.topLeft()), mapFromPath(pathRect.bottomRight()));
}

//==================================================================================================

void DrawingPathItem::setProperties(const QHash<QString,QVariant>& properties)
{
	if (properties.contains("pen"))
	{
		QVariant variant = properties.value("pen");
		if (variant.canConvert<QPen>()) mPen = variant.value<QPen>();
	}

	if (properties.contains("rect"))
	{
		QVariant variant = properties.value("rect");
		if (variant.canConvert<QRectF>())
		{
			QRectF rect = variant.value<QRectF>();
			setRect(QRectF(mapFromScene(rect.topLeft()), mapFromScene(rect.bottomRight())));
		}
		else updateItemGeometry();
	}
	else updateItemGeometry();}

QHash<QString,QVariant> DrawingPathItem::properties() const
{
	QHash<QString,QVariant> properties;

	properties.insert("pen", mPen);
	properties.insert("rect", QRectF(mapToScene(mRect.topLeft()), mapToScene(mRect.bottomRight())));

	return properties;
}

//==================================================================================================

QRectF DrawingPathItem::boundingRect() const
{
	return mBoundingRect;
}

QPainterPath DrawingPathItem::shape() const
{
	return mShape;
}

bool DrawingPathItem::isValid() const
{
	return (mRect.width() != 0 && mRect.height() != 0 && !mPath.isEmpty() && !mPathRect.isNull());
}

//==================================================================================================

void DrawingPathItem::render(QPainter* painter)
{
	if (isValid())
	{
		QBrush sceneBrush = painter->brush();
		QPen scenePen = painter->pen();

		// Draw rect
		painter->setBrush(Qt::transparent);
		painter->setPen(mPen);
		painter->drawPath(mTransformedPath);

		painter->setBrush(sceneBrush);
		painter->setPen(scenePen);
	}
}

//==================================================================================================

void DrawingPathItem::resize(DrawingItemPoint* point, const QPointF& position)
{
	QList<DrawingItemPoint*> points = DrawingPathItem::points();

	if (point == nullptr && points.size() > BottomRight) point = points[BottomRight];

	DrawingItem::resize(point, position);
	adjustFirstPointToOrigin();

	if (points.size() >= 8)
	{
		int pointIndex = points.indexOf(point);

		if (0 <= pointIndex && pointIndex < 8)
		{
			QRectF rect;

			rect.setTopLeft(points[TopLeft]->position());
			rect.setBottomRight(points[BottomRight]->position());

			switch (pointIndex)
			{
			case TopLeft: rect.setTopLeft(point->position()); break;
			case TopMiddle:	rect.setTop(point->y()); break;
			case TopRight: rect.setTopRight(point->position()); break;
			case MiddleRight: rect.setRight(point->x()); break;
			case BottomRight: rect.setBottomRight(point->position()); break;
			case BottomMiddle: rect.setBottom(point->y()); break;
			case BottomLeft: rect.setBottomLeft(point->position()); break;
			case MiddleLeft: rect.setLeft(point->x()); break;
			default: break;
			}

			setRect(rect);
		}
	}

	// Adjust position of connection points
	for(auto keyIter = mPathConnectionPoints.begin(); keyIter != mPathConnectionPoints.end(); keyIter++)
		keyIter.key()->setPosition(mapFromPath(keyIter.value()));
}

//==================================================================================================

void DrawingPathItem::writeToXml(QXmlStreamWriter* xml)
{
	if (xml)
	{
		if (name() != "") xml->writeAttribute("name", name());

		xml->writeAttribute("path-name", mPathName);

		writeTransformToXml(xml, "transform");

		if (mRect.left() != 0) xml->writeAttribute("left", QString::number(mRect.left()));
		if (mRect.top() != 0) xml->writeAttribute("top", QString::number(mRect.top()));
		xml->writeAttribute("width", QString::number(mRect.width()));
		xml->writeAttribute("height", QString::number(mRect.height()));

		writePenToXml(xml, "pen", mPen);

		xml->writeAttribute("view-left", QString::number(mPathRect.left()));
		xml->writeAttribute("view-top", QString::number(mPathRect.top()));
		xml->writeAttribute("view-width", QString::number(mPathRect.width()));
		xml->writeAttribute("view-height", QString::number(mPathRect.height()));

		xml->writeAttribute("d", pathToString(mPath));

		QString glueStr = pointsToString(connectionPoints());
		if (!glueStr.isEmpty()) xml->writeAttribute("glue-points", glueStr);
	}
}

void DrawingPathItem::readFromXml(QXmlStreamReader* xml)
{
	if (xml)
	{
		QXmlStreamAttributes attr = xml->attributes();
		QRectF rect;

		if (attr.hasAttribute("name")) setName(attr.value("name").toString());

		if (attr.hasAttribute("path-name")) mPathName = attr.value("path-name").toString();

		readTransformFromXml(xml, "transform");

		readPenFromXml(xml, "pen", mPen);

		mPath = QPainterPath();
		mPathRect = QRectF();
		if (attr.hasAttribute("view-left")) mPathRect.setLeft(attr.value("view-left").toDouble());
		if (attr.hasAttribute("view-top")) mPathRect.setTop(attr.value("view-top").toDouble());
		if (attr.hasAttribute("view-width")) mPathRect.setWidth(attr.value("view-width").toDouble());
		if (attr.hasAttribute("view-height")) mPathRect.setHeight(attr.value("view-height").toDouble());
		if (attr.hasAttribute("d"))	mPath = pathFromString(attr.value("d").toString());

		if (attr.hasAttribute("glue-points"))
			addConnectionPoints(pointsFromString(attr.value("glue-points").toString()));

		// Do this last so that we ensure a call to updateItemGeometry before exiting this function
		if (attr.hasAttribute("left")) rect.setLeft(attr.value("left").toDouble());
		if (attr.hasAttribute("top")) rect.setTop(attr.value("top").toDouble());
		if (attr.hasAttribute("width")) rect.setWidth(attr.value("width").toDouble());
		if (attr.hasAttribute("height")) rect.setHeight(attr.value("height").toDouble());
		setRect(rect);

		xml->skipCurrentElement();
	}
}

//==================================================================================================

void DrawingPathItem::updateItemGeometry()
{
	mBoundingRect = QRectF();
	mShape = QPainterPath();
	mTransformedPath = QPainterPath();

	if (isValid())
	{
		qreal halfPenWidth = mPen.widthF() / 2;
		QRectF normalizedRect = mRect.normalized();
		QPainterPath drawPath;

		// Transformed path
		mTransformedPath = transformedPath();

		// Bounding rect
		mBoundingRect = normalizedRect;
		mBoundingRect.adjust(-halfPenWidth, -halfPenWidth, halfPenWidth, halfPenWidth);

		// Shape
		//drawPath = mTransformedPath;
		//mShape = strokePath(drawPath, mPen);
		//mShape.addPath(drawPath);

		mShape.addRect(mBoundingRect);
	}
}

//==================================================================================================

QPainterPath DrawingPathItem::transformedPath() const
{
	QPainterPath transformedPath;
	QList<QPointF> curveDataPoints;

	for(int i = 0; i < mPath.elementCount(); i++)
	{
		QPainterPath::Element element = mPath.elementAt(i);

		switch (element.type)
		{
		case QPainterPath::MoveToElement:
			transformedPath.moveTo(mapFromPath(QPointF(element.x, element.y)));
			break;
		case QPainterPath::LineToElement:
			transformedPath.lineTo(mapFromPath(QPointF(element.x, element.y)));
			break;
		case QPainterPath::CurveToElement:
			curveDataPoints.append(mapFromPath(QPointF(element.x, element.y)));
			break;
		case QPainterPath::CurveToDataElement:
			if (curveDataPoints.size() >= 2)
			{
				transformedPath.cubicTo(curveDataPoints[0], curveDataPoints[1],
					mapFromPath(QPointF(element.x, element.y)));
				curveDataPoints.pop_front();
				curveDataPoints.pop_front();
			}
			else curveDataPoints.append(mapFromPath(QPointF(element.x, element.y)));
			break;
		}
	}

	return transformedPath;
}

//==================================================================================================

QString DrawingPathItem::pathToString(const QPainterPath& path) const
{
	QString pathStr;

	for(int i = 0; i < path.elementCount(); i++)
	{
		QPainterPath::Element element = path.elementAt(i);

		switch (element.type)
		{
		case QPainterPath::MoveToElement:
			pathStr += "M " + QString::number(element.x) + " " + QString::number(element.y) + " ";
			break;
		case QPainterPath::LineToElement:
			pathStr += "L " + QString::number(element.x) + " " + QString::number(element.y) + " ";
			break;
		case QPainterPath::CurveToElement:
			pathStr += "C " + QString::number(element.x) + " " + QString::number(element.y) + " ";
			break;
		case QPainterPath::CurveToDataElement:
			pathStr += QString::number(element.x) + " " + QString::number(element.y) + " ";
			break;
		}
	}

	return pathStr.trimmed();
}

QPainterPath DrawingPathItem::pathFromString(const QString& str) const
{
	QPainterPath path;

	QStringList tokenList = str.split(" ", QString::SkipEmptyParts);
	qreal x, y, x1, y1, x2, y2;
	bool xOk = false, yOk = false, x1Ok = false, x2Ok = false, y1Ok = false, y2Ok = false;

	for(int i = 0; i < tokenList.size(); i++)
	{
		if (tokenList[i] == "M" && i+2 < tokenList.size())
		{
			x = tokenList[i+1].toDouble(&xOk);
			y = tokenList[i+2].toDouble(&yOk);
			if (xOk && yOk) path.moveTo(x, y);
		}
		else if (tokenList[i] == "L" && i+2 < tokenList.size())
		{
			x = tokenList[i+1].toDouble(&xOk);
			y = tokenList[i+2].toDouble(&yOk);
			if (xOk && yOk) path.lineTo(x, y);
		}
		else if (tokenList[i] == "C" && i+6 < tokenList.size())
		{
			x1 = tokenList[i+1].toDouble(&x1Ok);
			y1 = tokenList[i+2].toDouble(&y1Ok);
			x2 = tokenList[i+3].toDouble(&x2Ok);
			y2 = tokenList[i+4].toDouble(&y2Ok);
			x = tokenList[i+5].toDouble(&xOk);
			y = tokenList[i+6].toDouble(&yOk);
			if (x1Ok && y1Ok && x2Ok && y2Ok && xOk && yOk) path.cubicTo(x1, y1, x2, y2, x, y);
		}
	}

	return path;
}
