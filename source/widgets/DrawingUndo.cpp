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
    DrawingWidget* drawing, OdgPage* page, const QString& name, const QVariant& value) : QUndoCommand("Move Page"),
    mDrawing(drawing), mPage(page), mName(name), mNewValue(value), mOldValue()
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
