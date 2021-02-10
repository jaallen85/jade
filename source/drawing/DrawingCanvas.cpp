// DrawingCanvas.cpp
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

#include "DrawingCanvas.h"
#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QPainter>
#include <QScrollBar>
#include <QStyle>
#include <QStyleHintReturnMask>
#include <QStyleOptionRubberBand>
#include <QtMath>
#include <QWheelEvent>

DrawingCanvas::DrawingCanvas(QWidget* parent) : QAbstractScrollArea(parent)
{
	setMouseTracking(true);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	mSceneRect = QRectF(-100, -100, 8200, 6200);
	mBackgroundBrush = QColor(255, 255, 255);

	mGrid = 50.0;
	mGridStyle = Drawing::GridGraphPaper;
	mGridBrush = QColor(0, 128, 128);
	mGridSpacingMajor = 8;
	mGridSpacingMinor = 2;
	mDynamicGridBaseValue = 1000;
	mDynamicGridEnabled = true;

	mScale = 1.0;
	mMode = Drawing::DefaultMode;

	mMouseLeftDragged = false;
	mMouseButtonDownHorizontalScrollValue = 0;
	mMouseButtonDownVerticalScrollValue = 0;

	mPanTimer.setInterval(16);
	connect(&mPanTimer, SIGNAL(timeout()), this, SLOT(mousePanEvent()));

	addActions();
}

DrawingCanvas::~DrawingCanvas() { }

//==================================================================================================

void DrawingCanvas::setSceneRect(const QRectF& rect)
{
	mSceneRect = rect;
}

QRectF DrawingCanvas::sceneRect() const
{
	return mSceneRect;
}

//==================================================================================================

void DrawingCanvas::setBackgroundBrush(const QBrush& brush)
{
	mBackgroundBrush = brush;
}

QBrush DrawingCanvas::backgroundBrush() const
{
	return mBackgroundBrush;
}

//==================================================================================================

void DrawingCanvas::setGrid(qreal grid)
{
	mGrid = grid;
}

void DrawingCanvas::setGridStyle(Drawing::GridStyle style)
{
	mGridStyle = style;
}

void DrawingCanvas::setGridBrush(const QBrush& brush)
{
	mGridBrush = brush;
}

void DrawingCanvas::setGridSpacing(int majorSpacing, int minorSpacing)
{
	mGridSpacingMajor = majorSpacing;
	mGridSpacingMinor = minorSpacing;
}

qreal DrawingCanvas::grid() const
{
	return mGrid;
}

Drawing::GridStyle DrawingCanvas::gridStyle() const
{
	return mGridStyle;
}

QBrush DrawingCanvas::gridBrush() const
{
	return mGridBrush;
}

int DrawingCanvas::gridSpacingMajor() const
{
	return mGridSpacingMajor;
}

int DrawingCanvas::gridSpacingMinor() const
{
	return mGridSpacingMinor;
}

qreal DrawingCanvas::roundToGrid(qreal value) const
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

QPointF DrawingCanvas::roundToGrid(const QPointF& position) const
{
	return QPointF(roundToGrid(position.x()), roundToGrid(position.y()));
}

//==================================================================================================

void DrawingCanvas::setDynamicGrid(qreal baseValue)
{
	mDynamicGridBaseValue = baseValue;
	updateDynamicGrid();
}

void DrawingCanvas::setDynamicGridEnabled(bool enable)
{
	mDynamicGridEnabled = enable;
	updateDynamicGrid();
}

qreal DrawingCanvas::dynamicGrid() const
{
	return mDynamicGridBaseValue;
}

bool DrawingCanvas::isDynamicGridEnabled() const
{
	return mDynamicGridEnabled;
}

//==================================================================================================

void DrawingCanvas::centerOn(const QPointF& position)
{
	QPointF oldPosition = mapToScene(viewport()->rect().center());

	int horizontalDelta = qRound((position.x() - oldPosition.x()) * mScale);
	int verticalDelta = qRound((position.y() - oldPosition.y()) * mScale);

	horizontalScrollBar()->setValue(horizontalScrollBar()->value() + horizontalDelta);
	verticalScrollBar()->setValue(verticalScrollBar()->value() + verticalDelta);
}

