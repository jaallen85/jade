// File: DrawingUndo.cpp
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

#include "DrawingUndo.h"
#include "DrawingWidget.h"
#include "OdgControlPoint.h"
#include "OdgGroupItem.h"
#include "OdgPage.h"

DrawingUndoCommand::DrawingUndoCommand(const QString& text) : QUndoCommand(text), mPage(nullptr), mViewRect()
{
    // Nothing to do here.
}

//======================================================================================================================

void DrawingUndoCommand::storeView(DrawingWidget* drawing)
{
    Q_ASSERT(drawing != nullptr);
    mPage = drawing->currentPage();
    mViewRect = drawing->visibleRect();
}

void DrawingUndoCommand::restoreView(DrawingWidget* drawing)
{
    Q_ASSERT(drawing != nullptr);
    drawing->setCurrentPage(mPage);
    drawing->ensureVisible(mViewRect);
}

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

DrawingSetPropertyCommand::DrawingSetPropertyCommand(
    DrawingWidget* drawing, const QString& name, const QVariant& value) : QUndoCommand("Set Property"),
    mDrawing(drawing), mName(name), mNewValue(value), mOldValue()
{
    Q_ASSERT(mDrawing != nullptr);
    mOldValue = mDrawing->property(mName.toUtf8().data());
}

//======================================================================================================================

void DrawingSetPropertyCommand::redo()
{
    mDrawing->setProperty(mName, mNewValue);
}

void DrawingSetPropertyCommand::undo()
{
    mDrawing->setProperty(mName, mOldValue);
}

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

DrawingInsertPageCommand::DrawingInsertPageCommand(DrawingWidget* drawing, OdgPage* page, int index) :
    QUndoCommand("Insert Page"), mDrawing(drawing), mPage(page), mIndex(index), mUndone(true)
{
    Q_ASSERT(mDrawing != nullptr);
    Q_ASSERT(mPage != nullptr);
}

DrawingInsertPageCommand::~DrawingInsertPageCommand()
{
    if (mUndone) delete mPage;
}

//======================================================================================================================

void DrawingInsertPageCommand::redo()
{
    mUndone = false;
    mDrawing->insertPage(mIndex, mPage);
}

void DrawingInsertPageCommand::undo()
{
    mDrawing->removePage(mPage);
    mUndone = true;
}

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

DrawingRemovePageCommand::DrawingRemovePageCommand(DrawingWidget* drawing, OdgPage* page) : QUndoCommand("Remove Page"),
    mDrawing(drawing), mPage(page), mIndex(-1), mUndone(true)
{
    Q_ASSERT(mDrawing != nullptr);
    Q_ASSERT(mPage != nullptr);
    mIndex = mDrawing->pages().indexOf(mPage);
}

DrawingRemovePageCommand::~DrawingRemovePageCommand()
{
    if (!mUndone) delete mPage;
}

//======================================================================================================================

void DrawingRemovePageCommand::redo()
{
    mUndone = false;
    mDrawing->removePage(mPage);
}

void DrawingRemovePageCommand::undo()
{
    mDrawing->insertPage(mIndex, mPage);
    mUndone = true;
}

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

DrawingMovePageCommand::DrawingMovePageCommand(DrawingWidget* drawing, OdgPage* page, int newIndex) :
    QUndoCommand("Move Page"), mDrawing(drawing), mPage(page), mNewIndex(newIndex), mOldIndex(-1)
{
    Q_ASSERT(mDrawing != nullptr);
    Q_ASSERT(mPage != nullptr);
    mOldIndex = mDrawing->pages().indexOf(mPage);
}

//======================================================================================================================

void DrawingMovePageCommand::redo()
{
    mDrawing->removePage(mPage);
    mDrawing->insertPage(mNewIndex, mPage);
}

void DrawingMovePageCommand::undo()
{
    mDrawing->removePage(mPage);
    mDrawing->insertPage(mOldIndex, mPage);
}

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

