// File: HelperWidgets.cpp
// Copyright (C) 2023  Jason Allen
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

#include "HelperWidgets.h"
#include <QColorDialog>
#include <QHBoxLayout>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QRegularExpression>
#include <QWidgetAction>

LengthEdit::LengthEdit(double length, Odg::Units units, bool lengthMustBeNonNegative) : QLineEdit(),
    mLength(length), mUnits(units), mLengthMustBeNonNegative(lengthMustBeNonNegative), mCachedText()
{
    if (mLengthMustBeNonNegative && mLength < 0) mLength = 0;

    mCachedText = generateText(mLength, mUnits);
    setText(mCachedText);

    connect(this, SIGNAL(editingFinished()), this, SLOT(updateTextAfterEdit()));
}

//======================================================================================================================

void LengthEdit::setLength(double length)
{
    if (length >= 0 || !mLengthMustBeNonNegative)
    {
        if (mLength != length)
        {
            mLength = length;

            mCachedText = generateText(mLength, mUnits);
            setText(mCachedText);

            emit lengthChanged(mLength);
        }
    }
    else setText(mCachedText);
}

void LengthEdit::setUnits(Odg::Units units)
{
    if (mUnits != units)
    {
        mLength = Odg::convertUnits(mLength, mUnits, units);
        mUnits = units;

        mCachedText = generateText(mLength, mUnits);
        setText(mCachedText);
    }
}

double LengthEdit::length() const
{
    return mLength;
}

Odg::Units LengthEdit::units() const
{
    return mUnits;
}

//======================================================================================================================

void LengthEdit::updateTextAfterEdit()
{
    static QRegularExpression validNumberPattern("[-+]?(?:(?:\\d*\\.\\d+)|(?:\\d+\\.?))(?:[Ee][+-]?\\d+)?");
    const QRegularExpressionMatch validNumberMatch = validNumberPattern.match(text());
    if (validNumberMatch.hasMatch())
    {
        bool lengthOk = false;
        const double length = validNumberMatch.captured(0).toDouble(&lengthOk);

        if (lengthOk)
        {
            QString remainingStr = text().mid(validNumberMatch.capturedLength(0)).trimmed();

            if (remainingStr.isEmpty())
            {
                // Assume the value provided is in the same units as mUnits
                setLength(length);

                blockSignals(true);
                clearFocus();
                blockSignals(false);
            }
            else
            {
                // Try to convert the provided value to the same units as mUnits; fail if unrecognized units
                // are provided
                bool providedUnitsOk = false;
                const Odg::Units providedUnits = Odg::unitsFromString(remainingStr, &providedUnitsOk);

                if (providedUnitsOk)
                {
                    setLength(Odg::convertUnits(length, providedUnits, mUnits));

                    blockSignals(true);
                    clearFocus();
                    blockSignals(false);
                }
                else setText(mCachedText);
            }
        }
        else setText(mCachedText);
    }
    else setText(mCachedText);
}

//======================================================================================================================

QString LengthEdit::generateText(double length, Odg::Units units) const
{
    double iPart = 0;
    const double fPart = std::modf(length, &iPart);

    // If length is really an integer, return a string without a decimal point
    if (fPart == 0)
        return QString::number(iPart, 'f', 0) + " " + Odg::unitsToString(units);

    return QString::number(length, 'g', 8) + " " + Odg::unitsToString(units);
}

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

PositionWidget::PositionWidget(const QPointF& position, Odg::Units units) : QWidget(),
    mXEdit(nullptr), mYEdit(nullptr)
{
    mXEdit = new LengthEdit(position.x(), units, false);
    mYEdit = new LengthEdit(position.y(), units, false);

    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(mXEdit);
    layout->addWidget(mYEdit);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);
    setLayout(layout);

    connect(mXEdit, SIGNAL(lengthChanged(double)), this, SLOT(sendPositionChanged()));
    connect(mYEdit, SIGNAL(lengthChanged(double)), this, SLOT(sendPositionChanged()));
}

//======================================================================================================================

void PositionWidget::setPosition(const QPointF& position)
{
    mXEdit->setLength(position.x());
    mYEdit->setLength(position.y());
}

void PositionWidget::setUnits(Odg::Units units)
{
    mXEdit->setUnits(units);
    mYEdit->setUnits(units);
}

QPointF PositionWidget::position() const
{
    return QPointF(mXEdit->length(), mYEdit->length());
}

