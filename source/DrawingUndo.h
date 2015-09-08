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

#ifndef DRAWINGUNDO_H
#define DRAWINGUNDO_H

#include <DrawingTypes.h>

class DrawingUndoCommand : public QUndoCommand
{
public:
	enum Type { AddItemsType, RemoveItemsType, MoveItemsType,
		RotateItemsType, RotateBackItemsType, FlipItemsType, ItemResizeType,
		ReorderItemsType, InsertItemPointType, RemoveItemPointType,
		PointConnectType, PointDisconnectType,
		UpdateItemPropertiesType, UpdatePropertiesType, NumberOfCommands };

public:
	DrawingUndoCommand(const QString& title = QString(), QUndoCommand* parent = nullptr);
	DrawingUndoCommand(const DrawingUndoCommand& command, QUndoCommand* parent = nullptr);
	virtual ~DrawingUndoCommand();

protected:
	virtual void mergeChildren(const QUndoCommand* command);
};

//==================================================================================================

class DrawingAddItemsCommand : public DrawingUndoCommand
{
private:
	DrawingWidget* mDrawing;
	QList<DrawingItem*> mItems;
	bool mUndone;

public:
	DrawingAddItemsCommand(DrawingWidget* drawing, DrawingItem* item, QUndoCommand* parent = nullptr);
	DrawingAddItemsCommand(DrawingWidget* drawing, const QList<DrawingItem*>& items, QUndoCommand* parent = nullptr);
	~DrawingAddItemsCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingRemoveItemsCommand : public DrawingUndoCommand
{
private:
	DrawingWidget* mDrawing;
	QList<DrawingItem*> mItems;
	bool mUndone;
	QMap<DrawingItem*,int> mItemIndex;

public:
	DrawingRemoveItemsCommand(DrawingWidget* drawing, DrawingItem* item, QUndoCommand* parent = nullptr);
	DrawingRemoveItemsCommand(DrawingWidget* drawing, const QList<DrawingItem*>& items, QUndoCommand* parent = nullptr);
	~DrawingRemoveItemsCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingMoveItemsCommand : public DrawingUndoCommand
{
private:
	QList<DrawingItem*> mItems;
	QMap<DrawingItem*,QPointF> mScenePos;
	QMap<DrawingItem*,QPointF> mOriginalScenePos;
	bool mFinalMove;

public:
	DrawingMoveItemsCommand(DrawingItem* item, const QPointF& newPos,
		const QPointF& initialPos, bool final, QUndoCommand* parent = nullptr);
	DrawingMoveItemsCommand(const QList<DrawingItem*>& items, const QMap<DrawingItem*,QPointF>& newPos,
		const QMap<DrawingItem*,QPointF>& initialPos, bool final, QUndoCommand* parent = nullptr);
	~DrawingMoveItemsCommand();

	int id() const;
	bool mergeWith(const QUndoCommand* command);

	void redo();
	void undo();
};

//==================================================================================================

class DrawingResizeItemCommand : public DrawingUndoCommand
{
private:
	DrawingItemPoint* mPoint;
	QPointF mScenePos;
	QPointF mOriginalScenePos;
	bool mFinalMove;

public:
	DrawingResizeItemCommand(DrawingItemPoint* point, const QPointF& scenePos, bool final, QUndoCommand* parent = nullptr);
	DrawingResizeItemCommand(const DrawingResizeItemCommand& command, QUndoCommand* parent = nullptr);
	~DrawingResizeItemCommand();

	int id() const;
	bool mergeWith(const QUndoCommand* command);

	void redo();
	void undo();
};

//==================================================================================================

class DrawingRotateItemsCommand : public DrawingUndoCommand
{
private:
	QList<DrawingItem*> mItems;
	QPointF mScenePos;

public:
	DrawingRotateItemsCommand(DrawingItem* item,
		const QPointF& scenePos, QUndoCommand* parent = nullptr);
	DrawingRotateItemsCommand(const QList<DrawingItem*>& items,
		const QPointF& scenePos, QUndoCommand* parent = nullptr);
	~DrawingRotateItemsCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingRotateBackItemsCommand : public DrawingUndoCommand
{
private:
	QList<DrawingItem*> mItems;
	QPointF mScenePos;

public:
	DrawingRotateBackItemsCommand(DrawingItem* item,
		const QPointF& scenePos, QUndoCommand* parent = nullptr);
	DrawingRotateBackItemsCommand(const QList<DrawingItem*>& items,
		const QPointF& scenePos, QUndoCommand* parent = nullptr);
	~DrawingRotateBackItemsCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingFlipItemsCommand : public DrawingUndoCommand
{
private:
	QList<DrawingItem*> mItems;
	QPointF mScenePos;

public:
	DrawingFlipItemsCommand(DrawingItem* item,
		const QPointF& scenePos, QUndoCommand* parent = nullptr);
	DrawingFlipItemsCommand(const QList<DrawingItem*>& items,
		const QPointF& scenePos, QUndoCommand* parent = nullptr);
	~DrawingFlipItemsCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingReorderItemsCommand : public DrawingUndoCommand
{
private:
	DrawingWidget* mDrawing;
	QList<DrawingItem*> mNewItemOrder, mOriginalItemOrder;

public:
	DrawingReorderItemsCommand(DrawingWidget* drawing,
		const QList<DrawingItem*>& newItemOrder, QUndoCommand* parent = nullptr);
	~DrawingReorderItemsCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingItemInsertPointCommand : public DrawingUndoCommand
{
private:
	DrawingItem* mItem;
	DrawingItemPoint* mPoint;
	int mPointIndex;
	bool mUndone;

public:
	DrawingItemInsertPointCommand(DrawingItem* item, DrawingItemPoint* point,
		int pointIndex, QUndoCommand* parent = nullptr);
	~DrawingItemInsertPointCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingItemRemovePointCommand : public DrawingUndoCommand
{
private:
	DrawingItem* mItem;
	DrawingItemPoint* mPoint;
	int mPointIndex;
	bool mUndone;

public:
	DrawingItemRemovePointCommand(DrawingItem* item, DrawingItemPoint* point,
		QUndoCommand* parent = nullptr);
	~DrawingItemRemovePointCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingItemPointConnectCommand : public DrawingUndoCommand
{
private:
	DrawingItemPoint* mPoint1;
	DrawingItemPoint* mPoint2;

public:
	DrawingItemPointConnectCommand(DrawingItemPoint* point1,
		DrawingItemPoint* point2, QUndoCommand* parent = NULL);
	DrawingItemPointConnectCommand(const DrawingItemPointConnectCommand& command,
		QUndoCommand* parent = NULL);
	~DrawingItemPointConnectCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingItemPointDisconnectCommand : public DrawingUndoCommand
{
private:
	DrawingItemPoint* mPoint1;
	DrawingItemPoint* mPoint2;

public:
	DrawingItemPointDisconnectCommand(DrawingItemPoint* point1,
		DrawingItemPoint* point2, QUndoCommand* parent = NULL);
	DrawingItemPointDisconnectCommand(const DrawingItemPointDisconnectCommand& command,
		QUndoCommand* parent = NULL);
	~DrawingItemPointDisconnectCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingUpdateItemPropertiesCommand : public DrawingUndoCommand
{
private:
	QList<DrawingItem*> mItems;
	QMap<QString,QVariant> mProperties;
	QMap<DrawingItem*, QMap<QString,QVariant>> mOriginalProperties;

public:
	DrawingUpdateItemPropertiesCommand(const QList<DrawingItem*>& items,
		const QMap<QString,QVariant>& newProperties, QUndoCommand* parent = nullptr);
	~DrawingUpdateItemPropertiesCommand();

	int id() const;

	void redo();
	void undo();
};

//==================================================================================================

class DrawingUpdatePropertiesCommand : public DrawingUndoCommand
{
private:
	DrawingWidget* mDrawing;

	QRectF mSceneRect;
	qreal mGrid;
	QBrush mBackgroundBrush;
	DrawingGridStyle mGridStyle;
	QBrush mGridBrush;
	int mGridSpacingMajor;
	int mGridSpacingMinor;

	QRectF mOriginalSceneRect;
	qreal mOriginalGrid;
	QBrush mOriginalBackgroundBrush;
	DrawingGridStyle mOriginalGridStyle;
	QBrush mOriginalGridBrush;
	int mOriginalGridSpacingMajor;
	int mOriginalGridSpacingMinor;

public:
	DrawingUpdatePropertiesCommand(DrawingWidget* drawing, const QRectF& sceneRect, qreal grid,
		const QBrush& backgroundBrush, DrawingGridStyle gridStyle, const QBrush& gridBrush,
		int gridSpacingMajor, int gridSpacingMinor, QUndoCommand* parent = nullptr);
	~DrawingUpdatePropertiesCommand();

	int id() const;

	void redo();
	void undo();
};

#endif
