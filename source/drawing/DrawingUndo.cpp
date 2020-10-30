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
#include "DrawingItem.h"
#include "DrawingItemPoint.h"

DrawingUndoCommand::DrawingUndoCommand(const QString& title, QUndoCommand* parent) :
	QUndoCommand(title, parent) { }

DrawingUndoCommand::DrawingUndoCommand(const DrawingUndoCommand& command, QUndoCommand* parent) :
	QUndoCommand(command.text(), parent)
{
	QList<QUndoCommand*> otherChildren;

	for(int i = 0; i < command.childCount(); i++)
		otherChildren.append(const_cast<QUndoCommand*>(command.child(i)));

	for(auto otherChildIter = otherChildren.begin();
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
	}
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
		}
	}
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingAddItemsCommand::DrawingAddItemsCommand(DrawingWidget* widget,
	const QList<DrawingItem*>& items, QUndoCommand* parent) :
	DrawingUndoCommand("Add Items", parent)
{
	mWidget = widget;
	mItems = items;
	mUndone = true;
}

DrawingAddItemsCommand::~DrawingAddItemsCommand()
{
	if (mUndone)
	{
		while (!mItems.isEmpty()) delete mItems.takeFirst();
	}
}

//==================================================================================================

int DrawingAddItemsCommand::id() const
{
	return AddItemsType;
}

//==================================================================================================

void DrawingAddItemsCommand::redo()
{
	mUndone = false;
	if (mWidget) mWidget->addItems(mItems);
	DrawingUndoCommand::redo();
}

void DrawingAddItemsCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mWidget) mWidget->removeItems(mItems);
	mUndone = true;
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingRemoveItemsCommand::DrawingRemoveItemsCommand(DrawingWidget* widget,
	const QList<DrawingItem*>& items, QUndoCommand* parent) :
	DrawingUndoCommand("Remove Items", parent)
{
	mWidget = widget;
	mItems = items;
	mUndone = true;

	if (mWidget)
	{
		QList<DrawingItem*> widgetItems = mWidget->items();
		for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
			mIndices[*itemIter] = widgetItems.indexOf(*itemIter);
	}
}

DrawingRemoveItemsCommand::~DrawingRemoveItemsCommand()
{
	if (!mUndone)
	{
		while (!mItems.isEmpty()) delete mItems.takeFirst();
	}
}

//==================================================================================================

int DrawingRemoveItemsCommand::id() const
{
	return RemoveItemsType;
}

//==================================================================================================

void DrawingRemoveItemsCommand::redo()
{
	mUndone = false;
	if (mWidget) mWidget->removeItems(mItems);
	DrawingUndoCommand::redo();
}

void DrawingRemoveItemsCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mWidget) mWidget->insertItems(mItems, mIndices);
	mUndone = true;
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingMoveItemsCommand::DrawingMoveItemsCommand(DrawingWidget* widget,
	const QList<DrawingItem*>& items, const QHash<DrawingItem*,QPointF>& positions, bool finalMove,
	QUndoCommand* parent) : DrawingUndoCommand("Move Items", parent)
{
	mWidget = widget;
	mItems = items;
	mPositions = positions;
	mFinalMove = finalMove;

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		mOriginalPositions[*itemIter] = (*itemIter)->position();
}

DrawingMoveItemsCommand::~DrawingMoveItemsCommand() { }

//==================================================================================================

int DrawingMoveItemsCommand::id() const
{
	return MoveItemsType;
}

bool DrawingMoveItemsCommand::mergeWith(const QUndoCommand* command)
{
	bool mergeSuccess = false;

	if (command && command->id() == MoveItemsType)
	{
		const DrawingMoveItemsCommand* moveCommand =
			static_cast<const DrawingMoveItemsCommand*>(command);

		if (moveCommand && mWidget == moveCommand->mWidget && mItems == moveCommand->mItems && !mFinalMove)
		{
			mPositions = moveCommand->mPositions;
			mFinalMove = moveCommand->mFinalMove;
			mergeChildren(moveCommand);
			mergeSuccess = true;
		}
	}

	return mergeSuccess;
}