DrawingSetPagePropertyCommand::DrawingSetPagePropertyCommand(
    DrawingWidget* drawing, OdgPage* page, const QString& name, const QVariant& value) :
    QUndoCommand("Set Page Property"), mDrawing(drawing), mPage(page), mName(name), mNewValue(value), mOldValue()
{
    Q_ASSERT(mDrawing != nullptr);
    Q_ASSERT(mPage != nullptr);
    mOldValue = mPage->property(mName.toUtf8().data());
}

//======================================================================================================================

void DrawingSetPagePropertyCommand::redo()
{
    mDrawing->setPageProperty(mPage, mName, mNewValue);
}

void DrawingSetPagePropertyCommand::undo()
{
    mDrawing->setPageProperty(mPage, mName, mOldValue);
}

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

DrawingAddItemsCommand::DrawingAddItemsCommand(DrawingWidget* drawing, OdgPage* page, const QList<OdgItem*>& items,
                                               bool place) :
    DrawingUndoCommand("Add Items"), mDrawing(drawing), mPage(page), mItems(items), mPlace(place), mUndone(true)
{
    Q_ASSERT(mDrawing != nullptr);
    storeView(mDrawing);
}

DrawingAddItemsCommand::~DrawingAddItemsCommand()
{
    if (mUndone) qDeleteAll(mItems);
}

//======================================================================================================================

void DrawingAddItemsCommand::redo()
{
    restoreView(mDrawing);
    mDrawing->addItems(mPage, mItems, mPlace);
    mUndone = false;
}

void DrawingAddItemsCommand::undo()
{
    restoreView(mDrawing);
    mDrawing->removeItems(mPage, mItems);
    mUndone = true;
}

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

DrawingRemoveItemsCommand::DrawingRemoveItemsCommand(DrawingWidget* drawing, OdgPage* page,
                                                     const QList<OdgItem*>& items) :
    DrawingUndoCommand("Remove Items"), mDrawing(drawing), mPage(page), mItems(items), mIndices(), mUndone(true)
{
    Q_ASSERT(mDrawing != nullptr);
    storeView(mDrawing);

    // Determine index of each item within the page for when we want to undo this command
    if (mPage)
    {
        QList<OdgItem*> pageItems = mPage->items();
        int index = -1;
        for(auto& item : mItems)
        {
            index = pageItems.indexOf(item);
            if (index >= 0) mIndices[item] = index;
        }
    }
}

DrawingRemoveItemsCommand::~DrawingRemoveItemsCommand()
{
    if (!mUndone) qDeleteAll(mItems);
}

//======================================================================================================================

void DrawingRemoveItemsCommand::redo()
{
    restoreView(mDrawing);
    mDrawing->removeItems(mPage, mItems);
    mUndone = false;
}

void DrawingRemoveItemsCommand::undo()
{
    restoreView(mDrawing);
    mDrawing->insertItems(mPage, mItems, mIndices, true);
    mUndone = true;
}

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

DrawingReorderItemsCommand::DrawingReorderItemsCommand(DrawingWidget* drawing, OdgPage* page,
                                                       const QList<OdgItem*>& items) :
    DrawingUndoCommand("Reorder Items"), mDrawing(drawing), mPage(page), mItemsOrdered(items), mOriginalItemsOrdered(),
    mSelectedItems()
{
    Q_ASSERT(mDrawing != nullptr);
    storeView(mDrawing);

    if (mPage) mOriginalItemsOrdered = mPage->items();
    mSelectedItems = mDrawing->selectedItems();
}

//======================================================================================================================

void DrawingReorderItemsCommand::redo()
{
    restoreView(mDrawing);
    mDrawing->reorderItems(mPage, mItemsOrdered);
    mDrawing->setSelectedItems(mSelectedItems);
}

void DrawingReorderItemsCommand::undo()
{
    restoreView(mDrawing);
    mDrawing->reorderItems(mPage, mOriginalItemsOrdered);
    mDrawing->setSelectedItems(mSelectedItems);
}

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

DrawingMoveItemsCommand::DrawingMoveItemsCommand(DrawingWidget* drawing, const QList<OdgItem*>& items,
                                                 const QHash<OdgItem*,QPointF>& positions, bool place) :
    DrawingUndoCommand("Move Items"),
    mDrawing(drawing), mItems(items), mPositions(positions), mOriginalPositions(), mPlace(place)
{
    Q_ASSERT(mDrawing != nullptr);
    storeView(mDrawing);

    for(auto& item : items) mOriginalPositions.insert(item, item->position());
}

