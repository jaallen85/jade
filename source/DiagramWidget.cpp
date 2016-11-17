/* DiagramWidget.cpp
 *
 * Copyright (C) 2013-2016 Jason Allen
 *
 * This file is part of the jade application.
 *
 * jade is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * jade is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with jade.  If not, see <http://www.gnu.org/licenses/>
 */

#include "DiagramWidget.h"

DiagramWidget::DiagramWidget() : DrawingWidget()
{
	mGridStyle = GridGraphPaper;
	mGridBrush = QColor(0, 128, 128);
	mGridSpacingMajor = 8;
	mGridSpacingMinor = 2;

	mDragged = false;

	addActions();
	createContextMenu();
	connect(this, SIGNAL(selectionChanged(const QList<DrawingItem*>&)), this, SLOT(updateActionsFromSelection()));
}

DiagramWidget::~DiagramWidget() { }

//==================================================================================================

void DiagramWidget::setGridStyle(GridRenderStyle style)
{
	mGridStyle = style;
}

void DiagramWidget::setGridBrush(const QBrush& brush)
{
	mGridBrush = brush;
}

void DiagramWidget::setGridSpacing(int majorSpacing, int minorSpacing)
{
	mGridSpacingMajor = majorSpacing;
	mGridSpacingMinor = minorSpacing;
}

DiagramWidget::GridRenderStyle DiagramWidget::gridStyle() const
{
	return mGridStyle;
}

QBrush DiagramWidget::gridBrush() const
{
	return mGridBrush;
}

int DiagramWidget::gridSpacingMajor() const
{
	return mGridSpacingMajor;
}

int DiagramWidget::gridSpacingMinor() const
{
	return mGridSpacingMinor;
}

//==================================================================================================

void DiagramWidget::drawBackground(QPainter* painter)
{
	QRectF visibleRect = DrawingWidget::visibleRect();
	QBrush backgroundBrush = DiagramWidget::backgroundBrush();
	qreal grid = DiagramWidget::grid();

	QPainter::RenderHints renderHints = painter->renderHints();
	painter->setRenderHints(renderHints, false);

	// Draw background
	painter->setBrush(backgroundBrush);
	painter->setPen(Qt::NoPen);
	painter->drawRect(visibleRect);

	// Draw grid
	QPen gridPen(mGridBrush, devicePixelRatio());
	gridPen.setCosmetic(true);

	if (mGridStyle != GridNone && grid >= 0)
	{
		painter->setPen(gridPen);

		if (mGridStyle == GridDots && mGridSpacingMajor > 0)
		{
			qreal spacing = grid * mGridSpacingMajor;
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

			qreal spacing = grid * mGridSpacingMinor;
			for(qreal y = qCeil(visibleRect.top() / spacing) * spacing; y < visibleRect.bottom(); y += spacing)
				painter->drawLine(QPointF(visibleRect.left(), y), QPointF(visibleRect.right(), y));
			for(qreal x = qCeil(visibleRect.left() / spacing) * spacing; x < visibleRect.right(); x += spacing)
				painter->drawLine(QPointF(x, visibleRect.top()), QPointF(x, visibleRect.bottom()));
		}

		if ((mGridStyle == GridLines || mGridStyle == GridGraphPaper) && mGridSpacingMajor > 0)
		{
			gridPen.setStyle(Qt::SolidLine);
			painter->setPen(gridPen);

			qreal spacing = grid * mGridSpacingMajor;
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
	QPen borderPen((backgroundBrush == Qt::black) ? Qt::white : Qt::black, devicePixelRatio() * 2);
	borderPen.setCosmetic(true);

	painter->setBrush(Qt::transparent);
	painter->setPen(borderPen);
	painter->drawRect(sceneRect());

	painter->setRenderHints(renderHints);
	painter->setBrush(backgroundBrush);
}

//==================================================================================================

void DiagramWidget::mousePressEvent(QMouseEvent* event)
{
	DrawingWidget::mousePressEvent(event);

	mButtonDownPos = event->pos();
	mDragged = false;
}

void DiagramWidget::mouseMoveEvent(QMouseEvent* event)
{
	QString mouseInfoText;

	DrawingWidget::mouseMoveEvent(event);

	mDragged = (mDragged |
		((mButtonDownPos - event->pos()).manhattanLength() >= QApplication::startDragDistance()));

	if ((event->buttons() & Qt::LeftButton) && mDragged)
	{
		QPointF p1 = mapToScene(mButtonDownPos);
		QPointF p2 = mapToScene(event->pos());

		if (mode() == PlaceMode || mode() == DefaultMode)
		{
			p1 = roundToGrid(p1);
			p2 = roundToGrid(p2);
		}

		QPointF delta = p2 - p1;

		mouseInfoText += "(" + QString::number(p1.x()) + "," + QString::number(p1.y()) + ")";
		mouseInfoText += " - (" + QString::number(p2.x()) + "," + QString::number(p2.y()) + ")";
		mouseInfoText += "  " + QString(QChar(0x394)) + " = (" +
			QString::number(delta.x()) + "," + QString::number(delta.y()) + ")";
	}
	else
	{
		QPointF p1 = mapToScene(event->pos());

		if (mode() == PlaceMode || mode() == DefaultMode)
			p1 = roundToGrid(p1);

		mouseInfoText = "(" + QString::number(p1.x()) + "," + QString::number(p1.y()) + ")";
	}

	emit mouseInfoChanged(mouseInfoText);
}

void DiagramWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::RightButton)
	{
		if (mode() == DefaultMode)
		{
			DrawingItem* mouseDownItem = itemAt(mapToScene(event->pos()));

			if (mouseDownItem && mouseDownItem->isSelected() && selectedItems().size() == 1)
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

		viewport()->update();
	}
	else DrawingWidget::mouseReleaseEvent(event);

	mDragged = false;
}

void DiagramWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	DrawingWidget::mouseDoubleClickEvent(event);

	if (mouseDownItem()) emit propertiesTriggered();
}

