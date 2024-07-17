// File: MultipleItemPropertiesWidget.cpp
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

#include "MultipleItemPropertiesWidget.h"
#include "HelperWidgets.h"
#include "OdgFontStyle.h"
#include "OdgItem.h"
#include <QCheckBox>
#include <QComboBox>
#include <QFontComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QToolButton>
#include <QVBoxLayout>

MultipleItemPropertiesWidget::MultipleItemPropertiesWidget() : QWidget(), mItems(),
    mRectGroup(nullptr), mRectLayout(nullptr), mRectCornerRadiusCheck(nullptr), mRectCornerRadiusEdit(nullptr),
    mPenBrushGroup(nullptr), mPenBrushLayout(nullptr), mPenStyleCheck(nullptr), mPenWidthCheck(nullptr),
    mPenColorCheck(nullptr), mBrushColorCheck(nullptr), mPenStyleCombo(nullptr), mPenWidthEdit(nullptr),
    mPenColorWidget(nullptr), mBrushColorWidget(nullptr),
    mMarkerGroup(nullptr), mMarkerLayout(nullptr), mStartMarkerStyleCheck(nullptr), mStartMarkerSizeCheck(nullptr),
    mEndMarkerStyleCheck(nullptr), mEndMarkerSizeCheck(nullptr), mStartMarkerStyleCombo(nullptr),
    mStartMarkerSizeEdit(nullptr), mEndMarkerStyleCombo(nullptr), mEndMarkerSizeEdit(nullptr),
    mTextGroup(nullptr), mTextLayout(nullptr), mFontFamilyCheck(nullptr), mFontSizeCheck(nullptr),
    mFontStyleCheck(nullptr), mTextAlignmentCheck(nullptr), mTextPaddingCheck(nullptr), mTextColorCheck(nullptr),
    mFontFamilyCombo(nullptr), mFontSizeEdit(nullptr), mFontStyleWidget(nullptr), mFontBoldButton(nullptr),
    mFontItalicButton(nullptr), mFontUnderlineButton(nullptr), mFontStrikeOutButton(nullptr),
    mTextAlignmentWidget(nullptr), mTextAlignmentLeftButton(nullptr), mTextAlignmentHCenterButton(nullptr),
    mTextAlignmentRightButton(nullptr), mTextAlignmentTopButton(nullptr), mTextAlignmentVCenterButton(nullptr),
    mTextAlignmentBottomButton(nullptr), mTextPaddingWidget(nullptr), mTextColorWidget(nullptr)
{
    const int labelWidth = QFontMetrics(QWidget::font()).boundingRect("Bottom-Right Margin:").width() + 8;

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(createRectGroup(labelWidth));
    layout->addWidget(createPenBrushGroup(labelWidth));
    layout->addWidget(createMarkerGroup(labelWidth));
    layout->addWidget(createTextGroup(labelWidth));
    layout->addWidget(new QWidget(), 100);
    setLayout(layout);
}

//======================================================================================================================

QGroupBox* MultipleItemPropertiesWidget::createRectGroup(int labelWidth)
{
    mRectCornerRadiusCheck = new QCheckBox("Corner Radius:");
    connect(mRectCornerRadiusCheck, SIGNAL(clicked(bool)), this, SLOT(handleCornerRadiusCheckClicked(bool)));
    mRectCornerRadiusEdit = new LengthEdit();
    connect(mRectCornerRadiusEdit, SIGNAL(lengthChanged(double)), this, SLOT(handleCornerRadiusChange(double)));

    mRectGroup = new QGroupBox("Rect");
    mRectLayout = new QFormLayout();
    mRectLayout->addRow(mRectCornerRadiusCheck, mRectCornerRadiusEdit);
    mRectLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    mRectLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mRectLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    mRectLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth);
    mRectGroup->setLayout(mRectLayout);

    return mRectGroup;
}

