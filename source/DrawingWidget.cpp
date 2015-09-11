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

#include "DrawingWidget.h"
#include "DrawingItem.h"
#include "DrawingItemGroup.h"
#include "DrawingItemPoint.h"
#include "DrawingUndo.h"

DrawingWidget::DrawingWidget() : QAbstractScrollArea()
{
	setMouseTracking(true);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	mSceneRect = QRectF(-5000, -3750, 10000, 7500);
	mGrid = 50;

	mBackgroundBrush = Qt::white;
	mGridStyle = GridGraphPaper;
	mGridBrush = QColor(0, 128, 128);
	mGridSpacingMajor = 8;
	mGridSpacingMinor = 2;

	mScale = 1.0;
	mMode = DefaultMode;

	mNewItem = nullptr;
	mMouseDownItem = nullptr;
	mFocusItem = nullptr;

	mUndoStack.setUndoLimit(64);
	connect(&mUndoStack, SIGNAL(cleanChanged(bool)), this, SIGNAL(cleanChanged(bool)));
	connect(&mUndoStack, SIGNAL(canRedoChanged(bool)), this, SIGNAL(canRedoChanged(bool)));
	connect(&mUndoStack, SIGNAL(canUndoChanged(bool)), this, SIGNAL(canUndoChanged(bool)));
	connect(this, SIGNAL(selectionChanged(const QList<DrawingItem*>&)), this, SLOT(updateSelectionCenter()));

	mMouseState = MouseReady;
	mScrollButtonDownHorizontalScrollValue = 0;
	mScrollButtonDownVerticalScrollValue = 0;
	mPanTimer.setInterval(16);
	connect(&mPanTimer, SIGNAL(timeout()), this, SLOT(mousePanEvent()));
	mConsecutivePastes = 0;

	addActions();
	createContextMenu();
	connect(this, SIGNAL(selectionChanged(const QList<DrawingItem*>&)), this, SLOT(updateActionsFromSelection()));
}

DrawingWidget::~DrawingWidget()
{
	setNewItem(nullptr);
	clearItems();
}

//==================================================================================================

void DrawingWidget::setSceneRect(const QRectF& rect)
{
	mSceneRect = rect;
}

void DrawingWidget::setSceneRect(qreal left, qreal top, qreal width, qreal height)
{
	setSceneRect(QRectF(left, top, width, height));
}

QRectF DrawingWidget::sceneRect() const
{
	return mSceneRect;
}

qreal DrawingWidget::width() const
{
	return sceneRect().width();
}

qreal DrawingWidget::height() const
{
	return sceneRect().height();
}

//==================================================================================================

void DrawingWidget::setGrid(qreal grid)
{
	mGrid = grid;
}

qreal DrawingWidget::grid() const
{
	return mGrid;
}

qreal DrawingWidget::roundToGrid(qreal value) const
{
	qreal result = value;

	if (mGrid > 0)
	{
		qreal mod = fmod(value, mGrid);
		result = value - mod;
		if (mod >= mGrid/2) result += mGrid;
		else if (mod <= -mGrid/2) result -= mGrid;
	}

	return result;
}

QPointF DrawingWidget::roundToGrid(const QPointF& scenePos) const
{
	return QPointF(roundToGrid(scenePos.x()), roundToGrid(scenePos.y()));
}

//==================================================================================================

void DrawingWidget::setBackgroundBrush(const QBrush& brush)
{
	mBackgroundBrush = brush;
}

QBrush DrawingWidget::backgroundBrush() const
{
	return mBackgroundBrush;
}

//==================================================================================================

void DrawingWidget::setGridStyle(DrawingGridStyle style)
{
	mGridStyle = style;
}

void DrawingWidget::setGridBrush(const QBrush& brush)
{
	mGridBrush = brush;
}

void DrawingWidget::setGridSpacing(int majorSpacing, int minorSpacing)
{
	mGridSpacingMajor = majorSpacing;
	mGridSpacingMinor = minorSpacing;
}

DrawingGridStyle DrawingWidget::gridStyle() const
{
	return mGridStyle;
}

QBrush DrawingWidget::gridBrush() const
{
	return mGridBrush;
}

int DrawingWidget::gridSpacingMajor() const
{
	return mGridSpacingMajor;
}

int DrawingWidget::gridSpacingMinor() const
{
	return mGridSpacingMinor;
}

//==================================================================================================

void DrawingWidget::setUndoLimit(int undoLimit)
{
	mUndoStack.setUndoLimit(undoLimit);
}

void DrawingWidget::pushUndoCommand(QUndoCommand* command)
{
	mUndoStack.push(command);
}

int DrawingWidget::undoLimit() const
{
	return mUndoStack.undoLimit();
}

bool DrawingWidget::isClean() const
{
	return mUndoStack.isClean();
}

bool DrawingWidget::canUndo() const
{
	return mUndoStack.canUndo();
}

bool DrawingWidget::canRedo() const
{
	return mUndoStack.canRedo();
}

QString DrawingWidget::undoText() const
{
	return mUndoStack.undoText();
}

QString DrawingWidget::redoText() const
{
	return mUndoStack.redoText();
}

//==================================================================================================

qreal DrawingWidget::scale() const
{
	return mScale;
}

DrawingWidget::Mode DrawingWidget::mode() const
{
	return mMode;
}

//==================================================================================================

void DrawingWidget::addItem(DrawingItem* item)
{
	if (item && !mItems.contains(item))
	{
		mItems.append(item);
		item->mDrawing = this;
		item->recalculateTransform();
	}
}

void DrawingWidget::insertItem(int index, DrawingItem* item)
{
	if (item && !mItems.contains(item))
	{
		mItems.insert(index, item);
		item->mDrawing = this;
		item->recalculateTransform();
	}
}

void DrawingWidget::removeItem(DrawingItem* item)
{
	if (item && mItems.contains(item))
	{
		if (item->isSelected()) deselectItem(item);
		mItems.removeAll(item);
		item->mDrawing = nullptr;
	}
}

void DrawingWidget::clearItems()
{
	DrawingItem* item;

	while (!mItems.isEmpty())
	{
		item = mItems.first();
		removeItem(item);
		delete item;
	}

	mUndoStack.clear();
	mSelectedItems.clear();
	mMouseDownItem = nullptr;
	mFocusItem = nullptr;
	mInitialPositions.clear();
}

QList<DrawingItem*> DrawingWidget::items() const
{
	return mItems;
}

QList<DrawingItem*> DrawingWidget::items(const QPointF& scenePos) const
{
	QList<DrawingItem*> items;

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		if (itemMatchesPoint(*itemIter, scenePos)) items.append(*itemIter);
	}

	return items;
}

QList<DrawingItem*> DrawingWidget::items(const QRectF& sceneRect) const
{
	QList<DrawingItem*> items;

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		if (itemMatchesRect(*itemIter, sceneRect, Qt::ContainsItemBoundingRect))
			items.append(*itemIter);
	}

	return items;
}

DrawingItem* DrawingWidget::itemAt(const QPointF& scenePos) const
{
	DrawingItem* item = nullptr;

	// Favor selected items
	auto itemIter = mSelectedItems.end();
	while (item == nullptr && itemIter != mSelectedItems.begin())
	{
		itemIter--;
		if (itemMatchesPoint(*itemIter, scenePos)) item = *itemIter;
	}

	// Search all items
	itemIter = mItems.end();
	while (item == nullptr && itemIter != mItems.begin())
	{
		itemIter--;
		if (itemMatchesPoint(*itemIter, scenePos)) item = *itemIter;
	}

	return item;
}

//==================================================================================================

void DrawingWidget::selectItem(DrawingItem* item)
{
	if (item && !mSelectedItems.contains(item))
	{
		item->setSelected(true);
		mSelectedItems.append(item);
	}
}

void DrawingWidget::deselectItem(DrawingItem* item)
{
	if (item && mSelectedItems.contains(item))
	{
		mSelectedItems.removeAll(item);
		item->setSelected(false);
	}
}

void DrawingWidget::selectItems(const QRectF& sceneRect, Qt::ItemSelectionMode mode)
{
	clearSelection();

	if (sceneRect.isValid())
	{
		for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
		{
			if (itemMatchesRect(*itemIter, sceneRect, mode))
				selectItem(*itemIter);
		}
	}
}

void DrawingWidget::clearSelection()
{
	QList<DrawingItem*> selectedItems = mSelectedItems;

	for(auto itemIter = selectedItems.begin(); itemIter != selectedItems.end(); itemIter++)
		deselectItem(*itemIter);
}

QList<DrawingItem*> DrawingWidget::selectedItems() const
{
	return mSelectedItems;
}

//==================================================================================================

void DrawingWidget::setNewItem(DrawingItem* item)
{
	if (mNewItem) mNewItem->mDrawing = nullptr;
	delete mNewItem;

	mNewItem = item;
	if (mNewItem)
	{
		mNewItem->mDrawing = this;
		mNewItem->setVisible(true);
		mNewItem->recalculateTransform();
	}
}

DrawingItem* DrawingWidget::newItem() const
{
	return mNewItem;
}

DrawingItem* DrawingWidget::mouseDownItem() const
{
	return mMouseDownItem;
}

DrawingItem* DrawingWidget::focusItem() const
{
	return mFocusItem;
}

//==================================================================================================

