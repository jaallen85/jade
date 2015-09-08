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

#ifndef MAINSTATUSBAR_H
#define MAINSTATUSBAR_H

#include <DrawingWidget.h>

class MainStatusBar : public QStatusBar
{
	Q_OBJECT

private:
	QLabel* mModeLabel;
	QLabel* mStatusLabel;
	QLabel* mNumberOfItemsLabel;
	QLabel* mMouseInfoLabel;

public:
	MainStatusBar();
	~MainStatusBar();

public slots:
	void updateMode(DrawingWidget::Mode mode);
	void updateClean(bool clean);
	void updateNumberOfItems(int itemCount);
	void updateMouseInfo(const QString& mouseInfo);
};

#endif