void DrawingCanvas::centerOnCursor(const QPointF& position)
{
	QPointF oldPosition = mapToScene(mapFromGlobal(QCursor::pos()));

	int horizontalDelta = qRound((position.x() - oldPosition.x()) * mScale);
	int verticalDelta = qRound((position.y() - oldPosition.y()) * mScale);

	horizontalScrollBar()->setValue(horizontalScrollBar()->value() + horizontalDelta);
	verticalScrollBar()->setValue(verticalScrollBar()->value() + verticalDelta);
}

void DrawingCanvas::fitToView(const QRectF& rect)
{
	qreal scaleX = (maximumViewportSize().width() - 5) / rect.width();
	qreal scaleY = (maximumViewportSize().height() - 5) / rect.height();

	mScale = qMin(scaleX, scaleY);

	recalculateContentSize(rect);
	centerOn(rect.center());
}

void DrawingCanvas::scaleBy(qreal scale)
{
	if (scale > 0)
	{
		QRectF scrollBarRect = mSceneRect;

		QPoint mousePos = mapFromGlobal(QCursor::pos());
		QPointF mouseScenePos = mapToScene(mousePos);
		if (viewport()->rect().contains(mousePos) && !mSceneRect.contains(mouseScenePos))
			scrollBarRect = scrollBarDefinedRect();

		mScale *= scale;

		recalculateContentSize(scrollBarRect);
	}
}

qreal DrawingCanvas::scale() const
{
	return mScale;
}

QPointF DrawingCanvas::mapToScene(const QPoint& point) const
{
	QPointF p = point;
	p.setX(p.x() + horizontalScrollBar()->value());
	p.setY(p.y() + verticalScrollBar()->value());
	return mSceneTransform.map(p);
}

QRectF DrawingCanvas::mapToScene(const QRect& rect) const
{
	return QRectF(mapToScene(rect.topLeft()), mapToScene(rect.bottomRight()));
}

QPoint DrawingCanvas::mapFromScene(const QPointF& point) const
{
	QPointF p = mViewportTransform.map(point);
	p.setX(p.x() - horizontalScrollBar()->value());
	p.setY(p.y() - verticalScrollBar()->value());
	return p.toPoint();
}

QRect DrawingCanvas::mapFromScene(const QRectF& rect) const
{
	return QRect(mapFromScene(rect.topLeft()), mapFromScene(rect.bottomRight()));
}

//==================================================================================================

void DrawingCanvas::setMode(Drawing::Mode mode)
{
	mMode = mode;
}

Drawing::Mode DrawingCanvas::mode() const
{
	return mMode;
}

//==================================================================================================

QAction* DrawingCanvas::addAction(const QString& text, QObject* slotObj, const char* slotFunction,
	const QString& iconPath, const QString& shortcut)
{
	QAction* action = new QAction(text, this);
	connect(action, SIGNAL(triggered()), slotObj, slotFunction);

	if (!iconPath.isEmpty()) action->setIcon(QIcon(iconPath));
	if (!shortcut.isEmpty()) action->setShortcut(QKeySequence(shortcut));

	QAbstractScrollArea::addAction(action);

	return action;
}

QAction* DrawingCanvas::addModeAction(const QString& text,
	const QString& iconPath, const QString& shortcut)
{
	QAction* action = new QAction(text, mModeActionGroup);

	if (!iconPath.isEmpty()) action->setIcon(QIcon(iconPath));
	if (!shortcut.isEmpty()) action->setShortcut(QKeySequence(shortcut));

	action->setCheckable(true);
	action->setActionGroup(mModeActionGroup);

	return action;
}

QList<QAction*> DrawingCanvas::modeActions() const
{
	return mModeActionGroup->actions();
}

//==================================================================================================

void DrawingCanvas::setScale(qreal scale)
{
	QPointF mousePos = mapToScene(mapFromGlobal(QCursor::pos()));

	scaleBy(scale / mScale);

	if (viewport()->rect().contains(mapFromGlobal(QCursor::pos())))
		centerOnCursor(mousePos);
	else
		centerOn(QPointF());

	emit scaleChanged(mScale);
	viewport()->update();
}

void DrawingCanvas::zoomIn()
{
	QPointF mousePos = mapToScene(mapFromGlobal(QCursor::pos()));

	scaleBy(qSqrt(2));

	if (viewport()->rect().contains(mapFromGlobal(QCursor::pos())))
		centerOnCursor(mousePos);
	else
		centerOn(QPointF());

	emit scaleChanged(mScale);
	viewport()->update();
}