QGroupBox* MultipleItemPropertiesWidget::createPenBrushGroup(int labelWidth)
{
    mPenStyleCheck = new QCheckBox("Pen Style:");
    connect(mPenStyleCheck, SIGNAL(clicked(bool)), this, SLOT(handlePenStyleCheckClicked(bool)));
    mPenStyleCombo = new QComboBox();
    mPenStyleCombo->addItems(QStringList({"None", "Solid", "Dashed", "Dotted", "Dash-Dotted", "Dash-Dot-Dotted"}));
    connect(mPenStyleCombo, SIGNAL(activated(int)), this, SLOT(handlePenStyleChange(int)));

    mPenWidthCheck = new QCheckBox("Pen Width:");
    connect(mPenWidthCheck, SIGNAL(clicked(bool)), this, SLOT(handlePenWidthCheckClicked(bool)));
    mPenWidthEdit = new LengthEdit();
    connect(mPenWidthEdit, SIGNAL(lengthChanged(double)), this, SLOT(handlePenWidthChange(double)));

    mPenColorCheck = new QCheckBox("Pen Color:");
    connect(mPenColorCheck, SIGNAL(clicked(bool)), this, SLOT(handlePenColorCheckClicked(bool)));
    mPenColorWidget = new ColorWidget();
    connect(mPenColorWidget, SIGNAL(colorChanged(QColor)), this, SLOT(handlePenColorChange(QColor)));

    mBrushColorCheck = new QCheckBox("Brush Color:");
    connect(mBrushColorCheck, SIGNAL(clicked(bool)), this, SLOT(handleBrushColorCheckClicked(bool)));
    mBrushColorWidget = new ColorWidget();
    connect(mBrushColorWidget, SIGNAL(colorChanged(QColor)), this, SLOT(handleBrushColorChange(QColor)));

    mPenBrushGroup = new QGroupBox("Pen / Brush");
    mPenBrushLayout = new QFormLayout();
    mPenBrushLayout->addRow(mPenStyleCheck, mPenStyleCombo);
    mPenBrushLayout->addRow(mPenWidthCheck, mPenWidthEdit);
    mPenBrushLayout->addRow(mPenColorCheck, mPenColorWidget);
    mPenBrushLayout->addRow(mBrushColorCheck, mBrushColorWidget);
    mPenBrushLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    mPenBrushLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mPenBrushLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    mPenBrushLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth);
    mPenBrushGroup->setLayout(mPenBrushLayout);

    return mPenBrushGroup;
}

QGroupBox* MultipleItemPropertiesWidget::createMarkerGroup(int labelWidth)
{
    mStartMarkerStyleCheck = new QCheckBox("Start Marker Style:");
    connect(mStartMarkerStyleCheck, SIGNAL(clicked(bool)), this, SLOT(handleStartMarkerStyleCheckClicked(bool)));
    mStartMarkerStyleCombo = new QComboBox();
    mStartMarkerStyleCombo->addItem(QIcon(":/icons/marker/marker-none.png"), "None");
    mStartMarkerStyleCombo->addItem(QIcon(":/icons/marker/marker-triangle-start.png"), "Triangle");
    mStartMarkerStyleCombo->addItem(QIcon(":/icons/marker/marker-circle-start.png"), "Circle");
    connect(mStartMarkerStyleCombo, SIGNAL(activated(int)), this, SLOT(handleStartMarkerStyleChange(int)));

    mStartMarkerSizeCheck = new QCheckBox("Start Marker Size:");
    connect(mStartMarkerStyleCheck, SIGNAL(clicked(bool)), this, SLOT(handleStartMarkerSizeCheckClicked(bool)));
    mStartMarkerSizeEdit = new LengthEdit();
    connect(mStartMarkerSizeEdit, SIGNAL(lengthChanged(double)), this, SLOT(handleStartMarkerSizeChange(double)));

    mEndMarkerStyleCheck = new QCheckBox("End Marker Style:");
    connect(mEndMarkerStyleCheck, SIGNAL(clicked(bool)), this, SLOT(handleEndMarkerStyleCheckClicked(bool)));
    mEndMarkerStyleCombo = new QComboBox();
    mEndMarkerStyleCombo->addItem(QIcon(":/icons/marker/marker-none.png"), "None");
    mEndMarkerStyleCombo->addItem(QIcon(":/icons/marker/marker-triangle-end.png"), "Triangle");
    mEndMarkerStyleCombo->addItem(QIcon(":/icons/marker/marker-circle-end.png"), "Circle");
    connect(mEndMarkerStyleCombo, SIGNAL(activated(int)), this, SLOT(handleEndMarkerStyleChange(int)));

    mEndMarkerSizeCheck = new QCheckBox("End Marker Size:");
    connect(mEndMarkerStyleCheck, SIGNAL(clicked(bool)), this, SLOT(handleEndMarkerSizeCheckClicked(bool)));
    mEndMarkerSizeEdit = new LengthEdit();
    connect(mEndMarkerSizeEdit, SIGNAL(lengthChanged(double)), this, SLOT(handleEndMarkerSizeChange(double)));

    mMarkerGroup = new QGroupBox("Marker");
    mMarkerLayout = new QFormLayout();
    mMarkerLayout->addRow(mStartMarkerStyleCheck, mStartMarkerStyleCombo);
    mMarkerLayout->addRow(mStartMarkerSizeCheck, mStartMarkerSizeEdit);
    mMarkerLayout->addRow(mEndMarkerStyleCheck, mEndMarkerStyleCombo);
    mMarkerLayout->addRow(mEndMarkerSizeCheck, mEndMarkerSizeEdit);
    mMarkerLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    mMarkerLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mMarkerLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    mMarkerLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth);
    mMarkerGroup->setLayout(mMarkerLayout);

    return mMarkerGroup;
}

