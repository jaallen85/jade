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
#include "OdgCurve.h"
#include "OdgItem.h"
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

    setItem(nullptr);
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
    mFontBoldButton->setIcon(QIcon(":/icons/oxygen/format-text-bold.png"));
    mFontBoldButton->setToolTip("Bold");
    mFontBoldButton->setCheckable(true);

    mFontItalicButton = new QToolButton();
    mFontItalicButton->setIcon(QIcon(":/icons/oxygen/format-text-italic.png"));
    mFontItalicButton->setToolTip("Italic");
    mFontItalicButton->setCheckable(true);

    mFontUnderlineButton = new QToolButton();
    mFontUnderlineButton->setIcon(QIcon(":/icons/oxygen/format-text-underline.png"));
    mFontUnderlineButton->setToolTip("Underline");
    mFontUnderlineButton->setCheckable(true);

    mFontStrikeOutButton = new QToolButton();
    mFontStrikeOutButton->setIcon(QIcon(":/icons/oxygen/format-text-strikethrough.png"));
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
    mTextAlignmentLeftButton->setIcon(QIcon(":/icons/oxygen/align-horizontal-left.png"));
    mTextAlignmentLeftButton->setToolTip("Align Left");
    mTextAlignmentLeftButton->setCheckable(true);
    mTextAlignmentLeftButton->setAutoExclusive(true);

    mTextAlignmentHCenterButton = new QToolButton();
    mTextAlignmentHCenterButton->setIcon(QIcon(":/icons/oxygen/align-horizontal-center.png"));
    mTextAlignmentHCenterButton->setToolTip("Align Center");
    mTextAlignmentHCenterButton->setCheckable(true);
    mTextAlignmentHCenterButton->setAutoExclusive(true);

    mTextAlignmentRightButton = new QToolButton();
    mTextAlignmentRightButton->setIcon(QIcon(":/icons/oxygen/align-horizontal-right.png"));
    mTextAlignmentRightButton->setToolTip("Align Right");
    mTextAlignmentRightButton->setCheckable(true);
    mTextAlignmentRightButton->setAutoExclusive(true);

    mTextAlignmentTopButton = new QToolButton();
    mTextAlignmentTopButton->setIcon(QIcon(":/icons/oxygen/align-vertical-top.png"));
    mTextAlignmentTopButton->setToolTip("Align Top");
    mTextAlignmentTopButton->setCheckable(true);
    mTextAlignmentTopButton->setAutoExclusive(true);

    mTextAlignmentVCenterButton = new QToolButton();
    mTextAlignmentVCenterButton->setIcon(QIcon(":/icons/oxygen/align-vertical-center.png"));
    mTextAlignmentVCenterButton->setToolTip("Align Center");
    mTextAlignmentVCenterButton->setCheckable(true);
    mTextAlignmentVCenterButton->setAutoExclusive(true);

    mTextAlignmentBottomButton = new QToolButton();
    mTextAlignmentBottomButton->setIcon(QIcon(":/icons/oxygen/align-vertical-bottom.png"));
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

void SingleItemPropertiesWidget::setPenStyle(Qt::PenStyle style)
{
    mPenStyleCombo->setCurrentIndex(static_cast<int>(style));
}

void SingleItemPropertiesWidget::setPenWidth(double width)
{
    mPenWidthEdit->setLength(width);
}

void SingleItemPropertiesWidget::setPenColor(const QColor& color)
{
    mPenColorWidget->setColor(color);
}

void SingleItemPropertiesWidget::setBrushColor(const QColor& color)
{
    mBrushColorWidget->setColor(color);
}

Qt::PenStyle SingleItemPropertiesWidget::penStyle() const
{
    return static_cast<Qt::PenStyle>(mPenStyleCombo->currentIndex());
}

double SingleItemPropertiesWidget::penWidth() const
{
    return mPenWidthEdit->length();
}

QColor SingleItemPropertiesWidget::penColor() const
{
    return mPenColorWidget->color();
}

QColor SingleItemPropertiesWidget::brushColor() const
{
    return mBrushColorWidget->color();
}

//======================================================================================================================

void SingleItemPropertiesWidget::setStartMarkerStyle(Odg::MarkerStyle style)
{
    mStartMarkerStyleCombo->setCurrentIndex(static_cast<int>(style));
}

void SingleItemPropertiesWidget::setStartMarkerSize(double size)
{
    mStartMarkerSizeEdit->setLength(size);
}

