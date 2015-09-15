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
#include "DrawingWidget.h"

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

		// write items
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

}

//==================================================================================================

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
