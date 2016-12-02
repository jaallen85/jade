/* DiagramReader.cpp
 *
 * Copyright (C) 2013-2016 Jason Allen
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

#include "DiagramReader.h"

DiagramReader::DiagramReader(QIODevice* device) : QXmlStreamReader(device) { }

DiagramReader::DiagramReader(const QString& data) : QXmlStreamReader(data) { }

DiagramReader::~DiagramReader() { }

//==================================================================================================

void DiagramReader::read(DiagramWidget* diagram)
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

					QRectF sceneRect = diagram->sceneRect();
					if (attr.hasAttribute("view-left")) sceneRect.setLeft(attr.value("view-left").toDouble());
					if (attr.hasAttribute("view-top")) sceneRect.setTop(attr.value("view-top").toDouble());
					if (attr.hasAttribute("view-width")) sceneRect.setWidth(attr.value("view-width").toDouble());
					if (attr.hasAttribute("view-height")) sceneRect.setHeight(attr.value("view-height").toDouble());
					diagram->setSceneRect(sceneRect);

					if (attr.hasAttribute("background-color"))
						diagram->setBackgroundBrush(colorFromString(attr.value("background-color").toString()));

					if (attr.hasAttribute("grid"))
						diagram->setGrid(attr.value("grid").toDouble());

					if (attr.hasAttribute("grid-color"))
						diagram->setGridBrush(colorFromString(attr.value("grid-color").toString()));
					if (attr.hasAttribute("grid-style"))
						diagram->setGridStyle(gridStyleFromString(attr.value("grid-style").toString()));
					if (attr.hasAttribute("grid-spacing-major"))
						diagram->setGridSpacing(attr.value("grid-spacing-major").toInt(), diagram->gridSpacingMinor());
					if (attr.hasAttribute("grid-spacing-minor"))
						diagram->setGridSpacing(diagram->gridSpacingMajor(), attr.value("grid-spacing-minor").toInt());

					// Read items
					while (readNextStartElement())
					{
						if (name() == "items")
						{
							items = readItemElements();
							for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
								diagram->addItem(*itemIter);

							// Connect items together
							QList<DrawingItemPoint*> itemPoints, otherItemPoints;
							qreal distance, threshold = diagram->grid() / 4000;
							QPointF vec;
							for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
							{
								for(auto otherItemIter = itemIter + 1; otherItemIter != items.end(); otherItemIter++)
								{
									itemPoints = (*itemIter)->points();
									otherItemPoints = (*otherItemIter)->points();

									for(auto itemPointIter = itemPoints.begin(); itemPointIter != itemPoints.end(); itemPointIter++)
									{
										for(auto otherItemPointIter = otherItemPoints.begin();
											otherItemPointIter != otherItemPoints.end(); otherItemPointIter++)
										{
											if ((*itemPointIter)->isConnectionPoint() && (*otherItemPointIter)->isConnectionPoint() &&
												((*itemPointIter)->isFree() || (*otherItemPointIter)->isFree()))
											{
												vec = (*itemIter)->mapToScene((*itemPointIter)->pos()) -
													(*otherItemIter)->mapToScene((*otherItemPointIter)->pos());
												distance = qSqrt(vec.x() * vec.x() + vec.y() * vec.y());

												if (distance <= threshold)
												{
													(*itemPointIter)->addConnection(*otherItemPointIter);
													(*otherItemPointIter)->addConnection(*itemPointIter);
												}
											}
										}
									}
								}
							}
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

void DiagramReader::readItems(QList<DrawingItem*>& items)
{
	while (readNextStartElement())
	{
		if (name() == "jade-items")
		{
			while (readNextStartElement())
			{
				if (name() == "items") items.append(readItemElements());
				else skipCurrentElement();
			}
		}
		else skipCurrentElement();
	}
}

//==================================================================================================

QList<DrawingItem*> DiagramReader::readItemElements()
{
	QList<DrawingItem*> items;

	DrawingItem* newItem = nullptr;
	QString itemName;

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
		else if (itemName == "text-rect") newItem = readTextRectItem();
		else if (itemName == "text-ellipse") newItem = readTextEllipseItem();
		else if (itemName == "text-polygon") newItem = readTextPolygonItem();
		else if (itemName == "path") newItem = readPathItem();
		else if (itemName == "group") newItem = readItemGroup();
		else skipCurrentElement();

		if (newItem) items.append(newItem);
	}

	return items;
}

//==================================================================================================

DrawingLineItem* DiagramReader::readLineItem()
{
	DrawingLineItem* item = new DrawingLineItem();
	QXmlStreamAttributes attr = attributes();

	if (attr.hasAttribute("transform")) transformFromString(attr.value("transform").toString(), item);

	QLineF line = item->line();
	QPointF p1 = line.p1();
	QPointF p2 = line.p2();
	if (attr.hasAttribute("x1")) p1.setX(attr.value("x1").toDouble());
	if (attr.hasAttribute("y1")) p1.setY(attr.value("y1").toDouble());
	if (attr.hasAttribute("x2")) p2.setX(attr.value("x2").toDouble());
	if (attr.hasAttribute("y2")) p2.setY(attr.value("y2").toDouble());
	item->setLine(QLineF(p1, p2));

	readItemStyle(item->style());

	skipCurrentElement();

	return item;
}

DrawingArcItem* DiagramReader::readArcItem()
{
	DrawingArcItem* item = new DrawingArcItem();
	QXmlStreamAttributes attr = attributes();

	if (attr.hasAttribute("transform")) transformFromString(attr.value("transform").toString(), item);

	QLineF line = item->arc();
	QPointF p1 = line.p1();
	QPointF p2 = line.p2();
	if (attr.hasAttribute("x1")) p1.setX(attr.value("x1").toDouble());
	if (attr.hasAttribute("y1")) p1.setY(attr.value("y1").toDouble());
	if (attr.hasAttribute("x2")) p2.setX(attr.value("x2").toDouble());
	if (attr.hasAttribute("y2")) p2.setY(attr.value("y2").toDouble());
	item->setArc(QLineF(p1, p2));

	readItemStyle(item->style());

	skipCurrentElement();

	return item;
}

DrawingPolylineItem* DiagramReader::readPolylineItem()
{
	DrawingPolylineItem* item = new DrawingPolylineItem();
	QXmlStreamAttributes attr = attributes();

	if (attr.hasAttribute("transform")) transformFromString(attr.value("transform").toString(), item);

	if (attr.hasAttribute("points"))
		item->setPolyline(pointsFromString(attr.value("points").toString()));

	readItemStyle(item->style());

	skipCurrentElement();

	return item;
}

DrawingCurveItem* DiagramReader::readCurveItem()
{
	DrawingCurveItem* item = new DrawingCurveItem();
	QXmlStreamAttributes attr = attributes();

	if (attr.hasAttribute("transform")) transformFromString(attr.value("transform").toString(), item);

	QPointF p1 = item->curveStartPos(), p2 = item->curveEndPos();
	QPointF cp1 = item->curveStartControlPos(), cp2 = item->curveEndControlPos();
	if (attr.hasAttribute("x1")) p1.setX(attr.value("x1").toDouble());
	if (attr.hasAttribute("y1")) p1.setY(attr.value("y1").toDouble());
	if (attr.hasAttribute("cx1")) cp1.setX(attr.value("cx1").toDouble());
	if (attr.hasAttribute("cy1")) cp1.setY(attr.value("cy1").toDouble());
	if (attr.hasAttribute("cx2")) cp2.setX(attr.value("cx2").toDouble());
	if (attr.hasAttribute("cy2")) cp2.setY(attr.value("cy2").toDouble());
	if (attr.hasAttribute("x2")) p2.setX(attr.value("x2").toDouble());
	if (attr.hasAttribute("y2")) p2.setY(attr.value("y2").toDouble());
	item->setCurve(p1, cp1, cp2, p2);

	readItemStyle(item->style());

	skipCurrentElement();

	return item;
}

DrawingRectItem* DiagramReader::readRectItem()
{
	DrawingRectItem* item = new DrawingRectItem();
	QXmlStreamAttributes attr = attributes();

	if (attr.hasAttribute("transform")) transformFromString(attr.value("transform").toString(), item);

	QRectF rect = item->rect();
	if (attr.hasAttribute("left")) rect.setLeft(attr.value("left").toDouble());
	if (attr.hasAttribute("top")) rect.setTop(attr.value("top").toDouble());
	if (attr.hasAttribute("width")) rect.setWidth(attr.value("width").toDouble());
	if (attr.hasAttribute("height")) rect.setHeight(attr.value("height").toDouble());
	item->setRect(rect);

	if (attr.hasAttribute("rx")) item->setCornerRadii(attr.value("rx").toDouble(), item->cornerRadiusY());
	if (attr.hasAttribute("ry")) item->setCornerRadii(item->cornerRadiusX(), attr.value("ry").toDouble());

	readItemStyle(item->style());

	skipCurrentElement();

	return item;
}

DrawingEllipseItem* DiagramReader::readEllipseItem()
{
	DrawingEllipseItem* item = new DrawingEllipseItem();
	QXmlStreamAttributes attr = attributes();

	if (attr.hasAttribute("transform")) transformFromString(attr.value("transform").toString(), item);

	QRectF rect = item->ellipse();
	if (attr.hasAttribute("left")) rect.setLeft(attr.value("left").toDouble());
	if (attr.hasAttribute("top")) rect.setTop(attr.value("top").toDouble());
	if (attr.hasAttribute("width")) rect.setWidth(attr.value("width").toDouble());
	if (attr.hasAttribute("height")) rect.setHeight(attr.value("height").toDouble());
	item->setEllipse(rect);

	readItemStyle(item->style());

	skipCurrentElement();

	return item;
}

DrawingPolygonItem* DiagramReader::readPolygonItem()
{
	DrawingPolygonItem* item = new DrawingPolygonItem();
	QXmlStreamAttributes attr = attributes();

	if (attr.hasAttribute("transform")) transformFromString(attr.value("transform").toString(), item);

	if (attr.hasAttribute("points"))
		item->setPolygon(pointsFromString(attr.value("points").toString()));

	readItemStyle(item->style());

	skipCurrentElement();

	return item;
}

DrawingTextItem* DiagramReader::readTextItem()
{
	DrawingTextItem* item = new DrawingTextItem();
	QXmlStreamAttributes attr = attributes();

	if (attr.hasAttribute("transform")) transformFromString(attr.value("transform").toString(), item);

	readItemStyle(item->style());

	if (readNext() == QXmlStreamReader::Characters)
	{
		item->setCaption(text().toString());
		skipCurrentElement();
	}

	return item;
}

DrawingTextRectItem* DiagramReader::readTextRectItem()
{
	DrawingTextRectItem* item = new DrawingTextRectItem();
	QXmlStreamAttributes attr = attributes();

	if (attr.hasAttribute("transform")) transformFromString(attr.value("transform").toString(), item);

	QRectF rect = item->rect();
	if (attr.hasAttribute("left")) rect.setLeft(attr.value("left").toDouble());
	if (attr.hasAttribute("top")) rect.setTop(attr.value("top").toDouble());
	if (attr.hasAttribute("width")) rect.setWidth(attr.value("width").toDouble());
	if (attr.hasAttribute("height")) rect.setHeight(attr.value("height").toDouble());
	item->setRect(rect);

	if (attr.hasAttribute("rx")) item->setCornerRadii(attr.value("rx").toDouble(), item->cornerRadiusY());
	if (attr.hasAttribute("ry")) item->setCornerRadii(item->cornerRadiusX(), attr.value("ry").toDouble());

	readItemStyle(item->style());

	if (readNext() == QXmlStreamReader::Characters)
	{
		item->setCaption(text().toString());
		skipCurrentElement();
	}

	return item;
}

DrawingTextEllipseItem* DiagramReader::readTextEllipseItem()
{
	DrawingTextEllipseItem* item = new DrawingTextEllipseItem();
	QXmlStreamAttributes attr = attributes();

	if (attr.hasAttribute("transform")) transformFromString(attr.value("transform").toString(), item);

	QRectF rect = item->ellipse();
	if (attr.hasAttribute("left")) rect.setLeft(attr.value("left").toDouble());
	if (attr.hasAttribute("top")) rect.setTop(attr.value("top").toDouble());
	if (attr.hasAttribute("width")) rect.setWidth(attr.value("width").toDouble());
	if (attr.hasAttribute("height")) rect.setHeight(attr.value("height").toDouble());
	item->setEllipse(rect);

	readItemStyle(item->style());

	if (readNext() == QXmlStreamReader::Characters)
	{
		item->setCaption(text().toString());
		skipCurrentElement();
	}

	return item;
}

DrawingTextPolygonItem* DiagramReader::readTextPolygonItem()
{
	DrawingTextPolygonItem* item = new DrawingTextPolygonItem();
	QXmlStreamAttributes attr = attributes();

	if (attr.hasAttribute("transform")) transformFromString(attr.value("transform").toString(), item);

	if (attr.hasAttribute("points"))
		item->setPolygon(pointsFromString(attr.value("points").toString()));

	readItemStyle(item->style());

	if (readNext() == QXmlStreamReader::Characters)
	{
		item->setCaption(text().toString());
		skipCurrentElement();
	}

	return item;
}

DrawingPathItem* DiagramReader::readPathItem()
{
	DrawingPathItem* item = new DrawingPathItem();
	QXmlStreamAttributes attr = attributes();

	if (attr.hasAttribute("name")) item->setName(attr.value("name").toString());

	if (attr.hasAttribute("transform")) transformFromString(attr.value("transform").toString(), item);

	QRectF pathRect = item->pathRect();
	if (attr.hasAttribute("view-left")) pathRect.setLeft(attr.value("view-left").toDouble());
	if (attr.hasAttribute("view-top")) pathRect.setTop(attr.value("view-top").toDouble());
	if (attr.hasAttribute("view-width")) pathRect.setWidth(attr.value("view-width").toDouble());
	if (attr.hasAttribute("view-height")) pathRect.setHeight(attr.value("view-height").toDouble());
	item->setPath(item->path(), pathRect);

	if (attr.hasAttribute("d"))
		item->setPath(pathFromString(attr.value("d").toString()), pathRect);

	QRectF rect = item->rect();
	if (attr.hasAttribute("left")) rect.setLeft(attr.value("left").toDouble());
	if (attr.hasAttribute("top")) rect.setTop(attr.value("top").toDouble());
	if (attr.hasAttribute("width")) rect.setWidth(attr.value("width").toDouble());
	if (attr.hasAttribute("height")) rect.setHeight(attr.value("height").toDouble());
	item->setRect(rect);

	if (attr.hasAttribute("glue-points"))
		item->addConnectionPoints(pointsFromString(attr.value("glue-points").toString()));

	readItemStyle(item->style());

	skipCurrentElement();

	return item;
}

DrawingItemGroup* DiagramReader::readItemGroup()
{
	DrawingItemGroup* item = new DrawingItemGroup();
	QXmlStreamAttributes attr = attributes();

	if (attr.hasAttribute("transform")) transformFromString(attr.value("transform").toString(), item);

	item->setItems(readItemElements());

	skipCurrentElement();

	return item;
}

//==================================================================================================

void DiagramReader::readItemStyle(DrawingItemStyle* style)
{
	QXmlStreamAttributes attr = attributes();

	// Pen
	if (style->hasValue(DrawingItemStyle::PenStyle))
	{
		Qt::PenStyle penStyle = (attr.hasAttribute("stroke-style")) ?
			penStyleFromString(attr.value("stroke-style").toString()) :	Qt::SolidLine;
		style->setValue(DrawingItemStyle::PenStyle, (uint)penStyle);
	}

	if (style->hasValue(DrawingItemStyle::PenWidth))
	{
		qreal penWidth = (attr.hasAttribute("stroke-width")) ? attr.value("stroke-width").toDouble() : 1.0;
		style->setValue(DrawingItemStyle::PenWidth, penWidth);
	}

	if (style->hasValue(DrawingItemStyle::PenColor))
	{
		QColor color = (attr.hasAttribute("stroke-color")) ?
			colorFromString(attr.value("stroke-color").toString()) : QColor(0, 0, 0);
		style->setValue(DrawingItemStyle::PenColor, color);
	}

	if (style->hasValue(DrawingItemStyle::PenOpacity))
	{
		qreal opacity = (attr.hasAttribute("stroke-opacity")) ?	attr.value("stroke-opacity").toDouble() : 1.0;
		style->setValue(DrawingItemStyle::PenOpacity, opacity);
	}

	// Brush
	if (style->hasValue(DrawingItemStyle::BrushColor))
	{
		QColor color = (attr.hasAttribute("fill-color")) ?
			colorFromString(attr.value("fill-color").toString()) : QColor(255, 255, 255);
		style->setValue(DrawingItemStyle::BrushColor, color);
	}

	if (style->hasValue(DrawingItemStyle::BrushOpacity))
	{
		qreal opacity = (attr.hasAttribute("fill-opacity")) ? attr.value("fill-opacity").toDouble() : 1.0;
		style->setValue(DrawingItemStyle::BrushOpacity, opacity);
	}

	// Font
	if (style->hasValue(DrawingItemStyle::FontName))
	{
		QString name = (attr.hasAttribute("font-name")) ? attr.value("font-name").toString() : "Arial";
		style->setValue(DrawingItemStyle::FontName, name);
	}

	if (style->hasValue(DrawingItemStyle::FontSize))
	{
		qreal size = (attr.hasAttribute("font-size")) ? attr.value("font-size").toDouble() : 0.0;
		style->setValue(DrawingItemStyle::FontSize, size);
	}

	if (style->hasValue(DrawingItemStyle::FontBold))
	{
		bool fontStyle = (attr.hasAttribute("font-bold")) ?
			(attr.value("font-bold").toString().toLower() == "true") : false;
		style->setValue(DrawingItemStyle::FontBold, fontStyle);
	}

	if (style->hasValue(DrawingItemStyle::FontItalic))
	{
		bool fontStyle = (attr.hasAttribute("font-italic")) ?
			(attr.value("font-italic").toString().toLower() == "true") : false;
		style->setValue(DrawingItemStyle::FontItalic, fontStyle);
	}

	if (style->hasValue(DrawingItemStyle::FontUnderline))
	{
		bool fontStyle = (attr.hasAttribute("font-underline")) ?
			(attr.value("font-underline").toString().toLower() == "true") : false;
		style->setValue(DrawingItemStyle::FontUnderline, fontStyle);
	}

	if (style->hasValue(DrawingItemStyle::FontStrikeThrough))
	{
		bool fontStyle = (attr.hasAttribute("font-strike-through")) ?
			(attr.value("font-strike-through").toString().toLower() == "true") : false;
		style->setValue(DrawingItemStyle::FontStrikeThrough, fontStyle);
	}

	if (style->hasValue(DrawingItemStyle::TextHorizontalAlignment))
	{
		Qt::Alignment textAlign = (attr.hasAttribute("text-alignment-horizontal")) ?
			alignmentFromString(attr.value("text-alignment-horizontal").toString()) : Qt::AlignHCenter;
		style->setValue(DrawingItemStyle::TextHorizontalAlignment, (uint)textAlign);
	}

	if (style->hasValue(DrawingItemStyle::TextVerticalAlignment))
	{
		Qt::Alignment textAlign = (attr.hasAttribute("text-alignment-vertical")) ?
			alignmentFromString(attr.value("text-alignment-vertical").toString()) : Qt::AlignVCenter;
		style->setValue(DrawingItemStyle::TextVerticalAlignment, (uint)textAlign);
	}

	if (style->hasValue(DrawingItemStyle::TextColor))
	{
		QColor color = (attr.hasAttribute("text-color")) ?
			colorFromString(attr.value("text-color").toString()) : QColor(0, 0, 0);
		style->setValue(DrawingItemStyle::TextColor, color);
	}

	if (style->hasValue(DrawingItemStyle::TextOpacity))
	{
		qreal opacity = (attr.hasAttribute("text-opacity")) ? attr.value("text-opacity").toDouble() : 1.0;
		style->setValue(DrawingItemStyle::TextOpacity, opacity);
	}

	// Arrows
	if (style->hasValue(DrawingItemStyle::StartArrowStyle))
	{
		DrawingItemStyle::ArrowStyle arrow = (attr.hasAttribute("arrow-start-style")) ?
			arrowStyleFromString(attr.value("arrow-start-style").toString()) : DrawingItemStyle::ArrowNone;
		style->setValue(DrawingItemStyle::StartArrowStyle, (uint)arrow);
	}

	if (style->hasValue(DrawingItemStyle::StartArrowSize))
	{
		qreal size = (attr.hasAttribute("arrow-start-size")) ? attr.value("arrow-start-size").toDouble() : 0.0;
		style->setValue(DrawingItemStyle::StartArrowSize, size);
	}

	if (style->hasValue(DrawingItemStyle::EndArrowStyle))
	{
		DrawingItemStyle::ArrowStyle arrow = (attr.hasAttribute("arrow-end-style")) ?
			arrowStyleFromString(attr.value("arrow-end-style").toString()) : DrawingItemStyle::ArrowNone;
		style->setValue(DrawingItemStyle::EndArrowStyle, (uint)arrow);
	}

	if (style->hasValue(DrawingItemStyle::EndArrowSize))
	{
		qreal size = (attr.hasAttribute("arrow-end-size")) ? attr.value("arrow-end-size").toDouble() : 0.0;
		style->setValue(DrawingItemStyle::EndArrowSize, size);
	}
}

//==================================================================================================

Qt::Alignment DiagramReader::alignmentFromString(const QString& str) const
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

DrawingItemStyle::ArrowStyle DiagramReader::arrowStyleFromString(const QString& str) const
{
	DrawingItemStyle::ArrowStyle style = DrawingItemStyle::ArrowNone;

	if (str == "normal") style = DrawingItemStyle::ArrowNormal;
	else if (str == "triangle") style = DrawingItemStyle::ArrowTriangle;
	else if (str == "triangle-filled") style = DrawingItemStyle::ArrowTriangleFilled;
	else if (str == "circle") style = DrawingItemStyle::ArrowCircle;
	else if (str == "circle-filled") style = DrawingItemStyle::ArrowCircleFilled;
	else if (str == "diamond") style = DrawingItemStyle::ArrowDiamond;
	else if (str == "diamond-filled") style = DrawingItemStyle::ArrowDiamondFilled;
	else if (str == "harpoon") style = DrawingItemStyle::ArrowHarpoon;
	else if (str == "harpoon-mirrored") style = DrawingItemStyle::ArrowHarpoonMirrored;
	else if (str == "concave") style = DrawingItemStyle::ArrowConcave;
	else if (str == "concave-filled") style = DrawingItemStyle::ArrowConcaveFilled;
	else if (str == "reverse") style = DrawingItemStyle::ArrowReverse;
	else if (str == "X") style = DrawingItemStyle::ArrowX;

	return style;
}

QColor DiagramReader::colorFromString(const QString& str) const
{
	QColor color;

	color.setRed(str.mid(1,2).toUInt(nullptr, 16));
	color.setGreen(str.mid(3,2).toUInt(nullptr, 16));
	color.setBlue(str.mid(5,2).toUInt(nullptr, 16));

	return color;
}

DiagramWidget::GridRenderStyle DiagramReader::gridStyleFromString(const QString& str) const
{
	DiagramWidget::GridRenderStyle style = DiagramWidget::GridNone;

	if (str == "dotted") style = DiagramWidget::GridDots;
	else if (str == "lined") style = DiagramWidget::GridLines;
	else if (str == "graph-paper") style = DiagramWidget::GridGraphPaper;

	return style;
}

QPainterPath DiagramReader::pathFromString(const QString& str) const
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

Qt::PenStyle DiagramReader::penStyleFromString(const QString& str) const
{
	Qt::PenStyle style = Qt::SolidLine;

	if (str == "none") style = Qt::NoPen;
	else if (str == "dash") style = Qt::DashLine;
	else if (str == "dot") style = Qt::DotLine;
	else if (str == "dash-dot") style = Qt::DashDotLine;
	else if (str == "dash-dot-dot") style = Qt::DashDotDotLine;

	return style;
}

Qt::PenCapStyle DiagramReader::penCapStyleFromString(const QString& str) const
{
	Qt::PenCapStyle style = Qt::RoundCap;

	if (str == "flat") style = Qt::FlatCap;
	else if (str == "square") style = Qt::SquareCap;

	return style;
}

Qt::PenJoinStyle DiagramReader::penJoinStyleFromString(const QString& str) const
{
	Qt::PenJoinStyle style = Qt::RoundJoin;

	if (str == "miter") style = Qt::MiterJoin;
	else if (str == "bevel") style = Qt::BevelJoin;

	return style;
}

QPolygonF DiagramReader::pointsFromString(const QString& str) const
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

void DiagramReader::transformFromString(const QString& str, DrawingItem* item)
{
	QStringList tokens = str.split(QRegExp("\\s+"));
	for(auto tokenIter = tokens.begin(); tokenIter != tokens.end(); tokenIter++)
	{
		if (tokenIter->startsWith("translate("))
		{
			int endIndex = tokenIter->indexOf(")");
			QStringList coords = tokenIter->mid(10, endIndex - 10).split(",");
			if (coords.size() == 2)
			{
				item->setX(coords.first().toDouble());
				item->setY(coords.last().toDouble());
			}
		}
		else if (tokenIter->startsWith("rotate("))
		{
			int endIndex = tokenIter->indexOf(")");
			QString angle = tokenIter->mid(7, endIndex - 7);
			item->setRotation(angle.toDouble());
		}
		else if (tokenIter->startsWith("scale("))
		{
			int endIndex = tokenIter->indexOf(")");
			QStringList coords = tokenIter->mid(6, endIndex - 6).split(",");
			if (coords.size() == 2) item->setFlipped(coords.first().toDouble() < 0);
		}
	}
}
