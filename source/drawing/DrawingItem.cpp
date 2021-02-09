// DrawingItem.cpp
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

#include "DrawingItem.h"
#include "DrawingWidget.h"
#include <QtMath>

DrawingItem::DrawingItem()
{
	mWidget = nullptr;
	mPlaceType = PlaceByMouseRelease;
	mSelected = false;
}

DrawingItem::DrawingItem(const DrawingItem& item)
{
	mWidget = nullptr;

	mPosition = item.mPosition;
	mTransform = item.mTransform;
	mTransformInverse = item.mTransformInverse;

	for(auto pointIter = item.mPoints.begin(); pointIter != item.mPoints.end(); pointIter++)
		addPoint(new DrawingItemPoint(**pointIter));

	mPlaceType = item.mPlaceType;

	mSelected = false;
}

DrawingItem::~DrawingItem()
{
	clearPoints();
	mWidget = nullptr;
}

//==================================================================================================

DrawingWidget* DrawingItem::widget() const
{
	return mWidget;
}

//==================================================================================================

void DrawingItem::setName(const QString& name)
{
	mName = name;
}

QString DrawingItem::name() const
{
	return mName;
}

//==================================================================================================

void DrawingItem::setPosition(const QPointF& position)
{
	mPosition = position;
}

void DrawingItem::setPosition(qreal x, qreal y)
{
	mPosition.setX(x);
	mPosition.setY(y);
}

void DrawingItem::setX(qreal x)
{
	mPosition.setX(x);
}

void DrawingItem::setY(qreal y)
{
	mPosition.setY(y);
}

QPointF DrawingItem::position() const
{
	return mPosition;
}

qreal DrawingItem::x() const
{
	return mPosition.x();
}

qreal DrawingItem::y() const
{
	return mPosition.y();
}

//==================================================================================================

void DrawingItem::setTransform(const QTransform& transform, bool combine)
{
	if (combine) mTransform = mTransform * transform;
	else mTransform = transform;

	mTransformInverse = mTransform.inverted();
}

QTransform DrawingItem::transform() const
{
	return mTransform;
}

QTransform DrawingItem::transformInverted() const
{
	return mTransformInverse;
}

//==================================================================================================

void DrawingItem::addPoint(DrawingItemPoint* point)
{
	if (point && point->mItem == nullptr)
	{
		mPoints.append(point);
		point->mItem = this;
	}
}

void DrawingItem::insertPoint(int index, DrawingItemPoint* point)
{
	if (point && point->mItem == nullptr)
	{
		mPoints.insert(index, point);
		point->mItem = this;
	}
}

void DrawingItem::removePoint(DrawingItemPoint* point)
{
	if (point && point->mItem == this)
	{
		mPoints.removeAll(point);
		point->mItem = nullptr;
	}
}

void DrawingItem::clearPoints()
{
	DrawingItemPoint* point = nullptr;

	while (!mPoints.empty())
	{
		point = mPoints.first();
		removePoint(point);
		delete point;
		point = nullptr;
	}
}

QList<DrawingItemPoint*> DrawingItem::points() const
{
	return mPoints;
}

//==================================================================================================

DrawingItemPoint* DrawingItem::pointAt(const QPointF& position) const
{
	DrawingItemPoint* itemPoint = nullptr;

	QList<DrawingItemPoint*> itemPoints = points();
	for(auto pointIter = itemPoints.begin(); itemPoint == nullptr && pointIter != itemPoints.end(); pointIter++)
	{
		if (position == (*pointIter)->position()) itemPoint = *pointIter;
	}

	return itemPoint;
}