QPointF DrawingWidget::mapToScene(const QPoint& screenPos) const
{
	QPointF p = screenPos;
	p.setX(p.x() + horizontalScrollBar()->value());
	p.setY(p.y() + verticalScrollBar()->value());
	return mSceneTransform.map(p);
}

QRectF DrawingWidget::mapToScene(const QRect& screenRect) const
{
	return QRectF(mapToScene(screenRect.topLeft()), mapToScene(screenRect.bottomRight()));
}

QPoint DrawingWidget::mapFromScene(const QPointF& scenePos) const
{
	QPointF p = mViewportTransform.map(scenePos);
	p.setX(p.x() - horizontalScrollBar()->value());
	p.setY(p.y() - verticalScrollBar()->value());
	return p.toPoint();
}

QRect DrawingWidget::mapFromScene(const QRectF& sceneRect) const
{
	return QRect(mapFromScene(sceneRect.topLeft()), mapFromScene(sceneRect.bottomRight()));
}

QRectF DrawingWidget::visibleRect() const
{
	return QRectF(mapToScene(QPoint(0, 0)), mapToScene(QPoint(viewport()->width(), viewport()->height())));
}

QRectF DrawingWidget::scrollBarDefinedRect() const
{
	QRectF scrollBarRect = mSceneRect;

	if (horizontalScrollBar()->minimum() < horizontalScrollBar()->maximum())
	{
		scrollBarRect.setLeft(horizontalScrollBar()->minimum() / mScale + mSceneRect.left());
		scrollBarRect.setWidth((horizontalScrollBar()->maximum() - horizontalScrollBar()->minimum() +
			maximumViewportSize().width()) / mScale);
	}

	if (verticalScrollBar()->minimum() < verticalScrollBar()->maximum())
	{
		scrollBarRect.setTop(verticalScrollBar()->minimum() / mScale + mSceneRect.top());
		scrollBarRect.setHeight((verticalScrollBar()->maximum() - verticalScrollBar()->minimum() +
			maximumViewportSize().height()) / mScale);
	}

	return scrollBarRect;
}

//==================================================================================================

void DrawingWidget::centerOn(const QPointF& scenePos)
{
	QPointF oldScenePos = mapToScene(viewport()->rect().center());

	int horizontalDelta = qRound((scenePos.x() - oldScenePos.x()) * mScale);
	int verticalDelta = qRound((scenePos.y() - oldScenePos.y()) * mScale);

	horizontalScrollBar()->setValue(horizontalScrollBar()->value() + horizontalDelta);
	verticalScrollBar()->setValue(verticalScrollBar()->value() + verticalDelta);
}

void DrawingWidget::centerOnCursor(const QPointF& scenePos)
{
	QPointF oldScenePos = mapToScene(mapFromGlobal(QCursor::pos()));

	int horizontalDelta = qRound((scenePos.x() - oldScenePos.x()) * mScale);
	int verticalDelta = qRound((scenePos.y() - oldScenePos.y()) * mScale);

	horizontalScrollBar()->setValue(horizontalScrollBar()->value() + horizontalDelta);
	verticalScrollBar()->setValue(verticalScrollBar()->value() + verticalDelta);
}

void DrawingWidget::fitToView(const QRectF& sceneRect)
{
	qreal scaleX = (maximumViewportSize().width() - 5) / sceneRect.width();
	qreal scaleY = (maximumViewportSize().height() - 5) / sceneRect.height();

	mScale = qMin(scaleX, scaleY);

	recalculateContentSize(sceneRect);

	centerOn(sceneRect.center());
}

void DrawingWidget::scaleBy(qreal scale)
{
	QPointF mousePos = mapToScene(mapFromGlobal(QCursor::pos()));
	QRectF scrollBarRect = scrollBarDefinedRect();

	mScale *= scale;

	recalculateContentSize(scrollBarRect);

	if (viewport()->rect().contains(mapFromGlobal(QCursor::pos()))) centerOnCursor(mousePos);
	else centerOn(QPointF());
}

//==================================================================================================

void DrawingWidget::zoomIn()
{
	scaleBy(qSqrt(2));

	emit scaleChanged(mScale);
	viewport()->update();
}

void DrawingWidget::zoomOut()
{
	scaleBy(qSqrt(2) / 2);

	emit scaleChanged(mScale);
	viewport()->update();
}

void DrawingWidget::zoomFit()
{
	fitToView(mSceneRect);

	emit scaleChanged(mScale);
	viewport()->update();
}

//==================================================================================================

void DrawingWidget::setDefaultMode()
{
	mMode = DefaultMode;
	setCursor(Qt::ArrowCursor);

	clearSelection();
	setNewItem(nullptr);

	emit selectionChanged(mSelectedItems);
	emit modeChanged(mMode);
	viewport()->update();
}

void DrawingWidget::setScrollMode()
{
	mMode = ScrollMode;
	setCursor(Qt::OpenHandCursor);

	clearSelection();
	setNewItem(nullptr);

	emit selectionChanged(mSelectedItems);
	emit modeChanged(mMode);
	viewport()->update();
}

void DrawingWidget::setZoomMode()
{
	mMode = ZoomMode;
	setCursor(Qt::CrossCursor);

	clearSelection();
	setNewItem(nullptr);

	emit selectionChanged(mSelectedItems);
	emit modeChanged(mMode);
	viewport()->update();
}

void DrawingWidget::setPlaceMode(DrawingItem* item)
{
	if (item)
	{
		mMode = PlaceMode;
		setCursor(Qt::CrossCursor);

		clearSelection();
		setNewItem(item);
		item->setPos(mapToScene(mapFromGlobal(QCursor::pos())));

		emit newItemChanged(item);
		emit modeChanged(mMode);
		viewport()->update();
	}
	else setDefaultMode();
}

//==================================================================================================

void DrawingWidget::undo()
{
	if (mMode == DefaultMode)
	{
		clearSelection();
		emit selectionChanged(mSelectedItems);

		mUndoStack.undo();
		emit numberOfItemsChanged(mItems.size());

		viewport()->update();
	}
	else setDefaultMode();
}

void DrawingWidget::redo()
{
	if (mMode == DefaultMode)
	{
		clearSelection();
		emit selectionChanged(mSelectedItems);

		mUndoStack.redo();
		emit numberOfItemsChanged(mItems.size());

		viewport()->update();
	}
	else setDefaultMode();
}

void DrawingWidget::setClean()
{
	mUndoStack.setClean();
	viewport()->update();
}

//==================================================================================================

void DrawingWidget::cut()
{
	copy();
	deleteSelection();
}

void DrawingWidget::copy()
{

}

void DrawingWidget::paste()
{
	//emit numberOfItemsChanged(mItems.size());
	//emit selectionChanged(mSelectedItems);
}

void DrawingWidget::deleteSelection()
{
	if (mMode == DefaultMode)
	{
		if (!mSelectedItems.isEmpty()) removeItems(mSelectedItems);
		emit numberOfItemsChanged(mItems.size());

		emit selectionChanged(mSelectedItems);
		viewport()->update();
	}
	else setDefaultMode();
}

//==================================================================================================

void DrawingWidget::selectAll()
{
	if (mMode == DefaultMode)
	{
		clearSelection();
		for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++) selectItem(*itemIter);

		emit selectionChanged(mSelectedItems);
		viewport()->update();
	}
}

void DrawingWidget::selectNone()
{
	if (mMode == DefaultMode)
	{
		clearSelection();

		emit selectionChanged(mSelectedItems);
		viewport()->update();
	}
}

//==================================================================================================

void DrawingWidget::moveSelection(const QPointF& newPos)
{
	if (mMode == PlaceMode && mNewItem)
	{
		mNewItem->moveItem(newPos);
		emit itemGeometryChanged(mNewItem);
		viewport()->update();
	}
	else if (mMode == DefaultMode && mSelectedItems.size() == 1 )
	{
		moveItems(mSelectedItems.first(), newPos, true);
		emit itemsGeometryChanged(mSelectedItems);
		viewport()->update();
	}
}

void DrawingWidget::resizeSelection(DrawingItemPoint* itemPoint, const QPointF& scenePos)
{
	if (mMode == PlaceMode && mNewItem && mNewItem->points().contains(itemPoint))
	{
		mNewItem->resizeItem(itemPoint, scenePos);
		emit itemGeometryChanged(mNewItem);
		viewport()->update();
	}
	else if (mMode == DefaultMode && mSelectedItems.size() == 1 && mSelectedItems.first()->points().contains(itemPoint))
	{
		resizeItem(itemPoint, scenePos, true, true);
		emit itemsGeometryChanged(mSelectedItems);
		viewport()->update();
	}
}

void DrawingWidget::rotateSelection()
{
	if (mMode == PlaceMode && mNewItem)
	{
		mNewItem->rotateItem(mNewItem->pos());
		emit itemGeometryChanged(mNewItem);
		viewport()->update();
	}
	else if (mMode == DefaultMode && !mSelectedItems.isEmpty())
	{
		rotateItems(mSelectedItems, roundToGrid(mSelectionCenter));
		emit itemsGeometryChanged(mSelectedItems);
		viewport()->update();
	}
}

