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
class OdgItem;
class OdgPage;

class DrawingUndoCommand : public QUndoCommand
{
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

#endif
