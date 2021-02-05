// DrawingCanvas.h
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

#ifndef DRAWINGCANVAS_H
#define DRAWINGCANVAS_H

#include <DrawingTypes.h>
#include <QAbstractScrollArea>
#include <QTimer>

class QActionGroup;

class DrawingCanvas : public QAbstractScrollArea
{
	Q_OBJECT

public:
	enum ActionIndex { ZoomInAction, ZoomOutAction, ZoomFitAction, NumberOfCanvasActions };
	enum ModeActionIndex { DefaultModeAction, ScrollModeAction, ZoomModeAction, NumberOfCanvasModeActions };

private:
	QRectF mSceneRect;
	QBrush mBackgroundBrush;

	qreal mGrid;
	Drawing::GridStyle mGridStyle;
	QBrush mGridBrush;
	int mGridSpacingMajor, mGridSpacingMinor;

	qreal mDynamicGridBaseValue;
	bool mDynamicGridEnabled;

	qreal mScale;
	QTransform mViewportTransform;
	QTransform mSceneTransform;

	Drawing::Mode mMode;

	QPoint mMouseLeftButtonDownPos;
	bool mMouseLeftDragged;
	int mMouseButtonDownHorizontalScrollValue;
	int mMouseButtonDownVerticalScrollValue;
	QRect mRubberBandZoomRect;
	QPoint mPanStartPos;
	QPoint mPanCurrentPos;
	QTimer mPanTimer;

	QActionGroup* mModeActionGroup;

public:
	DrawingCanvas(QWidget* parent = nullptr);
	virtual ~DrawingCanvas();

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
	qreal roundToGrid(qreal value) const;
	QPointF roundToGrid(const QPointF& position) const;

	void setDynamicGrid(qreal baseValue);
	void setDynamicGridEnabled(bool enable);
	qreal dynamicGrid() const;
	bool isDynamicGridEnabled() const;

	void centerOn(const QPointF& position);
	void centerOnCursor(const QPointF& position);
	void fitToView(const QRectF& rect);
	void scaleBy(qreal scale);
	qreal scale() const;
	QPoint mapFromScene(const QPointF& point) const;
	QRect mapFromScene(const QRectF& rect) const;
	QPointF mapToScene(const QPoint& point) const;
	QRectF mapToScene(const QRect& rect) const;

	void setMode(Drawing::Mode mode);
	Drawing::Mode mode() const;

	QAction* addAction(const QString& text, QObject* slotObj, const char* slotFunction,
		const QString& iconPath = QString(), const QString& shortcut = QString());
	QAction* addModeAction(const QString& text,
		const QString& iconPath = QString(), const QString& shortcut = QString());
	QList<QAction*> modeActions() const;

signals:
	void gridChanged(qreal grid);
	void scaleChanged(qreal scale);
	void modeChanged(Drawing::Mode mode);
	void modeActionChanged(QAction* action);

public slots:
	void setScale(qreal scale);
	void zoomIn();
	void zoomOut();
	void zoomFit();

	virtual void setDefaultMode();
	virtual void setScrollMode();
	virtual void setZoomMode();

protected slots:
	virtual void setModeFromAction(QAction* action);

protected:
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void resizeEvent(QResizeEvent* event) override;

	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
	virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
	virtual void wheelEvent(QWheelEvent* event) override;

	virtual void drawBackground(QPainter* painter);
	virtual void drawMain(QPainter* painter);
	virtual void drawForeground(QPainter* painter);
	virtual void drawRubberBand(QPainter* painter, const QRect& rect);

protected slots:
	void mousePanEvent();

private:
	void recalculateContentSize(const QRectF& rect = QRectF());
	void updateDynamicGrid();
	QRectF scrollBarDefinedRect() const;
	QRectF visibleRect() const;

	void addActions();
};

#endif