//======================================================================================================================

int DrawingMoveItemsCommand::id() const
{
    return MoveCommandId;
}

bool DrawingMoveItemsCommand::mergeWith(const QUndoCommand* command)
{
    if (command && command->id() == MoveCommandId)
    {
        const DrawingMoveItemsCommand* moveCommand = dynamic_cast<const DrawingMoveItemsCommand*>(command);
        if (moveCommand && mDrawing == moveCommand->mDrawing && mItems == moveCommand->mItems && !mPlace)
        {
            mPositions = moveCommand->mPositions;
            mPlace = moveCommand->mPlace;
            return true;
        }
    }

    return false;
}

//======================================================================================================================

void DrawingMoveItemsCommand::redo()
{
    restoreView(mDrawing);
    mDrawing->setSelectedItems(mItems);
    mDrawing->moveItems(mItems, mPositions, mPlace);
}

void DrawingMoveItemsCommand::undo()
{
    restoreView(mDrawing);
    mDrawing->setSelectedItems(mItems);
    mDrawing->moveItems(mItems, mOriginalPositions, mPlace);
}

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

DrawingResizeItemCommand::DrawingResizeItemCommand(DrawingWidget* drawing, OdgControlPoint* point,
                                                   const QPointF& position, bool snapTo45Degrees, bool place) :
    DrawingUndoCommand("Resize Item"),
    mDrawing(drawing), mControlPoint(point), mPosition(position), mOriginalPosition(),
    mSnapTo45Degrees(snapTo45Degrees), mPlace(place)
{
    Q_ASSERT(mDrawing != nullptr);
    storeView(mDrawing);

    if (point && point->item()) mOriginalPosition = point->item()->mapToScene(point->position());
}

//======================================================================================================================

int DrawingResizeItemCommand::id() const
{
    return ResizeCommandId;
}

bool DrawingResizeItemCommand::mergeWith(const QUndoCommand* command)
{
    if (command && command->id() == ResizeCommandId)
    {
        const DrawingResizeItemCommand* resizeCommand = dynamic_cast<const DrawingResizeItemCommand*>(command);
        if (resizeCommand && mDrawing == resizeCommand->mDrawing && mControlPoint == resizeCommand->mControlPoint &&
            !mPlace)
        {
            mPosition = resizeCommand->mPosition;
            mSnapTo45Degrees = resizeCommand->mSnapTo45Degrees;
            mPlace = resizeCommand->mPlace;
            return true;
        }
    }

    return false;
}

//======================================================================================================================

void DrawingResizeItemCommand::redo()
{
    if (mControlPoint && mControlPoint->item())
    {
        restoreView(mDrawing);
        mDrawing->setSelectedItems(QList<OdgItem*>(1, mControlPoint->item()));
        mDrawing->resizeItem(mControlPoint, mPosition, mSnapTo45Degrees, true, mPlace);
    }
}

void DrawingResizeItemCommand::undo()
{
    if (mControlPoint && mControlPoint->item())
    {
        restoreView(mDrawing);
        mDrawing->setSelectedItems(QList<OdgItem*>(1, mControlPoint->item()));
        mDrawing->resizeItem(mControlPoint, mOriginalPosition, mSnapTo45Degrees, true, mPlace);
    }
}

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

DrawingResizeItem2Command::DrawingResizeItem2Command(DrawingWidget* drawing, OdgControlPoint* point1, const QPointF& p1,
                                                     OdgControlPoint* point2, const QPointF& p2, bool place) :
    DrawingUndoCommand("Resize Item"),
    mDrawing(drawing), mControlPoint1(point1), mPosition1(p1), mOriginalPosition1(),
    mControlPoint2(point2), mPosition2(p2), mOriginalPosition2(), mPlace(place)
{
    Q_ASSERT(mDrawing != nullptr);
    storeView(mDrawing);

    if (point1 && point1->item()) mOriginalPosition1 = point1->item()->mapToScene(point1->position());
    if (point2 && point2->item()) mOriginalPosition2 = point2->item()->mapToScene(point2->position());
}