void DrawingCanvas::zoomOut()
{
	QPointF mousePos = mapToScene(mapFromGlobal(QCursor::pos()));

	scaleBy(qSqrt(2) / 2);

	if (viewport()->rect().contains(mapFromGlobal(QCursor::pos())))
		centerOnCursor(mousePos);
	else
		centerOn(QPointF());

	emit scaleChanged(mScale);
	viewport()->update();
}

void DrawingCanvas::zoomFit()
{
	fitToView(mSceneRect);

	emit scaleChanged(mScale);
	viewport()->update();
}

//==================================================================================================

void DrawingCanvas::setDefaultMode()
{
	mMode = Drawing::DefaultMode;

	setCursor(Qt::ArrowCursor);

	QList<QAction*> modeActionList = modeActions();
	if (!modeActionList[DefaultModeAction]->isChecked())
		modeActionList[DefaultModeAction]->setChecked(true);

	emit modeChanged(mMode);
	viewport()->update();
}

void DrawingCanvas::setScrollMode()
{
	mMode = Drawing::ScrollMode;

	setCursor(Qt::OpenHandCursor);

	QList<QAction*> modeActionList = modeActions();
	if (!modeActionList[ScrollModeAction]->isChecked())
		modeActionList[ScrollModeAction]->setChecked(true);

	emit modeChanged(mMode);
	viewport()->update();
}

void DrawingCanvas::setZoomMode()
{
	mMode = Drawing::ZoomMode;

	setCursor(Qt::CrossCursor);

	QList<QAction*> modeActionList = modeActions();
	if (!modeActionList[ZoomModeAction]->isChecked())
		modeActionList[ZoomModeAction]->setChecked(true);

	emit modeChanged(mMode);
	viewport()->update();
}

//==================================================================================================

void DrawingCanvas::setModeFromAction(QAction* action)
{
	QList<QAction*> modeActionList = modeActions();

	if (action == modeActionList[DefaultModeAction]) setDefaultMode();
	else if (action == modeActionList[ScrollModeAction]) setScrollMode();
	else if (action == modeActionList[ZoomModeAction]) setZoomMode();

	emit modeActionChanged(action);
}

//==================================================================================================

void DrawingCanvas::paintEvent(QPaintEvent* event)
{
	QImage image(viewport()->width(), viewport()->height(), QImage::Format_RGB32);
	image.fill(palette().brush(QPalette::Window).color());

	// Render scene
	QPainter painter(&image);

	painter.translate(-horizontalScrollBar()->value(), -verticalScrollBar()->value());
	painter.setTransform(mViewportTransform, true);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

	drawBackground(&painter);
	drawMain(&painter);
	drawForeground(&painter);

	painter.end();

	// Render scene image on to widget
	QPainter widgetPainter(viewport());
	widgetPainter.drawImage(0, 0, image);

	Q_UNUSED(event);
}

void DrawingCanvas::resizeEvent(QResizeEvent* event)
{
	QAbstractScrollArea::resizeEvent(event);
	recalculateContentSize();
}

//==================================================================================================

void DrawingCanvas::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		// Update button down screen and scene position
		mMouseLeftButtonDownPos = event->pos();
		mMouseLeftDragged = false;
		mRubberBandZoomRect = QRect();

		switch (mMode)
		{
		case Drawing::DefaultMode:
			// Nothing to do here.
			break;
		case Drawing::ScrollMode:
			setCursor(Qt::ClosedHandCursor);
			mMouseButtonDownHorizontalScrollValue = horizontalScrollBar()->value();
			mMouseButtonDownVerticalScrollValue = verticalScrollBar()->value();
			break;
		case Drawing::ZoomMode:
			// Nothing to do here.
			break;
		case Drawing::PlaceMode:
			// Nothing to do here.
			break;
		default:
			// Nothing to do here.
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

void DrawingCanvas::mouseMoveEvent(QMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		mMouseLeftDragged = (mMouseLeftDragged |
			((mMouseLeftButtonDownPos - event->pos()).manhattanLength() >= QApplication::startDragDistance()));
	}

	switch (mMode)
	{
	case Drawing::DefaultMode:
		// Nothing to do here.
		break;
	case Drawing::ScrollMode:
		if ((event->buttons() & Qt::LeftButton) && mMouseLeftDragged)
		{
			horizontalScrollBar()->setValue(
				mMouseButtonDownHorizontalScrollValue - (event->pos().x() - mMouseLeftButtonDownPos.x()));
			verticalScrollBar()->setValue(
				mMouseButtonDownVerticalScrollValue - (event->pos().y() - mMouseLeftButtonDownPos.y()));
		}
		break;
	case Drawing::ZoomMode:
		if ((event->buttons() & Qt::LeftButton) && mMouseLeftDragged)
		{
			mRubberBandZoomRect = QRect(event->pos(), mMouseLeftButtonDownPos).normalized();
		}
		break;
	case Drawing::PlaceMode:
		// Nothing to do here.
		break;
	default:
		// Nothing to do here.
		break;
	}

	if (mPanTimer.isActive()) mPanCurrentPos = event->pos();

	if (event->buttons() != Qt::NoButton) viewport()->update();
}

