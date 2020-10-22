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

public:
	enum ActionIndex { ZoomInAction, ZoomOutAction, ZoomFitAction, NumberOfActions };

private:
	QRectF mSceneRect;
	QBrush mBackgroundBrush;

	qreal mGrid;
	Drawing::GridStyle mGridStyle;
	QBrush mGridBrush;
	int mGridSpacingMajor, mGridSpacingMinor;

	qreal mScale;
	QTransform mViewportTransform;
	QTransform mSceneTransform;

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

	void centerOn(const QPointF& position);
	void centerOnCursor(const QPointF& position);
	void fitToView(const QRectF& rect);
	void scaleBy(qreal scale);
	qreal scale() const;
	QPoint mapFromScene(const QPointF& point) const;
	QPointF mapToScene(const QPoint& point) const;

	QAction* addAction(const QString& text, QObject* slotObj, const char* slotFunction,
		const QString& iconPath = QString(), const QString& shortcut = QString());

signals:
	void scaleChanged(qreal scale);

public slots:
	void setScale(qreal scale);
	void zoomIn();
	void zoomOut();
	void zoomFit();

protected:
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void resizeEvent(QResizeEvent* event) override;

	virtual void wheelEvent(QWheelEvent* event) override;

	virtual void drawBackground(QPainter* painter);

private:
	void recalculateContentSize(const QRectF& rect = QRectF());
	QRectF scrollBarDefinedRect() const;

	void addActions();
};

#endif
