// DrawingWidget.h
// Copyright (C) 2020  Jason Allen
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

#include <DrawingCanvas.h>
#include <QHash>
#include <QList>
#include <QMenu>
#include <QPointF>
#include <QUndoStack>
#include <QVariant>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

class DrawingItem;
class DrawingItemPoint;

class DrawingWidget : public DrawingCanvas
{
	Q_OBJECT

	friend class DrawingReorderItemsCommand;

public:
	enum ActionIndex { UndoAction = NumberOfCanvasActions, RedoAction,
		CutAction, CopyAction, PasteAction, DeleteAction, SelectAllAction, SelectNoneAction,
		RotateAction, RotateBackAction, FlipHorizontalAction, FlipVerticalAction,
		BringForwardAction, SendBackwardAction, BringToFrontAction, SendToBackAction,
		GroupAction, UngroupAction, InsertPointAction, RemovePointAction,
		PropertiesAction, NumberOfActions };

private:
	enum MouseState { MouseReady, MouseSelect, MouseMoveItems, MouseResizeItem, MouseRubberBand };

private:
	QList<DrawingItem*> mItems;

	QList<DrawingItem*> mSelectedItems;
	DrawingItemPoint* mSelectedPoint;
	QPointF mSelectionCenter;

	QList<DrawingItem*> mPlaceItems;
	DrawingItem* mMouseDownItem;
	DrawingItemPoint* mMouseDownPoint;
	DrawingItem* mFocusItem;

	QUndoStack mUndoStack;

	QPoint mMouseDownPosition;
	QPointF mMouseDownScenePosition;
	bool mMouseLeftDragged;
	MouseState mDefaultMouseState;
	QRect mDefaultRubberBandRect;
	QHash<DrawingItem*,QPointF> mDefaultInitialPositions;
	QPointF mMouseDownPointInitialPosition;
	DrawingItem* mRightMouseDownItem;

	QMenu mSingleItemContextMenu;
	QMenu mSinglePolyItemContextMenu;
	QMenu mMultipleItemContextMenu;
	QMenu mDrawingContextMenu;

public:
	DrawingWidget(QWidget* parent = nullptr);
	virtual ~DrawingWidget();

	void addItem(DrawingItem* item);
	void insertItem(int index, DrawingItem* item);
	void removeItem(DrawingItem* item);
	void clearItems();
	QList<DrawingItem*> items() const;

	virtual DrawingItem* itemAt(const QPointF& position) const;
	virtual QList<DrawingItem*> items(const QPointF& position) const;
	virtual QList<DrawingItem*> items(const QRectF& rect) const;
	virtual QList<DrawingItem*> items(const QPainterPath& path) const;
	virtual bool itemContainsPoint(DrawingItem* item, const QPointF& position) const;
	virtual bool itemIsWithinRect(DrawingItem* item, const QRectF& rect) const;
	virtual bool itemIsWithinPath(DrawingItem* item, const QPainterPath& path) const;
	virtual QPainterPath itemAdjustedShape(DrawingItem* item) const;
	virtual QRect pointRect(DrawingItemPoint* point) const;

	void clearSelection();
	QList<DrawingItem*> selectedItems() const;
	DrawingItemPoint* selectedPoint() const;
	QPointF selectionCenter() const;

	QList<DrawingItem*> placeItems() const;
	DrawingItem* mouseDownItem() const;
	DrawingItem* focusItem() const;

	void setUndoLimit(int undoLimit);
	void pushUndoCommand(QUndoCommand* command);
	int undoLimit() const;
	bool isClean() const;
	bool canUndo() const;
	bool canRedo() const;
	QString undoText() const;
	QString redoText() const;

	virtual void setProperties(const QHash<QString,QVariant>& properties);
	virtual QHash<QString,QVariant> properties() const;

	virtual void render(QPainter* painter);
	virtual void renderExport(QPainter* painter);

	virtual void writeToXml(QXmlStreamWriter* xml);
	virtual void readFromXml(QXmlStreamReader* xml);

signals:
	void cleanChanged(bool clean);
	void canUndoChanged(bool canUndo);
	void canRedoChanged(bool canRedo);
	void propertiesTriggered();
	void propertiesChanged(const QHash<QString,QVariant>& properties);
	void currentItemsChanged(const QList<DrawingItem*>& items);
	void itemsPropertiesChanged(const QList<DrawingItem*>& items);
	void mouseInfoChanged(const QPointF& position);
	void mouseInfoChanged(const QPointF& position1, const QPointF& position2);
	void mouseInfoChanged(const QString& text);

public slots:
	virtual void setDefaultMode() override;
	virtual void setScrollMode() override;
	virtual void setZoomMode() override;
	void setPlaceMode(const QList<DrawingItem*>& items);
private:
	void clearMode();

public slots:
	void undo();
	void redo();
	void setClean();

	void cut();
	void copy();
	void paste();
	void deleteSelection();

	void selectAll();
	void selectNone();

	void moveSelection(const QPointF& delta);
	void resizeSelection(DrawingItemPoint* point, const QPointF& position);
	void rotateSelection();
	void rotateBackSelection();
	void flipSelectionHorizontal();
	void flipSelectionVertical();

	void bringSelectionForward();
	void sendSelectionBackward();
	void bringSelectionToFront();
	void sendSelectionToBack();

	void groupSelection();
	void ungroupSelection();

