// File: DrawingUndo.h
// Copyright (C) 2023  Jason Allen
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

#ifndef DRAWINGUNDO_H
#define DRAWINGUNDO_H

#include <QHash>
#include <QList>
#include <QUndoCommand>
#include <QRectF>
#include <QVariant>

class DrawingWidget;
class OdgControlPoint;
class OdgGroupItem;
class OdgItem;
class OdgPage;

class DrawingUndoCommand : public QUndoCommand
{
public:
    enum CompressibleCommandIndex { MoveCommandId, ResizeCommandId, SetItemsPropertyCommandId };

private:
    OdgPage* mPage;
    QRectF mViewRect;

public:
    DrawingUndoCommand(const QString& text);

    void storeView(DrawingWidget* drawing);
    void restoreView(DrawingWidget* drawing);
};

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

class DrawingSetPropertyCommand : public QUndoCommand
{
private:
    DrawingWidget* mDrawing;
    QString mName;
    QVariant mNewValue;
    QVariant mOldValue;

public:
    DrawingSetPropertyCommand(DrawingWidget* drawing, const QString& name, const QVariant& value);

    void redo() override;
    void undo() override;
};

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

class DrawingInsertPageCommand : public QUndoCommand
{
private:
    DrawingWidget* mDrawing;
    OdgPage* mPage;
    int mIndex;
    bool mUndone;

public:
    DrawingInsertPageCommand(DrawingWidget* drawing, OdgPage* page, int index);
    ~DrawingInsertPageCommand();

    void redo() override;
    void undo() override;
};

//======================================================================================================================

class DrawingRemovePageCommand : public QUndoCommand
{
private:
    DrawingWidget* mDrawing;
    OdgPage* mPage;
    int mIndex;
    bool mUndone;

public:
    DrawingRemovePageCommand(DrawingWidget* drawing, OdgPage* page);
    ~DrawingRemovePageCommand();

    void redo() override;
    void undo() override;
};

//======================================================================================================================

class DrawingMovePageCommand : public QUndoCommand
{
private:
    DrawingWidget* mDrawing;
    OdgPage* mPage;
    int mNewIndex;
    int mOldIndex;

public:
    DrawingMovePageCommand(DrawingWidget* drawing, OdgPage* page, int index);

    void redo() override;
    void undo() override;
};

//======================================================================================================================

class DrawingSetPagePropertyCommand : public QUndoCommand
{
private:
    DrawingWidget* mDrawing;
    OdgPage* mPage;
    QString mName;
    QVariant mNewValue;
    QVariant mOldValue;

public:
    DrawingSetPagePropertyCommand(DrawingWidget* drawing, OdgPage* page, const QString& name, const QVariant& value);

    void redo() override;
    void undo() override;
};

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

class DrawingAddItemsCommand : public DrawingUndoCommand
{
private:
    DrawingWidget* mDrawing;
    OdgPage* mPage;
    QList<OdgItem*> mItems;
    bool mPlace;
    bool mUndone;

public:
    DrawingAddItemsCommand(DrawingWidget* drawing, OdgPage* page, const QList<OdgItem*>& items, bool place);
    ~DrawingAddItemsCommand();

    void redo() override;
    void undo() override;
};

//======================================================================================================================

class DrawingRemoveItemsCommand : public DrawingUndoCommand
{
private:
    DrawingWidget* mDrawing;
    OdgPage* mPage;
    QList<OdgItem*> mItems;
    QHash<OdgItem*,int> mIndices;
    bool mUndone;

public:
    DrawingRemoveItemsCommand(DrawingWidget* drawing, OdgPage* page, const QList<OdgItem*>& items);
    ~DrawingRemoveItemsCommand();

    void redo() override;
    void undo() override;
};

//======================================================================================================================

class DrawingReorderItemsCommand : public DrawingUndoCommand
{
private:
    DrawingWidget* mDrawing;
    OdgPage* mPage;
    QList<OdgItem*> mItemsOrdered;
    QList<OdgItem*> mOriginalItemsOrdered;
    QList<OdgItem*> mSelectedItems;

public:
    DrawingReorderItemsCommand(DrawingWidget* drawing, OdgPage* page, const QList<OdgItem*>& items);

    void redo() override;
    void undo() override;
};

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

class DrawingMoveItemsCommand : public DrawingUndoCommand
{
private:
    DrawingWidget* mDrawing;
    QList<OdgItem*> mItems;
    QHash<OdgItem*,QPointF> mPositions;
    QHash<OdgItem*,QPointF> mOriginalPositions;
    bool mPlace;

public:
    DrawingMoveItemsCommand(DrawingWidget* drawing, const QList<OdgItem*>& items,
                            const QHash<OdgItem*,QPointF>& positions, bool place);

    int id() const override;
    bool mergeWith(const QUndoCommand* command) override;

    void redo() override;
    void undo() override;
};

//======================================================================================================================

class DrawingResizeItemCommand : public DrawingUndoCommand
{
private:
    DrawingWidget* mDrawing;
    OdgControlPoint* mControlPoint;
    QPointF mPosition;
    QPointF mOriginalPosition;
    bool mSnapTo45Degrees;
    bool mPlace;

public:
    DrawingResizeItemCommand(DrawingWidget* drawing, OdgControlPoint* point, const QPointF& position,
                             bool snapTo45Degrees, bool place);

