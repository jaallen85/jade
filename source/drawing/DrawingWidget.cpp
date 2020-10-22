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
#include <QPainter>
#include <QScrollBar>
#include <QtMath>

DrawingWidget::DrawingWidget(QWidget* parent) : QAbstractScrollArea(parent)
{
	mSceneRect = QRectF(-100, -100, 8200, 6200);
	mBackgroundBrush = QColor(255, 255, 255);

	mGrid = 50.0;
	mGridStyle = Drawing::GridGraphPaper;
	mGridBrush = QColor(0, 128, 128);
	mGridSpacingMajor = 8;
	mGridSpacingMinor = 2;
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
