/* DrawingReader.cpp
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

#include "DrawingReader.h"
#include <DrawingWidget.h>
#include <DrawingItemGroup.h>
#include <DrawingItemPoint.h>
#include <DrawingPathItem.h>
#include <DrawingShapeItems.h>
#include <DrawingTextItem.h>
#include <DrawingTwoPointItems.h>

DrawingReader::DrawingReader(QIODevice* device) : QXmlStreamReader(device) { }

DrawingReader::~DrawingReader() { }

//==================================================================================================

void DrawingReader::read(DrawingWidget* drawing)
{
	QList<DrawingItem*> items;
	bool pageFound = false;

	while (readNextStartElement())
	{
		if (name() == "jade-drawing")
		{
			while (readNextStartElement())
			{
				if (name() == "page" && !pageFound)
				{
					// Read scene properties
					QXmlStreamAttributes attr = attributes();

					QRectF sceneRect = drawing->sceneRect();
					if (attr.hasAttribute("view-left")) sceneRect.setLeft(attr.value("view-left").toDouble());
					if (attr.hasAttribute("view-top")) sceneRect.setTop(attr.value("view-top").toDouble());
					if (attr.hasAttribute("view-width")) sceneRect.setWidth(attr.value("view-width").toDouble());
					if (attr.hasAttribute("view-height")) sceneRect.setHeight(attr.value("view-height").toDouble());
					drawing->setSceneRect(sceneRect);

					if (attr.hasAttribute("background-color"))
						drawing->setBackgroundBrush(colorFromString(attr.value("background-color").toString()));

					if (attr.hasAttribute("grid"))
						drawing->setGrid(attr.value("grid").toDouble());

					if (attr.hasAttribute("grid-color"))
						drawing->setGridBrush(colorFromString(attr.value("grid-color").toString()));
					if (attr.hasAttribute("grid-style"))
						drawing->setGridStyle(gridStyleFromString(attr.value("grid-style").toString()));
					if (attr.hasAttribute("grid-spacing-major"))
						drawing->setGridSpacing(attr.value("grid-spacing-major").toInt(), drawing->gridSpacingMinor());
					if (attr.hasAttribute("grid-spacing-minor"))
						drawing->setGridSpacing(drawing->gridSpacingMajor(), attr.value("grid-spacing-minor").toInt());

					// Read items
					while (readNextStartElement())
					{
						if (name() == "items")
						{
							items = readItemElements();
							for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
								drawing->addItem(*itemIter);
						}
						else skipCurrentElement();
					}

					pageFound = true;
				}
				else skipCurrentElement();
			}
		}
		else skipCurrentElement();
	}
}

void DrawingReader::readItems(QList<DrawingItem*>& items)
{
	while (readNextStartElement())
	{
		if (name() == "jade-items")
			items.append(readItemElements());
		else
			skipCurrentElement();
	}
}

//==================================================================================================

QList<DrawingItem*> DrawingReader::readItemElements()
{
	QList<DrawingItem*> items;

	DrawingItem* newItem = nullptr;
	QString itemName;
	QList<DrawingItemPoint*> points, otherPoints;

	while (readNextStartElement())
	{
		newItem = nullptr;
		itemName = name().toString();

		if (itemName == "line") newItem = readLineItem();
		else if (itemName == "arc") newItem = readArcItem();
		else if (itemName == "polyline") newItem = readPolylineItem();
		else if (itemName == "curve") newItem = readCurveItem();
		else if (itemName == "rect") newItem = readRectItem();
		else if (itemName == "ellipse") newItem = readEllipseItem();
		else if (itemName == "polygon") newItem = readPolygonItem();
		else if (itemName == "text") newItem = readTextItem();
		else if (itemName == "path") newItem = readPathItem();
		else if (itemName == "group") newItem = readItemGroup();
		else skipCurrentElement();

		if (newItem) items.append(newItem);
	}

	// Make item connections
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
	{
		for(auto otherItemIter = itemIter + 1; otherItemIter != items.end(); otherItemIter++)
		{
			points = (*itemIter)->points();
			otherPoints = (*otherItemIter)->points();

			for(auto itemPointIter = points.begin(); itemPointIter != points.end(); itemPointIter++)
			{
				for(auto otherItemPointIter = otherPoints.begin(); otherItemPointIter != otherPoints.end(); otherItemPointIter++)
				{
					if ((*itemPointIter)->shouldConnect(*otherItemPointIter))
					{
						(*itemPointIter)->addConnection(*otherItemPointIter);
						(*otherItemPointIter)->addConnection(*itemPointIter);
					}
				}
			}
		}
	}

	return items;
}

//==================================================================================================

DrawingLineItem* DrawingReader::readLineItem()
{
	DrawingLineItem* item = new DrawingLineItem();
	QXmlStreamAttributes attr = attributes();

	if (attr.hasAttribute("transform")) transformFromString(attr.value("transform").toString(), item);

	if (attr.hasAttribute("x1")) item->startPoint()->setX(attr.value("x1").toDouble());
	if (attr.hasAttribute("y1")) item->startPoint()->setY(attr.value("y1").toDouble());
	if (attr.hasAttribute("x2")) item->endPoint()->setX(attr.value("x2").toDouble());
	if (attr.hasAttribute("y2")) item->endPoint()->setY(attr.value("y2").toDouble());
	item->resizeItem(item->endPoint(), item->mapToScene(item->endPoint()->pos()));

	readItemProperties(item);

	skipCurrentElement();

	return item;
}

DrawingArcItem* DrawingReader::readArcItem()
{
	DrawingArcItem* item = new DrawingArcItem();
	QXmlStreamAttributes attr = attributes();

	if (attr.hasAttribute("transform")) transformFromString(attr.value("transform").toString(), item);

	if (attr.hasAttribute("x1")) item->startPoint()->setX(attr.value("x1").toDouble());
	if (attr.hasAttribute("y1")) item->startPoint()->setY(attr.value("y1").toDouble());
	if (attr.hasAttribute("x2")) item->endPoint()->setX(attr.value("x2").toDouble());
	if (attr.hasAttribute("y2")) item->endPoint()->setY(attr.value("y2").toDouble());

	readItemProperties(item);

	skipCurrentElement();

	return item;
}

DrawingPolylineItem* DrawingReader::readPolylineItem()
{
	DrawingPolylineItem* item = new DrawingPolylineItem();
	QXmlStreamAttributes attr = attributes();

	if (attr.hasAttribute("transform")) transformFromString(attr.value("transform").toString(), item);

	if (attr.hasAttribute("points"))
	{
		QPolygonF polygon = pointsFromString(attr.value("points").toString());
		if (!polygon.isEmpty())
		{
			item->clearPoints();
			for(auto polyIter = polygon.begin(); polyIter != polygon.end(); polyIter++)
			{
				if (polyIter == polygon.begin() || polyIter == polygon.end() - 1)
					item->addPoint(new DrawingItemPoint(*polyIter, DrawingItemPoint::Control | DrawingItemPoint::Connection | DrawingItemPoint::Free));
				else
					item->addPoint(new DrawingItemPoint(*polyIter, DrawingItemPoint::Control | DrawingItemPoint::Connection));
			}
		}
	}

	readItemProperties(item);

	skipCurrentElement();

	return item;
}

DrawingCurveItem* DrawingReader::readCurveItem()
{
	DrawingCurveItem* item = new DrawingCurveItem();
	QXmlStreamAttributes attr = attributes();

	if (attr.hasAttribute("transform")) transformFromString(attr.value("transform").toString(), item);

	if (attr.hasAttribute("x1")) item->startPoint()->setX(attr.value("x1").toDouble());
	if (attr.hasAttribute("y1")) item->startPoint()->setY(attr.value("y1").toDouble());
	if (attr.hasAttribute("cx1")) item->startControlPoint()->setX(attr.value("cx1").toDouble());
	if (attr.hasAttribute("cy1")) item->startControlPoint()->setY(attr.value("cy1").toDouble());
	if (attr.hasAttribute("cx2")) item->endControlPoint()->setX(attr.value("cx2").toDouble());
	if (attr.hasAttribute("cy2")) item->endControlPoint()->setY(attr.value("cy2").toDouble());
	if (attr.hasAttribute("x2")) item->endPoint()->setX(attr.value("x2").toDouble());
	if (attr.hasAttribute("y2")) item->endPoint()->setY(attr.value("y2").toDouble());

	readItemProperties(item);

	skipCurrentElement();

	return item;
}

DrawingRectItem* DrawingReader::readRectItem()
{
	DrawingRectItem* item = new DrawingRectItem();
	QXmlStreamAttributes attr = attributes();

	if (attr.hasAttribute("transform")) transformFromString(attr.value("transform").toString(), item);

	if (attr.hasAttribute("left")) item->topLeftPoint()->setX(attr.value("left").toDouble());
	if (attr.hasAttribute("top")) item->topLeftPoint()->setX(attr.value("top").toDouble());
	if (attr.hasAttribute("width")) item->bottomRightPoint()->setX(attr.value("width").toDouble());
	if (attr.hasAttribute("height")) item->bottomRightPoint()->setX(attr.value("height").toDouble());
	item->resizeItem(item->bottomRightPoint(), item->mapToScene(item->bottomRightPoint()->pos()));

	if (attr.hasAttribute("rx")) item->setCornerRadii(attr.value("rx").toDouble(), item->cornerRadiusY());
	if (attr.hasAttribute("ry")) item->setCornerRadii(item->cornerRadiusX(), attr.value("ry").toDouble());

	readItemProperties(item);

	skipCurrentElement();

	return item;
}

DrawingEllipseItem* DrawingReader::readEllipseItem()
{
	DrawingEllipseItem* item = new DrawingEllipseItem();
	QXmlStreamAttributes attr = attributes();

	if (attr.hasAttribute("transform")) transformFromString(attr.value("transform").toString(), item);

	if (attr.hasAttribute("left")) item->topLeftPoint()->setX(attr.value("left").toDouble());
	if (attr.hasAttribute("top")) item->topLeftPoint()->setX(attr.value("top").toDouble());
	if (attr.hasAttribute("width")) item->bottomRightPoint()->setX(attr.value("width").toDouble());
	if (attr.hasAttribute("height")) item->bottomRightPoint()->setX(attr.value("height").toDouble());
	item->resizeItem(item->bottomRightPoint(), item->mapToScene(item->bottomRightPoint()->pos()));

	readItemProperties(item);

	skipCurrentElement();

	return item;
}

DrawingPolygonItem* DrawingReader::readPolygonItem()
{
	DrawingPolygonItem* item = new DrawingPolygonItem();
	QXmlStreamAttributes attr = attributes();

	if (attr.hasAttribute("transform")) transformFromString(attr.value("transform").toString(), item);

	if (attr.hasAttribute("points"))
	{
		QPolygonF polygon = pointsFromString(attr.value("points").toString());
		if (!polygon.isEmpty())
		{
			item->clearPoints();
			for(auto polyIter = polygon.begin(); polyIter != polygon.end(); polyIter++)
				item->addPoint(new DrawingItemPoint(*polyIter, DrawingItemPoint::Control | DrawingItemPoint::Connection));
		}
	}

	readItemProperties(item);

	skipCurrentElement();

	return item;
}

DrawingTextItem* DrawingReader::readTextItem()
{
	DrawingTextItem* item = new DrawingTextItem();
	QXmlStreamAttributes attr = attributes();

	if (attr.hasAttribute("transform")) transformFromString(attr.value("transform").toString(), item);

	readItemProperties(item);

	if (readNext() == QXmlStreamReader::Characters)
	{
		item->setCaption(text().toString());
		skipCurrentElement();
	}

	return item;
}

DrawingPathItem* DrawingReader::readPathItem()
{
	DrawingPathItem* item = new DrawingPathItem();
	QXmlStreamAttributes attr = attributes();

	if (attr.hasAttribute("name")) item->setName(attr.value("name").toString());

	if (attr.hasAttribute("transform")) transformFromString(attr.value("transform").toString(), item);

	if (attr.hasAttribute("left")) item->topLeftPoint()->setX(attr.value("left").toDouble());
	if (attr.hasAttribute("top")) item->topLeftPoint()->setX(attr.value("top").toDouble());
	if (attr.hasAttribute("width")) item->bottomRightPoint()->setX(attr.value("width").toDouble());
	if (attr.hasAttribute("height")) item->bottomRightPoint()->setX(attr.value("height").toDouble());
	item->resizeItem(item->bottomRightPoint(), item->mapToScene(item->bottomRightPoint()->pos()));

	QRectF pathRect = item->pathRect();
	if (attr.hasAttribute("view-left")) pathRect.setLeft(attr.value("view-left").toDouble());
	if (attr.hasAttribute("view-top")) pathRect.setTop(attr.value("view-top").toDouble());
	if (attr.hasAttribute("view-width")) pathRect.setWidth(attr.value("view-width").toDouble());
	if (attr.hasAttribute("view-height")) pathRect.setHeight(attr.value("view-height").toDouble());
	item->setPath(item->path(), pathRect);

	if (attr.hasAttribute("d"))
		item->setPath(pathFromString(attr.value("d").toString()), pathRect);

	if (attr.hasAttribute("glue-points"))
		item->addConnectionPoints(pointsFromString(attr.value("glue-points").toString()));

	readItemProperties(item);

	skipCurrentElement();

	return item;
}

DrawingItemGroup* DrawingReader::readItemGroup()
{
	DrawingItemGroup* item = new DrawingItemGroup();
	QXmlStreamAttributes attr = attributes();

	if (attr.hasAttribute("transform")) transformFromString(attr.value("transform").toString(), item);

	item->setItems(readItemElements());

	skipCurrentElement();

	return item;
}

//==================================================================================================

void DrawingReader::readItemProperties(DrawingItem* item)
{
	QXmlStreamAttributes attr = attributes();

	// Pen
	if (attr.hasAttribute("stroke-color"))
	{
		QColor penColor = colorFromString(attr.value("stroke-color").toString());
		if (attr.hasAttribute("stroke-opacity"))
			penColor.setAlphaF(attr.value("stroke-opacity").toDouble());
		item->setProperty("Pen Color", QVariant(penColor));
	}
	if (attr.hasAttribute("stroke-width"))
		item->setProperty("Pen Width", QVariant(attr.value("stroke-width").toDouble()));
	if (attr.hasAttribute("stroke-style"))
		item->setProperty("Pen Style", QVariant((unsigned int)penStyleFromString(attr.value("stroke-style").toString())));
	if (attr.hasAttribute("stroke-linecap"))
		item->setProperty("Pen Cap Style", QVariant((unsigned int)penCapStyleFromString(attr.value("stroke-linecap").toString())));
	if (attr.hasAttribute("stroke-linejoin"))
		item->setProperty("Pen Join Style", QVariant((unsigned int)penJoinStyleFromString(attr.value("stroke-linejoin").toString())));

	// Brush
	if (attr.hasAttribute("fill-color"))
	{
		QColor brushColor = colorFromString(attr.value("fill-color").toString());
		if (attr.hasAttribute("fill-opacity"))
			brushColor.setAlphaF(attr.value("fill-opacity").toDouble());
		item->setProperty("Brush Color", QVariant(brushColor));
	}

	// Arrows
	if (attr.hasAttribute("arrow-start-style"))
		item->setProperty("Start Arrow Style", QVariant((unsigned int)arrowStyleFromString(attr.value("arrow-start-style").toString())));
	if (attr.hasAttribute("arrow-start-size"))
		item->setProperty("Start Arrow Size", QVariant(attr.value("arrow-start-size").toDouble()));
	if (attr.hasAttribute("arrow-end-style"))
		item->setProperty("End Arrow Style", QVariant((unsigned int)arrowStyleFromString(attr.value("arrow-end-style").toString())));
	if (attr.hasAttribute("arrow-end-size"))
		item->setProperty("End Arrow Size", QVariant(attr.value("arrow-end-size").toDouble()));

	// Font
	if (attr.hasAttribute("font-name"))
		item->setProperty("Font Family", QVariant(attr.value("font-name").toString()));
	if (attr.hasAttribute("font-size"))
		item->setProperty("Font Size", QVariant(attr.value("font-size").toDouble()));
	if (attr.hasAttribute("font-bold"))
		item->setProperty("Font Bold", QVariant(attr.value("font-bold").toString().toLower() == "true"));
	if (attr.hasAttribute("font-italic"))
		item->setProperty("Font Italic", QVariant(attr.value("font-italic").toString().toLower() == "true"));
	if (attr.hasAttribute("font-underline"))
		item->setProperty("Font Underline", QVariant(attr.value("font-underline").toString().toLower() == "true"));
	if (attr.hasAttribute("font-overline"))
		item->setProperty("Font Overline", QVariant(attr.value("font-overline").toString().toLower() == "true"));
	if (attr.hasAttribute("font-strike-through"))
		item->setProperty("Font Strike-Out", QVariant(attr.value("font-strike-through").toString().toLower() == "true"));

	// Text Alignment
	if (attr.hasAttribute("text-alignment-horizontal"))
		item->setProperty("Text Horizontal Alignment", QVariant((unsigned int)alignmentFromString(attr.value("text-alignment-horizontal").toString())));
	if (attr.hasAttribute("text-alignment-vertical"))
		item->setProperty("Text Vertical Alignment", QVariant((unsigned int)alignmentFromString(attr.value("text-alignment-vertical").toString())));

	// Text color
	if (attr.hasAttribute("text-color"))
	{
		QColor textColor = colorFromString(attr.value("text-color").toString());
		if (attr.hasAttribute("text-opacity"))
			textColor.setAlphaF(attr.value("text-opacity").toDouble());
		item->setProperty("Text Color", QVariant(textColor));
	}
}

//==================================================================================================

Qt::Alignment DrawingReader::alignmentFromString(const QString& str) const
{
	Qt::Alignment align;

	if (str == "left") align = Qt::AlignLeft;
	else if (str == "right") align = Qt::AlignRight;
	else if (str == "center") align = Qt::AlignHCenter;
	else if (str == "top") align = Qt::AlignTop;
	else if (str == "bottom") align = Qt::AlignBottom;
	else if (str == "middle") align = Qt::AlignVCenter;

	return align;
}

DrawingArrowStyle DrawingReader::arrowStyleFromString(const QString& str) const
{
	DrawingArrowStyle style = ArrowNone;

	if (str == "normal") style = ArrowNormal;
	else if (str == "triangle") style = ArrowTriangle;
	else if (str == "triangle-filled") style = ArrowTriangleFilled;
	else if (str == "circle") style = ArrowCircle;
	else if (str == "circle-filled") style = ArrowCircleFilled;
	else if (str == "diamond") style = ArrowDiamond;
	else if (str == "diamond-filled") style = ArrowDiamondFilled;
	else if (str == "harpoon") style = ArrowHarpoon;
	else if (str == "harpoon-mirrored") style = ArrowHarpoonMirrored;
	else if (str == "concave") style = ArrowConcave;
	else if (str == "concave-filled") style = ArrowConcaveFilled;
	else if (str == "reverse") style = ArrowReverse;
	else if (str == "X") style = ArrowX;

	return style;
}

QColor DrawingReader::colorFromString(const QString& str) const
{
	QColor color;

	color.setRed(str.mid(1,2).toUInt(nullptr, 16));
	color.setGreen(str.mid(3,2).toUInt(nullptr, 16));
	color.setBlue(str.mid(5,2).toUInt(nullptr, 16));

	return color;
}

DrawingGridStyle DrawingReader::gridStyleFromString(const QString& str) const
{
	DrawingGridStyle style = GridNone;

	if (str == "dotted") style = GridDotted;
	else if (str == "lined") style = GridLined;
	else if (str == "graphpaper") style = GridGraphPaper;

	return style;
}

QPainterPath DrawingReader::pathFromString(const QString& str) const
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

Qt::PenStyle DrawingReader::penStyleFromString(const QString& str) const
{
	Qt::PenStyle style = Qt::SolidLine;

	if (str == "none") style = Qt::NoPen;
	else if (str == "dash") style = Qt::DashLine;
	else if (str == "dot") style = Qt::DotLine;
	else if (str == "dash-dot") style = Qt::DashDotLine;
	else if (str == "dash-dot-dot") style = Qt::DashDotDotLine;

	return style;
}

Qt::PenCapStyle DrawingReader::penCapStyleFromString(const QString& str) const
{
	Qt::PenCapStyle style = Qt::RoundCap;

	if (str == "flat") style = Qt::FlatCap;
	else if (str == "square") style = Qt::SquareCap;

	return style;
}

Qt::PenJoinStyle DrawingReader::penJoinStyleFromString(const QString& str) const
{
	Qt::PenJoinStyle style = Qt::RoundJoin;

	if (str == "miter") style = Qt::MiterJoin;
	else if (str == "bevel") style = Qt::BevelJoin;

	return style;
}

QPolygonF DrawingReader::pointsFromString(const QString& str) const
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

void DrawingReader::transformFromString(const QString& str, DrawingItem* item)
{
	QStringList tokens = str.split(QRegExp("\\s+"));
	for(auto tokenIter = tokens.begin(); tokenIter != tokens.end(); tokenIter++)
	{
		if (tokenIter->startsWith("translate("))
		{
			int endIndex = tokenIter->indexOf(")");
			QStringList coords = tokenIter->mid(10, endIndex - 11).split(",");
			if (coords.size() == 2)
			{
				item->setX(coords.first().toDouble());
				item->setY(coords.last().toDouble());
			}
		}
		else if (tokenIter->startsWith("rotate("))
		{
			int endIndex = tokenIter->indexOf(")");
			QString angle = tokenIter->mid(7, endIndex - 8);
			item->setRotation(angle.toDouble());
		}
		else if (tokenIter->startsWith("scale("))
		{
			int endIndex = tokenIter->indexOf(")");
			QStringList coords = tokenIter->mid(6, endIndex - 7).split(",");
			if (coords.size() == 2) item->setFlipped(coords.first().toDouble() < 0);
		}
	}
}