Odg::Units PositionWidget::units() const
{
    return mXEdit->units();
}

//======================================================================================================================

void PositionWidget::sendPositionChanged()
{
    emit positionChanged(position());
}

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

SizeWidget::SizeWidget(const QSizeF& size, Odg::Units units, bool sizeMustBeNonNegative) : QWidget(),
    mWidthEdit(nullptr), mHeightEdit(nullptr)
{
    mWidthEdit = new LengthEdit(size.width(), units, sizeMustBeNonNegative);
    mHeightEdit = new LengthEdit(size.height(), units, sizeMustBeNonNegative);

    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(mWidthEdit);
    layout->addWidget(mHeightEdit);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);
    setLayout(layout);

    connect(mWidthEdit, SIGNAL(lengthChanged(double)), this, SLOT(sendSizeChanged()));
    connect(mHeightEdit, SIGNAL(lengthChanged(double)), this, SLOT(sendSizeChanged()));
}

//======================================================================================================================

void SizeWidget::setSize(const QSizeF& size)
{
    mWidthEdit->setLength(size.width());
    mHeightEdit->setLength(size.height());
}

void SizeWidget::setUnits(Odg::Units units)
{
    mWidthEdit->setUnits(units);
    mHeightEdit->setUnits(units);
}

QSizeF SizeWidget::size() const
{
    return QSizeF(mWidthEdit->length(), mHeightEdit->length());
}

Odg::Units SizeWidget::units() const
{
    return mWidthEdit->units();
}

//======================================================================================================================

void SizeWidget::sendSizeChanged()
{
    emit sizeChanged(size());
}

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

ColorWidget::ColorWidget(const QColor& color) : QPushButton(),
    mColor(255 - color.red(), 255 - color.green(), 255 - color.blue()), mColorSelectWidget(nullptr),
    mColorSelectWidgetAction(nullptr), mMoreColorsButton(nullptr), mMoreColorsAction(nullptr)
{
    mColorSelectWidget = new ColorSelectWidget();
    mColorSelectWidgetAction = new QWidgetAction(this);
    mColorSelectWidgetAction->setDefaultWidget(mColorSelectWidget);

    mMoreColorsButton = new QPushButton("More Colors...");
    mMoreColorsAction = new QWidgetAction(this);
    mMoreColorsAction->setDefaultWidget(mMoreColorsButton);

    mMenu = new QMenu(this);
    mMenu->addAction(mColorSelectWidgetAction);
    mMenu->addAction(mMoreColorsAction);
    setMenu(mMenu);

    blockSignals(true);
    setColor(color);
    blockSignals(false);

    connect(mColorSelectWidget, SIGNAL(colorSelected(QColor)), mMenu, SLOT(hide()));
    connect(mColorSelectWidget, SIGNAL(colorSelected(QColor)), this, SLOT(setColor(QColor)));
    connect(mMoreColorsButton, SIGNAL(clicked(bool)), mMenu, SLOT(hide()));
    connect(mMoreColorsButton, SIGNAL(clicked(bool)), this, SLOT(runColorDialog()));
}

//======================================================================================================================

void ColorWidget::setColor(const QColor& color)
{
    if (color != mColor)
    {
        mColor = color;
        mColorSelectWidget->selectColor(mColor);

        // Set button text
        if (mColor.alpha() == 255)
            setText(" " + mColor.name(QColor::HexRgb).toUpper());
        else
            setText(" " + mColor.name(QColor::HexArgb).toUpper());

        // Set button icon
        const int pixmapSize = 32;
        QPixmap pixmap(pixmapSize, pixmapSize);
        pixmap.fill(mColor);

        QPainter painter(&pixmap);
        if (mColor.alpha() == 0)
            painter.drawPixmap(QRect(0, 0, pixmapSize - 1, pixmapSize - 1), ColorSelectWidget::createTransparentPixmap());

        painter.setPen(QPen(QBrush(Qt::black), 0));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(0, 0, pixmap.width() - 1, pixmap.height() - 1);

        setIcon(QIcon(pixmap));

        emit colorChanged(mColor);
    }
}

QColor ColorWidget::color() const
{
    return mColor;
}

//======================================================================================================================

