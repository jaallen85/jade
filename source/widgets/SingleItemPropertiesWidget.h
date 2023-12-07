// File: SingleItemPropertiesWidget.h
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

#ifndef SINGLEITEMPROPERTIESWIDGET_H
#define SINGLEITEMPROPERTIESWIDGET_H

#include <QWidget>
#include "OdgGlobal.h"

class QComboBox;
class QFontComboBox;
class QFormLayout;
class QGroupBox;
class QPlainTextEdit;
class QToolButton;
class ColorWidget;
class LengthEdit;
class OdgControlPoint;
class OdgItem;
class PositionWidget;
class SizeWidget;

class SingleItemPropertiesWidget : public QWidget
{
    Q_OBJECT

private:
    OdgItem* mItem;

    QGroupBox* mPositionAndSizeGroup;
    QFormLayout* mPositionAndSizeLayout;
    PositionWidget* mPositionWidget;
    SizeWidget* mSizeWidget;

    QGroupBox* mLineGroup;
    QFormLayout* mLineLayout;
    PositionWidget* mLineP1Widget;
    PositionWidget* mLineP2Widget;

    QGroupBox* mCurveGroup;
    QFormLayout* mCurveLayout;
    PositionWidget* mCurveP1Widget;
    PositionWidget* mCurveCP1Widget;
    PositionWidget* mCurveCP2Widget;
    PositionWidget* mCurveP2Widget;

    QGroupBox* mRectGroup;
    QFormLayout* mRectLayout;
    PositionWidget* mRectPositionWidget;
    SizeWidget* mRectSizeWidget;
    LengthEdit* mRectCornerRadiusEdit;

    QGroupBox* mEllipseGroup;
    QFormLayout* mEllipseLayout;
    PositionWidget* mEllipsePositionWidget;
    SizeWidget* mEllipseSizeWidget;

    QGroupBox* mPolylineGroup;
    QFormLayout* mPolylineLayout;
    QList<PositionWidget*> mPolylineWidgets;

    QGroupBox* mPolygonGroup;
    QFormLayout* mPolygonLayout;
    QList<PositionWidget*> mPolygonWidgets;

    QGroupBox* mPenBrushGroup;
    QFormLayout* mPenBrushLayout;
    QComboBox* mPenStyleCombo;
    LengthEdit* mPenWidthEdit;
    ColorWidget* mPenColorWidget;
    ColorWidget* mBrushColorWidget;

    QGroupBox* mMarkerGroup;
    QFormLayout* mMarkerLayout;
    QComboBox* mStartMarkerStyleCombo;
    LengthEdit* mStartMarkerSizeEdit;
    QComboBox* mEndMarkerStyleCombo;
    LengthEdit* mEndMarkerSizeEdit;

    QGroupBox* mTextGroup;
    QFormLayout* mTextLayout;
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
    QPlainTextEdit* mCaptionEdit;

public:
    SingleItemPropertiesWidget();

private:
    QGroupBox* createPositionAndSizeGroup(int labelWidth);
    QGroupBox* createLineGroup(int labelWidth);
    QGroupBox* createCurveGroup(int labelWidth);
    QGroupBox* createRectGroup(int labelWidth);
    QGroupBox* createEllipseGroup(int labelWidth);
    QGroupBox* createPolylineGroup(int labelWidth);
    QGroupBox* createPolygonGroup(int labelWidth);
    QGroupBox* createPenBrushGroup(int labelWidth);
    QGroupBox* createMarkerGroup(int labelWidth);
    QGroupBox* createTextGroup(int labelWidth);

    void createFontStyleWidget();
    void createTextAlignmentWidget();

public slots:
    void setItem(OdgItem* item);
    void setUnits(Odg::Units units);
    void setUnits(int units);

signals:
    void itemMoved(const QPointF& position);
    void itemResized(OdgControlPoint* point, const QPointF& position);
    void itemResized2(OdgControlPoint* point1, const QPointF& p1, OdgControlPoint* point2, const QPointF& p2);
    void itemPropertyChanged(const QString& name, const QVariant& value);

private slots:
    void handlePositionChange(const QPointF& position);
    void handleSizeChange(const QSizeF& size);

    void handleLineP1Change(const QPointF& position);
    void handleLineP2Change(const QPointF& position);

    void handleCurveP1Change(const QPointF& position);
    void handleCurveCP1Change(const QPointF& position);
    void handleCurveCP2Change(const QPointF& position);
    void handleCurveP2Change(const QPointF& position);

    void handleRectPositionChange(const QPointF& position);
    void handleRectSizeChange(const QSizeF& size);
    void handleCornerRadiusChange(double length);

    void handleEllipsePositionChange(const QPointF& position);
    void handleEllipseSizeChange(const QSizeF& size);

    void handlePenStyleChange(int index);
    void handlePenWidthChange(double length);
    void handlePenColorChange(const QColor& color);
    void handleBrushColorChange(const QColor& color);

    void handleStartMarkerStyleChange(int index);
    void handleStartMarkerSizeChange(double length);
    void handleEndMarkerStyleChange(int index);
    void handleEndMarkerSizeChange(double length);

    void handleFontFamilyChange(int index);
    void handleFontSizeChange(double length);
    void handleFontStyleChange();
    void handleTextAlignmentChange();
    void handleTextPaddingChange(const QSizeF& size);
    void handleTextColorChange(const QColor& color);
    void handleCaptionChanged();
};

#endif