void DrawingCanvas::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		switch (mMode)
		{
		case Drawing::DefaultMode:
			// Nothing to do here.
			break;
		case Drawing::ScrollMode:
			setCursor(Qt::OpenHandCursor);
			break;
		case Drawing::ZoomMode:
			if (mRubberBandZoomRect.isValid())
			{
				fitToView(mapToScene(mRubberBandZoomRect));
				emit scaleChanged(mScale);
				setDefaultMode();
			}
			break;
		case Drawing::PlaceMode:
			// Nothing to do here.
			break;
		default:
			// Nothing to do here.
			break;
		}
	}
	else if (event->button() == Qt::RightButton)
	{
		if (mMode != Drawing::DefaultMode) setDefaultMode();
	}

	// Disable panning mode, if it is active
	if (mPanTimer.isActive())
	{
		setCursor(Qt::ArrowCursor);
		mPanTimer.stop();
	}

	// Clear mouse state variables
	mRubberBandZoomRect = QRect();

	viewport()->update();
}

void DrawingCanvas::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (mMode == Drawing::DefaultMode)
			mousePressEvent(event);
		else
			setDefaultMode();
	}
}

void DrawingCanvas::wheelEvent(QWheelEvent* event)
{
	if (event->modifiers() & Qt::ControlModifier)
	{
		if (event->angleDelta().y() > 0) zoomIn();
		else if (event->angleDelta().y() < 0) zoomOut();
	}
	else QAbstractScrollArea::wheelEvent(event);
}

//==================================================================================================

