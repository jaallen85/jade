// File: HelperWidgets.h
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

#ifndef HELPERWIDGETS_H
#define HELPERWIDGETS_H

#include <QLineEdit>
#include <QPushButton>
#include "OdgGlobal.h"

class LengthEdit : public QLineEdit
{
    Q_OBJECT

private:
    qreal mLength;
    Odg::Units mUnits;
    bool mLengthMustBeNonNegative;

    QString mCachedText;

public:
    LengthEdit(qreal length = 0, Odg::Units units = Odg::UnitsMillimeters, bool lengthMustBeNonNegative = false);

    void setLength(qreal length);
    void setUnits(Odg::Units units);
    qreal length() const;
    Odg::Units units() const;

signals:
    void lengthChanged(qreal length);

private slots:
    void updateTextAfterEdit();

private:
    QString generateText(qreal length, Odg::Units units) const;
};

//======================================================================================================================

class PositionWidget : public QWidget
{
    Q_OBJECT

private:
    LengthEdit* mXEdit;
    LengthEdit* mYEdit;

public:
    PositionWidget(const QPointF& position = QPointF(0, 0), Odg::Units units = Odg::UnitsMillimeters);

    void setPosition(const QPointF& size);
    void setUnits(Odg::Units units);
    QPointF position() const;
    Odg::Units units() const;

signals:
    void positionChanged(const QPointF& position);

private slots:
    void sendPositionChanged();
};

//======================================================================================================================

class SizeWidget : public QWidget
{
    Q_OBJECT

private:
    LengthEdit* mWidthEdit;
    LengthEdit* mHeightEdit;

public:
    SizeWidget(const QSizeF& size = QSizeF(0, 0), Odg::Units units = Odg::UnitsMillimeters,
               bool sizeMustBeNonNegative = false);

    void setSize(const QSizeF& size);
    void setUnits(Odg::Units units);
    QSizeF size() const;
    Odg::Units units() const;

signals:
    void sizeChanged(const QSizeF& size);

private slots:
    void sendSizeChanged();
};

//======================================================================================================================

class ColorSelectWidget;
class QWidgetAction;

class ColorWidget : public QPushButton
{
    Q_OBJECT

private:
    QColor mColor;

    QMenu* mMenu;
    ColorSelectWidget* mColorSelectWidget;
    QWidgetAction* mColorSelectWidgetAction;
    QPushButton* mMoreColorsButton;
    QWidgetAction* mMoreColorsAction;

public:
    ColorWidget(const QColor& color = QColor());

public slots:
    void setColor(const QColor& color);
public:
    QColor color() const;

signals:
    void colorChanged(const QColor& color);

private slots:
    void runColorDialog();
};

//======================================================================================================================

class ColorSelectWidget : public QWidget
{
    Q_OBJECT

public:
    static const QList<QColor> sStandardColors;
    static QList<QColor> sCustomColors;

private:
    QRect mStandardColorsLabelRect;
    QList<QRect> mStandardColorsRects;

    QRect mCustomColorsLabelRect;
    QList<QRect> mCustomColorsRects;

    QColor mHoverColor;
    QRect mHoverRect;

public:
    ColorSelectWidget();

public slots:
    void selectColor(const QColor& color);
public:
    QColor selectedColor() const;

signals:
    void colorSelected(const QColor& color);

private:
    void paintEvent(QPaintEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

public:
    static QPixmap createTransparentPixmap();
};

#endif
