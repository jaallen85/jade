/* DiagramUndo.cpp
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

#include "DiagramUndo.h"

DiagramSetItemsStyleCommand::DiagramSetItemsStyleCommand(DiagramWidget* diagram,
	const QList<DrawingItem*>& items, const QHash<DrawingItemStyle::Property,QVariant>& properties)
	: QUndoCommand("Set Item Properties")
{

}

DiagramSetItemsStyleCommand::~DiagramSetItemsStyleCommand() { }

int DiagramSetItemsStyleCommand::id() const
{
	return DiagramSetItemsStyleCommandId;
}

void DiagramSetItemsStyleCommand::redo()
{
	if (mDiagram) mDiagram->setItemsStyle(mItems, mProperties);
	QUndoCommand::redo();
}

void DiagramSetItemsStyleCommand::undo()
{

}

//==================================================================================================

DiagramSetItemCornerRadiusCommand::DiagramSetItemCornerRadiusCommand(DiagramWidget* diagram,
	DrawingItem* item, qreal radiusX, qreal radiusY) : QUndoCommand("Set Item Corner Radius")
{

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

}

DiagramSetPropertiesCommand::~DiagramSetPropertiesCommand() { }

int DiagramSetPropertiesCommand::id() const
{
	return DiagramSetPropertiesCommandId;
}

void DiagramSetPropertiesCommand::redo()
{
	if (mDiagram) mDiagram->setDiagramProperties(mProperties);
	QUndoCommand::redo();
}

void DiagramSetPropertiesCommand::undo()
{
	QUndoCommand::undo();
	if (mDiagram) mDiagram->setDiagramProperties(mOriginalProperties);
}