void SingleItemPropertiesWidget::setEndMarkerStyle(Odg::MarkerStyle style)
{
    mEndMarkerStyleCombo->setCurrentIndex(static_cast<int>(style));
}

void SingleItemPropertiesWidget::setEndMarkerSize(double size)
{
    mEndMarkerSizeEdit->setLength(size);
}

Odg::MarkerStyle SingleItemPropertiesWidget::startMarkerStyle() const
{
    return static_cast<Odg::MarkerStyle>(mStartMarkerStyleCombo->currentIndex());
}

double SingleItemPropertiesWidget::startMarkerSize() const
{
    return mStartMarkerSizeEdit->length();
}

Odg::MarkerStyle SingleItemPropertiesWidget::endMarkerStyle() const
{
    return static_cast<Odg::MarkerStyle>(mEndMarkerStyleCombo->currentIndex());
}

double SingleItemPropertiesWidget::endMarkerSize() const
{
    return mEndMarkerSizeEdit->length();
}

//======================================================================================================================

void SingleItemPropertiesWidget::setFontFamily(const QString& family)
{
    QFont font;
    font.setFamily(family);
    mFontFamilyCombo->setCurrentFont(font);
}

void SingleItemPropertiesWidget::setFontSize(double size)
{
    mFontSizeEdit->setLength(size);
}

void SingleItemPropertiesWidget::setFontStyle(const OdgFontStyle& style)
{
    mFontBoldButton->setChecked(style.bold());
    mFontItalicButton->setChecked(style.italic());
    mFontUnderlineButton->setChecked(style.underline());
    mFontStrikeOutButton->setChecked(style.strikeOut());
}

void SingleItemPropertiesWidget::setTextAlignment(Qt::Alignment alignment)
{
    Qt::Alignment horizontal = (alignment & Qt::AlignHorizontal_Mask);
    if (horizontal & Qt::AlignHCenter) mTextAlignmentHCenterButton->setChecked(true);
    else if (horizontal & Qt::AlignRight) mTextAlignmentRightButton->setChecked(true);
    else mTextAlignmentLeftButton->setChecked(true);

    Qt::Alignment vertical = (alignment & Qt::AlignVertical_Mask);
    if (vertical & Qt::AlignVCenter) mTextAlignmentVCenterButton->setChecked(true);
    else if (vertical & Qt::AlignBottom) mTextAlignmentBottomButton->setChecked(true);
    else mTextAlignmentTopButton->setChecked(true);
}

void SingleItemPropertiesWidget::setTextPadding(const QSizeF& padding)
{
    mTextPaddingWidget->setSize(padding);
}

void SingleItemPropertiesWidget::setTextColor(const QColor& color)
{
    mTextColorWidget->setColor(color);
}

QString SingleItemPropertiesWidget::fontFamily() const
{
    return mFontFamilyCombo->currentFont().family();
}

double SingleItemPropertiesWidget::fontSize() const
{
    return mFontSizeEdit->length();
}

OdgFontStyle SingleItemPropertiesWidget::fontStyle() const
{
    OdgFontStyle style;
    style.setBold(mFontBoldButton->isChecked());
    style.setItalic(mFontItalicButton->isChecked());
    style.setUnderline(mFontUnderlineButton->isChecked());
    style.setStrikeOut(mFontStrikeOutButton->isChecked());
    return style;
}

Qt::Alignment SingleItemPropertiesWidget::textAlignment() const
{
    Qt::Alignment horizontal = Qt::AlignLeft;
    if (mTextAlignmentHCenterButton->isChecked())
        horizontal = Qt::AlignHCenter;
    else if (mTextAlignmentRightButton->isChecked())
        horizontal = Qt::AlignRight;

    Qt::Alignment vertical = Qt::AlignTop;
    if (mTextAlignmentVCenterButton->isChecked())
        vertical = Qt::AlignVCenter;
    else if (mTextAlignmentBottomButton->isChecked())
        vertical = Qt::AlignBottom;

    return (horizontal | vertical);
}

QSizeF SingleItemPropertiesWidget::textPadding() const
{
    return mTextPaddingWidget->size();
}

QColor SingleItemPropertiesWidget::textColor() const
{
    return mTextColorWidget->color();
}

//======================================================================================================================

