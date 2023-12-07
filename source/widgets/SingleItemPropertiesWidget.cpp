// File: SingleItemPropertiesWidget.cpp
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

#include "SingleItemPropertiesWidget.h"
#include "HelperWidgets.h"
#include <QComboBox>
#include <QFontComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QToolButton>
#include <QVBoxLayout>

SingleItemPropertiesWidget::SingleItemPropertiesWidget() : QWidget(), mItem(nullptr),
    mPositionAndSizeGroup(nullptr), mPositionAndSizeLayout(nullptr), mPositionWidget(nullptr), mSizeWidget(nullptr),
    mLineGroup(nullptr), mLineLayout(nullptr), mLineP1Widget(nullptr), mLineP2Widget(nullptr),
    mCurveGroup(nullptr), mCurveLayout(nullptr), mCurveP1Widget(nullptr), mCurveCP1Widget(nullptr),
    mCurveCP2Widget(nullptr), mCurveP2Widget(nullptr),
    mRectGroup(nullptr), mRectLayout(nullptr), mRectPositionWidget(nullptr), mRectSizeWidget(nullptr),
    mRectCornerRadiusEdit(nullptr),
    mEllipseGroup(nullptr), mEllipseLayout(nullptr), mEllipsePositionWidget(nullptr), mEllipseSizeWidget(nullptr),
    mPolylineGroup(nullptr), mPolylineLayout(nullptr), mPolylineWidgets(),
    mPolygonGroup(nullptr), mPolygonLayout(nullptr), mPolygonWidgets(),
    mPenBrushGroup(nullptr), mPenBrushLayout(nullptr), mPenStyleCombo(nullptr), mPenWidthEdit(nullptr),
    mPenColorWidget(nullptr), mBrushColorWidget(nullptr),
    mMarkerGroup(nullptr), mMarkerLayout(nullptr), mStartMarkerStyleCombo(nullptr), mStartMarkerSizeEdit(nullptr),
    mEndMarkerStyleCombo(nullptr), mEndMarkerSizeEdit(nullptr),
    mTextGroup(nullptr), mTextLayout(nullptr), mFontFamilyCombo(nullptr), mFontSizeEdit(nullptr),
    mFontStyleWidget(nullptr), mFontBoldButton(nullptr), mFontItalicButton(nullptr), mFontUnderlineButton(nullptr),
    mFontStrikeOutButton(nullptr), mTextAlignmentWidget(nullptr), mTextAlignmentLeftButton(nullptr),
    mTextAlignmentHCenterButton(nullptr), mTextAlignmentRightButton(nullptr), mTextAlignmentTopButton(nullptr),
    mTextAlignmentVCenterButton(nullptr), mTextAlignmentBottomButton(nullptr), mTextPaddingWidget(nullptr),
    mTextColorWidget(nullptr), mCaptionEdit(nullptr)
{
    const int labelWidth = QFontMetrics(QWidget::font()).boundingRect("Bottom-Right Margin:").width() + 8;

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(createPositionAndSizeGroup(labelWidth));
    layout->addWidget(createLineGroup(labelWidth));
    layout->addWidget(createCurveGroup(labelWidth));
    layout->addWidget(createRectGroup(labelWidth));
    layout->addWidget(createEllipseGroup(labelWidth));
    layout->addWidget(createPolylineGroup(labelWidth));
    layout->addWidget(createPolygonGroup(labelWidth));
    layout->addWidget(createPenBrushGroup(labelWidth));
    layout->addWidget(createMarkerGroup(labelWidth));
    layout->addWidget(createTextGroup(labelWidth));
    layout->addWidget(new QWidget(), 100);
    setLayout(layout);
}

//======================================================================================================================

