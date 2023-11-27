// File: DrawingWidget.cpp
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

#include "DrawingWidget.h"
#include <QActionGroup>
#include <QContextMenuEvent>
#include <QMenu>

DrawingWidget::DrawingWidget() : QAbstractScrollArea(), OdgDrawing(),
    mModeActionGroup(nullptr), mContextMenu(nullptr)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setMouseTracking(true);

    createActions();
    createContextMenu();
}

DrawingWidget::~DrawingWidget()
{
    clear();
}

//======================================================================================================================

void DrawingWidget::createActions()
{
    mModeActionGroup = new QActionGroup(this);
    connect(mModeActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(setModeFromAction(QAction*)));

    addNormalAction("Undo", this, SLOT(undo()), ":/icons/edit-undo.png", "Ctrl+Z");
    addNormalAction("Redo", this, SLOT(redo()), ":/icons/edit-redo.png", "Ctrl+Shift+Z");
    addNormalAction("Cut", this, SLOT(cut()), ":/icons/edit-cut.png", "Ctrl+X");
    addNormalAction("Copy", this, SLOT(copy()), ":/icons/edit-copy.png", "Ctrl+C");
    addNormalAction("Paste", this, SLOT(paste()), ":/icons/edit-paste.png", "Ctrl+V");
    addNormalAction("Delete", this, SLOT(removeItems()), ":/icons/edit-delete.png", "Delete");
    addNormalAction("Select All", this, SLOT(selectAll()), ":/icons/edit-select-all.png", "Ctrl+A");
    addNormalAction("Select None", this, SLOT(selectNone()));

    addModeAction("Select Mode", ":/icons/edit-select.png", "Escape");
    addModeAction("Scroll Mode", ":/icons/transform-move.png");
    addModeAction("Zoom Mode", ":/icons/page-zoom.png");
    addModeAction("Place Line", ":/icons/draw-line.png");
    addModeAction("Place Curve", ":/icons/draw-curve.png");
    addModeAction("Place Polyline", ":/icons/draw-polyline.png");
    addModeAction("Place Rectangle", ":/icons/draw-rectangle.png");
    addModeAction("Place Ellipse", ":/icons/draw-ellipse.png");
    addModeAction("Place Polygon", ":/icons/draw-polygon.png");
    addModeAction("Place Text", ":/icons/draw-text.png");
    addModeAction("Place Text Rectangle", ":/icons/text-rectangle.png");
    addModeAction("Place Text Ellipse", ":/icons/text-ellipse.png");

    addNormalAction("Rotate", this, SLOT(rotate()), ":/icons/object-rotate-right.png", "R");
    addNormalAction("Rotate Back", this, SLOT(rotateBack()), ":/icons/object-rotate-left.png", "Shift+R");
    addNormalAction("Flip Horizontal", this, SLOT(flipHorizontal()), ":/icons/object-flip-horizontal.png", "F");
    addNormalAction("Flip Vertical", this, SLOT(flipVertical()), ":/icons/object-flip-vertical.png", "Shift+F");
    addNormalAction("Bring Forward", this, SLOT(bringForward()), ":/icons/object-bring-forward.png");
    addNormalAction("Send Backward", this, SLOT(sendBackward()), ":/icons/object-send-backward.png");
    addNormalAction("Bring to Front", this, SLOT(bringToFront()), ":/icons/object-bring-to-front.png");
    addNormalAction("Send to Back", this, SLOT(sendToBack()), ":/icons/object-send-to-back.png");
    addNormalAction("Group", this, SLOT(group()), ":/icons/merge.png", "Ctrl+G");
    addNormalAction("Ungroup", this, SLOT(ungroup()), ":/icons/split.png");
    addNormalAction("Insert Point", this, SLOT(insertPoint()), ":/icons/format-add-node.png");
    addNormalAction("Remove Point", this, SLOT(removePoint()), ":/icons/format-remove-node.png");

    addNormalAction("Insert Page", this, SLOT(insertPage()), ":/icons/archive-insert.png");
    addNormalAction("Duplicate Page", this, SLOT(duplicatePage()), ":/icons/edit-copy.png");
    addNormalAction("Remove Page", this, SLOT(removePage()), ":/icons/archive-remove.png");

    addNormalAction("Zoom In", this, SLOT(zoomIn()), ":/icons/zoom-in.png", ".");
    addNormalAction("Zoom Out", this, SLOT(zoomOut()), ":/icons/zoom-out.png", ",");
    addNormalAction("Zoom Fit", this, SLOT(zoomFit()), ":/icons/zoom-fit-best.png", "/");
}