void SingleItemPropertiesWidget::setItem(OdgItem* item)
{
    mItem = item;

    blockSignals(true);
    updatePositionAndSizeGroup();
    updateLineGroup();
    updateCurveGroup();
    updateRectGroup();
    updateEllipseGroup();
    updatePolygonGroup();
    updatePolylineGroup();
    updatePenBrushGroup();
    updateMarkerGroup();
    updateTextGroup();
    blockSignals(false);
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

void SingleItemPropertiesWidget::updatePositionAndSizeGroup()
{
    if (mItem)
    {
        bool showPosition = false, showSize = false;
        const QPointF position = checkProperty<QPointF>("position", showPosition);
        const QSizeF size = checkProperty<QSizeF>("size", showSize);

        if (showPosition) mPositionWidget->setPosition(position);
        if (showSize) mSizeWidget->setSize(size);

        mPositionAndSizeLayout->setRowVisible(mPositionWidget, showPosition);
        mPositionAndSizeLayout->setRowVisible(mSizeWidget, showSize);
        mPositionAndSizeGroup->setVisible(showPosition || showSize);
    }
    else mPositionAndSizeGroup->setVisible(false);
}

void SingleItemPropertiesWidget::updateLineGroup()
{
    if (mItem)
    {
        bool showLine = false;
        const QLineF line = checkProperty<QLineF>("line", showLine);

        if (showLine)
        {
            mLineP1Widget->setPosition(mItem->mapToScene(line.p1()));
            mLineP2Widget->setPosition(mItem->mapToScene(line.p2()));
        }

        mLineGroup->setVisible(showLine);
    }
    else mLineGroup->setVisible(false);
}

void SingleItemPropertiesWidget::updateCurveGroup()
{
    if (mItem)
    {
        bool showCurve = false;
        const OdgCurve curve = checkProperty<OdgCurve>("curve", showCurve);

        if (showCurve)
        {
            mCurveP1Widget->setPosition(mItem->mapToScene(curve.p1()));
            mCurveCP1Widget->setPosition(mItem->mapToScene(curve.cp1()));
            mCurveCP2Widget->setPosition(mItem->mapToScene(curve.cp2()));
            mCurveP2Widget->setPosition(mItem->mapToScene(curve.p2()));
        }

        mCurveGroup->setVisible(showCurve);
    }
    else mCurveGroup->setVisible(false);
}

void SingleItemPropertiesWidget::updateRectGroup()
{
    if (mItem)
    {
        bool showRect = false, showCornerRadius = false;
        const QRectF rect = checkProperty<QRectF>("rect", showRect);
        const double cornerRadius = checkDoubleProperty("cornerRadius", showCornerRadius);

        if (showRect)
        {
            mRectPositionWidget->setPosition(mItem->mapToScene(rect.center()));

            if (mItem->rotation() % 2 == 1)
                mRectSizeWidget->setSize(QSizeF(rect.height(), rect.width()));
            else
                mRectSizeWidget->setSize(rect.size());
        }
        if (showCornerRadius) mRectCornerRadiusEdit->setLength(cornerRadius);

        mRectLayout->setRowVisible(mRectPositionWidget, showRect);
        mRectLayout->setRowVisible(mRectSizeWidget, showRect);
        mRectLayout->setRowVisible(mRectCornerRadiusEdit, showCornerRadius);
        mRectGroup->setVisible(showRect || showCornerRadius);
    }
    else mRectGroup->setVisible(false);
}

void SingleItemPropertiesWidget::updateEllipseGroup()
{
    if (mItem)
    {
        bool showEllipse = false;
        const QRectF ellipse = checkProperty<QRectF>("ellipse", showEllipse);

        if (showEllipse)
        {
            mEllipsePositionWidget->setPosition(mItem->mapToScene(ellipse.center()));

            if (mItem->rotation() % 2 == 1)
                mEllipseSizeWidget->setSize(QSizeF(ellipse.height(), ellipse.width()));
            else
                mEllipseSizeWidget->setSize(ellipse.size());
        }

        mEllipseGroup->setVisible(showEllipse);
        if (showEllipse) mRectGroup->setVisible(false);
    }
    else mEllipseGroup->setVisible(false);
}

void SingleItemPropertiesWidget::updatePolylineGroup()
{
    if (mItem)
    {
        bool showPolyline = false;
        const QPolygonF polyline = checkProperty<QPolygonF>("polyline", showPolyline);

        if (showPolyline)
            updatePositionWidgets(polyline, mPolylineWidgets, mPolylineLayout, this, SLOT(handlePolylineChange(QPointF)));
        else
            clearPositionWidgets(mPolylineWidgets, mPolylineLayout);

        mPolylineGroup->setVisible(showPolyline);
    }
    else
    {
        clearPositionWidgets(mPolylineWidgets, mPolylineLayout);
        mPolylineGroup->setVisible(false);
    }
}

void SingleItemPropertiesWidget::updatePolygonGroup()
{
    if (mItem)
    {
        bool showPolygon = false;
        const QPolygonF polygon = checkProperty<QPolygonF>("polygon", showPolygon);

        if (showPolygon)
            updatePositionWidgets(polygon, mPolygonWidgets, mPolygonLayout, this, SLOT(handlePolygonChange(QPointF)));
        else
            clearPositionWidgets(mPolygonWidgets, mPolygonLayout);

        mPolygonGroup->setVisible(showPolygon);
    }
    else
    {
        clearPositionWidgets(mPolygonWidgets, mPolygonLayout);
        mPolygonGroup->setVisible(false);
    }
}

void SingleItemPropertiesWidget::updatePenBrushGroup()
{
    if (mItem)
    {
        bool showPenStyle = false, showPenWidth = false, showPenColor = false, showBrushColor = false;
        const Qt::PenStyle penStyle = static_cast<Qt::PenStyle>(checkIntProperty("penStyle", showPenStyle));
        const double penWidth = checkDoubleProperty("penWidth", showPenWidth);
        const QColor penColor = checkProperty<QColor>("penColor", showPenColor);
        const QColor brushColor = checkProperty<QColor>("brushColor", showBrushColor);

        if (showPenStyle) setPenStyle(penStyle);
        if (showPenWidth) setPenWidth(penWidth);
        if (showPenColor) setPenColor(penColor);
        if (showBrushColor) setBrushColor(brushColor);

        mPenBrushLayout->setRowVisible(mPenStyleCombo, showPenStyle);
        mPenBrushLayout->setRowVisible(mPenWidthEdit, showPenWidth);
        mPenBrushLayout->setRowVisible(mPenColorWidget, showPenColor);
        mPenBrushLayout->setRowVisible(mBrushColorWidget, showBrushColor);
        mPenBrushGroup->setVisible(showPenStyle || showPenWidth || showPenColor || showBrushColor);
    }
    else
    {
        mPenBrushLayout->setRowVisible(mPenStyleCombo, true);
        mPenBrushLayout->setRowVisible(mPenWidthEdit, true);
        mPenBrushLayout->setRowVisible(mPenColorWidget, true);
        mPenBrushLayout->setRowVisible(mBrushColorWidget, true);
        mPenBrushGroup->setVisible(true);
    }
}

void SingleItemPropertiesWidget::updateMarkerGroup()
{
    if (mItem)
    {
        bool showStartMarkerStyle = false, showStartMarkerSize = false, showEndMarkerStyle = false,
            showEndMarkerSize = false;
        const Odg::MarkerStyle startMarkerStyle = static_cast<Odg::MarkerStyle>(checkIntProperty("startMarkerStyle",
                                                                                                 showStartMarkerStyle));
        const double startMarkerSize = checkDoubleProperty("startMarkerSize", showStartMarkerSize);
        const Odg::MarkerStyle endMarkerStyle = static_cast<Odg::MarkerStyle>(checkIntProperty("endMarkerStyle",
                                                                                               showEndMarkerStyle));
        const double endMarkerSize = checkDoubleProperty("endMarkerSize", showEndMarkerSize);

        if (showStartMarkerStyle) setStartMarkerStyle(startMarkerStyle);
        if (showStartMarkerSize) setStartMarkerSize(startMarkerSize);
        if (showEndMarkerStyle) setEndMarkerStyle(endMarkerStyle);
        if (showEndMarkerSize) setEndMarkerSize(endMarkerSize);

        mMarkerLayout->setRowVisible(mStartMarkerStyleCombo, showStartMarkerStyle);
        mMarkerLayout->setRowVisible(mStartMarkerSizeEdit, showStartMarkerSize);
        mMarkerLayout->setRowVisible(mEndMarkerStyleCombo, showEndMarkerStyle);
        mMarkerLayout->setRowVisible(mEndMarkerSizeEdit, showEndMarkerSize);
        mMarkerGroup->setVisible(showStartMarkerStyle || showStartMarkerSize || showEndMarkerStyle || showEndMarkerSize);
    }
    else
    {
        mMarkerLayout->setRowVisible(mStartMarkerStyleCombo, true);
        mMarkerLayout->setRowVisible(mStartMarkerSizeEdit, true);
        mMarkerLayout->setRowVisible(mEndMarkerStyleCombo, true);
        mMarkerLayout->setRowVisible(mEndMarkerSizeEdit, true);
        mMarkerGroup->setVisible(true);
    }
}

void SingleItemPropertiesWidget::updateTextGroup()
{
    if (mItem)
    {
        bool showFontFamily = false, showFontSize = false, showFontStyle = false, showTextAlignment = false,
            showTextPadding = false, showTextColor = false, showCaption = false;
        const QString fontFamily = checkStringProperty("fontFamily", showFontFamily);
        const double fontSize = checkDoubleProperty("fontSize", showFontSize);
        const OdgFontStyle fontStyle = checkProperty<OdgFontStyle>("fontStyle", showFontStyle);
        const Qt::Alignment textAlignment = static_cast<Qt::Alignment>(checkIntProperty("textAlignment",
                                                                                        showTextAlignment));
        const QSizeF textPadding = checkProperty<QSizeF>("textPadding", showTextPadding);
        const QColor textColor = checkProperty<QColor>("textColor", showTextColor);
        const QString caption = checkStringProperty("caption", showCaption);

        if (showFontFamily) setFontFamily(fontFamily);
        if (showFontSize) setFontSize(fontSize);
        if (showFontStyle) setFontStyle(fontStyle);
        if (showTextAlignment) setTextAlignment(textAlignment);
        if (showTextPadding) setTextPadding(textPadding);
        if (showTextColor) setTextColor(textColor);
        if (showCaption && mCaptionEdit->toPlainText() != caption) mCaptionEdit->setPlainText(caption);

        mTextLayout->setRowVisible(mFontFamilyCombo, showFontFamily);
        mTextLayout->setRowVisible(mFontSizeEdit, showFontSize);
        mTextLayout->setRowVisible(mFontStyleWidget, showFontStyle);
        mTextLayout->setRowVisible(mTextAlignmentWidget, showTextAlignment);
        mTextLayout->setRowVisible(mTextPaddingWidget, showTextPadding);
        mTextLayout->setRowVisible(mTextColorWidget, showTextColor);
        mTextLayout->setRowVisible(mCaptionEdit, showCaption);
        mTextGroup->setVisible(showFontFamily || showFontSize || showFontStyle || showTextAlignment ||
                               showTextPadding || showTextColor || showCaption);
    }
    else
    {
        mTextLayout->setRowVisible(mFontFamilyCombo, true);
        mTextLayout->setRowVisible(mFontSizeEdit, true);
        mTextLayout->setRowVisible(mFontStyleWidget, true);
        mTextLayout->setRowVisible(mTextAlignmentWidget, true);
        mTextLayout->setRowVisible(mTextPaddingWidget, true);
        mTextLayout->setRowVisible(mTextColorWidget, true);
        mTextLayout->setRowVisible(mCaptionEdit, false);
        mTextGroup->setVisible(true);
    }
}

//======================================================================================================================

int SingleItemPropertiesWidget::checkIntProperty(const QString& name, bool& hasProperty) const
{
    if (mItem)
    {
        const QVariant propertyValue = mItem->property(name);
        hasProperty = (!propertyValue.isNull() && propertyValue.canConvert<int>());
        if (hasProperty) return propertyValue.toInt();
    }

    hasProperty = false;
    return 0;
}

double SingleItemPropertiesWidget::checkDoubleProperty(const QString& name, bool& hasProperty) const
{
    if (mItem)
    {
        const QVariant propertyValue = mItem->property(name);
        hasProperty = (!propertyValue.isNull() && propertyValue.canConvert<double>());
        if (hasProperty) return propertyValue.toDouble();
    }

    hasProperty = false;
    return 0;
}

QString SingleItemPropertiesWidget::checkStringProperty(const QString& name, bool& hasProperty) const
{
    if (mItem)
    {
        const QVariant propertyValue = mItem->property(name);
        hasProperty = (!propertyValue.isNull() && propertyValue.canConvert<QString>());
        if (hasProperty) return propertyValue.toString();
    }

    hasProperty = false;
    return QString();
}

template<class T> T SingleItemPropertiesWidget::checkProperty(const QString& name, bool& hasProperty) const
{
    if (mItem)
    {
        const QVariant propertyValue = mItem->property(name);
        hasProperty = (!propertyValue.isNull() && propertyValue.canConvert<T>());
        if (hasProperty) return propertyValue.value<T>();
    }

    hasProperty = false;
    return T();
}

//======================================================================================================================

void SingleItemPropertiesWidget::updatePositionWidgets(const QPolygonF& polygon,
                                                       QList<PositionWidget*>& positionWidgets, QFormLayout* layout,
                                                       QObject* slotObject, const char* slotFunction)
{
    if (polygon.size() != positionWidgets.size())
    {
        const int labelWidth = QFontMetrics(QWidget::font()).boundingRect("Bottom-Right Margin:").width() + 8;

        // Clear and delete old position widgets
        clearPositionWidgets(positionWidgets, layout);

        // Create one position widget for each polygon vertex
        PositionWidget* positionWidget = nullptr;
        for(auto& point : polygon)
        {
            positionWidget = new PositionWidget(QPointF(), mPositionWidget->units());
            connect(positionWidget, SIGNAL(positionChanged(QPointF)), slotObject, slotFunction);

            if (positionWidgets.size() == 0)
            {
                layout->addRow("Start Point:", positionWidget);
                layout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth);
            }
            else if (positionWidgets.size() == polygon.size() - 1)
                layout->addRow("End Point:", positionWidget);
            else
                layout->addRow("", positionWidget);

            positionWidgets.append(positionWidget);
        }
    }

    // Set position widget values
    const int size = qMin(polygon.size(), positionWidgets.size());
    for(int i = 0; i < size; i++) positionWidgets.at(i)->setPosition(mItem->mapToScene(polygon.at(i)));
}

