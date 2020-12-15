// DrawingWidget.cpp
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

#include "DrawingWidget.h"
#include "DrawingCurveItem.h"
#include "DrawingEllipseItem.h"
#include "DrawingItemGroup.h"
#include "DrawingLineItem.h"
#include "DrawingPathItem.h"
#include "DrawingPolygonItem.h"
#include "DrawingPolylineItem.h"
#include "DrawingRectItem.h"
#include "DrawingTextEllipseItem.h"
#include "DrawingTextItem.h"
#include "DrawingTextRectItem.h"
#include "DrawingUndo.h"
#include <QApplication>
#include <QClipboard>
#include <QMouseEvent>

DrawingWidget::DrawingWidget(QWidget* parent) : DrawingCanvas(parent)
{
	mSelectedPoint = nullptr;
	mMouseDownItem = nullptr;
	mMouseDownPoint = nullptr;
	mFocusItem = nullptr;

	mMouseLeftDragged = false;
	mDefaultMouseState = MouseReady;
	mRightMouseDownItem = nullptr;

	mUndoStack.setUndoLimit(64);
	connect(&mUndoStack, SIGNAL(cleanChanged(bool)), this, SIGNAL(cleanChanged(bool)));
	connect(&mUndoStack, SIGNAL(canRedoChanged(bool)), this, SIGNAL(canRedoChanged(bool)));
	connect(&mUndoStack, SIGNAL(canUndoChanged(bool)), this, SIGNAL(canUndoChanged(bool)));

	addActions();
	createContextMenus();

	connect(this, SIGNAL(gridChanged(qreal)), this, SLOT(updateGridProperty(qreal)));
	connect(this, SIGNAL(currentItemsChanged(const QList<DrawingItem*>&)), this, SLOT(updateActionsFromSelection()));
	connect(this, SIGNAL(currentItemsChanged(const QList<DrawingItem*>&)), this, SLOT(updateSelectionCenter()));
	connect(this, SIGNAL(itemsPropertiesChanged(const QList<DrawingItem*>&)), this, SLOT(updateSelectionCenter()));
}

DrawingWidget::~DrawingWidget()
{
	clearItems();
	while (!mPathItems.isEmpty()) delete mPathItems.takeFirst();
}

//==================================================================================================

void DrawingWidget::addItem(DrawingItem* item)
{
	if (item && item->mWidget == nullptr)
	{
		mItems.append(item);
		item->mWidget = this;
	}
}

void DrawingWidget::insertItem(int index, DrawingItem* item)
{
	if (item && item->mWidget == nullptr)
	{
		mItems.insert(index, item);
		item->mWidget = this;
	}
}

void DrawingWidget::removeItem(DrawingItem* item)
{
	if (item && item->mWidget == this)
	{
		mItems.removeAll(item);
		item->mWidget = nullptr;
	}
}

void DrawingWidget::clearItems()
{
	setDefaultMode();

	mMouseDownItem = nullptr;
	mMouseDownPoint = nullptr;
	mFocusItem = nullptr;
	mDefaultInitialPositions.clear();
	mRightMouseDownItem = nullptr;

	DrawingItem* item = nullptr;

	while (!mItems.empty())
	{
		item = mItems.first();
		removeItem(item);
		delete item;
		item = nullptr;
	}
}

QList<DrawingItem*> DrawingWidget::items() const
{
	return mItems;
}

//==================================================================================================

DrawingItem* DrawingWidget::itemAt(const QPointF& position) const
{
	DrawingItem* item = nullptr;

	// Favor selected items
	auto selectedItemIter = mSelectedItems.end();
	while (item == nullptr && selectedItemIter != mSelectedItems.begin())
	{
		selectedItemIter--;
		if (itemContainsPoint(*selectedItemIter, position)) item = *selectedItemIter;
	}

	// Search all visible items
	auto itemIter = mItems.end();
	while (item == nullptr && itemIter != mItems.begin())
	{
		itemIter--;
		if (itemContainsPoint(*itemIter, position)) item = *itemIter;
	}

	return item;
}

QList<DrawingItem*> DrawingWidget::items(const QPointF& position) const
{
	QList<DrawingItem*> items;

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		if (itemContainsPoint(*itemIter, position)) items.append(*itemIter);
	}

	return items;
}

QList<DrawingItem*> DrawingWidget::items(const QRectF& rect) const
{
	QList<DrawingItem*> items;

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		if (itemIsWithinRect(*itemIter, rect)) items.append(*itemIter);
	}

	return items;
}

QList<DrawingItem*> DrawingWidget::items(const QPainterPath& path) const
{
	QList<DrawingItem*> items;

	for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
	{
		if (itemIsWithinPath(*itemIter, path)) items.append(*itemIter);
	}

	return items;
}

bool DrawingWidget::itemContainsPoint(DrawingItem* item, const QPointF& position) const
{
	bool match = false;

	if (item)
	{
		// Check item shape
		match = itemAdjustedShape(item).contains(item->mapFromScene(position));

		// Check item points
		if (!match && mSelectedItems.contains(item))
		{
			QList<DrawingItemPoint*> itemPoints = item->points();
			QRectF pointSceneRect;

			for(auto pointIter = itemPoints.begin(); !match && pointIter != itemPoints.end(); pointIter++)
			{
				pointSceneRect = mapToScene(pointRect(*pointIter));
				match = pointSceneRect.contains(position);
			}
		}
	}

	return match;
}

bool DrawingWidget::itemIsWithinRect(DrawingItem* item, const QRectF& rect) const
{
	bool match = false;

	if (item)
	{
		// Check item bounding rect
		match = rect.contains(item->mapToScene(item->boundingRect()).boundingRect());

		// Check item points
		if (!match && mSelectedItems.contains(item))
		{
			QList<DrawingItemPoint*> itemPoints = item->points();
			QRectF pointSceneRect;

			for(auto pointIter = itemPoints.begin(), pointEnd = itemPoints.end();
				!match && pointIter != pointEnd; pointIter++)
			{
				pointSceneRect = mapToScene(pointRect(*pointIter));
				match = rect.contains(pointSceneRect);
			}
		}
	}

	return match;
}

bool DrawingWidget::itemIsWithinPath(DrawingItem* item, const QPainterPath& path) const
{
	bool match = false;

	if (item)
	{
		// Check item bounding rect
		match = path.contains(item->mapToScene(item->boundingRect()).boundingRect());

		// Check item points
		if (!match && mSelectedItems.contains(item))
		{
			QList<DrawingItemPoint*> itemPoints = item->points();
			QRectF pointSceneRect;

			for(auto pointIter = itemPoints.begin(), pointEnd = itemPoints.end();
				!match && pointIter != pointEnd; pointIter++)
			{
				pointSceneRect = mapToScene(pointRect(*pointIter));
				match = path.contains(pointSceneRect);
			}
		}
	}

	return match;
}

QPainterPath DrawingWidget::itemAdjustedShape(DrawingItem* item) const
{
	QPainterPath adjustedShape;

	if (item)
	{
		QHash<QString,QVariant> properties = item->properties();

		if (properties.contains("pen"))
		{
			QPen pen;
			QVariant variant = properties.value("pen");
			if (variant.canConvert<QPen>())
			{
				QPen pen = variant.value<QPen>();

				// Determine minimum pen width
				const int penWidthHint = 8;
				QPointF mappedPenSize = item->mapFromScene(item->position() +
					mapToScene(QPoint(penWidthHint, penWidthHint)) - mapToScene(QPoint(0, 0)));
				qreal minimumPenWidth = qMax(qAbs(mappedPenSize.x()), qAbs(mappedPenSize.y()));

				// Override item's default pen width if needed
				if (0 < pen.widthF() && pen.widthF() < minimumPenWidth)
				{
					QHash<QString,QVariant> newPenProperties;
					QPen newPen = pen;
					newPen.setWidthF(minimumPenWidth);
					newPenProperties["pen"] = newPen;
					item->setProperties(newPenProperties);

					adjustedShape = item->shape();

					newPenProperties["pen"] = pen;
					item->setProperties(newPenProperties);
				}
			}
		}

		if (adjustedShape.isEmpty()) adjustedShape = item->shape();
	}

	return adjustedShape;
}

QRect DrawingWidget::pointRect(DrawingItemPoint* point) const
{
	const QSize pointSizeHint(8 * devicePixelRatio(), 8 * devicePixelRatio());

	QRect viewRect;

	if (point && point->item())
	{
		QPoint centerPoint = mapFromScene(point->item()->mapToScene(point->position()));

		int hDelta = pointSizeHint.width() / 2 * devicePixelRatio();
		int vDelta = pointSizeHint.height() / 2 * devicePixelRatio();
		QPoint deltaPoint(hDelta, vDelta);

		viewRect = QRect(centerPoint - deltaPoint, centerPoint + deltaPoint);
	}

	return viewRect;
}

//==================================================================================================

void DrawingWidget::clearSelection()
{
	for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
		(*itemIter)->setSelected(false);
	mSelectedItems.clear();
	mSelectedPoint = nullptr;
	mSelectionCenter = QPointF();
}

QList<DrawingItem*> DrawingWidget::selectedItems() const
{
	return mSelectedItems;
}

DrawingItemPoint* DrawingWidget::selectedPoint() const
{
	return mSelectedPoint;
}

QPointF DrawingWidget::selectionCenter() const
{
	return mSelectionCenter;
}

//==================================================================================================

