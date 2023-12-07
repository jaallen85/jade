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
    connect(mPenStyleCheck, SIGNAL(clicked(bool)), this, SLOT(handleBrushColorCheckClicked(bool)));
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

void MultipleItemPropertiesWidget::handleCornerRadiusCheckClicked(bool checked)
{

}

void MultipleItemPropertiesWidget::handleCornerRadiusChange(double length)
{

}

//======================================================================================================================

void MultipleItemPropertiesWidget::handlePenStyleCheckClicked(bool checked)
{

}

void MultipleItemPropertiesWidget::handlePenWidthCheckClicked(bool checked)
{

}

void MultipleItemPropertiesWidget::handlePenColorCheckClicked(bool checked)
{

}

void MultipleItemPropertiesWidget::handleBrushColorCheckClicked(bool checked)
{

}

void MultipleItemPropertiesWidget::handlePenStyleChange(int index)
{

}

void MultipleItemPropertiesWidget::handlePenWidthChange(double length)
{

}

void MultipleItemPropertiesWidget::handlePenColorChange(const QColor& color)
{

}

void MultipleItemPropertiesWidget::handleBrushColorChange(const QColor& color)
{

}

//======================================================================================================================

void MultipleItemPropertiesWidget::handleStartMarkerStyleCheckClicked(bool checked)
{

}

void MultipleItemPropertiesWidget::handleStartMarkerSizeCheckClicked(bool checked)
{

}

void MultipleItemPropertiesWidget::handleEndMarkerStyleCheckClicked(bool checked)
{

}

void MultipleItemPropertiesWidget::handleEndMarkerSizeCheckClicked(bool checked)
{

}

void MultipleItemPropertiesWidget::handleStartMarkerStyleChange(int index)
{

}

void MultipleItemPropertiesWidget::handleStartMarkerSizeChange(double length)
{

}

void MultipleItemPropertiesWidget::handleEndMarkerStyleChange(int index)
{

}

void MultipleItemPropertiesWidget::handleEndMarkerSizeChange(double length)
{

}

//======================================================================================================================

void MultipleItemPropertiesWidget::handleFontFamilyCheckClicked(bool checked)
{

}

void MultipleItemPropertiesWidget::handleFontSizeCheckClicked(bool checked)
{

}

void MultipleItemPropertiesWidget::handleFontStyleCheckClicked(bool checked)
{

}

void MultipleItemPropertiesWidget::handleTextAlignmentCheckClicked(bool checked)
{

}

void MultipleItemPropertiesWidget::handleTextPaddingCheckClicked(bool checked)
{

}

void MultipleItemPropertiesWidget::handleTextColorCheckClicked(bool checked)
{

}

void MultipleItemPropertiesWidget::handleFontFamilyChange(int index)
{

}

void MultipleItemPropertiesWidget::handleFontSizeChange(double length)
{

}

void MultipleItemPropertiesWidget::handleFontStyleChange()
{

}

void MultipleItemPropertiesWidget::handleTextAlignmentChange()
{

}

void MultipleItemPropertiesWidget::handleTextPaddingChange(const QSizeF& size)
{

}

void MultipleItemPropertiesWidget::handleTextColorChange(const QColor& color)
{

}