void ColorWidget::runColorDialog()
{
    QColorDialog dialog(this);
    dialog.setOptions(QColorDialog::ShowAlphaChannel | QColorDialog::DontUseNativeDialog);

    // Use the same standard/custom colors as ColorSelectWidget
    const int standardColorsSize = qMin(48, ColorSelectWidget::sStandardColors.size());
    for(int standardColorsIndex = 0; standardColorsIndex < standardColorsSize; standardColorsIndex++)
        dialog.setStandardColor(standardColorsIndex, ColorSelectWidget::sStandardColors[standardColorsIndex]);

    const int customColorsSize = qMin(16, ColorSelectWidget::sCustomColors.size());
    for(int customColorsIndex = 0; customColorsIndex < customColorsSize; customColorsIndex++)
        dialog.setCustomColor(customColorsIndex, ColorSelectWidget::sCustomColors[customColorsIndex]);

    dialog.setCurrentColor(mColor);
    dialog.adjustSize();

    if (dialog.exec() == QColorDialog::Accepted)
    {
        // Save the custom colors back to ColorSelectWidget in case they were changed
        for(int customColorsIndex = 0; customColorsIndex < customColorsSize; customColorsIndex++)
            ColorSelectWidget::sCustomColors[customColorsIndex] = dialog.customColor(customColorsIndex);

        setColor(dialog.currentColor());
    }
}

//======================================================================================================================
//======================================================================================================================
//======================================================================================================================

const QList<QColor> ColorSelectWidget::sStandardColors = {
    QColor(250, 209, 209),  // Lighter-Red      #fad1d1
    QColor(240, 117, 117),  // Light-Red        #f07575
    QColor(230, 25, 25),    // Red              #e61919
    QColor(184, 20, 20),    // Dark-Red         #b81414
    QColor(138, 15, 15),    // Darker-Red       #8a0f0f
    QColor(0, 0, 0, 0),     // Transparent

    QColor(253, 226, 206),  // Lighter-Orange   #fde2ce
    QColor(248, 167, 109),  // Light-Orange     #f8a76d
    QColor(244, 108, 11),   // Orange           #f46c0b
    QColor(195, 87, 9),     // Dark-Orange      #c35709
    QColor(146, 65, 7),     // Darker-Orange    #924107
    QColor(255, 255, 255),  // White            #ffffff

    QColor(255, 248, 204),  // Lighter-Yellow   #fff8cc
    QColor(255, 235, 102),  // Light-Yellow     #ffeb66
    QColor(255, 221, 0),    // Yellow           #ffdd00
    QColor(204, 177, 0),    // Dark-Yellow      #ccb100
    QColor(153, 133, 0),    // Darker-Yellow    #998500
    QColor(240, 240, 240),  // Lighter-Gray     #f0f0f0

    QColor(217, 242, 220),  // Lighter-Green    #d9f2dc
    QColor(140, 217, 150),  // Light-Green      #8cd996
    QColor(64, 191, 81),    // Green            #40bf51
    QColor(51, 153, 65),    // Dark-Green       #339941
    QColor(38, 115, 48),    // Darker-Green     #267330
    QColor(192, 192, 192),  // Light-Gray       #c0c0c0

    QColor(207, 244, 252),  // Lighter-Cyan     #cff4fc
    QColor(110, 222, 247),  // Light-Cyan       #6edef7
    QColor(14, 199, 241),   // Cyan             #0ec7f1
    QColor(11, 159, 193),   // Dark-Cyan        #0b9fc1
    QColor(8, 120, 145),    // Darker-Cyan      #087891
    QColor(128, 128, 128),  // Gray             #808080

    QColor(212, 225, 247),  // Lighter-Blue     #d4e1f7
    QColor(127, 165, 230),  // Light-Blue       #7fa5e6
    QColor(43, 106, 213),    // Blue            #2b6ad5
    QColor(34, 84, 170),    // Dark-Blue        #2254aa
    QColor(25, 63, 128),    // Darker-Blue      #193f80
    QColor(96, 96, 96),     // Dark-Gray        #606060

    QColor(239, 211, 248),  // Lighter-Purple   #efd3f8
    QColor(207, 124, 233),  // Light-Purple     #cf7ce9
    QColor(176, 37, 218),   // Purple           #b025da
    QColor(141, 30, 174),   // Dark-Purple      #8d1eae
    QColor(105, 22, 131),   // Darker-Purple    #691683
    QColor(64, 64, 64),     // Darker-Gray      #404040

    QColor(251, 208, 249),  // Lighter-Magenta  #fbd0f9
    QColor(244, 113, 238),  // Light-Magenta    #f471ee
    QColor(237, 18, 226),   // Magenta          #ed12e2
    QColor(190, 14, 181),   // Dark-Magenta     #be0eb5
    QColor(142, 11, 136),   // Darker-Magenta   #8e0b88
    QColor(0, 0, 0),        // Black            #000000
};