//==================================================================================================

void DrawingMoveItemsCommand::redo()
{
	if (mWidget) mWidget->moveItems(mItems, mPositions);
	DrawingUndoCommand::redo();
}

void DrawingMoveItemsCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mWidget) mWidget->moveItems(mItems, mOriginalPositions);
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingResizeItemCommand::DrawingResizeItemCommand(DrawingWidget* widget, DrawingItemPoint* point,
	const QPointF& position, bool finalResize, QUndoCommand* parent) :
	DrawingUndoCommand("Resize Item", parent)
{
	mWidget = widget;
	mPoint = point;
	mPosition = position;
	mFinalResize = finalResize;

	if (mPoint && mPoint->item())
		mOriginalPosition = mPoint->item()->mapToScene(mPoint->position());
}

DrawingResizeItemCommand::DrawingResizeItemCommand(const DrawingResizeItemCommand& command,
	QUndoCommand* parent) : DrawingUndoCommand("Resize Item", parent)
{
	mWidget = command.mWidget;
	mPoint = command.mPoint;
	mPosition = command.mPosition;
	mOriginalPosition = command.mOriginalPosition;
	mFinalResize = command.mFinalResize;
}

DrawingResizeItemCommand::~DrawingResizeItemCommand() { }

//==================================================================================================

int DrawingResizeItemCommand::id() const
{
	return ResizeItemType;
}

bool DrawingResizeItemCommand::mergeWith(const QUndoCommand* command)
{
	bool mergeSuccess = false;

	if (command && command->id() == ResizeItemType)
	{
		const DrawingResizeItemCommand* resizeCommand =
			static_cast<const DrawingResizeItemCommand*>(command);

		if (resizeCommand && mWidget == resizeCommand->mWidget &&
			mPoint == resizeCommand->mPoint && !mFinalResize)
		{
			mPosition = resizeCommand->mPosition;
			mFinalResize = resizeCommand->mFinalResize;
			mergeChildren(resizeCommand);
			mergeSuccess = true;
		}
	}

	return mergeSuccess;
}

//==================================================================================================

void DrawingResizeItemCommand::redo()
{
	if (mWidget) mWidget->resizeItem(mPoint, mPosition);
	DrawingUndoCommand::redo();
}

void DrawingResizeItemCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mWidget) mWidget->resizeItem(mPoint, mOriginalPosition);
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingRotateItemsCommand::DrawingRotateItemsCommand(DrawingWidget* widget,
	const QList<DrawingItem*>& items, const QPointF& position, QUndoCommand* parent) :
	DrawingUndoCommand("Rotate Items", parent)
{
	mWidget = widget;
	mItems = items;
	mPosition = position;
}

DrawingRotateItemsCommand::~DrawingRotateItemsCommand() { }

//==================================================================================================

int DrawingRotateItemsCommand::id() const
{
	return RotateItemsType;
}

//==================================================================================================

void DrawingRotateItemsCommand::redo()
{
	if (mWidget) mWidget->rotateItems(mItems, mPosition);
	DrawingUndoCommand::redo();
}

void DrawingRotateItemsCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mWidget) mWidget->rotateBackItems(mItems, mPosition);
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingRotateBackItemsCommand::DrawingRotateBackItemsCommand(DrawingWidget* widget,
	const QList<DrawingItem*>& items, const QPointF& position, QUndoCommand* parent) :
	DrawingUndoCommand("Rotate Back Items", parent)
{
	mWidget = widget;
	mItems = items;
	mPosition = position;
}

DrawingRotateBackItemsCommand::~DrawingRotateBackItemsCommand() { }

//==================================================================================================

int DrawingRotateBackItemsCommand::id() const
{
	return RotateBackItemsType;
}

//==================================================================================================

void DrawingRotateBackItemsCommand::redo()
{
	if (mWidget) mWidget->rotateBackItems(mItems, mPosition);
	DrawingUndoCommand::redo();
}

void DrawingRotateBackItemsCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mWidget) mWidget->rotateItems(mItems, mPosition);
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingFlipItemsHorizontalCommand::DrawingFlipItemsHorizontalCommand(DrawingWidget* widget,
	const QList<DrawingItem*>& items, const QPointF& position, QUndoCommand* parent) :
	DrawingUndoCommand("Flip Items Horizontal", parent)
{
	mWidget = widget;
	mItems = items;
	mPosition = position;
}

DrawingFlipItemsHorizontalCommand::~DrawingFlipItemsHorizontalCommand() { }

//==================================================================================================

int DrawingFlipItemsHorizontalCommand::id() const
{
	return FlipItemsHorizontalType;
}

//==================================================================================================

void DrawingFlipItemsHorizontalCommand::redo()
{
	if (mWidget) mWidget->flipItemsHorizontal(mItems, mPosition);
	DrawingUndoCommand::redo();
}

void DrawingFlipItemsHorizontalCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mWidget) mWidget->flipItemsHorizontal(mItems, mPosition);
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingFlipItemsVerticalCommand::DrawingFlipItemsVerticalCommand(DrawingWidget* widget,
	const QList<DrawingItem*>& items, const QPointF& position, QUndoCommand* parent) :
	DrawingUndoCommand("Flip Items Vertical", parent)
{
	mWidget = widget;
	mItems = items;
	mPosition = position;
}

DrawingFlipItemsVerticalCommand::~DrawingFlipItemsVerticalCommand() { }

//==================================================================================================

int DrawingFlipItemsVerticalCommand::id() const
{
	return FlipItemsVerticalType;
}

//==================================================================================================

void DrawingFlipItemsVerticalCommand::redo()
{
	if (mWidget) mWidget->flipItemsVertical(mItems, mPosition);
	DrawingUndoCommand::redo();
}

void DrawingFlipItemsVerticalCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mWidget) mWidget->flipItemsVertical(mItems, mPosition);
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingSelectItemsCommand::DrawingSelectItemsCommand(DrawingWidget* widget,
	const QList<DrawingItem*>& items, bool finalSelect, QUndoCommand* parent) :
	DrawingUndoCommand("Select Items", parent)
{
	mWidget = widget;
	mSelectedItems = items;
	mFinalSelect = finalSelect;

	if (mWidget) mOriginalSelectedItems = mWidget->selectedItems();
}

DrawingSelectItemsCommand::~DrawingSelectItemsCommand() { }

//==================================================================================================

int DrawingSelectItemsCommand::id() const
{
	return SelectItemsType;
}

bool DrawingSelectItemsCommand::mergeWith(const QUndoCommand* command)
{
	bool mergeSuccess = false;

	if (command && command->id() == SelectItemsType)
	{
		const DrawingSelectItemsCommand* selectCommand =
			static_cast<const DrawingSelectItemsCommand*>(command);

		if (selectCommand && mWidget == selectCommand->mWidget && !mFinalSelect)
		{
			for(auto itemIter = selectCommand->mSelectedItems.begin();
				itemIter != selectCommand->mSelectedItems.end(); itemIter++)
			{
				if (!mSelectedItems.contains(*itemIter)) mSelectedItems.append(*itemIter);
			}

			mFinalSelect = selectCommand->mFinalSelect;
			mergeChildren(selectCommand);
			mergeSuccess = true;
		}
	}

	return mergeSuccess;
}

//==================================================================================================

void DrawingSelectItemsCommand::redo()
{
	if (mWidget) mWidget->selectItems(mSelectedItems);
	DrawingUndoCommand::redo();
}

void DrawingSelectItemsCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mWidget) mWidget->selectItems(mOriginalSelectedItems);
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingReorderItemsCommand::DrawingReorderItemsCommand(DrawingWidget* widget,
	const QList<DrawingItem*>& itemOrder, QUndoCommand* parent) :
	DrawingUndoCommand("Reorder Items", parent)
{
	mWidget = widget;
	mItemOrder = itemOrder;
	if (mWidget) mOriginalItemOrder = mWidget->items();
}