void DrawingWidget::createContextMenu()
{
    QList<QAction*> actions = this->actions();

    mContextMenu = new QMenu(this);
    mContextMenu->addAction(actions[UndoAction]);
    mContextMenu->addAction(actions[RedoAction]);
    mContextMenu->addSeparator();
    mContextMenu->addAction(actions[CutAction]);
    mContextMenu->addAction(actions[CopyAction]);
    mContextMenu->addAction(actions[PasteAction]);
    mContextMenu->addAction(actions[DeleteAction]);
    mContextMenu->addSeparator();
    mContextMenu->addAction(actions[InsertPointAction]);
    mContextMenu->addAction(actions[RemovePointAction]);
    mContextMenu->addSeparator();
    mContextMenu->addAction(actions[RotateAction]);
    mContextMenu->addAction(actions[RotateBackAction]);
    mContextMenu->addAction(actions[FlipHorizontalAction]);
    mContextMenu->addAction(actions[FlipVerticalAction]);
    mContextMenu->addSeparator();
    mContextMenu->addAction(actions[BringForwardAction]);
    mContextMenu->addAction(actions[SendBackwardAction]);
    mContextMenu->addAction(actions[BringToFrontAction]);
    mContextMenu->addAction(actions[SendToBackAction]);
    mContextMenu->addSeparator();
    mContextMenu->addAction(actions[GroupAction]);
    mContextMenu->addAction(actions[UngroupAction]);
    mContextMenu->addSeparator();
    mContextMenu->addAction(actions[ZoomInAction]);
    mContextMenu->addAction(actions[ZoomOutAction]);
    mContextMenu->addAction(actions[ZoomFitAction]);
}

void DrawingWidget::addNormalAction(const QString& text, const QObject* slotObject, const char* slotFunction,
                                 const QString& iconPath, const QString& keySequence)
{
    QAction* action = new QAction(text, this);
    if (!iconPath.isEmpty())
        action->setIcon(QIcon(iconPath));
    if (!keySequence.isEmpty())
        action->setShortcut(QKeySequence(keySequence));
    QWidget::addAction(action);

    connect(action, SIGNAL(triggered(bool)), slotObject, slotFunction);
}


void DrawingWidget::addModeAction(const QString& text, const QString& iconPath, const QString& keySequence)
{
    QAction* action = new QAction(text, mModeActionGroup);
    if (!iconPath.isEmpty())
        action->setIcon(QIcon(iconPath));
    if (!keySequence.isEmpty())
        action->setShortcut(QKeySequence(keySequence));
    QWidget::addAction(action);

    action->setCheckable(true);
    if (mModeActionGroup->actions().size() == 1)
        action->setChecked(true);
}

//======================================================================================================================

void DrawingWidget::createNew()
{

}

bool DrawingWidget::load(const QString& fileName)
{
    return OdgDrawing::load(fileName);
}

bool DrawingWidget::save(const QString& fileName)
{
    return isClean();
}

void DrawingWidget::clear()
{

}

bool DrawingWidget::isClean() const
{
    return true;
}

//======================================================================================================================

void DrawingWidget::setScale(double scale)
{

}

void DrawingWidget::zoomIn()
{

}

void DrawingWidget::zoomOut()
{

}

void DrawingWidget::zoomFit()
{

}

void DrawingWidget::zoomFitAll()
{

}

//======================================================================================================================

void DrawingWidget::insertPage()
{

}

void DrawingWidget::duplicatePage()
{

}

void DrawingWidget::removePage()
{

}

//======================================================================================================================

void DrawingWidget::undo()
{

}

void DrawingWidget::redo()
{

}

//======================================================================================================================

void DrawingWidget::removeItems()
{

}

//======================================================================================================================

void DrawingWidget::cut()
{

}

void DrawingWidget::copy()
{

}

void DrawingWidget::paste()
{

}

//======================================================================================================================

void DrawingWidget::selectAll()
{

}

void DrawingWidget::selectNone()
{

}

//======================================================================================================================

void DrawingWidget::rotate()
{

}

void DrawingWidget::rotateBack()
{

}

void DrawingWidget::flipHorizontal()
{

}

void DrawingWidget::flipVertical()
{

}

//======================================================================================================================

void DrawingWidget::bringForward()
{

}

void DrawingWidget::sendBackward()
{

}

void DrawingWidget::bringToFront()
{

}

void DrawingWidget::sendToBack()
{

}

//======================================================================================================================

void DrawingWidget::group()
{

}

void DrawingWidget::ungroup()
{

}

//======================================================================================================================

void DrawingWidget::insertPoint()
{

}

void DrawingWidget::removePoint()
{

}

//======================================================================================================================

void DrawingWidget::contextMenuEvent(QContextMenuEvent* event)
{
    mContextMenu->popup(event->globalPos());
}

//======================================================================================================================

void DrawingWidget::setModeFromAction(QAction* action)
{

}