//======================================================================================================================

void DrawingResizeItem2Command::redo()
{
    if (mControlPoint1 && mControlPoint1->item() && mControlPoint2 && mControlPoint2->item())
    {
        restoreView(mDrawing);
        mDrawing->setSelectedItems(QList<OdgItem*>(1, mControlPoint1->item()));
        mDrawing->resizeItem2(mControlPoint1, mPosition1, mControlPoint2, mPosition2, mPlace);
    }
}

void DrawingResizeItem2Command::undo()
{
    if (mControlPoint1 && mControlPoint1->item() && mControlPoint2 && mControlPoint2->item())
    {
        restoreView(mDrawing);
        mDrawing->setSelectedItems(QList<OdgItem*>(1, mControlPoint1->item()));
        mDrawing->resizeItem2(mControlPoint1, mOriginalPosition1, mControlPoint2, mOriginalPosition2, mPlace);
    }
}

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

DrawingRotateItemsCommand::DrawingRotateItemsCommand(DrawingWidget* drawing, const QList<OdgItem*>& items,
                                                     const QPointF& position) :
    DrawingUndoCommand("Rotate Items"), mDrawing(drawing), mItems(items), mPosition(position)
{
    Q_ASSERT(mDrawing != nullptr);
    storeView(mDrawing);
}

//======================================================================================================================

void DrawingRotateItemsCommand::redo()
{
    restoreView(mDrawing);
    mDrawing->setSelectedItems(mItems);
    mDrawing->rotateItems(mItems, mPosition);
}

void DrawingRotateItemsCommand::undo()
{
    restoreView(mDrawing);
    mDrawing->setSelectedItems(mItems);
    mDrawing->rotateBackItems(mItems, mPosition);
}

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

DrawingRotateBackItemsCommand::DrawingRotateBackItemsCommand(DrawingWidget* drawing, const QList<OdgItem*>& items,
                                                             const QPointF& position) :
    DrawingUndoCommand("Rotate Back Items"), mDrawing(drawing), mItems(items), mPosition(position)
{
    Q_ASSERT(mDrawing != nullptr);
    storeView(mDrawing);
}

//======================================================================================================================

void DrawingRotateBackItemsCommand::redo()
{
    restoreView(mDrawing);
    mDrawing->setSelectedItems(mItems);
    mDrawing->rotateBackItems(mItems, mPosition);
}

void DrawingRotateBackItemsCommand::undo()
{
    restoreView(mDrawing);
    mDrawing->setSelectedItems(mItems);
    mDrawing->rotateItems(mItems, mPosition);
}

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

DrawingFlipItemsHorizontalCommand::DrawingFlipItemsHorizontalCommand(DrawingWidget* drawing, const QList<OdgItem*>& items,
                                                     const QPointF& position) :
    DrawingUndoCommand("Flip Items Horizontal"), mDrawing(drawing), mItems(items), mPosition(position)
{
    Q_ASSERT(mDrawing != nullptr);
    storeView(mDrawing);
}

//======================================================================================================================

void DrawingFlipItemsHorizontalCommand::redo()
{
    restoreView(mDrawing);
    mDrawing->setSelectedItems(mItems);
    mDrawing->flipItemsHorizontal(mItems, mPosition);
}

void DrawingFlipItemsHorizontalCommand::undo()
{
    restoreView(mDrawing);
    mDrawing->setSelectedItems(mItems);
    mDrawing->flipItemsHorizontal(mItems, mPosition);
}

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

DrawingFlipItemsVerticalCommand::DrawingFlipItemsVerticalCommand(DrawingWidget* drawing, const QList<OdgItem*>& items,
                                                     const QPointF& position) :
    DrawingUndoCommand("Flip Items Vertical"), mDrawing(drawing), mItems(items), mPosition(position)
{
    Q_ASSERT(mDrawing != nullptr);
    storeView(mDrawing);
}

//======================================================================================================================

