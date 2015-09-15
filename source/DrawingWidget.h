/* DrawingWidget.cpp
 *
 * Copyright (C) 2013-2015 Jason Allen
 *
 * This file is part of the libdrawing library.
 *
 * libdrawing is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libdrawing is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libdrawing.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef DRAWINGWIDGET_H
#define DRAWINGWIDGET_H

#include <DrawingItem.h>

class DrawingWidget : public QAbstractScrollArea
{
	Q_OBJECT

	friend class DrawingReorderItemsCommand;

public:
	enum Mode { DefaultMode, ScrollMode, ZoomMode, PlaceMode, NumberOfModes };
	enum ActionIndex { UndoAction, RedoAction, CutAction, CopyAction, PasteAction, DeleteAction,
		SelectAllAction, SelectNoneAction, RotateAction, RotateBackAction, FlipAction,
		BringForwardAction, SendBackwardAction, BringToFrontAction, SendToBackAction,
		InsertPointAction, RemovePointAction, GroupAction, UngroupAction,
		ZoomInAction, ZoomOutAction, ZoomFitAction, PropertiesAction, NumberOfActions };

private:
	enum MouseState { MouseReady, MouseSelect, MouseMoveItems, MouseResizeItem, MouseRubberBand };

private:
	QRectF mSceneRect;
	qreal mGrid;

	QBrush mBackgroundBrush;
	DrawingGridStyle mGridStyle;
	QBrush mGridBrush;
	int mGridSpacingMajor, mGridSpacingMinor;

	qreal mScale;
	Mode mMode;

	QList<DrawingItem*> mItems;

	// Internal variables
	QTransform mViewportTransform;
	QTransform mSceneTransform;

	QList<DrawingItem*> mSelectedItems;
	DrawingItem* mNewItem;
	DrawingItem* mMouseDownItem;
	DrawingItem* mFocusItem;

	QUndoStack mUndoStack;
	DrawingMouseEvent mMouseEvent;
	MouseState mMouseState;
	QMap<DrawingItem*,QPointF> mInitialPositions;
	QRect mRubberBandRect;
	int mScrollButtonDownHorizontalScrollValue;
	int mScrollButtonDownVerticalScrollValue;
	QPoint mPanStartPos;
	QPoint mPanCurrentPos;
	QTimer mPanTimer;
	int mConsecutivePastes;
	QPointF mSelectionCenter;

	QMenu mSingleItemContextMenu;
	QMenu mSinglePolyItemContextMenu;
	QMenu mMultipleItemContextMenu;
	QMenu mDrawingContextMenu;

public:
	DrawingWidget();
	~DrawingWidget();

	// Selectors
	void setSceneRect(const QRectF& rect);
	void setSceneRect(qreal left, qreal top, qreal width, qreal height);
	QRectF sceneRect() const;
	qreal width() const;
	qreal height() const;

	void setGrid(qreal grid);
	qreal grid() const;
	qreal roundToGrid(qreal value) const;
	QPointF roundToGrid(const QPointF& scenePos) const;

	void setBackgroundBrush(const QBrush& brush);
	QBrush backgroundBrush() const;

	void setGridStyle(DrawingGridStyle style);
	void setGridBrush(const QBrush& brush);
	void setGridSpacing(int majorSpacing, int minorSpacing = 1);
	DrawingGridStyle gridStyle() const;
	QBrush gridBrush() const;
	int gridSpacingMajor() const;
	int gridSpacingMinor() const;

	void setUndoLimit(int undoLimit);
	void pushUndoCommand(QUndoCommand* command);
	int undoLimit() const;
	bool isClean() const;
	bool canUndo() const;
	bool canRedo() const;
	QString undoText() const;
	QString redoText() const;

	qreal scale() const;
	Mode mode() const;

	// Items
	void addItem(DrawingItem* item);
	void insertItem(int index, DrawingItem* item);
	void removeItem(DrawingItem* item);
	void clearItems();
	QList<DrawingItem*> items() const;
	QList<DrawingItem*> items(const QPointF& scenePos) const;
	QList<DrawingItem*> items(const QRectF& sceneRect) const;
	DrawingItem* itemAt(const QPointF& scenePos) const;

	void selectItem(DrawingItem* item);
	void deselectItem(DrawingItem* item);
	void selectItems(const QRectF& sceneRect, Qt::ItemSelectionMode mode);
	void clearSelection();
	QList<DrawingItem*> selectedItems() const;

	void setNewItem(DrawingItem* item);
	DrawingItem* newItem() const;
	DrawingItem* mouseDownItem() const;
	DrawingItem* focusItem() const;

	// View mapping
	QPointF mapToScene(const QPoint& screenPos) const;
	QRectF mapToScene(const QRect& screenRect) const;
	QPoint mapFromScene(const QPointF& scenePos) const;
	QRect mapFromScene(const QRectF& sceneRect) const;
	QRectF visibleRect() const;
	QRectF scrollBarDefinedRect() const;

	void centerOn(const QPointF& scenePos);
	void centerOnCursor(const QPointF& scenePos);
	void fitToView(const QRectF& sceneRect);
	void scaleBy(qreal scale);

public slots:
	void zoomIn();
	void zoomOut();
	void zoomFit();

	void setDefaultMode();
	void setScrollMode();
	void setZoomMode();
	void setPlaceMode(DrawingItem* newItem);

	void undo();
	void redo();
	void setClean();

	void cut();
	void copy();
	void paste();
	void deleteSelection();

	void selectAll();
	void selectNone();

	void moveSelection(const QPointF& newPos);
	void resizeSelection(DrawingItemPoint* itemPoint, const QPointF& scenePos);
	void rotateSelection();
	void rotateBackSelection();
	void flipSelection();
	void updateSelectionProperties(const QMap<QString,QVariant>& properties);

	void bringForward();
	void sendBackward();
	void bringToFront();
	void sendToBack();

	void insertItemPoint();
	void removeItemPoint();

	void group();
	void ungroup();

	void properties();
	void updateProperties(const QRectF& sceneRect, qreal grid, const QBrush& backgroundBrush,
		DrawingGridStyle gridStyle, const QBrush& gridBrush, int gridSpacingMajor, int gridSpacingMinor);

signals:
	void scaleChanged(qreal scale);
	void modeChanged(DrawingWidget::Mode mode);

	void cleanChanged(bool clean);
	void canUndoChanged(bool canUndo);
	void canRedoChanged(bool canRedo);

	void propertiesTriggered();
	void selectionChanged(const QList<DrawingItem*>& items);
	void newItemChanged(DrawingItem* item);
	void itemsGeometryChanged(const QList<DrawingItem*>& items);
	void itemGeometryChanged(DrawingItem* item);
	void numberOfItemsChanged(int itemCount);

	void mouseInfoChanged(const QString& infoText);

	void propertiesChanged(const QRectF& sceneRect, qreal grid, const QBrush& backgroundBrush,
		DrawingGridStyle gridStyle, const QBrush& gridBrush, int gridSpacingMajor, int gridSpacingMinor);

protected:
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual void mouseDoubleClickEvent(QMouseEvent* event);
	virtual void wheelEvent(QWheelEvent* event);

	virtual void defaultMousePressEvent(DrawingMouseEvent* event);
	virtual void defaultMouseMoveEvent(DrawingMouseEvent* event);
	virtual void defaultMouseReleaseEvent(DrawingMouseEvent* event);
	virtual void defaultMouseDoubleClickEvent(DrawingMouseEvent* event);

	virtual void placeModeMousePressEvent(DrawingMouseEvent* event);
	virtual void placeModeMouseMoveEvent(DrawingMouseEvent* event);
	virtual void placeModeMouseReleaseEvent(DrawingMouseEvent* event);
	virtual void placeModeMouseDoubleClickEvent(DrawingMouseEvent* event);

	virtual void keyPressEvent(QKeyEvent* event);
	virtual void keyReleaseEvent(QKeyEvent* event);

	virtual void paintEvent(QPaintEvent* event);
	virtual void drawBackground(QPainter* painter);
	virtual void drawItems(QPainter* painter);
	virtual void drawForeground(QPainter* painter);

protected slots:
	void mousePanEvent();

	void updateSelectionCenter();
	void updateActionsFromSelection();

protected:
	void addItems(const QList<DrawingItem*>& items, bool place, QUndoCommand* command = nullptr);
	void addItems(DrawingItem* item, bool place, QUndoCommand* command = nullptr);
	void removeItems(const QList<DrawingItem*>& items, QUndoCommand* command = nullptr);
	void removeItems(DrawingItem* item, QUndoCommand* command = nullptr);

	void moveItems(const QList<DrawingItem*>& items, const QMap<DrawingItem*,QPointF>& newPos,
		const QMap<DrawingItem*,QPointF>& initialPos, bool place, QUndoCommand* command = nullptr);
	void moveItems(const QList<DrawingItem*>& items, const QMap<DrawingItem*,QPointF>& newPos,
		bool place, QUndoCommand* command = nullptr);
	void moveItems(DrawingItem* item, const QPointF& newPos, bool place, QUndoCommand* command = nullptr);
	void resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos,
		bool place, bool disconnect, QUndoCommand* command = nullptr);

	void rotateItems(const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* command = nullptr);
	void rotateBackItems(const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* command = nullptr);
	void flipItems(const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* command = nullptr);

	void placeItems(const QList<DrawingItem*>& items, QUndoCommand* command);
	void placeItems(DrawingItem* item, QUndoCommand* command);
	void unplaceItems(const QList<DrawingItem*>& items, QUndoCommand* command);
	void unplaceItems(DrawingItem* item, QUndoCommand* command);

	void tryToMaintainConnections(const QList<DrawingItem*>& items, bool allowResize,
		bool checkControlPoints, DrawingItemPoint* pointToSkip, QUndoCommand* command);
	void tryToMaintainConnections(DrawingItem* item, bool allowResize,
		bool checkControlPoints, DrawingItemPoint* pointToSkip, QUndoCommand* command);
	void disconnectAll(DrawingItemPoint* itemPoint, QUndoCommand* command);

	void connectItemPoints(DrawingItemPoint* point0, DrawingItemPoint* point1, QUndoCommand* command = nullptr);
	void disconnectItemPoints(DrawingItemPoint* point0, DrawingItemPoint* point1, QUndoCommand* command = nullptr);

	void updateItemProperties(const QList<DrawingItem*>& items, const QMap<QString,QVariant>& properties,
		QUndoCommand* command = nullptr);

protected:
	bool itemMatchesPoint(DrawingItem* item, const QPointF& scenePos) const;
	bool itemMatchesRect(DrawingItem* item, const QRectF& rect, Qt::ItemSelectionMode mode) const;

	void recalculateContentSize(const QRectF& targetSceneRect = QRectF());

	void updateMouseInfo(const QPointF& pos);
	void updateMouseInfo(const QPointF& p1, const QPointF& p2);

	void addActions();
	void createContextMenu();
	QAction* addAction(const QString& text, QObject* slotObj, const char* slotFunction,
		const QString& iconPath = QString(), const QString& shortcut = QString());
};

#endif
