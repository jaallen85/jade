// SvgWriter.cpp
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

#include "SvgWriter.h"
#include <DrawingCurveItem.h>
#include <DrawingEllipseItem.h>
#include <DrawingItemGroup.h>
#include <DrawingLineItem.h>
#include <DrawingPathItem.h>
#include <DrawingPolygonItem.h>
#include <DrawingPolylineItem.h>
#include <DrawingReferenceItem.h>
#include <DrawingRectItem.h>
#include <DrawingTextItem.h>
#include <DrawingTextEllipseItem.h>
#include <DrawingTextRectItem.h>
#include <DrawingWidget.h>
#include <QFile>
#include <QMessageBox>
#include <QXmlStreamWriter>

SvgWriter::SvgWriter(DrawingWidget* drawing)
{
	mDrawing = drawing;
}

SvgWriter::~SvgWriter()
{
	mDrawing = nullptr;
}

//==================================================================================================

void SvgWriter::save(const QString& filePath, const QSize& size)
{
	QFile svgFile(filePath);

	bool fileError = !svgFile.open(QIODevice::WriteOnly);
	if (!fileError)
	{
		QXmlStreamWriter xml(&svgFile);
		xml.setAutoFormatting(true);
		xml.setAutoFormattingIndent(2);

		xml.writeStartDocument();
		writeSvg(&xml, size);
		xml.writeEndDocument();
	}

	if (fileError)
	{
		QMessageBox::critical(mDrawing, "Error Exporting File",
			"Unable to open file for SVG export.  File not exported: " + filePath);
	}
}

//==================================================================================================

void SvgWriter::writeSvg(QXmlStreamWriter* xml, const QSize& size)
{
	QRectF sceneRect = mDrawing->sceneRect();
	QBrush backgroundBrush = mDrawing->backgroundBrush();

	xml->writeStartElement("svg");

	if (size.isValid())
	{
		xml->writeAttribute("width", QString::number(size.width()));
		xml->writeAttribute("height", QString::number(size.height()));
	}

	xml->writeAttribute("viewBox", QString("%1 %2 %3 %4").arg(sceneRect.left()).arg(
		sceneRect.top()).arg(sceneRect.width()).arg(sceneRect.height()));
	xml->writeAttribute("style", "background-color:" + colorToString(backgroundBrush.color()));
	xml->writeAttribute("xmlns", "http://www.w3.org/2000/svg");

	if (!mDrawing->referenceItems().isEmpty())
	{
		xml->writeStartElement("defs");
		writeItems(xml, mDrawing->referenceItems());
		xml->writeEndElement();
	}

	writeItems(xml, mDrawing->items());

	xml->writeEndElement();
}

void SvgWriter::writeItems(QXmlStreamWriter* xml, const QList<DrawingItem*>& items)
{
	QString key;

	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
	{
		key = (*itemIter)->uniqueKey();

		if (key == "rect") writeRectItem(xml, dynamic_cast<DrawingRectItem*>(*itemIter));
		else if (key == "ellipse") writeEllipseItem(xml, dynamic_cast<DrawingEllipseItem*>(*itemIter));
		else if (key == "line") writeLineItem(xml, dynamic_cast<DrawingLineItem*>(*itemIter));
		else if (key == "curve") writeCurveItem(xml, dynamic_cast<DrawingCurveItem*>(*itemIter));
		else if (key == "polyline") writePolylineItem(xml, dynamic_cast<DrawingPolylineItem*>(*itemIter));
		else if (key == "polygon") writePolygonItem(xml, dynamic_cast<DrawingPolygonItem*>(*itemIter));
		else if (key == "text") writeTextItem(xml, dynamic_cast<DrawingTextItem*>(*itemIter));
		else if (key == "textRect") writeTextRectItem(xml, dynamic_cast<DrawingTextRectItem*>(*itemIter));
		else if (key == "textEllipse") writeTextEllipseItem(xml, dynamic_cast<DrawingTextEllipseItem*>(*itemIter));
		else if (key == "path") writePathItem(xml, dynamic_cast<DrawingPathItem*>(*itemIter));
		else if (key == "group") writeGroupItem(xml, dynamic_cast<DrawingItemGroup*>(*itemIter));
		else if (key == "reference") writeReferenceItem(xml, dynamic_cast<DrawingReferenceItem*>(*itemIter));
	}
}

//==================================================================================================

