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
#include <QUndoStack>
#include <QTimer>
#include "OdgDrawing.h"
#include "OdgMarker.h"

class QActionGroup;
class QMenu;
class OdgControlPoint;
class OdgGluePoint;
class OdgItem;
class OdgPathItem;
class OdgStyle;

class DrawingWidget : public QAbstractScrollArea, public OdgDrawing
{
    Q_OBJECT

    friend class DrawingReorderItemsCommand;

public:
    enum ActionIndex { UndoAction, RedoAction, CutAction, CopyAction, PasteAction, DeleteAction,
                       SelectAllAction, SelectNoneAction, SelectModeAction, ScrollModeAction, ZoomModeAction,
                       PlaceLineAction, PlaceCurveAction, PlacePolylineAction, PlaceRectangleAction, PlaceEllipseAction,
                       PlacePolygonAction, PlaceTextAction, PlaceTextRectangleAction, PlaceTextEllipseAction,
                       ElectricItemsMenuAction, LogicItemsMenuAction,
                       RotateAction, RotateBackAction, FlipHorizontalAction, FlipVerticalAction,
                       BringForwardAction, SendBackwardAction, BringToFrontAction, SendToBackAction,
                       GroupAction, UngroupAction, InsertPointAction, RemovePointAction,
                       InsertPageAction, DuplicatePageAction, RemovePageAction,
                       ZoomInAction, ZoomOutAction, ZoomFitAction};

private:
    enum MouseState { MouseIdle, HandlingLeftButtonEvent, HandlingRightButtonEvent, HandlingMiddleButtonEvent };

    enum SelectModeMouseState { SelectMouseIdle, SelectMouseSelectItem, SelectMouseMoveItems, SelectMouseResizeItem,
                                SelectMouseRubberBand };

private:
    OdgDrawing* mDrawingTemplate;
    OdgStyle* mDefaultStyle;

    OdgPage* mCurrentPage;
    int mNewPageCount;

    QTransform mTransform, mTransformInverse;

    Odg::DrawingMode mMode;

    QUndoStack mUndoStack;

    MouseState mMouseState;
    QPoint mMouseButtonDownPosition;
    QPointF mMouseButtonDownScenePosition;
    bool mMouseDragged;

    SelectModeMouseState mSelectMouseState;
    QList<OdgItem*> mSelectedItems;
    QPointF mSelectedItemsCenter;
    OdgItem* mSelectMouseDownItem;
    OdgControlPoint* mSelectMouseDownPoint;
    OdgItem* mSelectFocusItem;
	QHash<OdgItem*,QPointF> mSelectMoveItemsInitialPositions;
	QPointF mSelectMoveItemsPreviousDeltaPosition;
	QPointF mSelectResizeItemInitialPosition;
	QPointF mSelectResizeItemPreviousPosition;
    QRect mSelectRubberBandRect;

    int mScrollInitialHorizontalValue;
    int mScrollInitialVerticalValue;

    QRect mZoomRubberBandRect;

    QList<OdgItem*> mPlaceItems;
    bool mPlaceByMousePressAndRelease;

    Qt::CursorShape mPanOriginalCursor;
    QPoint mPanStartPosition;
    QPoint mPanCurrentPosition;
    QTimer mPanTimer;

    QActionGroup* mModeActionGroup;
    QMenu* mNoItemContextMenu;
    QMenu* mSingleItemContextMenu;
    QMenu* mSinglePolyItemContextMenu;
    QMenu* mSingleGroupItemContextMenu;
    QMenu* mMultipleItemContextMenu;

    QList<OdgPathItem*> mPathItems;

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
    void addPathItems(const QString& name, const QList<OdgPathItem*>& items, const QStringList& icons);
public:
    void setDrawingTemplate(OdgDrawing* temp);
    void setDefaultStyle(OdgStyle* style);
    OdgDrawing* drawingTemplate() const;
    OdgStyle* defaultStyle() const;

    void setUnits(Odg::Units units) override;
    void setPageSize(const QSizeF& size) override;
    void setPageMargins(const QMarginsF& margins) override;
    void setBackgroundColor(const QColor& color) override;
    void setGrid(double grid) override;
    void setGridStyle(Odg::GridStyle style) override;
    void setGridColor(const QColor& color) override;
    void setGridSpacingMajor(int spacing) override;
    void setGridSpacingMinor(int spacing) override;

    void setProperty(const QString& name, const QVariant& value) override;
    QVariant property(const QString& name) const override;

    OdgPage* currentPage() const;
    int currentPageIndex() const;

