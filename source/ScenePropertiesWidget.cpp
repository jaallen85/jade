/* ScenePropertiesWidget.cpp
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

#include "ScenePropertiesWidget.h"
#include "ColorButton.h"
#include "PositionWidget.h"

ScenePropertiesWidget::ScenePropertiesWidget() : QWidget()
{
	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->addWidget(new QWidget(), 100);
	setLayout(vLayout);

	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
}

ScenePropertiesWidget::~ScenePropertiesWidget() { }

//==================================================================================================

QSize ScenePropertiesWidget::sizeHint() const
{
	return QSize(260, layout()->sizeHint().height());
}