void DrawingFlipItemsVerticalCommand::redo()
{
    restoreView(mDrawing);
    mDrawing->setSelectedItems(mItems);
    mDrawing->flipItemsVertical(mItems, mPosition);
}

void DrawingFlipItemsVerticalCommand::undo()
{
    restoreView(mDrawing);
    mDrawing->setSelectedItems(mItems);
    mDrawing->flipItemsVertical(mItems, mPosition);
}

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

DrawingGroupItemsCommand::DrawingGroupItemsCommand(DrawingWidget* drawing, OdgPage* page, const QList<OdgItem*>& items) :
    DrawingUndoCommand("Group"), mDrawing(drawing), mPage(page), mItemsToRemove(items), mItemsToAdd(), mUndone(true)
{
    Q_ASSERT(mDrawing != nullptr);
    Q_ASSERT(mPage != nullptr);
    storeView(mDrawing);

    // Create the group item and put its position equal to the center of all the items.  Then adjust each
    // item's position accordingly
    OdgGroupItem* groupItem = new OdgGroupItem();

    QRectF itemsRect;
    for(auto& item : items)
        itemsRect = itemsRect.united(item->mapToScene(item->boundingRect()).normalized());
    groupItem->setPosition(itemsRect.center());

    const QList<OdgItem*> groupChildItems = OdgItem::copyItems(mItemsToRemove);
    for(auto& item : groupChildItems)
        item->setPosition(groupItem->mapFromScene(item->position()));
    groupItem->setItems(groupChildItems);

    mItemsToAdd.append(groupItem);
}

DrawingGroupItemsCommand::~DrawingGroupItemsCommand()
{
    if (!mUndone) qDeleteAll(mItemsToRemove);
    else qDeleteAll(mItemsToAdd);
}

//======================================================================================================================

void DrawingGroupItemsCommand::redo()
{
    mUndone = false;
    restoreView(mDrawing);
    mDrawing->removeItems(mPage, mItemsToRemove);
    mDrawing->addItems(mPage, mItemsToAdd, false);
    mDrawing->setSelectedItems(mItemsToAdd);
}

void DrawingGroupItemsCommand::undo()
{
    restoreView(mDrawing);
    mDrawing->removeItems(mPage, mItemsToAdd);
    mDrawing->addItems(mPage, mItemsToRemove, false);
    mDrawing->setSelectedItems(mItemsToRemove);
    mUndone = true;
}

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

DrawingUngroupItemsCommand::DrawingUngroupItemsCommand(DrawingWidget* drawing, OdgPage* page, OdgGroupItem* groupItem) :
    DrawingUndoCommand("Ungroup"), mDrawing(drawing), mPage(page), mItemsToRemove(), mItemsToAdd(), mUndone(true)
{
    Q_ASSERT(mDrawing != nullptr);
    Q_ASSERT(mPage != nullptr);
    Q_ASSERT(groupItem != nullptr);
    storeView(mDrawing);

    mItemsToRemove.append(groupItem);

    mItemsToAdd = OdgItem::copyItems(groupItem->items());
    for(auto& item : qAsConst(mItemsToAdd))
    {
        // Apply the group's position/transform to each item
        item->setPosition(groupItem->mapToScene(item->position()));
        item->setRotation(item->rotation() + groupItem->rotation());
        if (groupItem->isFlipped()) item->setFlipped(!item->isFlipped());
    }
}

DrawingUngroupItemsCommand::~DrawingUngroupItemsCommand()
{
    if (!mUndone) qDeleteAll(mItemsToRemove);
    else qDeleteAll(mItemsToAdd);
}

//======================================================================================================================

void DrawingUngroupItemsCommand::redo()
{
    mUndone = false;
    restoreView(mDrawing);
    mDrawing->removeItems(mPage, mItemsToRemove);
    mDrawing->addItems(mPage, mItemsToAdd, false);
    mDrawing->setSelectedItems(mItemsToAdd);
}

void DrawingUngroupItemsCommand::undo()
{
    restoreView(mDrawing);
    mDrawing->removeItems(mPage, mItemsToAdd);
    mDrawing->addItems(mPage, mItemsToRemove, false);
    mDrawing->setSelectedItems(mItemsToRemove);
    mUndone = true;
}

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

