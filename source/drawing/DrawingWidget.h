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

#include <DrawingTypes.h>
#include <QAbstractScrollArea>

class DrawingWidget : public QAbstractScrollArea
{
	Q_OBJECT

private:
	QRectF mSceneRect;
	QBrush mBackgroundBrush;

	qreal mGrid;
	Drawing::GridStyle mGridStyle;
	QBrush mGridBrush;
	int mGridSpacingMajor, mGridSpacingMinor;

	QTransform mViewportTransform;

public:
	DrawingWidget(QWidget* parent = nullptr);
	virtual ~DrawingWidget();

	void setSceneRect(const QRectF& rect);
	QRectF sceneRect() const;

	void setBackgroundBrush(const QBrush& brush);
	QBrush backgroundBrush() const;

	void setGrid(qreal grid);
	void setGridStyle(Drawing::GridStyle style);
	void setGridBrush(const QBrush& brush);
	void setGridSpacing(int majorSpacing, int minorSpacing = 1);
	qreal grid() const;
	Drawing::GridStyle gridStyle() const;
	QBrush gridBrush() const;
	int gridSpacingMajor() const;
	int gridSpacingMinor() const;

protected:
	void paintEvent(QPaintEvent* event) override;

	virtual void drawBackground(QPainter* painter);
};

#endif
