// File: DrawingWidget.h
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

#ifndef DRAWINGWIDGET_H
#define DRAWINGWIDGET_H

#include <QAbstractScrollArea>
#include "OdgDrawing.h"

class QActionGroup;
class QMenu;
class OdgItem;

class DrawingWidget : public QAbstractScrollArea, public OdgDrawing
{
    Q_OBJECT

public:
    enum ActionIndex { UndoAction, RedoAction, CutAction, CopyAction, PasteAction, DeleteAction,
                       SelectAllAction, SelectNoneAction, SelectModeAction, ScrollModeAction, ZoomModeAction,
                       PlaceLineAction, PlaceCurveAction, PlacePolylineAction, PlaceRectangleAction, PlaceEllipseAction,
                       PlacePolygonAction, PlaceTextAction, PlaceTextRectangleAction, PlaceTextEllipseAction,
                       RotateAction, RotateBackAction, FlipHorizontalAction, FlipVerticalAction,
                       BringForwardAction, SendBackwardAction, BringToFrontAction, SendToBackAction,
                       GroupAction, UngroupAction, InsertPointAction, RemovePointAction,
                       InsertPageAction, DuplicatePageAction, RemovePageAction,
                       ZoomInAction, ZoomOutAction, ZoomFitAction};

private:
    OdgPage* mCurrentPage;

    QTransform mTransform, mTransformInverse;

    QActionGroup* mModeActionGroup;
    QMenu* mContextMenu;

public:
    DrawingWidget();
    ~DrawingWidget();

private:
    void createActions();
    void createContextMenu();
    void addNormalAction(const QString& text, const QObject* slotObject, const char* slotFunction,
                         const QString& iconPath = QString(), const QString& keySequence = QString());
    void addModeAction(const QString& text, const QString& iconPath = QString(),
                       const QString& keySequence = QString());

public:
    double scale() const;
    QRectF visibleRect() const;
    QPointF mapToScene(const QPoint& position) const;
    QRectF mapToScene(const QRect& rect) const;
    QPoint mapFromScene(const QPointF& position) const;
    QRect mapFromScene(const QRectF& rect) const;

    void paint(QPainter& painter, bool isExport = false);

public:
    void createNew();
    bool load(const QString& fileName);
    bool save(const QString& fileName);
    void clear();
    bool isClean() const;

public slots:
    void setScale(double scale);
    void zoomIn();
    void zoomOut();
    void zoomFit();
    void zoomToRect(const QRectF& rect);
    void ensureVisible(const QRectF& rect);
    void centerOn(const QPointF& position);
    void mouseCursorOn(const QPointF& position);

    void undo();
    void redo();

    void insertPage();
    void duplicatePage();
    void removePage();

    void removeItems();

    void cut();
    void copy();
    void paste();

    void selectAll();
    void selectNone();

    void rotate();
    void rotateBack();
    void flipHorizontal();
    void flipVertical();

    void bringForward();
    void sendBackward();
    void bringToFront();
    void sendToBack();

    void group();
    void ungroup();

    void insertPoint();
    void removePoint();

signals:
    void scaleChanged(double scale);
    void modeTextChanged(const QString& modeText);
    void cleanTextChanged(const QString& modeText);
    void mouseInfoChanged(const QString& modeText);

private:
    void paintEvent(QPaintEvent* event) override;
    void drawBackground(QPainter& painter, bool drawBorder, bool drawGrid);
    void drawGridLines(QPainter& painter, const QColor& color, int spacing);
    void drawItems(QPainter& painter, const QList<OdgItem*>& items);

    void resizeEvent(QResizeEvent* event) override;
    void updateTransformAndScrollBars(double scale = 0.0);

    void wheelEvent(QWheelEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private slots:
    void setModeFromAction(QAction* action);
};

#endif