void SingleItemPropertiesWidget::clearPositionWidgets(QList<PositionWidget*>& positionWidgets, QFormLayout* layout)
{
    positionWidgets.clear();
    while (layout->rowCount() > 0) layout->removeRow(0);
}

//======================================================================================================================

void SingleItemPropertiesWidget::handlePositionChange(const QPointF& position)
{
    if (mItem) emit itemMoved(position);
}

void SingleItemPropertiesWidget::handleSizeChange(const QSizeF& size)
{
    if (mItem)
    {
        emit itemResized2(mItem->placeResizeStartPoint(),
                          mItem->mapToScene(QPointF(-size.width() / 2, -size.height() / 2)),
                          mItem->placeResizeEndPoint(),
                          mItem->mapToScene(QPointF(size.width() / 2, size.height() / 2)));
    }
}

//======================================================================================================================

void SingleItemPropertiesWidget::handleLineP1Change(const QPointF& position)
{
    if (mItem) emit itemResized(mItem->placeResizeStartPoint(), position);
}

void SingleItemPropertiesWidget::handleLineP2Change(const QPointF& position)
{
    if (mItem) emit itemResized(mItem->placeResizeEndPoint(), position);
}

//======================================================================================================================

void SingleItemPropertiesWidget::handleCurveP1Change(const QPointF& position)
{
    if (mItem)
    {
        const QList<OdgControlPoint*> controlPoints = mItem->controlPoints();
        if (controlPoints.size() >= 4) emit itemResized(controlPoints.at(0), position);
    }
}

