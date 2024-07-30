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
#include "DrawingWidget.h"
#include "OdgPage.h"
#include <QContextMenuEvent>
#include <QMenu>

PagesWidget::PagesWidget(DrawingWidget* drawing) : QListWidget(), mDrawing(drawing), mContextMenu(nullptr)
{
    Q_ASSERT(mDrawing != nullptr);

    setAlternatingRowColors(true);
    setSelectionMode(SingleSelection);
    setDragDropMode(InternalMove);

    // Connect signals/slots
    connect(mDrawing, SIGNAL(pageInserted(OdgPage*,int)), this, SLOT(insertItemForPage(OdgPage*,int)));
    connect(mDrawing, SIGNAL(pageRemoved(OdgPage*,int)), this, SLOT(removeItemForPage(OdgPage*,int)));
    connect(mDrawing, SIGNAL(currentPageIndexChanged(int)), this, SLOT(updateCurrentRow(int)));
    connect(mDrawing, SIGNAL(currentPagePropertyChanged(QString,QVariant)),
            this, SLOT(updateCurrentItemText(QString,QVariant)));

    connect(this, SIGNAL(currentRowChanged(int)), this, SLOT(setCurrentPageIndex(int)));
    connect(this, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(setCurrentPageName(QListWidgetItem*)));

    // Create context menu
    createContextMenu();
}

//======================================================================================================================

QSize PagesWidget::sizeHint() const
{
    return QSize(200, -1);
}

//======================================================================================================================

void PagesWidget::createContextMenu()
{
    QAction* insertAction = addAction("Insert", mDrawing, SLOT(insertPage()), ":/icons/oxygen/archive-insert.png");
    QAction* duplicateAction = addAction("Duplicate", mDrawing, SLOT(duplicatePage()), ":/icons/oxygen/edit-copy.png");
    QAction* renameAction = addAction("Rename...", this, SLOT(renamePage()));
    QAction* removeAction = addAction("Remove", mDrawing, SLOT(removePage()), ":/icons/oxygen/archive-remove.png");

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

void PagesWidget::renamePage()
{
    if (currentItem()) editItem(currentItem());
}

//======================================================================================================================

void PagesWidget::contextMenuEvent(QContextMenuEvent* event)
{
    mContextMenu->popup(event->globalPos());
}

void PagesWidget::dropEvent(QDropEvent* event)
{
    Q_ASSERT(event != nullptr);
    const QListWidgetItem* eventItem = itemAt(event->position().toPoint());
    const QList<QListWidgetItem*> selectedItems = this->selectedItems();
    if (eventItem && !selectedItems.isEmpty())
    {
        const int currentIndex = row(selectedItems.first());
        int newIndex = row(eventItem);
        if (currentIndex != newIndex)
        {
            if (dropIndicatorPosition() == BelowItem && currentIndex > newIndex)
                newIndex = newIndex + 1;
            else if (dropIndicatorPosition() == AboveItem && currentIndex < newIndex)
                newIndex = newIndex - 1;

            if (currentIndex != newIndex)
                mDrawing->movePage(newIndex);
        }
    }
}

//======================================================================================================================

void PagesWidget::insertItemForPage(OdgPage* page, int index)
{
    if (page)
    {
        blockSignals(true);
        QListWidgetItem* newItem = new QListWidgetItem(page->name());
        newItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled);
        insertItem(index, newItem);
        blockSignals(false);
    }
}

void PagesWidget::removeItemForPage(OdgPage* page, int index)
{
    if (0 <= index && index < count())
    {
        blockSignals(true);
        delete takeItem(index);
        blockSignals(false);
    }
}

void PagesWidget::updateCurrentRow(int index)
{
    blockSignals(true);
    setCurrentRow(index);
    blockSignals(false);
}

void PagesWidget::updateCurrentItemText(const QString& name, const QVariant& value)
{
    if (currentItem() && name == "name" && value.canConvert<QString>())
    {
        blockSignals(true);
        currentItem()->setText(value.toString());
        blockSignals(false);
    }
}

//======================================================================================================================

void PagesWidget::setCurrentPageIndex(int index)
{
    mDrawing->setCurrentPageIndex(index);
}

void PagesWidget::setCurrentPageName(QListWidgetItem* item)
{
    if (item) mDrawing->renamePage(item->text());
}