QList<DrawingItem*> DrawingWidget::placeItems() const
{
	return mPlaceItems;
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

void DrawingWidget::setProperties(const QHash<QString,QVariant>& properties)
{
	if (properties.contains("sceneRect"))
	{
		QVariant variant = properties.value("sceneRect");
		if (variant.canConvert<QRectF>())
		{
			setSceneRect(variant.value<QRectF>());
			zoomFit();
		}
	}

	if (properties.contains("backgroundBrush"))
	{
		QVariant variant = properties.value("backgroundBrush");
		if (variant.canConvert<QBrush>()) setBackgroundBrush(variant.value<QBrush>());
	}

	if (properties.contains("grid"))
	{
		QVariant variant = properties.value("grid");
		if (variant.canConvert<qreal>()) setGrid(variant.value<qreal>());
	}

	if (properties.contains("gridStyle"))
	{
		QVariant variant = properties.value("gridStyle");
		if (variant.canConvert<uint>()) setGridStyle((Drawing::GridStyle)variant.value<uint>());
	}

	if (properties.contains("gridBrush"))
	{
		QVariant variant = properties.value("gridBrush");
		if (variant.canConvert<QBrush>()) setGridBrush(variant.value<QBrush>());
	}

	if (properties.contains("gridSpacingMajor"))
	{
		QVariant variant = properties.value("gridSpacingMajor");
		if (variant.canConvert<int>()) setGridSpacing(variant.value<int>(), gridSpacingMinor());
	}

	if (properties.contains("gridSpacingMinor"))
	{
		QVariant variant = properties.value("gridSpacingMinor");
		if (variant.canConvert<int>()) setGridSpacing(gridSpacingMajor(), variant.value<int>());
	}

	if (properties.contains("dynamicGrid"))
	{
		QVariant variant = properties.value("dynamicGrid");
		if (variant.canConvert<qreal>()) setDynamicGrid(variant.value<qreal>());
	}

	if (properties.contains("dynamicGridEnabled"))
	{
		QVariant variant = properties.value("dynamicGridEnabled");
		if (variant.canConvert<bool>()) setDynamicGridEnabled(variant.value<bool>());
	}

	if (!properties.isEmpty()) emit propertiesChanged(properties);
}

QHash<QString,QVariant> DrawingWidget::properties() const
{
	QHash<QString,QVariant> properties;

	properties.insert("sceneRect", sceneRect());
	properties.insert("backgroundBrush", backgroundBrush());

	properties.insert("grid", grid());
	properties.insert("gridStyle", (uint)gridStyle());
	properties.insert("gridBrush", gridBrush());
	properties.insert("gridSpacingMajor", gridSpacingMajor());
	properties.insert("gridSpacingMinor", gridSpacingMinor());
	properties.insert("dynamicGrid", dynamicGrid());
	properties.insert("dynamicGridEnabled", isDynamicGridEnabled());

	return properties;
}

//==================================================================================================

void DrawingWidget::render(QPainter* painter)
{
	drawBackground(painter);
	drawItems(painter, mItems);
}

void DrawingWidget::renderExport(QPainter* painter)
{
	painter->setBrush(backgroundBrush());
	painter->setPen(Qt::NoPen);
	painter->drawRect(sceneRect());

	drawItems(painter, mItems);
}

//==================================================================================================

void DrawingWidget::writeToXml(QXmlStreamWriter* xml)
{
	if (xml)
	{
		QRectF sceneRect = this->sceneRect();
		xml->writeAttribute("view-left", QString::number(sceneRect.left()));
		xml->writeAttribute("view-top", QString::number(sceneRect.top()));
		xml->writeAttribute("view-width", QString::number(sceneRect.width()));
		xml->writeAttribute("view-height", QString::number(sceneRect.height()));

		writeBrushToXml(xml, "background", backgroundBrush());

		xml->writeAttribute("grid", QString::number(grid()));

		writeBrushToXml(xml, "grid", gridBrush());

		QString gridStyleStr;
		switch (gridStyle())
		{
		case Drawing::GridDots: gridStyleStr = "dotted"; break;
		case Drawing::GridLines: gridStyleStr = "lined"; break;
		case Drawing::GridGraphPaper: gridStyleStr = "graph-paper"; break;
		default: break;
		}
		xml->writeAttribute("grid-style", gridStyleStr);

		xml->writeAttribute("grid-spacing-major", QString::number(gridSpacingMajor()));
		xml->writeAttribute("grid-spacing-minor", QString::number(gridSpacingMinor()));

		xml->writeAttribute("dynamic-grid", QString::number(dynamicGrid()));
		xml->writeAttribute("dynamic-grid-enabled", isDynamicGridEnabled() ? "true" : "false");

		xml->writeStartElement("items");
		DrawingItem::factory.writeItemsToXml(xml, mItems);
		xml->writeEndElement();
	}
}

void DrawingWidget::readFromXml(QXmlStreamReader* xml)
{
	if (xml)
	{
		QXmlStreamAttributes attr = xml->attributes();

		QRectF sceneRect;
		if (attr.hasAttribute("view-left"))
			sceneRect.setLeft(attr.value("view-left").toDouble());
		if (attr.hasAttribute("view-top"))
			sceneRect.setTop(attr.value("view-top").toDouble());
		if (attr.hasAttribute("view-width"))
			sceneRect.setWidth(attr.value("view-width").toDouble());
		if (attr.hasAttribute("view-height"))
			sceneRect.setHeight(attr.value("view-height").toDouble());
		setSceneRect(sceneRect);

		QBrush backgroundBrush = Qt::white;
		readBrushFromXml(xml, "background", backgroundBrush);
		setBackgroundBrush(backgroundBrush);

		qreal grid = 0;
		if (attr.hasAttribute("grid")) grid = attr.value("grid").toDouble();
		setGrid(grid);

		QBrush gridBrush = QColor(0, 128, 128);
		readBrushFromXml(xml, "grid", gridBrush);
		setGridBrush(gridBrush);

		Drawing::GridStyle gridStyle = Drawing::GridNone;
		if (attr.hasAttribute("grid-style"))
		{
			QString gridStyleStr = attr.value("grid-style").toString().toLower();

			if (gridStyleStr == "dotted") gridStyle = Drawing::GridDots;
			else if (gridStyleStr == "lined") gridStyle = Drawing::GridLines;
			else if (gridStyleStr == "graph-paper") gridStyle = Drawing::GridGraphPaper;
		}
		setGridStyle(gridStyle);

		int gridSpacingMajor = 0, gridSpacingMinor = 0;
		if (attr.hasAttribute("grid-spacing-major"))
			gridSpacingMajor = attr.value("grid-spacing-major").toInt();
		if (attr.hasAttribute("grid-spacing-minor"))
			gridSpacingMinor = attr.value("grid-spacing-minor").toInt();
		setGridSpacing(gridSpacingMajor, gridSpacingMinor);

		if (attr.hasAttribute("dynamic-grid"))
			setDynamicGrid(attr.value("dynamic-grid").toDouble());
		else
			setDynamicGrid(1000);

		if (attr.hasAttribute("dynamic-grid-enabled"))
			setDynamicGridEnabled(attr.value("dynamic-grid-enabled").toString().toLower() == "true");
		else
			setDynamicGridEnabled(false);

		clearItems();
		while (xml->readNextStartElement())
		{
			if (xml->name() == "items")
			{
				QList<DrawingItem*> items = DrawingItem::factory.readItemsFromXml(xml);
				for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
					addItem(*itemIter);
			}
			else xml->skipCurrentElement();
		}
	}
}

//==================================================================================================

QAction* DrawingWidget::addPlaceAction(DrawingItem* item, const QString& text,
	const QString& iconPath, const QString& shortcut)
{
	QAction* action = nullptr;

	if (item)
	{
		action = addModeAction(text, iconPath, shortcut);

		DrawingPathItem* pathItem = dynamic_cast<DrawingPathItem*>(item);
		if (pathItem)
		{
			mPathItems.append(pathItem);
			action->setProperty("pathName", pathItem->pathName());
		}
		else
		{
			DrawingItem::factory.registerItem(item);
			action->setProperty("name", item->name());
		}
	}

	return action;
}

//==================================================================================================

void DrawingWidget::setDefaultMode()
{
	clearMode();
	DrawingCanvas::setDefaultMode();
}

void DrawingWidget::setScrollMode()
{
	clearMode();
	DrawingCanvas::setScrollMode();
}

void DrawingWidget::setZoomMode()
{
	clearMode();
	DrawingCanvas::setZoomMode();
}

void DrawingWidget::setPlaceMode(const QList<DrawingItem*>& items)
{
	if (!items.isEmpty())
	{
		QPointF centerPosition, deltaPosition;

		clearMode();

		setMode(Drawing::PlaceMode);
		setCursor(Qt::CrossCursor);

		mPlaceItems = items;

		// Move place items so that they are centered under the mouse cursor
		for(auto itemIter = mPlaceItems.begin(); itemIter != mPlaceItems.end(); itemIter++)
			centerPosition += (*itemIter)->mapToScene((*itemIter)->centerPosition());
		centerPosition /= mPlaceItems.size();

		deltaPosition = roundToGrid(mapToScene(mapFromGlobal(QCursor::pos())) - centerPosition);

		for(auto itemIter = mPlaceItems.begin(); itemIter != mPlaceItems.end(); itemIter++)
			(*itemIter)->setPosition((*itemIter)->position() + deltaPosition);

		emit currentItemsChanged(mPlaceItems);

		emit modeChanged(mode());
		viewport()->update();
	}
	else setDefaultMode();
}

void DrawingWidget::clearMode()
{
	clearSelection();

	if (!mPlaceItems.isEmpty())
	{
		while (!mPlaceItems.isEmpty()) delete mPlaceItems.takeFirst();
		emit currentItemsChanged(mPlaceItems);
	}
}

//==================================================================================================

void DrawingWidget::undo()
{
	if (mode() == Drawing::DefaultMode && mUndoStack.canUndo())
	{
		mUndoStack.undo();
		viewport()->update();
	}
}

void DrawingWidget::redo()
{
	if (mode() == Drawing::DefaultMode && mUndoStack.canRedo())
	{
		mUndoStack.redo();
		viewport()->update();
	}
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
	if (mode() == Drawing::DefaultMode && !mSelectedItems.isEmpty())
	{
		QClipboard* clipboard = QApplication::clipboard();
		if (clipboard)
			clipboard->setText(DrawingItem::factory.writeItemsToString(mSelectedItems));
	}
}

void DrawingWidget::paste()
{
	if (mode() == Drawing::DefaultMode)
	{
		QClipboard* clipboard = QApplication::clipboard();

		if (clipboard)
		{
			QList<DrawingItem*> newItems = DrawingItem::factory.readItemsFromString(clipboard->text());

			if (!newItems.isEmpty())
			{
				for(auto itemIter = newItems.begin(); itemIter != newItems.end(); itemIter++)
					(*itemIter)->setPlaceType(DrawingItem::PlaceByMouseRelease);

				selectNone();
				setPlaceMode(newItems);
			}
		}
	}
}

void DrawingWidget::deleteSelection()
{
	if (mode() == Drawing::DefaultMode)
	{
		if (!mSelectedItems.isEmpty())
		{
			QList<DrawingItem*> itemsToRemove = mSelectedItems;

			QUndoCommand* deleteCommand = new QUndoCommand("Delete Items");

			selectItemsCommand(QList<DrawingItem*>(), deleteCommand);
			removeItemsCommand(itemsToRemove, deleteCommand);

			mUndoStack.push(deleteCommand);

			viewport()->update();
		}
	}
	else setDefaultMode();
}

//==================================================================================================

void DrawingWidget::selectAll()
{
	if (mode() == Drawing::DefaultMode && mSelectedItems != mItems)
	{
		selectItemsCommand(mItems, true);
		viewport()->update();
	}
}

void DrawingWidget::selectNone()
{
	if (mode() == Drawing::DefaultMode && !mSelectedItems.isEmpty())
	{
		selectItemsCommand(QList<DrawingItem*>(), true);
		viewport()->update();
	}
}

//==================================================================================================

void DrawingWidget::moveSelection(const QPointF& delta)
{
	if (mode() == Drawing::DefaultMode && !mSelectedItems.isEmpty())
	{
		QHash<DrawingItem*,QPointF> newPositions;

		for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
			newPositions[*itemIter] = (*itemIter)->position() + delta;

		moveItemsCommand(mSelectedItems, newPositions, true);
		viewport()->update();
	}
}

void DrawingWidget::resizeSelection(DrawingItemPoint* point, const QPointF& position)
{
	if (mode() == Drawing::DefaultMode && mSelectedItems.size() == 1)
	{
		QList<DrawingItemPoint*> points = mSelectedItems.first()->points();
		if (point && point->isControlPoint() && points.contains(point) && points.size() >= 2)
		{
			resizeItemCommand(point, position, true, true);
			viewport()->update();
		}
	}
}

void DrawingWidget::rotateSelection()
{
	if (mode() == Drawing::DefaultMode && !mSelectedItems.isEmpty())
	{
		rotateItemsCommand(mSelectedItems, roundToGrid(mSelectionCenter));
		viewport()->update();
	}
	else if (mode() == Drawing::PlaceMode && !mPlaceItems.isEmpty())
	{
		QPointF position = roundToGrid(mapToScene(mapFromGlobal(QCursor::pos())));
		rotateItems(mPlaceItems, position);
		viewport()->update();
	}
}

void DrawingWidget::rotateBackSelection()
{
	if (mode() == Drawing::DefaultMode && !mSelectedItems.isEmpty())
	{
		rotateBackItemsCommand(mSelectedItems, roundToGrid(mSelectionCenter));
		viewport()->update();
	}
	else if (mode() == Drawing::PlaceMode && !mPlaceItems.isEmpty())
	{
		QPointF position = roundToGrid(mapToScene(mapFromGlobal(QCursor::pos())));
		rotateBackItems(mPlaceItems, position);
		viewport()->update();
	}
}

void DrawingWidget::flipSelectionHorizontal()
{
	if (mode() == Drawing::DefaultMode && !mSelectedItems.isEmpty())
	{
		flipItemsHorizontalCommand(mSelectedItems, roundToGrid(mSelectionCenter));
		viewport()->update();
	}
	else if (mode() == Drawing::PlaceMode && !mPlaceItems.isEmpty())
	{
		QPointF position = roundToGrid(mapToScene(mapFromGlobal(QCursor::pos())));
		flipItemsHorizontal(mPlaceItems, position);
		viewport()->update();
	}
}

void DrawingWidget::flipSelectionVertical()
{
	if (mode() == Drawing::DefaultMode && !mSelectedItems.isEmpty())
	{
		flipItemsVerticalCommand(mSelectedItems, roundToGrid(mSelectionCenter));
		viewport()->update();
	}
	else if (mode() == Drawing::PlaceMode && !mPlaceItems.isEmpty())
	{
		QPointF position = roundToGrid(mapToScene(mapFromGlobal(QCursor::pos())));
		flipItemsVertical(mPlaceItems, position);
		viewport()->update();
	}
}

//==================================================================================================

void DrawingWidget::bringSelectionForward()
{
	if (mode() == Drawing::DefaultMode && !mSelectedItems.isEmpty())
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

		reorderItemsCommand(itemsOrdered);
		viewport()->update();
	}
}

