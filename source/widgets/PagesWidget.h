// File: PagesWidget.h
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

#ifndef PAGESWIDGET_H
#define PAGESWIDGET_H

#include <QListWidget>

class QMenu;
class DrawingWidget;
class OdgPage;

class PagesWidget : public QListWidget
{
    Q_OBJECT

private:
    DrawingWidget* mDrawing;
    QMenu* mContextMenu;

public:
    PagesWidget(DrawingWidget* drawing);

    QSize sizeHint() const override;

private:
    void createContextMenu();
    QAction* addAction(const QString& text, const QObject* slotObject, const char* slotFunction,
                       const QString& iconPath = QString());

public slots:
    void renamePage();

private:
    void contextMenuEvent(QContextMenuEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private slots:
    void insertItemForPage(OdgPage* page, int index);
    void removeItemForPage(OdgPage* page, int index);
    void updateCurrentRow(int index);
    void updateCurrentItemText(const QString& name, const QVariant& value);

    void setCurrentPageIndex(int index);
    void setCurrentPageName(QListWidgetItem* item);
};

#endif