    double scale() const;
    QRectF visibleRect() const;
    QPointF mapToScene(const QPoint& position) const;
    QRectF mapToScene(const QRect& rect) const;
    QPoint mapFromScene(const QPointF& position) const;
    QRect mapFromScene(const QRectF& rect) const;

    Odg::DrawingMode mode() const;
    QList<OdgItem*> currentItems() const;
    QList<OdgItem*> selectedItems() const;
    OdgItem* mouseDownItem() const;
    OdgControlPoint* mouseDownPoint() const;
    OdgItem* focusItem() const;
    QList<OdgItem*> placeItems() const;

    void paint(QPainter& painter, bool isExport = false);

    void createNew();
    bool load(const QString& fileName);
    bool save(const QString& fileName);
    void clear();
    bool isClean() const;

    void insertPage(int index, OdgPage* page) override;
    void removePage(OdgPage* page) override;

    void setPageProperty(OdgPage* page, const QString& name, const QVariant& value);

    void addItems(OdgPage* page, const QList<OdgItem*>& items, bool place);
    void insertItems(OdgPage* page, const QList<OdgItem*>& items, const QHash<OdgItem*,int>& indices, bool place);
    void removeItems(OdgPage* page, const QList<OdgItem*>& items);
private:
    void reorderItems(OdgPage* page, const QList<OdgItem*>& items);

public:
    void moveItems(const QList<OdgItem*>& items, const QHash<OdgItem*,QPointF>& positions, bool place);
    void resizeItem(OdgControlPoint* point, const QPointF& position, bool snapTo45Degrees, bool disconnect, bool place);
    void resizeItem2(OdgControlPoint* point1, const QPointF& p1, OdgControlPoint* point2, const QPointF& p2,
                     bool place);

    void rotateItems(const QList<OdgItem*>& items, const QPointF& position);
    void rotateBackItems(const QList<OdgItem*>& items, const QPointF& position);
    void flipItemsHorizontal(const QList<OdgItem*>& items, const QPointF& position);
    void flipItemsVertical(const QList<OdgItem*>& items, const QPointF& position);

    void insertItemPoint(OdgItem* item, int index, OdgControlPoint* point);
    void removeItemPoint(OdgItem* item, OdgControlPoint* point);

    void placeItems(const QList<OdgItem*>& items);
    void unplaceItems(const QList<OdgItem*>& items);
    void maintainItemConnections(const QList<OdgItem*>& items);

    void setItemsProperty(const QList<OdgItem*>& items, const QString& name, const QVariant& value);
    void setItemsProperty(const QList<OdgItem*>& items, const QString& name, const QHash<OdgItem*,QVariant>& values);

    void setSelectedItems(const QList<OdgItem*>& items);

public slots:
    void setScale(double scale);
    void zoomIn();
    void zoomOut();
    void zoomFit();
    void zoomToRect(const QRectF& rect);
    void ensureVisible(const QRectF& rect);
    void centerOn(const QPointF& position);
    void mouseCursorOn(const QPointF& position);

    void setSelectMode();
    void setScrollMode();
    void setZoomMode();
    void setPlaceMode(const QList<OdgItem*>& items, bool placeByMousePressAndRelease);

    void undo();
    void redo();

    void setDrawingProperty(const QString& name, const QVariant& value);

    void insertPage();
    void duplicatePage();
    void removePage();
    void movePage(int newIndex);

    void setCurrentPage(OdgPage* page);
    void setCurrentPageIndex(int index);

    void setPageProperty(const QString& name, const QVariant& value);
    void renamePage(const QString& name);

    void removeItems();

    void cut();
    void copy();
    void paste();

    void selectAll();
    void selectNone();

    void move(const QPointF& position);
    void moveDelta(const QPointF& delta);
    void resize(OdgControlPoint* point, const QPointF& position);
    void resize2(OdgControlPoint* point1, const QPointF& p1, OdgControlPoint* point2, const QPointF& p2);

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

    void setItemsProperty(const QString& name, const QVariant& value);
    void setDefaultStyleProperty(const QString& name, const QVariant& value);

signals:
    void scaleChanged(double scale);
    void modeChanged(int mode);
    void modeTextChanged(const QString& modeText);
    void cleanChanged(bool clean);
    void cleanTextChanged(const QString& modeText);
    void mouseInfoChanged(const QString& modeText);
    void propertiesChanged();
    void propertyChanged(const QString& name, const QVariant& value);

