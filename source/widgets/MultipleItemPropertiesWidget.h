// File: MultipleItemPropertiesWidget.h
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

#ifndef MULTIPLEITEMPROPERTIESWIDGET_H
#define MULTIPLEITEMPROPERTIESWIDGET_H

#include <QWidget>
#include "HelperWidgets.h"
#include "OdgGlobal.h"

class QCheckBox;
class QComboBox;
class QFontComboBox;
class QFormLayout;
class QGroupBox;
class QToolButton;
class ColorWidget;
class LengthEdit;
class OdgItem;
class SizeWidget;

class MultipleItemPropertiesWidget : public QWidget
{
    Q_OBJECT

private:
    QList<OdgItem*> mItems;

    QGroupBox* mRectGroup;
    QFormLayout* mRectLayout;
    QCheckBox* mRectCornerRadiusCheck;
    LengthEdit* mRectCornerRadiusEdit;

    QGroupBox* mPenBrushGroup;
    QFormLayout* mPenBrushLayout;
    QCheckBox* mPenStyleCheck;
    QCheckBox* mPenWidthCheck;
    QCheckBox* mPenColorCheck;
    QCheckBox* mBrushColorCheck;
    QComboBox* mPenStyleCombo;
    LengthEdit* mPenWidthEdit;
    ColorWidget* mPenColorWidget;
    ColorWidget* mBrushColorWidget;

    QGroupBox* mMarkerGroup;
    QFormLayout* mMarkerLayout;
    QCheckBox* mStartMarkerStyleCheck;
    QCheckBox* mStartMarkerSizeCheck;
    QCheckBox* mEndMarkerStyleCheck;
    QCheckBox* mEndMarkerSizeCheck;
    QComboBox* mStartMarkerStyleCombo;
    LengthEdit* mStartMarkerSizeEdit;
    QComboBox* mEndMarkerStyleCombo;
    LengthEdit* mEndMarkerSizeEdit;

    QGroupBox* mTextGroup;
    QFormLayout* mTextLayout;
    QCheckBox* mFontFamilyCheck;
    QCheckBox* mFontSizeCheck;
    QCheckBox* mFontStyleCheck;
    QCheckBox* mTextAlignmentCheck;
    QCheckBox* mTextPaddingCheck;
    QCheckBox* mTextColorCheck;
    QFontComboBox* mFontFamilyCombo;
    LengthEdit* mFontSizeEdit;
    QWidget* mFontStyleWidget;
    QToolButton* mFontBoldButton;
    QToolButton* mFontItalicButton;
    QToolButton* mFontUnderlineButton;
    QToolButton* mFontStrikeOutButton;
    QWidget* mTextAlignmentWidget;
    QToolButton* mTextAlignmentLeftButton;
    QToolButton* mTextAlignmentHCenterButton;
    QToolButton* mTextAlignmentRightButton;
    QToolButton* mTextAlignmentTopButton;
    QToolButton* mTextAlignmentVCenterButton;
    QToolButton* mTextAlignmentBottomButton;
    SizeWidget* mTextPaddingWidget;
    ColorWidget* mTextColorWidget;

public:
    MultipleItemPropertiesWidget();

private:
    QGroupBox* createRectGroup(int labelWidth);
    QGroupBox* createPenBrushGroup(int labelWidth);
    QGroupBox* createMarkerGroup(int labelWidth);
    QGroupBox* createTextGroup(int labelWidth);

    void createFontStyleWidget();
    void createTextAlignmentWidget();

public slots:
    void setItems(const QList<OdgItem*>& items);
    void setUnits(Odg::Units units);
    void setUnits(int units);

signals:
    void itemsMovedDelta(const QPointF& delta);
    void itemsPropertyChanged(const QString& name, const QVariant& value);

private slots:
    void handleCornerRadiusCheckClicked(bool checked);
    void handleCornerRadiusChange(double length);

    void handlePenStyleCheckClicked(bool checked);
    void handlePenWidthCheckClicked(bool checked);
    void handlePenColorCheckClicked(bool checked);
    void handleBrushColorCheckClicked(bool checked);
    void handlePenStyleChange(int index);
    void handlePenWidthChange(double length);
    void handlePenColorChange(const QColor& color);
    void handleBrushColorChange(const QColor& color);

    void handleStartMarkerStyleCheckClicked(bool checked);
    void handleStartMarkerSizeCheckClicked(bool checked);
    void handleEndMarkerStyleCheckClicked(bool checked);
    void handleEndMarkerSizeCheckClicked(bool checked);
    void handleStartMarkerStyleChange(int index);
    void handleStartMarkerSizeChange(double length);
    void handleEndMarkerStyleChange(int index);
    void handleEndMarkerSizeChange(double length);

    void handleFontFamilyCheckClicked(bool checked);
    void handleFontSizeCheckClicked(bool checked);
    void handleFontStyleCheckClicked(bool checked);
    void handleTextAlignmentCheckClicked(bool checked);
    void handleTextPaddingCheckClicked(bool checked);
    void handleTextColorCheckClicked(bool checked);
    void handleFontFamilyChange(int index);
    void handleFontSizeChange(double length);
    void handleFontStyleChange();
    void handleTextAlignmentChange();
    void handleTextPaddingChange(const QSizeF& size);
    void handleTextColorChange(const QColor& color);
};

#endif