void DrawingWidget::rotateBackSelection()
{
	if (mMode == PlaceMode && mNewItem)
	{
		mNewItem->rotateBackItem(mNewItem->pos());
		emit itemGeometryChanged(mNewItem);
		viewport()->update();
	}
	else if (mMode == DefaultMode && !mSelectedItems.isEmpty())
	{
		rotateBackItems(mSelectedItems, roundToGrid(mSelectionCenter));
		emit itemsGeometryChanged(mSelectedItems);
		viewport()->update();
	}
}

void DrawingWidget::flipSelection()
{
	if (mMode == PlaceMode && mNewItem)
	{
		mNewItem->flipItem(mNewItem->pos());
		emit itemGeometryChanged(mNewItem);
		viewport()->update();
	}
	else if (mMode == DefaultMode && !mSelectedItems.isEmpty())
	{
		flipItems(mSelectedItems, roundToGrid(mSelectionCenter));
		emit itemsGeometryChanged(mSelectedItems);
		viewport()->update();
	}
}

void DrawingWidget::updateSelectionProperties(const QMap<QString,QVariant>& properties)
{
	if (mMode == PlaceMode && mNewItem)
	{
		mNewItem->updateProperties(properties);
		viewport()->update();
	}
	else if (mMode == DefaultMode && !mSelectedItems.isEmpty())
	{
		updateItemProperties(mSelectedItems, properties);
		viewport()->update();
	}
}

//==================================================================================================

void DrawingWidget::bringForward()
{
	if (mMode == DefaultMode && !mSelectedItems.isEmpty())
	{
		QList<DrawingItem*> itemsToReorder = mSelectedItems;
		QList<DrawingItem*> itemsOrdered = mItems;
		DrawingItem* item;
		int itemIndex;

		while (!itemsToReorder.empty())
		{
			item = itemsToReorder.takeLast();

			itemIndex = itemsOrdered.indexOf(item);
			if (itemIndex >= 0)
			{
				itemsOrdered.removeAll(item);
				itemsOrdered.insert(itemIndex + 1, item);
			}
		}

		mUndoStack.push(new DrawingReorderItemsCommand(this, itemsOrdered));
		viewport()->update();
	}
}

void DrawingWidget::sendBackward()
{
	if (mMode == DefaultMode && !mSelectedItems.isEmpty())
	{
		QList<DrawingItem*> itemsToReorder = mSelectedItems;
		QList<DrawingItem*> itemsOrdered = mItems;
		DrawingItem* item;
		int itemIndex;

		while (!itemsToReorder.empty())
		{
			item = itemsToReorder.takeLast();

			itemIndex = itemsOrdered.indexOf(item);
			if (itemIndex >= 0)
			{
				itemsOrdered.removeAll(item);
				itemsOrdered.insert(itemIndex - 1, item);
			}
		}

		mUndoStack.push(new DrawingReorderItemsCommand(this, itemsOrdered));
		viewport()->update();
	}
}

void DrawingWidget::bringToFront()
{
	if (mMode == DefaultMode && !mSelectedItems.isEmpty())
	{
		QList<DrawingItem*> itemsToReorder = mSelectedItems;
		QList<DrawingItem*> itemsOrdered = mItems;
		DrawingItem* item;
		int itemIndex;

		while (!itemsToReorder.empty())
		{
			item = itemsToReorder.takeLast();

			itemIndex = itemsOrdered.indexOf(item);
			if (itemIndex >= 0)
			{
				itemsOrdered.removeAll(item);
				itemsOrdered.append(item);
			}
		}

		mUndoStack.push(new DrawingReorderItemsCommand(this, itemsOrdered));
		viewport()->update();
	}
}

void DrawingWidget::sendToBack()
{
	if (mMode == DefaultMode && !mSelectedItems.isEmpty())
	{
		QList<DrawingItem*> itemsToReorder = mSelectedItems;
		QList<DrawingItem*> itemsOrdered = mItems;
		DrawingItem* item;
		int itemIndex;

		while (!itemsToReorder.empty())
		{
			item = itemsToReorder.takeLast();

			itemIndex = itemsOrdered.indexOf(item);
			if (itemIndex >= 0)
			{
				itemsOrdered.removeAll(item);
				itemsOrdered.prepend(item);
			}
		}

		mUndoStack.push(new DrawingReorderItemsCommand(this, itemsOrdered));
		viewport()->update();
	}
}

//==================================================================================================

void DrawingWidget::insertItemPoint()
{
	if (mMode == DefaultMode && mSelectedItems.size() == 1)
	{
		DrawingItem* item = mSelectedItems.first();

		if (item)
		{
			DrawingItemPoint* itemPoint = new DrawingItemPoint(
				QPointF(), DrawingItemPoint::Control | DrawingItemPoint::Connection);
			itemPoint->setPos(item->mapFromScene(roundToGrid(mMouseEvent.buttonDownScenePos())));

			int index = item->insertItemPoint(itemPoint);
			if (index >= 0)
				mUndoStack.push(new DrawingItemInsertPointCommand(item, itemPoint, index));
			else
				delete itemPoint;

			emit itemsGeometryChanged(mSelectedItems);
			emit selectionChanged(mSelectedItems);
		}

		viewport()->update();
	}
}

void DrawingWidget::removeItemPoint()
{
	if (mMode == DefaultMode && mSelectedItems.size() == 1)
	{
		DrawingItem* item = mSelectedItems.first();

		if (item)
		{
			DrawingItemPoint* itemPoint = item->pointNearest(item->mapFromScene(mMouseEvent.buttonDownScenePos()));

			if (item->removeItemPoint(itemPoint))
				mUndoStack.push(new DrawingItemRemovePointCommand(item, itemPoint));

			emit itemsGeometryChanged(mSelectedItems);
			emit selectionChanged(mSelectedItems);
		}

		viewport()->update();
	}
}

//==================================================================================================

void DrawingWidget::group()
{
	if (mMode == DefaultMode && mSelectedItems.size() > 1)
	{
		QUndoCommand* command = new QUndoCommand("Group Items");

		QList<DrawingItem*> items = DrawingItem::copyItems(mSelectedItems);
		DrawingItemGroup* itemGroup = new DrawingItemGroup();

		itemGroup->setPos(items.first()->pos());
		for(auto iter = items.begin(); iter != items.end(); iter++)
			(*iter)->setPos(itemGroup->mapFromScene((*iter)->pos()));
		itemGroup->setItems(items);

		removeItems(mSelectedItems, command);
		addItems(itemGroup, false, command);
		mUndoStack.push(command);
		emit numberOfItemsChanged(mItems.size());

		selectItem(itemGroup);
		emit selectionChanged(mSelectedItems);

		viewport()->update();
	}
}

void DrawingWidget::ungroup()
{
	if (mMode == DefaultMode && mSelectedItems.size() == 1)
	{
		DrawingItemGroup* itemGroup = dynamic_cast<DrawingItemGroup*>(mSelectedItems.first());
		if (itemGroup)
		{
			QUndoCommand* command = new QUndoCommand("Ungroup Items");

			QList<DrawingItem*> items = DrawingItem::copyItems(itemGroup->items());
			for(auto iter = items.begin(); iter != items.end(); iter++)
				(*iter)->setPos(itemGroup->mapToScene((*iter)->pos()));

			removeItems(itemGroup, command);
			addItems(items, false, command);
			mUndoStack.push(command);
			emit numberOfItemsChanged(mItems.size());

			for(auto iter = items.begin(); iter != items.end(); iter++)	selectItem(*iter);
			emit selectionChanged(mSelectedItems);
		}

		viewport()->update();
	}
}

//==================================================================================================

void DrawingWidget::properties()
{
	emit propertiesTriggered();
}

void DrawingWidget::updateProperties(const QRectF& sceneRect, qreal grid, const QBrush& backgroundBrush,
	DrawingGridStyle gridStyle, const QBrush& gridBrush, int gridSpacingMajor, int gridSpacingMinor)
{
	DrawingUpdatePropertiesCommand* propertiesCommand = new DrawingUpdatePropertiesCommand(this,
		sceneRect, grid, backgroundBrush, gridStyle, gridBrush, gridSpacingMajor, gridSpacingMinor);

	mUndoStack.push(propertiesCommand);
}

//==================================================================================================

void DrawingWidget::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		mMouseEvent.setFromEvent(event, this);

		switch (mMode)
		{
		case ScrollMode:
			setCursor(Qt::ClosedHandCursor);
			mScrollButtonDownHorizontalScrollValue = horizontalScrollBar()->value();
			mScrollButtonDownVerticalScrollValue = verticalScrollBar()->value();
			break;
		case ZoomMode:
			break;
		case PlaceMode:
			placeModeMousePressEvent(&mMouseEvent);
			break;
		default:
			defaultMousePressEvent(&mMouseEvent);
			break;
		}

	}
	else if (event->button() == Qt::MiddleButton)
	{
		setCursor(Qt::SizeAllCursor);

		mPanStartPos = event->pos();
		mPanCurrentPos = event->pos();
		mPanTimer.start();
	}

	viewport()->update();
}