QGroupBox* SingleItemPropertiesWidget::createPositionAndSizeGroup(int labelWidth)
{
    mPositionWidget = new PositionWidget();
    connect(mPositionWidget, SIGNAL(positionChanged(QPointF)), this, SLOT(handlePositionChange(QPointF)));

    mSizeWidget = new SizeWidget();
    connect(mSizeWidget, SIGNAL(sizeChanged(QSizeF)), this, SLOT(handleSizeChange(QSizeF)));

    mPositionAndSizeGroup = new QGroupBox("Position / Size");
    mPositionAndSizeLayout = new QFormLayout();
    mPositionAndSizeLayout->addRow("Position:", mPositionWidget);
    mPositionAndSizeLayout->addRow("Size:", mSizeWidget);
    mPositionAndSizeLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    mPositionAndSizeLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mPositionAndSizeLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    mPositionAndSizeLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth);
    mPositionAndSizeGroup->setLayout(mPositionAndSizeLayout);

    return mPositionAndSizeGroup;
}

QGroupBox* SingleItemPropertiesWidget::createLineGroup(int labelWidth)
{
    mLineP1Widget = new PositionWidget();
    connect(mLineP1Widget, SIGNAL(positionChanged(QPointF)), this, SLOT(handleLineP1Change(QPointF)));

    mLineP2Widget = new PositionWidget();
    connect(mLineP2Widget, SIGNAL(positionChanged(QPointF)), this, SLOT(handleLineP2Change(QPointF)));

    mLineGroup = new QGroupBox("Line");
    mLineLayout = new QFormLayout();
    mLineLayout->addRow("Start Point:", mLineP1Widget);
    mLineLayout->addRow("End Point:", mLineP2Widget);
    mLineLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    mLineLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mLineLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    mLineLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth);
    mLineGroup->setLayout(mLineLayout);

    return mLineGroup;
}

QGroupBox* SingleItemPropertiesWidget::createCurveGroup(int labelWidth)
{
    mCurveP1Widget = new PositionWidget();
    connect(mCurveP1Widget, SIGNAL(positionChanged(QPointF)), this, SLOT(handleCurveP1Change(QPointF)));

    mCurveCP1Widget = new PositionWidget();
    connect(mCurveCP1Widget, SIGNAL(positionChanged(QPointF)), this, SLOT(handleCurveCP1Change(QPointF)));

    mCurveCP2Widget = new PositionWidget();
    connect(mCurveCP2Widget, SIGNAL(positionChanged(QPointF)), this, SLOT(handleCurveCP2Change(QPointF)));

    mCurveP2Widget = new PositionWidget();
    connect(mCurveP2Widget, SIGNAL(positionChanged(QPointF)), this, SLOT(handleCurveP2Change(QPointF)));

    mCurveGroup = new QGroupBox("Curve");
    mCurveLayout = new QFormLayout();
    mCurveLayout->addRow("Start Point:", mCurveP1Widget);
    mCurveLayout->addRow("Start Control Point:", mCurveCP1Widget);
    mCurveLayout->addRow("End Control Point:", mCurveCP2Widget);
    mCurveLayout->addRow("End Point:", mCurveP2Widget);
    mCurveLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    mCurveLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mCurveLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    mCurveLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth);
    mCurveGroup->setLayout(mCurveLayout);

    return mCurveGroup;
}

QGroupBox* SingleItemPropertiesWidget::createRectGroup(int labelWidth)
{
    mRectPositionWidget = new PositionWidget();
    connect(mRectPositionWidget, SIGNAL(positionChanged(QPointF)), this, SLOT(handleRectPositionChange(QPointF)));

    mRectSizeWidget = new SizeWidget();
    connect(mRectSizeWidget, SIGNAL(sizeChanged(QSizeF)), this, SLOT(handleRectSizeChange(QSizeF)));

    mRectCornerRadiusEdit = new LengthEdit();
    connect(mRectCornerRadiusEdit, SIGNAL(lengthChanged(double)), this, SLOT(handleCornerRadiusChange(double)));

    mRectGroup = new QGroupBox("Rect");
    mRectLayout = new QFormLayout();
    mRectLayout->addRow("Position:", mRectPositionWidget);
    mRectLayout->addRow("Size:", mRectSizeWidget);
    mRectLayout->addRow("Corner Radius:", mRectCornerRadiusEdit);
    mRectLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    mRectLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mRectLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    mRectLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth);
    mRectGroup->setLayout(mRectLayout);

    return mRectGroup;
}