QGroupBox* MultipleItemPropertiesWidget::createTextGroup(int labelWidth)
{
    mFontFamilyCheck = new QCheckBox("Font:");
    connect(mFontFamilyCheck, SIGNAL(clicked(bool)), this, SLOT(handleFontFamilyCheckClicked(bool)));
    mFontFamilyCombo = new QFontComboBox();
    mFontFamilyCombo->setMaximumWidth(162);
    connect(mFontFamilyCombo, SIGNAL(activated(int)), this, SLOT(handleFontFamilyChange(int)));

    mFontSizeCheck = new QCheckBox("Font Size:");
    connect(mFontSizeCheck, SIGNAL(clicked(bool)), this, SLOT(handleFontSizeCheckClicked(bool)));
    mFontSizeEdit = new LengthEdit();
    connect(mFontSizeEdit, SIGNAL(lengthChanged(double)), this, SLOT(handleFontSizeChange(double)));

    mFontStyleCheck = new QCheckBox("Font Style:");
    connect(mFontStyleCheck, SIGNAL(clicked(bool)), this, SLOT(handleFontStyleCheckClicked(bool)));
    createFontStyleWidget();
    connect(mFontBoldButton, SIGNAL(clicked(bool)), this, SLOT(handleFontStyleChange()));
    connect(mFontItalicButton, SIGNAL(clicked(bool)), this, SLOT(handleFontStyleChange()));
    connect(mFontUnderlineButton, SIGNAL(clicked(bool)), this, SLOT(handleFontStyleChange()));
    connect(mFontStrikeOutButton, SIGNAL(clicked(bool)), this, SLOT(handleFontStyleChange()));

    mTextAlignmentCheck = new QCheckBox("Text Alignment:");
    connect(mTextAlignmentCheck, SIGNAL(clicked(bool)), this, SLOT(handleTextAlignmentCheckClicked(bool)));
    createTextAlignmentWidget();
    connect(mTextAlignmentLeftButton, SIGNAL(clicked(bool)), this, SLOT(handleTextAlignmentChange()));
    connect(mTextAlignmentHCenterButton, SIGNAL(clicked(bool)), this, SLOT(handleTextAlignmentChange()));
    connect(mTextAlignmentRightButton, SIGNAL(clicked(bool)), this, SLOT(handleTextAlignmentChange()));
    connect(mTextAlignmentTopButton, SIGNAL(clicked(bool)), this, SLOT(handleTextAlignmentChange()));
    connect(mTextAlignmentVCenterButton, SIGNAL(clicked(bool)), this, SLOT(handleTextAlignmentChange()));
    connect(mTextAlignmentBottomButton, SIGNAL(clicked(bool)), this, SLOT(handleTextAlignmentChange()));

    mTextPaddingCheck = new QCheckBox("Text Padding:");
    connect(mTextPaddingCheck, SIGNAL(clicked(bool)), this, SLOT(handleTextPaddingCheckClicked(bool)));
    mTextPaddingWidget = new SizeWidget();
    connect(mTextPaddingWidget, SIGNAL(sizeChanged(QSizeF)), this, SLOT(handleTextPaddingChange(QSizeF)));

    mTextColorCheck = new QCheckBox("Text Color:");
    connect(mTextColorCheck, SIGNAL(clicked(bool)), this, SLOT(handleTextColorCheckClicked(bool)));
    mTextColorWidget = new ColorWidget();
    connect(mTextColorWidget, SIGNAL(colorChanged(QColor)), this, SLOT(handleTextColorChange(QColor)));

    mTextGroup = new QGroupBox("Text");
    mTextLayout = new QFormLayout();
    mTextLayout->addRow(mFontFamilyCheck, mFontFamilyCombo);
    mTextLayout->addRow(mFontSizeCheck, mFontSizeEdit);
    mTextLayout->addRow(mFontStyleCheck, mFontStyleWidget);
    mTextLayout->addRow(mTextAlignmentCheck, mTextAlignmentWidget);
    mTextLayout->addRow(mTextPaddingCheck, mTextPaddingWidget);
    mTextLayout->addRow(mTextColorCheck, mTextColorWidget);
    mTextLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    mTextLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mTextLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    mTextLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth);
    mTextGroup->setLayout(mTextLayout);

    return mTextGroup;
}

