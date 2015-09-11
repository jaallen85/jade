/* PositionWidget.cpp
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

#include "PositionWidget.h"

PositionWidget::PositionWidget(QWidget* parent) : QWidget(parent)
{
	setupWidget();
}

PositionWidget::PositionWidget(const QPointF& pos, QWidget* parent) : QWidget(parent)
{
	setupWidget();
	setPos(pos);
}

PositionWidget::~PositionWidget() { }

//==================================================================================================

void PositionWidget::setPos(const QPointF& pos)
{
	mXEdit->setText(QString::number(pos.x()));
	mYEdit->setText(QString::number(pos.y()));
}

void PositionWidget::setX(qreal x)
{
	mXEdit->setText(QString::number(x));
}

void PositionWidget::setY(qreal y)
{
	mYEdit->setText(QString::number(y));
}

QPointF PositionWidget::pos() const
{
	return QPointF(mXEdit->text().toDouble(), mYEdit->text().toDouble());
}

qreal PositionWidget::x() const
{
	return mXEdit->text().toDouble();
}

qreal PositionWidget::y() const
{
	return mYEdit->text().toDouble();
}

//==================================================================================================

void PositionWidget::handleEditingFinished()
{
	emit positionChanged(pos());
}

//==================================================================================================

void PositionWidget::setupWidget()
{
	mXEdit = new QLineEdit("0");
	mYEdit = new QLineEdit("0");

	mXEdit->setValidator(new QDoubleValidator(std::numeric_limits<qreal>::min(), std::numeric_limits<qreal>::max(), 6));
	mYEdit->setValidator(new QDoubleValidator(std::numeric_limits<qreal>::min(), std::numeric_limits<qreal>::max(), 6));

	QHBoxLayout* hLayout = new QHBoxLayout();
	hLayout->addWidget(mXEdit);
	hLayout->addWidget(mYEdit);
	hLayout->setSpacing(2);
	hLayout->setContentsMargins(0, 0, 0, 0);
	setLayout(hLayout);

	connect(mXEdit, SIGNAL(editingFinished()), this, SLOT(handleEditingFinished()));
	connect(mYEdit, SIGNAL(editingFinished()), this, SLOT(handleEditingFinished()));
}
