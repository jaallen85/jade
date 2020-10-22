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
#include <QAction>
#include <QActionGroup>
#include <QPainter>
#include <QScrollBar>
#include <QStyle>
#include <QtMath>
#include <QWheelEvent>

DrawingWidget::DrawingWidget(QWidget* parent) : QAbstractScrollArea(parent)
{
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	mSceneRect = QRectF(-100, -100, 8200, 6200);
	mBackgroundBrush = QColor(255, 255, 255);

	mGrid = 50.0;
	mGridStyle = Drawing::GridGraphPaper;
	mGridBrush = QColor(0, 128, 128);
	mGridSpacingMajor = 8;
	mGridSpacingMinor = 2;

	mScale = 1.0;
	mMode = Drawing::DefaultMode;

	addActions();
}

DrawingWidget::~DrawingWidget()
{

}

//==================================================================================================

void DrawingWidget::setSceneRect(const QRectF& rect)
{
	mSceneRect = rect;
}

QRectF DrawingWidget::sceneRect() const
{
	return mSceneRect;
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

void DrawingWidget::setGrid(qreal grid)
{
	mGrid = grid;
}

void DrawingWidget::setGridStyle(Drawing::GridStyle style)
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

qreal DrawingWidget::grid() const
{
	return mGrid;
}

Drawing::GridStyle DrawingWidget::gridStyle() const
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

void DrawingWidget::centerOn(const QPointF& position)
{
	QPointF oldPosition = mapToScene(viewport()->rect().center());

	int horizontalDelta = qRound((position.x() - oldPosition.x()) * mScale);
	int verticalDelta = qRound((position.y() - oldPosition.y()) * mScale);

	horizontalScrollBar()->setValue(horizontalScrollBar()->value() + horizontalDelta);
	verticalScrollBar()->setValue(verticalScrollBar()->value() + verticalDelta);
}

void DrawingWidget::centerOnCursor(const QPointF& position)
{
	QPointF oldPosition = mapToScene(mapFromGlobal(QCursor::pos()));

	int horizontalDelta = qRound((position.x() - oldPosition.x()) * mScale);
	int verticalDelta = qRound((position.y() - oldPosition.y()) * mScale);

	horizontalScrollBar()->setValue(horizontalScrollBar()->value() + horizontalDelta);
	verticalScrollBar()->setValue(verticalScrollBar()->value() + verticalDelta);
}

void DrawingWidget::fitToView(const QRectF& rect)
{
	qreal scaleX = (maximumViewportSize().width() - 5) / rect.width();
	qreal scaleY = (maximumViewportSize().height() - 5) / rect.height();

	mScale = qMin(scaleX, scaleY);

	recalculateContentSize(rect);
	centerOn(rect.center());
}

void DrawingWidget::scaleBy(qreal scale)
{
	if (scale > 0)
	{
		QRectF scrollBarRect = scrollBarDefinedRect();

		mScale *= scale;

		recalculateContentSize(scrollBarRect);
	}
}

qreal DrawingWidget::scale() const
{
	return mScale;
}

QPointF DrawingWidget::mapToScene(const QPoint& point) const
{
	QPointF p = point;
	p.setX(p.x() + horizontalScrollBar()->value());
	p.setY(p.y() + verticalScrollBar()->value());
	return mSceneTransform.map(p);
}

QPoint DrawingWidget::mapFromScene(const QPointF& point) const
{
	QPointF p = mViewportTransform.map(point);
	p.setX(p.x() - horizontalScrollBar()->value());
	p.setY(p.y() - verticalScrollBar()->value());
	return p.toPoint();
}

//==================================================================================================

Drawing::Mode DrawingWidget::mode() const
{
	return mMode;
}

//==================================================================================================

QAction* DrawingWidget::addAction(const QString& text, QObject* slotObj, const char* slotFunction,
	const QString& iconPath, const QString& shortcut)
{
	QAction* action = new QAction(text, this);
	connect(action, SIGNAL(triggered()), slotObj, slotFunction);

	if (!iconPath.isEmpty()) action->setIcon(QIcon(iconPath));
	if (!shortcut.isEmpty()) action->setShortcut(QKeySequence(shortcut));

	QAbstractScrollArea::addAction(action);

	return action;
}

QAction* DrawingWidget::addModeAction(const QString& text,
	const QString& iconPath, const QString& shortcut)
{
	QAction* action = new QAction(text, mModeActionGroup);

	if (!iconPath.isEmpty()) action->setIcon(QIcon(iconPath));
	if (!shortcut.isEmpty()) action->setShortcut(QKeySequence(shortcut));

	action->setCheckable(true);
	action->setActionGroup(mModeActionGroup);

	return action;
}

QList<QAction*> DrawingWidget::modeActions() const
{
	return mModeActionGroup->actions();
}

//==================================================================================================

void DrawingWidget::setScale(qreal scale)
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

void DrawingWidget::zoomIn()
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

void DrawingWidget::zoomOut()
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

void DrawingWidget::zoomFit()
{
	fitToView(mSceneRect);

	emit scaleChanged(mScale);
	viewport()->update();
}

//==================================================================================================

void DrawingWidget::setDefaultMode()
{
	mMode = Drawing::DefaultMode;

	setCursor(Qt::ArrowCursor);

	QList<QAction*> modeActionList = modeActions();
	if (!modeActionList[DefaultModeAction]->isChecked())
		modeActionList[DefaultModeAction]->setChecked(true);

	emit modeChanged(mMode);
	viewport()->update();
}

void DrawingWidget::setScrollMode()
{
	mMode = Drawing::ScrollMode;

	setCursor(Qt::OpenHandCursor);

	QList<QAction*> modeActionList = modeActions();
	if (!modeActionList[ScrollModeAction]->isChecked())
		modeActionList[ScrollModeAction]->setChecked(true);

	emit modeChanged(mMode);
	viewport()->update();
}

void DrawingWidget::setZoomMode()
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

	painter.end();

	// Render scene image on to widget
	QPainter widgetPainter(viewport());
	widgetPainter.drawImage(0, 0, image);

	Q_UNUSED(event);
}

