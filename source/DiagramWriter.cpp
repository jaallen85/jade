/* DiagramWriter.cpp
 *
 * Copyright (C) 2013-2017 Jason Allen
 *
 * This file is part of the jade application.
 *
 * jade is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * jade is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with jade.  If not, see <http://www.gnu.org/licenses/>
 */

#include "DiagramWriter.h"

DiagramWriter::DiagramWriter(QIODevice* device) : QXmlStreamWriter(device)
{
	setAutoFormatting(true);
	setAutoFormattingIndent(2);
}

DiagramWriter::DiagramWriter(QString* string) : QXmlStreamWriter(string)
{
	setAutoFormatting(true);
	setAutoFormattingIndent(2);
}

DiagramWriter::~DiagramWriter() { }

//==================================================================================================

void DiagramWriter::write(DiagramWidget* diagram)
{
	writeStartDocument();
	writeStartElement("jade-drawing");
	writeAttribute("version", "1.1");
	writeStartElement("page");

	if (diagram)
	{
		DrawingScene* scene = diagram->scene();

		if (scene)
		{
			QRectF sceneRect = scene->sceneRect();
			writeAttribute("view-left", QString::number(sceneRect.left()));
			writeAttribute("view-top", QString::number(sceneRect.top()));
			writeAttribute("view-width", QString::number(sceneRect.width()));
			writeAttribute("view-height", QString::number(sceneRect.height()));

			writeAttribute("background-color", colorToString(scene->backgroundBrush().color()));
		}

		writeAttribute("grid", QString::number(diagram->grid()));

		writeAttribute("grid-color", colorToString(diagram->gridBrush().color()));
		writeAttribute("grid-style", gridStyleToString(diagram->gridStyle()));
		writeAttribute("grid-spacing-major", QString::number(diagram->gridSpacingMajor()));
		writeAttribute("grid-spacing-minor", QString::number(diagram->gridSpacingMinor()));

		if (scene)
		{
			writeStartElement("items");
			writeItemElements(scene->items());
			writeEndElement();
		}
	}

	writeEndElement();
	writeEndElement();
	writeEndDocument();
}

void DiagramWriter::writeItems(const QList<DrawingItem*>& items)
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

void DiagramWriter::writeItemElements(const QList<DrawingItem*>& items)
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
		DrawingTextRectItem* textRectItem = dynamic_cast<DrawingTextRectItem*>(*itemIter);
		DrawingTextEllipseItem* textEllipseItem = dynamic_cast<DrawingTextEllipseItem*>(*itemIter);
		DrawingTextPolygonItem* textPolygonItem = dynamic_cast<DrawingTextPolygonItem*>(*itemIter);
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
		else if (textRectItem) writeTextRectItem(textRectItem);
		else if (textEllipseItem) writeTextEllipseItem(textEllipseItem);
		else if (textPolygonItem) writeTextPolygonItem(textPolygonItem);
		else if (pathItem) writePathItem(pathItem);
		else if (groupItem) writeItemGroup(groupItem);
	}
}

//==================================================================================================

void DiagramWriter::writeLineItem(DrawingLineItem* item)
{
	writeStartElement("line");

	writeAttribute("transform", transformToString(item));

	QLineF line = item->line();
	writeAttribute("x1", QString::number(line.x1()));
	writeAttribute("y1", QString::number(line.y1()));
	writeAttribute("x2", QString::number(line.x2()));
	writeAttribute("y2", QString::number(line.y2()));

	writeItemStyle(item->style());

	writeEndElement();
}

void DiagramWriter::writeArcItem(DrawingArcItem* item)
{
	writeStartElement("arc");

	writeAttribute("transform", transformToString(item));

	QLineF line = item->arc();
	writeAttribute("x1", QString::number(line.x1()));
	writeAttribute("y1", QString::number(line.y1()));
	writeAttribute("x2", QString::number(line.x2()));
	writeAttribute("y2", QString::number(line.y2()));

	writeItemStyle(item->style());

	writeEndElement();
}

void DiagramWriter::writePolylineItem(DrawingPolylineItem* item)
{
	writeStartElement("polyline");

	writeAttribute("transform", transformToString(item));

	QPolygonF polygon;
	QList<DrawingItemPoint*> points = item->points();
	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		polygon.append((*pointIter)->position());
	if (!polygon.isEmpty()) writeAttribute("points", pointsToString(polygon));

	writeItemStyle(item->style());

	writeEndElement();
}

