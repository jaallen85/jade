// DrawingWidget.cpp
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

#include "DrawingWidget.h"

DrawingWidget::DrawingWidget(QWidget* parent) : DrawingCanvas(parent)
{

}

DrawingWidget::~DrawingWidget()
{

}

//==================================================================================================

void DrawingWidget::setProperties(const QHash<QString,QVariant>& properties)
{
	if (properties.contains("sceneRect"))
	{
		QVariant variant = properties.value("sceneRect");
		if (variant.canConvert<QRectF>()) setSceneRect(variant.value<QRectF>());
	}

	if (properties.contains("backgroundBrush"))
	{
		QVariant variant = properties.value("backgroundBrush");
		if (variant.canConvert<QBrush>()) setBackgroundBrush(variant.value<QBrush>());
	}

	if (properties.contains("grid"))
	{
		QVariant variant = properties.value("grid");
		if (variant.canConvert<qreal>()) setGrid(variant.value<qreal>());
	}

	if (properties.contains("gridStyle"))
	{
		QVariant variant = properties.value("gridStyle");
		if (variant.canConvert<uint>()) setGridStyle((Drawing::GridStyle)variant.value<uint>());
	}

	if (properties.contains("gridBrush"))
	{
		QVariant variant = properties.value("gridBrush");
		if (variant.canConvert<QBrush>()) setGridBrush(variant.value<QBrush>());
	}

	if (properties.contains("gridSpacingMajor"))
	{
		QVariant variant = properties.value("gridSpacingMajor");
		if (variant.canConvert<int>()) setGridSpacing(variant.value<int>(), gridSpacingMinor());
	}

	if (properties.contains("gridSpacingMinor"))
	{
		QVariant variant = properties.value("gridSpacingMinor");
		if (variant.canConvert<int>()) setGridSpacing(gridSpacingMajor(), variant.value<int>());
	}
}

QHash<QString,QVariant> DrawingWidget::properties() const
{
	QHash<QString,QVariant> properties;

	properties.insert("sceneRect", sceneRect());
	properties.insert("backgroundBrush", backgroundBrush());

	properties.insert("grid", grid());
	properties.insert("gridStyle", (uint)gridStyle());
	properties.insert("gridBrush", gridBrush());
	properties.insert("gridSpacingMajor", gridSpacingMajor());
	properties.insert("gridSpacingMinor", gridSpacingMinor());

	return properties;
}

//==================================================================================================

void DrawingWidget::updateProperties(const QHash<QString,QVariant>& properties)
{
	setProperties(properties);
	if (properties.contains("sceneRect")) zoomFit();
	else viewport()->update();
}
