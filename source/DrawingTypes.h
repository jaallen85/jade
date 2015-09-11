/* DrawingTypes.cpp
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

#ifndef DRAWINGTYPES_H
#define DRAWINGTYPES_H

#include <QtWidgets>

class DrawingWidget;
class DrawingItem;
class DrawingItemPoint;

enum DrawingGridStyle { GridNone, GridDotted, GridLined, GridGraphPaper };

enum DrawingArrowStyle { ArrowNone, ArrowNormal, ArrowReverse, ArrowTriangle, ArrowTriangleFilled,
	ArrowConcave, ArrowConcaveFilled, ArrowCircle, ArrowCircleFilled,
	ArrowDiamond, ArrowDiamondFilled, ArrowHarpoon, ArrowHarpoonMirrored, ArrowX };

//==================================================================================================

class DrawingMouseEvent : public QEvent
{
private:
	QPoint mPos;
	QPointF mScenePos;

	QPoint mButtonDownPos;
	QPointF mButtonDownScenePos;

	Qt::MouseButton mButton;
	Qt::MouseButtons mButtons;
	Qt::KeyboardModifiers mModifiers;

	bool mDragged;

public:
	DrawingMouseEvent();
	~DrawingMouseEvent();

	void setFromEvent(QMouseEvent* event, DrawingWidget* widget);
	QPoint pos() const;
	QPointF scenePos() const;
	QPoint buttonDownPos() const;
	QPointF buttonDownScenePos() const;
	Qt::MouseButton button() const;
	Qt::MouseButtons buttons() const;
	Qt::KeyboardModifiers modifiers() const;
	bool isDragged() const;
};

#endif