void DiagramWriter::writeCurveItem(DrawingCurveItem* item)
{
	writeStartElement("curve");

	writeAttribute("transform", transformToString(item));

	writeAttribute("x1", QString::number(item->curveStartPos().x()));
	writeAttribute("y1", QString::number(item->curveStartPos().y()));
	writeAttribute("cx1", QString::number(item->curveStartControlPos().x()));
	writeAttribute("cy1", QString::number(item->curveStartControlPos().y()));
	writeAttribute("cx2", QString::number(item->curveEndControlPos().x()));
	writeAttribute("cy2", QString::number(item->curveEndControlPos().y()));
	writeAttribute("x2", QString::number(item->curveEndPos().x()));
	writeAttribute("y2", QString::number(item->curveEndPos().y()));

	writeItemStyle(item->style());

	writeEndElement();
}

void DiagramWriter::writeRectItem(DrawingRectItem* item)
{
	writeStartElement("rect");

	writeAttribute("transform", transformToString(item));

	QRectF rect = item->rect();
	writeAttribute("left", QString::number(rect.left()));
	writeAttribute("top", QString::number(rect.top()));
	writeAttribute("width", QString::number(rect.width()));
	writeAttribute("height", QString::number(rect.height()));

	if (item->cornerRadiusX() != 0) writeAttribute("rx", QString::number(item->cornerRadiusX()));
	if (item->cornerRadiusY() != 0) writeAttribute("ry", QString::number(item->cornerRadiusY()));

	writeItemStyle(item->style());

	writeEndElement();
}

void DiagramWriter::writeEllipseItem(DrawingEllipseItem* item)
{
	writeStartElement("ellipse");

	writeAttribute("transform", transformToString(item));

	QRectF rect = item->ellipse();
	writeAttribute("left", QString::number(rect.left()));
	writeAttribute("top", QString::number(rect.top()));
	writeAttribute("width", QString::number(rect.width()));
	writeAttribute("height", QString::number(rect.height()));

	writeItemStyle(item->style());

	writeEndElement();
}

void DiagramWriter::writePolygonItem(DrawingPolygonItem* item)
{
	writeStartElement("polygon");

	writeAttribute("transform", transformToString(item));

	QPolygonF polygon;
	QList<DrawingItemPoint*> points = item->points();
	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		polygon.append((*pointIter)->position());
	if (!polygon.isEmpty()) writeAttribute("points", pointsToString(polygon));

	writeItemStyle(item->style());

	writeEndElement();
}

void DiagramWriter::writeTextItem(DrawingTextItem* item)
{
	writeStartElement("text");

	writeAttribute("transform", transformToString(item));

	writeItemStyle(item->style());

	writeCharacters(item->caption());

	writeEndElement();
}

void DiagramWriter::writeTextRectItem(DrawingTextRectItem* item)
{
	writeStartElement("text-rect");

	writeAttribute("transform", transformToString(item));

	QRectF rect = item->rect();
	writeAttribute("left", QString::number(rect.left()));
	writeAttribute("top", QString::number(rect.top()));
	writeAttribute("width", QString::number(rect.width()));
	writeAttribute("height", QString::number(rect.height()));

	if (item->cornerRadiusX() != 0) writeAttribute("rx", QString::number(item->cornerRadiusX()));
	if (item->cornerRadiusY() != 0) writeAttribute("ry", QString::number(item->cornerRadiusY()));

	writeItemStyle(item->style());

	writeCharacters(item->caption());

	writeEndElement();
}

void DiagramWriter::writeTextEllipseItem(DrawingTextEllipseItem* item)
{
	writeStartElement("text-ellipse");

	writeAttribute("transform", transformToString(item));

	QRectF rect = item->ellipse();
	writeAttribute("left", QString::number(rect.left()));
	writeAttribute("top", QString::number(rect.top()));
	writeAttribute("width", QString::number(rect.width()));
	writeAttribute("height", QString::number(rect.height()));

	writeItemStyle(item->style());

	writeCharacters(item->caption());

	writeEndElement();
}

void DiagramWriter::writeTextPolygonItem(DrawingTextPolygonItem* item)
{
	writeStartElement("text-polygon");

	writeAttribute("transform", transformToString(item));

	QPolygonF polygon;
	QList<DrawingItemPoint*> points = item->points();
	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		polygon.append((*pointIter)->position());
	if (!polygon.isEmpty()) writeAttribute("points", pointsToString(polygon));

	writeItemStyle(item->style());

	writeCharacters(item->caption());

	writeEndElement();
}