//==================================================================================================

void DiagramWidget::updateActionsFromSelection()
{
	QList<QAction*> actions = DiagramWidget::actions();
	QList<DrawingItem*> selectedItems = DiagramWidget::selectedItems();

	bool canInsertRemovePoints = false;
	bool canGroup = (selectedItems.size() > 1);
	bool canUngroup = false;

	if (selectedItems.size() == 1)
	{
		DrawingItem* item = selectedItems.first();
		DrawingItemGroup* groupItem = dynamic_cast<DrawingItemGroup*>(item);

		canInsertRemovePoints = ((item->flags() & DrawingItem::CanInsertPoints) ||
			(item->flags() & DrawingItem::CanRemovePoints));
		canUngroup = (groupItem);
	}

	actions[InsertPointAction]->setEnabled(canInsertRemovePoints);
	actions[RemovePointAction]->setEnabled(canInsertRemovePoints);
	actions[GroupAction]->setEnabled(canGroup);
	actions[UngroupAction]->setEnabled(canUngroup);
}

//==================================================================================================

void DiagramWidget::addActions()
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

	addAction("Properties...", this, SIGNAL(propertiesTriggered()), ":/icons/oxygen/games-config-board.png");
}

void DiagramWidget::createContextMenu()
{
	QList<QAction*> actions = DrawingWidget::actions();

	mSingleItemContextMenu.addAction(actions[CutAction]);
	mSingleItemContextMenu.addAction(actions[CopyAction]);
	mSingleItemContextMenu.addAction(actions[PasteAction]);
	mSingleItemContextMenu.addSeparator();
	mSingleItemContextMenu.addAction(actions[RotateAction]);
	mSingleItemContextMenu.addAction(actions[RotateBackAction]);
	mSingleItemContextMenu.addAction(actions[FlipAction]);
	mSingleItemContextMenu.addAction(actions[DeleteAction]);
	mSingleItemContextMenu.addSeparator();
	mSingleItemContextMenu.addAction(actions[GroupAction]);
	mSingleItemContextMenu.addAction(actions[UngroupAction]);

	mSinglePolyItemContextMenu.addAction(actions[CutAction]);
	mSinglePolyItemContextMenu.addAction(actions[CopyAction]);
	mSinglePolyItemContextMenu.addAction(actions[PasteAction]);
	mSinglePolyItemContextMenu.addSeparator();
	mSinglePolyItemContextMenu.addAction(actions[InsertPointAction]);
	mSinglePolyItemContextMenu.addAction(actions[RemovePointAction]);
	mSinglePolyItemContextMenu.addSeparator();
	mSinglePolyItemContextMenu.addAction(actions[RotateAction]);
	mSinglePolyItemContextMenu.addAction(actions[RotateBackAction]);
	mSinglePolyItemContextMenu.addAction(actions[FlipAction]);
	mSinglePolyItemContextMenu.addAction(actions[DeleteAction]);
	mSinglePolyItemContextMenu.addSeparator();
	mSinglePolyItemContextMenu.addAction(actions[GroupAction]);
	mSinglePolyItemContextMenu.addAction(actions[UngroupAction]);

	mMultipleItemContextMenu.addAction(actions[CutAction]);
	mMultipleItemContextMenu.addAction(actions[CopyAction]);
	mMultipleItemContextMenu.addAction(actions[PasteAction]);
	mMultipleItemContextMenu.addSeparator();
	mMultipleItemContextMenu.addAction(actions[RotateAction]);
	mMultipleItemContextMenu.addAction(actions[RotateBackAction]);
	mMultipleItemContextMenu.addAction(actions[FlipAction]);
	mMultipleItemContextMenu.addAction(actions[DeleteAction]);
	mMultipleItemContextMenu.addSeparator();
	mMultipleItemContextMenu.addAction(actions[GroupAction]);
	mMultipleItemContextMenu.addAction(actions[UngroupAction]);

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

QAction* DiagramWidget::addAction(const QString& text, QObject* slotObj, const char* slotFunction,
	const QString& iconPath, const QString& shortcut)
{
	QAction* action = new QAction(text, this);
	if (slotObj) connect(action, SIGNAL(triggered()), slotObj, slotFunction);

	if (!iconPath.isEmpty()) action->setIcon(QIcon(iconPath));
	if (!shortcut.isEmpty()) action->setShortcut(QKeySequence(shortcut));

	QAbstractScrollArea::addAction(action);
	return action;
}