QList<QColor> ColorSelectWidget::sCustomColors(16, QColor(255, 255, 255));

//======================================================================================================================

ColorSelectWidget::ColorSelectWidget() : QWidget(),
    mStandardColorsLabelRect(), mStandardColorsRects(sStandardColors.size(), QRect()),
    mCustomColorsLabelRect(), mCustomColorsRects(sCustomColors.size(), QRect()), mHoverColor(), mHoverRect()
{
    setMouseTracking(true);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Determine widget size
    const int fontHeight = QFontMetrics(font()).height();
    const int rectSize = qRound(fontHeight * (double)1.25);
    const int numberOfColumns = 8;
    const int numberOfStandardRows = qCeil(sStandardColors.size() / numberOfColumns);
    const int numberOfCustomRows = qCeil(sCustomColors.size() / numberOfColumns);
    const int padding = qRound(fontHeight / (double)3.0);

    const int fixedWidth = padding + (rectSize + padding) * numberOfColumns;
    const int fixedHeight = (padding + fontHeight + padding + (rectSize + padding) * numberOfStandardRows +
                             fontHeight + padding + (rectSize + padding) * numberOfCustomRows);
    setFixedWidth(fixedWidth);
    setFixedHeight(fixedHeight);

    // Determine layout for standard colors label and color rects
    mStandardColorsLabelRect = QRect(padding, padding, fixedWidth - 2 * padding, fontHeight);

    const int standardColorsSize = sStandardColors.size();
    int rowIndex = 0, columnIndex = 0;
    for(int standardColorsIndex = 0; standardColorsIndex < standardColorsSize; standardColorsIndex++)
    {
        rowIndex = standardColorsIndex % numberOfStandardRows;
        columnIndex = standardColorsIndex / numberOfStandardRows;
        mStandardColorsRects[standardColorsIndex] = QRect(
            padding + (rectSize + padding) * columnIndex,
            mStandardColorsLabelRect.bottom() + padding + (rectSize + padding) * rowIndex,
            rectSize, rectSize);
    }

    // Determine layout for custom colors label and color rects
    mCustomColorsLabelRect = QRect(
        padding, padding + fontHeight + padding + (rectSize + padding) * numberOfStandardRows,
        fixedWidth - 2 * padding, fontHeight);

    const int customColorsSize = sCustomColors.size();
    for(int customColorsIndex = 0; customColorsIndex < customColorsSize; customColorsIndex++)
    {
        rowIndex = customColorsIndex % numberOfCustomRows;
        columnIndex = customColorsIndex / numberOfCustomRows;
        mCustomColorsRects[customColorsIndex] = QRect(
            padding + (rectSize + padding) * columnIndex,
            mCustomColorsLabelRect.bottom() + padding + (rectSize + padding) * rowIndex,
            rectSize, rectSize);
    }
}

//======================================================================================================================

void ColorSelectWidget::selectColor(const QColor& color)
{
    mHoverColor = color;

    // Select the color if it is one of the standard/custom colors
    mHoverRect = QRect();

    const int standardColorsSize = qMin(sStandardColors.size(), mStandardColorsRects.size());
    for(int standardColorsIndex = 0; standardColorsIndex < standardColorsSize; standardColorsIndex++)
    {
        if (sStandardColors[standardColorsIndex] == mHoverColor)
        {
            mHoverRect = mStandardColorsRects[standardColorsIndex];
            break;
        }
    }

    if (mHoverRect.isNull())
    {
        const int customColorsSize = qMin(sCustomColors.size(), mCustomColorsRects.size());
        for(int customColorsIndex = 0; customColorsIndex < customColorsSize; customColorsIndex++)
        {
            if (sCustomColors[customColorsIndex] == mHoverColor)
            {
                mHoverRect = mCustomColorsRects[customColorsIndex];
                break;
            }
        }
    }

    update();
}

QColor ColorSelectWidget::selectedColor() const
{
    return mHoverColor;
}

//======================================================================================================================

void ColorSelectWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    QFont font = this->font();
    font.setUnderline(true);
    const QTextOption textOption = (Qt::AlignLeft | Qt::AlignTop);

    // Draw labels
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(QBrush(Qt::black), 0));
    painter.setFont(font);
    painter.drawText(mStandardColorsLabelRect, "Standard Colors", textOption);
    painter.drawText(mCustomColorsLabelRect, "Custom Colors", textOption);

    // Draw standard colors
    const int standardColorsSize = qMin(sStandardColors.size(), mStandardColorsRects.size());
    QColor color;
    QRect rect;
    for(int standardColorsIndex = 0; standardColorsIndex < standardColorsSize; standardColorsIndex++)
    {
        color = sStandardColors[standardColorsIndex];
        rect = mStandardColorsRects[standardColorsIndex];

        if (color.alpha() == 0)
        {
            painter.drawPixmap(rect.adjusted(0, 0, -1, -1), createTransparentPixmap());

            painter.setBrush(Qt::NoBrush);
            painter.drawRect(rect.adjusted(0, 0, -1, -1));
        }
        else
        {
            painter.setBrush(QBrush(QColor(color.red(), color.green(), color.blue())));
            painter.drawRect(rect.adjusted(0, 0, -1, -1));
        }
    }

    // Draw custom colors
    const int customColorsSize = qMin(sCustomColors.size(), mCustomColorsRects.size());
    for(int customColorsIndex = 0; customColorsIndex < customColorsSize; customColorsIndex++)
    {
        color = sCustomColors[customColorsIndex];
        rect = mCustomColorsRects[customColorsIndex];

        if (color.alpha() == 0)
        {
            painter.drawPixmap(rect.adjusted(0, 0, -1, -1), createTransparentPixmap());

            painter.setBrush(Qt::NoBrush);
            painter.drawRect(rect.adjusted(0, 0, -1, -1));
        }
        else
        {
            painter.setBrush(QBrush(QColor(color.red(), color.green(), color.blue())));
            painter.drawRect(rect.adjusted(0, 0, -1, -1));
        }
    }

    // Draw hover rect
    if (mHoverRect.width() > 0 && mHoverRect.height() > 0)
    {
        painter.setPen(QPen(QBrush(QColor(197, 197, 197)), 1.5 * devicePixelRatioF()));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(mHoverRect.adjusted(0, 0, -1, -1));
    }
}

//======================================================================================================================

void ColorSelectWidget::mouseMoveEvent(QMouseEvent* event)
{
    // Show hover rect if mouse cursor is over one of the standard/custom color rects
    mHoverRect = QRect();

    const int standardColorsSize = qMin(sStandardColors.size(), mStandardColorsRects.size());
    for(int standardColorsIndex = 0; standardColorsIndex < standardColorsSize; standardColorsIndex++)
    {
        if (mStandardColorsRects[standardColorsIndex].contains(event->pos()))
        {
            mHoverColor = sStandardColors[standardColorsIndex];
            mHoverRect = mStandardColorsRects[standardColorsIndex];
            break;
        }
    }

    if (mHoverRect.isNull())
    {
        const int customColorsSize = qMin(sCustomColors.size(), mCustomColorsRects.size());
        for(int customColorsIndex = 0; customColorsIndex < customColorsSize; customColorsIndex++)
        {
            if (mCustomColorsRects[customColorsIndex].contains(event->pos()))
            {
                mHoverColor = sCustomColors[customColorsIndex];
                mHoverRect = mCustomColorsRects[customColorsIndex];
                break;
            }
        }
    }

    update();
}

void ColorSelectWidget::mouseReleaseEvent(QMouseEvent* event)
{
    // Emit the colorSelected signal if the user clicked on a color
    if (mHoverRect.width() > 0 && mHoverRect.height() > 0)
        emit colorSelected(mHoverColor);
}

//======================================================================================================================

QPixmap ColorSelectWidget::createTransparentPixmap()
{
    const int pixmapSize = 48;
    const int rectSize = pixmapSize / 6;

    QPixmap pixmap(pixmapSize, pixmapSize);
    pixmap.fill(QColor(255, 255, 255));

    QPainter painter(&pixmap);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, false);

    // Draw checkerboard pattern
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(170, 170, 170)));
    for(int y = 0; y < pixmapSize; y += 2 * rectSize)
    {
        for(int x = 0; x < pixmapSize; x += 2 * rectSize)
        {
            painter.drawRect(x, y, rectSize, rectSize);
            painter.drawRect(x + rectSize, y + rectSize, rectSize, rectSize);
        }
    }

    painter.end();

    return pixmap;
}
