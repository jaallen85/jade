/* MainStatusBar.cpp
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

#include "MainStatusBar.h"

MainStatusBar::MainStatusBar() : QStatusBar()
{
	mModeLabel = new QLabel("Select Mode");
	mStatusLabel = new QLabel("");
	mNumberOfItemsLabel = new QLabel("0");
	mMouseInfoLabel = new QLabel("");

	mModeLabel->setMinimumWidth(120);
	mStatusLabel->setMinimumWidth(120);
	mNumberOfItemsLabel->setMinimumWidth(80);
	mMouseInfoLabel->setMinimumWidth(240);

	addWidget(mModeLabel);
	addWidget(mStatusLabel);
	addWidget(mNumberOfItemsLabel);
	addWidget(mMouseInfoLabel);
}

MainStatusBar::~MainStatusBar() { }

//==================================================================================================

void MainStatusBar::updateMode(DrawingWidget::Mode mode)
{
	QString modeText = "Select Mode";

	switch (mode)
	{
	case DrawingWidget::ScrollMode: modeText = "Scroll Mode"; break;
	case DrawingWidget::ZoomMode: modeText = "Zoom Mode"; break;
	case DrawingWidget::PlaceMode: modeText = "Place Mode"; break;
	default: break;
	}

	mModeLabel->setText(modeText);
}

void MainStatusBar::updateClean(bool clean)
{
	mStatusLabel->setText((clean) ? "" : "Modified");
}

void MainStatusBar::updateNumberOfItems(int itemCount)
{
	mNumberOfItemsLabel->setText(QString::number(itemCount));
}

void MainStatusBar::updateMouseInfo(const QString& mouseInfo)
{
	mMouseInfoLabel->setText(mouseInfo);
}