void DrawingWidget::mouseMoveEvent(QMouseEvent* event)
{
	mMouseEvent.setFromEvent(event, this);

	switch (mMode)
	{
	case ScrollMode:
		if ((mMouseEvent.buttons() & Qt::LeftButton) && mMouseEvent.isDragged())
		{
			horizontalScrollBar()->setValue(
				mScrollButtonDownHorizontalScrollValue - (mMouseEvent.pos().x() - mMouseEvent.buttonDownPos().x()));
			verticalScrollBar()->setValue(
				mScrollButtonDownVerticalScrollValue - (mMouseEvent.pos().y() - mMouseEvent.buttonDownPos().y()));

			updateMouseInfo(mMouseEvent.buttonDownScenePos());
		}
		else updateMouseInfo(mMouseEvent.scenePos());
		break;
	case ZoomMode:
		if ((mMouseEvent.buttons() & Qt::LeftButton) && mMouseEvent.isDragged())
		{
			QPoint p1 = mMouseEvent.pos();
			QPoint p2 = mMouseEvent.buttonDownPos();
			mRubberBandRect = QRect(qMin(p1.x(), p2.x()), qMin(p1.y(), p2.y()), qAbs(p2.x() - p1.x()), qAbs(p2.y() - p1.y()));

			updateMouseInfo(mMouseEvent.buttonDownScenePos(), mMouseEvent.scenePos());
		}
		else updateMouseInfo(mMouseEvent.scenePos());
	case PlaceMode:
		placeModeMouseMoveEvent(&mMouseEvent);
		break;
	default:
		defaultMouseMoveEvent(&mMouseEvent);
		break;
	}

	if (mPanTimer.isActive()) mPanCurrentPos = event->pos();

	viewport()->update();
}

void DrawingWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		mMouseEvent.setFromEvent(event, this);

		switch (mMode)
		{
		case ScrollMode:
			setCursor(Qt::OpenHandCursor);
			break;
		case ZoomMode:
			if (mRubberBandRect.isValid())
			{
				fitToView(mapToScene(mRubberBandRect));
				mRubberBandRect = QRect();

				emit scaleChanged(mScale);
				setDefaultMode();
			}
			break;
		case PlaceMode:
			placeModeMouseReleaseEvent(&mMouseEvent);
			break;
		default:
			defaultMouseReleaseEvent(&mMouseEvent);
			break;
		}
	}
	else if (event->button() == Qt::RightButton)
	{
		if (mMode == DefaultMode)
		{
			DrawingItem* mouseDownItem = itemAt(mapToScene(event->pos()));

			if (mouseDownItem && mouseDownItem->isSelected() && mSelectedItems.size() == 1)
			{
				if (actions()[InsertPointAction]->isEnabled())
					mSinglePolyItemContextMenu.popup(event->globalPos() + QPoint(2, 2));
				else
					mSingleItemContextMenu.popup(event->globalPos() + QPoint(2, 2));
			}
			else if (mouseDownItem && mouseDownItem->isSelected())
			{
				mMultipleItemContextMenu.popup(event->globalPos() + QPoint(2, 2));
			}
			else
			{
				if (mouseDownItem == nullptr) clearSelection();
				mDrawingContextMenu.popup(event->globalPos() + QPoint(2, 2));
			}
		}
		else setDefaultMode();
	}

	if (mPanTimer.isActive())
	{
		setCursor(Qt::ArrowCursor);
		mPanTimer.stop();
	}

	viewport()->update();
}

void DrawingWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	mMouseEvent.setFromEvent(event, this);

	if (mMode == PlaceMode) placeModeMouseDoubleClickEvent(&mMouseEvent);
	else if (mMode != DefaultMode) setDefaultMode();
	else defaultMouseDoubleClickEvent(&mMouseEvent);

	viewport()->update();
}

void DrawingWidget::wheelEvent(QWheelEvent* event)
{
	if (event->modifiers() && Qt::ControlModifier)
	{
		if (event->delta() > 0) zoomIn();
		else if (event->delta() < 0) zoomOut();
	}
	else QAbstractScrollArea::wheelEvent(event);
}

//==================================================================================================

void DrawingWidget::defaultMousePressEvent(DrawingMouseEvent* event)
{
	mMouseState = MouseSelect;

	if (event->button() == Qt::LeftButton)
	{
		mMouseDownItem = itemAt(event->scenePos());

		if (mMouseDownItem)
		{
			mInitialPositions.clear();
			for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
				mInitialPositions[*itemIter] = (*itemIter)->pos();

			if (mMouseDownItem->isSelected() && mSelectedItems.size() == 1)
				mMouseDownItem->mousePressEvent(event);
		}

		mFocusItem = mMouseDownItem;

		event->accept();
	}
}

void DrawingWidget::defaultMouseMoveEvent(DrawingMouseEvent* event)
{
	QMap<DrawingItem*,QPointF> newPositions;

	if (event->buttons() & Qt::LeftButton)
	{
		switch (mMouseState)
		{
		case MouseSelect:
			if (event->isDragged())
			{
				if (mMouseDownItem && mMouseDownItem->isSelected())
				{
					bool resizeItem = (mSelectedItems.size() == 1 &&
									   mSelectedItems.first()->selectedPoint() &&
									   mSelectedItems.first()->selectedPoint()->isControlPoint());
					mMouseState = (resizeItem) ? MouseResizeItem : MouseMoveItems;
				}
				else mMouseState = MouseRubberBand;
			}
			updateMouseInfo(mMouseEvent.scenePos());
			break;

		case MouseMoveItems:
			for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
			{
				newPositions[*itemIter] = mInitialPositions[*itemIter] +
					roundToGrid(event->scenePos() - event->buttonDownScenePos());
			}

			if (!newPositions.isEmpty())
			{
				moveItems(newPositions.keys(), newPositions, mInitialPositions, false);
				emit itemsGeometryChanged(mSelectedItems);
			}
			updateMouseInfo(roundToGrid(mMouseEvent.buttonDownScenePos()), roundToGrid(mMouseEvent.scenePos()));
			break;

		case MouseResizeItem:
			resizeItem(mMouseDownItem->selectedPoint(), roundToGrid(event->scenePos()), false, true);
			emit itemsGeometryChanged(mSelectedItems);
			updateMouseInfo(roundToGrid(mMouseEvent.buttonDownScenePos()), roundToGrid(mMouseEvent.scenePos()));
			break;

		case MouseRubberBand:
			mRubberBandRect = QRect(event->pos(), event->buttonDownPos()).normalized();
			updateMouseInfo(mMouseEvent.buttonDownScenePos(), mMouseEvent.scenePos());
			break;

		default:
			updateMouseInfo(mMouseEvent.scenePos());
			break;
		}

		if (mMouseDownItem && mMouseDownItem->isSelected() && mSelectedItems.size() == 1)
			mMouseDownItem->mouseMoveEvent(event);
	}
	else updateMouseInfo(mMouseEvent.scenePos());
}

void DrawingWidget::defaultMouseReleaseEvent(DrawingMouseEvent* event)
{
	bool controlDown = ((event->modifiers() & Qt::ControlModifier) != 0);
	QMap<DrawingItem*,QPointF> newPositions;

	switch (mMouseState)
	{
	case MouseSelect:
		if (!controlDown) clearSelection();
		if (mMouseDownItem)
		{
			if (controlDown && mMouseDownItem->isSelected()) deselectItem(mMouseDownItem);
			else selectItem(mMouseDownItem);

			emit selectionChanged(mSelectedItems);
		}
		break;

	case MouseMoveItems:
		for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
		{
			newPositions[*itemIter] = mInitialPositions[*itemIter] +
				roundToGrid(event->scenePos() - event->buttonDownScenePos());
		}

		if (!newPositions.isEmpty())
		{
			moveItems(newPositions.keys(), newPositions, mInitialPositions, true);
			emit itemsGeometryChanged(mSelectedItems);
		}
		break;

	case MouseResizeItem:
		resizeItem(mMouseDownItem->selectedPoint(), roundToGrid(event->scenePos()), true, true);
		emit itemsGeometryChanged(mSelectedItems);
		break;

	case MouseRubberBand:
		if (!controlDown) clearSelection();
		selectItems(mapToScene(mRubberBandRect), Qt::ContainsItemBoundingRect);
		mRubberBandRect = QRect();
		emit selectionChanged(mSelectedItems);
		break;

	default:
		break;
	}

	if (mMouseDownItem && mMouseDownItem->isSelected() && mSelectedItems.size() == 1)
		mMouseDownItem->mouseReleaseEvent(event);

	mMouseState = MouseReady;
	mConsecutivePastes = 0;

	updateSelectionCenter();
}

void DrawingWidget::defaultMouseDoubleClickEvent(DrawingMouseEvent* event)
{
	mMouseState = MouseSelect;

	if (event->button() == Qt::LeftButton)
	{
		mMouseDownItem = itemAt(event->scenePos());

		if (mMouseDownItem)
		{
			mInitialPositions.clear();
			for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
				mInitialPositions[*itemIter] = (*itemIter)->pos();

			emit propertiesTriggered();
		}

		mFocusItem = mMouseDownItem;
	}
}

//==================================================================================================

void DrawingWidget::placeModeMousePressEvent(DrawingMouseEvent* event)
{
	if (mNewItem)
	{
		mNewItem->newMousePressEvent(event);
		emit itemGeometryChanged(mNewItem);
	}
}

void DrawingWidget::placeModeMouseMoveEvent(DrawingMouseEvent* event)
{
	if (mNewItem)
	{
		mNewItem->newMouseMoveEvent(event);
		emit itemGeometryChanged(mNewItem);

		if (event->buttons() & Qt::LeftButton)
			updateMouseInfo(roundToGrid(mMouseEvent.buttonDownScenePos()), roundToGrid(mMouseEvent.scenePos()));
		else
			updateMouseInfo(roundToGrid(mMouseEvent.scenePos()));
	}
}