void DrawingCanvas::drawBackground(QPainter* painter)
{
	QPainter::RenderHints renderHints = painter->renderHints();
	painter->setRenderHints(renderHints, false);

	// Draw background
	painter->setBrush(mBackgroundBrush);
	painter->setPen(Qt::NoPen);
	painter->drawRect(mSceneRect);

	// Draw grid
	QPen gridPen(mGridBrush, devicePixelRatio());
	gridPen.setCosmetic(true);

	if (mGridStyle != Drawing::GridNone && mGrid >= 0)
	{
		painter->setPen(gridPen);

		if (mGridStyle == Drawing::GridDots && mGridSpacingMajor > 0)
		{
			qreal spacing = mGrid * mGridSpacingMajor;
			for(qreal y = qCeil(mSceneRect.top() / spacing) * spacing; y < mSceneRect.bottom(); y += spacing)
			{
				for(qreal x = qCeil(mSceneRect.left() / spacing) * spacing; x < mSceneRect.right(); x += spacing)
					painter->drawPoint(QPointF(x, y));
			}
		}

		if (mGridStyle == Drawing::GridGraphPaper && mGridSpacingMinor > 0)
		{
			gridPen.setStyle(Qt::DotLine);
			painter->setPen(gridPen);

			qreal spacing = mGrid * mGridSpacingMinor;
			for(qreal y = qCeil(mSceneRect.top() / spacing) * spacing; y < mSceneRect.bottom(); y += spacing)
				painter->drawLine(QPointF(mSceneRect.left(), y), QPointF(mSceneRect.right(), y));
			for(qreal x = qCeil(mSceneRect.left() / spacing) * spacing; x < mSceneRect.right(); x += spacing)
				painter->drawLine(QPointF(x, mSceneRect.top()), QPointF(x, mSceneRect.bottom()));
		}

		if ((mGridStyle == Drawing::GridLines || mGridStyle == Drawing::GridGraphPaper) && mGridSpacingMajor > 0)
		{
			gridPen.setStyle(Qt::SolidLine);
			painter->setPen(gridPen);

			qreal spacing = mGrid * mGridSpacingMajor;
			for(qreal y = qCeil(mSceneRect.top() / spacing) * spacing; y < mSceneRect.bottom(); y += spacing)
				painter->drawLine(QPointF(mSceneRect.left(), y), QPointF(mSceneRect.right(), y));
			for(qreal x = qCeil(mSceneRect.left() / spacing) * spacing; x < mSceneRect.right(); x += spacing)
				painter->drawLine(QPointF(x, mSceneRect.top()), QPointF(x, mSceneRect.bottom()));
		}
	}

	// Draw origin
	if (mGridStyle != Drawing::GridNone && mGrid >= 0 && mSceneRect.contains(QPointF(0,0)))
	{
		qreal radius = mapToScene(QPoint(3, 3)).x() - mapToScene(QPoint(0, 0)).x();
		painter->drawLine(QPointF(-radius, -radius), QPointF(-radius, radius));
		painter->drawLine(QPointF(-radius, radius), QPointF(radius, radius));
		painter->drawLine(QPointF(radius, radius), QPointF(radius, -radius));
		painter->drawLine(QPointF(radius, -radius), QPointF(-radius, -radius));
	}

	// Draw border
	QPen borderPen(QColor(128, 128, 128), devicePixelRatio());
	borderPen.setCosmetic(true);

	painter->setBrush(Qt::transparent);
	painter->setPen(borderPen);
	painter->drawRect(mSceneRect);

	painter->setRenderHints(renderHints);
	painter->setBrush(mBackgroundBrush);
}

void DrawingCanvas::drawMain(QPainter* painter)
{
	drawRubberBand(painter, mRubberBandZoomRect);
}

void DrawingCanvas::drawForeground(QPainter* painter)
{
	Q_UNUSED(painter);
}

void DrawingCanvas::drawRubberBand(QPainter* painter, const QRect& rect)
{
	if (rect.isValid())
	{
		QStyleOptionRubberBand option;
		option.initFrom(viewport());
		option.rect = rect;
		option.shape = QRubberBand::Rectangle;

		painter->save();
		painter->resetTransform();

		QStyleHintReturnMask mask;
		if (viewport()->style()->styleHint(QStyle::SH_RubberBand_Mask, &option, viewport(), &mask))
			painter->setClipRegion(mask.region, Qt::IntersectClip);

		viewport()->style()->drawControl(QStyle::CE_RubberBand, &option, painter, viewport());

		painter->restore();
	}
}

//==================================================================================================

void DrawingCanvas::mousePanEvent()
{
	QRectF visibleRect = DrawingCanvas::visibleRect();

	if (mPanCurrentPos.x() - mPanStartPos.x() < 0)
	{
		int delta = (mPanCurrentPos.x() - mPanStartPos.x()) / 16;

		if (horizontalScrollBar()->value() + delta < horizontalScrollBar()->minimum())
		{
			if (horizontalScrollBar()->minimum() >= horizontalScrollBar()->maximum())
				horizontalScrollBar()->setMinimum(qFloor((visibleRect.left() - mSceneRect.left()) * mScale) + delta);
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
				horizontalScrollBar()->setMaximum(qFloor((mSceneRect.right() - visibleRect.right()) * mScale) + delta);
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
				verticalScrollBar()->setMinimum(qFloor((visibleRect.top() - mSceneRect.top()) * mScale) + delta);
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
				verticalScrollBar()->setMaximum(qFloor((mSceneRect.bottom() - visibleRect.bottom()) * mScale) + delta);
			else
				verticalScrollBar()->setMaximum(verticalScrollBar()->value() + delta);

			verticalScrollBar()->setValue(verticalScrollBar()->maximum());
		}
		else verticalScrollBar()->setValue(verticalScrollBar()->value() + delta);
	}
}

//==================================================================================================

