// File: PagesWidget.cpp
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

#include "PagesWidget.h"
#include <QContextMenuEvent>
#include <QMenu>

PagesWidget::PagesWidget() : QListWidget(), mContextMenu(nullptr)
{
    createContextMenu();
}

//======================================================================================================================

void PagesWidget::createContextMenu()
{
    QAction* insertAction = addAction("Insert", this, SLOT(insertPage()), ":/icons/archive-insert.png");
    QAction* duplicateAction = addAction("Duplicate", this, SLOT(duplicatePage()), ":/icons/edit-copy.png");
    QAction* renameAction = addAction("Rename...", this, SLOT(renamePage()));
    QAction* removeAction = addAction("Remove", this, SLOT(removePage()), ":/icons/archive-remove.png");

    mContextMenu = new QMenu(this);
    mContextMenu->addAction(insertAction);
    mContextMenu->addAction(duplicateAction);
    mContextMenu->addAction(renameAction);
    mContextMenu->addAction(removeAction);
}

QAction* PagesWidget::addAction(const QString& text, const QObject* slotObject, const char* slotFunction,
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

void PagesWidget::insertPage()
{

}

void PagesWidget::duplicatePage()
{

}

void PagesWidget::renamePage()
{

}

void PagesWidget::removePage()
{

}

//======================================================================================================================

void PagesWidget::contextMenuEvent(QContextMenuEvent* event)
{
    mContextMenu->popup(event->globalPos());
}