    int id() const override;
    bool mergeWith(const QUndoCommand* command) override;

    void redo() override;
    void undo() override;
};

//======================================================================================================================

class DrawingResizeItem2Command : public DrawingUndoCommand
{
private:
    DrawingWidget* mDrawing;
    OdgControlPoint* mControlPoint1;
    QPointF mPosition1;
    QPointF mOriginalPosition1;
    OdgControlPoint* mControlPoint2;
    QPointF mPosition2;
    QPointF mOriginalPosition2;
    bool mPlace;

public:
    DrawingResizeItem2Command(DrawingWidget* drawing, OdgControlPoint* point1, const QPointF& p1,
                              OdgControlPoint* point2, const QPointF& p2, bool place);

    void redo() override;
    void undo() override;
};

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

class DrawingRotateItemsCommand : public DrawingUndoCommand
{
private:
    DrawingWidget* mDrawing;
    QList<OdgItem*> mItems;
    QPointF mPosition;

public:
    DrawingRotateItemsCommand(DrawingWidget* drawing, const QList<OdgItem*>& items, const QPointF& position);

    void redo() override;
    void undo() override;
};

//======================================================================================================================

class DrawingRotateBackItemsCommand : public DrawingUndoCommand
{
private:
    DrawingWidget* mDrawing;
    QList<OdgItem*> mItems;
    QPointF mPosition;

public:
    DrawingRotateBackItemsCommand(DrawingWidget* drawing, const QList<OdgItem*>& items, const QPointF& position);

    void redo() override;
    void undo() override;
};

//======================================================================================================================

class DrawingFlipItemsHorizontalCommand : public DrawingUndoCommand
{
private:
    DrawingWidget* mDrawing;
    QList<OdgItem*> mItems;
    QPointF mPosition;

public:
    DrawingFlipItemsHorizontalCommand(DrawingWidget* drawing, const QList<OdgItem*>& items, const QPointF& position);

    void redo() override;
    void undo() override;
};

//======================================================================================================================

class DrawingFlipItemsVerticalCommand : public DrawingUndoCommand
{
private:
    DrawingWidget* mDrawing;
    QList<OdgItem*> mItems;
    QPointF mPosition;

public:
    DrawingFlipItemsVerticalCommand(DrawingWidget* drawing, const QList<OdgItem*>& items, const QPointF& position);

    void redo() override;
    void undo() override;
};

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

class DrawingGroupItemsCommand : public DrawingUndoCommand
{
private:
    DrawingWidget* mDrawing;
    OdgPage* mPage;
    QList<OdgItem*> mItemsToRemove;
    QList<OdgItem*> mItemsToAdd;
    bool mUndone;

public:
    DrawingGroupItemsCommand(DrawingWidget* drawing, OdgPage* page, const QList<OdgItem*>& items);
    ~DrawingGroupItemsCommand();

    void redo() override;
    void undo() override;
};

//======================================================================================================================

class DrawingUngroupItemsCommand : public DrawingUndoCommand
{
private:
    DrawingWidget* mDrawing;
    OdgPage* mPage;
    QList<OdgItem*> mItemsToRemove;
    QList<OdgItem*> mItemsToAdd;
    bool mUndone;

public:
    DrawingUngroupItemsCommand(DrawingWidget* drawing, OdgPage* page, OdgGroupItem* groupItem);
    ~DrawingUngroupItemsCommand();

    void redo() override;
    void undo() override;
};

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

class DrawingInsertPointCommand : public DrawingUndoCommand
{
private:
    DrawingWidget* mDrawing;
    OdgItem* mItem;
    int mIndex;
    OdgControlPoint* mPoint;
    bool mUndone;

public:
    DrawingInsertPointCommand(DrawingWidget* drawing, OdgItem* item, int index, OdgControlPoint* point);
    ~DrawingInsertPointCommand();

    void redo() override;
    void undo() override;
};

//======================================================================================================================

class DrawingRemovePointCommand : public DrawingUndoCommand
{
private:
    DrawingWidget* mDrawing;
    OdgItem* mItem;
    int mIndex;
    OdgControlPoint* mPoint;
    bool mUndone;

public:
    DrawingRemovePointCommand(DrawingWidget* drawing, OdgItem* item, OdgControlPoint* point);
    ~DrawingRemovePointCommand();

    void redo() override;
    void undo() override;
};

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

class DrawingSetItemsPropertyCommand : public DrawingUndoCommand
{
private:
    DrawingWidget* mDrawing;
    QList<OdgItem*> mItems;
    QString mName;
    QVariant mNewValue;
    QHash<OdgItem*,QVariant> mOldValues;

public:
    DrawingSetItemsPropertyCommand(DrawingWidget* drawing, const QList<OdgItem*>& items, const QString& name,
                                   const QVariant& value);

    int id() const override;
    bool mergeWith(const QUndoCommand* command) override;

    void redo() override;
    void undo() override;
};

#endif
