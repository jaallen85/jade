// DrawingHelperWidgets.cpp
// Copyright (C) 2020  Jason Allen
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "DrawingHelperWidgets.h"
#include <QColorDialog>
#include <QHBoxLayout>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOptionButton>
#include <QStylePainter>
#include <QToolButton>
#include <QWidgetAction>

DrawingHideableCheckBox::DrawingHideableCheckBox(bool checkHidden, const QString& text,
	QWidget* parent) : QCheckBox(text, parent)
{
	mHideCheck = checkHidden;
}

DrawingHideableCheckBox::DrawingHideableCheckBox(const QString& text, QWidget* parent) :
	QCheckBox(text, parent)
{
	mHideCheck = true;
}

DrawingHideableCheckBox::DrawingHideableCheckBox(QWidget* parent) : QCheckBox(parent)
{
	mHideCheck = true;
}

DrawingHideableCheckBox::~DrawingHideableCheckBox() { }

//==================================================================================================

void DrawingHideableCheckBox::setCheckHidden(bool hidden)
{
	mHideCheck = hidden;
}

bool DrawingHideableCheckBox::isCheckHidden() const
{
	return mHideCheck;
}

//==================================================================================================

void DrawingHideableCheckBox::paintEvent(QPaintEvent* event)
{
	QStyle *style = QWidget::style();

	if (!mHideCheck)
	{
		// Draw check box as normal
		QStylePainter p(this);
		QStyleOptionButton opt;
		initStyleOption(&opt);

		QStyleOptionButton subopt = opt;

		subopt.rect = style->subElementRect(QStyle::SE_CheckBoxIndicator, &opt, this);
		style->proxy()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &subopt, &p, this);

		subopt.rect = style->subElementRect(QStyle::SE_CheckBoxContents, &opt, this);
		style->proxy()->drawControl(QStyle::CE_CheckBoxLabel, &subopt, &p, this);

		if (opt.state & QStyle::State_HasFocus)
		{
			QStyleOptionFocusRect fropt;
			fropt.QStyleOption::operator=(opt);
			fropt.rect = style->subElementRect(QStyle::SE_CheckBoxFocusRect, &opt, this);
			style->proxy()->drawPrimitive(QStyle::PE_FrameFocusRect, &fropt, &p, this);
		}
	}
	else
	{
		// Draw as a label (no check box)
		QPainter p(this);
		QStyleOption opt;
		opt.initFrom(this);

		style->drawItemText(&p, contentsRect(), (Qt::AlignLeft | Qt::AlignVCenter),
			opt.palette, isEnabled(), text(), foregroundRole());
	}

	Q_UNUSED(event);
}

//==================================================================================================

void DrawingHideableCheckBox::mousePressEvent(QMouseEvent* event)
{
	if (!mHideCheck) QCheckBox::mousePressEvent(event);
}

void DrawingHideableCheckBox::mouseMoveEvent(QMouseEvent* event)
{
	if (!mHideCheck) QCheckBox::mouseMoveEvent(event);
}

