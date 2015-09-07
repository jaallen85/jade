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

#include "DrawingTypes.h"
#include "DrawingWidget.h"

DrawingMouseEvent::DrawingMouseEvent() : QEvent(QEvent::User)
{
	mButton = Qt::NoButton;
	mButtons = Qt::NoButton;
	mModifiers = Qt::NoModifier;
	mDragged = false;
}

DrawingMouseEvent::~DrawingMouseEvent() { }

//==================================================================================================

void DrawingMouseEvent::setFromEvent(QMouseEvent* event, DrawingWidget* widget)
{
	mPos = event->pos();
	mScenePos = widget->mapToScene(mPos);

	mButton	= event->button();
	mButtons = event->buttons();
	mModifiers = event->modifiers();

	if (event->type() == QEvent::MouseButtonPress)
	{
		mButtonDownPos = mPos;
		mButtonDownScenePos = mScenePos;
		mDragged = false;
	}
	else if (event->type() == QEvent::MouseMove && (mButtons & Qt::LeftButton))
	{
		mDragged = (mDragged |
			((mButtonDownPos - mPos).manhattanLength() >= QApplication::startDragDistance()));
	}
}

//==================================================================================================

QPoint DrawingMouseEvent::pos() const
{
	return mPos;
}

QPointF DrawingMouseEvent::scenePos() const
{
	return mScenePos;
}

QPoint DrawingMouseEvent::buttonDownPos() const
{
	return mButtonDownPos;
}

QPointF DrawingMouseEvent::buttonDownScenePos() const
{
	return mButtonDownScenePos;
}

Qt::MouseButton DrawingMouseEvent::button() const
{
	return mButton;
}

Qt::MouseButtons DrawingMouseEvent::buttons() const
{
	return mButtons;
}

Qt::KeyboardModifiers DrawingMouseEvent::modifiers() const
{
	return mModifiers;
}

bool DrawingMouseEvent::isDragged() const
{
	return mDragged;
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingContextMenuEvent::DrawingContextMenuEvent() : QEvent((QEvent::Type)(QEvent::User + 1))
{
	mReason = QContextMenuEvent::Other;
}

DrawingContextMenuEvent::~DrawingContextMenuEvent() { }

//==================================================================================================

void DrawingContextMenuEvent::setFromEvent(QContextMenuEvent* event, DrawingWidget* widget)
{
	mGlobalPos = event->globalPos();
	mPos = event->pos();
	mScenePos = widget->mapToScene(mPos);
	mReason = event->reason();
}

//==================================================================================================

QPoint DrawingContextMenuEvent::globalPos() const
{
	return mGlobalPos;
}

QPoint DrawingContextMenuEvent::pos() const
{
	return mPos;
}

QPointF DrawingContextMenuEvent::scenePos() const
{
	return mScenePos;
}

QContextMenuEvent::Reason DrawingContextMenuEvent::reason() const
{
	return mReason;
}