//======================================================================================================================

void MultipleItemPropertiesWidget::createFontStyleWidget()
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

void MultipleItemPropertiesWidget::createTextAlignmentWidget()
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

void MultipleItemPropertiesWidget::setItems(const QList<OdgItem*>& items)
{
    mItems = items;

    blockSignals(true);
    updateRectGroup();
    updatePenBrushGroup();
    updateMarkerGroup();
    updateTextGroup();
    blockSignals(false);
}

void MultipleItemPropertiesWidget::setUnits(Odg::Units units)
{
    mRectCornerRadiusEdit->setUnits(units);

    mPenWidthEdit->setUnits(units);

    mStartMarkerSizeEdit->setUnits(units);
    mEndMarkerSizeEdit->setUnits(units);

    mFontSizeEdit->setUnits(units);
    mTextPaddingWidget->setUnits(units);
}

void MultipleItemPropertiesWidget::setUnits(int units)
{
    setUnits(static_cast<Odg::Units>(units));
}

//======================================================================================================================

void MultipleItemPropertiesWidget::updateRectGroup()
{
    bool showCornerRadius = false;
    bool cornerRadiusMatches = false;
    const double cornerRadius = checkDoubleProperty("cornerRadius", showCornerRadius, cornerRadiusMatches);

    if (showCornerRadius)
    {
        mRectCornerRadiusEdit->setLength(cornerRadius);
        mRectCornerRadiusEdit->setEnabled(cornerRadiusMatches);
        mRectCornerRadiusCheck->setChecked(cornerRadiusMatches);
    }

    mRectGroup->setVisible(showCornerRadius);
}

void MultipleItemPropertiesWidget::updatePenBrushGroup()
{
    bool showPenStyle = false, showPenWidth = false, showPenColor = false, showBrushColor = false;
    bool penStyleMatches = false, penWidthMatches = false, penColorMatches = false, brushColorMatches = false;
    const int penStyle = checkIntProperty("penStyle", showPenStyle, penStyleMatches);
    const double penWidth = checkDoubleProperty("penWidth", showPenWidth, penWidthMatches);
    const QColor penColor = checkProperty<QColor>("penColor", showPenColor, penColorMatches);
    const QColor brushColor = checkProperty<QColor>("brushColor", showBrushColor, brushColorMatches);

    if (showPenStyle)
    {
        mPenStyleCombo->setCurrentIndex(penStyle);
        mPenStyleCombo->setEnabled(penStyleMatches);
        mPenStyleCheck->setChecked(penStyleMatches);
    }
    if (showPenWidth)
    {
        mPenWidthEdit->setLength(penWidth);
        mPenWidthEdit->setEnabled(penWidthMatches);
        mPenWidthCheck->setChecked(penWidthMatches);
    }
    if (showPenColor)
    {
        mPenColorWidget->setColor(penColor);
        mPenColorWidget->setEnabled(penColorMatches);
        mPenColorCheck->setChecked(penColorMatches);
    }
    if (showBrushColor)
    {
        mBrushColorWidget->setColor(brushColor);
        mBrushColorWidget->setEnabled(brushColorMatches);
        mBrushColorCheck->setChecked(brushColorMatches);
    }

    mPenBrushLayout->setRowVisible(mPenStyleCombo, showPenStyle);
    mPenBrushLayout->setRowVisible(mPenWidthEdit, showPenWidth);
    mPenBrushLayout->setRowVisible(mPenColorWidget, showPenColor);
    mPenBrushLayout->setRowVisible(mBrushColorWidget, showBrushColor);
    mPenBrushGroup->setVisible(showPenStyle || showPenWidth || showPenColor || showBrushColor);
}

