// DrawingUndo.cpp
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

#include "DrawingUndo.h"
#include "DrawingWidget.h"

DrawingUndoCommand::DrawingUndoCommand(const QString& title, QUndoCommand* parent) :
	QUndoCommand(title, parent) { }

DrawingUndoCommand::DrawingUndoCommand(const DrawingUndoCommand& command, QUndoCommand* parent) :
	QUndoCommand(command.text(), parent)
{
	QList<QUndoCommand*> otherChildren;

	for(int i = 0; i < command.childCount(); i++)
		otherChildren.append(const_cast<QUndoCommand*>(command.child(i)));

	/*for(auto otherChildIter = otherChildren.begin();
		otherChildIter != otherChildren.end(); otherChildIter++)
	{
		switch ((*otherChildIter)->id())
		{
		case ResizeItemType:
			new DrawingResizeItemCommand(
				*static_cast<DrawingResizeItemCommand*>(*otherChildIter), this);
			break;
		case PointConnectType:
			new DrawingItemPointConnectCommand(
				*static_cast<DrawingItemPointConnectCommand*>(*otherChildIter), this);
			break;
		case PointDisconnectType:
			new DrawingItemPointDisconnectCommand(
				*static_cast<DrawingItemPointDisconnectCommand*>(*otherChildIter), this);
			break;
		default:
			break;
		}
	}*/
}

DrawingUndoCommand::~DrawingUndoCommand() { }

//==================================================================================================

void DrawingUndoCommand::mergeChildren(const QUndoCommand* command)
{
	bool mergeSuccess;
	QList<QUndoCommand*> children, otherChildren;

	for(int i = 0; i < childCount(); i++)
		children.append(const_cast<QUndoCommand*>(child(i)));
	for(int i = 0; i < command->childCount(); i++)
		otherChildren.append(const_cast<QUndoCommand*>(command->child(i)));

	for(auto otherChildIter = otherChildren.begin();
		otherChildIter != otherChildren.end(); otherChildIter++)
	{
		mergeSuccess = false;
		for(auto childIter = children.begin(); childIter != children.end(); childIter++)
			mergeSuccess = ((*childIter)->mergeWith(*otherChildIter) && mergeSuccess);

		if (!mergeSuccess)
		{
			/*switch ((*otherChildIter)->id())
			{
			case ResizeItemType:
				new DrawingResizeItemCommand(
					*static_cast<DrawingResizeItemCommand*>(*otherChildIter), this);
				break;
			case PointConnectType:
				new DrawingItemPointConnectCommand(
					*static_cast<DrawingItemPointConnectCommand*>(*otherChildIter), this);
				break;
			case PointDisconnectType:
				new DrawingItemPointDisconnectCommand(
					*static_cast<DrawingItemPointDisconnectCommand*>(*otherChildIter), this);
				break;
			default:
				break;
			}*/
		}
	}
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingSetPropertiesCommand::DrawingSetPropertiesCommand(DrawingWidget* drawing,
	const QHash<QString,QVariant>& properties, QUndoCommand* parent) : DrawingUndoCommand("Set Properties", parent)
{
	mDrawing = drawing;
	mProperties = properties;
	if (mDrawing) mOriginalProperties = mDrawing->properties();
}

DrawingSetPropertiesCommand::~DrawingSetPropertiesCommand() { }

//==================================================================================================

int DrawingSetPropertiesCommand::id() const
{
	return SetDrawingPropertiesType;
}

//==================================================================================================

void DrawingSetPropertiesCommand::redo()
{
	if (mDrawing) mDrawing->setProperties(mProperties);
	QUndoCommand::redo();
}

void DrawingSetPropertiesCommand::undo()
{
	QUndoCommand::undo();
	if (mDrawing) mDrawing->setProperties(mOriginalProperties);
}