void DrawingWidget::placeModeMouseReleaseEvent(DrawingMouseEvent* event)
{
	if (mNewItem)
	{
		bool addNewItem = mNewItem->newMouseReleaseEvent(event);
		if (addNewItem && mNewItem->isValid())
		{
			mNewItem->mDrawing = nullptr;
			addItems(mNewItem, true);
			emit numberOfItemsChanged(mItems.size());

			mNewItem = mNewItem->copy();
			mNewItem->mDrawing = this;
			mNewItem->recalculateTransform();
			mNewItem->setPos(roundToGrid(event->scenePos()));
			emit newItemChanged(mNewItem);

			if (!mNewItem->newItemCopyEvent()) setDefaultMode();
		}
	}
}

void DrawingWidget::placeModeMouseDoubleClickEvent(DrawingMouseEvent* event)
{
	if (mNewItem)
	{
		bool addNewItem = mNewItem->newMouseDoubleClickEvent(event);

		if (addNewItem && mNewItem->isValid())
		{
			mNewItem->mDrawing = nullptr;
			addItems(mNewItem, true);
			emit numberOfItemsChanged(mItems.size());

			mNewItem = mNewItem->copy();
			mNewItem->mDrawing = this;
			mNewItem->recalculateTransform();
			mNewItem->setPos(roundToGrid(event->scenePos()));
			emit newItemChanged(mNewItem);

			if (!mNewItem->newItemCopyEvent()) setDefaultMode();
		}
	}
}

//==================================================================================================

void DrawingWidget::keyPressEvent(QKeyEvent* event)
{
	if (mFocusItem) mFocusItem->keyPressEvent(event);
}

void DrawingWidget::keyReleaseEvent(QKeyEvent* event)
{
	if (mFocusItem) mFocusItem->keyReleaseEvent(event);
}

//==================================================================================================

void DrawingWidget::paintEvent(QPaintEvent* event)
{
	QImage image(viewport()->width(), viewport()->height(), QImage::Format_RGB32);
	image.fill(palette().brush(QPalette::Window).color());

	// Render scene
	QPainter painter(&image);

	painter.translate(-horizontalScrollBar()->value(), -verticalScrollBar()->value());
	painter.setTransform(mViewportTransform, true);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

	drawBackground(&painter);
	drawItems(&painter);
	drawForeground(&painter);

	painter.end();

	// Render scene image on to widget
	QPainter widgetPainter(viewport());
	widgetPainter.drawImage(0, 0, image);

	Q_UNUSED(event);
}

void DrawingWidget::drawBackground(QPainter* painter)
{
	QRectF visibleRect = DrawingWidget::visibleRect();

	QPainter::RenderHints renderHints = painter->renderHints();
	painter->setRenderHints(renderHints, false);

	// Draw background
	painter->setBrush(mBackgroundBrush);
	painter->setPen(Qt::NoPen);
	painter->drawRect(visibleRect);

	// Draw grid
	QPen gridPen(mGridBrush, devicePixelRatio());
	gridPen.setCosmetic(true);

	if (mGridStyle != GridNone && mGrid >= 0)
	{
		painter->setPen(gridPen);

		if (mGridStyle == GridDotted && mGridSpacingMajor > 0)
		{
			qreal spacing = mGrid * mGridSpacingMajor;
			for(qreal y = qCeil(visibleRect.top() / spacing) * spacing; y < visibleRect.bottom(); y += spacing)
			{
				for(qreal x = qCeil(visibleRect.left() / spacing) * spacing; x < visibleRect.right(); x += spacing)
					painter->drawPoint(QPointF(x, y));
			}
		}

		if (mGridStyle == GridGraphPaper && mGridSpacingMinor > 0)
		{
			gridPen.setStyle(Qt::DotLine);
			painter->setPen(gridPen);

			qreal spacing = mGrid * mGridSpacingMinor;
			for(qreal y = qCeil(visibleRect.top() / spacing) * spacing; y < visibleRect.bottom(); y += spacing)
				painter->drawLine(QPointF(visibleRect.left(), y), QPointF(visibleRect.right(), y));
			for(qreal x = qCeil(visibleRect.left() / spacing) * spacing; x < visibleRect.right(); x += spacing)
				painter->drawLine(QPointF(x, visibleRect.top()), QPointF(x, visibleRect.bottom()));
		}

		if ((mGridStyle == GridLined || mGridStyle == GridGraphPaper) && mGridSpacingMajor > 0)
		{
			gridPen.setStyle(Qt::SolidLine);
			painter->setPen(gridPen);

			qreal spacing = mGrid * mGridSpacingMajor;
			for(qreal y = qCeil(visibleRect.top() / spacing) * spacing; y < visibleRect.bottom(); y += spacing)
				painter->drawLine(QPointF(visibleRect.left(), y), QPointF(visibleRect.right(), y));
			for(qreal x = qCeil(visibleRect.left() / spacing) * spacing; x < visibleRect.right(); x += spacing)
				painter->drawLine(QPointF(x, visibleRect.top()), QPointF(x, visibleRect.bottom()));
		}
	}

	// Draw origin
	painter->save();
	painter->setBrush(Qt::transparent);
	painter->setPen(gridPen);
	painter->resetTransform();
	painter->drawEllipse(mapFromScene(QPointF(0, 0)), 4, 4);
	painter->restore();

	// Draw border
	QPen borderPen((mBackgroundBrush == Qt::black) ? Qt::white : Qt::black, devicePixelRatio() * 2);
	borderPen.setCosmetic(true);

	painter->setBrush(Qt::transparent);
	painter->setPen(borderPen);
	painter->drawRect(mSceneRect);

	painter->setRenderHints(renderHints);
	painter->setBrush(mBackgroundBrush);
}

void DrawingWidget::drawItems(QPainter* painter)
{
	// Draw items
	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		if ((*itemIter)->isVisible())
		{
			painter->translate((*itemIter)->pos());
			painter->setTransform((*itemIter)->transform().inverted(), true);
			(*itemIter)->paint(painter);
			painter->setTransform((*itemIter)->transform(), true);
			painter->translate(-(*itemIter)->pos());
		}
	}

	// Draw new item
	if (mNewItem)
	{
		painter->translate(mNewItem->pos());
		painter->setTransform(mNewItem->transform().inverted(), true);
		mNewItem->paint(painter);
		painter->setTransform(mNewItem->transform(), true);
		painter->translate(-mNewItem->pos());
	}

	// Draw item points
	QColor color = mBackgroundBrush.color();
	color.setRed(255 - color.red());
	color.setGreen(255 - color.green());
	color.setBlue(255 - color.blue());

	painter->save();

	painter->resetTransform();
	painter->setRenderHints(QPainter::Antialiasing, false);
	painter->setPen(QPen(color, 1));
	painter->setBrush(QColor(0, 224, 0));

	for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
	{
		if ((*itemIter)->isVisible())
		{
			QList<DrawingItemPoint*> itemPoints = (*itemIter)->points();

			for(auto pointIter = itemPoints.begin(); pointIter != itemPoints.end(); pointIter++)
			{
				QRect pointRect = (*pointIter)->screenRect().adjusted(0, 0, -1, -1);

				if (((*pointIter)->flags() & DrawingItemPoint::Control) ||
					((*pointIter)->flags() == DrawingItemPoint::NoFlags))
				{
					pointRect.adjust(1, 1, -1, -1);
					painter->drawRect(pointRect);
				}

				if ((*pointIter)->flags() & DrawingItemPoint::Connection)
				{
					for(int x = 0; x <= pointRect.width(); x++)
					{
						painter->drawPoint(pointRect.left() + x, pointRect.bottom() + 1 - x);
						painter->drawPoint(pointRect.left() + x, pointRect.top() + x);
					}
				}
			}
		}
	}

	painter->restore();
}

void DrawingWidget::drawForeground(QPainter* painter)
{
	// Draw hotpoints
	QList<DrawingItem*> items = mSelectedItems;
	if (mNewItem) items.prepend(mNewItem);

	painter->save();

	painter->resetTransform();
	painter->setRenderHints(QPainter::Antialiasing, false);
	painter->setBrush(QColor(255, 128, 0, 128));
	painter->setPen(QPen(painter->brush(), 1));

	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
	{
		QList<DrawingItemPoint*> itemPoints = (*itemIter)->points();

		for(auto pointIter = itemPoints.begin(); pointIter != itemPoints.end(); pointIter++)
		{
			for(auto otherItemIter = mItems.begin(); otherItemIter != mItems.end(); otherItemIter++)
			{
				if ((*itemIter) != (*otherItemIter))
				{
					QList<DrawingItemPoint*> otherItemPoints = (*otherItemIter)->points();

					for(auto otherItemPointIter = otherItemPoints.begin();
						otherItemPointIter != otherItemPoints.end(); otherItemPointIter++)
					{
						if ((*pointIter)->shouldConnect(*otherItemPointIter))
						{
							QRect pointRect = (*pointIter)->screenRect();
							pointRect.adjust(-pointRect.width() / 2, -pointRect.width() / 2,
								pointRect.width() / 2, pointRect.width() / 2);

							painter->drawEllipse(pointRect);
						}
					}
				}
			}
		}
	}

	painter->restore();

	// Draw rubber band
	QStyleOptionRubberBand option;
	option.initFrom(viewport());
	option.rect = mRubberBandRect;
	option.shape = QRubberBand::Rectangle;

	painter->save();
	painter->resetTransform();

	QStyleHintReturnMask mask;
	if (viewport()->style()->styleHint(QStyle::SH_RubberBand_Mask, &option, viewport(), &mask))
		painter->setClipRegion(mask.region, Qt::IntersectClip);

	viewport()->style()->drawControl(QStyle::CE_RubberBand, &option, painter, viewport());

	painter->restore();
}

