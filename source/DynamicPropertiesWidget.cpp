/* DynamicPropertiesWidget.cpp
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

#include "DynamicPropertiesWidget.h"
#include "HelperWidgets.h"

DynamicPropertiesWidget::DynamicPropertiesWidget(DiagramWidget* diagram) : QScrollArea()
{
	QFormLayout* groupLayout;

	mPositionWidget = new PositionWidget();

	mStartPositionWidget = new PositionWidget();
	mStartControlPositionWidget = new PositionWidget();
	mEndPositionWidget = new PositionWidget();
	mEndControlPositionWidget = new PositionWidget();

	mRectTopLeftWidget = new PositionWidget();
	mRectSizeWidget = new SizeWidget();
	mCornerRadiusWidget = new SizeWidget();

	for(int i = 0; i < 4; i++)
		mPointPositionWidgets.append(new PositionWidget());

	mPenStyleCombo = new PenStyleCombo();
	mPenWidthEdit = new SizeEdit();
	mPenColorWidget = new ColorWidget();
	mBrushColorWidget = new ColorWidget();

	mFontComboBox = new QFontComboBox();
	mFontSizeEdit = new SizeEdit();
	mFontStyleWidget = new FontStyleWidget();
	mTextAlignmentWidget = new TextAlignmentWidget();
	mTextColorWidget = new ColorWidget();
	mCaptionEdit = new QTextEdit();

	mStartArrowCombo = new ArrowStyleCombo();
	mStartArrowSizeEdit = new SizeEdit();
	mEndArrowCombo = new ArrowStyleCombo();
	mEndArrowSizeEdit = new SizeEdit();

	QGroupBox* positionGroup = new QGroupBox("Position");
	groupLayout = new QFormLayout();
	groupLayout->addRow("Position:", mPositionWidget);
	groupLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
	groupLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	groupLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	groupLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
	positionGroup->setLayout(groupLayout);

	QGroupBox* pointsGroup = new QGroupBox("Points");
	groupLayout = new QFormLayout();
	groupLayout->addRow("Start Point:", mStartPositionWidget);
	groupLayout->addRow("Control Point:", mStartControlPositionWidget);
	groupLayout->addRow("End Point:", mEndPositionWidget);
	groupLayout->addRow("Control Point:", mEndControlPositionWidget);
	groupLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
	groupLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	groupLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	groupLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
	pointsGroup->setLayout(groupLayout);

	QGroupBox* rectGroup = new QGroupBox("Rectangle");
	groupLayout = new QFormLayout();
	groupLayout->addRow("Top Left:", mRectTopLeftWidget);
	groupLayout->addRow("Size:", mRectSizeWidget);
	groupLayout->addRow("Corner Radius:", mCornerRadiusWidget);
	groupLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
	groupLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	groupLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	groupLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
	rectGroup->setLayout(groupLayout);

	QString label;
	QGroupBox* polyGroup = new QGroupBox("Polygon");
	groupLayout = new QFormLayout();
	for(auto pointIter = mPointPositionWidgets.begin(); pointIter != mPointPositionWidgets.end(); pointIter++)
	{
		if (pointIter == mPointPositionWidgets.begin()) label = "First Point:";
		else if (pointIter == mPointPositionWidgets.end() - 1) label = "Last Point:";
		else label = "";

		groupLayout->addRow(label, *pointIter);
	}
	groupLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
	groupLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	groupLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	groupLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
	polyGroup->setLayout(groupLayout);

	QGroupBox* penBrushGroup = new QGroupBox("Pen and Brush");
	groupLayout = new QFormLayout();
	groupLayout->addRow("Pen Style:", mPenStyleCombo);
	groupLayout->addRow("Pen Width:", mPenWidthEdit);
	groupLayout->addRow("Pen Color:", mPenColorWidget);
	groupLayout->addRow("Brush Color:", mBrushColorWidget);
	groupLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
	groupLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	groupLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	groupLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
	penBrushGroup->setLayout(groupLayout);

	QGroupBox* textGroup = new QGroupBox("Text");
	groupLayout = new QFormLayout();
	groupLayout->addRow("Font:", mFontComboBox);
	groupLayout->addRow("Font Size:", mFontSizeEdit);
	groupLayout->addRow("Font Style:", mFontStyleWidget);
	groupLayout->addRow("Alignment:", mTextAlignmentWidget);
	groupLayout->addRow("Text Color:", mTextColorWidget);
	groupLayout->addRow("Caption:", mCaptionEdit);
	groupLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
	groupLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	groupLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	groupLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
	textGroup->setLayout(groupLayout);

	QGroupBox* arrowGroup = new QGroupBox("Arrow");
	groupLayout = new QFormLayout();
	groupLayout->addRow("Start Arrow:", mStartArrowCombo);
	groupLayout->addRow("Arrow Size:", mStartArrowSizeEdit);
	groupLayout->addRow("End Arrow:", mEndArrowCombo);
	groupLayout->addRow("Arrow Size:", mEndArrowSizeEdit);
	groupLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
	groupLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	groupLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	groupLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
	arrowGroup->setLayout(groupLayout);

	QWidget* mainWidget = new QWidget();
	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addWidget(positionGroup);
	mainLayout->addWidget(pointsGroup);
	mainLayout->addWidget(rectGroup);
	mainLayout->addWidget(polyGroup);
	mainLayout->addWidget(penBrushGroup);
	mainLayout->addWidget(textGroup);
	mainLayout->addWidget(arrowGroup);
	mainLayout->addWidget(new QWidget(), 100);
	mainWidget->setLayout(mainLayout);

	setWidget(mainWidget);
	setWidgetResizable(true);

	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
}

DynamicPropertiesWidget::~DynamicPropertiesWidget()
{

}

//==================================================================================================

QSize DynamicPropertiesWidget::sizeHint() const
{
	return QSize(350, -1);
	//return QSize(labelWidth() + FontComboBox::preferredWidth() + 8, -1);
}

int DynamicPropertiesWidget::labelWidth() const
{
	return QFontMetrics(font()).width("Background Color:") + 8;
}

//==================================================================================================

void DynamicPropertiesWidget::setSelectedItems(const QList<DrawingItem*>& selectedItems)
{

}

void DynamicPropertiesWidget::setNewItem(DrawingItem* item)
{

}

//==================================================================================================

void DynamicPropertiesWidget::updateGeometryFromSelectedItems(const QList<DrawingItem*>& items)
{

}

void DynamicPropertiesWidget::updateCaptionFromSelectedItem()
{

}

void DynamicPropertiesWidget::updateCornerRadiusFromSelectedItem()
{

}

void DynamicPropertiesWidget::updateStylePropertiesFromSelectedItems()
{

}

void DynamicPropertiesWidget::updatePropertiesFromDiagram()
{

}