DrawingReorderItemsCommand::~DrawingReorderItemsCommand() { }

//==================================================================================================

int DrawingReorderItemsCommand::id() const
{
	return ReorderItemsType;
}

//==================================================================================================

void DrawingReorderItemsCommand::redo()
{
	if (mWidget) mWidget->reorderItems(mItemOrder);
	DrawingUndoCommand::redo();
}

void DrawingReorderItemsCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mWidget) mWidget->reorderItems(mOriginalItemOrder);
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingItemInsertPointCommand::DrawingItemInsertPointCommand(DrawingWidget* widget,
	DrawingItem* item, DrawingItemPoint* point, int pointIndex, QUndoCommand* parent)
	: DrawingUndoCommand("Insert Point", parent)
{
	mWidget = widget;
	mItem = item;
	mPoint = point;
	mPointIndex = pointIndex;
	mUndone = true;
}

DrawingItemInsertPointCommand::~DrawingItemInsertPointCommand()
{
	if (mUndone) delete mPoint;
}

//==================================================================================================

int DrawingItemInsertPointCommand::id() const
{
	return InsertItemPointType;
}

//==================================================================================================

void DrawingItemInsertPointCommand::redo()
{
	mUndone = false;
	if (mWidget) mWidget->insertItemPoint(mItem, mPoint, mPointIndex);
	DrawingUndoCommand::redo();
}

void DrawingItemInsertPointCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mWidget) mWidget->removeItemPoint(mItem, mPoint);
	mUndone = true;
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingItemRemovePointCommand::DrawingItemRemovePointCommand(DrawingWidget* widget,
	DrawingItem* item, DrawingItemPoint* point, QUndoCommand* parent)
	: DrawingUndoCommand("Remove Point", parent)
{
	mWidget = widget;
	mItem = item;
	mPoint = point;
	mUndone = true;

	mPointIndex = (mItem) ? mItem->points().indexOf(mPoint) : -1;
}

DrawingItemRemovePointCommand::~DrawingItemRemovePointCommand()
{
	if (!mUndone) delete mPoint;
}

//==================================================================================================

int DrawingItemRemovePointCommand::id() const
{
	return RemoveItemPointType;
}

//==================================================================================================

void DrawingItemRemovePointCommand::redo()
{
	mUndone = false;
	if (mWidget) mWidget->removeItemPoint(mItem, mPoint);
	DrawingUndoCommand::redo();
}

void DrawingItemRemovePointCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mWidget) mWidget->insertItemPoint(mItem, mPoint, mPointIndex);
	mUndone = true;
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingItemPointConnectCommand::DrawingItemPointConnectCommand(DrawingWidget* widget,
	DrawingItemPoint* point1, DrawingItemPoint* point2, QUndoCommand* parent) :
	DrawingUndoCommand("Connect Points", parent)
{
	mWidget = widget;
	mPoint1 = point1;
	mPoint2 = point2;
}

DrawingItemPointConnectCommand::DrawingItemPointConnectCommand(const DrawingItemPointConnectCommand& command,
	QUndoCommand* parent) :	DrawingUndoCommand("Connect Points", parent)
{
	mWidget = command.mWidget;
	mPoint1 = command.mPoint1;
	mPoint2 = command.mPoint2;
}

DrawingItemPointConnectCommand::~DrawingItemPointConnectCommand() { }

//==================================================================================================

int DrawingItemPointConnectCommand::id() const
{
	return PointConnectType;
}

//==================================================================================================

void DrawingItemPointConnectCommand::redo()
{
	if (mWidget) mWidget->connectPoints(mPoint1, mPoint2);
	DrawingUndoCommand::redo();
}

void DrawingItemPointConnectCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mWidget) mWidget->disconnectPoints(mPoint1, mPoint2);
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingItemPointDisconnectCommand::DrawingItemPointDisconnectCommand(DrawingWidget* widget,
	DrawingItemPoint* point1, DrawingItemPoint* point2, QUndoCommand* parent) :
	DrawingUndoCommand("Disconnect Points", parent)
{
	mWidget = widget;
	mPoint1 = point1;
	mPoint2 = point2;
}

DrawingItemPointDisconnectCommand::DrawingItemPointDisconnectCommand(const DrawingItemPointDisconnectCommand& command,
	QUndoCommand* parent) :	DrawingUndoCommand("Disconnect Points", parent)
{
	mWidget = command.mWidget;
	mPoint1 = command.mPoint1;
	mPoint2 = command.mPoint2;
}

DrawingItemPointDisconnectCommand::~DrawingItemPointDisconnectCommand() { }

//==================================================================================================

int DrawingItemPointDisconnectCommand::id() const
{
	return PointDisconnectType;
}

//==================================================================================================

void DrawingItemPointDisconnectCommand::redo()
{
	if (mWidget) mWidget->disconnectPoints(mPoint1, mPoint2);
	DrawingUndoCommand::redo();
}

void DrawingItemPointDisconnectCommand::undo()
{
	DrawingUndoCommand::undo();
	if (mWidget) mWidget->connectPoints(mPoint1, mPoint2);
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingItemsSetPropertiesCommand::DrawingItemsSetPropertiesCommand(DrawingWidget* widget,
	const QList<DrawingItem*>& items, const QHash< DrawingItem*, QHash<QString,QVariant> >& properties,
	QUndoCommand* parent) : DrawingUndoCommand("Set Items' Properties", parent)
{
	QHash<QString,QVariant> newProperties, originalProperties, originalPropertiesToStore;

	mWidget = widget;
	mProperties = properties;

	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
	{
		newProperties = properties.value(*itemIter);
		originalProperties = (*itemIter)->properties();

		originalPropertiesToStore.clear();
		for(auto propIter = newProperties.begin(); propIter != newProperties.end(); propIter++)
		{
			if (originalProperties.contains(propIter.key()))
				originalPropertiesToStore.insert(propIter.key(), originalProperties.value(propIter.key()));
		}

		mOriginalProperties.insert(*itemIter, originalPropertiesToStore);
	}
}

DrawingItemsSetPropertiesCommand::~DrawingItemsSetPropertiesCommand() { }

//==================================================================================================

int DrawingItemsSetPropertiesCommand::id() const
{
	return SetItemsPropertiesType;
}

bool DrawingItemsSetPropertiesCommand::mergeWith(const QUndoCommand* command)
{
	bool mergeSuccess = false;

	if (command && command->id() == SetItemsPropertiesType)
	{
		const DrawingItemsSetPropertiesCommand* propertiesCommand =
			static_cast<const DrawingItemsSetPropertiesCommand*>(command);

		if (propertiesCommand && mWidget == propertiesCommand->mWidget)
		{
			QList<DrawingItem*> keys = mProperties.keys();
			QList<DrawingItem*> otherKeys = propertiesCommand->mProperties.keys();

			if (keys.size() == 1 && otherKeys.size() == 1 && keys.first() == otherKeys.first())
			{
				QHash<QString,QVariant> properties = mProperties.value(keys.first());
				QHash<QString,QVariant> otherProperties = propertiesCommand->mProperties.value(otherKeys.first());

				if (properties.size() == 1 && otherProperties.size() == 1 &&
					properties.contains("caption") && otherProperties.contains("caption"))
				{
					mProperties = propertiesCommand->mProperties;
					mergeChildren(propertiesCommand);
					mergeSuccess = true;
				}
			}
		}
	}

	return mergeSuccess;
}

//==================================================================================================

void DrawingItemsSetPropertiesCommand::redo()
{
	if (mWidget) mWidget->setItemsProperties(mProperties);
	QUndoCommand::redo();
}

void DrawingItemsSetPropertiesCommand::undo()
{
	QUndoCommand::undo();
	if (mWidget) mWidget->setItemsProperties(mOriginalProperties);
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