void DrawingWidget::sendSelectionBackward()
{
	if (mode() == Drawing::DefaultMode && !mSelectedItems.isEmpty())
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

		reorderItemsCommand(itemsOrdered);
		viewport()->update();
	}
}

void DrawingWidget::bringSelectionToFront()
{
	if (mode() == Drawing::DefaultMode && !mSelectedItems.isEmpty())
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

		reorderItemsCommand(itemsOrdered);
		viewport()->update();
	}
}

void DrawingWidget::sendSelectionToBack()
{
	if (mode() == Drawing::DefaultMode && !mSelectedItems.isEmpty())
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

		reorderItemsCommand(itemsOrdered);
		viewport()->update();
	}
}

//==================================================================================================

void DrawingWidget::groupSelection()
{
	if (mode() == Drawing::DefaultMode && mSelectedItems.size() > 1)
	{
		QUndoCommand* command = new QUndoCommand("Group Items");

		QList<DrawingItem*> itemsToGroup = mSelectedItems;
		QList<DrawingItem*> items = DrawingItem::copyItems(mSelectedItems);
		DrawingItemGroup* itemGroup = new DrawingItemGroup();
		QList<DrawingItem*> itemsToAdd;

		itemGroup->setPosition(items.first()->position());
		for(auto iter = items.begin(); iter != items.end(); iter++)
			(*iter)->setPosition(itemGroup->mapFromScene((*iter)->position()));
		itemGroup->setItems(items);
		itemsToAdd.append(itemGroup);

		selectItemsCommand(QList<DrawingItem*>(), true, command);
		removeItemsCommand(itemsToGroup, command);
		addItemsCommand(itemsToAdd, false, command);
		selectItemsCommand(itemsToAdd, true, command);

		mUndoStack.push(command);
		viewport()->update();
	}
}

void DrawingWidget::ungroupSelection()
{
	if (mode() == Drawing::DefaultMode && mSelectedItems.size() == 1)
	{
		DrawingItemGroup* itemGroup = dynamic_cast<DrawingItemGroup*>(mSelectedItems.first());
		if (itemGroup)
		{
			QUndoCommand* command = new QUndoCommand("Ungroup Items");
			QList<DrawingItem*> itemsToRemove;
			itemsToRemove.append(itemGroup);

			QList<DrawingItem*> items = DrawingItem::copyItems(itemGroup->items());
			for(auto iter = items.begin(); iter != items.end(); iter++)
			{
				(*iter)->setPosition(itemGroup->mapToScene((*iter)->position()));
				(*iter)->setTransform(itemGroup->transform(), true);
			}

			selectItemsCommand(QList<DrawingItem*>(), true, command);
			removeItemsCommand(itemsToRemove, command);
			addItemsCommand(items, false, command);
			selectItemsCommand(items, true, command);

			mUndoStack.push(command);
			viewport()->update();
		}
	}
}

//==================================================================================================

void DrawingWidget::insertItemPointInSelection()
{
	if (mode() == Drawing::DefaultMode && mSelectedItems.size() == 1)
	{
		DrawingItem* item = mSelectedItems.first();
		DrawingItemPoint* point = item->insertNewPoint(roundToGrid(mMouseDownScenePosition));

		if (point)
		{
			int pointIndex = item->points().indexOf(point);
			item->removePoint(point);
			insertPointCommand(item, point, pointIndex);
			viewport()->update();
		}
	}
}

void DrawingWidget::removeItemPointFromSelection()
{
	if (mode() == Drawing::DefaultMode && mSelectedItems.size() == 1)
	{
		DrawingItem* item = mSelectedItems.first();
		int pointIndex = -1;
		DrawingItemPoint* point = item->removeExistingPoint(roundToGrid(mMouseDownScenePosition), pointIndex);

		if (point)
		{
			item->insertPoint(pointIndex, point);

			QUndoCommand* removeCommand = new QUndoCommand("Remove Point");

			disconnectAll(point, removeCommand);
			removePointCommand(item, point, removeCommand);

			mUndoStack.push(removeCommand);
			viewport()->update();
		}
	}
}

//==================================================================================================

