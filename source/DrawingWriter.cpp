/* DrawingWriter.cpp
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

#include "DrawingWriter.h"
#include <DrawingWidget.h>
#include <DrawingItemGroup.h>
#include <DrawingItemPoint.h>
#include <DrawingPathItem.h>
#include <DrawingShapeItems.h>
#include <DrawingTextItem.h>
#include <DrawingTwoPointItems.h>

DrawingWriter::DrawingWriter(QIODevice* device) : QXmlStreamWriter(device)
{
	setAutoFormatting(true);
	setAutoFormattingIndent(2);
}

DrawingWriter::~DrawingWriter() { }

//==================================================================================================

void DrawingWriter::write(DrawingWidget* drawing)
{
	writeStartDocument();
	writeStartElement("jade-drawing");
	writeAttribute("version", "1.1");
	writeStartElement("page");

	if (drawing)
	{
		QRectF sceneRect = drawing->sceneRect();
		writeAttribute("view-left", QString::number(sceneRect.left()));
		writeAttribute("view-top", QString::number(sceneRect.top()));
		writeAttribute("view-width", QString::number(sceneRect.width()));
		writeAttribute("view-height", QString::number(sceneRect.height()));

		writeAttribute("background-color", colorToString(drawing->backgroundBrush().color()));

		writeAttribute("grid", QString::number(drawing->grid()));

		writeAttribute("grid-color", colorToString(drawing->gridBrush().color()));
		writeAttribute("grid-style", gridStyleToString(drawing->gridStyle()));
		writeAttribute("grid-spacing-major", QString::number(drawing->gridSpacingMajor()));
		writeAttribute("grid-spacing-minor", QString::number(drawing->gridSpacingMinor()));

		writeStartElement("items");
		writeItemElements(drawing->items());
		writeEndElement();
	}

	writeEndElement();
	writeEndElement();
	writeEndDocument();
}

void DrawingWriter::writeItems(const QList<DrawingItem*>& items)
{
	writeStartDocument();
	writeStartElement("jade-items");
	writeAttribute("version", "1.1");

	writeStartElement("items");
	writeItemElements(items);
	writeEndElement();

	writeEndElement();
	writeEndDocument();
}

//==================================================================================================

void DrawingWriter::writeItemElements(const QList<DrawingItem*>& items)
{
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
	{
		DrawingLineItem* lineItem = dynamic_cast<DrawingLineItem*>(*itemIter);
		DrawingArcItem* arcItem = dynamic_cast<DrawingArcItem*>(*itemIter);
		DrawingPolylineItem* polylineItem = dynamic_cast<DrawingPolylineItem*>(*itemIter);
		DrawingCurveItem* curveItem = dynamic_cast<DrawingCurveItem*>(*itemIter);
		DrawingRectItem* rectItem = dynamic_cast<DrawingRectItem*>(*itemIter);
		DrawingEllipseItem* ellipseItem = dynamic_cast<DrawingEllipseItem*>(*itemIter);
		DrawingPolygonItem* polygonItem = dynamic_cast<DrawingPolygonItem*>(*itemIter);
		DrawingTextItem* textItem = dynamic_cast<DrawingTextItem*>(*itemIter);
		DrawingPathItem* pathItem = dynamic_cast<DrawingPathItem*>(*itemIter);
		DrawingItemGroup* groupItem = dynamic_cast<DrawingItemGroup*>(*itemIter);

		if (lineItem) writeLineItem(lineItem);
		else if (arcItem) writeArcItem(arcItem);
		else if (polylineItem) writePolylineItem(polylineItem);
		else if (curveItem) writeCurveItem(curveItem);
		else if (rectItem) writeRectItem(rectItem);
		else if (ellipseItem) writeEllipseItem(ellipseItem);
		else if (polygonItem) writePolygonItem(polygonItem);
		else if (textItem) writeTextItem(textItem);
		else if (pathItem) writePathItem(pathItem);
		else if (groupItem) writeItemGroup(groupItem);
	}
}

//==================================================================================================

void DrawingWriter::writeLineItem(DrawingLineItem* item)
{
	writeStartElement("line");

	writeAttribute("transform", transformToString(item->pos(), item->rotation(), item->isFlipped()));

	writeAttribute("x1", QString::number(item->startPoint()->x()));
	writeAttribute("y1", QString::number(item->startPoint()->y()));
	writeAttribute("x2", QString::number(item->endPoint()->x()));
	writeAttribute("y2", QString::number(item->endPoint()->y()));

	writeItemProperties(item);

	writeEndElement();
}

void DrawingWriter::writeArcItem(DrawingArcItem* item)
{
	writeStartElement("arc");

	writeAttribute("transform", transformToString(item->pos(), item->rotation(), item->isFlipped()));

	writeAttribute("x1", QString::number(item->startPoint()->x()));
	writeAttribute("y1", QString::number(item->startPoint()->y()));
	writeAttribute("x2", QString::number(item->endPoint()->x()));
	writeAttribute("y2", QString::number(item->endPoint()->y()));

	writeItemProperties(item);

	writeEndElement();
}

void DrawingWriter::writePolylineItem(DrawingPolylineItem* item)
{
	writeStartElement("polyline");

	writeAttribute("transform", transformToString(item->pos(), item->rotation(), item->isFlipped()));

	QPolygonF polygon;
	QList<DrawingItemPoint*> points = item->points();
	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		polygon.append((*pointIter)->pos());
	if (!polygon.isEmpty()) writeAttribute("points", pointsToString(polygon));

	writeItemProperties(item);

	writeEndElement();
}

void DrawingWriter::writeCurveItem(DrawingCurveItem* item)
{
	writeStartElement("curve");

	writeAttribute("transform", transformToString(item->pos(), item->rotation(), item->isFlipped()));

	writeAttribute("x1", QString::number(item->startPoint()->x()));
	writeAttribute("y1", QString::number(item->startPoint()->y()));
	writeAttribute("cx1", QString::number(item->startControlPoint()->x()));
	writeAttribute("cy1", QString::number(item->startControlPoint()->y()));
	writeAttribute("cx2", QString::number(item->endControlPoint()->x()));
	writeAttribute("cy2", QString::number(item->endControlPoint()->y()));
	writeAttribute("x2", QString::number(item->endPoint()->x()));
	writeAttribute("y2", QString::number(item->endPoint()->y()));

	writeItemProperties(item);

	writeEndElement();
}

void DrawingWriter::writeRectItem(DrawingRectItem* item)
{
	writeStartElement("rect");

	writeAttribute("transform", transformToString(item->pos(), item->rotation(), item->isFlipped()));

	QRectF rect(item->topLeftPoint()->pos(), item->bottomRightPoint()->pos());
	writeAttribute("left", QString::number(rect.left()));
	writeAttribute("top", QString::number(rect.top()));
	writeAttribute("width", QString::number(rect.width()));
	writeAttribute("height", QString::number(rect.height()));

	writeAttribute("rx", QString::number(item->cornerRadiusX()));
	writeAttribute("ry", QString::number(item->cornerRadiusY()));

	writeItemProperties(item);

	writeEndElement();
}

void DrawingWriter::writeEllipseItem(DrawingEllipseItem* item)
{
	writeStartElement("ellipse");

	writeAttribute("transform", transformToString(item->pos(), item->rotation(), item->isFlipped()));

	QRectF rect(item->topLeftPoint()->pos(), item->bottomRightPoint()->pos());
	writeAttribute("left", QString::number(rect.left()));
	writeAttribute("top", QString::number(rect.top()));
	writeAttribute("width", QString::number(rect.width()));
	writeAttribute("height", QString::number(rect.height()));

	writeItemProperties(item);

	writeEndElement();
}

void DrawingWriter::writePolygonItem(DrawingPolygonItem* item)
{
	writeStartElement("polygon");

	writeAttribute("transform", transformToString(item->pos(), item->rotation(), item->isFlipped()));

	QPolygonF polygon;
	QList<DrawingItemPoint*> points = item->points();
	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		polygon.append((*pointIter)->pos());
	if (!polygon.isEmpty()) writeAttribute("points", pointsToString(polygon));

	writeItemProperties(item);

	writeEndElement();
}

void DrawingWriter::writeTextItem(DrawingTextItem* item)
{
	writeStartElement("text");

	writeAttribute("transform", transformToString(item->pos(), item->rotation(), item->isFlipped()));

	writeItemProperties(item);

	writeCharacters(item->caption());

	writeEndElement();
}

void DrawingWriter::writePathItem(DrawingPathItem* item)
{
	writeStartElement("path");

	writeAttribute("name", item->name());

	writeAttribute("transform", transformToString(item->pos(), item->rotation(), item->isFlipped()));

	QRectF rect(item->topLeftPoint()->pos(), item->bottomRightPoint()->pos());
	writeAttribute("left", QString::number(rect.left()));
	writeAttribute("top", QString::number(rect.top()));
	writeAttribute("width", QString::number(rect.width()));
	writeAttribute("height", QString::number(rect.height()));

	writeItemProperties(item);

	QRectF pathRect = item->pathRect();
	writeAttribute("view-left", QString::number(pathRect.left()));
	writeAttribute("view-top", QString::number(pathRect.top()));
	writeAttribute("view-width", QString::number(pathRect.width()));
	writeAttribute("view-height", QString::number(pathRect.height()));

	writeAttribute("d", pathToString(item->path()));

	QString glueStr = pointsToString(item->connectionPoints());
	if (!glueStr.isEmpty()) writeAttribute("glue-points", glueStr);

	writeEndElement();
}

void DrawingWriter::writeItemGroup(DrawingItemGroup* item)
{
	writeStartElement("group");

	writeAttribute("transform", transformToString(item->pos(), item->rotation(), item->isFlipped()));

	writeItemElements(item->items());

	writeEndElement();
}

//==================================================================================================

void DrawingWriter::writeItemProperties(DrawingItem* item)
{
	// Pen
	if (item->hasProperty("Pen Color"))
	{
		QColor color = item->property("Pen Color").value<QColor>();
		writeAttribute("stroke-color", colorToString(color));
		writeAttribute("stroke-opacity", QString::number(color.alphaF()));
	}
	if (item->hasProperty("Pen Width"))
		writeAttribute("stroke-width", QString::number(item->property("Pen Width").toDouble()));
	if (item->hasProperty("Pen Style"))
		writeAttribute("stroke-style", penStyleToString((Qt::PenStyle)item->property("Pen Style").toUInt()));
	if (item->hasProperty("Pen Cap Style"))
		writeAttribute("stroke-linecap", penCapStyleToString((Qt::PenCapStyle)item->property("Pen Cap Style").toUInt()));
	if (item->hasProperty("Pen Join Style"))
		writeAttribute("stroke-linejoin", penJoinStyleToString((Qt::PenJoinStyle)item->property("Pen Join Style").toUInt()));

	// Brush
	if (item->hasProperty("Brush Color"))
	{
		QColor color = item->property("Brush Color").value<QColor>();
		writeAttribute("fill-color", colorToString(color));
		writeAttribute("fill-opacity", QString::number(color.alphaF()));
	}

	// Arrows
	if (item->hasProperty("Start Arrow Style"))
		writeAttribute("arrow-start-style", arrowStyleToString((DrawingArrowStyle)item->property("Start Arrow Style").toUInt()));
	if (item->hasProperty("Start Arrow Size"))
		writeAttribute("arrow-start-size", QString::number(item->property("Start Arrow Size").toDouble()));
	if (item->hasProperty("End Arrow Style"))
		writeAttribute("arrow-end-style", arrowStyleToString((DrawingArrowStyle)item->property("End Arrow Style").toUInt()));
	if (item->hasProperty("End Arrow Size"))
		writeAttribute("arrow-end-size", QString::number(item->property("End Arrow Size").toDouble()));

	// Font
	if (item->hasProperty("Font Family"))
		writeAttribute("font-name", item->property("Font Family").toString());
	if (item->hasProperty("Font Size"))
		writeAttribute("font-size", QString::number(item->property("Font Size").toDouble()));
	if (item->hasProperty("Font Bold"))
		writeAttribute("font-bold", (item->property("Font Bold").toBool()) ? "true" : "false");
	if (item->hasProperty("Font Italic"))
		writeAttribute("font-italic", (item->property("Font Italic").toBool()) ? "true" : "false");
	if (item->hasProperty("Font Underline"))
		writeAttribute("font-underline", (item->property("Font Underline").toBool()) ? "true" : "false");
	if (item->hasProperty("Font Overline"))
		writeAttribute("font-overline", (item->property("Font Overline").toBool()) ? "true" : "false");
	if (item->hasProperty("Font Strike-Out"))
		writeAttribute("font-strike-through", (item->property("Font Strike-Out").toBool()) ? "true" : "false");

	// Text Alignment
	if (item->hasProperty("Text Horizontal Alignment"))
	{
		Qt::Alignment align = (Qt::Alignment)(item->property("Text Horizontal Alignment").toUInt() & Qt::AlignHorizontal_Mask);
		writeAttribute("text-alignment-horizontal", alignmentToString(align));
	}
	if (item->hasProperty("Text Vertical Alignment"))
	{
		Qt::Alignment align = (Qt::Alignment)(item->property("Text Vertical Alignment").toUInt() & Qt::AlignVertical_Mask);
		writeAttribute("text-alignment-vertical", alignmentToString(align));
	}

	// Text color
	if (item->hasProperty("Text Color"))
	{
		QColor color = item->property("Text Color").value<QColor>();
		writeAttribute("text-color", colorToString(color));
		writeAttribute("text-opacity", QString::number(color.alphaF()));
	}
}

//==================================================================================================

QString DrawingWriter::alignmentToString(Qt::Alignment align) const
{
	QString str;

	if (align & Qt::AlignLeft) str = "left";
	else if (align & Qt::AlignRight) str = "right";
	else if (align & Qt::AlignHCenter) str = "center";
	else if (align & Qt::AlignTop) str = "top";
	else if (align & Qt::AlignBottom) str = "bottom";
	else if (align & Qt::AlignVCenter) str = "middle";

	return str;
}

QString DrawingWriter::arrowStyleToString(DrawingArrowStyle style) const
{
	QString str;

	switch (style)
	{
	case ArrowNormal: str = "normal"; break;
	case ArrowTriangle: str = "triangle"; break;
	case ArrowTriangleFilled: str = "triangle-filled"; break;
	case ArrowCircle: str = "circle"; break;
	case ArrowCircleFilled: str = "circle-filled"; break;
	case ArrowDiamond: str = "diamond"; break;
	case ArrowDiamondFilled: str = "diamond-filled"; break;
	case ArrowHarpoon: str = "harpoon"; break;
	case ArrowHarpoonMirrored: str = "harpoon-mirrored"; break;
	case ArrowConcave: str = "concave"; break;
	case ArrowConcaveFilled: str = "concave-filled"; break;
	case ArrowReverse: str = "reverse"; break;
	case ArrowX: str = "X"; break;
	default: str = "none"; break;
	}

	return str;
}

QString DrawingWriter::colorToString(const QColor& color) const
{
	QString str = "#";

	if (color.red() < 16) str += "0";
	str += QString::number(color.red(), 16).toLower();
	if (color.green() < 16) str += "0";
	str += QString::number(color.green(), 16).toLower();
	if (color.blue() < 16) str += "0";
	str += QString::number(color.blue(), 16).toLower();

	return str;
}

QString DrawingWriter::gridStyleToString(DrawingGridStyle gridStyle) const
{
	QString str;

	switch (gridStyle)
	{
	case GridDotted: str = "dotted"; break;
	case GridLined: str = "lined"; break;
	case GridGraphPaper: str = "graphpaper"; break;
	default: str = "none"; break;
	}

	return str;
}

QString DrawingWriter::pathToString(const QPainterPath& path) const
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

QString DrawingWriter::penStyleToString(Qt::PenStyle style) const
{
	QString str;

	switch (style)
	{
	case Qt::NoPen: str = "none"; break;
	case Qt::DashLine: str = "dash"; break;
	case Qt::DotLine: str = "dot"; break;
	case Qt::DashDotLine: str = "dash-dot"; break;
	case Qt::DashDotDotLine: str = "dash-dot-dot"; break;
	default: str = "solid"; break;
	}

	return str;
}

QString DrawingWriter::penCapStyleToString(Qt::PenCapStyle style) const
{
	QString str;

	switch (style)
	{
	case Qt::FlatCap: str = "flat"; break;
	case Qt::SquareCap: str = "square"; break;
	default: str = "round"; break;
	}

	return str;
}

QString DrawingWriter::penJoinStyleToString(Qt::PenJoinStyle style) const
{
	QString str;

	switch (style)
	{
	case Qt::SvgMiterJoin:
	case Qt::MiterJoin: str = "miter"; break;
	case Qt::BevelJoin: str = "bevel"; break;
	default: str = "round"; break;
	}

	return str;
}

QString DrawingWriter::pointsToString(const QPolygonF& points) const
{
	QString pointsStr;

	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		pointsStr += QString::number((*pointIter).x()) + "," + QString::number((*pointIter).y()) + " ";

	return pointsStr.trimmed();
}

QString DrawingWriter::transformToString(const QPointF& pos, qreal rotation, bool flipped) const
{
	QString str = "translate(" + QString::number(pos.x()) + "," + QString::number(pos.y()) + ")";
	str += (rotation != 0) ? " rotate(" + QString::number(rotation) + ")" : "";
	str += (flipped) ? " scale(-1.0,1.0)" : "";
	return str;
}
