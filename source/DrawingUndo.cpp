/* DrawingUndo.cpp
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

	for(auto otherChildIter = otherChildren.begin(); otherChildIter != otherChildren.end(); otherChildIter++)
	{
		switch ((*otherChildIter)->id())
		{
		case ItemResizeType:
			new DrawingResizeItemCommand(*static_cast<DrawingResizeItemCommand*>(*otherChildIter), this);
			break;
		case PointConnectType:
			new DrawingItemPointConnectCommand(*static_cast<DrawingItemPointConnectCommand*>(*otherChildIter), this);
			break;
		case PointDisconnectType:
			new DrawingItemPointDisconnectCommand(*static_cast<DrawingItemPointDisconnectCommand*>(*otherChildIter), this);
			break;
		default:
			break;
		}
	}
}

DrawingUndoCommand::~DrawingUndoCommand() { }

void DrawingUndoCommand::mergeChildren(const QUndoCommand* command)
{
	bool mergeSuccess;
	QList<QUndoCommand*> children, otherChildren;

	for(int i = 0; i < childCount(); i++)
		children.append(const_cast<QUndoCommand*>(child(i)));
	for(int i = 0; i < command->childCount(); i++)
		otherChildren.append(const_cast<QUndoCommand*>(command->child(i)));

	for(auto otherChildIter = otherChildren.begin(); otherChildIter != otherChildren.end(); otherChildIter++)
	{
		mergeSuccess = false;
		for(auto childIter = children.begin(); childIter != children.end(); childIter++)
			mergeSuccess = ((*childIter)->mergeWith(*otherChildIter) && mergeSuccess);

		if (!mergeSuccess)
		{
			switch ((*otherChildIter)->id())
			{
			case ItemResizeType:
				new DrawingResizeItemCommand(*static_cast<DrawingResizeItemCommand*>(*otherChildIter), this);
				break;
			case PointConnectType:
				new DrawingItemPointConnectCommand(*static_cast<DrawingItemPointConnectCommand*>(*otherChildIter), this);
				break;
			case PointDisconnectType:
				new DrawingItemPointDisconnectCommand(*static_cast<DrawingItemPointDisconnectCommand*>(*otherChildIter), this);
				break;
			default:
				break;
			}
		}
	}
}

//==================================================================================================

DrawingAddItemsCommand::DrawingAddItemsCommand(DrawingWidget* drawing, DrawingItem* item,
	QUndoCommand* parent) : DrawingUndoCommand("Add Item", parent)
{
	mDrawing = drawing;
	mItems.append(item);
	mUndone = true;
}

DrawingAddItemsCommand::DrawingAddItemsCommand(DrawingWidget* drawing, const QList<DrawingItem*>& items,
	QUndoCommand* parent) : DrawingUndoCommand("Add Items", parent)
{
	mDrawing = drawing;
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

int DrawingAddItemsCommand::id() const
{
	return AddItemsType;
}

void DrawingAddItemsCommand::redo()
{
	mUndone = false;
	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		mDrawing->addItem(*itemIter);
	DrawingUndoCommand::redo();
}

void DrawingAddItemsCommand::undo()
{
	DrawingUndoCommand::undo();
	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		mDrawing->removeItem(*itemIter);
	mUndone = true;
}

//==================================================================================================

DrawingRemoveItemsCommand::DrawingRemoveItemsCommand(DrawingWidget* drawing, DrawingItem* item,
	QUndoCommand* parent) : DrawingUndoCommand("Remove Item", parent)
{
	mDrawing = drawing;
	if (item) mItems.append(item);
	mUndone = true;
}

DrawingRemoveItemsCommand::DrawingRemoveItemsCommand(DrawingWidget* drawing, const QList<DrawingItem*>& items,
	QUndoCommand* parent) : DrawingUndoCommand("Remove Items", parent)
{
	mDrawing = drawing;
	mItems = items;
	mUndone = true;
}

DrawingRemoveItemsCommand::~DrawingRemoveItemsCommand()
{
	if (!mUndone)
	{
		while (!mItems.isEmpty()) delete mItems.takeFirst();
	}
}

int DrawingRemoveItemsCommand::id() const
{
	return RemoveItemsType;
}

void DrawingRemoveItemsCommand::redo()
{
	mUndone = false;

	QList<DrawingItem*> sceneItems = mDrawing->items();

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		mItemIndex[*itemIter] = sceneItems.indexOf(*itemIter);
		mDrawing->removeItem(*itemIter);
	}

	DrawingUndoCommand::redo();
}

void DrawingRemoveItemsCommand::undo()
{
	DrawingUndoCommand::undo();

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		mDrawing->insertItem(mItemIndex[*itemIter], *itemIter);

	mUndone = true;
}

//==================================================================================================

DrawingMoveItemsCommand::DrawingMoveItemsCommand(const QList<DrawingItem*>& items, const QMap<DrawingItem*,QPointF>& newPos,
	const QMap<DrawingItem*,QPointF>& initialPos, bool final, QUndoCommand* parent) : DrawingUndoCommand("Move Items", parent)
{
	mItems = items;
	mOriginalScenePos = initialPos;
	mScenePos = newPos;
	mFinalMove = final;
}

DrawingMoveItemsCommand::DrawingMoveItemsCommand(DrawingItem* item, const QPointF& newPos,
	const QPointF& initialPos, bool final, QUndoCommand* parent) : DrawingUndoCommand("Move Items", parent)
{
	mItems.append(item);
	mOriginalScenePos[item] = initialPos;
	mScenePos[item] = newPos;
	mFinalMove = final;
}

DrawingMoveItemsCommand::~DrawingMoveItemsCommand() { }

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

		if (moveCommand && mItems == moveCommand->mItems && !mFinalMove)
		{
			mScenePos = moveCommand->mScenePos;
			mFinalMove = moveCommand->mFinalMove;
			mergeChildren(moveCommand);
			mergeSuccess = true;
		}
	}

	return mergeSuccess;
}

void DrawingMoveItemsCommand::redo()
{
	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		(*itemIter)->setPos(mScenePos[*itemIter]);

	DrawingUndoCommand::redo();
}

void DrawingMoveItemsCommand::undo()
{
	DrawingUndoCommand::undo();

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		(*itemIter)->setPos(mOriginalScenePos[*itemIter]);
}

//==================================================================================================

DrawingResizeItemCommand::DrawingResizeItemCommand(DrawingItemPoint* point, const QPointF& scenePos,
	bool final, QUndoCommand* parent) :	DrawingUndoCommand("Resize Item", parent)
{
	mPoint = point;
	mScenePos = scenePos;
	mOriginalScenePos = mPoint->item()->mapToScene(mPoint->pos());
	mFinalMove = final;
}

DrawingResizeItemCommand::DrawingResizeItemCommand(const DrawingResizeItemCommand& command,
	QUndoCommand* parent) : DrawingUndoCommand(command, parent)
{
	mPoint = command.mPoint;
	mScenePos = command.mScenePos;
	mOriginalScenePos = command.mOriginalScenePos;
	mFinalMove = command.mFinalMove;
}

DrawingResizeItemCommand::~DrawingResizeItemCommand() { }

int DrawingResizeItemCommand::id() const
{
	return ItemResizeType;
}

bool DrawingResizeItemCommand::mergeWith(const QUndoCommand* command)
{
	bool mergeSuccess = false;

	if (command && command->id() == ItemResizeType)
	{
		const DrawingResizeItemCommand* resizeCommand =
			static_cast<const DrawingResizeItemCommand*>(command);

		if (resizeCommand && mPoint == resizeCommand->mPoint && !mFinalMove)
		{
			mScenePos = resizeCommand->mScenePos;
			mFinalMove = resizeCommand->mFinalMove;
			mergeChildren(resizeCommand);
			mergeSuccess = true;
		}
	}

	return mergeSuccess;
}

void DrawingResizeItemCommand::redo()
{
	if (mPoint && mPoint->item()) mPoint->item()->resizeItem(mPoint, mScenePos);

	DrawingUndoCommand::redo();
}

void DrawingResizeItemCommand::undo()
{
	DrawingUndoCommand::undo();

	if (mPoint && mPoint->item()) mPoint->item()->resizeItem(mPoint, mOriginalScenePos);
}

//==================================================================================================

DrawingRotateItemsCommand::DrawingRotateItemsCommand(DrawingItem* item,
	const QPointF& scenePos, QUndoCommand* parent) : DrawingUndoCommand("Rotate Item", parent)
{
	mItems.append(item);
	mScenePos = scenePos;
}

DrawingRotateItemsCommand::DrawingRotateItemsCommand(const QList<DrawingItem*>& items,
	const QPointF& scenePos, QUndoCommand* parent) : DrawingUndoCommand("Rotate Items", parent)
{
	mItems = items;
	mScenePos = scenePos;
}

DrawingRotateItemsCommand::~DrawingRotateItemsCommand() { }

int DrawingRotateItemsCommand::id() const
{
	return RotateItemsType;
}

void DrawingRotateItemsCommand::redo()
{
	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++) (*itemIter)->rotateItem(mScenePos);

	DrawingUndoCommand::redo();
}

void DrawingRotateItemsCommand::undo()
{
	DrawingUndoCommand::undo();

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++) (*itemIter)->rotateBackItem(mScenePos);
}

//==================================================================================================

DrawingRotateBackItemsCommand::DrawingRotateBackItemsCommand(DrawingItem* item,
	const QPointF& scenePos, QUndoCommand* parent) : DrawingUndoCommand("Rotate Back Item", parent)
{
	mItems.append(item);
	mScenePos = scenePos;
}

DrawingRotateBackItemsCommand::DrawingRotateBackItemsCommand(const QList<DrawingItem*>& items,
	const QPointF& scenePos, QUndoCommand* parent) : DrawingUndoCommand("Rotate Back Items", parent)
{
	mItems = items;
	mScenePos = scenePos;
}

DrawingRotateBackItemsCommand::~DrawingRotateBackItemsCommand() { }

int DrawingRotateBackItemsCommand::id() const
{
	return RotateBackItemsType;
}

void DrawingRotateBackItemsCommand::redo()
{
	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++) (*itemIter)->rotateBackItem(mScenePos);

	DrawingUndoCommand::redo();
}

void DrawingRotateBackItemsCommand::undo()
{
	DrawingUndoCommand::undo();

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++) (*itemIter)->rotateItem(mScenePos);
}

//==================================================================================================

DrawingFlipItemsCommand::DrawingFlipItemsCommand(DrawingItem* item,
	const QPointF& scenePos, QUndoCommand* parent) : DrawingUndoCommand("Flip Item", parent)
{
	mItems.append(item);
	mScenePos = scenePos;
}

DrawingFlipItemsCommand::DrawingFlipItemsCommand(const QList<DrawingItem*>& items,
	const QPointF& scenePos, QUndoCommand* parent) : DrawingUndoCommand("Flip Items", parent)
{
	mItems = items;
	mScenePos = scenePos;
}

DrawingFlipItemsCommand::~DrawingFlipItemsCommand() { }

int DrawingFlipItemsCommand::id() const
{
	return FlipItemsType;
}

void DrawingFlipItemsCommand::redo()
{
	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++) (*itemIter)->flipItem(mScenePos);

	DrawingUndoCommand::redo();
}

void DrawingFlipItemsCommand::undo()
{
	DrawingUndoCommand::undo();

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++) (*itemIter)->flipItem(mScenePos);
}

//==================================================================================================

DrawingReorderItemsCommand::DrawingReorderItemsCommand(DrawingWidget* drawing,
	const QList<DrawingItem*>& newItemOrder, QUndoCommand* parent) : DrawingUndoCommand("Reorder Items", parent)
{
	mDrawing = drawing;
	mNewItemOrder = newItemOrder;
	mOriginalItemOrder = mDrawing->items();
}

DrawingReorderItemsCommand::~DrawingReorderItemsCommand() { }

int DrawingReorderItemsCommand::id() const
{
	return ReorderItemsType;
}

void DrawingReorderItemsCommand::redo()
{
	mDrawing->mItems = mNewItemOrder;
	DrawingUndoCommand::redo();
}

void DrawingReorderItemsCommand::undo()
{
	DrawingUndoCommand::undo();
	mDrawing->mItems = mOriginalItemOrder;
}

//==================================================================================================

DrawingItemInsertPointCommand::DrawingItemInsertPointCommand(DrawingItem* item, DrawingItemPoint* point,
	int pointIndex, QUndoCommand* parent) : DrawingUndoCommand("Insert Point", parent)
{
	mItem = item;
	mPoint = point;
	mPointIndex = pointIndex;
	mUndone = true;
}

DrawingItemInsertPointCommand::~DrawingItemInsertPointCommand()
{
	if (mUndone) delete mPoint;
}

int DrawingItemInsertPointCommand::id() const
{
	return InsertItemPointType;
}

void DrawingItemInsertPointCommand::redo()
{
	mItem->insertPoint(mPointIndex, mPoint);
	mUndone = false;

	DrawingUndoCommand::redo();
}

void DrawingItemInsertPointCommand::undo()
{
	DrawingUndoCommand::undo();

	mPointIndex = mItem->points().indexOf(mPoint);
	mItem->removePoint(mPoint);
	mUndone = true;
}

//==================================================================================================

DrawingItemRemovePointCommand::DrawingItemRemovePointCommand(DrawingItem* item, DrawingItemPoint* point,
	QUndoCommand* parent) : DrawingUndoCommand("Remove Point", parent)
{
	mItem = item;
	mPoint = point;
	mPointIndex = mItem->points().indexOf(mPoint);
	mUndone = true;
}

DrawingItemRemovePointCommand::~DrawingItemRemovePointCommand()
{
	if (!mUndone) delete mPoint;
}

int DrawingItemRemovePointCommand::id() const
{
	return RemoveItemPointType;
}

void DrawingItemRemovePointCommand::redo()
{
	mPointIndex = mItem->points().indexOf(mPoint);
	mItem->removePoint(mPoint);
	mUndone = false;

	DrawingUndoCommand::redo();
}

void DrawingItemRemovePointCommand::undo()
{
	DrawingUndoCommand::undo();

	mItem->insertPoint(mPointIndex, mPoint);
	mUndone = true;
}

//==================================================================================================

DrawingItemPointConnectCommand::DrawingItemPointConnectCommand(DrawingItemPoint* point1,
	DrawingItemPoint* point2, QUndoCommand* parent) : DrawingUndoCommand("Connect Points", parent)
{
	mPoint1 = point1;
	mPoint2 = point2;
}

DrawingItemPointConnectCommand::DrawingItemPointConnectCommand(
	const DrawingItemPointConnectCommand& command, QUndoCommand* parent) : DrawingUndoCommand(command, parent)
{
	mPoint1 = command.mPoint1;
	mPoint2 = command.mPoint2;
}

DrawingItemPointConnectCommand::~DrawingItemPointConnectCommand()
{
	mPoint1 = nullptr;
	mPoint2 = nullptr;
}

int DrawingItemPointConnectCommand::id() const
{
	return PointConnectType;
}

void DrawingItemPointConnectCommand::redo()
{
	mPoint1->addConnection(mPoint2);
	mPoint2->addConnection(mPoint1);
	DrawingUndoCommand::redo();
}

void DrawingItemPointConnectCommand::undo()
{
	DrawingUndoCommand::undo();
	mPoint1->removeConnection(mPoint2);
	mPoint2->removeConnection(mPoint1);
}

//==================================================================================================

DrawingItemPointDisconnectCommand::DrawingItemPointDisconnectCommand(DrawingItemPoint* point1,
	DrawingItemPoint* point2, QUndoCommand* parent) : DrawingUndoCommand("Disconnect Points", parent)
{
	mPoint1 = point1;
	mPoint2 = point2;
}

DrawingItemPointDisconnectCommand::DrawingItemPointDisconnectCommand(
	const DrawingItemPointDisconnectCommand& command, QUndoCommand* parent) : DrawingUndoCommand(command, parent)
{
	mPoint1 = command.mPoint1;
	mPoint2 = command.mPoint2;
}

DrawingItemPointDisconnectCommand::~DrawingItemPointDisconnectCommand()
{
	mPoint1 = nullptr;
	mPoint2 = nullptr;
}

int DrawingItemPointDisconnectCommand::id() const
{
	return PointDisconnectType;
}

void DrawingItemPointDisconnectCommand::redo()
{
	mPoint1->removeConnection(mPoint2);
	mPoint2->removeConnection(mPoint1);
	DrawingUndoCommand::redo();
}

void DrawingItemPointDisconnectCommand::undo()
{
	DrawingUndoCommand::undo();
	mPoint1->addConnection(mPoint2);
	mPoint2->addConnection(mPoint1);
}

//==================================================================================================

DrawingUpdateItemPropertiesCommand::DrawingUpdateItemPropertiesCommand(const QList<DrawingItem*>& items,
	const QMap<QString,QVariant>& newProperties, QUndoCommand* parent) : DrawingUndoCommand("Update Properties", parent)
{
	mItems = items;
	mProperties = newProperties;

	QStringList keys = newProperties.keys();

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		QMap<QString,QVariant> properties;

		for(auto keyIter = keys.begin(); keyIter != keys.end(); keyIter++)
		{
			if ((*itemIter)->hasProperty(*keyIter))
				properties[*keyIter] = (*itemIter)->property(*keyIter);
		}

		mOriginalProperties[*itemIter] = properties;
	}
}

DrawingUpdateItemPropertiesCommand::~DrawingUpdateItemPropertiesCommand() { }

int DrawingUpdateItemPropertiesCommand::id() const
{
	return UpdateItemPropertiesType;
}

void DrawingUpdateItemPropertiesCommand::redo()
{
	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		(*itemIter)->updateProperties(mProperties);

	DrawingUndoCommand::redo();
}

void DrawingUpdateItemPropertiesCommand::undo()
{
	DrawingUndoCommand::undo();

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		(*itemIter)->updateProperties(mOriginalProperties[*itemIter]);
}

bool DrawingUpdateItemPropertiesCommand::mergeWith(const QUndoCommand* command)
{
	bool mergeSuccess = false;

	if (command && command->id() == UpdateItemPropertiesType)
	{
		const DrawingUpdateItemPropertiesCommand* propertiesCommand =
			static_cast<const DrawingUpdateItemPropertiesCommand*>(command);

		if (propertiesCommand && mProperties.keys() == propertiesCommand->mProperties.keys())
		{
			mProperties = propertiesCommand->mProperties;
			mergeSuccess = true;
		}
	}

	return mergeSuccess;
}

//==================================================================================================

DrawingUpdatePropertiesCommand::DrawingUpdatePropertiesCommand(DrawingWidget* drawing,
	const QRectF& sceneRect, qreal grid, const QBrush& backgroundBrush,
	DrawingGridStyle gridStyle, const QBrush& gridBrush, int gridSpacingMajor, int gridSpacingMinor,
	QUndoCommand* parent) : DrawingUndoCommand("Update Properties", parent)
{
	mDrawing = drawing;

	mSceneRect = sceneRect;
	mGrid = grid;
	mBackgroundBrush = backgroundBrush;
	mGridStyle = gridStyle;
	mGridBrush = gridBrush;
	mGridSpacingMajor = gridSpacingMajor;
	mGridSpacingMinor = gridSpacingMinor;

	mOriginalSceneRect = drawing->sceneRect();
	mOriginalGrid = drawing->grid();
	mOriginalBackgroundBrush = drawing->backgroundBrush();
	mOriginalGridStyle = drawing->gridStyle();
	mOriginalGridBrush = drawing->gridBrush();
	mOriginalGridSpacingMajor = drawing->gridSpacingMajor();
	mOriginalGridSpacingMinor = drawing->gridSpacingMinor();
}

DrawingUpdatePropertiesCommand::~DrawingUpdatePropertiesCommand() { }

int DrawingUpdatePropertiesCommand::id() const
{
	return UpdatePropertiesType;
}

void DrawingUpdatePropertiesCommand::redo()
{
	mDrawing->setSceneRect(mSceneRect);
	mDrawing->setGrid(mGrid);
	mDrawing->setBackgroundBrush(mBackgroundBrush);
	mDrawing->setGridStyle(mGridStyle);
	mDrawing->setGridBrush(mGridBrush);
	mDrawing->setGridSpacing(mGridSpacingMajor, mGridSpacingMinor);

	DrawingUndoCommand::redo();
}

void DrawingUpdatePropertiesCommand::undo()
{
	DrawingUndoCommand::undo();

	mDrawing->setSceneRect(mOriginalSceneRect);
	mDrawing->setGrid(mOriginalGrid);
	mDrawing->setBackgroundBrush(mOriginalBackgroundBrush);
	mDrawing->setGridStyle(mOriginalGridStyle);
	mDrawing->setGridBrush(mOriginalGridBrush);
	mDrawing->setGridSpacing(mOriginalGridSpacingMajor, mOriginalGridSpacingMinor);
}