void DrawingWidget::updateItemsProperties(const QHash< DrawingItem*, QHash<QString,QVariant> >& properties)
{
	if (!mSelectedItems.isEmpty())
	{
		QUndoCommand* propertiesCommand = nullptr;
		QHash< DrawingItem*, QHash<QString,QVariant> > remainingPropertiesToUpdate;

		DrawingItem* firstSelectedItem = mSelectedItems.first();
		QList<DrawingItemPoint*> firstSelectedItemPoints = firstSelectedItem->points();

		for(auto iter = properties.begin(); iter != properties.end(); iter++)
		{
			if (mSelectedItems.contains(iter.key()))
			{
				QHash<QString,QVariant> itemProperties = iter.value();
				QHash<QString,QVariant> remainingItemPropertiesToUpdate = itemProperties;

				for(auto propIter = itemProperties.begin(); propIter != itemProperties.end(); propIter++)
				{
					if (itemProperties.contains("position"))
					{
						if (!propertiesCommand) propertiesCommand = new QUndoCommand("Set Items' Properties");

						QVariant variant = itemProperties.value("position");
						if (variant.canConvert<QPointF>())
						{
							QPointF delta = variant.value<QPointF>() - firstSelectedItem->position();

							QHash<DrawingItem*,QPointF> newPositions;

							for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
								newPositions[*itemIter] = (*itemIter)->position() + delta;

							moveItemsCommand(mSelectedItems, newPositions, true, propertiesCommand);
						}

						remainingItemPropertiesToUpdate.remove("position");
					}

					if (itemProperties.contains("line") && firstSelectedItemPoints.size() > 2)
					{
						if (!propertiesCommand) propertiesCommand = new QUndoCommand("Set Items' Properties");

						QVariant variant = itemProperties.value("line");
						if (variant.canConvert<QLineF>())
						{
							QLineF line = variant.value<QLineF>();
							DrawingItemPoint* startPoint = firstSelectedItemPoints[0];
							DrawingItemPoint* endPoint = firstSelectedItemPoints[1];

							if (firstSelectedItem->mapToScene(startPoint->position()) != line.p1())
								resizeItemCommand(startPoint, line.p1(), true, true, propertiesCommand);
							if (firstSelectedItem->mapToScene(endPoint->position()) != line.p2())
								resizeItemCommand(endPoint, line.p2(), true, true, propertiesCommand);
						}

						remainingItemPropertiesToUpdate.remove("line");
					}

					if (itemProperties.contains("rect"))
					{
						if (!propertiesCommand) propertiesCommand = new QUndoCommand("Set Items' Properties");

						QVariant variant = itemProperties.value("rect");
						if (variant.canConvert<QRectF>() && firstSelectedItemPoints.size() > 2)
						{
							QRectF rect = variant.value<QRectF>();
							DrawingItemPoint* topLeftPoint = firstSelectedItemPoints[0];
							DrawingItemPoint* bottomRightPoint = firstSelectedItemPoints[1];

							if (firstSelectedItem->mapToScene(topLeftPoint->position()) != rect.topLeft())
								resizeItemCommand(topLeftPoint, rect.topLeft(), true, true, propertiesCommand);
							if (firstSelectedItem->mapToScene(bottomRightPoint->position()) != rect.bottomRight())
								resizeItemCommand(bottomRightPoint, rect.bottomRight(), true, true, propertiesCommand);
						}

						remainingItemPropertiesToUpdate.remove("rect");
					}

					if (itemProperties.contains("ellipse"))
					{
						if (!propertiesCommand) propertiesCommand = new QUndoCommand("Set Items' Properties");

						QVariant variant = itemProperties.value("ellipse");
						if (variant.canConvert<QRectF>() && firstSelectedItemPoints.size() > 2)
						{
							QRectF ellipse = variant.value<QRectF>();
							DrawingItemPoint* topLeftPoint = firstSelectedItemPoints[0];
							DrawingItemPoint* bottomRightPoint = firstSelectedItemPoints[1];

							if (firstSelectedItem->mapToScene(topLeftPoint->position()) != ellipse.topLeft())
								resizeItemCommand(topLeftPoint, ellipse.topLeft(), true, true, propertiesCommand);
							if (firstSelectedItem->mapToScene(bottomRightPoint->position()) != ellipse.bottomRight())
								resizeItemCommand(bottomRightPoint, ellipse.bottomRight(), true, true, propertiesCommand);
						}

						remainingItemPropertiesToUpdate.remove("ellipse");
					}

					if (itemProperties.contains("polyline"))
					{
						if (!propertiesCommand) propertiesCommand = new QUndoCommand("Set Items' Properties");

						QVariant variant = itemProperties.value("polyline");
						if (variant.canConvert<QPolygonF>())
						{
							QPolygonF polyline = variant.value<QPolygonF>();
							for(int i = 0; i < firstSelectedItemPoints.size() && i < polyline.size(); i++)
							{
								if (firstSelectedItem->mapToScene(firstSelectedItemPoints[i]->position()) != polyline[i])
									resizeItemCommand(firstSelectedItemPoints[i], polyline[i], true, true, propertiesCommand);
							}
						}

						remainingItemPropertiesToUpdate.remove("polyline");
					}

					if (itemProperties.contains("polygon"))
					{
						if (!propertiesCommand) propertiesCommand = new QUndoCommand("Set Items' Properties");

						QVariant variant = itemProperties.value("polygon");
						if (variant.canConvert<QPolygonF>())
						{
							QPolygonF polygon = variant.value<QPolygonF>();
							for(int i = 0; i < firstSelectedItemPoints.size() && i < polygon.size(); i++)
							{
								if (firstSelectedItem->mapToScene(firstSelectedItemPoints[i]->position()) != polygon[i])
									resizeItemCommand(firstSelectedItemPoints[i], polygon[i], true, true, propertiesCommand);
							}
						}

						remainingItemPropertiesToUpdate.remove("polygon");
					}

					if (itemProperties.contains("curve"))
					{
						if (!propertiesCommand) propertiesCommand = new QUndoCommand("Set Items' Properties");

						QVariant variant = itemProperties.value("curve");
						if (variant.canConvert<QPolygonF>())
						{
							QPolygonF curve = variant.value<QPolygonF>();
							if (firstSelectedItemPoints.size() >= 4 && curve.size() >= 4)
							{
								DrawingItemPoint* startPoint = firstSelectedItemPoints[0];
								DrawingItemPoint* startControlPoint = firstSelectedItemPoints[2];
								DrawingItemPoint* endControlPoint = firstSelectedItemPoints[3];
								DrawingItemPoint* endPoint = firstSelectedItemPoints[1];

								if (firstSelectedItem->mapToScene(startPoint->position()) != curve[0])
									resizeItemCommand(startPoint, curve[0], true, true, propertiesCommand);
								if (firstSelectedItem->mapToScene(startControlPoint->position()) != curve[1])
									resizeItemCommand(startControlPoint, curve[1], true, true, propertiesCommand);
								if (firstSelectedItem->mapToScene(endPoint->position()) != curve[3])
									resizeItemCommand(endPoint, curve[3], true, true, propertiesCommand);
								if (firstSelectedItem->mapToScene(endControlPoint->position()) != curve[2])
									resizeItemCommand(endControlPoint, curve[2], true, true, propertiesCommand);
							}
						}

						remainingItemPropertiesToUpdate.remove("curve");
					}
				}

				if (!remainingItemPropertiesToUpdate.isEmpty())
					remainingPropertiesToUpdate.insert(iter.key(), remainingItemPropertiesToUpdate);
			}
		}

		if (!remainingPropertiesToUpdate.isEmpty())
			setItemsPropertiesCommand(remainingPropertiesToUpdate, propertiesCommand);

		if (propertiesCommand) mUndoStack.push(propertiesCommand);
		viewport()->update();
	}
}

void DrawingWidget::updateProperties(const QHash<QString,QVariant>& properties)
{
	bool propertyChanged = false;

	QHash<QString,QVariant> originalProperties = this->properties();
	for(auto propIter = properties.begin(); !propertyChanged && propIter != properties.end(); propIter++)
	{
		if (originalProperties.contains(propIter.key()))
			propertyChanged = (propIter.value() != originalProperties.value(propIter.key()));
	}

	if (propertyChanged)
	{
		setPropertiesCommand(properties);
		viewport()->update();
	}
}

//==================================================================================================

void DrawingWidget::addItems(const QList<DrawingItem*>& items)
{
	for(auto itemIter = items.begin(), itemEnd = items.end(); itemIter != itemEnd; itemIter++)
		addItem(*itemIter);
}

void DrawingWidget::insertItems(const QList<DrawingItem*>& items,
	const QHash<DrawingItem*,int>& indices)
{
	for(auto itemIter = items.begin(), itemEnd = items.end(); itemIter != itemEnd; itemIter++)
		insertItem(indices[*itemIter], *itemIter);
}

void DrawingWidget::removeItems(const QList<DrawingItem*>& items)
{
	for(auto itemIter = items.begin(), itemEnd = items.end(); itemIter != itemEnd; itemIter++)
		removeItem(*itemIter);
}

void DrawingWidget::moveItems(const QList<DrawingItem*>& items,
	const QHash<DrawingItem*,QPointF>& positions)
{
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		(*itemIter)->move(positions[*itemIter]);

	emit itemsPropertiesChanged(items);
}

void DrawingWidget::resizeItem(DrawingItemPoint* point, const QPointF& position)
{
	if (point && point->item())
	{
		point->item()->resize(point, position);

		QList<DrawingItem*> items;
		items.append(point->item());
		emit itemsPropertiesChanged(items);
	}
}

void DrawingWidget::rotateItems(const QList<DrawingItem*>& items, const QPointF& position)
{
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		(*itemIter)->rotate(position);

	emit itemsPropertiesChanged(items);
}

void DrawingWidget::rotateBackItems(const QList<DrawingItem*>& items, const QPointF& position)
{
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		(*itemIter)->rotateBack(position);

	emit itemsPropertiesChanged(items);
}

void DrawingWidget::flipItemsHorizontal(const QList<DrawingItem*>& items, const QPointF& position)
{
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		(*itemIter)->flipHorizontal(position);

	emit itemsPropertiesChanged(items);
}

void DrawingWidget::flipItemsVertical(const QList<DrawingItem*>& items, const QPointF& position)
{
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
		(*itemIter)->flipVertical(position);

	emit itemsPropertiesChanged(items);
}

void DrawingWidget::selectItems(const QList<DrawingItem*>& items)
{
	for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
		(*itemIter)->setSelected(false);

	mSelectedItems = items;

	for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
		(*itemIter)->setSelected(true);

	emit currentItemsChanged(mSelectedItems);
}

void DrawingWidget::reorderItems(const QList<DrawingItem*>& items)
{
	mItems = items;
}

void DrawingWidget::insertItemPoint(DrawingItem* item, DrawingItemPoint* point, int index)
{
	if (item && point)
	{
		QList<DrawingItem*> items;
		items.append(item);

		item->insertPoint(index, point);
		item->resize(point, item->mapToScene(point->position()));

		emit itemsPropertiesChanged(items);
	}
}

void DrawingWidget::removeItemPoint(DrawingItem* item, DrawingItemPoint* point)
{
	if (item && point)
	{
		QList<DrawingItem*> items;
		items.append(item);

		item->removePoint(point);
		item->resize(point, item->mapToScene(point->position()));

		emit itemsPropertiesChanged(items);
	}
}