QGroupBox* SingleItemPropertiesWidget::createEllipseGroup(int labelWidth)
{
    mEllipsePositionWidget = new PositionWidget();
    connect(mEllipsePositionWidget, SIGNAL(positionChanged(QPointF)), this, SLOT(handleEllipsePositionChange(QPointF)));

    mEllipseSizeWidget = new SizeWidget();
    connect(mEllipseSizeWidget, SIGNAL(sizeChanged(QSizeF)), this, SLOT(handleEllipseSizeChange(QSizeF)));

    mEllipseGroup = new QGroupBox("Ellipse");
    mEllipseLayout = new QFormLayout();
    mEllipseLayout->addRow("Position:", mEllipsePositionWidget);
    mEllipseLayout->addRow("Size:", mEllipseSizeWidget);
    mEllipseLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    mEllipseLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mEllipseLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    mEllipseLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth);
    mEllipseGroup->setLayout(mEllipseLayout);

    return mEllipseGroup;
}

QGroupBox* SingleItemPropertiesWidget::createPolylineGroup(int labelWidth)
{
    mPolylineGroup = new QGroupBox("Polyline");
    mPolylineLayout = new QFormLayout();
    mPolylineLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    mPolylineLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mPolylineLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    mPolylineGroup->setLayout(mPolylineLayout);

    return mPolylineGroup;
}

QGroupBox* SingleItemPropertiesWidget::createPolygonGroup(int labelWidth)
{
    mPolygonGroup = new QGroupBox("Polygon");
    mPolygonLayout = new QFormLayout();
    mPolygonLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    mPolygonLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mPolygonLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    mPolygonGroup->setLayout(mPolygonLayout);

    return mPolygonGroup;
}

QGroupBox* SingleItemPropertiesWidget::createPenBrushGroup(int labelWidth)
{
    mPenStyleCombo = new QComboBox();
    mPenStyleCombo->addItems(QStringList({"None", "Solid", "Dashed", "Dotted", "Dash-Dotted", "Dash-Dot-Dotted"}));
    connect(mPenStyleCombo, SIGNAL(activated(int)), this, SLOT(handlePenStyleChange(int)));

    mPenWidthEdit = new LengthEdit();
    connect(mPenWidthEdit, SIGNAL(lengthChanged(double)), this, SLOT(handlePenWidthChange(double)));

    mPenColorWidget = new ColorWidget();
    connect(mPenColorWidget, SIGNAL(colorChanged(QColor)), this, SLOT(handlePenColorChange(QColor)));

    mBrushColorWidget = new ColorWidget();
    connect(mBrushColorWidget, SIGNAL(colorChanged(QColor)), this, SLOT(handleBrushColorChange(QColor)));

    mPenBrushGroup = new QGroupBox("Pen / Brush");
    mPenBrushLayout = new QFormLayout();
    mPenBrushLayout->addRow("Pen Style:", mPenStyleCombo);
    mPenBrushLayout->addRow("Pen Width:", mPenWidthEdit);
    mPenBrushLayout->addRow("Pen Color:", mPenColorWidget);
    mPenBrushLayout->addRow("Brush Color:", mBrushColorWidget);
    mPenBrushLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    mPenBrushLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mPenBrushLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    mPenBrushLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth);
    mPenBrushGroup->setLayout(mPenBrushLayout);

    return mPenBrushGroup;
}