	void insertItemPointInSelection();
	void removeItemPointFromSelection();

	void updateItemsProperties(const QHash< DrawingItem*, QHash<QString,QVariant> >& properties);
	void updateProperties(const QHash<QString,QVariant>& properties);

public:
	void addItems(const QList<DrawingItem*>& items);
	void insertItems(const QList<DrawingItem*>& items, const QHash<DrawingItem*,int>& indices);
	void removeItems(const QList<DrawingItem*>& items);
	void moveItems(const QList<DrawingItem*>& items, const QHash<DrawingItem*,QPointF>& positions);
	void resizeItem(DrawingItemPoint* point, const QPointF& position);
	void rotateItems(const QList<DrawingItem*>& items, const QPointF& position);
	void rotateBackItems(const QList<DrawingItem*>& items, const QPointF& position);
	void flipItemsHorizontal(const QList<DrawingItem*>& items, const QPointF& position);
	void flipItemsVertical(const QList<DrawingItem*>& items, const QPointF& position);
	void selectItems(const QList<DrawingItem*>& items);
private:
	void reorderItems(const QList<DrawingItem*>& items);
public:
	void insertItemPoint(DrawingItem* item, DrawingItemPoint* point, int index);
	void removeItemPoint(DrawingItem* item, DrawingItemPoint* point);
	void connectPoints(DrawingItemPoint* point1, DrawingItemPoint* point2);
	void disconnectPoints(DrawingItemPoint* point1, DrawingItemPoint* point2);
	void setItemsProperties(const QHash< DrawingItem*, QHash<QString,QVariant> >& properties);

protected:
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
	virtual void mouseDoubleClickEvent(QMouseEvent* event) override;

	virtual void drawMain(QPainter* painter) override;
	virtual void drawItems(QPainter* painter, const QList<DrawingItem*>& items);
	virtual void drawItemPoints(QPainter* painter, const QList<DrawingItem*>& items);
	virtual void drawHotpoints(QPainter* painter, const QList<DrawingItem*>& items);

private slots:
	void updateGridProperty(qreal grid);
	void updateActionsFromSelection();
	void updateSelectionCenter();

private:
	void addItemsCommand(const QList<DrawingItem*>& items, bool place,
		QUndoCommand* command = nullptr);
	void removeItemsCommand(const QList<DrawingItem*>& items, QUndoCommand* command = nullptr);
	void moveItemsCommand(const QList<DrawingItem*>& items,
		const QHash<DrawingItem*,QPointF>& positions, bool place, QUndoCommand* command = nullptr);
	void resizeItemCommand(DrawingItemPoint* point, const QPointF& position, bool place,
		bool disconnect, QUndoCommand* command = nullptr);
	void rotateItemsCommand(const QList<DrawingItem*>& items, const QPointF& position,
		QUndoCommand* command = nullptr);
	void rotateBackItemsCommand(const QList<DrawingItem*>& items, const QPointF& position,
		QUndoCommand* command = nullptr);
	void flipItemsHorizontalCommand(const QList<DrawingItem*>& items, const QPointF& position,
		QUndoCommand* command = nullptr);
	void flipItemsVerticalCommand(const QList<DrawingItem*>& items, const QPointF& position,
		QUndoCommand* command = nullptr);
	void selectItemsCommand(const QList<DrawingItem*>& items, bool finalSelect,
		QUndoCommand* command = nullptr);
	void reorderItemsCommand(const QList<DrawingItem*>& items, QUndoCommand* command = nullptr);
	void insertPointCommand(DrawingItem* item, DrawingItemPoint* point, int index,
		QUndoCommand* command = nullptr);
	void removePointCommand(DrawingItem* item, DrawingItemPoint* point,
		QUndoCommand* command = nullptr);
	void connectPointsCommand(DrawingItemPoint* point1, DrawingItemPoint* point2,
		QUndoCommand* command = nullptr);
	void disconnectPointsCommand(DrawingItemPoint* point1, DrawingItemPoint* point2,
		QUndoCommand* command = nullptr);
	void setItemsPropertiesCommand(const QHash< DrawingItem*, QHash<QString,QVariant> >& properties,
		QUndoCommand* command = nullptr);
	void setPropertiesCommand(const QHash<QString,QVariant>& properties,
		QUndoCommand* command = nullptr);

	void placeItems(const QList<DrawingItem*>& items, QUndoCommand* command);
	void unplaceItems(const QList<DrawingItem*>& items, QUndoCommand* command);
	void tryToMaintainConnections(const QList<DrawingItem*>& items, bool allowResize,
		bool checkControlPoints, DrawingItemPoint* pointToSkip, QUndoCommand* command);
	void disconnectAll(DrawingItemPoint* point, QUndoCommand* command);

	bool shouldConnect(DrawingItemPoint* point1, DrawingItemPoint* point2) const;
	bool shouldDisconnect(DrawingItemPoint* point1, DrawingItemPoint* point2) const;
	DrawingItemPoint* pointAt(DrawingItem* item, const QPointF& position) const;

	void writeBrushToXml(QXmlStreamWriter* xml, const QString& name, const QBrush& brush);
	QString colorToString(const QColor& color) const;
	void readBrushFromXml(QXmlStreamReader* xml, const QString& name, QBrush& brush);
	QColor colorFromString(const QString& str) const;

	void addActions();
	void createContextMenus();
};

#endif
