/* DiagramWidget.h
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

#ifndef DIAGRAMWIDGET_H
#define DIAGRAMWIDGET_H

#include <Drawing.h>

//undo command for style changes
//may need to provide access to Drawing undo commands for property changes

class DiagramWidget : public DrawingWidget
{
	Q_OBJECT

	friend class DiagramSetItemsStyleCommand;
	friend class DiagramSetItemCornerRadiusCommand;
	friend class DiagramSetItemCaptionCommand;
	friend class DiagramSetPropertiesCommand;

public:
	enum GridRenderStyle { GridNone, GridDots, GridLines, GridGraphPaper };

	enum Property { SceneRect, BackgroundColor, Grid, GridStyle, GridColor,
		GridSpacingMajor, GridSpacingMinor, NumberOfProperties};

	enum ActionIndex { UndoAction, RedoAction, CutAction, CopyAction, PasteAction, DeleteAction,
		SelectAllAction, SelectNoneAction, RotateAction, RotateBackAction, FlipAction,
		BringForwardAction, SendBackwardAction, BringToFrontAction, SendToBackAction,
		InsertPointAction, RemovePointAction, GroupAction, UngroupAction,
		ZoomInAction, ZoomOutAction, ZoomFitAction, PropertiesAction, NumberOfActions };

private:
	GridRenderStyle mGridStyle;
	QBrush mGridBrush;
	int mGridSpacingMajor, mGridSpacingMinor;

	QMenu mSingleItemContextMenu;
	QMenu mSinglePolyItemContextMenu;
	QMenu mMultipleItemContextMenu;
	QMenu mDrawingContextMenu;

	QPoint mButtonDownPos;
	bool mDragged;

public:
	DiagramWidget();
	~DiagramWidget();

	void setGridStyle(GridRenderStyle style);
	void setGridBrush(const QBrush& brush);
	void setGridSpacing(int majorSpacing, int minorSpacing = 1);
	GridRenderStyle gridStyle() const;
	QBrush gridBrush() const;
	int gridSpacingMajor() const;
	int gridSpacingMinor() const;

public slots:
	//void cut();
	//void copy();
	//void paste();

	void setSelectionStyleProperties(const QHash<DrawingItemStyle::Property,QVariant>& properties);
	void setSelectionCornerRadius(qreal radiusX, qreal radiusY);
	void setSelectionCaption(const QString& newCaption);
	void setProperties(const QHash<DiagramWidget::Property,QVariant>& properties);

signals:
	void propertiesTriggered();
	void mouseInfoChanged(const QString& mouseInfo);

	void itemsStyleChanged();
	void itemCornerRadiusChanged();
	void itemCaptionChanged();
	void diagramPropertiesChanged();

protected:
	void drawBackground(QPainter* painter);

	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mouseDoubleClickEvent(QMouseEvent* event);

private slots:
	void updateActionsFromSelection();

private:
	// Functions called by undo command classes
	void setItemsStyle(const QHash< DrawingItem*, QHash<DrawingItemStyle::Property,QVariant> >& properties);
	void setItemCornerRadius(DrawingItem* item, qreal radiusX, qreal radiusY);
	void setItemCaption(DrawingItem* item, const QString& caption);
	void setDiagramProperties(const QHash<DiagramWidget::Property,QVariant>& properties);

	void addActions();
	void createContextMenu();
	QAction* addAction(const QString& text, QObject* slotObj, const char* slotFunction,
		const QString& iconPath = QString(), const QString& shortcut = QString());
};

#endif