//==================================================================================================

void DrawingWidget::mousePanEvent()
{
	QRectF visibleRect = DrawingWidget::visibleRect();

	if (mPanCurrentPos.x() - mPanStartPos.x() < 0)
	{
		int delta = (mPanCurrentPos.x() - mPanStartPos.x()) / 16;

		if (horizontalScrollBar()->value() + delta < horizontalScrollBar()->minimum())
		{
			if (horizontalScrollBar()->minimum() >= horizontalScrollBar()->maximum())
				horizontalScrollBar()->setMinimum((visibleRect.left() - mSceneRect.left()) * mScale + delta);
			else
				horizontalScrollBar()->setMinimum(horizontalScrollBar()->value() + delta);

			horizontalScrollBar()->setValue(horizontalScrollBar()->minimum());
		}
		else horizontalScrollBar()->setValue(horizontalScrollBar()->value() + delta);
	}
	else if (mPanCurrentPos.x() - mPanStartPos.x() > 0)
	{
		int delta = (mPanCurrentPos.x() - mPanStartPos.x()) / 16;

		if (horizontalScrollBar()->value() + delta > horizontalScrollBar()->maximum())
		{
			if (horizontalScrollBar()->minimum() > horizontalScrollBar()->maximum())
				horizontalScrollBar()->setMaximum((mSceneRect.right() - visibleRect.right()) * mScale + delta);
			else
				horizontalScrollBar()->setMaximum(horizontalScrollBar()->value() + delta);

			horizontalScrollBar()->setValue(horizontalScrollBar()->maximum());
		}
		else horizontalScrollBar()->setValue(horizontalScrollBar()->value() + delta);
	}

	if (mPanCurrentPos.y() - mPanStartPos.y() < 0)
	{
		int delta = (mPanCurrentPos.y() - mPanStartPos.y()) / 16;

		if (verticalScrollBar()->value() + delta < verticalScrollBar()->minimum())
		{
			if (verticalScrollBar()->minimum() >= verticalScrollBar()->maximum())
				verticalScrollBar()->setMinimum((visibleRect.top() - mSceneRect.top()) * mScale + delta);
			else
				verticalScrollBar()->setMinimum(verticalScrollBar()->value() + delta);

			verticalScrollBar()->setValue(verticalScrollBar()->minimum());
		}
		else verticalScrollBar()->setValue(verticalScrollBar()->value() + delta);
	}
	else if (mPanCurrentPos.y() - mPanStartPos.y() > 0)
	{
		int delta = (mPanCurrentPos.y() - mPanStartPos.y()) / 16;

		if (verticalScrollBar()->value() + delta > verticalScrollBar()->maximum())
		{
			if (verticalScrollBar()->minimum() >= verticalScrollBar()->maximum())
				verticalScrollBar()->setMaximum((mSceneRect.bottom() - visibleRect.bottom()) * mScale + delta);
			else
				verticalScrollBar()->setMaximum(verticalScrollBar()->value() + delta);

			verticalScrollBar()->setValue(verticalScrollBar()->maximum());
		}
		else verticalScrollBar()->setValue(verticalScrollBar()->value() + delta);
	}
}

//==================================================================================================

void DrawingWidget::updateSelectionCenter()
{
	mSelectionCenter = QPointF();

	if (!mSelectedItems.isEmpty())
	{
		for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
			mSelectionCenter += (*itemIter)->mapToScene((*itemIter)->centerPos());

		mSelectionCenter /= mSelectedItems.size();
	}
}

void DrawingWidget::updateActionsFromSelection()
{
	QList<QAction*> actions = DrawingWidget::actions();

	bool canInsertRemovePoints = false;
	bool canGroup = (mSelectedItems.size() > 1);
	bool canUngroup = false;

	if (mSelectedItems.size() == 1)
	{
		QString typeName = QString(typeid(*mSelectedItems.first()).name());

		canInsertRemovePoints = (typeName == "class DrawingPolygonItem" || typeName == "class DrawingPolylineItem");
		canUngroup = (typeName == "class DrawingItemGroup");
	}

	actions[InsertPointAction]->setEnabled(canInsertRemovePoints);
	actions[RemovePointAction]->setEnabled(canInsertRemovePoints);
	actions[GroupAction]->setEnabled(canGroup);
	actions[UngroupAction]->setEnabled(canUngroup);
}

//==================================================================================================

void DrawingWidget::addItems(const QList<DrawingItem*>& items, bool place, QUndoCommand* command)
{
	DrawingAddItemsCommand* addCommand = new DrawingAddItemsCommand(this, items, command);

	addCommand->redo();
	if (place) placeItems(items, addCommand);
	addCommand->undo();

	if (!command) mUndoStack.push(addCommand);
}

void DrawingWidget::addItems(DrawingItem* item, bool place, QUndoCommand* command)
{
	QList<DrawingItem*> items;
	items.append(item);
	addItems(items, place, command);
}

void DrawingWidget::removeItems(const QList<DrawingItem*>& items, QUndoCommand* command)
{
	DrawingRemoveItemsCommand* removeCommand = new DrawingRemoveItemsCommand(this, items, command);

	removeCommand->redo();
	unplaceItems(items, removeCommand);
	removeCommand->undo();

	if (!command) mUndoStack.push(removeCommand);
}

void DrawingWidget::removeItems(DrawingItem* item, QUndoCommand* command)
{
	QList<DrawingItem*> items;
	items.append(item);
	removeItems(items, command);
}

//==================================================================================================

void DrawingWidget::moveItems(const QList<DrawingItem*>& items, const QMap<DrawingItem*,QPointF>& newPos,
	const QMap<DrawingItem*,QPointF>& initialPos, bool place, QUndoCommand* command)
{
	DrawingMoveItemsCommand* moveCommand =
		new DrawingMoveItemsCommand(items, newPos, initialPos, place, command);

	moveCommand->redo();
	tryToMaintainConnections(items, true, true, nullptr, moveCommand);
	if (place) placeItems(items, moveCommand);
	moveCommand->undo();

	if (!command) mUndoStack.push(moveCommand);
}

void DrawingWidget::moveItems(const QList<DrawingItem*>& items, const QMap<DrawingItem*,QPointF>& newPos,
	bool place, QUndoCommand* command)
{
	QMap<DrawingItem*,QPointF> initialPos;
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		initialPos[*itemIter] = (*itemIter)->pos();

	DrawingMoveItemsCommand* moveCommand =
		new DrawingMoveItemsCommand(items, newPos, initialPos, place, command);

	moveCommand->redo();
	tryToMaintainConnections(items, true, true, nullptr, moveCommand);
	if (place) placeItems(items, moveCommand);
	moveCommand->undo();

	if (!command) mUndoStack.push(moveCommand);
}

void DrawingWidget::moveItems(DrawingItem* item, const QPointF& newPos, bool place, QUndoCommand* command)
{
	QList<DrawingItem*> items;
	QMap<DrawingItem*,QPointF> newPosMap;
	items.append(item);
	newPosMap[item] = newPos;

	moveItems(items, newPosMap, place, command);
}

void DrawingWidget::resizeItem(DrawingItemPoint* itemPoint, const QPointF& scenePos,
	bool place, bool disconnect, QUndoCommand* command)
{
	if (itemPoint)
	{
		DrawingResizeItemCommand* resizeCommand =
			new DrawingResizeItemCommand(itemPoint, scenePos, place, command);

		resizeCommand->redo();
		if (disconnect) disconnectAll(itemPoint, resizeCommand);
		tryToMaintainConnections(itemPoint->item(), true, !(itemPoint->flags() & DrawingItemPoint::Free), itemPoint, resizeCommand);
		if (place) placeItems(itemPoint->item(), resizeCommand);
		resizeCommand->undo();

		if (!command) mUndoStack.push(resizeCommand);
	}
}

//==================================================================================================

void DrawingWidget::rotateItems(const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* command)
{
	DrawingRotateItemsCommand* rotateCommand =
		new DrawingRotateItemsCommand(items, scenePos, command);

	rotateCommand->redo();
	tryToMaintainConnections(items, true, true, nullptr, rotateCommand);
	rotateCommand->undo();

	if (!command) mUndoStack.push(rotateCommand);
}

void DrawingWidget::rotateBackItems(const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* command)
{
	DrawingRotateBackItemsCommand* rotateCommand =
		new DrawingRotateBackItemsCommand(items, scenePos, command);

	rotateCommand->redo();
	tryToMaintainConnections(items, true, true, nullptr, rotateCommand);
	rotateCommand->undo();

	if (!command) mUndoStack.push(rotateCommand);
}

void DrawingWidget::flipItems(const QList<DrawingItem*>& items, const QPointF& scenePos, QUndoCommand* command)
{
	DrawingFlipItemsCommand* flipCommand =
		new DrawingFlipItemsCommand(items, scenePos, command);

	flipCommand->redo();
	tryToMaintainConnections(items, true, true, nullptr, flipCommand);
	flipCommand->undo();

	if (!command) mUndoStack.push(flipCommand);
}

