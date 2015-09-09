/* ColorButton.h
 *
 * Copyright (C) 2013-2014 Jason Allen
 *
 * This file is part of the Jade Diagram Editor.
 *
 * Jade is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Jade is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Jade.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QtWidgets>

class ColorWidget;

class ColorPushButton : public QPushButton
{
	Q_OBJECT

private:
	QColor mColor;

	QMenu* mMenu;
	ColorWidget* mColorWidget;
	QPushButton* mMoreColorsButton;

public:
	ColorPushButton();
	~ColorPushButton();

	void setColor(const QColor& color);
	QColor color() const;

signals:
	void colorChanged(const QColor& color);

private slots:
	void widgetColorChanged(const QColor& color);
	void runColorDialog();

private:
	QString textFromColor(const QColor& color);
	QIcon iconFromColor(const QColor& color);
};

//==================================================================================================
		
class ColorWidget : public QWidget
{
	Q_OBJECT

private:
	QColor mSelectedColor;

	QColor mHoverColor;
	QRect mHoverColorRect;

	QList<QColor> mThemeColors;
	QList<QRect> mThemeColorRects;
	QRect mThemeColorTextRect;
	QRect mThemeColorAreaRect;

	QList<QColor> mStandardColors;
	QList<QRect> mStandardColorRects;
	QRect mStandardColorTextRect;
	QRect mStandardColorAreaRect;

public:
	ColorWidget();
	~ColorWidget();

	QColor selectedColor() const;

signals:
	void colorSelected(const QColor& color);

private:
	void paintEvent(QPaintEvent* event);
	void showEvent(QShowEvent* event);

	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

	QColor colorAt(const QPoint& point) const;
	QRect colorRectAt(const QPoint& point) const;

	void rebuildWidget();

public:
	static QPixmap transparentPixmap(const QSize& rectSize, int subRectSize);
};

#endif