void DrawingHideableCheckBox::mouseReleaseEvent(QMouseEvent* event)
{
	if (!mHideCheck) QCheckBox::mouseReleaseEvent(event);
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingPositionWidget::DrawingPositionWidget(const QPointF& position) : QWidget()
{
	mXEdit = new DrawingPositionEdit(position.x());
	mYEdit = new DrawingPositionEdit(position.y());

	QHBoxLayout* layout = new QHBoxLayout();
	layout->addWidget(mXEdit);
	layout->addWidget(mYEdit);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(2);
	setLayout(layout);

	connect(mXEdit, SIGNAL(positionChanged(qreal)), this, SLOT(sendPositionChanged()));
	connect(mYEdit, SIGNAL(positionChanged(qreal)), this, SLOT(sendPositionChanged()));
}

DrawingPositionWidget::~DrawingPositionWidget() { }

//==================================================================================================

void DrawingPositionWidget::setPosition(const QPointF& position)
{
	mXEdit->setPosition(position.x());
	mYEdit->setPosition(position.y());
}

QPointF DrawingPositionWidget::position() const
{
	return QPointF(mXEdit->position(), mYEdit->position());
}

//==================================================================================================

void DrawingPositionWidget::sendPositionChanged()
{
	emit positionChanged(position());
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingPositionEdit::DrawingPositionEdit(qreal position) : QLineEdit()
{
	setValidator(new QDoubleValidator(-std::numeric_limits<qreal>::max(), std::numeric_limits<qreal>::max(), 8));
	setPosition(position);

	//connect(this, SIGNAL(returnPressed()), this, SLOT(sendPositionChanged()));
	connect(this, SIGNAL(editingFinished()), this, SLOT(sendPositionChanged()));
}

DrawingPositionEdit::~DrawingPositionEdit() { }

//==================================================================================================

QSize DrawingPositionEdit::sizeHint() const
{
	QFontMetrics fontMetrics(font());
	return fontMetrics.boundingRect("8888.8888").size() + QSize(16, 2);
}

//==================================================================================================

void DrawingPositionEdit::setPosition(qreal position)
{
	setText(QString::number(position, 'g', 8));
	mPositionCached = position;
}

qreal DrawingPositionEdit::position() const
{
	bool ok = false;
	qreal value = text().toDouble(&ok);
	return (ok) ? value : 0;
}

//==================================================================================================

void DrawingPositionEdit::sendPositionChanged()
{
	qreal position = DrawingPositionEdit::position();
	if (mPositionCached != position)
	{
		mPositionCached = position;
		emit positionChanged(position);
	}
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingSizeWidget::DrawingSizeWidget(const QSizeF& size) : QWidget()
{
	mXEdit = new DrawingSizeEdit(size.width());
	mYEdit = new DrawingSizeEdit(size.height());

	QHBoxLayout* layout = new QHBoxLayout();
	layout->addWidget(mXEdit);
	layout->addWidget(mYEdit);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(2);
	setLayout(layout);

	connect(mXEdit, SIGNAL(sizeChanged(qreal)), this, SLOT(sendSizeChanged()));
	connect(mYEdit, SIGNAL(sizeChanged(qreal)), this, SLOT(sendSizeChanged()));
}

DrawingSizeWidget::~DrawingSizeWidget() { }

//==================================================================================================

void DrawingSizeWidget::setSize(const QSizeF& size)
{
	mXEdit->setSize(size.width());
	mYEdit->setSize(size.height());
}

QSizeF DrawingSizeWidget::size() const
{
	return QSizeF(mXEdit->size(), mYEdit->size());
}

//==================================================================================================

void DrawingSizeWidget::sendSizeChanged()
{
	emit sizeChanged(size());
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingSizeEdit::DrawingSizeEdit(qreal size) : QLineEdit()
{
	setValidator(new QDoubleValidator(0, std::numeric_limits<qreal>::max(), 8));
	setSize(size);

	//connect(this, SIGNAL(returnPressed()), this, SLOT(sendSizeChanged()));
	connect(this, SIGNAL(editingFinished()), this, SLOT(sendSizeChanged()));
}

DrawingSizeEdit::~DrawingSizeEdit() { }

//==================================================================================================

QSize DrawingSizeEdit::sizeHint() const
{
	QFontMetrics fontMetrics(font());
	return fontMetrics.boundingRect("8888.8888").size() + QSize(16, 2);
}

//==================================================================================================

void DrawingSizeEdit::setSize(qreal size)
{
	setText(QString::number(size, 'g', 8));
	mSizeCached = size;
}

qreal DrawingSizeEdit::size() const
{
	bool ok = false;
	qreal value = text().toDouble(&ok);
	return (ok) ? value : 0;
}

//==================================================================================================

void DrawingSizeEdit::sendSizeChanged()
{
	qreal size = DrawingSizeEdit::size();
	if (mSizeCached != size)
	{
		mSizeCached = size;
		emit sizeChanged(size);
	}
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingColorWidget::DrawingColorWidget(const QColor& color) : QPushButton()
{
	setColor(color);

	DrawingColorSelectWidget* selectWidget = new DrawingColorSelectWidget();
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

DrawingColorWidget::~DrawingColorWidget() { }

//==================================================================================================

void DrawingColorWidget::setColor(const QColor& color)
{
	mColor = color;

	setText(textFromColor(color));
	setIcon(iconFromColor(color));
}

QColor DrawingColorWidget::color() const
{
	return mColor;
}

//==================================================================================================

void DrawingColorWidget::runColorDialog()
{
	QColorDialog colorDialog(this);
	colorDialog.setOptions(QColorDialog::ShowAlphaChannel | QColorDialog::DontUseNativeDialog);

	for(int i = 0; i < DrawingColorSelectWidget::standardColors.size(); i++)
		QColorDialog::setStandardColor(i, DrawingColorSelectWidget::standardColors[i]);
	for(int i = 0; i < DrawingColorSelectWidget::customColors.size(); i++)
		QColorDialog::setCustomColor(i, DrawingColorSelectWidget::customColors[i]);
	colorDialog.setCurrentColor(mColor);

	if (colorDialog.exec() == QDialog::Accepted)
	{
		QColor selectedColor = colorDialog.currentColor();
		QColor color;
		bool standardColor = false, customColor = false;

		for(int i = 0; i < DrawingColorSelectWidget::customColors.size(); i++)
			DrawingColorSelectWidget::customColors[i] = QColorDialog::customColor(i);

		for(int i = 0; i < DrawingColorSelectWidget::standardColors.size(); i++)
		{
			color = DrawingColorSelectWidget::standardColors[i];
			standardColor = (standardColor || (selectedColor.red() == color.red() &&
				selectedColor.green() == color.green() && selectedColor.blue() == color.blue()));
		}
		for(int i = 0; i < DrawingColorSelectWidget::customColors.size(); i++)
		{
			color = DrawingColorSelectWidget::customColors[i];
			customColor = (customColor || (selectedColor.red() == color.red() &&
				selectedColor.green() == color.green() && selectedColor.blue() == color.blue()));
		}

		if (!standardColor && !customColor)
		{
			for(int i = DrawingColorSelectWidget::customColors.size() - 1; i > 0; i--)
				DrawingColorSelectWidget::customColors[i] = DrawingColorSelectWidget::customColors[i-1];
			DrawingColorSelectWidget::customColors[0] = colorDialog.currentColor();
		}

		setColor(selectedColor);
		emit colorChanged(selectedColor);
	}
}

//==================================================================================================

QString DrawingColorWidget::textFromColor(const QColor& color) const
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

QIcon DrawingColorWidget::iconFromColor(const QColor& color) const
{
	QPixmap pixmap(16, 16);

	if (color.alpha() == 0)
		pixmap = DrawingColorSelectWidget::createTransparentPixmap(16, 16);
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

QList<QColor> DrawingColorSelectWidget::standardColors = (QList<QColor>() <<
	QColor(255, 128, 128) << QColor(255, 0, 0) << QColor(128, 64, 64) << QColor(128, 0, 0) << QColor(64, 0, 0) << QColor(0, 0, 0) <<
	QColor(255, 255, 128) << QColor(255, 255, 0) << QColor(255, 128, 64) << QColor(255, 128, 0) << QColor(128, 64, 0) << QColor(64, 64, 64) <<
	QColor(128, 255, 128) << QColor(128, 255, 0) << QColor(0, 255, 0) << QColor(0, 128, 0) << QColor(0, 64, 0) << QColor(128, 128, 128) <<
	QColor(0, 255, 128) << QColor(0, 255, 64) << QColor(0, 128, 128) << QColor(0, 128, 64) << QColor(0, 64, 64) << QColor(160, 160, 160) <<
	QColor(128, 255, 255) << QColor(0, 255, 255) << QColor(0, 64, 128) << QColor(0, 0, 255) << QColor(0, 0, 128) << QColor(192, 192, 192) <<
	QColor(0, 128, 255) << QColor(0, 128, 192) << QColor(128, 128, 255) << QColor(0, 0, 160) << QColor(0, 0, 164) << QColor(224, 224, 224) <<
	QColor(255, 128, 192) << QColor(128, 128, 192) << QColor(128, 0, 64) << QColor(128, 0, 128) << QColor(64, 0, 64) << QColor(255, 255, 255) <<
	QColor(255, 128, 255) << QColor(255, 0, 255) << QColor(255, 0, 128) << QColor(128, 0, 255) << QColor(64, 0, 128) << QColor(255, 255, 255, 0));

QList<QColor> DrawingColorSelectWidget::customColors = (QList<QColor>() <<
	QColor(255, 255, 255) << QColor(255, 255, 255) << QColor(255, 255, 255) << QColor(255, 255, 255) <<
	QColor(255, 255, 255) << QColor(255, 255, 255) << QColor(255, 255, 255) << QColor(255, 255, 255) <<
	QColor(255, 255, 255) << QColor(255, 255, 255) << QColor(255, 255, 255) << QColor(255, 255, 255) <<
	QColor(255, 255, 255) << QColor(255, 255, 255) << QColor(255, 255, 255) << QColor(255, 255, 255));

//==================================================================================================

DrawingColorSelectWidget::DrawingColorSelectWidget() : QWidget()
{
	mHoverColor = QColor(0, 0, 0);
	mHoverRect = QRect();

	setMouseTracking(true);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

DrawingColorSelectWidget::~DrawingColorSelectWidget() { }

//==================================================================================================

QSize DrawingColorSelectWidget::sizeHint() const
{
	int rectSize = DrawingColorSelectWidget::rectSize();
	int margin = DrawingColorSelectWidget::margin();
	int fontHeight = QFontMetrics(font()).height();

	return QSize(8 * rectSize + 9 * margin, 8 * rectSize + 2 * fontHeight + 12 * margin);
}

//==================================================================================================

void DrawingColorSelectWidget::paintEvent(QPaintEvent* event)
{
	int rectSize = DrawingColorSelectWidget::rectSize();
	int margin = DrawingColorSelectWidget::margin();
	int fontHeight = QFontMetrics(font()).height();

	QPainter painter(this);
	QColor color;
	QRect actualRect;
	int yPos = margin;

	QPen pen(Qt::black, 1);
	pen.setCosmetic(true);
	painter.setPen(pen);

	QFont font(DrawingColorSelectWidget::font());
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

void DrawingColorSelectWidget::mouseMoveEvent(QMouseEvent* event)
{
	mHoverColor = colorAt(event->pos(), mHoverRect);
	update();
}

void DrawingColorSelectWidget::mouseReleaseEvent(QMouseEvent* event)
{
	mHoverColor = colorAt(event->pos(), mHoverRect);

	if (mHoverRect.isValid()) emit colorSelected(mHoverColor);
	mHoverColor = QColor(0, 0, 0);
	mHoverRect = QRect();

	update();
}

//==================================================================================================

QColor DrawingColorSelectWidget::colorAt(const QPoint& position, QRect& rect) const
{
	QColor foundColor;

	int rectSize = DrawingColorSelectWidget::rectSize();
	int margin = DrawingColorSelectWidget::margin();
	int fontHeight = QFontMetrics(font()).height();
	QRect testRect;

	rect = QRect();

	int yPos = fontHeight + 2 * margin;
	for(int i = 0; rect.isEmpty() && i < standardColors.size(); i++)
	{
		testRect = QRect((i / 6) * (margin + rectSize) + margin,
			(i % 6) * (margin + rectSize) + yPos, rectSize - 1, rectSize - 1);
		if (testRect.contains(position))
		{
			foundColor = standardColors[i];
			rect = testRect;
		}
	}

	yPos = 2 * fontHeight + 6 * rectSize + 10 * margin;
	for(int i = 0; rect.isEmpty() && i < customColors.size(); i++)
	{
		testRect = QRect((i / 2) * (margin + rectSize) + margin,
			(i % 2) * (margin + rectSize) + yPos, rectSize - 1, rectSize - 1);
		if (testRect.contains(position))
		{
			foundColor = customColors[i];
			rect = testRect;
		}
	}
	return foundColor;
}

int DrawingColorSelectWidget::rectSize() const
{
	return QFontMetrics(font()).height();
}

int DrawingColorSelectWidget::margin() const
{
	return rectSize() / 3;
}

//==================================================================================================

QPixmap DrawingColorSelectWidget::createTransparentPixmap(int width, int height)
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