void SvgWriter::writeRectItem(QXmlStreamWriter* xml, DrawingRectItem* item)
{
	if (item)
	{
		xml->writeStartElement("rect");
		if (item->name() != "") xml->writeAttribute("id", item->name());

		QRectF rect(item->mapToScene(item->rect().topLeft()),
					item->mapToScene(item->rect().bottomRight()));
		rect = rect.normalized();
		xml->writeAttribute("x", QString::number(rect.left()));
		xml->writeAttribute("y", QString::number(rect.top()));
		xml->writeAttribute("width", QString::number(rect.width()));
		xml->writeAttribute("height", QString::number(rect.height()));

		if (item->cornerRadius() > 0)
		{
			xml->writeAttribute("rx", QString::number(item->cornerRadius()));
			xml->writeAttribute("ry", QString::number(item->cornerRadius()));
		}

		xml->writeAttribute("style", styleToString(item->brush(), item->pen()));

		xml->writeEndElement();
	}
}

void SvgWriter::writeEllipseItem(QXmlStreamWriter* xml, DrawingEllipseItem* item)
{
	if (item)
	{
		xml->writeStartElement("ellipse");
		if (item->name() != "") xml->writeAttribute("id", item->name());

		QRectF ellipse(item->mapToScene(item->ellipse().topLeft()),
					item->mapToScene(item->ellipse().bottomRight()));
		ellipse = ellipse.normalized();
		xml->writeAttribute("cx", QString::number(ellipse.center().x()));
		xml->writeAttribute("cy", QString::number(ellipse.center().y()));
		xml->writeAttribute("rx", QString::number(ellipse.width() / 2));
		xml->writeAttribute("ry", QString::number(ellipse.height() / 2));

		xml->writeAttribute("style", styleToString(item->brush(), item->pen()));

		xml->writeEndElement();
	}
}

void SvgWriter::writeLineItem(QXmlStreamWriter* xml, DrawingLineItem* item)
{
	if (item)
	{
		Drawing::ArrowStyle startArrowStyle = item->startArrow().style();
		Drawing::ArrowStyle endArrowStyle = item->endArrow().style();

		if (startArrowStyle != Drawing::ArrowNone || endArrowStyle != Drawing::ArrowNone)
		{
			xml->writeStartElement("g");
			if (item->name() != "") xml->writeAttribute("id", item->name());

			xml->writeStartElement("line");
		}
		else
		{
			xml->writeStartElement("line");
			if (item->name() != "") xml->writeAttribute("id", item->name());
		}

		QPointF p1 = item->mapToScene(item->line().p1());
		QPointF p2 = item->mapToScene(item->line().p2());
		xml->writeAttribute("x1", QString::number(p1.x()));
		xml->writeAttribute("y1", QString::number(p1.y()));
		xml->writeAttribute("x2", QString::number(p2.x()));
		xml->writeAttribute("y2", QString::number(p2.y()));

		xml->writeAttribute("style", styleToString(Qt::transparent, item->pen()));

		if (startArrowStyle != Drawing::ArrowNone || endArrowStyle != Drawing::ArrowNone)
		{
			xml->writeEndElement();

			if (startArrowStyle != Drawing::ArrowNone) writeArrow(xml, item, item->startArrow(), item->pen());
			if (endArrowStyle != Drawing::ArrowNone) writeArrow(xml, item, item->endArrow(), item->pen());

			xml->writeEndElement();
		}
		else xml->writeEndElement();
	}
}

void SvgWriter::writeCurveItem(QXmlStreamWriter* xml, DrawingCurveItem* item)
{
	if (item)
	{
		Drawing::ArrowStyle startArrowStyle = item->startArrow().style();
		Drawing::ArrowStyle endArrowStyle = item->endArrow().style();

		if (startArrowStyle != Drawing::ArrowNone || endArrowStyle != Drawing::ArrowNone)
		{
			xml->writeStartElement("g");
			if (item->name() != "") xml->writeAttribute("id", item->name());

			xml->writeStartElement("path");
		}
		else
		{
			xml->writeStartElement("path");
			if (item->name() != "") xml->writeAttribute("id", item->name());
		}

		QPointF startPos = item->mapToScene(item->curveStartPosition());
		QPointF startControlPos = item->mapToScene(item->curveStartControlPosition());
		QPointF endControlPos = item->mapToScene(item->curveEndPosition());
		QPointF endPos = item->mapToScene(item->curveEndControlPosition());
		xml->writeAttribute("d", QString("M %1,%2 C %3,%4 %5,%6 %7,%8").arg(
			startPos.x()).arg(startPos.y()).arg(
			startControlPos.x()).arg(startControlPos.y()).arg(
			endControlPos.x()).arg(endControlPos.y()).arg(
			endPos.x()).arg(endPos.y()));

		xml->writeAttribute("style", styleToString(Qt::transparent, item->pen()));

		if (startArrowStyle != Drawing::ArrowNone || endArrowStyle != Drawing::ArrowNone)
		{
			xml->writeEndElement();

			if (startArrowStyle != Drawing::ArrowNone) writeArrow(xml, item, item->startArrow(), item->pen());
			if (endArrowStyle != Drawing::ArrowNone) writeArrow(xml, item, item->endArrow(), item->pen());

			xml->writeEndElement();
		}
		else xml->writeEndElement();
	}
}