void DrawingCanvas::recalculateContentSize(const QRectF& rect)
{
	QRectF targetRect = mSceneRect;

	if (rect.isValid())
	{
		targetRect.setLeft(qMin(rect.left(), mSceneRect.left()));
		targetRect.setTop(qMin(rect.top(), mSceneRect.top()));
		targetRect.setRight(qMax(rect.right(), mSceneRect.right()));
		targetRect.setBottom(qMax(rect.bottom(), mSceneRect.bottom()));
	}

	int contentWidth = qRound(targetRect.width() * mScale);
	int contentHeight = qRound(targetRect.height() * mScale);
	int viewportWidth = maximumViewportSize().width();
	int viewportHeight = maximumViewportSize().height();
	int scrollBarExtent = style()->pixelMetric(QStyle::PM_ScrollBarExtent, nullptr, this);

	// Set scroll bar range
	if (contentWidth > viewportWidth)
	{
		int contentLeft = qFloor((targetRect.left() - mSceneRect.left()) * mScale);

		if (verticalScrollBarPolicy() == Qt::ScrollBarAsNeeded) viewportWidth -= scrollBarExtent;

		horizontalScrollBar()->setRange(contentLeft - 1, contentLeft + contentWidth - viewportWidth + 1);
		horizontalScrollBar()->setSingleStep(viewportWidth / 80);
		horizontalScrollBar()->setPageStep(viewportWidth);
	}
	else horizontalScrollBar()->setRange(0, 0);

	if (contentHeight > viewportHeight)
	{
		int contentTop = qFloor((targetRect.top() - mSceneRect.top()) * mScale);

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
	mViewportTransform.translate(dx, dy);
	mViewportTransform.scale(mScale, mScale);

	mSceneTransform = mViewportTransform.inverted();

	// Update dynamic grid, if enabled
	updateDynamicGrid();
}

void DrawingCanvas::updateDynamicGrid()
{
	if (mDynamicGridEnabled && mDynamicGridBaseValue > 0)
	{
		const int minimumScreenGridValue = devicePixelRatio() * 4;		// pixels

		QPointF gridPoint = mapToScene(QPoint(minimumScreenGridValue, minimumScreenGridValue)) -
			mapToScene(QPoint(0, 0));
		qreal gridValue = qMax(gridPoint.x(), gridPoint.y());

		qreal baseOffset = qLn(mDynamicGridBaseValue) / qLn(2);
		baseOffset = baseOffset - qFloor(baseOffset);

		mGrid = qPow(2, qCeil(qLn(gridValue) / qLn(2) - baseOffset) + baseOffset);

		emit gridChanged(mGrid);
	}
}

QRectF DrawingCanvas::scrollBarDefinedRect() const
{
	QRectF scrollBarRect = mSceneRect;

	if (horizontalScrollBar()->minimum() < horizontalScrollBar()->maximum())
	{
		scrollBarRect.setLeft(horizontalScrollBar()->minimum() / mScale + scrollBarRect.left());
		scrollBarRect.setWidth((horizontalScrollBar()->maximum() - horizontalScrollBar()->minimum() +
			maximumViewportSize().width()) / mScale);
	}

	if (verticalScrollBar()->minimum() < verticalScrollBar()->maximum())
	{
		scrollBarRect.setTop(verticalScrollBar()->minimum() / mScale + scrollBarRect.top());
		scrollBarRect.setHeight((verticalScrollBar()->maximum() - verticalScrollBar()->minimum() +
			maximumViewportSize().height()) / mScale);
	}

	return scrollBarRect;
}

QRectF DrawingCanvas::visibleRect() const
{
	return QRectF(mapToScene(QPoint(0, 0)), mapToScene(QPoint(viewport()->width(), viewport()->height())));
}

//==================================================================================================

void DrawingCanvas::addActions()
{
	addAction("Zoom In", this, SLOT(zoomIn()), "", ".");
	addAction("Zoom Out", this, SLOT(zoomOut()), "", ",");
	addAction("Zoom Fit", this, SLOT(zoomFit()), "", "/");

	mModeActionGroup = new QActionGroup(this);
	connect(mModeActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(setModeFromAction(QAction*)));

	QAction* defaultModeAction = addModeAction("Default Mode", "", "Escape");
	addModeAction("Scroll Mode");
	addModeAction("Zoom Mode");

	defaultModeAction->setChecked(true);
}