QGroupBox* SingleItemPropertiesWidget::createMarkerGroup(int labelWidth)
{
    mStartMarkerStyleCombo = new QComboBox();
    mStartMarkerStyleCombo->addItem(QIcon(":/icons/marker/marker-none.png"), "None");
    mStartMarkerStyleCombo->addItem(QIcon(":/icons/marker/marker-triangle-start.png"), "Triangle");
    mStartMarkerStyleCombo->addItem(QIcon(":/icons/marker/marker-circle-start.png"), "Circle");
    connect(mStartMarkerStyleCombo, SIGNAL(activated(int)), this, SLOT(handleStartMarkerStyleChange(int)));

    mStartMarkerSizeEdit = new LengthEdit();
    connect(mStartMarkerSizeEdit, SIGNAL(lengthChanged(double)), this, SLOT(handleStartMarkerSizeChange(double)));

    mEndMarkerStyleCombo = new QComboBox();
    mEndMarkerStyleCombo->addItem(QIcon(":/icons/marker/marker-none.png"), "None");
    mEndMarkerStyleCombo->addItem(QIcon(":/icons/marker/marker-triangle-end.png"), "Triangle");
    mEndMarkerStyleCombo->addItem(QIcon(":/icons/marker/marker-circle-end.png"), "Circle");
    connect(mEndMarkerStyleCombo, SIGNAL(activated(int)), this, SLOT(handleEndMarkerStyleChange(int)));

    mEndMarkerSizeEdit = new LengthEdit();
    connect(mEndMarkerSizeEdit, SIGNAL(lengthChanged(double)), this, SLOT(handleEndMarkerSizeChange(double)));

    mMarkerGroup = new QGroupBox("Marker");
    mMarkerLayout = new QFormLayout();
    mMarkerLayout->addRow("Start Marker Style:", mStartMarkerStyleCombo);
    mMarkerLayout->addRow("Start Marker Size:", mStartMarkerSizeEdit);
    mMarkerLayout->addRow("End Marker Style:", mEndMarkerStyleCombo);
    mMarkerLayout->addRow("End Marker Size:", mEndMarkerSizeEdit);
    mMarkerLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    mMarkerLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mMarkerLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    mMarkerLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth);
    mMarkerGroup->setLayout(mMarkerLayout);

    return mMarkerGroup;
}

QGroupBox* SingleItemPropertiesWidget::createTextGroup(int labelWidth)
{
    mFontFamilyCombo = new QFontComboBox();
    mFontFamilyCombo->setMaximumWidth(162);
    connect(mFontFamilyCombo, SIGNAL(activated(int)), this, SLOT(handleFontFamilyChange(int)));

    mFontSizeEdit = new LengthEdit();
    connect(mFontSizeEdit, SIGNAL(lengthChanged(double)), this, SLOT(handleFontSizeChange(double)));

    createFontStyleWidget();
    connect(mFontBoldButton, SIGNAL(clicked(bool)), this, SLOT(handleFontStyleChange()));
    connect(mFontItalicButton, SIGNAL(clicked(bool)), this, SLOT(handleFontStyleChange()));
    connect(mFontUnderlineButton, SIGNAL(clicked(bool)), this, SLOT(handleFontStyleChange()));
    connect(mFontStrikeOutButton, SIGNAL(clicked(bool)), this, SLOT(handleFontStyleChange()));

    createTextAlignmentWidget();
    connect(mTextAlignmentLeftButton, SIGNAL(clicked(bool)), this, SLOT(handleTextAlignmentChange()));
    connect(mTextAlignmentHCenterButton, SIGNAL(clicked(bool)), this, SLOT(handleTextAlignmentChange()));
    connect(mTextAlignmentRightButton, SIGNAL(clicked(bool)), this, SLOT(handleTextAlignmentChange()));
    connect(mTextAlignmentTopButton, SIGNAL(clicked(bool)), this, SLOT(handleTextAlignmentChange()));
    connect(mTextAlignmentVCenterButton, SIGNAL(clicked(bool)), this, SLOT(handleTextAlignmentChange()));
    connect(mTextAlignmentBottomButton, SIGNAL(clicked(bool)), this, SLOT(handleTextAlignmentChange()));

    mTextPaddingWidget = new SizeWidget();
    connect(mTextPaddingWidget, SIGNAL(sizeChanged(QSizeF)), this, SLOT(handleTextPaddingChange(QSizeF)));

    mTextColorWidget = new ColorWidget();
    connect(mTextColorWidget, SIGNAL(colorChanged(QColor)), this, SLOT(handleTextColorChange(QColor)));

    mCaptionEdit = new QPlainTextEdit();
    connect(mCaptionEdit, SIGNAL(textChanged()), this, SLOT(handleCaptionChanged()));

    mTextGroup = new QGroupBox("Text");
    mTextLayout = new QFormLayout();
    mTextLayout->addRow("Font:", mFontFamilyCombo);
    mTextLayout->addRow("Font Size:", mFontSizeEdit);
    mTextLayout->addRow("Font Style:", mFontStyleWidget);
    mTextLayout->addRow("Text Alignment:", mTextAlignmentWidget);
    mTextLayout->addRow("Text Padding:", mTextPaddingWidget);
    mTextLayout->addRow("Text Color:", mTextColorWidget);
    mTextLayout->addRow("Text:", mCaptionEdit);
    mTextLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    mTextLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mTextLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    mTextLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth);
    mTextGroup->setLayout(mTextLayout);

    return mTextGroup;
}