//==================================================================================================

void DrawingWidget::placeItems(const QList<DrawingItem*>& items, QUndoCommand* command)
{
	QList<DrawingItemPoint*> itemPoints, otherItemPoints;

	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
	{
		for(auto otherItemIter = mItems.begin(); otherItemIter != mItems.end(); otherItemIter++)
		{
			if (!items.contains(*otherItemIter) && (mNewItem == nullptr || (*otherItemIter) != mNewItem))
			{
				itemPoints = (*itemIter)->points();
				otherItemPoints = (*otherItemIter)->points();

				for(auto itemPointIter = itemPoints.begin(); itemPointIter != itemPoints.end(); itemPointIter++)
				{
					for(auto otherItemPointIter = otherItemPoints.begin(); otherItemPointIter != otherItemPoints.end(); otherItemPointIter++)
					{
						if ((*itemPointIter)->shouldConnect(*otherItemPointIter))
							connectItemPoints(*itemPointIter, *otherItemPointIter, command);
					}
				}
			}
		}
	}
}

void DrawingWidget::placeItems(DrawingItem* item, QUndoCommand* command)
{
	if (item)
	{
		QList<DrawingItem*> items;
		items.append(item);
		placeItems(items, command);
	}
}

void DrawingWidget::unplaceItems(const QList<DrawingItem*>& items, QUndoCommand* command)
{
	DrawingItem* item;
	DrawingItemPoint* itemPoint;
	QList<DrawingItemPoint*> itemPoints, targetPoints;

	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
	{
		item = *itemIter;
		itemPoints = item->points();

		for(auto itemPointIter = itemPoints.begin(); itemPointIter != itemPoints.end(); itemPointIter++)
		{
			itemPoint = *itemPointIter;
			targetPoints = itemPoint->connections();
			for(auto targetPointIter = targetPoints.begin(); targetPointIter != targetPoints.end(); targetPointIter++)
			{
				if (!items.contains((*targetPointIter)->item()))
					disconnectItemPoints(itemPoint, *targetPointIter, command);
			}
		}
	}
}

void DrawingWidget::unplaceItems(DrawingItem* item, QUndoCommand* command)
{
	if (item)
	{
		QList<DrawingItem*> items;
		items.append(item);
		unplaceItems(items, command);
	}
}

//==================================================================================================

void DrawingWidget::tryToMaintainConnections(const QList<DrawingItem*>& items, bool allowResize,
	bool checkControlPoints, DrawingItemPoint* pointToSkip, QUndoCommand* command)
{
	QList<DrawingItemPoint*> itemPoints, targetPoints;
	DrawingItem* item;
	DrawingItem* targetItem;
	DrawingItemPoint* itemPoint;
	DrawingItemPoint* targetItemPoint;

	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
	{
		item = *itemIter;
		itemPoints = item->points();

		for(auto itemPointIter = itemPoints.begin(); itemPointIter != itemPoints.end(); itemPointIter++)
		{
			itemPoint = *itemPointIter;
			if (itemPoint != pointToSkip && (checkControlPoints || !(itemPoint->flags() & DrawingItemPoint::Control)))
			{
				targetPoints = itemPoint->connections();
				for(auto targetPointIter = targetPoints.begin(); targetPointIter != targetPoints.end(); targetPointIter++)
				{
					targetItemPoint = *targetPointIter;
					targetItem = targetItemPoint->item();

					if (item->mapToScene(itemPoint->pos()) != targetItem->mapToScene(targetItemPoint->pos()))
					{
						// Try to maintain the connection by resizing targetPoint if possible
						if (allowResize && (targetItemPoint->flags() & DrawingItemPoint::Free) && !itemPoint->shouldDisconnect(targetItemPoint))
							resizeItem(targetItemPoint, item->mapToScene(itemPoint->pos()), false, false, command);
						else
							disconnectItemPoints(itemPoint, targetItemPoint, command);
					}
				}
			}
		}
	}
}

void DrawingWidget::tryToMaintainConnections(DrawingItem* item, bool allowResize,
	bool checkControlPoints, DrawingItemPoint* pointToSkip, QUndoCommand* command)
{
	if (item)
	{
		QList<DrawingItem*> items;
		items.append(item);
		tryToMaintainConnections(items, allowResize, checkControlPoints, pointToSkip, command);
	}
}

void DrawingWidget::disconnectAll(DrawingItemPoint* itemPoint, QUndoCommand* command)
{
	QList<DrawingItemPoint*> targetPoints;

	if (itemPoint)
	{
		targetPoints = itemPoint->connections();
		for(auto targetPointIter = targetPoints.begin(); targetPointIter != targetPoints.end(); targetPointIter++)
			disconnectItemPoints(itemPoint, *targetPointIter, command);
	}
}

//==================================================================================================

void DrawingWidget::connectItemPoints(DrawingItemPoint* point0, DrawingItemPoint* point1, QUndoCommand* command)
{
	DrawingItemPointConnectCommand* connectCommand = new DrawingItemPointConnectCommand(point0, point1, command);
	QPointF point0Pos = point0->item()->mapToScene(point0->pos());
	QPointF point1Pos = point1->item()->mapToScene(point1->pos());

	if (point0Pos != point1Pos)
	{
		if (point1->isControlPoint())
			resizeItem(point1, point0Pos, false, true, connectCommand);
		else if (point0->isControlPoint())
			resizeItem(point0, point1Pos, false, true, connectCommand);
	}

	if (!command) mUndoStack.push(connectCommand);
}

void DrawingWidget::disconnectItemPoints(DrawingItemPoint* point0, DrawingItemPoint* point1, QUndoCommand* command)
{
	DrawingItemPointDisconnectCommand* disconnectCommand =
		new DrawingItemPointDisconnectCommand(point0, point1, command);

	if (!command) mUndoStack.push(disconnectCommand);
}

//==================================================================================================

void DrawingWidget::updateItemProperties(const QList<DrawingItem*>& items,
	const QMap<QString,QVariant>& properties, QUndoCommand* command)
{
	DrawingUpdateItemPropertiesCommand* propertiesCommand =
		new DrawingUpdateItemPropertiesCommand(items, properties, command);

	if (!command) mUndoStack.push(propertiesCommand);
}

//==================================================================================================

bool DrawingWidget::itemMatchesPoint(DrawingItem* item, const QPointF& scenePos) const
{
	bool match = false;

	if (item && item->isVisible())
	{
		// Check item shape
		match = item->shape().contains(item->mapFromScene(scenePos));

		// Check item points
		if (!match && item->isSelected())
		{
			QList<DrawingItemPoint*> itemPoints = item->points();
			for(auto pointIter = itemPoints.begin(); !match && pointIter != itemPoints.end(); pointIter++)
				match = (*pointIter)->sceneRect().contains(scenePos);
		}
	}

	return match;
}

bool DrawingWidget::itemMatchesRect(DrawingItem* item, const QRectF& rect, Qt::ItemSelectionMode mode) const
{
	bool match = false;

	if (item && item->isVisible())
	{
		// Check item boundingRect or shape
		switch (mode)
		{
		case Qt::IntersectsItemShape:
			match = item->shape().intersects(item->mapFromScene(rect));
			break;
		case Qt::ContainsItemShape:
			match = rect.contains(item->mapToScene(item->shape().boundingRect()));
			break;
		case Qt::IntersectsItemBoundingRect:
			match = rect.intersects(item->mapToScene(item->boundingRect()));
			break;
		default:	// Qt::ContainsItemBoundingRect
			match = rect.contains(item->mapToScene(item->boundingRect()));
			break;
		}

		// Check item points
		if (!match && item->isSelected())
		{
			QList<DrawingItemPoint*> itemPoints = item->points();
			for(auto pointIter = itemPoints.begin(); !match && pointIter != itemPoints.end(); pointIter++)
			{
				if (mode == Qt::IntersectsItemBoundingRect || mode == Qt::IntersectsItemShape)
					match = rect.intersects((*pointIter)->sceneRect());
				else
					match = rect.contains((*pointIter)->sceneRect());
			}
		}
	}

	return match;
}

//==================================================================================================