void DrawingWidget::resizeEvent(QResizeEvent* event)
{
	QAbstractScrollArea::resizeEvent(event);
	recalculateContentSize();
}

//==================================================================================================

void DrawingWidget::wheelEvent(QWheelEvent* event)
{
	if (event->modifiers() & Qt::ControlModifier)
	{
		if (event->angleDelta().y() > 0) zoomIn();
		else if (event->angleDelta().y() < 0) zoomOut();
	}
	else QAbstractScrollArea::wheelEvent(event);
}

//==================================================================================================

void DrawingWidget::drawBackground(QPainter* painter)
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

	// Draw border
	QPen borderPen(QColor(128, 128, 128), devicePixelRatio());
	borderPen.setCosmetic(true);

	painter->setBrush(Qt::transparent);
	painter->setPen(borderPen);
	painter->drawRect(mSceneRect);

	painter->setRenderHints(renderHints);
	painter->setBrush(mBackgroundBrush);
}

//==================================================================================================

void DrawingWidget::setModeFromAction(QAction* action)
{
	QList<QAction*> modeActionList = modeActions();

	if (action == modeActionList[DefaultModeAction]) setDefaultMode();
	else if (action == modeActionList[ScrollModeAction]) setScrollMode();
	else if (action == modeActionList[ZoomModeAction]) setZoomMode();
	else setDefaultMode();
}

//==================================================================================================

void DrawingWidget::recalculateContentSize(const QRectF& rect)
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
	mViewportTransform.translate(qRound(dx), qRound(dy));
	mViewportTransform.scale(mScale, mScale);

	mSceneTransform = mViewportTransform.inverted();
}

QRectF DrawingWidget::scrollBarDefinedRect() const
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

//==================================================================================================

void DrawingWidget::addActions()
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