//======================================================================================================================

void SingleItemPropertiesWidget::createFontStyleWidget()
{
    mFontBoldButton = new QToolButton();
    mFontBoldButton->setIcon(QIcon(":/icons/format-text-bold.png"));
    mFontBoldButton->setToolTip("Bold");
    mFontBoldButton->setCheckable(true);

    mFontItalicButton = new QToolButton();
    mFontItalicButton->setIcon(QIcon(":/icons/format-text-italic.png"));
    mFontItalicButton->setToolTip("Italic");
    mFontItalicButton->setCheckable(true);

    mFontUnderlineButton = new QToolButton();
    mFontUnderlineButton->setIcon(QIcon(":/icons/format-text-underline.png"));
    mFontUnderlineButton->setToolTip("Underline");
    mFontUnderlineButton->setCheckable(true);

    mFontStrikeOutButton = new QToolButton();
    mFontStrikeOutButton->setIcon(QIcon(":/icons/format-text-strikethrough.png"));
    mFontStrikeOutButton->setToolTip("Strike-Through");
    mFontStrikeOutButton->setCheckable(true);

    mFontStyleWidget = new QWidget();
    QHBoxLayout* fontStyleLayout = new QHBoxLayout();
    fontStyleLayout->addWidget(mFontBoldButton);
    fontStyleLayout->addWidget(mFontItalicButton);
    fontStyleLayout->addWidget(mFontUnderlineButton);
    fontStyleLayout->addWidget(mFontStrikeOutButton);
    fontStyleLayout->addWidget(new QWidget(), 100);
    fontStyleLayout->setSpacing(2);
    fontStyleLayout->setContentsMargins(0, 0, 0, 0);
    mFontStyleWidget->setLayout(fontStyleLayout);
}