void SvgWriter::writePolylineItem(QXmlStreamWriter* xml, DrawingPolylineItem* item)
{
	if (item)
	{
		Drawing::ArrowStyle startArrowStyle = item->startArrow().style();
		Drawing::ArrowStyle endArrowStyle = item->endArrow().style();

		if (startArrowStyle != Drawing::ArrowNone || endArrowStyle != Drawing::ArrowNone)
		{
			xml->writeStartElement("g");
			if (item->name() != "") xml->writeAttribute("id", item->name());

			xml->writeStartElement("polyline");
		}
		else
		{
			xml->writeStartElement("polyline");
			if (item->name() != "") xml->writeAttribute("id", item->name());
		}

		xml->writeAttribute("points", pointsToString(item->mapToScene(item->polyline())));

		xml->writeAttribute("style", styleToString(Qt::transparent, item->pen()));

		if (startArrowStyle != Drawing::ArrowNone || endArrowStyle != Drawing::ArrowNone)
		{
			xml->writeEndElement();

			if (startArrowStyle != Drawing::ArrowNone) writeArrow(xml, item, item->startArrow(), item->pen());
			if (endArrowStyle != Drawing::ArrowNone) writeArrow(xml, item, item->endArrow(), item->pen());

			xml->writeEndElement();
		}
		else xml->writeEndElement();
	}
}

void SvgWriter::writePolygonItem(QXmlStreamWriter* xml, DrawingPolygonItem* item)
{
	if (item)
	{
		xml->writeStartElement("polygon");
		if (item->name() != "") xml->writeAttribute("id", item->name());

		xml->writeAttribute("points", pointsToString(item->mapToScene(item->polygon())));

		xml->writeAttribute("style", styleToString(item->brush(), item->pen()));

		xml->writeEndElement();
	}
}

void SvgWriter::writeTextItem(QXmlStreamWriter* xml, DrawingTextItem* item)
{
	if (item)
	{
		xml->writeStartElement("text");
		if (item->name() != "") xml->writeAttribute("id", item->name());

		xml->writeAttribute("transform", transformToString(item->position(), item->transform()));

		if (item->alignment() & Qt::AlignTop)
			xml->writeAttribute("y", QString::number(item->font().pointSizeF() * 3 / 4));
		if (item->alignment() & Qt::AlignBottom)
			xml->writeAttribute("y", QString::number(-item->font().pointSizeF() * 3 / 4));

		xml->writeAttribute("style", styleToString(item->textBrush(), Qt::NoPen, item->font(), item->alignment()));

		xml->writeCharacters(item->caption());

		xml->writeEndElement();
	}
}

void SvgWriter::writeTextRectItem(QXmlStreamWriter* xml, DrawingTextRectItem* item)
{
	if (item)
	{
		xml->writeStartElement("g");
		if (item->name() != "") xml->writeAttribute("id", item->name());

		xml->writeAttribute("transform", transformToString(item->position(), item->transform()));

		// Rect
		xml->writeStartElement("rect");

		QRectF rect = item->rect().normalized();
		xml->writeAttribute("x", QString::number(rect.left()));
		xml->writeAttribute("y", QString::number(rect.top()));
		xml->writeAttribute("width", QString::number(rect.width()));
		xml->writeAttribute("height", QString::number(rect.height()));

		if (item->cornerRadius() > 0)
		{
			xml->writeAttribute("rx", QString::number(item->cornerRadius()));
			xml->writeAttribute("ry", QString::number(item->cornerRadius()));
		}

		xml->writeAttribute("style", styleToString(item->brush(), item->pen()));

		xml->writeEndElement();

		// Text
		xml->writeStartElement("text");

		xml->writeAttribute("x", QString::number(rect.center().x()));
		xml->writeAttribute("y", QString::number(rect.center().y()));

		xml->writeAttribute("style", styleToString(item->textBrush(), Qt::NoPen, item->font(), Qt::AlignCenter));

		xml->writeCharacters(item->caption());

		xml->writeEndElement();

		xml->writeEndElement();
	}
}

