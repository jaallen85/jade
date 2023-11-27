// File: StylesWidget.cpp
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

#include "StylesWidget.h"
#include <QContextMenuEvent>
#include <QMenu>

StylesWidget::StylesWidget() : QListWidget(), mContextMenu(nullptr)
{
    createContextMenu();
}

//======================================================================================================================

void StylesWidget::createContextMenu()
{
    QAction* newAction = addAction("New...", this, SLOT(insertStyle()), ":/icons/archive-insert.png");
    QAction* editAction = addAction("Edit...", this, SLOT(editStyle()), ":/icons/document-edit.png");
    QAction* removeAction = addAction("Remove", this, SLOT(removeStyle()), ":/icons/archive-remove.png");

    mContextMenu = new QMenu(this);
    mContextMenu->addAction(newAction);
    mContextMenu->addAction(editAction);
    mContextMenu->addAction(removeAction);
}

QAction* StylesWidget::addAction(const QString& text, const QObject* slotObject, const char* slotFunction,
                                 const QString& iconPath)
{
    QAction* action = new QAction(text, this);
    connect(action, SIGNAL(triggered(bool)), slotObject, slotFunction);

    if (!iconPath.isEmpty())
        action->setIcon(QIcon(iconPath));

    QListWidget::addAction(action);
    return action;
}

//======================================================================================================================

void StylesWidget::insertStyle()
{

}

void StylesWidget::editStyle()
{

}

void StylesWidget::removeStyle()
{

}

//======================================================================================================================

void StylesWidget::contextMenuEvent(QContextMenuEvent* event)
{
    mContextMenu->popup(event->globalPos());
}
