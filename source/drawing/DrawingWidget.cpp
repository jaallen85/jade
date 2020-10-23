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
#include "DrawingUndo.h"

DrawingWidget::DrawingWidget(QWidget* parent) : DrawingCanvas(parent)
{
	mUndoStack.setUndoLimit(64);
	connect(&mUndoStack, SIGNAL(cleanChanged(bool)), this, SIGNAL(cleanChanged(bool)));
	connect(&mUndoStack, SIGNAL(canRedoChanged(bool)), this, SIGNAL(canRedoChanged(bool)));
	connect(&mUndoStack, SIGNAL(canUndoChanged(bool)), this, SIGNAL(canUndoChanged(bool)));

	addActions();
}

DrawingWidget::~DrawingWidget()
{

}

//==================================================================================================

void DrawingWidget::setUndoLimit(int undoLimit)
{
	mUndoStack.setUndoLimit(undoLimit);
}

void DrawingWidget::pushUndoCommand(QUndoCommand* command)
{
	mUndoStack.push(command);
}

int DrawingWidget::undoLimit() const
{
	return mUndoStack.undoLimit();
}

bool DrawingWidget::isClean() const
{
	return mUndoStack.isClean();
}

bool DrawingWidget::canUndo() const
{
	return mUndoStack.canUndo();
}

bool DrawingWidget::canRedo() const
{
	return mUndoStack.canRedo();
}

QString DrawingWidget::undoText() const
{
	return mUndoStack.undoText();
}

QString DrawingWidget::redoText() const
{
	return mUndoStack.redoText();
}

//==================================================================================================

void DrawingWidget::setProperties(const QHash<QString,QVariant>& properties)
{
	if (properties.contains("sceneRect"))
	{
		QVariant variant = properties.value("sceneRect");
		if (variant.canConvert<QRectF>())
		{
			setSceneRect(variant.value<QRectF>());
			zoomFit();
		}
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

	if (!properties.isEmpty()) emit propertiesChanged(properties);
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

void DrawingWidget::writeToXml(QXmlStreamWriter* xml)
{
	if (xml)
	{
		QRectF sceneRect = this->sceneRect();
		xml->writeAttribute("view-left", QString::number(sceneRect.left()));
		xml->writeAttribute("view-top", QString::number(sceneRect.top()));
		xml->writeAttribute("view-width", QString::number(sceneRect.width()));
		xml->writeAttribute("view-height", QString::number(sceneRect.height()));

		writeBrushToXml(xml, "background", backgroundBrush());

		xml->writeAttribute("grid", QString::number(grid()));

		writeBrushToXml(xml, "grid", gridBrush());

		QString gridStyleStr;
		switch (gridStyle())
		{
		case Drawing::GridDots: gridStyleStr = "dotted"; break;
		case Drawing::GridLines: gridStyleStr = "lined"; break;
		case Drawing::GridGraphPaper: gridStyleStr = "graph-paper"; break;
		default: break;
		}
		xml->writeAttribute("grid-style", gridStyleStr);

		xml->writeAttribute("grid-spacing-major", QString::number(gridSpacingMajor()));
		xml->writeAttribute("grid-spacing-minor", QString::number(gridSpacingMinor()));
	}
}

void DrawingWidget::readFromXml(QXmlStreamReader* xml)
{
	if (xml)
	{
		QXmlStreamAttributes attr = xml->attributes();

		QRectF sceneRect;
		if (attr.hasAttribute("view-left"))
			sceneRect.setLeft(attr.value("view-left").toDouble());
		if (attr.hasAttribute("view-top"))
			sceneRect.setTop(attr.value("view-top").toDouble());
		if (attr.hasAttribute("view-width"))
			sceneRect.setWidth(attr.value("view-width").toDouble());
		if (attr.hasAttribute("view-height"))
			sceneRect.setHeight(attr.value("view-height").toDouble());
		setSceneRect(sceneRect);

		QBrush backgroundBrush = Qt::white;
		readBrushFromXml(xml, "background", backgroundBrush);
		setBackgroundBrush(backgroundBrush);

		qreal grid = 0;
		if (attr.hasAttribute("grid")) grid = attr.value("grid").toDouble();
		setGrid(grid);

		QBrush gridBrush = QColor(0, 128, 128);
		readBrushFromXml(xml, "grid", gridBrush);
		setGridBrush(gridBrush);

		Drawing::GridStyle gridStyle = Drawing::GridNone;
		if (attr.hasAttribute("grid-style"))
		{
			QString gridStyleStr = attr.value("grid-style").toString().toLower();

			if (gridStyleStr == "dotted") gridStyle = Drawing::GridDots;
			else if (gridStyleStr == "lined") gridStyle = Drawing::GridLines;
			else if (gridStyleStr == "graph-paper") gridStyle = Drawing::GridGraphPaper;
		}
		setGridStyle(gridStyle);

		int gridSpacingMajor = 0, gridSpacingMinor = 0;
		if (attr.hasAttribute("grid-spacing-major"))
			gridSpacingMajor = attr.value("grid-spacing-major").toInt();
		if (attr.hasAttribute("grid-spacing-minor"))
			gridSpacingMinor = attr.value("grid-spacing-minor").toInt();
		setGridSpacing(gridSpacingMajor, gridSpacingMinor);
	}
}

//==================================================================================================

void DrawingWidget::undo()
{
	if (mode() == Drawing::DefaultMode && mUndoStack.canUndo())
	{
		mUndoStack.undo();
		viewport()->update();
	}
}

void DrawingWidget::redo()
{
	if (mode() == Drawing::DefaultMode && mUndoStack.canRedo())
	{
		mUndoStack.redo();
		viewport()->update();
	}
}

void DrawingWidget::setClean()
{
	mUndoStack.setClean();
	viewport()->update();
}

//==================================================================================================

void DrawingWidget::updateProperties(const QHash<QString,QVariant>& properties)
{
	bool propertyChanged = false;

	QHash<QString,QVariant> originalProperties = this->properties();
	for(auto propIter = properties.begin(); !propertyChanged && propIter != properties.end(); propIter++)
	{
		if (originalProperties.contains(propIter.key()))
			propertyChanged = (propIter.value() != originalProperties.value(propIter.key()));
	}

	if (propertyChanged)
	{
		pushUndoCommand(new DrawingSetPropertiesCommand(this, properties));
		viewport()->update();
	}
}

//==================================================================================================

void DrawingWidget::writeBrushToXml(QXmlStreamWriter* xml, const QString& name, const QBrush& brush)
{
	if (xml) xml->writeAttribute(name + "-color", colorToString(brush.color()));
}

QString DrawingWidget::colorToString(const QColor& color) const
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

void DrawingWidget::readBrushFromXml(QXmlStreamReader* xml, const QString& name, QBrush& brush)
{
	if (xml)
	{
		QXmlStreamAttributes attr = xml->attributes();

		if (attr.hasAttribute(name + "-color"))
			brush = colorFromString(attr.value(name + "-color").toString());
	}
}

QColor DrawingWidget::colorFromString(const QString& str) const
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

//==================================================================================================

void DrawingWidget::addActions()
{
	addAction("Undo", this, SLOT(undo()), "", "Ctrl+Z");
	addAction("Redo", this, SLOT(redo()), "", "Ctrl+Shift+Z");
}