void DrawingWidget::connectPoints(DrawingItemPoint* point1, DrawingItemPoint* point2)
{
	if (point1 && point2)
	{
		point1->addConnection(point2);
		point2->addConnection(point1);
	}
}

void DrawingWidget::disconnectPoints(DrawingItemPoint* point1, DrawingItemPoint* point2)
{
	if (point1 && point2)
	{
		point1->removeConnection(point2);
		point2->removeConnection(point1);
	}
}

void DrawingWidget::setItemsProperties(const QHash< DrawingItem*, QHash<QString,QVariant> >& properties)
{
	for(auto propIter = properties.begin(), propEnd = properties.end(); propIter != propEnd; propIter++)
		propIter.key()->setProperties(propIter.value());

	emit itemsPropertiesChanged(properties.keys());
}

//==================================================================================================

void DrawingWidget::setModeFromAction(QAction* action)
{
	QList<QAction*> modeActionList = modeActions();

	if (action)
	{
		if (action == modeActionList[DefaultModeAction]) setDefaultMode();
		else if (action == modeActionList[ScrollModeAction]) setScrollMode();
		else if (action == modeActionList[ZoomModeAction]) setZoomMode();
		else
		{
			DrawingItem* item = DrawingItem::factory.createItem(action->property("name").toString());

			if (item == nullptr)
			{
				QString pathName = action->property("pathName").toString();
				for(auto itemIter = mPathItems.begin(); item == nullptr && itemIter != mPathItems.end(); itemIter++)
				{
					if ((*itemIter)->pathName() == pathName) item = (*itemIter)->copy();
				}
			}

			if (item)
			{
				QList<DrawingItem*> items;
				items.append(item);
				setPlaceMode(items);
			}
			else setDefaultMode();
		}
	}
}

//==================================================================================================

void DrawingWidget::mousePressEvent(QMouseEvent* event)
{
	DrawingCanvas::mousePressEvent(event);

	if (event->button() == Qt::LeftButton)
	{
		// Update button down screen and scene position
		mMouseDownPosition = event->pos();
		mMouseDownScenePosition = mapToScene(mMouseDownPosition);
		mMouseLeftDragged = false;
		mDefaultRubberBandRect = QRect();

		switch (mode())
		{
		case Drawing::DefaultMode:
			// Update button down item
			mDefaultMouseState = MouseSelect;

			mMouseDownItem = itemAt(mMouseDownScenePosition);
			if (mMouseDownItem)
			{
				// Update initial item positions in case the user tries to move them
				mDefaultInitialPositions.clear();
				for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
					mDefaultInitialPositions[*itemIter] = (*itemIter)->position();

				if (mSelectedItems.size() == 1 && mSelectedItems.contains(mMouseDownItem))
				{
					// Determine if the mouse event means the user clicked on one of the mouse
					// down item's points
					mMouseDownPoint = pointAt(mMouseDownItem,
						mMouseDownItem->mapFromScene(mMouseDownScenePosition));
					if (mMouseDownPoint)
					{
						if (mMouseDownPoint->isControlPoint())
						{
							// Update initial point position in case the user tries to resize the item
							mMouseDownPointInitialPosition = mMouseDownItem->mapToScene(
								mMouseDownPoint->position());
						}
						else mMouseDownPoint = nullptr;
					}
				}
			}

			mFocusItem = mMouseDownItem;
			break;
		case Drawing::PlaceMode:
			// Nothing to do here.
			break;
		case Drawing::ScrollMode:
			// Handled in DrawingCanvas::mousePressEvent().
			break;
		case Drawing::ZoomMode:
			// Handled in DrawingCanvas::mousePressEvent().
			break;
		case Drawing::UserMode:
			// Nothing to do here.
			break;
		}
	}
	else if (event->button() == Qt::RightButton)
	{
		if (mode() == Drawing::DefaultMode)
		{
			mMouseDownPosition = event->pos();
			mMouseDownScenePosition = mapToScene(mMouseDownPosition);
			mRightMouseDownItem = itemAt(mMouseDownScenePosition);
		}
	}

	emit mouseInfoChanged("");
	viewport()->update();
}

void DrawingWidget::mouseMoveEvent(QMouseEvent* event)
{
	DrawingCanvas::mouseReleaseEvent(event);

	QPoint eventPos = event->pos();
	QPointF eventScenePos = mapToScene(eventPos);

	if (event->buttons() & Qt::LeftButton)
	{
		// Determine if the user has moved the mouse enough to consider this a drag event
		mMouseLeftDragged = (mMouseLeftDragged |
			((mMouseDownPosition - eventPos).manhattanLength() >= QApplication::startDragDistance()));
	}

	// By default, the mouse info text should show a range (i.e. mouseInfoPos1 != mouseInfoPos2) if
	// the user is clicking and dragging.  Otherwise, the mouse info text should show a single
	// point (the event scene pos).  This may be overridden below to show more info about the
	// current operation.
	QPointF mouseInfoPos1 = ((event->buttons() & Qt::LeftButton) && mMouseLeftDragged) ? mMouseDownPosition : eventScenePos;
	QPointF mouseInfoPos2 = eventScenePos;

	switch (mode())
	{
	case Drawing::DefaultMode:
		if (event->buttons() & Qt::LeftButton)
		{
			switch (mDefaultMouseState)
			{
			case MouseSelect:
				// Stay in the MouseSelect state until the mouse is dragged
				if (mMouseLeftDragged)
				{
					// If we clicked on an control point within a selected item and the item can be
					//   resized, then resize the item.
					// Otherwise, if we clicked on a selected item, move the item.
					// Otherwise, if we didn't click on an item, start drawing a rubber band for
					//   item selection.
					if (mMouseDownItem && mSelectedItems.contains(mMouseDownItem))
					{
						bool resizeItem = (mSelectedItems.size() == 1 &&
							mSelectedItems.first()->points().size() >= 2 &&
							mMouseDownPoint && mMouseDownPoint->isControlPoint());
						mDefaultMouseState = (resizeItem) ? MouseResizeItem : MouseMoveItems;
					}
					else mDefaultMouseState = MouseRubberBand;
				}
				break;
			case MouseMoveItems:
				// Move the selected items within the scene
				if (!mSelectedItems.isEmpty())
				{
					QHash<DrawingItem*,QPointF> newPositions;
					QPointF deltaPos = roundToGrid(eventScenePos - mMouseDownScenePosition);

					for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
						newPositions[*itemIter] = mDefaultInitialPositions[*itemIter] + deltaPos;

					moveItemsCommand(mSelectedItems, newPositions, false);

					// Mouse info should show the amount the first selected item is moved by
					mouseInfoPos1 = mDefaultInitialPositions[mSelectedItems.first()];
					mouseInfoPos2 = mouseInfoPos1 + deltaPos;
				}
				break;
			case MouseResizeItem:
				// Resize the selected item within the scene
				resizeItemCommand(mMouseDownPoint, roundToGrid(eventScenePos), false, true);
				// Mouse info should show the amount the item is resized by
				mouseInfoPos1 = mMouseDownPointInitialPosition;
				mouseInfoPos2 = roundToGrid(eventScenePos);
				break;
			case MouseRubberBand:
				// Update the rubber band rect to be used for item selection
				mDefaultRubberBandRect = QRect(eventPos, mMouseDownPosition).normalized();
				break;
			default:
				break;
			}
		}
		break;
	case Drawing::PlaceMode:
		if ((event->buttons() & Qt::LeftButton) && mPlaceItems.size() == 1 &&
			mPlaceItems.first()->placeType() == DrawingItem::PlaceByMousePressAndRelease &&
			mPlaceItems.first()->points().size() > 1)
		{
			resizeItem(mPlaceItems.first()->points()[1], roundToGrid(eventScenePos));

			mouseInfoPos1 = roundToGrid(mouseInfoPos1);
			mouseInfoPos2 = roundToGrid(mouseInfoPos2);
		}
		else
		{
			// Move the items in mPlaceItems within the scene relative to the center of those items
			QHash<DrawingItem*,QPointF> newPositions;
			QPointF deltaPosition, centerPosition;

			for(auto itemIter = mPlaceItems.begin(); itemIter != mPlaceItems.end(); itemIter++)
				centerPosition += (*itemIter)->mapToScene((*itemIter)->centerPosition());
			if (!mPlaceItems.isEmpty()) centerPosition /= mPlaceItems.size();

			deltaPosition = roundToGrid(eventScenePos - centerPosition);

			for(auto itemIter = mPlaceItems.begin(); itemIter != mPlaceItems.end(); itemIter++)
				newPositions[*itemIter] = (*itemIter)->position() + deltaPosition;

			moveItems(mPlaceItems, newPositions);

			mouseInfoPos1 = mouseInfoPos2 = roundToGrid(eventScenePos);
		}
		break;
	case Drawing::ScrollMode:
		// Handled in DrawingCanvas::mouseMoveEvent().
		break;
	case Drawing::ZoomMode:
		// Handled in DrawingCanvas::mouseMoveEvent().
		break;
	case Drawing::UserMode:
		// Nothing to do here.
		break;
	}

	// Update mouse info text
	if (mouseInfoPos1 != mouseInfoPos2)
		emit mouseInfoChanged(mouseInfoPos1, mouseInfoPos2);
	else
		emit mouseInfoChanged(mouseInfoPos1);

	if (event->buttons() != Qt::NoButton || mode() == Drawing::PlaceMode) viewport()->update();
}