void MultipleItemPropertiesWidget::updateMarkerGroup()
{
    bool showStartMarkerStyle = false, showStartMarkerSize = false, showEndMarkerStyle = false, showEndMarkerSize = false;
    bool startMarkerStyleMatches = false, startMarkerSizeMatches = false, endMarkerStyleMatches = false, endMarkerSizeMatches = false;
    const int startMarkerStyle = checkIntProperty("startMarkerStyle", showStartMarkerStyle, startMarkerStyleMatches);
    const double startMarkerSize = checkDoubleProperty("startMarkerSize", showStartMarkerSize, startMarkerSizeMatches);
    const int endMarkerStyle = checkIntProperty("endMarkerStyle", showEndMarkerStyle, endMarkerStyleMatches);
    const double endMarkerSize = checkDoubleProperty("endMarkerSize", showEndMarkerSize, endMarkerSizeMatches);

    if (showStartMarkerStyle)
    {
        mStartMarkerStyleCombo->setCurrentIndex(startMarkerStyle);
        mStartMarkerStyleCombo->setEnabled(startMarkerStyleMatches);
        mStartMarkerStyleCheck->setChecked(startMarkerStyleMatches);
    }
    if (showStartMarkerSize)
    {
        mStartMarkerSizeEdit->setLength(startMarkerSize);
        mStartMarkerSizeEdit->setEnabled(startMarkerSizeMatches);
        mStartMarkerSizeCheck->setChecked(startMarkerSizeMatches);
    }
    if (showEndMarkerStyle)
    {
        mEndMarkerStyleCombo->setCurrentIndex(endMarkerStyle);
        mEndMarkerStyleCombo->setEnabled(endMarkerStyleMatches);
        mEndMarkerStyleCheck->setChecked(endMarkerStyleMatches);
    }
    if (showEndMarkerSize)
    {
        mEndMarkerSizeEdit->setLength(endMarkerSize);
        mEndMarkerSizeEdit->setEnabled(endMarkerSizeMatches);
        mEndMarkerSizeCheck->setChecked(endMarkerSizeMatches);
    }

    mMarkerLayout->setRowVisible(mStartMarkerStyleCombo, showStartMarkerStyle);
    mMarkerLayout->setRowVisible(mStartMarkerSizeEdit, showStartMarkerSize);
    mMarkerLayout->setRowVisible(mEndMarkerStyleCombo, showEndMarkerStyle);
    mMarkerLayout->setRowVisible(mEndMarkerSizeEdit, showEndMarkerSize);
    mMarkerGroup->setVisible(showStartMarkerStyle || showStartMarkerSize || showEndMarkerStyle || showEndMarkerSize);
}