void SingleItemPropertiesWidget::createTextAlignmentWidget()
{
    mTextAlignmentLeftButton = new QToolButton();
    mTextAlignmentLeftButton->setIcon(QIcon(":/icons/align-horizontal-left.png"));
    mTextAlignmentLeftButton->setToolTip("Align Left");
    mTextAlignmentLeftButton->setCheckable(true);
    mTextAlignmentLeftButton->setAutoExclusive(true);

    mTextAlignmentHCenterButton = new QToolButton();
    mTextAlignmentHCenterButton->setIcon(QIcon(":/icons/align-horizontal-center.png"));
    mTextAlignmentHCenterButton->setToolTip("Align Center");
    mTextAlignmentHCenterButton->setCheckable(true);
    mTextAlignmentHCenterButton->setAutoExclusive(true);

    mTextAlignmentRightButton = new QToolButton();
    mTextAlignmentRightButton->setIcon(QIcon(":/icons/align-horizontal-right.png"));
    mTextAlignmentRightButton->setToolTip("Align Right");
    mTextAlignmentRightButton->setCheckable(true);
    mTextAlignmentRightButton->setAutoExclusive(true);

    mTextAlignmentTopButton = new QToolButton();
    mTextAlignmentTopButton->setIcon(QIcon(":/icons/align-vertical-top.png"));
    mTextAlignmentTopButton->setToolTip("Align Top");
    mTextAlignmentTopButton->setCheckable(true);
    mTextAlignmentTopButton->setAutoExclusive(true);

    mTextAlignmentVCenterButton = new QToolButton();
    mTextAlignmentVCenterButton->setIcon(QIcon(":/icons/align-vertical-center.png"));
    mTextAlignmentVCenterButton->setToolTip("Align Center");
    mTextAlignmentVCenterButton->setCheckable(true);
    mTextAlignmentVCenterButton->setAutoExclusive(true);

    mTextAlignmentBottomButton = new QToolButton();
    mTextAlignmentBottomButton->setIcon(QIcon(":/icons/align-vertical-bottom.png"));
    mTextAlignmentBottomButton->setToolTip("Align Bottom");
    mTextAlignmentBottomButton->setCheckable(true);
    mTextAlignmentBottomButton->setAutoExclusive(true);

    QWidget* textAlignmentHorizontalWidget = new QWidget();
    QHBoxLayout* textAlignmentHorizontalLayout = new QHBoxLayout();
    textAlignmentHorizontalLayout->addWidget(mTextAlignmentLeftButton);
    textAlignmentHorizontalLayout->addWidget(mTextAlignmentHCenterButton);
    textAlignmentHorizontalLayout->addWidget(mTextAlignmentRightButton);
    textAlignmentHorizontalLayout->setSpacing(2);
    textAlignmentHorizontalLayout->setContentsMargins(0, 0, 0, 0);
    textAlignmentHorizontalWidget->setLayout(textAlignmentHorizontalLayout);

    QFrame* textAlignmentSeparator = new QFrame();
    textAlignmentSeparator->setFrameStyle(QFrame::VLine | QFrame::Raised);

    QWidget* textAlignmentVerticalWidget = new QWidget();
    QHBoxLayout* textAlignmentVerticalLayout = new QHBoxLayout();
    textAlignmentVerticalLayout->addWidget(mTextAlignmentTopButton);
    textAlignmentVerticalLayout->addWidget(mTextAlignmentVCenterButton);
    textAlignmentVerticalLayout->addWidget(mTextAlignmentBottomButton);
    textAlignmentVerticalLayout->setSpacing(2);
    textAlignmentVerticalLayout->setContentsMargins(0, 0, 0, 0);
    textAlignmentVerticalWidget->setLayout(textAlignmentVerticalLayout);

    mTextAlignmentWidget = new QWidget();
    QHBoxLayout* textAlignmentLayout = new QHBoxLayout();
    textAlignmentLayout->addWidget(textAlignmentHorizontalWidget);
    textAlignmentLayout->addWidget(textAlignmentSeparator);
    textAlignmentLayout->addWidget(textAlignmentVerticalWidget);
    textAlignmentLayout->addWidget(new QWidget(), 100);
    textAlignmentLayout->setSpacing(2);
    textAlignmentLayout->setContentsMargins(0, 0, 0, 0);
    mTextAlignmentWidget->setLayout(textAlignmentLayout);
}

//======================================================================================================================

void SingleItemPropertiesWidget::setItem(OdgItem* item)
{

}