void DrawingWidget::mouseReleaseEvent(QMouseEvent* event)
{
	Drawing::Mode originalMode = mode();

	DrawingCanvas::mouseReleaseEvent(event);

	QPoint eventPos = event->pos();
	QPointF eventScenePos = mapToScene(eventPos);

	if (event->button() == Qt::LeftButton)
	{
		switch (originalMode)
		{
		case Drawing::DefaultMode:
			switch (mDefaultMouseState)
			{
			case MouseSelect:
				// Select or deselect items as needed
				{
					bool controlDown = ((event->modifiers() & Qt::ControlModifier) != 0);
					QList<DrawingItem*> newSelection = (controlDown) ? mSelectedItems : QList<DrawingItem*>();

					if (mMouseDownItem)
					{
						if (controlDown && mSelectedItems.contains(mMouseDownItem))
							newSelection.removeAll(mMouseDownItem);
						else
							newSelection.append(mMouseDownItem);
					}

					if (mSelectedItems != newSelection) selectItemsCommand(newSelection, true);
				}
				break;
			case MouseMoveItems:
				// Move the selected items within the scene
				if (!mSelectedItems.isEmpty())
				{
					QHash<DrawingItem*,QPointF> newPositions;
					QPointF deltaPos = roundToGrid(eventScenePos - mMouseDownScenePosition);

					for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
						newPositions[*itemIter] = mDefaultInitialPositions[*itemIter] + deltaPos;

					moveItemsCommand(mSelectedItems, newPositions, true);
				}
				break;
			case MouseResizeItem:
				// Resize the selected item within the scene
				resizeItemCommand(mMouseDownPoint, roundToGrid(eventScenePos), true, true);
				break;
			case MouseRubberBand:
				// Select the items within the rubber band rect (if the rect is valid)
				if (mDefaultRubberBandRect.isValid())
				{
					QList<DrawingItem*> newSelection = items(mapToScene(mDefaultRubberBandRect));
					if (newSelection != mSelectedItems) selectItemsCommand(newSelection, true);
				}
				break;
			default:
				break;
			}
			break;
		case Drawing::PlaceMode:
			// Place the items within the scene, then create new copies to continue place mode
			if (mPlaceItems.size() > 1 ||
				(mPlaceItems.size() == 1 && mPlaceItems.first()->isValid()))
			{
				addItemsCommand(mPlaceItems, true);

				QList<DrawingItem*> newItems = DrawingItem::copyItems(mPlaceItems);
				mPlaceItems.clear();
				setPlaceMode(newItems);

				if (mPlaceItems.size() == 1)
				{
					DrawingItem* placeItem = mPlaceItems.first();

					if (placeItem->placeType() == DrawingItem::PlaceByMousePressAndRelease)
					{
						QList<DrawingItemPoint*> points = placeItem->points();
						for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
							(*pointIter)->setPosition(0, 0);
						placeItem->resize(points.first(), placeItem->mapToScene(QPointF(0, 0)));
					}
				}
			}
			break;
		case Drawing::ScrollMode:
			// Handled in DrawingCanvas::mouseReleaseEvent().
			break;
		case Drawing::ZoomMode:
			// Handled in DrawingCanvas::mouseReleaseEvent().
			break;
		case Drawing::UserMode:
			// Nothing to do here.
			break;
		}
	}
	else if (event->button() == Qt::RightButton)
	{
		if (originalMode == Drawing::DefaultMode)
		{
			// Handle context menu event
			if (mRightMouseDownItem && mSelectedItems.contains(mRightMouseDownItem))
			{
				if (mSelectedItems.size() == 1)
				{
					if (actions()[InsertPointAction]->isEnabled())
						mSinglePolyItemContextMenu.popup(event->globalPos() + QPoint(2, 2));
					else
						mSingleItemContextMenu.popup(event->globalPos() + QPoint(2, 2));
				}
				else mMultipleItemContextMenu.popup(event->globalPos() + QPoint(2, 2));
			}
			else
			{
				if (mRightMouseDownItem == nullptr) clearSelection();
				mDrawingContextMenu.popup(event->globalPos() + QPoint(2, 2));
			}
		}
		else setDefaultMode();
	}

	// Clear mouse state variables
	mDefaultMouseState = MouseReady;
	mDefaultRubberBandRect = QRect();

	emit mouseInfoChanged("");
	viewport()->update();
}

void DrawingWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	Drawing::Mode originalMode = mode();

	DrawingCanvas::mouseDoubleClickEvent(event);

	if (event->button() == Qt::LeftButton && originalMode == Drawing::DefaultMode && mMouseDownItem)
		emit propertiesTriggered();
}

//==================================================================================================

void DrawingWidget::drawMain(QPainter* painter)
{
	drawItems(painter, mItems);
	drawItems(painter, mPlaceItems);
	drawItemPoints(painter, mSelectedItems);
	drawHotpoints(painter, mSelectedItems + mPlaceItems);
	drawRubberBand(painter, mDefaultRubberBandRect);

	DrawingCanvas::drawMain(painter);
}

void DrawingWidget::drawItems(QPainter* painter, const QList<DrawingItem*>& items)
{
	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
	{
		painter->translate((*itemIter)->position());
		painter->setTransform((*itemIter)->transformInverted(), true);

		(*itemIter)->render(painter);

//		painter->save();
//		painter->setBrush(QColor(255, 0, 255, 128));
//		painter->setPen(QPen(QColor(255, 0, 255, 128), 1));
//		painter->drawPath(itemAdjustedShape(*itemIter));
//		painter->restore();

		painter->setTransform((*itemIter)->transform(), true);
		painter->translate(-(*itemIter)->position());
	}
}

void DrawingWidget::drawItemPoints(QPainter* painter, const QList<DrawingItem*>& items)
{
	QColor color = backgroundBrush().color();
	color.setRed(255 - color.red());
	color.setGreen(255 - color.green());
	color.setBlue(255 - color.blue());

	painter->save();

	painter->resetTransform();
	painter->setRenderHints(QPainter::Antialiasing, false);
	painter->setPen(QPen(color, 1));

	for(auto itemIter = items.begin(), itemEnd = items.end(); itemIter != itemEnd; itemIter++)
	{
		QList<DrawingItemPoint*> itemPoints = (*itemIter)->points();

		for(auto pointIter = itemPoints.begin(); pointIter != itemPoints.end(); pointIter++)
		{
			bool controlPoint = ((*pointIter)->isControlPoint() ||
				(*pointIter)->type() == DrawingItemPoint::None);
			bool connectionPoint = (*pointIter)->isConnectionPoint();

			if (controlPoint || connectionPoint)
			{
				QRect pointRect = DrawingWidget::pointRect(*pointIter).adjusted(1, 1, -2, -2);

				if (connectionPoint && !controlPoint)
					painter->setBrush(QColor(255, 255, 0));
				else
					painter->setBrush(QColor(0, 224, 0));

				painter->drawRect(pointRect);
			}
		}
	}

	painter->restore();
}