DrawingItemPoint* DrawingItem::pointNearest(const QPointF& position) const
{
	DrawingItemPoint *itemPoint = nullptr;

	QList<DrawingItemPoint*> itemPoints = points();
	if (!itemPoints.isEmpty())
	{
		itemPoint = itemPoints.first();

		QPointF vec = (itemPoint->position() - position);
		qreal minimumDistanceSq = vec.x() * vec.x() + vec.y() * vec.y();
		qreal distanceSq;

		for(auto pointIter = itemPoints.begin() + 1; pointIter != itemPoints.end(); pointIter++)
		{
			vec = ((*pointIter)->position() - position);
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

void DrawingItem::setPlaceType(PlaceType type)
{
	mPlaceType = type;
}

DrawingItem::PlaceType DrawingItem::placeType() const
{
	return mPlaceType;
}

//==================================================================================================

QPointF DrawingItem::mapFromScene(const QPointF& point) const
{
	return mTransform.map(point - mPosition);
}

QPolygonF DrawingItem::mapFromScene(const QRectF& rect) const
{
	return mapFromScene(QPolygonF(rect));
}

QPolygonF DrawingItem::mapFromScene(const QPolygonF& polygon) const
{
	QPolygonF poly = polygon;
	poly.translate(-mPosition);
	return mTransform.map(poly);
}

QPainterPath DrawingItem::mapFromScene(const QPainterPath& path) const
{
	QPainterPath painterPath = path;
	painterPath.translate(-mPosition);
	return mTransform.map(painterPath);
}

QPointF DrawingItem::mapToScene(const QPointF& point) const
{
	return mTransformInverse.map(point) + mPosition;
}

QPolygonF DrawingItem::mapToScene(const QRectF& rect) const
{
	return mapToScene(QPolygonF(rect));
}

QPolygonF DrawingItem::mapToScene(const QPolygonF& polygon) const
{
	QPolygonF poly = mTransformInverse.map(polygon);
	poly.translate(mPosition);
	return poly;
}

QPainterPath DrawingItem::mapToScene(const QPainterPath& path) const
{
	QPainterPath painterPath = mTransformInverse.map(path);
	painterPath.translate(mPosition);
	return painterPath;
}

//==================================================================================================

void DrawingItem::setSelected(bool selected)
{
	mSelected = selected;
}

bool DrawingItem::isSelected() const
{
	return mSelected;
}

//==================================================================================================

void DrawingItem::setProperties(const QHash<QString,QVariant>& properties)
{
	Q_UNUSED(properties);
}

QHash<QString,QVariant> DrawingItem::properties() const
{
	return QHash<QString,QVariant>();
}

//==================================================================================================

QPainterPath DrawingItem::shape() const
{
	QPainterPath path;
	path.addRect(boundingRect());
	return path;
}

QPointF DrawingItem::centerPosition() const
{
	return boundingRect().center();
}

bool DrawingItem::isValid() const
{
	return boundingRect().isValid();
}

//==================================================================================================

void DrawingItem::move(const QPointF& position)
{
	mPosition = position;
}

void DrawingItem::resize(DrawingItemPoint* point, const QPointF& position)
{
	if (point) point->setPosition(mapFromScene(position));
}

void DrawingItem::rotate(const QPointF& position)
{
	QPointF difference(mPosition - position);

	// Calculate new position of reference point
	mPosition = QPointF(position.x() + difference.y(), position.y() - difference.x());

	// Update orientation
	mTransform.rotate(90);
	mTransformInverse = mTransform.inverted();
}

void DrawingItem::rotateBack(const QPointF& position)
{
	QPointF difference(mPosition - position);

	// Calculate new position of reference point
	mPosition = QPointF(position.x() - difference.y(), position.y() + difference.x());

	// Update orientation
	mTransform.rotate(-90);
	mTransformInverse = mTransform.inverted();
}

void DrawingItem::flipHorizontal(const QPointF& position)
{
	// Calculate new position of reference point
	mPosition.setX(2 * position.x() - mPosition.x());

	// Update orientation
	mTransform.scale(-1, 1);
	mTransformInverse = mTransform.inverted();
}

void DrawingItem::flipVertical(const QPointF& position)
{
	// Calculate new position of reference point
	mPosition.setY(2 * position.y() - mPosition.y());

	// Update orientation
	mTransform.scale(1, -1);
	mTransformInverse = mTransform.inverted();
}

DrawingItemPoint* DrawingItem::insertNewPoint(const QPointF& position)
{
	Q_UNUSED(position);
	return nullptr;
}

DrawingItemPoint* DrawingItem::removeExistingPoint(const QPointF& position, int& pointIndex)
{
	Q_UNUSED(position);
	pointIndex = -1;
	return nullptr;
}

//==================================================================================================

void DrawingItem::writeToXml(QXmlStreamWriter* xml)
{
	Q_UNUSED(xml);
}

void DrawingItem::readFromXml(QXmlStreamReader* xml)
{
	Q_UNUSED(xml);
}

//==================================================================================================

void DrawingItem::exportToSvg(QXmlStreamWriter* xml)
{
	Q_UNUSED(xml);
}

//==================================================================================================

void DrawingItem::writeTransformToXml(QXmlStreamWriter* xml, const QString& name)
{
	if (xml)
	{
		QString transformStr;

		QTransform transform = mTransform;

		qreal rotation = qAsin(transform.m12()) * 180 / 3.141592654;
		transform.rotate(-rotation);

		qreal hScale = transform.m11();
		qreal vScale = transform.m22();

		transformStr= "translate(" + QString::number(mPosition.x()) + "," + QString::number(mPosition.y()) + ")";
		transformStr += (rotation != 0) ? " rotate(" + QString::number(rotation) + ")" : "";
		transformStr += (hScale != 1.0 || vScale != 1.0) ? " scale(" + QString::number(hScale) + "," + QString::number(vScale) + ")" : "";

		xml->writeAttribute(name, transformStr);
	}
}

void DrawingItem::writeBrushToXml(QXmlStreamWriter* xml, const QString& name, const QBrush& brush)
{
	if (xml) xml->writeAttribute(name + "-color", colorToString(brush.color()));
}

void DrawingItem::writePenToXml(QXmlStreamWriter* xml, const QString& name, const QPen& pen)
{
	if (xml)
	{
		writeBrushToXml(xml, "pen", pen.brush());
		xml->writeAttribute(name + "-width", QString::number(pen.widthF()));

		if (pen.style() != Qt::SolidLine)
		{
			QString penStyleStr;

			switch (pen.style())
			{
			case Qt::NoPen: penStyleStr = "none"; break;
			case Qt::DashLine: penStyleStr = "dash"; break;
			case Qt::DotLine: penStyleStr = "dot"; break;
			case Qt::DashDotLine: penStyleStr = "dash-dot"; break;
			case Qt::DashDotDotLine: penStyleStr = "dash-dot-dot"; break;
			default: penStyleStr = "solid"; break;
			}

			xml->writeAttribute(name + "-style", penStyleStr);
		}
	}
}

void DrawingItem::writeArrowToXml(QXmlStreamWriter* xml, const QString& name, const DrawingArrow& arrow)
{
	if (xml)
	{
		if (arrow.style() != Drawing::ArrowNone)
		{
			QString arrowStyleStr;

			switch (arrow.style())
			{
			case Drawing::ArrowNormal: arrowStyleStr = "normal"; break;
			case Drawing::ArrowTriangle: arrowStyleStr = "triangle"; break;
			case Drawing::ArrowTriangleFilled: arrowStyleStr = "triangle-filled"; break;
			case Drawing::ArrowCircle: arrowStyleStr = "circle"; break;
			case Drawing::ArrowCircleFilled: arrowStyleStr = "circle-filled"; break;
			case Drawing::ArrowConcave: arrowStyleStr = "concave"; break;
			case Drawing::ArrowConcaveFilled: arrowStyleStr = "concave-filled"; break;
			default: arrowStyleStr = "none"; break;
			}

			xml->writeAttribute(name + "-style", arrowStyleStr);
		}

		if (arrow.size() != 0)
			xml->writeAttribute(name + "-size", QString::number(arrow.size()));
	}
}

void DrawingItem::writeFontToXml(QXmlStreamWriter* xml, const QString& name, const QFont& font)
{
	if (xml)
	{
		xml->writeAttribute(name + "-name", font.family());
		xml->writeAttribute(name + "-size", QString::number(font.pointSizeF()));
		if (font.bold()) xml->writeAttribute(name + "-bold", "true");
		if (font.italic()) xml->writeAttribute(name + "-italic", "true");
		if (font.underline()) xml->writeAttribute(name + "-underline", "true");
		if (font.strikeOut()) xml->writeAttribute(name + "-strike-through", "true");
	}
}

void DrawingItem::writeAlignmentToXml(QXmlStreamWriter* xml, const QString& name, Qt::Alignment alignment)
{
	Qt::Alignment hAlignment = (alignment & Qt::AlignHorizontal_Mask);
	Qt::Alignment vAlignment = (alignment & Qt::AlignVertical_Mask);

	if (hAlignment != Qt::AlignHCenter)
	{
		QString hString;

		if (hAlignment & Qt::AlignLeft) hString = "left";
		else if (hAlignment & Qt::AlignRight) hString = "right";
		else if (hAlignment & Qt::AlignHCenter) hString = "center";

		xml->writeAttribute(name + "-horizontal", hString);
	}

	if (vAlignment != Qt::AlignVCenter)
	{
		QString vString;

		if (vAlignment & Qt::AlignTop) vString = "top";
		else if (vAlignment & Qt::AlignBottom) vString = "bottom";
		else if (vAlignment & Qt::AlignVCenter) vString = "middle";

		xml->writeAttribute(name + "-vertical", vString);
	}
}

void DrawingItem::readTransformFromXml(QXmlStreamReader* xml, const QString& name)
{
	if (xml)
	{
		QXmlStreamAttributes attr = xml->attributes();

		setPosition(0, 0);
		setTransform(QTransform());

		if (attr.hasAttribute(name))
		{
			QString transformStr = attr.value(name).toString();
			QStringList tokens = transformStr.split(QRegExp("\\s+"));

			for(auto tokenIter = tokens.begin(); tokenIter != tokens.end(); tokenIter++)
			{
				if (tokenIter->startsWith("translate("))
				{
					int endIndex = tokenIter->indexOf(")");
					QStringList coords = tokenIter->mid(10, endIndex - 10).split(",");

					if (coords.size() == 2)
					{
						setX(coords.first().toDouble());
						setY(coords.last().toDouble());
					}
				}
				else if (tokenIter->startsWith("rotate("))
				{
					int endIndex = tokenIter->indexOf(")");
					QString angle = tokenIter->mid(7, endIndex - 7);

					QTransform transform;
					transform.rotate(angle.toDouble());
					setTransform(transform, true);
				}
				else if (tokenIter->startsWith("scale("))
				{
					int endIndex = tokenIter->indexOf(")");
					QStringList coords = tokenIter->mid(6, endIndex - 6).split(",");
					if (coords.size() == 2)
					{
						QTransform transform;
						transform.scale(coords.first().toDouble(), coords.last().toDouble());
						setTransform(transform, true);
					}
				}
			}
		}
	}
}

void DrawingItem::readBrushFromXml(QXmlStreamReader* xml, const QString& name, QBrush& brush)
{
	if (xml)
	{
		QXmlStreamAttributes attr = xml->attributes();

		brush = Qt::white;

		if (attr.hasAttribute(name + "-color"))
			brush = colorFromString(attr.value(name + "-color").toString());
	}
}

void DrawingItem::readPenFromXml(QXmlStreamReader* xml, const QString& name, QPen& pen)
{
	if (xml)
	{
		QXmlStreamAttributes attr = xml->attributes();

		pen = QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
		QBrush brush = pen.brush();

		readBrushFromXml(xml, "pen", brush);
		pen.setBrush(brush);

		if (attr.hasAttribute(name + "-width"))
			pen.setWidthF(attr.value(name + "-width").toDouble());

		if (attr.hasAttribute(name + "-style"))
		{
			QString penStyleStr = attr.value(name + "-style").toString().toLower();
			Qt::PenStyle penStyle = Qt::SolidLine;

			if (penStyleStr == "none") penStyle = Qt::NoPen;
			else if (penStyleStr == "dash") penStyle = Qt::DashLine;
			else if (penStyleStr == "dot") penStyle = Qt::DotLine;
			else if (penStyleStr == "dash-dot") penStyle = Qt::DashDotLine;
			else if (penStyleStr == "dash-dot-dot") penStyle = Qt::DashDotDotLine;

			pen.setStyle(penStyle);
		}
	}
}

void DrawingItem::readArrowFromXml(QXmlStreamReader* xml, const QString& name, const QPen& pen, DrawingArrow& arrow)
{
	if (xml)
	{
		QXmlStreamAttributes attr = xml->attributes();

		arrow.setStyle(Drawing::ArrowNone);
		arrow.setSize(0);
		arrow.setPen(pen);

		if (attr.hasAttribute(name + "-style"))
		{
			QString arrowStyleStr = attr.value(name + "-style").toString().toLower();
			Drawing::ArrowStyle arrowStyle = Drawing::ArrowNone;

			if (arrowStyleStr == "normal") arrowStyle = Drawing::ArrowNormal;
			else if (arrowStyleStr == "triangle") arrowStyle = Drawing::ArrowTriangle;
			else if (arrowStyleStr == "triangle-filled") arrowStyle = Drawing::ArrowTriangleFilled;
			else if (arrowStyleStr == "circle") arrowStyle = Drawing::ArrowCircle;
			else if (arrowStyleStr == "circle-filled") arrowStyle = Drawing::ArrowCircleFilled;
			else if (arrowStyleStr == "concave") arrowStyle = Drawing::ArrowConcave;
			else if (arrowStyleStr == "concave-filled") arrowStyle = Drawing::ArrowConcaveFilled;

			arrow.setStyle(arrowStyle);
		}

		if (attr.hasAttribute(name + "-size"))
			arrow.setSize(attr.value(name + "-size").toDouble());
	}
}

void DrawingItem::readFontFromXml(QXmlStreamReader* xml, const QString& name, QFont& font)
{
	if (xml)
	{
		QXmlStreamAttributes attr = xml->attributes();

		font = QFont();

		if (attr.hasAttribute(name + "-name"))
			font.setFamily(attr.value(name + "-name").toString());
		if (attr.hasAttribute(name + "-size"))
			font.setPointSizeF(attr.value(name + "-size").toDouble());
		if (attr.hasAttribute(name + "-bold"))
			font.setBold(attr.value(name + "-bold").toString().toLower() == "true");
		if (attr.hasAttribute(name + "-italic"))
			font.setItalic(attr.value(name + "-italic").toString().toLower() == "true");
		if (attr.hasAttribute(name + "-underline"))
			font.setUnderline(attr.value(name + "-underline").toString().toLower() == "true");
		if (attr.hasAttribute(name + "-strike-through"))
			font.setStrikeOut(attr.value(name + "-strike-through").toString().toLower() == "true");
	}
}

void DrawingItem::readAlignmentFromXml(QXmlStreamReader* xml, const QString& name, Qt::Alignment& alignment)
{
	if (xml)
	{
		QXmlStreamAttributes attr = xml->attributes();

		Qt::Alignment hAligment = Qt::AlignHCenter;
		Qt::Alignment vAligment = Qt::AlignVCenter;

		if (attr.hasAttribute(name + "-horizontal"))
		{
			QString hString = attr.value(name + "-horizontal").toString().toLower();

			if (hString == "left") hAligment = Qt::AlignLeft;
			else if (hString == "right") hAligment = Qt::AlignRight;
			else if (hString == "center") hAligment = Qt::AlignHCenter;
		}

		if (attr.hasAttribute(name + "-vertical"))
		{
			QString vString = attr.value(name + "-vertical").toString().toLower();

			if (vString == "top") vAligment = Qt::AlignTop;
			else if (vString == "bottom") vAligment = Qt::AlignBottom;
			else if (vString == "middle") vAligment = Qt::AlignVCenter;
		}

		alignment = (hAligment | vAligment);
	}
}

//==================================================================================================

void DrawingItem::exportStyleToSvg(QXmlStreamWriter* xml, const QBrush& fill, const QPen& stroke)
{
	QString styleString;
	QColor fillColor = fill.color();
	QColor strokeColor = stroke.brush().color();

	// fill and fill-opacity attributes
	if (fillColor.alpha() != 0)
	{
		if (fillColor.alpha() != 255)
		{
			qreal fillOpacity = fillColor.alphaF();
			fillColor.setAlpha(255);
			styleString = "fill:" + colorToString(fillColor) + ";fill-opacity:" + QString::number(fillOpacity);
		}
		else
		{
			styleString = "fill:" + colorToString(fillColor);
		}
	}
	else styleString = "fill:none";

	if (stroke.style() != Qt::NoPen)
	{
		// stroke and stroke-opacity attributes
		if (strokeColor.alpha() != 0)
		{
			if (strokeColor.alpha() != 255)
			{
				qreal strokeOpacity = strokeColor.alphaF();
				strokeColor.setAlpha(255);
				styleString += ";stroke:" + colorToString(strokeColor) + ";stroke-opacity:" + QString::number(strokeOpacity);
			}
			else
			{
				styleString += ";stroke:" + colorToString(strokeColor);
			}

			// stroke-width
			styleString += ";stroke-width:" + QString::number(stroke.widthF());

			// stroke-dasharray
			switch (stroke.style())
			{
			case Qt::DashLine:
				styleString += QString(";stroke-dasharray:%1 %2").arg(3 * stroke.widthF()).arg(2 * stroke.widthF());
				break;
			case Qt::DotLine:
				styleString += QString(";stroke-dasharray:%1 %2").arg(stroke.widthF()).arg(2 * stroke.widthF());
				break;
			case Qt::DashDotLine:
				styleString += QString(";stroke-dasharray:%1 %3 %2 %3").arg(3 * stroke.widthF()).arg(stroke.widthF()).arg(2 * stroke.widthF());
				break;
			case Qt::DashDotDotLine:
				styleString += QString(";stroke-dasharray:%1 %3 %2 %3 %2 %3").arg(3 * stroke.widthF()).arg(stroke.widthF()).arg(2 * stroke.widthF());
				break;
			default:
				break;
			}

			// stroke-linecap
			switch (stroke.capStyle())
			{
			case Qt::SquareCap:
				styleString += ";stroke-linecap:square";
				break;
			case Qt::RoundCap:
				styleString += ";stroke-linecap:round";
				break;
			default:
				break;
			}

			// stroke-linejoin
			switch (stroke.joinStyle())
			{
			case Qt::BevelJoin:
				styleString += ";stroke-linejoin:bevel";
				break;
			case Qt::RoundJoin:
				styleString += ";stroke-linejoin:round";
				break;
			default:
				break;
			}
		}
		else styleString += ";stroke:none";
	}
	else styleString += ";stroke:none";

	xml->writeAttribute("style", styleString);
}

void DrawingItem::exportArrowToSvg(QXmlStreamWriter* xml, const QPen& pen, const DrawingArrow& arrow)
{
	QPolygonF polygon;

	QPen arrowPen = pen;
	arrowPen.setStyle(Qt::SolidLine);

	switch (arrow.style())
	{
	case Drawing::ArrowNormal:
		polygon = arrow.polygon();
		xml->writeStartElement("path");
		xml->writeAttribute("d", QString("M %1,%2 L %3,%4 M %1,%2 L %5,%6").arg(
			mapToScene(polygon[0]).x()).arg(mapToScene(polygon[0]).y()).arg(
			mapToScene(polygon[1]).x()).arg(mapToScene(polygon[1]).y()).arg(
			mapToScene(polygon[2]).x()).arg(mapToScene(polygon[2]).y()));
		exportStyleToSvg(xml, Qt::transparent, arrowPen);
		xml->writeEndElement();
		break;
	case Drawing::ArrowTriangle:
	case Drawing::ArrowTriangleFilled:
	case Drawing::ArrowConcave:
	case Drawing::ArrowConcaveFilled:
		polygon = arrow.polygon();
		xml->writeStartElement("polygon");
		xml->writeAttribute("points", pointsToString(mapToScene(polygon)));
		if (arrow.style() == Drawing::ArrowTriangleFilled || arrow.style() == Drawing::ArrowConcaveFilled)
			exportStyleToSvg(xml, arrowPen.brush(), arrowPen);
		else if (widget())
			exportStyleToSvg(xml, widget()->backgroundBrush(), arrowPen);
		else
			exportStyleToSvg(xml, Qt::transparent, arrowPen);
		xml->writeEndElement();
		break;
	case Drawing::ArrowCircle:
	case Drawing::ArrowCircleFilled:
		xml->writeStartElement("ellipse");
		xml->writeAttribute("cx", QString::number(mapToScene(arrow.position()).x()));
		xml->writeAttribute("cy", QString::number(mapToScene(arrow.position()).y()));
		xml->writeAttribute("rx", QString::number(arrow.size() / 2));
		xml->writeAttribute("ry", QString::number(arrow.size() / 2));
		if (arrow.style() == Drawing::ArrowCircleFilled)
			exportStyleToSvg(xml, arrowPen.brush(), arrowPen);
		else if (widget())
			exportStyleToSvg(xml, widget()->backgroundBrush(), arrowPen);
		else
			exportStyleToSvg(xml, Qt::transparent, arrowPen);
		xml->writeEndElement();
		break;
	default:
		break;
	}
}

//==================================================================================================

QString DrawingItem::colorToString(const QColor& color) const
{
	QString colorStr;

	if (color.alpha() == 255)
	{
		colorStr = QString("#%1%2%3").arg(color.red(), 2, 16, QChar('0')).arg(
			color.green(), 2, 16, QChar('0')).arg(color.blue(), 2, 16, QChar('0')).toLower();
	}
	else
	{
		colorStr = QString("#%1%2%3%4").arg(color.alpha(), 2, 16, QChar('0')).arg(
		color.red(), 2, 16, QChar('0')).arg(color.green(), 2, 16, QChar('0')).arg(
		color.blue(), 2, 16, QChar('0')).toLower();

	}

	return colorStr;
}

QString DrawingItem::pointsToString(const QPolygonF& points) const
{
	QString pointsStr;

	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		pointsStr += QString::number((*pointIter).x()) + "," + QString::number((*pointIter).y()) + " ";

	return pointsStr.trimmed();
}

QColor DrawingItem::colorFromString(const QString& str) const
{
	QColor color;

	if (str.length() >= 9)
	{
		color.setAlpha(str.mid(1,2).toUInt(nullptr, 16));
		color.setRed(str.mid(3,2).toUInt(nullptr, 16));
		color.setGreen(str.mid(5,2).toUInt(nullptr, 16));
		color.setBlue(str.mid(7,2).toUInt(nullptr, 16));
	}
	else if (str.length() >= 7)
	{
		color.setRed(str.mid(1,2).toUInt(nullptr, 16));
		color.setGreen(str.mid(3,2).toUInt(nullptr, 16));
		color.setBlue(str.mid(5,2).toUInt(nullptr, 16));
	}

	return color;
}

QPolygonF DrawingItem::pointsFromString(const QString& str) const
{
	QPolygonF points;

	QStringList tokenList = str.split(" ");
	QStringList coordList;
	qreal x, y;
	bool xOk = false, yOk = false;

	for(int i = 0; i < tokenList.size(); i++)
	{
		coordList = tokenList[i].split(",");
		if (coordList.size() == 2)
		{
			x = coordList[0].toDouble(&xOk);
			y = coordList[1].toDouble(&yOk);
			if (xOk && yOk) points.append(QPointF(x, y));
		}
	}

	return points;
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

void DrawingItem::adjustFirstPointToOrigin()
{
	// Adjust position of item and item points so that point(0)->position() == QPointF(0, 0)
	QPointF pointDelta = -mPoints.first()->position();
	QPointF newPosition = mapToScene(mPoints.first()->position());

	for(auto pointIter = mPoints.begin(); pointIter != mPoints.end(); pointIter++)
		(*pointIter)->setPosition((*pointIter)->position() + pointDelta);

	setPosition(newPosition);
}

//==================================================================================================

DrawingItemFactory DrawingItem::factory;

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
	for(auto itemIter = items.begin(), itemEnd = items.end(); itemIter != itemEnd; itemIter++)
		copiedItems.append((*itemIter)->copy());

	// Maintain connections to other items in this list
	for(int itemIndex = 0, itemSize = items.size(); itemIndex < itemSize; itemIndex++)
	{
		itemPoints = items[itemIndex]->points();
		for(int pointIndex = 0, pointSize = itemPoints.size(); pointIndex < pointSize; pointIndex++)
		{
			targetPoints = itemPoints[pointIndex]->connections();
			for(auto targetIter = targetPoints.begin(), targetEnd = targetPoints.end();
				targetIter != targetEnd; targetIter++)
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