void DiagramWriter::writePathItem(DrawingPathItem* item)
{
	writeStartElement("path");

	writeAttribute("name", item->name());

	writeAttribute("transform", transformToString(item));

	QRectF rect = item->rect();
	writeAttribute("left", QString::number(rect.left()));
	writeAttribute("top", QString::number(rect.top()));
	writeAttribute("width", QString::number(rect.width()));
	writeAttribute("height", QString::number(rect.height()));

	writeItemStyle(item->style());

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

void DiagramWriter::writeItemGroup(DrawingItemGroup* item)
{
	writeStartElement("group");

	writeAttribute("transform", transformToString(item));

	writeItemElements(item->items());

	writeEndElement();
}

//==================================================================================================

void DiagramWriter::writeItemStyle(DrawingItemStyle* style)
{
	if (style)
	{
		// Pen
		if (style->hasValue(DrawingItemStyle::PenStyle))
		{
			Qt::PenStyle penStyle = (Qt::PenStyle)style->value(DrawingItemStyle::PenStyle).toUInt();
			if (penStyle != Qt::SolidLine) writeAttribute("stroke-style", penStyleToString(penStyle));
		}

		if (style->hasValue(DrawingItemStyle::PenWidth))
			writeAttribute("stroke-width", QString::number(style->value(DrawingItemStyle::PenWidth).toReal()));

		if (style->hasValue(DrawingItemStyle::PenColor))
			writeAttribute("stroke-color", colorToString(style->value(DrawingItemStyle::PenColor).value<QColor>()));

		if (style->hasValue(DrawingItemStyle::PenOpacity))
		{
			qreal opacity = style->value(DrawingItemStyle::PenOpacity).toReal();
			if (opacity != 1.0) writeAttribute("stroke-opacity", QString::number(opacity));
		}

		// Brush
		if (style->hasValue(DrawingItemStyle::BrushColor))
			writeAttribute("fill-color", colorToString(style->value(DrawingItemStyle::BrushColor).value<QColor>()));

		if (style->hasValue(DrawingItemStyle::BrushOpacity))
		{
			qreal opacity = style->value(DrawingItemStyle::BrushOpacity).toReal();
			if (opacity != 1.0) writeAttribute("fill-opacity", QString::number(opacity));
		}

		// Font
		if (style->hasValue(DrawingItemStyle::FontName))
			writeAttribute("font-name", style->value(DrawingItemStyle::FontName).toString());

		if (style->hasValue(DrawingItemStyle::FontSize))
			writeAttribute("font-size", QString::number(style->value(DrawingItemStyle::FontSize).toReal()));

		if (style->hasValue(DrawingItemStyle::FontBold))
		{
			bool fontStyle = style->value(DrawingItemStyle::FontBold).toBool();
			if (fontStyle) writeAttribute("font-bold", "true");
		}

		if (style->hasValue(DrawingItemStyle::FontItalic))
		{
			bool fontStyle = style->value(DrawingItemStyle::FontItalic).toBool();
			if (fontStyle) writeAttribute("font-italic", "true");
		}

		if (style->hasValue(DrawingItemStyle::FontUnderline))
		{
			bool fontStyle = style->value(DrawingItemStyle::FontUnderline).toBool();
			if (fontStyle) writeAttribute("font-underline", "true");
		}

		if (style->hasValue(DrawingItemStyle::FontStrikeThrough))
		{
			bool fontStyle = style->value(DrawingItemStyle::FontStrikeThrough).toBool();
			if (fontStyle) writeAttribute("font-strike-through", "true");
		}

		if (style->hasValue(DrawingItemStyle::TextHorizontalAlignment))
		{
			Qt::Alignment textAlign = ((Qt::Alignment)style->value(DrawingItemStyle::TextHorizontalAlignment).toUInt() & Qt::AlignHorizontal_Mask);
			if (textAlign != Qt::AlignHCenter) writeAttribute("text-alignment-horizontal", alignmentToString(textAlign));
		}

		if (style->hasValue(DrawingItemStyle::TextVerticalAlignment))
		{
			Qt::Alignment textAlign = ((Qt::Alignment)style->value(DrawingItemStyle::TextVerticalAlignment).toUInt() & Qt::AlignVertical_Mask);
			if (textAlign != Qt::AlignVCenter) writeAttribute("text-alignment-vertical", alignmentToString(textAlign));
		}

		if (style->hasValue(DrawingItemStyle::TextColor))
			writeAttribute("text-color", colorToString(style->value(DrawingItemStyle::TextColor).value<QColor>()));

		if (style->hasValue(DrawingItemStyle::TextOpacity))
		{
			qreal opacity = style->value(DrawingItemStyle::TextOpacity).toReal();
			if (opacity != 1.0) writeAttribute("text-opacity", QString::number(opacity));
		}

		// Arrows
		if (style->hasValue(DrawingItemStyle::StartArrowStyle) && style->hasValue(DrawingItemStyle::StartArrowSize))
		{
			DrawingItemStyle::ArrowStyle arrow = (DrawingItemStyle::ArrowStyle)style->value(DrawingItemStyle::StartArrowStyle).toUInt();
			qreal arrowSize = style->value(DrawingItemStyle::StartArrowSize).toReal();

			if (arrow != DrawingItemStyle::ArrowNone)
				writeAttribute("arrow-start-style", arrowStyleToString(arrow));
			if (arrowSize != 0)
				writeAttribute("arrow-start-size", QString::number(arrowSize));
		}

		if (style->hasValue(DrawingItemStyle::EndArrowStyle) && style->hasValue(DrawingItemStyle::EndArrowSize))
		{
			DrawingItemStyle::ArrowStyle arrow = (DrawingItemStyle::ArrowStyle)style->value(DrawingItemStyle::EndArrowStyle).toUInt();
			qreal arrowSize = style->value(DrawingItemStyle::EndArrowSize).toReal();

			if (arrow != DrawingItemStyle::ArrowNone)
				writeAttribute("arrow-end-style", arrowStyleToString(arrow));
			if (arrowSize != 0)
				writeAttribute("arrow-end-size", QString::number(arrowSize));
		}
	}
}

//==================================================================================================

QString DiagramWriter::alignmentToString(Qt::Alignment align) const
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

QString DiagramWriter::arrowStyleToString(DrawingItemStyle::ArrowStyle style) const
{
	QString str;

	switch (style)
	{
	case DrawingItemStyle::ArrowNormal: str = "normal"; break;
	case DrawingItemStyle::ArrowTriangle: str = "triangle"; break;
	case DrawingItemStyle::ArrowTriangleFilled: str = "triangle-filled"; break;
	case DrawingItemStyle::ArrowCircle: str = "circle"; break;
	case DrawingItemStyle::ArrowCircleFilled: str = "circle-filled"; break;
	case DrawingItemStyle::ArrowDiamond: str = "diamond"; break;
	case DrawingItemStyle::ArrowDiamondFilled: str = "diamond-filled"; break;
	case DrawingItemStyle::ArrowHarpoon: str = "harpoon"; break;
	case DrawingItemStyle::ArrowHarpoonMirrored: str = "harpoon-mirrored"; break;
	case DrawingItemStyle::ArrowConcave: str = "concave"; break;
	case DrawingItemStyle::ArrowConcaveFilled: str = "concave-filled"; break;
	case DrawingItemStyle::ArrowReverse: str = "reverse"; break;
	case DrawingItemStyle::ArrowX: str = "X"; break;
	default: str = "none"; break;
	}

	return str;
}

QString DiagramWriter::colorToString(const QColor& color) const
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

QString DiagramWriter::gridStyleToString(DiagramWidget::GridRenderStyle gridStyle) const
{
	QString str;

	switch (gridStyle)
	{
	case DiagramWidget::GridDots: str = "dotted"; break;
	case DiagramWidget::GridLines: str = "lined"; break;
	case DiagramWidget::GridGraphPaper: str = "graph-paper"; break;
	default: str = "none"; break;
	}

	return str;
}

QString DiagramWriter::pathToString(const QPainterPath& path) const
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

QString DiagramWriter::penStyleToString(Qt::PenStyle style) const
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

QString DiagramWriter::penCapStyleToString(Qt::PenCapStyle style) const
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

QString DiagramWriter::penJoinStyleToString(Qt::PenJoinStyle style) const
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

QString DiagramWriter::pointsToString(const QPolygonF& points) const
{
	QString pointsStr;

	for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
		pointsStr += QString::number((*pointIter).x()) + "," + QString::number((*pointIter).y()) + " ";

	return pointsStr.trimmed();
}

QString DiagramWriter::transformToString(DrawingItem* item) const
{
	QPointF pos = item->position();
	QTransform transform = item->transform();

	qreal rotation = qAsin(transform.m12()) * 180 / 3.141592654;
	transform.rotate(-rotation);

	qreal hScale = transform.m11();
	qreal vScale = transform.m22();

	QString str = "translate(" + QString::number(pos.x()) + "," + QString::number(pos.y()) + ")";
	str += (rotation != 0) ? " rotate(" + QString::number(rotation) + ")" : "";
	str += (hScale != 1.0 || vScale != 1.0) ? " scale(" + QString::number(hScale) + "," + QString::number(vScale) + ")" : "";
	return str;
}