    void pageInserted(OdgPage* page, int index);
    void pageRemoved(OdgPage* page, int index);
    void currentPageChanged(OdgPage* page);
    void currentPageIndexChanged(int index);
    void currentPagePropertyChanged(const QString& name, const QVariant& value);

    void itemsInserted(const QList<OdgItem*>& items);
    void itemsRemoved(const QList<OdgItem*>& items);
    void currentItemsChanged(const QList<OdgItem*>& items);
    void currentItemsGeometryChanged(const QList<OdgItem*>& items);
    void currentItemsPropertyChanged(const QList<OdgItem*>& items);

private:
    void paintEvent(QPaintEvent* event) override;
    void drawBackground(QPainter& painter, bool drawBorder, bool drawGrid);
    void drawGridLines(QPainter& painter, const QColor& color, int spacing);
    void drawItems(QPainter& painter, const QList<OdgItem*>& items);
    void drawItemPoints(QPainter& painter, const QList<OdgItem*>& items);
    void drawHotpoints(QPainter& painter, const QList<OdgItem*>& items);
    void drawRubberBand(QPainter& painter, const QRect& rect);

    void resizeEvent(QResizeEvent* event) override;
    void updateTransformAndScrollBars(double scale = 0.0);

    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

    void selectModeNoButtonMouseMoveEvent(QMouseEvent* event);
    void selectModeLeftMousePressEvent(QMouseEvent* event);
    void selectModeLeftMouseDragEvent(QMouseEvent* event);
    void selectModeLeftMouseReleaseEvent(QMouseEvent* event);
    void selectModeRightMousePressEvent(QMouseEvent* event);
    void selectModeRightMouseReleaseEvent(QMouseEvent* event);

	void selectModeSingleSelectEvent(QMouseEvent* event);

	void selectModeRubberBandStartEvent(QMouseEvent* event);
	void selectModeRubberBandUpdateEvent(QMouseEvent* event);
	void selectModeRubberBandEndEvent(QMouseEvent* event);

    void selectModeMoveItemsStartEvent(QMouseEvent* event);
    void selectModeMoveItemsUpdateEvent(QMouseEvent* event);
    void selectModeMoveItemsEndEvent(QMouseEvent* event);
    void selectModeMoveItems(const QPointF& mousePosition, bool finalMove, bool placeItems);

    void selectModeResizeItemStartEvent(QMouseEvent* event);
    void selectModeResizeItemUpdateEvent(QMouseEvent* event);
    void selectModeResizeItemEndEvent(QMouseEvent* event);
	void selectModeResizeItem(const QPointF& mousePosition, bool snapTo45Degrees, bool finalResize);

    void scrollModeNoButtonMouseMoveEvent(QMouseEvent* event);
    void scrollModeLeftMousePressEvent(QMouseEvent* event);
    void scrollModeLeftMouseDragEvent(QMouseEvent* event);
    void scrollModeLeftMouseReleaseEvent(QMouseEvent* event);

    void zoomModeNoButtonMouseMoveEvent(QMouseEvent* event);
    void zoomModeLeftMousePressEvent(QMouseEvent* event);
    void zoomModeLeftMouseDragEvent(QMouseEvent* event);
    void zoomModeLeftMouseReleaseEvent(QMouseEvent* event);

    void placeModeNoButtonMouseMoveEvent(QMouseEvent* event);
    void placeModeLeftMousePressEvent(QMouseEvent* event);
    void placeModeLeftMouseDragEvent(QMouseEvent* event);
    void placeModeLeftMouseReleaseEvent(QMouseEvent* event);

    QString createMouseInfo(const QPointF& position) const;
    QString createMouseInfo(const QPointF& p1, const QPointF& p2) const;

	OdgItem* itemAt(const QPointF& position) const;
	QList<OdgItem*> items(const QRectF& rect) const;

    QRectF itemsRect(const QList<OdgItem*>& items) const;
    QPointF itemsCenter(const QList<OdgItem*>& items) const;
    bool isItemInRect(OdgItem* item, const QRectF& rect) const;
    bool isPointInItem(OdgItem* item, const QPointF& position) const;
    QPainterPath itemAdjustedShape(OdgItem* item) const;

    bool shouldConnect(OdgControlPoint* controlPoint, OdgGluePoint* gluePoint) const;
    QRectF pointRect(OdgControlPoint* point) const;
    QRectF pointRect(OdgGluePoint* point) const;

	void updateSelectionCenter();
	void updateActions();

private slots:
    void mousePanEvent();

    void setModeFromAction(QAction* action);
    void emitCleanChanged(bool clean);
};

#endif
