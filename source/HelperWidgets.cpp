/* HelperWidgets.cpp
 *
 * Copyright (C) 2013-2016 Jason Allen
 *
 * This file is part of the jade application.
 *
 * jade is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * jade is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with jade.  If not, see <http://www.gnu.org/licenses/>
 */

#include "HelperWidgets.h"

PositionWidget::PositionWidget(const QPointF& pos) : QWidget()
{
	mXEdit = new PositionEdit(pos.x());
	mYEdit = new PositionEdit(pos.y());

	QHBoxLayout* layout = new QHBoxLayout();
	layout->addWidget(mXEdit);
	layout->addWidget(mYEdit);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(2);
	setLayout(layout);

	connect(mXEdit, SIGNAL(positionChanged(qreal)), this, SLOT(sendPositionChanged()));
	connect(mYEdit, SIGNAL(positionChanged(qreal)), this, SLOT(sendPositionChanged()));
}

PositionWidget::~PositionWidget() { }

//==================================================================================================

void PositionWidget::setPos(const QPointF& pos)
{
	mXEdit->setPos(pos.x());
	mYEdit->setPos(pos.y());
}

QPointF PositionWidget::pos() const
{
	return QPointF(mXEdit->pos(), mYEdit->pos());
}

//==================================================================================================

void PositionWidget::sendPositionChanged()
{
	emit positionChanged(pos());
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

PositionEdit::PositionEdit(qreal pos) : QLineEdit()
{
	setValidator(new QDoubleValidator(-std::numeric_limits<qreal>::max(), std::numeric_limits<qreal>::max(), 8));
	setPos(pos);

	connect(this, SIGNAL(editingFinished()), this, SLOT(sendPositionChanged()));
}

PositionEdit::~PositionEdit() { }

//==================================================================================================

QSize PositionEdit::sizeHint() const
{
	QFontMetrics fontMetrics(font());
	return QSize(fontMetrics.width("8888.8888") + 16, fontMetrics.height() + 2);
}

//==================================================================================================

void PositionEdit::setPos(qreal pos)
{
	setText(QString::number(pos, 'g', 8));
}

qreal PositionEdit::pos() const
{
	bool ok = false;
	qreal value = text().toDouble(&ok);
	return (ok) ? value : 0;
}

//==================================================================================================

void PositionEdit::sendPositionChanged()
{
	emit positionChanged(pos());
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

SizeWidget::SizeWidget(const QSizeF& size) : QWidget()
{
	mXEdit = new SizeEdit(size.width());
	mYEdit = new SizeEdit(size.height());

	QHBoxLayout* layout = new QHBoxLayout();
	layout->addWidget(mXEdit);
	layout->addWidget(mYEdit);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(2);
	setLayout(layout);

	connect(mXEdit, SIGNAL(sizeChanged(qreal)), this, SLOT(sendSizeChanged()));
	connect(mYEdit, SIGNAL(sizeChanged(qreal)), this, SLOT(sendSizeChanged()));
}

SizeWidget::~SizeWidget() { }

//==================================================================================================

void SizeWidget::setSize(const QSizeF& size)
{
	mXEdit->setSize(size.width());
	mYEdit->setSize(size.height());
}

QSizeF SizeWidget::size() const
{
	return QSizeF(mXEdit->size(), mYEdit->size());
}

//==================================================================================================

void SizeWidget::sendSizeChanged()
{
	emit sizeChanged(size());
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

SizeEdit::SizeEdit(qreal size) : QLineEdit()
{
	setValidator(new QDoubleValidator(0, std::numeric_limits<qreal>::max(), 8));
	setSize(size);

	connect(this, SIGNAL(editingFinished()), this, SLOT(sendPositionChanged()));
}

SizeEdit::~SizeEdit() { }

//==================================================================================================

QSize SizeEdit::sizeHint() const
{
	QFontMetrics fontMetrics(font());
	return QSize(fontMetrics.width("8888.8888") + 16, fontMetrics.height() + 2);
}

//==================================================================================================

void SizeEdit::setSize(qreal size)
{
	setText(QString::number(size, 'g', 8));
}

qreal SizeEdit::size() const
{
	bool ok = false;
	qreal value = text().toDouble(&ok);
	return (ok) ? value : 0;
}

//==================================================================================================

void SizeEdit::sendSizeChanged()
{
	emit sizeChanged(size());
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

ColorWidget::ColorWidget(const QColor& color) : QPushButton()
{
	setColor(color);

	ColorSelectWidget* selectWidget = new ColorSelectWidget();
	QWidgetAction* selectWidgetAction = new QWidgetAction(this);
	selectWidgetAction->setDefaultWidget(selectWidget);

	QPushButton* moreColorsButton = new QPushButton("More Colors...");
	QWidgetAction* moreColorsAction = new QWidgetAction(this);
	moreColorsAction->setDefaultWidget(moreColorsButton);

	QMenu* menu = new QMenu(this);
	menu->addAction(selectWidgetAction);
	menu->addAction(moreColorsAction);
	setMenu(menu);

	connect(selectWidget, SIGNAL(colorSelected(const QColor&)), menu, SLOT(hide()));
	connect(selectWidget, SIGNAL(colorSelected(const QColor&)), this, SLOT(setColor(const QColor&)));
	connect(selectWidget, SIGNAL(colorSelected(const QColor&)), this, SIGNAL(colorChanged(const QColor&)));
	connect(moreColorsButton, SIGNAL(clicked(bool)), menu, SLOT(hide()));
	connect(moreColorsButton, SIGNAL(clicked(bool)), this, SLOT(runColorDialog()));
}

ColorWidget::~ColorWidget() { }

//==================================================================================================

void ColorWidget::setColor(const QColor& color)
{
	mColor = color;

	setText(textFromColor(color));
	setIcon(iconFromColor(color));
}

QColor ColorWidget::color() const
{
	return mColor;
}

//==================================================================================================

void ColorWidget::runColorDialog()
{
	QColorDialog colorDialog(this);
	colorDialog.setOptions(QColorDialog::ShowAlphaChannel | QColorDialog::DontUseNativeDialog);

	for(int i = 0; i < ColorSelectWidget::standardColors.size(); i++)
		QColorDialog::setStandardColor(i, ColorSelectWidget::standardColors[i]);
	for(int i = 0; i < ColorSelectWidget::customColors.size(); i++)
		QColorDialog::setCustomColor(i, ColorSelectWidget::customColors[i]);
	colorDialog.setCurrentColor(mColor);

	if (colorDialog.exec() == QDialog::Accepted)
	{
		QColor selectedColor = colorDialog.currentColor();
		QColor color;
		bool standardColor = false, customColor = false;

		for(int i = 0; i < ColorSelectWidget::customColors.size(); i++)
			ColorSelectWidget::customColors[i] = QColorDialog::customColor(i);

		for(int i = 0; i < ColorSelectWidget::standardColors.size(); i++)
		{
			color = ColorSelectWidget::standardColors[i];
			standardColor = (standardColor || (selectedColor.red() == color.red() &&
				selectedColor.green() == color.green() && selectedColor.blue() == color.blue()));
		}
		for(int i = 0; i < ColorSelectWidget::customColors.size(); i++)
		{
			color = ColorSelectWidget::customColors[i];
			customColor = (customColor || (selectedColor.red() == color.red() &&
				selectedColor.green() == color.green() && selectedColor.blue() == color.blue()));
		}

		if (!standardColor && !customColor)
		{
			for(int i = ColorSelectWidget::customColors.size(); i > 0; i--)
				ColorSelectWidget::customColors[i] = ColorSelectWidget::customColors[i-1];
			ColorSelectWidget::customColors[0] = colorDialog.currentColor();
		}

		setColor(selectedColor);
		emit colorChanged(selectedColor);
	}
}

//==================================================================================================

QString ColorWidget::textFromColor(const QColor& color) const
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

QIcon ColorWidget::iconFromColor(const QColor& color) const
{
	QPixmap pixmap(16, 16);

	if (color.alpha() == 0)
		pixmap = ColorSelectWidget::createTransparentPixmap(16, 16);
	else
		pixmap.fill(QColor(color.red(), color.green(), color.blue()));

	QPainter painter(&pixmap);
	painter.setPen(QPen(Qt::black, 1));
	painter.setBrush(Qt::transparent);
	painter.drawRect(0, 0, 15, 15);
	painter.end();

	return QIcon(pixmap);
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

QList<QColor> ColorSelectWidget::standardColors = (QList<QColor>() <<
	QColor(255, 128, 128) << QColor(255, 0, 0) << QColor(128, 64, 64) << QColor(128, 0, 0) << QColor(64, 0, 0) << QColor(0, 0, 0) <<
	QColor(255, 255, 128) << QColor(255, 255, 0) << QColor(255, 128, 64) << QColor(255, 128, 0) << QColor(128, 64, 0) << QColor(64, 64, 64) <<
	QColor(128, 255, 128) << QColor(128, 255, 0) << QColor(0, 255, 0) << QColor(0, 128, 0) << QColor(0, 64, 0) << QColor(128, 128, 128) <<
	QColor(0, 255, 128) << QColor(0, 255, 64) << QColor(0, 128, 128) << QColor(0, 128, 64) << QColor(0, 64, 64) << QColor(160, 160, 160) <<
	QColor(128, 255, 255) << QColor(0, 255, 255) << QColor(0, 64, 128) << QColor(0, 0, 255) << QColor(0, 0, 128) << QColor(192, 192, 192) <<
	QColor(0, 128, 255) << QColor(0, 128, 192) << QColor(128, 128, 255) << QColor(0, 0, 160) << QColor(0, 0, 164) << QColor(224, 224, 224) <<
	QColor(255, 128, 192) << QColor(128, 128, 192) << QColor(128, 0, 64) << QColor(128, 0, 128) << QColor(64, 0, 64) << QColor(255, 255, 255) <<
	QColor(255, 128, 255) << QColor(255, 0, 255) << QColor(255, 0, 128) << QColor(128, 0, 255) << QColor(64, 0, 128) << QColor(255, 255, 255, 0));

QList<QColor> ColorSelectWidget::customColors = (QList<QColor>() <<
	QColor(255, 255, 255) << QColor(255, 255, 255) << QColor(255, 255, 255) << QColor(255, 255, 255) <<
	QColor(255, 255, 255) << QColor(255, 255, 255) << QColor(255, 255, 255) << QColor(255, 255, 255) <<
	QColor(255, 255, 255) << QColor(255, 255, 255) << QColor(255, 255, 255) << QColor(255, 255, 255) <<
	QColor(255, 255, 255) << QColor(255, 255, 255) << QColor(255, 255, 255) << QColor(255, 255, 255));

//==================================================================================================

ColorSelectWidget::ColorSelectWidget() : QWidget()
{
	mHoverColor = QColor(0, 0, 0);
	mHoverRect = QRect();

	setMouseTracking(true);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

ColorSelectWidget::~ColorSelectWidget() { }

//==================================================================================================

QSize ColorSelectWidget::sizeHint() const
{
	int rectSize = ColorSelectWidget::rectSize();
	int margin = ColorSelectWidget::margin();
	int fontHeight = QFontMetrics(font()).height();

	return QSize(8 * rectSize + 9 * margin, 8 * rectSize + 2 * fontHeight + 12 * margin);
}

//==================================================================================================

void ColorSelectWidget::paintEvent(QPaintEvent* event)
{
	int rectSize = ColorSelectWidget::rectSize();
	int margin = ColorSelectWidget::margin();
	int fontHeight = QFontMetrics(font()).height();

	QPainter painter(this);
	QColor color;
	QRect actualRect;
	int yPos = margin;

	QPen pen(Qt::black, 1);
	pen.setCosmetic(true);
	painter.setPen(pen);

	QFont font(ColorSelectWidget::font());
	font.setUnderline(true);
	painter.setFont(font);

	painter.drawText(QRect(margin, yPos, width() - 2 * margin, fontHeight),
		Qt::AlignLeft | Qt::AlignTop, "Basic Colors", &actualRect);
	yPos = fontHeight + 2 * margin;

	for(int i = 0; i < standardColors.size(); i++)
	{
		color = standardColors[i];
		if (color.alpha() == 0)
		{
			painter.drawPixmap((i / 6) * (margin + rectSize) + margin,
				(i % 6) * (margin + rectSize) + yPos,
				createTransparentPixmap(rectSize, rectSize));
			painter.setBrush(Qt::transparent);
			painter.drawRect((i / 6) * (margin + rectSize) + margin,
				(i % 6) * (margin + rectSize) + yPos, rectSize - 1, rectSize - 1);
		}
		else
		{
			painter.setBrush(QColor(color.red(), color.green(), color.blue()));
			painter.drawRect((i / 6) * (margin + rectSize) + margin,
				(i % 6) * (margin + rectSize) + yPos, rectSize - 1, rectSize - 1);
		}
	}
	yPos += 6 * rectSize + 7 * margin;

	painter.drawText(QRect(margin, yPos, width() - 2 * margin, fontHeight),
		Qt::AlignLeft | Qt::AlignTop, "Recent Colors", &actualRect);
	yPos = 2 * fontHeight + 6 * rectSize + 10 * margin;

	for(int i = 0; i < customColors.size(); i++)
	{
		color = customColors[i];
		painter.setBrush(QColor(color.red(), color.green(), color.blue()));
		painter.drawRect((i / 2) * (margin + rectSize) + margin,
			(i % 2) * (margin + rectSize) + yPos, rectSize - 1, rectSize - 1);
	}

	if (mHoverRect.isValid())
	{
		pen.setColor(QColor(197, 197, 197));
		painter.setPen(pen);

		painter.setBrush(Qt::transparent);

		painter.drawRect(mHoverRect);
		painter.drawRect(mHoverRect.adjusted(1, 1, -1, -1));
	}

	painter.end();

	Q_UNUSED(event);
}

//==================================================================================================

void ColorSelectWidget::mouseMoveEvent(QMouseEvent* event)
{
	mHoverColor = colorAt(event->pos(), mHoverRect);
	update();
}

void ColorSelectWidget::mouseReleaseEvent(QMouseEvent* event)
{
	mHoverColor = colorAt(event->pos(), mHoverRect);

	if (mHoverRect.isValid()) emit colorSelected(mHoverColor);
	mHoverColor = QColor(0, 0, 0);
	mHoverRect = QRect();

	update();
}

//==================================================================================================

QColor ColorSelectWidget::colorAt(const QPoint& pos, QRect& colorRect) const
{
	QColor foundColor;

	int rectSize = ColorSelectWidget::rectSize();
	int margin = ColorSelectWidget::margin();
	int fontHeight = QFontMetrics(font()).height();
	QRect rect;

	colorRect = QRect();

	int yPos = fontHeight + 2 * margin;
	for(int i = 0; colorRect.isEmpty() && i < standardColors.size(); i++)
	{
		rect = QRect((i / 6) * (margin + rectSize) + margin,
			(i % 6) * (margin + rectSize) + yPos, rectSize - 1, rectSize - 1);
		if (rect.contains(pos))
		{
			foundColor = standardColors[i];
			colorRect = rect;
		}
	}

	yPos = 2 * fontHeight + 6 * rectSize + 10 * margin;
	for(int i = 0; colorRect.isEmpty() && i < customColors.size(); i++)
	{
		rect = QRect((i / 2) * (margin + rectSize) + margin,
			(i % 2) * (margin + rectSize) + yPos, rectSize - 1, rectSize - 1);
		if (rect.contains(pos))
		{
			foundColor = customColors[i];
			colorRect = rect;
		}
	}
	return foundColor;
}

int ColorSelectWidget::rectSize() const
{
	return QFontMetrics(font()).height();
}

int ColorSelectWidget::margin() const
{
	return rectSize() / 3;
}

//==================================================================================================

QPixmap ColorSelectWidget::createTransparentPixmap(int width, int height)
{
	const int subRectSize = 2;

	QPixmap pixmap(width, height);
	pixmap.fill(QColor(255, 255, 255));

	QPainter painter(&pixmap);

	painter.setPen(Qt::NoPen);
	painter.setBrush(QColor(170, 170, 170));
	for(int y = 1; y < height; y += 2 * subRectSize)
	{
		for(int x = 1; x < width; x += 2 * subRectSize)
		{
			painter.drawRect(x, y, subRectSize, subRectSize);
			painter.drawRect(x + subRectSize, y + subRectSize, subRectSize, subRectSize);
		}
	}

	painter.end();

	return pixmap;
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

ArrowStyleCombo::ArrowStyleCombo(DrawingItemStyle::ArrowStyle style) : QComboBox()
{
	addItem(QIcon(":/icons/arrow/arrow_none.png"), "None");
	addItem(QIcon(":/icons/arrow/arrow_normal.png"), "Normal");
	addItem(QIcon(":/icons/arrow/arrow_reverse.png"), "Reverse");
	addItem(QIcon(":/icons/arrow/arrow_triangle.png"), "Triangle");
	addItem(QIcon(":/icons/arrow/arrow_triangle_filled.png"), "Triangle Filled");
	addItem(QIcon(":/icons/arrow/arrow_concave.png"), "Concave");
	addItem(QIcon(":/icons/arrow/arrow_concave_filled.png"), "Concave Filled");
	addItem(QIcon(":/icons/arrow/arrow_circle.png"), "Circle");
	addItem(QIcon(":/icons/arrow/arrow_circle_filled.png"), "Circle Filled");
	addItem(QIcon(":/icons/arrow/arrow_diamond.png"), "Diamond");
	addItem(QIcon(":/icons/arrow/arrow_diamond_filled.png"), "Diamond Filled");
	addItem(QIcon(":/icons/arrow/arrow_harpoon.png"), "Harpoon");
	addItem(QIcon(":/icons/arrow/arrow_harpoon_mirrored.png"), "Harpoon Mirrored");
	addItem(QIcon(":/icons/arrow/arrow_x.png"), "X");

	setStyle(style);
}

ArrowStyleCombo::~ArrowStyleCombo() { }

void ArrowStyleCombo::setStyle(DrawingItemStyle::ArrowStyle style)
{
	setCurrentIndex((int)style);
}

DrawingItemStyle::ArrowStyle ArrowStyleCombo::style() const
{
	return (DrawingItemStyle::ArrowStyle)currentIndex();
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

PenStyleCombo::PenStyleCombo(Qt::PenStyle style) : QComboBox()
{
	addItem("No Pen");
	addItem("Solid Line");
	addItem("Dashed Line");
	addItem("Dotted Line");
	addItem("Dash-Dotted Line");
	addItem("Dash-Dot-Dotted Line");

	setStyle(style);
}

PenStyleCombo::~PenStyleCombo() { }

void PenStyleCombo::setStyle(Qt::PenStyle style)
{
	setCurrentIndex((int)style);
}

Qt::PenStyle PenStyleCombo::style() const
{
	return (Qt::PenStyle)currentIndex();
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

GridStyleCombo::GridStyleCombo(DiagramWidget::GridRenderStyle style) : QComboBox()
{
	addItem("None");
	addItem("Dotted");
	addItem("Lined");
	addItem("Graph Paper");

	setStyle(style);
}

GridStyleCombo::~GridStyleCombo() { }

void GridStyleCombo::setStyle(DiagramWidget::GridRenderStyle style)
{
	setCurrentIndex((int)style);
}

DiagramWidget::GridRenderStyle GridStyleCombo::style() const
{
	return (DiagramWidget::GridRenderStyle)currentIndex();
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

FontStyleWidget::FontStyleWidget() : QWidget()
{
	mBoldButton = new QToolButton();
	mBoldButton->setIcon(QIcon(":/icons/oxygen/format-text-bold.png"));
	mBoldButton->setToolTip("Bold");
	mBoldButton->setCheckable(true);

	mItalicButton = new QToolButton();
	mItalicButton->setIcon(QIcon(":/icons/oxygen/format-text-italic.png"));
	mItalicButton->setToolTip("Italic");
	mItalicButton->setCheckable(true);

	mUnderlineButton = new QToolButton();
	mUnderlineButton->setIcon(QIcon(":/icons/oxygen/format-text-underline.png"));
	mUnderlineButton->setToolTip("Underline");
	mUnderlineButton->setCheckable(true);

	mStrikeThroughButton = new QToolButton();
	mStrikeThroughButton->setIcon(QIcon(":/icons/oxygen/format-text-strikethrough.png"));
	mStrikeThroughButton->setToolTip("Strike-Through");
	mStrikeThroughButton->setCheckable(true);

	QHBoxLayout* styleLayout = new QHBoxLayout();
	styleLayout->addWidget(mBoldButton);
	styleLayout->addWidget(mItalicButton);
	styleLayout->addWidget(mUnderlineButton);
	styleLayout->addWidget(mStrikeThroughButton);
	styleLayout->addWidget(new QWidget(), 100);
	styleLayout->setSpacing(2);
	styleLayout->setContentsMargins(0, 0, 0, 0);
	setLayout(styleLayout);

	connect(mBoldButton, SIGNAL(toggled(bool)), this, SLOT(boldChanged(bool)));
	connect(mItalicButton, SIGNAL(toggled(bool)), this, SLOT(italicChanged(bool)));
	connect(mUnderlineButton, SIGNAL(toggled(bool)), this, SLOT(underlineChanged(bool)));
	connect(mStrikeThroughButton, SIGNAL(toggled(bool)), this, SLOT(strikeThroughChanged(bool)));
}

FontStyleWidget::~FontStyleWidget() { }

//==================================================================================================

void FontStyleWidget::setBold(bool bold)
{
	mBoldButton->setChecked(bold);
}

void FontStyleWidget::setItalic(bool italic)
{
	mItalicButton->setChecked(italic);
}

void FontStyleWidget::setUnderline(bool underline)
{
	mUnderlineButton->setChecked(underline);
}

void FontStyleWidget::setStrikeThrough(bool strikeThrough)
{
	mStrikeThroughButton->setChecked(strikeThrough);
}

bool FontStyleWidget::isBold() const
{
	return mBoldButton->isChecked();
}

bool FontStyleWidget::isItalic() const
{
	return mItalicButton->isChecked();
}

bool FontStyleWidget::isUnderline() const
{
	return mUnderlineButton->isChecked();
}

bool FontStyleWidget::isStrikeThrough() const
{
	return mStrikeThroughButton->isChecked();
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

TextAlignmentWidget::TextAlignmentWidget() : QWidget()
{
	mLeftAlignButton = new QToolButton();
	mLeftAlignButton->setIcon(QIcon(":/icons/oxygen/align-horizontal-left.png"));
	mLeftAlignButton->setToolTip("Align Left");
	mLeftAlignButton->setCheckable(true);
	mLeftAlignButton->setAutoExclusive(true);

	mHCenterAlignButton = new QToolButton();
	mHCenterAlignButton->setIcon(QIcon(":/icons/oxygen/align-horizontal-center.png"));
	mHCenterAlignButton->setToolTip("Align Center");
	mHCenterAlignButton->setCheckable(true);
	mHCenterAlignButton->setAutoExclusive(true);
	mHCenterAlignButton->setChecked(true);

	mRightAlignButton = new QToolButton();
	mRightAlignButton->setIcon(QIcon(":/icons/oxygen/align-horizontal-right.png"));
	mRightAlignButton->setToolTip("Align Right");
	mRightAlignButton->setCheckable(true);
	mRightAlignButton->setAutoExclusive(true);

	mTopAlignButton = new QToolButton();
	mTopAlignButton->setIcon(QIcon(":/icons/oxygen/align-vertical-top.png"));
	mTopAlignButton->setToolTip("Align Top");
	mTopAlignButton->setCheckable(true);
	mTopAlignButton->setAutoExclusive(true);

	mVCenterAlignButton = new QToolButton();
	mVCenterAlignButton->setIcon(QIcon(":/icons/oxygen/align-vertical-center.png"));
	mVCenterAlignButton->setToolTip("Align Center");
	mVCenterAlignButton->setCheckable(true);
	mVCenterAlignButton->setAutoExclusive(true);
	mVCenterAlignButton->setChecked(true);

	mBottomAlignButton = new QToolButton();
	mBottomAlignButton->setIcon(QIcon(":/icons/oxygen/align-vertical-bottom.png"));
	mBottomAlignButton->setToolTip("Align Bottom");
	mBottomAlignButton->setCheckable(true);
	mBottomAlignButton->setAutoExclusive(true);

	QWidget* horizontalAlignWidget = new QWidget();
	QHBoxLayout* horizontalAlignLayout = new QHBoxLayout();
	horizontalAlignLayout->addWidget(mLeftAlignButton);
	horizontalAlignLayout->addWidget(mHCenterAlignButton);
	horizontalAlignLayout->addWidget(mRightAlignButton);
	horizontalAlignLayout->setSpacing(2);
	horizontalAlignLayout->setContentsMargins(0, 0, 0, 0);
	horizontalAlignWidget->setLayout(horizontalAlignLayout);

	QWidget* verticalAlignWidget = new QWidget();
	QHBoxLayout* verticalAlignLayout = new QHBoxLayout();
	verticalAlignLayout->addWidget(mTopAlignButton);
	verticalAlignLayout->addWidget(mVCenterAlignButton);
	verticalAlignLayout->addWidget(mBottomAlignButton);
	verticalAlignLayout->setSpacing(2);
	verticalAlignLayout->setContentsMargins(0, 0, 0, 0);
	verticalAlignWidget->setLayout(verticalAlignLayout);

	QFrame* separator = new QFrame();
	separator->setFrameStyle(QFrame::VLine | QFrame::Raised);

	QHBoxLayout* textAlignmentLayout = new QHBoxLayout();
	textAlignmentLayout->addWidget(horizontalAlignWidget);
	textAlignmentLayout->addWidget(separator);
	textAlignmentLayout->addWidget(verticalAlignWidget);
	textAlignmentLayout->addWidget(new QWidget(), 100);
	textAlignmentLayout->setContentsMargins(0, 0, 0, 0);
	setLayout(textAlignmentLayout);

	connect(mLeftAlignButton, SIGNAL(toggled(bool)), this, SLOT(handleHorizontalAlignmentChange()));
	connect(mHCenterAlignButton, SIGNAL(toggled(bool)), this, SLOT(handleHorizontalAlignmentChange()));
	connect(mRightAlignButton, SIGNAL(toggled(bool)), this, SLOT(handleHorizontalAlignmentChange()));

	connect(mTopAlignButton, SIGNAL(toggled(bool)), this, SLOT(handleVerticalAlignmentChange()));
	connect(mVCenterAlignButton, SIGNAL(toggled(bool)), this, SLOT(handleVerticalAlignmentChange()));
	connect(mBottomAlignButton, SIGNAL(toggled(bool)), this, SLOT(handleVerticalAlignmentChange()));
}

TextAlignmentWidget::~TextAlignmentWidget() { }

//==================================================================================================

void TextAlignmentWidget::setAlignment(Qt::Alignment horizontal, Qt::Alignment vertical)
{
	horizontal = (horizontal & Qt::AlignHorizontal_Mask);
	vertical = (vertical & Qt::AlignVertical_Mask);

	if (horizontal & Qt::AlignLeft) mLeftAlignButton->setChecked(true);
	else if (horizontal & Qt::AlignRight) mRightAlignButton->setChecked(true);
	else mHCenterAlignButton->setChecked(true);

	if (vertical & Qt::AlignTop) mTopAlignButton->setChecked(true);
	else if (vertical & Qt::AlignBottom) mBottomAlignButton->setChecked(true);
	else mVCenterAlignButton->setChecked(true);
}

Qt::Alignment TextAlignmentWidget::horizontalAlignment() const
{
	Qt::Alignment alignment = Qt::AlignHCenter;

	if (mLeftAlignButton->isChecked()) alignment = Qt::AlignLeft;
	else if (mRightAlignButton->isChecked()) alignment = Qt::AlignRight;

	return alignment;
}

Qt::Alignment TextAlignmentWidget::verticalAlignment() const
{
	Qt::Alignment alignment = Qt::AlignVCenter;

	if (mTopAlignButton->isChecked()) alignment = Qt::AlignTop;
	else if (mBottomAlignButton->isChecked()) alignment = Qt::AlignBottom;

	return alignment;
}

//==================================================================================================

void TextAlignmentWidget::handleHorizontalAlignmentChange()
{
	emit horizontalAlignmentChanged(horizontalAlignment());
}

void TextAlignmentWidget::handleVerticalAlignmentChange()
{
	emit verticalAlignmentChanged(verticalAlignment());
}
