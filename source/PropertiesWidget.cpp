/* PropertiesWidget.cpp
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

#include "PropertiesWidget.h"

PropertiesWidget::PropertiesWidget() : QFrame()
{
	QCheckBox* check = new QCheckBox("Check Me!");
	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(check);
	setLayout(layout);

	check->setCheckable(false);
}

PropertiesWidget::~PropertiesWidget() { }

//==================================================================================================

void PropertiesWidget::setFromItems(const QList<DrawingItem*>& items)
{
	qDebug() << "PropertiesWidget::setFromItems";
}

void PropertiesWidget::setFromItem(DrawingItem* item)
{
	QList<DrawingItem*> items;
	items.append(item);
	setFromItems(items);
}

//==================================================================================================

void PropertiesWidget::updateItems(const QList<DrawingItem*>& items)
{
	qDebug() << "PropertiesWidget::updateItems";
}

void PropertiesWidget::updateItem(DrawingItem* item)
{
	QList<DrawingItem*> items;
	items.append(item);
	updateItems(items);
}

//==================================================================================================

QSize PropertiesWidget::sizeHint() const
{
	return QSize(280, 400);
}