void MultipleItemPropertiesWidget::updateTextGroup()
{
    bool showFontFamily = false, showFontSize = false, showFontStyle = false, showTextAlignment = false,
        showTextPadding = false, showTextColor = false;
    bool fontFamilyMatches = false, fontSizeMatches = false, fontStyleMatches = false, textAlignmentMatches = false,
        textPaddingMatches = false, textColorMatches = false;
    const QString fontFamily = checkStringProperty("fontFamily", showFontFamily, fontFamilyMatches);
    const double fontSize = checkDoubleProperty("fontSize", showFontSize, fontSizeMatches);
    const OdgFontStyle fontStyle = checkProperty<OdgFontStyle>("fontStyle", showFontStyle, fontStyleMatches);
    const Qt::Alignment textAlignment = static_cast<Qt::Alignment>(checkIntProperty("textAlignment", showTextAlignment,
                                                                                    textAlignmentMatches));
    const QSizeF textPadding = checkProperty<QSizeF>("textPadding", showTextPadding, textPaddingMatches);
    const QColor textColor = checkProperty<QColor>("textColor", showTextColor, textColorMatches);

    if (showFontFamily)
    {
        QFont font;
        font.setFamily(fontFamily);
        mFontFamilyCombo->setCurrentFont(font);

        mFontFamilyCombo->setEnabled(fontFamilyMatches);
        mFontFamilyCheck->setChecked(fontFamilyMatches);
    }
    if (showFontSize)
    {
        mFontSizeEdit->setLength(fontSize);
        mFontSizeEdit->setEnabled(fontSizeMatches);
        mFontSizeCheck->setChecked(fontSizeMatches);
    }
    if (showFontStyle)
    {
        mFontBoldButton->setChecked(fontStyle.bold());
        mFontItalicButton->setChecked(fontStyle.italic());
        mFontUnderlineButton->setChecked(fontStyle.underline());
        mFontStrikeOutButton->setChecked(fontStyle.strikeOut());

        mFontStyleWidget->setEnabled(fontStyleMatches);
        mFontStyleCheck->setChecked(fontStyleMatches);
    }
    if (showTextAlignment)
    {
        Qt::Alignment horizontal = (textAlignment & Qt::AlignHorizontal_Mask);
        if (horizontal & Qt::AlignHCenter) mTextAlignmentHCenterButton->setChecked(true);
        else if (horizontal & Qt::AlignRight) mTextAlignmentRightButton->setChecked(true);
        if (horizontal & Qt::AlignLeft) mTextAlignmentLeftButton->setChecked(true);

        Qt::Alignment vertical = (textAlignment & Qt::AlignVertical_Mask);
        if (vertical & Qt::AlignVCenter) mTextAlignmentVCenterButton->setChecked(true);
        else if (vertical & Qt::AlignBottom) mTextAlignmentBottomButton->setChecked(true);
        if (vertical & Qt::AlignLeft) mTextAlignmentTopButton->setChecked(true);

        mTextAlignmentWidget->setEnabled(textAlignmentMatches);
        mTextAlignmentCheck->setChecked(textAlignmentMatches);
    }
    if (showTextPadding)
    {
        mTextPaddingWidget->setSize(textPadding);
        mTextPaddingWidget->setEnabled(textPaddingMatches);
        mTextPaddingCheck->setChecked(textPaddingMatches);
    }
    if (showTextColor)
    {
        mTextColorWidget->setColor(textColor);
        mTextColorWidget->setEnabled(textColorMatches);
        mTextColorCheck->setChecked(textColorMatches);
    }

    mTextLayout->setRowVisible(mFontFamilyCombo, showFontFamily);
    mTextLayout->setRowVisible(mFontSizeEdit, showFontSize);
    mTextLayout->setRowVisible(mFontStyleWidget, showFontStyle);
    mTextLayout->setRowVisible(mTextAlignmentWidget, showTextAlignment);
    mTextLayout->setRowVisible(mTextPaddingWidget, showTextPadding);
    mTextLayout->setRowVisible(mTextColorWidget, showTextColor);
    mTextGroup->setVisible(showFontFamily || showFontSize || showFontStyle || showTextAlignment ||
                           showTextPadding || showTextColor);
}

//======================================================================================================================

int MultipleItemPropertiesWidget::checkIntProperty(const QString& name, bool& anyItemHasProperty,
                                                   bool& propertyValuesMatch) const
{
    int value = 0;
    anyItemHasProperty = false;
    propertyValuesMatch = false;

    QVariant propertyValue;
    bool propertyValid = false;
    for(auto& item : mItems)
    {
        propertyValue = item->property(name);
        propertyValid = (!propertyValue.isNull() && propertyValue.canConvert<int>());
        if (propertyValid)
        {
            if (!anyItemHasProperty)
            {
                anyItemHasProperty = true;
                value = propertyValue.toInt();
                propertyValuesMatch = true;
            }
            else
            {
                propertyValuesMatch = (value == propertyValue.toInt());
                if (!propertyValuesMatch) break;
            }
        }
    }

    return value;
}