void SingleItemPropertiesWidget::handleCurveCP1Change(const QPointF& position)
{
    if (mItem)
    {
        const QList<OdgControlPoint*> controlPoints = mItem->controlPoints();
        if (controlPoints.size() >= 4) emit itemResized(controlPoints.at(1), position);
    }
}

void SingleItemPropertiesWidget::handleCurveCP2Change(const QPointF& position)
{
    if (mItem)
    {
        const QList<OdgControlPoint*> controlPoints = mItem->controlPoints();
        if (controlPoints.size() >= 4) emit itemResized(controlPoints.at(2), position);
    }
}

void SingleItemPropertiesWidget::handleCurveP2Change(const QPointF& position)
{
    if (mItem)
    {
        const QList<OdgControlPoint*> controlPoints = mItem->controlPoints();
        if (controlPoints.size() >= 4) emit itemResized(controlPoints.at(3), position);
    }
}

//======================================================================================================================

void SingleItemPropertiesWidget::handleRectPositionChange(const QPointF& position)
{
    if (mItem) emit itemMoved(position);
}

void SingleItemPropertiesWidget::handleRectSizeChange(const QSizeF& size)
{
    if (mItem)
    {
        if (mItem->rotation() % 2 == 1)
        {
            emit itemResized2(mItem->placeResizeStartPoint(),
                              mItem->mapToScene(QPointF(-size.height() / 2, -size.width() / 2)),
                              mItem->placeResizeEndPoint(),
                              mItem->mapToScene(QPointF(size.height() / 2, size.width() / 2)));
        }
        else
        {
            emit itemResized2(mItem->placeResizeStartPoint(),
                              mItem->mapToScene(QPointF(-size.width() / 2, -size.height() / 2)),
                              mItem->placeResizeEndPoint(),
                              mItem->mapToScene(QPointF(size.width() / 2, size.height() / 2)));
        }
    }
}