void DrawingWidget::drawHotpoints(QPainter* painter, const QList<DrawingItem*>& items)
{
	painter->save();

	painter->resetTransform();
	painter->setRenderHints(QPainter::Antialiasing, false);
	painter->setBrush(QColor(255, 128, 0, 128));
	painter->setPen(QPen(painter->brush(), 1));

	for(auto itemIter = items.begin(), itemEnd = items.end(); itemIter != itemEnd; itemIter++)
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
						if (shouldConnect(*pointIter, *otherItemPointIter))
						{
							QRect pointRect = DrawingWidget::pointRect(*pointIter);
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
}

//==================================================================================================

void DrawingWidget::updateGridProperty(qreal grid)
{
	QHash<QString,QVariant> properties;
	properties.insert("grid", grid);
	emit propertiesChanged(properties);
}

void DrawingWidget::updateActionsFromSelection()
{
	QList<QAction*> actionList = actions();

	bool canGroup = (mSelectedItems.size() > 1);
	bool canUngroup = false;
	bool canInsertRemovePoints = false;

	if (mSelectedItems.size() == 1)
	{
		DrawingItem* item = mSelectedItems.first();
		canUngroup = (dynamic_cast<DrawingItemGroup*>(item));
		canInsertRemovePoints = (dynamic_cast<DrawingPolygonItem*>(item) || dynamic_cast<DrawingPolylineItem*>(item));
	}

	actionList[GroupAction]->setEnabled(canGroup);
	actionList[UngroupAction]->setEnabled(canUngroup);
	actionList[InsertPointAction]->setEnabled(canInsertRemovePoints);
	actionList[RemovePointAction]->setEnabled(canInsertRemovePoints);
}

void DrawingWidget::updateSelectionCenter()
{
	mSelectionCenter = QPointF();

	if (!mSelectedItems.isEmpty())
	{
		for(auto itemIter = mSelectedItems.begin(); itemIter != mSelectedItems.end(); itemIter++)
			mSelectionCenter += (*itemIter)->mapToScene((*itemIter)->centerPosition());

		mSelectionCenter /= mSelectedItems.size();
	}
}

//==================================================================================================

void DrawingWidget::addItemsCommand(const QList<DrawingItem*>& items, bool place,
	QUndoCommand* command)
{
	DrawingAddItemsCommand* addCommand = new DrawingAddItemsCommand(this, items, command);

	addCommand->redo();
	if (place) placeItems(items, addCommand);
	addCommand->undo();

	if (!command) mUndoStack.push(addCommand);
}

void DrawingWidget::removeItemsCommand(const QList<DrawingItem*>& items, QUndoCommand* command)
{
	DrawingRemoveItemsCommand* removeCommand = new DrawingRemoveItemsCommand(this, items, command);

	removeCommand->redo();
	unplaceItems(items, removeCommand);
	removeCommand->undo();

	if (!command) mUndoStack.push(removeCommand);
}

void DrawingWidget::moveItemsCommand(const QList<DrawingItem*>& items,
	const QHash<DrawingItem*,QPointF>& positions, bool place, QUndoCommand* command)
{
	DrawingMoveItemsCommand* moveCommand =
		new DrawingMoveItemsCommand(this, items, positions, place, command);

	moveCommand->redo();
	tryToMaintainConnections(items, true, true, nullptr, moveCommand);
	if (place) placeItems(items, moveCommand);
	moveCommand->undo();

	if (!command) mUndoStack.push(moveCommand);
}

void DrawingWidget::resizeItemCommand(DrawingItemPoint* point, const QPointF& position, bool place,
	bool disconnect, QUndoCommand* command)
{
	if (point && point->item())
	{
		DrawingResizeItemCommand* resizeCommand =
			new DrawingResizeItemCommand(this, point, position, place, command);

		QList<DrawingItem*> resizeItems;
		resizeItems.append(point->item());

		resizeCommand->redo();
		if (disconnect) disconnectAll(point, resizeCommand);
		tryToMaintainConnections(resizeItems, true, !point->isFree(), point, resizeCommand);
		if (place) placeItems(resizeItems, resizeCommand);
		resizeCommand->undo();

		if (!command) mUndoStack.push(resizeCommand);
	}
}

void DrawingWidget::rotateItemsCommand(const QList<DrawingItem*>& items,
	const QPointF& position, QUndoCommand* command)
{
	DrawingRotateItemsCommand* rotateCommand =
		new DrawingRotateItemsCommand(this, items, position, command);

	rotateCommand->redo();
	tryToMaintainConnections(items, true, true, nullptr, rotateCommand);
	rotateCommand->undo();

	if (!command) mUndoStack.push(rotateCommand);
}

void DrawingWidget::rotateBackItemsCommand(const QList<DrawingItem*>& items,
	const QPointF& position, QUndoCommand* command)
{
	DrawingRotateBackItemsCommand* rotateCommand =
		new DrawingRotateBackItemsCommand(this, items, position, command);

	rotateCommand->redo();
	tryToMaintainConnections(items, true, true, nullptr, rotateCommand);
	rotateCommand->undo();

	if (!command) mUndoStack.push(rotateCommand);
}

void DrawingWidget::flipItemsHorizontalCommand(const QList<DrawingItem*>& items,
	const QPointF& position, QUndoCommand* command)
{
	DrawingFlipItemsHorizontalCommand* flipCommand =
		new DrawingFlipItemsHorizontalCommand(this, items, position, command);

	flipCommand->redo();
	tryToMaintainConnections(items, true, true, nullptr, flipCommand);
	flipCommand->undo();

	if (!command) mUndoStack.push(flipCommand);
}

void DrawingWidget::flipItemsVerticalCommand(const QList<DrawingItem*>& items,
	const QPointF& position, QUndoCommand* command)
{
	DrawingFlipItemsVerticalCommand* flipCommand =
		new DrawingFlipItemsVerticalCommand(this, items, position, command);

	flipCommand->redo();
	tryToMaintainConnections(items, true, true, nullptr, flipCommand);
	flipCommand->undo();

	if (!command) mUndoStack.push(flipCommand);
}

void DrawingWidget::selectItemsCommand(const QList<DrawingItem*>& items, bool finalSelect,
	QUndoCommand* command)
{
	DrawingSelectItemsCommand* selectCommand =
		new DrawingSelectItemsCommand(this, items, finalSelect, command);

	if (!command) mUndoStack.push(selectCommand);
}

void DrawingWidget::reorderItemsCommand(const QList<DrawingItem*>& items, QUndoCommand* command)
{
	DrawingReorderItemsCommand* selectCommand =
		new DrawingReorderItemsCommand(this, items, command);

	if (!command) mUndoStack.push(selectCommand);
}

void DrawingWidget::insertPointCommand(DrawingItem* item, DrawingItemPoint* point, int index,
	QUndoCommand* command)
{
	DrawingItemInsertPointCommand* insertPointCommand =
		new DrawingItemInsertPointCommand(this, item, point, index, command);

	if (!command) mUndoStack.push(insertPointCommand);
}

void DrawingWidget::removePointCommand(DrawingItem* item, DrawingItemPoint* point,
	QUndoCommand* command)
{
	DrawingItemRemovePointCommand* removePointCommand =
		new DrawingItemRemovePointCommand(this, item, point, command);

	if (!command) mUndoStack.push(removePointCommand);
}

void DrawingWidget::connectPointsCommand(DrawingItemPoint* point1, DrawingItemPoint* point2,
	QUndoCommand* command)
{
	DrawingItemPointConnectCommand* connectCommand =
		new DrawingItemPointConnectCommand(this, point1, point2, command);

	QPointF point0Pos = point1->item()->mapToScene(point1->position());
	QPointF point1Pos = point2->item()->mapToScene(point2->position());

	if (point0Pos != point1Pos)
	{
		if (point2->isControlPoint())
			resizeItemCommand(point2, point0Pos, false, true, connectCommand);
		else if (point1->isControlPoint())
			resizeItemCommand(point1, point1Pos, false, true, connectCommand);
	}

	if (!command) mUndoStack.push(connectCommand);
}

void DrawingWidget::disconnectPointsCommand(DrawingItemPoint* point1, DrawingItemPoint* point2,
	QUndoCommand* command)
{
	DrawingItemPointDisconnectCommand* disconnectCommand =
		new DrawingItemPointDisconnectCommand(this, point1, point2, command);

	if (!command) mUndoStack.push(disconnectCommand);
}

void DrawingWidget::setItemsPropertiesCommand(const QHash< DrawingItem*, QHash<QString,QVariant> >& properties,
	QUndoCommand* command)
{
	DrawingItemsSetPropertiesCommand* propertiesCommand =
		new DrawingItemsSetPropertiesCommand(this, properties.keys(), properties, command);

	if (!command) mUndoStack.push(propertiesCommand);
}

void DrawingWidget::setPropertiesCommand(const QHash<QString,QVariant>& properties,
	QUndoCommand* command)
{
	DrawingSetPropertiesCommand* propertiesCommand =
		new DrawingSetPropertiesCommand(this, properties, command);

	if (!command) mUndoStack.push(propertiesCommand);
}

//==================================================================================================

void DrawingWidget::placeItems(const QList<DrawingItem*>& items, QUndoCommand* command)
{
	QList<DrawingItemPoint*> itemPoints, otherItemPoints;

	for(auto itemIter = items.begin(); itemIter != items.end(); itemIter++)
	{
		for(auto otherItemIter = mItems.begin(); otherItemIter != mItems.end(); otherItemIter++)
		{
			if (!items.contains(*otherItemIter) && !mPlaceItems.contains(*otherItemIter))
			{
				itemPoints = (*itemIter)->points();
				otherItemPoints = (*otherItemIter)->points();

				for(auto itemPointIter = itemPoints.begin();
					itemPointIter != itemPoints.end(); itemPointIter++)
				{
					for(auto otherItemPointIter = otherItemPoints.begin();
						otherItemPointIter != otherItemPoints.end(); otherItemPointIter++)
					{
						if (shouldConnect(*itemPointIter, *otherItemPointIter))
							connectPointsCommand(*itemPointIter, *otherItemPointIter, command);
					}
				}
			}
		}
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

		for(auto itemPointIter = itemPoints.begin();
			itemPointIter != itemPoints.end(); itemPointIter++)
		{
			itemPoint = *itemPointIter;
			targetPoints = itemPoint->connections();

			for(auto targetPointIter = targetPoints.begin();
				targetPointIter != targetPoints.end(); targetPointIter++)
			{
				if (!items.contains((*targetPointIter)->item()))
					disconnectPointsCommand(itemPoint, *targetPointIter, command);
			}
		}
	}
}

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

		for(auto itemPointIter = itemPoints.begin();
			itemPointIter != itemPoints.end(); itemPointIter++)
		{
			itemPoint = *itemPointIter;
			if (itemPoint != pointToSkip && (checkControlPoints || !itemPoint->isControlPoint()))
			{
				targetPoints = itemPoint->connections();

				for(auto targetPointIter = targetPoints.begin();
					targetPointIter != targetPoints.end(); targetPointIter++)
				{
					targetItemPoint = *targetPointIter;
					targetItem = targetItemPoint->item();

					if (item->mapToScene(itemPoint->position()) !=
						targetItem->mapToScene(targetItemPoint->position()))
					{
						// Try to maintain the connection by resizing targetPoint if possible
						if (allowResize && targetItemPoint->isFree() &&
							!shouldDisconnect(itemPoint, targetItemPoint))
						{
							resizeItemCommand(targetItemPoint,
								item->mapToScene(itemPoint->position()), false, false, command);
						}
						else
							disconnectPointsCommand(itemPoint, targetItemPoint, command);
					}
				}
			}
		}
	}
}

void DrawingWidget::disconnectAll(DrawingItemPoint* point, QUndoCommand* command)
{
	QList<DrawingItemPoint*> targetPoints;

	if (point)
	{
		targetPoints = point->connections();

		for(auto targetPointIter = targetPoints.begin();
			targetPointIter != targetPoints.end(); targetPointIter++)
		{
			disconnectPointsCommand(point, *targetPointIter, command);
		}
	}
}

//==================================================================================================

bool DrawingWidget::shouldConnect(DrawingItemPoint* point1, DrawingItemPoint* point2) const
{
	bool shouldConnect = false;

	if (point1 && point1->item() && point2 && point2->item() && point1->item() != point2->item())
	{
		qreal threshold = grid() / 4000;
		QPointF vec = point1->item()->mapToScene(point1->position()) -
			point2->item()->mapToScene(point2->position());
		qreal distance = qSqrt(vec.x() * vec.x() + vec.y() * vec.y());

		shouldConnect = (point1->isConnectionPoint() && point2->isConnectionPoint() &&
			(point1->isFree() || point2->isFree()) &&
			!point1->isConnected(point2) && !point1->isConnected(point2->item()) &&
			distance <= threshold);
	}

	return shouldConnect;
}

bool DrawingWidget::shouldDisconnect(DrawingItemPoint* point1, DrawingItemPoint* point2) const
{
	bool shouldDisconnect = true;

	if (point1 && point1->item() && point2 && point2->item())
	{
		shouldDisconnect = (
			point1->item()->mapToScene(point1->position()) !=
			point2->item()->mapToScene(point2->position()) &&
			!point2->isControlPoint());
	}

	return shouldDisconnect;
}