void SvgWriter::writeTextEllipseItem(QXmlStreamWriter* xml, DrawingTextEllipseItem* item)
{
	if (item)
	{
		xml->writeStartElement("g");
		if (item->name() != "") xml->writeAttribute("id", item->name());

		xml->writeAttribute("transform", transformToString(item->position(), item->transform()));

		// Ellipse
		xml->writeStartElement("ellipse");

		QRectF ellipse = item->ellipse().normalized();
		xml->writeAttribute("cx", QString::number(ellipse.center().x()));
		xml->writeAttribute("cy", QString::number(ellipse.center().y()));
		xml->writeAttribute("rx", QString::number(ellipse.width() / 2));
		xml->writeAttribute("ry", QString::number(ellipse.height() / 2));

		xml->writeAttribute("style", styleToString(item->brush(), item->pen()));

		xml->writeEndElement();

		// Text
		xml->writeStartElement("text");

		xml->writeAttribute("x", QString::number(ellipse.center().x()));
		xml->writeAttribute("y", QString::number(ellipse.center().y()));

		xml->writeAttribute("style", styleToString(item->textBrush(), Qt::NoPen, item->font(), Qt::AlignCenter));

		xml->writeCharacters(item->caption());

		xml->writeEndElement();

		xml->writeEndElement();
	}
}

void SvgWriter::writePathItem(QXmlStreamWriter* xml, DrawingPathItem* item)
{
	if (item)
	{
		QPainterPath path = item->path();
		QString pathStr;

		xml->writeStartElement("path");
		if (item->name() != "") xml->writeAttribute("id", item->name());

		xml->writeAttribute("transform", transformToString(item->position(), item->transform()));

		for(int i = 0; i < path.elementCount(); i++)
		{
			QPainterPath::Element element = path.elementAt(i);
			QPointF mappedElement = item->mapFromPath(QPointF(element.x, element.y));

			switch (element.type)
			{
			case QPainterPath::MoveToElement:
				pathStr += "M " + QString::number(mappedElement.x()) + " " + QString::number(mappedElement.y()) + " ";
				break;
			case QPainterPath::LineToElement:
				pathStr += "L " + QString::number(mappedElement.x()) + " " + QString::number(mappedElement.y()) + " ";
				break;
			case QPainterPath::CurveToElement:
				pathStr += "C " + QString::number(mappedElement.x()) + " " + QString::number(mappedElement.y()) + " ";
				break;
			case QPainterPath::CurveToDataElement:
				pathStr += QString::number(mappedElement.x()) + " " + QString::number(mappedElement.y()) + " ";
				break;
			}
		}
		xml->writeAttribute("d", pathStr.trimmed());

		xml->writeAttribute("style", styleToString(Qt::transparent, item->pen()));

		xml->writeEndElement();
	}
}

void SvgWriter::writeGroupItem(QXmlStreamWriter* xml, DrawingItemGroup* item)
{

}

void SvgWriter::writeReferenceItem(QXmlStreamWriter* xml, DrawingReferenceItem* item)
{

}

//==================================================================================================

void SvgWriter::writeArrow(QXmlStreamWriter* xml, DrawingItem* item, const DrawingArrow& arrow, const QPen& pen)
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
			item->mapToScene(polygon[0]).x()).arg(item->mapToScene(polygon[0]).y()).arg(
			item->mapToScene(polygon[1]).x()).arg(item->mapToScene(polygon[1]).y()).arg(
			item->mapToScene(polygon[2]).x()).arg(item->mapToScene(polygon[2]).y()));
		xml->writeAttribute("style", styleToString(Qt::transparent, arrowPen));
		xml->writeEndElement();
		break;
	case Drawing::ArrowTriangle:
	case Drawing::ArrowTriangleFilled:
	case Drawing::ArrowConcave:
	case Drawing::ArrowConcaveFilled:
		polygon = arrow.polygon();
		xml->writeStartElement("polygon");
		xml->writeAttribute("points", pointsToString(item->mapToScene(polygon)));
		if (arrow.style() == Drawing::ArrowTriangleFilled || arrow.style() == Drawing::ArrowConcaveFilled)
			xml->writeAttribute("style", styleToString(arrowPen.brush(), arrowPen));
		else if (item->widget())
			xml->writeAttribute("style", styleToString(item->widget()->backgroundBrush(), arrowPen));
		else
			xml->writeAttribute("style", styleToString(Qt::transparent, arrowPen));
		xml->writeEndElement();
		break;
	case Drawing::ArrowCircle:
	case Drawing::ArrowCircleFilled:
		xml->writeStartElement("ellipse");
		xml->writeAttribute("cx", QString::number(item->mapToScene(arrow.position()).x()));
		xml->writeAttribute("cy", QString::number(item->mapToScene(arrow.position()).y()));
		xml->writeAttribute("rx", QString::number(arrow.size() / 2));
		xml->writeAttribute("ry", QString::number(arrow.size() / 2));
		if (arrow.style() == Drawing::ArrowCircleFilled)
			xml->writeAttribute("style", styleToString(arrowPen.brush(), arrowPen));
		else if (item->widget())
			xml->writeAttribute("style", styleToString(item->widget()->backgroundBrush(), arrowPen));
		else
			xml->writeAttribute("style", styleToString(Qt::transparent, arrowPen));
		xml->writeEndElement();
		break;
	default:
		break;
	}
}

