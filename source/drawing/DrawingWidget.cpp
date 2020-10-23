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

void DrawingWidget::addActions()
{
	addAction("Undo", this, SLOT(undo()), "", "Ctrl+Z");
	addAction("Redo", this, SLOT(redo()), "", "Ctrl+Shift+Z");
}