double MultipleItemPropertiesWidget::checkDoubleProperty(const QString& name, bool& anyItemHasProperty,
                                                         bool& propertyValuesMatch) const
{
    double value = 0;
    anyItemHasProperty = false;
    propertyValuesMatch = false;

    QVariant propertyValue;
    bool propertyValid = false;
    for(auto& item : mItems)
    {
        propertyValue = item->property(name);
        propertyValid = (!propertyValue.isNull() && propertyValue.canConvert<double>());
        if (propertyValid)
        {
            if (!anyItemHasProperty)
            {
                anyItemHasProperty = true;
                value = propertyValue.toDouble();
                propertyValuesMatch = true;
            }
            else
            {
                propertyValuesMatch = (value == propertyValue.toDouble());
                if (!propertyValuesMatch) break;
            }
        }
    }

    return value;
}

QString MultipleItemPropertiesWidget::checkStringProperty(const QString& name, bool& anyItemHasProperty,
                                                          bool& propertyValuesMatch) const
{
    QString value = 0;
    anyItemHasProperty = false;
    propertyValuesMatch = false;

    QVariant propertyValue;
    bool propertyValid = false;
    for(auto& item : mItems)
    {
        propertyValue = item->property(name);
        propertyValid = (!propertyValue.isNull() && propertyValue.canConvert<QString>());
        if (propertyValid)
        {
            if (!anyItemHasProperty)
            {
                anyItemHasProperty = true;
                value = propertyValue.toString();
                propertyValuesMatch = true;
            }
            else
            {
                propertyValuesMatch = (value == propertyValue.toString());
                if (!propertyValuesMatch) break;
            }
        }
    }

    return value;
}

template<class T> T MultipleItemPropertiesWidget::checkProperty(const QString& name, bool& anyItemHasProperty,
                                                                bool& propertyValuesMatch) const
{
    T value;
    anyItemHasProperty = false;
    propertyValuesMatch = false;

    QVariant propertyValue;
    bool propertyValid = false;
    for(auto& item : mItems)
    {
        propertyValue = item->property(name);
        propertyValid = (!propertyValue.isNull() && propertyValue.canConvert<T>());
        if (propertyValid)
        {
            if (!anyItemHasProperty)
            {
                anyItemHasProperty = true;
                value = propertyValue.value<T>();
                propertyValuesMatch = true;
            }
            else
            {
                propertyValuesMatch = (value == propertyValue.value<T>());
                if (!propertyValuesMatch) break;
            }
        }
    }

    return value;
}

//======================================================================================================================

void MultipleItemPropertiesWidget::handleCornerRadiusCheckClicked(bool checked)
{
    mRectCornerRadiusEdit->setEnabled(checked);
    if (checked) handleCornerRadiusChange(mRectCornerRadiusEdit->length());
}

void MultipleItemPropertiesWidget::handleCornerRadiusChange(double length)
{
    emit itemsPropertyChanged("cornerRadius", length);
}

//======================================================================================================================

void MultipleItemPropertiesWidget::handlePenStyleCheckClicked(bool checked)
{
    mPenStyleCombo->setEnabled(checked);
    if (checked) handlePenStyleChange(mPenStyleCombo->currentIndex());
}

void MultipleItemPropertiesWidget::handlePenWidthCheckClicked(bool checked)
{
    mPenWidthEdit->setEnabled(checked);
    if (checked) handlePenWidthChange(mPenWidthEdit->length());
}

void MultipleItemPropertiesWidget::handlePenColorCheckClicked(bool checked)
{
    mPenColorWidget->setEnabled(checked);
    if (checked) handlePenColorChange(mPenColorWidget->color());
}

void MultipleItemPropertiesWidget::handleBrushColorCheckClicked(bool checked)
{
    mBrushColorWidget->setEnabled(checked);
    if (checked) handleBrushColorChange(mBrushColorWidget->color());
}

void MultipleItemPropertiesWidget::handlePenStyleChange(int index)
{
    emit itemsPropertyChanged("penStyle", index);
}

void MultipleItemPropertiesWidget::handlePenWidthChange(double length)
{
    emit itemsPropertyChanged("penWidth", length);
}

void MultipleItemPropertiesWidget::handlePenColorChange(const QColor& color)
{
    emit itemsPropertyChanged("penColor", color);
}

void MultipleItemPropertiesWidget::handleBrushColorChange(const QColor& color)
{
    emit itemsPropertyChanged("brushColor", color);
}

//======================================================================================================================

void MultipleItemPropertiesWidget::handleStartMarkerStyleCheckClicked(bool checked)
{
    mStartMarkerStyleCombo->setEnabled(checked);
    if (checked) handleStartMarkerStyleChange(mStartMarkerStyleCombo->currentIndex());
}

