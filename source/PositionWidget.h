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

#ifndef POSITIONWIDGET_H
#define POSITIONWIDGET_H

#include <QtWidgets>

class PositionWidget : public QWidget
{
	Q_OBJECT

private:
	QLineEdit* mXEdit;
	QLineEdit* mYEdit;

public:
	PositionWidget(QWidget* parent = nullptr);
	PositionWidget(const QPointF& pos, QWidget* parent = nullptr);
	~PositionWidget();

	QPointF pos() const;
	qreal x() const;
	qreal y() const;

public slots:
	void setPos(const QPointF& pos);
	void setX(qreal x);
	void setY(qreal y);

signals:
	void positionChanged(const QPointF& pos);

private slots:
	void handleEditingFinished();

private:
	void setupWidget();
};

#endif