void SingleItemPropertiesWidget::setUnits(Odg::Units units)
{
    mPositionWidget->setUnits(units);
    mSizeWidget->setUnits(units);

    mLineP1Widget->setUnits(units);
    mLineP2Widget->setUnits(units);

    mCurveP1Widget->setUnits(units);
    mCurveCP1Widget->setUnits(units);
    mCurveCP2Widget->setUnits(units);
    mCurveP2Widget->setUnits(units);

    mRectPositionWidget->setUnits(units);
    mRectSizeWidget->setUnits(units);
    mRectCornerRadiusEdit->setUnits(units);

    mEllipsePositionWidget->setUnits(units);
    mEllipseSizeWidget->setUnits(units);

    for(auto& positionWidget : qAsConst(mPolylineWidgets))
        positionWidget->setUnits(units);

    for(auto& positionWidget : qAsConst(mPolygonWidgets))
        positionWidget->setUnits(units);

    mPenWidthEdit->setUnits(units);

    mStartMarkerSizeEdit->setUnits(units);
    mEndMarkerSizeEdit->setUnits(units);

    mFontSizeEdit->setUnits(units);
    mTextPaddingWidget->setUnits(units);
}

void SingleItemPropertiesWidget::setUnits(int units)
{
    setUnits(static_cast<Odg::Units>(units));
}

//======================================================================================================================

void SingleItemPropertiesWidget::handlePositionChange(const QPointF& position)
{

}

void SingleItemPropertiesWidget::handleSizeChange(const QSizeF& size)
{

}

//======================================================================================================================

void SingleItemPropertiesWidget::handleLineP1Change(const QPointF& position)
{

}

void SingleItemPropertiesWidget::handleLineP2Change(const QPointF& position)
{

}

//======================================================================================================================

void SingleItemPropertiesWidget::handleCurveP1Change(const QPointF& position)
{

}

void SingleItemPropertiesWidget::handleCurveCP1Change(const QPointF& position)
{

}

void SingleItemPropertiesWidget::handleCurveCP2Change(const QPointF& position)
{

}

void SingleItemPropertiesWidget::handleCurveP2Change(const QPointF& position)
{

}

//======================================================================================================================

void SingleItemPropertiesWidget::handleRectPositionChange(const QPointF& position)
{

}

void SingleItemPropertiesWidget::handleRectSizeChange(const QSizeF& size)
{

}

void SingleItemPropertiesWidget::handleCornerRadiusChange(double length)
{

}

//======================================================================================================================

void SingleItemPropertiesWidget::handleEllipsePositionChange(const QPointF& position)
{

}

void SingleItemPropertiesWidget::handleEllipseSizeChange(const QSizeF& size)
{

}

//======================================================================================================================

void SingleItemPropertiesWidget::handlePenStyleChange(int index)
{

}

void SingleItemPropertiesWidget::handlePenWidthChange(double length)
{

}

void SingleItemPropertiesWidget::handlePenColorChange(const QColor& color)
{

}

void SingleItemPropertiesWidget::handleBrushColorChange(const QColor& color)
{

}

//======================================================================================================================

void SingleItemPropertiesWidget::handleStartMarkerStyleChange(int index)
{

}

void SingleItemPropertiesWidget::handleStartMarkerSizeChange(double length)
{

}

void SingleItemPropertiesWidget::handleEndMarkerStyleChange(int index)
{

}

void SingleItemPropertiesWidget::handleEndMarkerSizeChange(double length)
{

}

//======================================================================================================================

void SingleItemPropertiesWidget::handleFontFamilyChange(int index)
{

}

void SingleItemPropertiesWidget::handleFontSizeChange(double length)
{

}

void SingleItemPropertiesWidget::handleFontStyleChange()
{

}

void SingleItemPropertiesWidget::handleTextAlignmentChange()
{

}

void SingleItemPropertiesWidget::handleTextPaddingChange(const QSizeF& size)
{

}

void SingleItemPropertiesWidget::handleTextColorChange(const QColor& color)
{

}

void SingleItemPropertiesWidget::handleCaptionChanged()
{

}