DrawingItemPoint* DrawingWidget::pointAt(DrawingItem* item, const QPointF& position) const
{
	DrawingItemPoint* point = nullptr;

	if (item)
	{
		QList<DrawingItemPoint*> itemPoints = item->points();
		QRectF pointItemRect;

		for(auto pointIter = itemPoints.begin(); point == nullptr && pointIter != itemPoints.end(); pointIter++)
		{
			pointItemRect = item->mapFromScene(mapToScene(pointRect(*pointIter))).boundingRect();
			if (pointItemRect.contains(position)) point = *pointIter;
		}
	}

	return point;
}

//==================================================================================================

void DrawingWidget::writeBrushToXml(QXmlStreamWriter* xml, const QString& name, const QBrush& brush)
{
	if (xml) xml->writeAttribute(name + "-color", colorToString(brush.color()));
}

QString DrawingWidget::colorToString(const QColor& color) const
{
	QString colorStr;

	if (color.alpha() == 255)
	{
		colorStr = QString("#%1%2%3").arg(color.red(), 2, 16, QChar('0')).arg(
			color.green(), 2, 16, QChar('0')).arg(color.blue(), 2, 16, QChar('0')).toLower();
	}
	else
	{
		colorStr = QString("#%1%2%3%4").arg(color.alpha(), 2, 16, QChar('0')).arg(
		color.red(), 2, 16, QChar('0')).arg(color.green(), 2, 16, QChar('0')).arg(
		color.blue(), 2, 16, QChar('0')).toLower();
	}

	return colorStr;
}

void DrawingWidget::readBrushFromXml(QXmlStreamReader* xml, const QString& name, QBrush& brush)
{
	if (xml)
	{
		QXmlStreamAttributes attr = xml->attributes();

		if (attr.hasAttribute(name + "-color"))
			brush = colorFromString(attr.value(name + "-color").toString());
	}
}

QColor DrawingWidget::colorFromString(const QString& str) const
{
	QColor color;

	if (str.length() >= 9)
	{
		color.setAlpha(str.mid(1,2).toUInt(nullptr, 16));
		color.setRed(str.mid(3,2).toUInt(nullptr, 16));
		color.setGreen(str.mid(5,2).toUInt(nullptr, 16));
		color.setBlue(str.mid(7,2).toUInt(nullptr, 16));
	}
	else if (str.length() >= 7)
	{
		color.setRed(str.mid(1,2).toUInt(nullptr, 16));
		color.setGreen(str.mid(3,2).toUInt(nullptr, 16));
		color.setBlue(str.mid(5,2).toUInt(nullptr, 16));
	}

	return color;
}

//==================================================================================================

void DrawingWidget::addActions()
{
	addAction("Undo", this, SLOT(undo()), "", "Ctrl+Z");
	addAction("Redo", this, SLOT(redo()), "", "Ctrl+Shift+Z");

	addAction("Cut", this, SLOT(cut()), "", "Ctrl+X");
	addAction("Copy", this, SLOT(copy()), "", "Ctrl+C");
	addAction("Paste", this, SLOT(paste()), "", "Ctrl+V");
	addAction("Delete", this, SLOT(deleteSelection()), "", "Delete");

	addAction("Select All", this, SLOT(selectAll()), "", "Ctrl+A");
	addAction("Select None", this, SLOT(selectNone()), "", "Ctrl+Shift+A");

	addAction("Rotate", this, SLOT(rotateSelection()), "", "R");
	addAction("Rotate Back", this, SLOT(rotateBackSelection()), "", "Shift+R");
	addAction("Flip Horizontal", this, SLOT(flipSelectionHorizontal()), "", "F");
	addAction("Flip Vertical", this, SLOT(flipSelectionVertical()), "", "F");

	addAction("Bring Forward", this, SLOT(bringSelectionForward()));
	addAction("Send Backward", this, SLOT(sendSelectionBackward()));
	addAction("Bring to Front", this, SLOT(bringSelectionToFront()));
	addAction("Send to Back", this, SLOT(sendSelectionToBack()));

	addAction("Group", this, SLOT(groupSelection()), "", "Ctrl+G");
	addAction("Ungroup", this, SLOT(ungroupSelection()), "", "Ctrl+Shift+G");

	addAction("Insert Point", this, SLOT(insertItemPointInSelection()), "");
	addAction("Remove Point", this, SLOT(removeItemPointFromSelection()), "");

	addAction("Properties...", this, SIGNAL(propertiesTriggered()));

	addPlaceAction(new DrawingLineItem(), "Place Line", ":/icons/oxygen/draw-line.png");
	addPlaceAction(new DrawingCurveItem(), "Place Curve", ":/icons/oxygen/draw-curve.png");
	addPlaceAction(new DrawingPolylineItem(), "Place Polyline", ":/icons/oxygen/draw-polyline.png");
	addPlaceAction(new DrawingRectItem(), "Place Rect", ":/icons/oxygen/draw-rectangle.png");
	addPlaceAction(new DrawingEllipseItem(), "Place Ellipse", ":/icons/oxygen/draw-ellipse.png");
	addPlaceAction(new DrawingPolygonItem(), "Place Polygon", ":/icons/oxygen/draw-polygon.png");
	addPlaceAction(new DrawingTextItem(), "Place Text", ":/icons/oxygen/draw-text.png");
	addPlaceAction(new DrawingTextRectItem(), "Place Text Rect", ":/icons/oxygen/draw-textrect.png");
	addPlaceAction(new DrawingTextEllipseItem(), "Place Text Ellipse", ":/icons/oxygen/draw-textellipse.png");
}

void DrawingWidget::createContextMenus()
{
	QList<QAction*> actionList = actions();

	mSingleItemContextMenu.addAction(actionList[CutAction]);
	mSingleItemContextMenu.addAction(actionList[CopyAction]);
	mSingleItemContextMenu.addAction(actionList[PasteAction]);
	mSingleItemContextMenu.addSeparator();
	mSingleItemContextMenu.addAction(actionList[RotateAction]);
	mSingleItemContextMenu.addAction(actionList[RotateBackAction]);
	mSingleItemContextMenu.addAction(actionList[FlipHorizontalAction]);
	mSingleItemContextMenu.addAction(actionList[FlipVerticalAction]);
	mSingleItemContextMenu.addAction(actionList[DeleteAction]);
	mSingleItemContextMenu.addSeparator();
	mSingleItemContextMenu.addAction(actionList[BringForwardAction]);
	mSingleItemContextMenu.addAction(actionList[SendBackwardAction]);
	mSingleItemContextMenu.addAction(actionList[BringToFrontAction]);
	mSingleItemContextMenu.addAction(actionList[SendToBackAction]);
	mSingleItemContextMenu.addSeparator();
	mSingleItemContextMenu.addAction(actionList[GroupAction]);
	mSingleItemContextMenu.addAction(actionList[UngroupAction]);

	mSinglePolyItemContextMenu.addAction(actionList[CutAction]);
	mSinglePolyItemContextMenu.addAction(actionList[CopyAction]);
	mSinglePolyItemContextMenu.addAction(actionList[PasteAction]);
	mSinglePolyItemContextMenu.addSeparator();
	mSinglePolyItemContextMenu.addAction(actionList[InsertPointAction]);
	mSinglePolyItemContextMenu.addAction(actionList[RemovePointAction]);
	mSinglePolyItemContextMenu.addSeparator();
	mSinglePolyItemContextMenu.addAction(actionList[RotateAction]);
	mSinglePolyItemContextMenu.addAction(actionList[RotateBackAction]);
	mSinglePolyItemContextMenu.addAction(actionList[FlipHorizontalAction]);
	mSinglePolyItemContextMenu.addAction(actionList[FlipVerticalAction]);
	mSinglePolyItemContextMenu.addAction(actionList[DeleteAction]);
	mSinglePolyItemContextMenu.addSeparator();
	mSinglePolyItemContextMenu.addAction(actionList[BringForwardAction]);
	mSinglePolyItemContextMenu.addAction(actionList[SendBackwardAction]);
	mSinglePolyItemContextMenu.addAction(actionList[BringToFrontAction]);
	mSinglePolyItemContextMenu.addAction(actionList[SendToBackAction]);
	mSinglePolyItemContextMenu.addSeparator();
	mSinglePolyItemContextMenu.addAction(actionList[GroupAction]);
	mSinglePolyItemContextMenu.addAction(actionList[UngroupAction]);

	mMultipleItemContextMenu.addAction(actionList[CutAction]);
	mMultipleItemContextMenu.addAction(actionList[CopyAction]);
	mMultipleItemContextMenu.addAction(actionList[PasteAction]);
	mMultipleItemContextMenu.addSeparator();
	mMultipleItemContextMenu.addAction(actionList[RotateAction]);
	mMultipleItemContextMenu.addAction(actionList[RotateBackAction]);
	mMultipleItemContextMenu.addAction(actionList[FlipHorizontalAction]);
	mMultipleItemContextMenu.addAction(actionList[FlipVerticalAction]);
	mMultipleItemContextMenu.addAction(actionList[DeleteAction]);
	mMultipleItemContextMenu.addSeparator();
	mMultipleItemContextMenu.addAction(actionList[BringForwardAction]);
	mMultipleItemContextMenu.addAction(actionList[SendBackwardAction]);
	mMultipleItemContextMenu.addAction(actionList[BringToFrontAction]);
	mMultipleItemContextMenu.addAction(actionList[SendToBackAction]);
	mMultipleItemContextMenu.addSeparator();
	mMultipleItemContextMenu.addAction(actionList[GroupAction]);
	mMultipleItemContextMenu.addAction(actionList[UngroupAction]);

	mDrawingContextMenu.addAction(actionList[UndoAction]);
	mDrawingContextMenu.addAction(actionList[RedoAction]);
	mDrawingContextMenu.addSeparator();
	mDrawingContextMenu.addAction(actionList[CutAction]);
	mDrawingContextMenu.addAction(actionList[CopyAction]);
	mDrawingContextMenu.addAction(actionList[PasteAction]);
	mDrawingContextMenu.addSeparator();
	mDrawingContextMenu.addAction(actionList[ZoomInAction]);
	mDrawingContextMenu.addAction(actionList[ZoomOutAction]);
	mDrawingContextMenu.addAction(actionList[ZoomFitAction]);
}
