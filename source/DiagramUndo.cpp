/* DiagramUndo.cpp
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

#include "DiagramUndo.h"

DiagramSetItemsStyleCommand::DiagramSetItemsStyleCommand(DiagramWidget* diagram,
	const QList<DrawingItem*>& items, const QHash<DrawingItemStyle::Property,QVariant>& properties)
	: QUndoCommand("Set Item Properties")
{
	mDiagram = diagram;

	QList<DrawingItemStyle::Property> styleProperties = properties.keys();
	QHash<DrawingItemStyle::Property,QVariant> originalProperties;

	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
	{
		mProperties[*itemIter] = properties;

		originalProperties.clear();
		for (auto propIter = styleProperties.begin(); propIter != styleProperties.end(); propIter++)
		{
			if ((*itemIter)->style()->hasValue(*propIter))
				originalProperties[*propIter] = (*itemIter)->style()->value(*propIter);
		}

		mOriginalProperties[*itemIter] = originalProperties;
	}
}

DiagramSetItemsStyleCommand::~DiagramSetItemsStyleCommand() { }

int DiagramSetItemsStyleCommand::id() const
{
	return DiagramSetItemsStyleCommandId;
}

void DiagramSetItemsStyleCommand::redo()
{
	if (mDiagram) mDiagram->setItemsStyle(mProperties);
	QUndoCommand::redo();
}

void DiagramSetItemsStyleCommand::undo()
{
	QUndoCommand::undo();
	if (mDiagram) mDiagram->setItemsStyle(mOriginalProperties);
}

//==================================================================================================

DiagramSetItemCornerRadiusCommand::DiagramSetItemCornerRadiusCommand(DiagramWidget* diagram,
	DrawingItem* item, qreal radiusX, qreal radiusY) : QUndoCommand("Set Item Corner Radius")
{
	mDiagram = diagram;
	mItem = item;
	mCornerRadiusX = radiusX;
	mCornerRadiusY = radiusY;
	mOriginalCornerRadiusX = 0;
	mOriginalCornerRadiusY = 0;

	DrawingRectItem* rectItem = dynamic_cast<DrawingRectItem*>(item);
	DrawingTextRectItem* textRectItem = dynamic_cast<DrawingTextRectItem*>(item);

	if (rectItem)
	{
		mOriginalCornerRadiusX = rectItem->cornerRadiusX();
		mOriginalCornerRadiusY = rectItem->cornerRadiusY();
	}
	if (textRectItem)
	{
		mOriginalCornerRadiusX = textRectItem->cornerRadiusX();
		mOriginalCornerRadiusY = textRectItem->cornerRadiusY();
	}
}

DiagramSetItemCornerRadiusCommand::~DiagramSetItemCornerRadiusCommand() { }

int DiagramSetItemCornerRadiusCommand::id() const
{
	return DiagramSetItemCornerRadiusCommandId;
}

void DiagramSetItemCornerRadiusCommand::redo()
{
	if (mDiagram) mDiagram->setItemCornerRadius(mItem, mCornerRadiusX, mCornerRadiusY);
	QUndoCommand::redo();
}

void DiagramSetItemCornerRadiusCommand::undo()
{
	QUndoCommand::undo();
	if (mDiagram) mDiagram->setItemCornerRadius(mItem, mOriginalCornerRadiusX, mOriginalCornerRadiusY);
}

//==================================================================================================

DiagramSetItemCaptionCommand::DiagramSetItemCaptionCommand(DiagramWidget* diagram,
	DrawingItem* item, const QString& caption) : QUndoCommand("Set Item Caption")
{
	mDiagram = diagram;
	mItem = item;
	mCaption = caption;

	DrawingTextItem* textItem = dynamic_cast<DrawingTextItem*>(item);
	DrawingTextRectItem* textRectItem = dynamic_cast<DrawingTextRectItem*>(item);
	DrawingTextEllipseItem* textEllipseItem = dynamic_cast<DrawingTextEllipseItem*>(item);
	DrawingTextPolygonItem* textPolygonItem = dynamic_cast<DrawingTextPolygonItem*>(item);

	if (textItem) mOriginalCaption = textItem->caption();
	if (textRectItem) mOriginalCaption = textRectItem->caption();
	if (textEllipseItem) mOriginalCaption = textEllipseItem->caption();
	if (textPolygonItem) mOriginalCaption = textPolygonItem->caption();
}

DiagramSetItemCaptionCommand::~DiagramSetItemCaptionCommand() { }

int DiagramSetItemCaptionCommand::id() const
{
	return DiagramSetItemCaptionCommandId;
}

void DiagramSetItemCaptionCommand::redo()
{
	if (mDiagram) mDiagram->setItemCaption(mItem, mCaption);
	QUndoCommand::redo();
}

void DiagramSetItemCaptionCommand::undo()
{
	QUndoCommand::undo();
	if (mDiagram) mDiagram->setItemCaption(mItem, mOriginalCaption);
}

bool DiagramSetItemCaptionCommand::mergeWith(const QUndoCommand* command)
{
	bool mergeSuccess = false;

	if (command && command->id() == DiagramSetItemCaptionCommandId)
	{
		const DiagramSetItemCaptionCommand* captionCommand =
			static_cast<const DiagramSetItemCaptionCommand*>(command);

		if (captionCommand && mDiagram == captionCommand->mDiagram &&
			mItem == captionCommand->mItem)
		{
			mCaption = captionCommand->mCaption;
			mergeSuccess = true;
		}
	}

	return mergeSuccess;
}

//==================================================================================================

DiagramSetPropertiesCommand::DiagramSetPropertiesCommand(DiagramWidget* diagram,
	const QHash<DiagramWidget::Property,QVariant>& properties) : QUndoCommand("Set Diagram Properties")
{
	mDiagram = diagram;
	mProperties = properties;

	if (mDiagram)
	{
		DrawingScene* scene = mDiagram->scene();

		if (scene && properties.contains(DiagramWidget::SceneRect))
			mOriginalProperties[DiagramWidget::SceneRect] = scene->sceneRect();
		if (scene && properties.contains(DiagramWidget::BackgroundColor))
			mOriginalProperties[DiagramWidget::BackgroundColor] = scene->backgroundBrush().color();
		if (properties.contains(DiagramWidget::Grid))
			mOriginalProperties[DiagramWidget::Grid] = mDiagram->grid();
		if (properties.contains(DiagramWidget::GridStyle))
			mOriginalProperties[DiagramWidget::GridStyle] = (quint32)(mDiagram->gridStyle());
		if (properties.contains(DiagramWidget::GridColor))
			mOriginalProperties[DiagramWidget::GridColor] = mDiagram->gridBrush().color();
		if (properties.contains(DiagramWidget::GridSpacingMajor))
			mOriginalProperties[DiagramWidget::GridSpacingMajor] = mDiagram->gridSpacingMajor();
		if (properties.contains(DiagramWidget::GridSpacingMinor))
			mOriginalProperties[DiagramWidget::GridSpacingMinor] = mDiagram->gridSpacingMinor();
	}
}

DiagramSetPropertiesCommand::~DiagramSetPropertiesCommand() { }

int DiagramSetPropertiesCommand::id() const
{
	return DiagramSetPropertiesCommandId;
}

void DiagramSetPropertiesCommand::redo()
{
	if (mDiagram) mDiagram->setViewProperties(mProperties);
	QUndoCommand::redo();
}

void DiagramSetPropertiesCommand::undo()
{
	QUndoCommand::undo();
	if (mDiagram) mDiagram->setViewProperties(mOriginalProperties);
}