void SingleItemPropertiesWidget::handleCornerRadiusChange(double length)
{
    emit itemPropertyChanged("cornerRadius", length);
}

//======================================================================================================================

void SingleItemPropertiesWidget::handleEllipsePositionChange(const QPointF& position)
{
    handleRectPositionChange(position);
}

void SingleItemPropertiesWidget::handleEllipseSizeChange(const QSizeF& size)
{
    handleRectSizeChange(size);
}

//======================================================================================================================

void SingleItemPropertiesWidget::handlePolylineChange(const QPointF& position)
{
    if (mItem)
    {
        const QList<OdgControlPoint*> controlPoints = mItem->controlPoints();

        const int index = mPolylineWidgets.indexOf(sender());
        if (0 <= index && index < controlPoints.size())
            emit itemResized(controlPoints.at(index), position);
    }
}

void SingleItemPropertiesWidget::handlePolygonChange(const QPointF& position)
{
    if (mItem)
    {
        const QList<OdgControlPoint*> controlPoints = mItem->controlPoints();

        const int index = mPolygonWidgets.indexOf(sender());
        if (0 <= index && index < controlPoints.size())
            emit itemResized(controlPoints.at(index), position);
    }
}

//======================================================================================================================

void SingleItemPropertiesWidget::handlePenStyleChange(int index)
{
    emit itemPropertyChanged("penStyle", index);
}