void DrawingWidget::recalculateContentSize(const QRectF& targetSceneRect)
{
	QRectF targetRect;
	if (targetSceneRect.isValid())
	{
		targetRect.setLeft(qMin(targetSceneRect.left(), mSceneRect.left()));
		targetRect.setTop(qMin(targetSceneRect.top(), mSceneRect.top()));
		targetRect.setRight(qMax(targetSceneRect.right(), mSceneRect.right()));
		targetRect.setBottom(qMax(targetSceneRect.bottom(), mSceneRect.bottom()));
	}
	else targetRect = mSceneRect;

	int contentWidth = qRound(targetRect.width() * mScale);
	int contentHeight = qRound(targetRect.height() * mScale);
	int viewportWidth = maximumViewportSize().width();
	int viewportHeight = maximumViewportSize().height();
	int scrollBarExtent = style()->pixelMetric(QStyle::PM_ScrollBarExtent, 0, this);

	// Set scroll bar range
	if (contentWidth > viewportWidth)
	{
		int contentLeft = (targetRect.left() - mSceneRect.left()) * mScale;

		if (verticalScrollBarPolicy() == Qt::ScrollBarAsNeeded) viewportWidth -= scrollBarExtent;

		horizontalScrollBar()->setRange(contentLeft - 1, contentLeft + contentWidth - viewportWidth + 1);
		horizontalScrollBar()->setSingleStep(viewportWidth / 80);
		horizontalScrollBar()->setPageStep(viewportWidth);
	}
	else horizontalScrollBar()->setRange(0, 0);

	if (contentHeight > viewportHeight)
	{
		int contentTop = (targetRect.top() - mSceneRect.top()) * mScale;

		if (horizontalScrollBarPolicy() == Qt::ScrollBarAsNeeded) viewportHeight -= scrollBarExtent;

		verticalScrollBar()->setRange(contentTop - 1, contentTop + contentHeight - viewportHeight + 1);
		verticalScrollBar()->setSingleStep(viewportHeight / 80);
		verticalScrollBar()->setPageStep(viewportHeight);
	}
	else verticalScrollBar()->setRange(0, 0);

	// Recalculate transforms
	qreal dx = -targetRect.left() * mScale;
	qreal dy = -targetRect.top() * mScale;

	if (horizontalScrollBar()->maximum() <= horizontalScrollBar()->minimum())
		dx += -(targetRect.width() * mScale - viewportWidth) / 2;
	if (verticalScrollBar()->maximum() <= verticalScrollBar()->minimum())
		dy += -(targetRect.height() * mScale - viewportHeight) / 2;

	mViewportTransform = QTransform();
	mViewportTransform.translate(qRound(dx), qRound(dy));
	mViewportTransform.scale(mScale, mScale);

	mSceneTransform = mViewportTransform.inverted();
}

//==================================================================================================

void DrawingWidget::updateMouseInfo(const QPointF& pos)
{
	emit mouseInfoChanged("(" + QString::number(pos.x()) + "," + QString::number(pos.y()) + ")");
}

void DrawingWidget::updateMouseInfo(const QPointF& p1, const QPointF& p2)
{
	QString mouseInfoText;
	QPointF delta = p2 - p1;

	mouseInfoText += "(" + QString::number(p1.x()) + "," + QString::number(p1.y()) + ")";
	mouseInfoText += " - (" + QString::number(p2.x()) + "," + QString::number(p2.y()) + ")";
	mouseInfoText += "  " + QString(QChar(0x394)) + " = (" + QString::number(delta.x()) + "," + QString::number(delta.y()) + ")";

	emit mouseInfoChanged(mouseInfoText);
}

//==================================================================================================

void DrawingWidget::addActions()
{
	addAction("Undo", this, SLOT(undo()), ":/icons/oxygen/edit-undo.png", "Ctrl+Z");
	addAction("Redo", this, SLOT(redo()), ":/icons/oxygen/edit-redo.png", "Ctrl+Shift+Z");
	addAction("Cut", this, SLOT(cut()), ":/icons/oxygen/edit-cut.png", "Ctrl+X");
	addAction("Copy", this, SLOT(copy()), ":/icons/oxygen/edit-copy.png", "Ctrl+C");
	addAction("Paste", this, SLOT(paste()), ":/icons/oxygen/edit-paste.png", "Ctrl+V");
	addAction("Delete", this, SLOT(deleteSelection()), ":/icons/oxygen/edit-delete.png", "Delete");
	addAction("Select All", this, SLOT(selectAll()), ":/icons/oxygen/edit-select-all.png", "Ctrl+A");
	addAction("Select None", this, SLOT(selectNone()), "", "Ctrl+Shift+A");

	addAction("Rotate", this, SLOT(rotateSelection()), ":/icons/oxygen/object-rotate-right.png", "R");
	addAction("Rotate Back", this, SLOT(rotateBackSelection()), ":/icons/oxygen/object-rotate-left.png", "Shift+R");
	addAction("Flip", this, SLOT(flipSelection()), ":/icons/oxygen/object-flip-horizontal.png", "F");

	addAction("Bring Forward", this, SLOT(bringForward()), ":/icons/oxygen/object-bring-forward.png");
	addAction("Send Backward", this, SLOT(sendBackward()), ":/icons/oxygen/object-send-backward.png");
	addAction("Bring to Front", this, SLOT(bringToFront()), ":/icons/oxygen/object-bring-to-front.png");
	addAction("Send to Back", this, SLOT(sendToBack()), ":/icons/oxygen/object-send-to-back.png");

	addAction("Insert Point", this, SLOT(insertItemPoint()), "");
	addAction("Remove Point", this, SLOT(removeItemPoint()), "");

	addAction("Group", this, SLOT(group()), ":/icons/oxygen/merge.png", "Ctrl+G");
	addAction("Ungroup", this, SLOT(ungroup()), ":/icons/oxygen/split.png", "Ctrl+Shift+G");

	addAction("Zoom In", this, SLOT(zoomIn()), ":/icons/oxygen/zoom-in.png", ".");
	addAction("Zoom Out", this, SLOT(zoomOut()), ":/icons/oxygen/zoom-out.png", ",");
	addAction("Zoom Fit", this, SLOT(zoomFit()), ":/icons/oxygen/zoom-fit-best.png", "/");

	addAction("Properties...", this, SLOT(properties()), ":/icons/oxygen/games-config-board.png");
}

void DrawingWidget::createContextMenu()
{
	QList<QAction*> actions = DrawingWidget::actions();

	mSingleItemContextMenu.addAction(actions[PropertiesAction]);
	mSingleItemContextMenu.addSeparator();
	mSingleItemContextMenu.addAction(actions[RotateAction]);
	mSingleItemContextMenu.addAction(actions[RotateBackAction]);
	mSingleItemContextMenu.addAction(actions[FlipAction]);
	mSingleItemContextMenu.addAction(actions[DeleteAction]);
	mSingleItemContextMenu.addSeparator();
	mSingleItemContextMenu.addAction(actions[GroupAction]);
	mSingleItemContextMenu.addAction(actions[UngroupAction]);
	mSingleItemContextMenu.addSeparator();
	mSingleItemContextMenu.addAction(actions[CutAction]);
	mSingleItemContextMenu.addAction(actions[CopyAction]);
	mSingleItemContextMenu.addAction(actions[PasteAction]);

	mSinglePolyItemContextMenu.addAction(actions[PropertiesAction]);
	mSinglePolyItemContextMenu.addSeparator();
	mSinglePolyItemContextMenu.addAction(actions[InsertPointAction]);
	mSinglePolyItemContextMenu.addAction(actions[RemovePointAction]);
	mSinglePolyItemContextMenu.addSeparator();
	mSinglePolyItemContextMenu.addAction(actions[RotateAction]);
	mSinglePolyItemContextMenu.addAction(actions[RotateBackAction]);
	mSinglePolyItemContextMenu.addAction(actions[FlipAction]);
	mSinglePolyItemContextMenu.addAction(actions[DeleteAction]);
	mSinglePolyItemContextMenu.addSeparator();
	mSinglePolyItemContextMenu.addAction(actions[CutAction]);
	mSinglePolyItemContextMenu.addAction(actions[CopyAction]);
	mSinglePolyItemContextMenu.addAction(actions[PasteAction]);

	mMultipleItemContextMenu.addAction(actions[PropertiesAction]);
	mMultipleItemContextMenu.addSeparator();
	mMultipleItemContextMenu.addAction(actions[RotateAction]);
	mMultipleItemContextMenu.addAction(actions[RotateBackAction]);
	mMultipleItemContextMenu.addAction(actions[FlipAction]);
	mMultipleItemContextMenu.addAction(actions[DeleteAction]);
	mMultipleItemContextMenu.addSeparator();
	mMultipleItemContextMenu.addAction(actions[GroupAction]);
	mMultipleItemContextMenu.addAction(actions[UngroupAction]);
	mMultipleItemContextMenu.addSeparator();
	mMultipleItemContextMenu.addAction(actions[CutAction]);
	mMultipleItemContextMenu.addAction(actions[CopyAction]);
	mMultipleItemContextMenu.addAction(actions[PasteAction]);

	mDrawingContextMenu.addAction(actions[PropertiesAction]);
	mDrawingContextMenu.addSeparator();
	mDrawingContextMenu.addAction(actions[UndoAction]);
	mDrawingContextMenu.addAction(actions[RedoAction]);
	mDrawingContextMenu.addSeparator();
	mDrawingContextMenu.addAction(actions[CutAction]);
	mDrawingContextMenu.addAction(actions[CopyAction]);
	mDrawingContextMenu.addAction(actions[PasteAction]);
	mDrawingContextMenu.addSeparator();
	mDrawingContextMenu.addAction(actions[ZoomInAction]);
	mDrawingContextMenu.addAction(actions[ZoomOutAction]);
	mDrawingContextMenu.addAction(actions[ZoomFitAction]);
}

QAction* DrawingWidget::addAction(const QString& text, QObject* slotObj, const char* slotFunction,
	const QString& iconPath, const QString& shortcut)
{
	QAction* action = new QAction(text, this);
	if (slotObj) connect(action, SIGNAL(triggered()), slotObj, slotFunction);

	if (!iconPath.isEmpty()) action->setIcon(QIcon(iconPath));
	if (!shortcut.isEmpty()) action->setShortcut(QKeySequence(shortcut));

	QAbstractScrollArea::addAction(action);
	return action;
}
