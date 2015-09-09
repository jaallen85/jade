/* ColorButton.cpp
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

#include "ColorButton.h"

ColorPushButton::ColorPushButton() : QPushButton()
{
	setColor(QColor(0, 0, 0));

	mColorWidget = new ColorWidget();
	connect(mColorWidget, SIGNAL(colorSelected(const QColor&)), this, SLOT(widgetColorChanged(const QColor&)));
	mMoreColorsButton = new QPushButton("More Colors...");
	connect(mMoreColorsButton, SIGNAL(clicked()), this, SLOT(runColorDialog()));

	mMenu = new QMenu(this);
	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->addWidget(mColorWidget);
	vLayout->addWidget(mMoreColorsButton);
	vLayout->setContentsMargins(1, 1, 1, 1);
	mMenu->setLayout(vLayout);
	setMenu(mMenu);

	//QFontMetrics fontMetrics(font());
	//setMaximumWidth(fontMetrics.width("#88888888") * 1.5 + 32);
}

ColorPushButton::~ColorPushButton() { }

//==================================================================================================

void ColorPushButton::setColor(const QColor& color)
{
	mColor = color;
	setText(textFromColor(mColor));
	setIcon(iconFromColor(mColor));
}

QColor ColorPushButton::color() const
{
	return mColor;
}

//==================================================================================================

void ColorPushButton::widgetColorChanged(const QColor& color)
{
	setColor(color);
	emit colorChanged(mColor);
}

void ColorPushButton::runColorDialog()
{
	QColorDialog colorDialog(this);
	colorDialog.setOptions(QColorDialog::ShowAlphaChannel);
	colorDialog.setCurrentColor(mColor);

	if (colorDialog.exec() == QDialog::Accepted)
	{
		setColor(colorDialog.currentColor());
		emit colorChanged(mColor);
	}
}

//==================================================================================================

QString ColorPushButton::textFromColor(const QColor& color)
{
	QString text = "#";

	if (color.red() < 16) text += "0";
	text += QString::number(color.red(), 16).toUpper();
	if (color.green() < 16) text += "0";
	text += QString::number(color.green(), 16).toUpper();
	if (color.blue() < 16) text += "0";
	text += QString::number(color.blue(), 16).toUpper();
	if (color.alpha() != 255)
	{
		if (color.alpha() < 16) text += "0";
		text += QString::number(color.alpha(), 16).toUpper();
	}

	return text;
}

QIcon ColorPushButton::iconFromColor(const QColor& color)
{
	QPixmap pixmap(32, 32);
	QPainter painter(&pixmap);

	if (color.alpha() == 0)
	{
		painter.drawPixmap(QPoint(1, 1), ColorWidget::transparentPixmap(QSize(30, 30), 6));

		painter.setBrush(QBrush(Qt::transparent));
		painter.setPen(QPen(Qt::black, 1));
		painter.drawRect(0, 0, pixmap.width() - 1, pixmap.height() - 1);
	}
	else
	{
		QColor fullColor = color;
		fullColor.setAlpha(255);

		painter.setBrush(QBrush(fullColor));
		painter.setPen(QPen(Qt::black, 1));
		painter.drawRect(0, 0, pixmap.width() - 1, pixmap.height() - 1);
	}

	painter.end();
	return QIcon(pixmap);
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

ColorWidget::ColorWidget() : QWidget()
{
	setMouseTracking(true);

	mThemeColors << QColor(255, 255, 255) << QColor(242, 242, 242) << QColor(216, 216, 216) << QColor(191, 191, 191) << QColor(165, 165, 165) << QColor(127, 127, 127)
		<< QColor(0, 0, 0) << QColor(127, 127, 127) << QColor(89, 89, 89) << QColor(63, 63, 63) << QColor(38, 38, 38) << QColor(12, 12, 12)
		<< QColor(238, 236, 225) << QColor(221, 217, 195) << QColor(196, 189, 151) << QColor(147, 137, 83) << QColor(73, 68, 41) << QColor(29, 27, 16)
		<< QColor(31, 73, 125) << QColor(198, 217, 240) << QColor(141, 179, 226) << QColor(84, 141, 212) << QColor(23, 54, 93) << QColor(15, 36, 62)
		<< QColor(79, 129, 189) << QColor(219, 229, 241) << QColor(184, 204, 228) << QColor(149, 179, 215) << QColor(54, 96, 146) << QColor(36, 64, 97)
		<< QColor(192, 80, 77) << QColor(242, 220, 219) << QColor(229, 185, 183) << QColor(217, 150, 148) << QColor(149, 55, 52) << QColor(99, 36, 35)
		<< QColor(155, 187, 89) << QColor(235, 241, 221) << QColor(215, 227, 188) << QColor(195, 214, 155) << QColor(118, 146, 60) << QColor(79, 97, 40)
		<< QColor(128, 100, 162) << QColor(229, 224, 236) << QColor(204, 193, 217) << QColor(178, 162, 199) << QColor(95, 73, 122) << QColor(63, 49, 81)
		<< QColor(75, 172, 198) << QColor(219, 238, 243) << QColor(183, 221, 232) << QColor(146, 205, 220) << QColor(49, 133, 155) << QColor(32, 88, 103)
		<< QColor(247, 150, 70) << QColor(253, 234, 218) << QColor(251, 213, 181) << QColor(250, 192, 143) << QColor(227, 108, 9) << QColor(151, 72, 6);
	mStandardColors << QColor(0, 0, 0, 0) << QColor(192, 0, 0) << QColor(255, 0, 0) << QColor(255, 128, 0) << QColor(255, 255, 0)
		<< QColor(146, 208, 80) << QColor(0, 128, 0) <<	QColor(0, 176, 240) << QColor(0, 112, 192) << QColor(112, 48, 160);
}

ColorWidget::~ColorWidget() { }

//==================================================================================================

QColor ColorWidget::selectedColor() const
{
	return mSelectedColor;
}

//==================================================================================================

void ColorWidget::paintEvent(QPaintEvent* event)
{
	QFontMetrics fontMetrics(font());
	int rectSize = fontMetrics.height() - 2;
	int margin = rectSize / 3;

	QPainter painter(this);
	painter.setFont(font());

	// Theme color text
	painter.setPen(Qt::NoPen);
	painter.setBrush(palette().brush(QPalette::Window));
	painter.drawRect(mThemeColorTextRect);

	painter.setPen(QPen(palette().brush(QPalette::WindowText), 1));
	painter.drawText(mThemeColorTextRect.adjusted(2 * margin, 0, 0, 0), Qt::AlignLeft | Qt::AlignVCenter, "Theme Colors:");

	// Standard color text
	painter.setPen(Qt::NoPen);
	painter.setBrush(palette().brush(QPalette::Window));
	painter.drawRect(mStandardColorTextRect);

	painter.setPen(QPen(palette().brush(QPalette::WindowText), 1));
	painter.drawText(mStandardColorTextRect.adjusted(2 * margin, 0, 0, 0), Qt::AlignLeft | Qt::AlignVCenter, "Standard Colors:");

	// Theme color area
	painter.setPen(QPen(QColor(197, 197, 197), 1));
	painter.setBrush(palette().brush(QPalette::Base));
	painter.drawRect(mThemeColorAreaRect.adjusted(-1, 0, 1, 0));

	painter.setPen(Qt::NoPen);
	for(int i = 0; i < mThemeColors.length(); i++)
	{
		if (i < mThemeColorRects.length())
		{
			painter.setBrush(mThemeColors[i]);
			painter.drawRect(mThemeColorRects[i]);
		}
	}

	painter.setPen(QPen(QColor(197, 197, 197), 1));
	painter.setBrush(Qt::transparent);
	for(int i = 0; i < mThemeColors.length() / 6; i++)
	{
		painter.drawRect(margin + i * (rectSize + margin), mThemeColorAreaRect.top() + margin,	rectSize - 1, rectSize - 1);
		painter.drawRect(margin + i * (rectSize + margin), mThemeColorAreaRect.top() + 2 * margin + rectSize, rectSize - 1, rectSize * 5 - 1);
	}

	// Standard color area
	painter.setPen(QPen(QColor(197, 197, 197), 1));
	painter.setBrush(palette().brush(QPalette::Base));
	painter.drawRect(mStandardColorAreaRect.adjusted(-1, 0, 1, 0));

	painter.setPen(Qt::NoPen);
	for(int i = 0; i < mStandardColors.length(); i++)
	{
		if (i < mStandardColorRects.length())
		{
			if (mStandardColors[i].alpha() == 0)
				painter.drawPixmap(mStandardColorRects[i].topLeft(), transparentPixmap(mStandardColorRects[i].size(), 3));
			else
			{
				painter.setBrush(mStandardColors[i]);
				painter.drawRect(mStandardColorRects[i]);
			}
		}
	}

	painter.setPen(QPen(QColor(197, 197, 197), 1));
	painter.setBrush(Qt::transparent);
	for(int i = 0; i < mStandardColors.length(); i++)
		painter.drawRect(margin + i * (rectSize + margin), mStandardColorAreaRect.top() + margin, rectSize - 1, rectSize - 1);

	// Hover selection
	if (mHoverColorRect.isValid())
	{
		painter.setPen(QPen(QColor(197, 197, 197), 1));
		painter.setBrush(Qt::transparent);
		painter.drawRect(mHoverColorRect.adjusted(1, 1, -2, -2));
		painter.drawRect(mHoverColorRect.adjusted(0, 0, -1, -1));
	}

	Q_UNUSED(event);
}

void ColorWidget::showEvent(QShowEvent* event)
{
	Q_UNUSED(event);
	rebuildWidget();
}

//==================================================================================================

void ColorWidget::mouseMoveEvent(QMouseEvent* event)
{
	QWidget::mouseMoveEvent(event);

	mHoverColor = colorAt(event->pos());
	mHoverColorRect = colorRectAt(event->pos());

	update();
}

void ColorWidget::mouseReleaseEvent(QMouseEvent* event)
{
	QWidget::mouseReleaseEvent(event);

	QRect selectedColorRect = colorRectAt(event->pos());

	if (selectedColorRect.isValid())
	{
		mSelectedColor = colorAt(event->pos());
		emit colorSelected(mSelectedColor);
	}

	mHoverColor = QColor();
	mHoverColorRect = QRect();

	update();
}

//==================================================================================================

QColor ColorWidget::colorAt(const QPoint& point) const
{
	QColor color;

	for(int i = 0; i < mThemeColorRects.size() && i < mThemeColors.size(); i++)
	{
		if (mThemeColorRects[i].contains(point)) color = mThemeColors[i];
	}

	for(int i = 0; i < mStandardColorRects.size() && i < mStandardColors.size(); i++)
	{
		if (mStandardColorRects[i].contains(point)) color = mStandardColors[i];
	}

	return color;
}

QRect ColorWidget::colorRectAt(const QPoint& point) const
{
	QRect rect;

	for(int i = 0; i < mThemeColorRects.size(); i++)
	{
		if (mThemeColorRects[i].contains(point)) rect = mThemeColorRects[i];
	}

	for(int i = 0; i < mStandardColorRects.size(); i++)
	{
		if (mStandardColorRects[i].contains(point)) rect = mStandardColorRects[i];
	}

	return rect;
}

//==================================================================================================

void ColorWidget::rebuildWidget()
{
	QFontMetrics fontMetrics(font());
	int rectSize = fontMetrics.height() - 2;
	int margin = rectSize / 3;
	int width = qMax(mThemeColors.length() / 6, mStandardColors.length()) * (rectSize + margin) + margin;

	mThemeColorTextRect = QRect(0, 0, width, rectSize + 2 * margin);
	mThemeColorAreaRect = QRect(0, mThemeColorTextRect.bottom(), width, 6 * rectSize + 3 * margin);
	mStandardColorTextRect = QRect(0, mThemeColorAreaRect.bottom(), width, rectSize + 2 * margin);
	mStandardColorAreaRect = QRect(0, mStandardColorTextRect.bottom(), width, rectSize + 2 * margin);
	setFixedSize(width, mStandardColorAreaRect.bottom());

	mThemeColorRects.clear();
	for(int j = 0; j < mThemeColors.length() / 6; j++)
	{
		for(int i = 0; i < 6; i++)
		{
			if (i == 0)
			{
				mThemeColorRects.append(QRect(mThemeColorAreaRect.left() + margin + j * (rectSize + margin),
					mThemeColorAreaRect.top() + margin, rectSize, rectSize));
			}
			else
			{
				mThemeColorRects.append(QRect(mThemeColorAreaRect.left() + margin + j * (rectSize + margin),
					mThemeColorAreaRect.top() + 2 * margin + i * rectSize, rectSize, rectSize));
			}
		}
	}

	for(int i = 0; i < mStandardColors.length(); i++)
	{
		mStandardColorRects.append(QRect(mStandardColorAreaRect.left() + margin + i * (rectSize + margin),
			mStandardColorAreaRect.top() + margin, rectSize, rectSize));
	}
}

//==================================================================================================

QPixmap ColorWidget::transparentPixmap(const QSize& rectSize, int subRectSize)
{
	QPixmap pixmap(rectSize);
	QPainter painter(&pixmap);

	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::white);
	painter.drawRect(0, 0, rectSize.width(), rectSize.height());

	painter.setBrush(QColor(170, 170, 170));
	for(int	y = 0; y < rectSize.height(); y += 2 * subRectSize)
	{
		for(int	x = 0; x < rectSize.width(); x += 2 * subRectSize)
		{
			painter.drawRect(x + 1, y + 1, subRectSize, subRectSize);
			painter.drawRect(x + subRectSize + 1, y + subRectSize + 1, subRectSize, subRectSize);
		}
	}

	painter.end();
	return pixmap;
}