void SingleItemPropertiesWidget::handlePenWidthChange(double length)
{
    emit itemPropertyChanged("penWidth", length);
}

void SingleItemPropertiesWidget::handlePenColorChange(const QColor& color)
{
    emit itemPropertyChanged("penColor", color);
}

void SingleItemPropertiesWidget::handleBrushColorChange(const QColor& color)
{
    emit itemPropertyChanged("brushColor", color);
}

//======================================================================================================================

void SingleItemPropertiesWidget::handleStartMarkerStyleChange(int index)
{
    emit itemPropertyChanged("startMarkerStyle", index);
}

void SingleItemPropertiesWidget::handleStartMarkerSizeChange(double length)
{
    emit itemPropertyChanged("startMarkerSize", length);
}

void SingleItemPropertiesWidget::handleEndMarkerStyleChange(int index)
{
    emit itemPropertyChanged("endMarkerStyle", index);
}

void SingleItemPropertiesWidget::handleEndMarkerSizeChange(double length)
{
    emit itemPropertyChanged("endMarkerSize", length);
}

//======================================================================================================================

void SingleItemPropertiesWidget::handleFontFamilyChange(int index)
{
    emit itemPropertyChanged("fontFamily", fontFamily());
}

void SingleItemPropertiesWidget::handleFontSizeChange(double length)
{
    emit itemPropertyChanged("fontSize", length);
}

void SingleItemPropertiesWidget::handleFontStyleChange()
{
    emit itemPropertyChanged("fontStyle", QVariant::fromValue<OdgFontStyle>(fontStyle()));
}

void SingleItemPropertiesWidget::handleTextAlignmentChange()
{
    emit itemPropertyChanged("textAlignment", static_cast<int>(textAlignment()));
}

void SingleItemPropertiesWidget::handleTextPaddingChange(const QSizeF& size)
{
    emit itemPropertyChanged("textPadding", size);
}

void SingleItemPropertiesWidget::handleTextColorChange(const QColor& color)
{
    emit itemPropertyChanged("textColor", color);
}

void SingleItemPropertiesWidget::handleCaptionChanged()
{
    emit itemPropertyChanged("caption", mCaptionEdit->toPlainText());
}