DrawingInsertPointCommand::DrawingInsertPointCommand(DrawingWidget* drawing, OdgItem* item, int index,
                                                     OdgControlPoint* point) :
    DrawingUndoCommand("Insert Point"), mDrawing(drawing), mItem(item), mIndex(index), mPoint(point), mUndone(true)
{
    Q_ASSERT(mDrawing != nullptr);
    storeView(mDrawing);
}

DrawingInsertPointCommand::~DrawingInsertPointCommand()
{
    if (mUndone) delete mPoint;
}

//======================================================================================================================

void DrawingInsertPointCommand::redo()
{
    mUndone = false;
    restoreView(mDrawing);
    mDrawing->setSelectedItems(QList<OdgItem*>(1, mItem));
    mDrawing->insertItemPoint(mItem, mIndex, mPoint);
}

void DrawingInsertPointCommand::undo()
{
    restoreView(mDrawing);
    mDrawing->setSelectedItems(QList<OdgItem*>(1, mItem));
    mDrawing->removeItemPoint(mItem, mPoint);
    mUndone = true;
}

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

DrawingRemovePointCommand::DrawingRemovePointCommand(DrawingWidget* drawing, OdgItem* item, OdgControlPoint* point) :
    DrawingUndoCommand("Remove Point"), mDrawing(drawing), mItem(item), mIndex(-1), mPoint(point), mUndone(true)
{
    Q_ASSERT(mDrawing != nullptr);
    storeView(mDrawing);

    if (mItem) mIndex = mItem->controlPoints().indexOf(mPoint);
}

DrawingRemovePointCommand::~DrawingRemovePointCommand()
{
    if (!mUndone) delete mPoint;
}

//======================================================================================================================

void DrawingRemovePointCommand::redo()
{
    mUndone = false;
    restoreView(mDrawing);
    mDrawing->setSelectedItems(QList<OdgItem*>(1, mItem));
    mDrawing->removeItemPoint(mItem, mPoint);
}

void DrawingRemovePointCommand::undo()
{
    restoreView(mDrawing);
    mDrawing->setSelectedItems(QList<OdgItem*>(1, mItem));
    mDrawing->insertItemPoint(mItem, mIndex, mPoint);
    mUndone = true;
}

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

DrawingSetItemsPropertyCommand::DrawingSetItemsPropertyCommand(DrawingWidget* drawing, const QList<OdgItem*>& items,
                                                               const QString& name, const QVariant& value) :
    DrawingUndoCommand("Set Items' Property"),
    mDrawing(drawing), mItems(items), mName(name), mNewValue(value), mOldValues()
{
    Q_ASSERT(mDrawing != nullptr);
    storeView(mDrawing);

    for(auto& item : items) mOldValues.insert(item, item->property(name));
}

//======================================================================================================================

int DrawingSetItemsPropertyCommand::id() const
{
    return SetItemsPropertyCommandId;
}

bool DrawingSetItemsPropertyCommand::mergeWith(const QUndoCommand* command)
{
    if (command && command->id() == SetItemsPropertyCommandId)
    {
        const DrawingSetItemsPropertyCommand* propertyCommand =
            dynamic_cast<const DrawingSetItemsPropertyCommand*>(command);
        if (propertyCommand && mDrawing == propertyCommand->mDrawing && mItems == propertyCommand->mItems)
        {
            if (mItems.size() == 1 && mName == "caption" && propertyCommand->mName == "caption")
            {
                mNewValue = propertyCommand->mNewValue;
                return true;
            }
        }
    }

    return false;
}

//======================================================================================================================

void DrawingSetItemsPropertyCommand::redo()
{
    restoreView(mDrawing);
    mDrawing->setSelectedItems(mItems);
    mDrawing->setItemsProperty(mItems, mName, mNewValue);
}

void DrawingSetItemsPropertyCommand::undo()
{
    restoreView(mDrawing);
    mDrawing->setSelectedItems(mItems);
    mDrawing->setItemsProperty(mItems, mName, mOldValues);
}