//==================================================================================================

QString SvgWriter::colorToString(const QColor& color) const
{
	return QString("#%1%2%3").arg(color.red(), 2, 16, QChar('0')).arg(
		color.green(), 2, 16, QChar('0')).arg(color.blue(), 2, 16, QChar('0')).toUpper();
}

QString SvgWriter::pointsToString(const QPolygonF& polygon) const
{
	QString pointsStr;

	for(auto pointIter = polygon.begin(); pointIter != polygon.end(); pointIter++)
		pointsStr += QString::number((*pointIter).x()) + "," + QString::number((*pointIter).y()) + " ";

	return pointsStr.trimmed();
}

QString SvgWriter::styleToString(const QBrush& brush, const QPen& pen)
{
	QString styleString;

	QColor fillColor = brush.color();
	QColor strokeColor = pen.brush().color();

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

	if (pen.style() != Qt::NoPen)
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
			styleString += ";stroke-width:" + QString::number(pen.widthF());

			// stroke-dasharray
			switch (pen.style())
			{
			case Qt::DashLine:
				styleString += QString(";stroke-dasharray:%1 %2").arg(3 * pen.widthF()).arg(2 * pen.widthF());
				break;
			case Qt::DotLine:
				styleString += QString(";stroke-dasharray:%1 %2").arg(pen.widthF()).arg(2 * pen.widthF());
				break;
			case Qt::DashDotLine:
				styleString += QString(";stroke-dasharray:%1 %3 %2 %3").arg(3 * pen.widthF()).arg(pen.widthF()).arg(2 * pen.widthF());
				break;
			case Qt::DashDotDotLine:
				styleString += QString(";stroke-dasharray:%1 %3 %2 %3 %2 %3").arg(3 * pen.widthF()).arg(pen.widthF()).arg(2 * pen.widthF());
				break;
			default:
				break;
			}

			// stroke-linecap
			switch (pen.capStyle())
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
			switch (pen.joinStyle())
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

	return styleString;
}

QString SvgWriter::styleToString(const QBrush& brush, const QPen& pen, const QFont& font, Qt::Alignment alignment)
{
	QString styleString = styleToString(brush, pen);

	styleString += ";font-family:" + font.family();
	styleString += ";font-size:" + QString::number(font.pointSizeF()) + "px";

	if (font.bold()) styleString += ";font-weight:bold";
	if (font.italic()) styleString += ";font-style:italic";
	if (font.underline()) styleString += ";text-decoration:underline";
	else if (font.strikeOut()) styleString += ";text-decoration:line-through";

	Qt::Alignment horizontalAlign = (alignment & Qt::AlignHorizontal_Mask);
	if (horizontalAlign & Qt::AlignHCenter)
		 styleString += ";text-anchor:middle";
	else if (horizontalAlign & Qt::AlignRight)
		 styleString += ";text-anchor:end";

	styleString += ";dominant-baseline:central";

	return styleString;
}

QString SvgWriter::transformToString(const QPointF& position, const QTransform& transform) const
{
	QString transformStr;

	QTransform transformCopy = transform;
	qreal rotation = qAsin(transformCopy.m12()) * 180 / 3.141592654;
	transformCopy.rotate(-rotation);

	qreal hScale = transformCopy.m11();
	qreal vScale = transformCopy.m22();

	transformStr = "translate(" + QString::number(position.x()) + "," + QString::number(position.y()) + ")";
	transformStr += (rotation != 0) ? " rotate(" + QString::number(-rotation) + ")" : "";
	transformStr += (hScale != 1.0 || vScale != 1.0) ? " scale(" + QString::number(hScale) + "," + QString::number(vScale) + ")" : "";

	return transformStr;
}