void MultipleItemPropertiesWidget::handleStartMarkerSizeCheckClicked(bool checked)
{
    mStartMarkerSizeEdit->setEnabled(checked);
    if (checked) handleStartMarkerSizeChange(mStartMarkerSizeEdit->length());
}

void MultipleItemPropertiesWidget::handleEndMarkerStyleCheckClicked(bool checked)
{
    mEndMarkerStyleCombo->setEnabled(checked);
    if (checked) handleEndMarkerStyleChange(mEndMarkerStyleCombo->currentIndex());
}

void MultipleItemPropertiesWidget::handleEndMarkerSizeCheckClicked(bool checked)
{
    mEndMarkerSizeEdit->setEnabled(checked);
    if (checked) handleEndMarkerSizeChange(mEndMarkerSizeEdit->length());
}

void MultipleItemPropertiesWidget::handleStartMarkerStyleChange(int index)
{
    emit itemsPropertyChanged("startMarkerStyle", index);
}

void MultipleItemPropertiesWidget::handleStartMarkerSizeChange(double length)
{
    emit itemsPropertyChanged("startMarkerSize", length);
}

void MultipleItemPropertiesWidget::handleEndMarkerStyleChange(int index)
{
    emit itemsPropertyChanged("endMarkerStyle", index);
}

void MultipleItemPropertiesWidget::handleEndMarkerSizeChange(double length)
{
    emit itemsPropertyChanged("endMarkerSize", length);
}

//======================================================================================================================

void MultipleItemPropertiesWidget::handleFontFamilyCheckClicked(bool checked)
{
    mFontFamilyCombo->setEnabled(checked);
    if (checked) handleFontFamilyChange(mFontFamilyCombo->currentIndex());
}

void MultipleItemPropertiesWidget::handleFontSizeCheckClicked(bool checked)
{
    mFontSizeEdit->setEnabled(checked);
    if (checked) handleFontSizeChange(mFontSizeEdit->length());
}

void MultipleItemPropertiesWidget::handleFontStyleCheckClicked(bool checked)
{
    mFontStyleWidget->setEnabled(checked);
    if (checked) handleFontStyleChange();
}

void MultipleItemPropertiesWidget::handleTextAlignmentCheckClicked(bool checked)
{
    mTextAlignmentWidget->setEnabled(checked);
    if (checked) handleTextAlignmentChange();
}

void MultipleItemPropertiesWidget::handleTextPaddingCheckClicked(bool checked)
{
    mTextPaddingWidget->setEnabled(checked);
    if (checked) handleTextPaddingChange(mTextPaddingWidget->size());
}

void MultipleItemPropertiesWidget::handleTextColorCheckClicked(bool checked)
{
    mTextColorWidget->setEnabled(checked);
    if (checked) handleTextColorChange(mTextColorWidget->color());
}

void MultipleItemPropertiesWidget::handleFontFamilyChange(int index)
{
    emit itemsPropertyChanged("fontFamily", mFontFamilyCombo->currentFont().family());
}

void MultipleItemPropertiesWidget::handleFontSizeChange(double length)
{
    emit itemsPropertyChanged("fontSize", length);
}

void MultipleItemPropertiesWidget::handleFontStyleChange()
{
    OdgFontStyle fontStyle;
    fontStyle.setBold(mFontBoldButton->isChecked());
    fontStyle.setItalic(mFontItalicButton->isChecked());
    fontStyle.setUnderline(mFontUnderlineButton->isChecked());
    fontStyle.setStrikeOut(mFontStrikeOutButton->isChecked());
    emit itemsPropertyChanged("fontStyle", QVariant::fromValue<OdgFontStyle>(fontStyle));
}

void MultipleItemPropertiesWidget::handleTextAlignmentChange()
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

    emit itemsPropertyChanged("textAlignment", static_cast<int>(horizontal | vertical));
}

void MultipleItemPropertiesWidget::handleTextPaddingChange(const QSizeF& size)
{
    emit itemsPropertyChanged("textPadding", size);
}

void MultipleItemPropertiesWidget::handleTextColorChange(const QColor& color)
{
    emit itemsPropertyChanged("textColor", color);
}
