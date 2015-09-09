/* PropertiesWidget.cpp
 *
 * Copyright (C) 2013-2015 Jason Allen
 *
 * This file is part of the libdrawing library.
 *
 * libdrawing is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libdrawing is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libdrawing.  If not, see <http://www.gnu.org/licenses/>
 */

#include "PropertiesWidget.h"
#include "ColorButton.h"
#include <DrawingItem.h>

PropertiesWidget::PropertiesWidget() : QFrame()
{
	mItemWidget = new ItemPropertiesWidget(QList<DrawingItem*>());
	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(mItemWidget);
	setLayout(layout);
}

PropertiesWidget::~PropertiesWidget() { }

//==================================================================================================

void PropertiesWidget::setFromItems(const QList<DrawingItem*>& items)
{
	qDebug() << "PropertiesWidget::setFromItems";
}

void PropertiesWidget::setFromItem(DrawingItem* item)
{
	QList<DrawingItem*> items;
	items.append(item);
	setFromItems(items);
}

//==================================================================================================

void PropertiesWidget::updateItems(const QList<DrawingItem*>& items)
{
	qDebug() << "PropertiesWidget::updateItems";
}

void PropertiesWidget::updateItem(DrawingItem* item)
{
	QList<DrawingItem*> items;
	items.append(item);
	updateItems(items);
}

//==================================================================================================

QSize PropertiesWidget::sizeHint() const
{
	return QSize(260, 400);
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

ItemPropertiesWidget::ItemPropertiesWidget(const QList<DrawingItem*>& items)
{
	mItems = items;

	mPosXEdit = nullptr;
	mPosYEdit = nullptr;
	mRectLeftEdit = nullptr;
	mRectTopEdit = nullptr;
	mRectRightEdit = nullptr;
	mRectBottomEdit = nullptr;
	mRectCornerRadiusXEdit = nullptr;
	mRectCornerRadiusYEdit = nullptr;
	mStartPosXEdit = nullptr;
	mStartPosYEdit = nullptr;
	mStartArrowStyleCombo = nullptr;
	mStartArrowSizeEdit = nullptr;
	mEndPosXEdit = nullptr;
	mEndPosYEdit = nullptr;
	mEndArrowStyleCombo = nullptr;
	mEndArrowSizeEdit = nullptr;
	mPenStyleCombo = nullptr;
	mPenWidthEdit = nullptr;
	mPenColorButton = nullptr;
	mBrushColorButton = nullptr;
	mFontFamilyCombo = nullptr;
	mFontSizeEdit = nullptr;
	mFontStyleWidget = nullptr;
	mFontBoldButton = nullptr;
	mFontItalicButton = nullptr;
	mFontUnderlineButton = nullptr;
	mFontOverlineButton = nullptr;
	mFontStrikeOutButton = nullptr;
	mTextAlignmentHorizontalCombo = nullptr;
	mTextAlignmentVerticalCombo = nullptr;
	mTextColorButton = nullptr;
	mCaptionEdit = nullptr;
	//mControlPointsWidget = nullptr;
	//mPolyPointsWidget = nullptr;

	//QGroupBox* positionGroup = createPositionGroup();
	//QGroupBox* rectGroup = createRectGroup();
	//QGroupBox* startPointGroup = createStartPointGroup();
	//QGroupBox* endPointGroup = createEndPointGroup();
	QGroupBox* penBrushGroup = createPenBrushGroup();
	//QGroupBox* textGroup = createTextGroup();
	//QGroupBox* controlPointsGroup = createControlPointsGroup();
	//QGroupBox* polyPointsGroup = createPolyPointsGroup();

	QVBoxLayout* vLayout = new QVBoxLayout();
	//if (positionGroup) vLayout->addWidget(positionGroup);
	//if (rectGroup) vLayout->addWidget(rectGroup);
	//if (startPointGroup) vLayout->addWidget(startPointGroup);
	//if (endPointGroup) vLayout->addWidget(endPointGroup);
	if (penBrushGroup) vLayout->addWidget(penBrushGroup);
	//if (textGroup) vLayout->addWidget(textGroup);
	//if (controlPointsGroup) vLayout->addWidget(controlPointsGroup);
	//if (polyPointsGroup) vLayout->addWidget(polyPointsGroup);
	vLayout->setContentsMargins(0, 0, 0, 0);
	setLayout(vLayout);
}

ItemPropertiesWidget::~ItemPropertiesWidget() { }

//==================================================================================================

QGroupBox* ItemPropertiesWidget::createPenBrushGroup()
{
	QGroupBox* penBrushGroup = nullptr;
	QFormLayout* penBrushLayout = nullptr;
	bool propertiesMatch = false;
	QVariant propertyValue;

	if (checkAllItemsForProperty("Pen Style", propertiesMatch, propertyValue))
	{
		mPenStyleCombo = new QComboBox();
		mPenStyleCombo->addItem(QIcon(":/icons/penstyle/pen_none.png"), "No Pen");
		mPenStyleCombo->addItem(QIcon(":/icons/penstyle/pen_solid.png"), "Solid Line");
		mPenStyleCombo->addItem(QIcon(":/icons/penstyle/pen_dashed.png"), "Dashed Line");
		mPenStyleCombo->addItem(QIcon(":/icons/penstyle/pen_dotted.png"), "Dotted Line");
		mPenStyleCombo->addItem(QIcon(":/icons/penstyle/pen_dash_dotted.png"), "Dash-Dotted Line");
		mPenStyleCombo->addItem(QIcon(":/icons/penstyle/pen_dash_dot_dotted.png"), "Dash-Dot-Dotted Line");

		if (propertyValue.isValid()) mPenStyleCombo->setCurrentIndex(propertyValue.toUInt());

		addWidget(penBrushLayout, "Pen Style: ", mPenStyleCombo, propertiesMatch);
	}

	if (checkAllItemsForProperty("Pen Width", propertiesMatch, propertyValue))
	{
		mPenWidthEdit = new QLineEdit();
		mPenWidthEdit->setValidator(new QDoubleValidator(0, std::numeric_limits<qreal>::max(), 6));

		if (propertyValue.isValid()) mPenWidthEdit->setText(QString::number(propertyValue.toDouble()));

		addWidget(penBrushLayout, "Pen Width: ", mPenWidthEdit, propertiesMatch);
	}

	if (checkAllItemsForProperty("Pen Color", propertiesMatch, propertyValue))
	{
		mPenColorButton = new ColorPushButton();

		if (propertyValue.isValid()) mPenColorButton->setColor(propertyValue.value<QColor>());

		addWidget(penBrushLayout, "Pen Color: ", mPenColorButton, propertiesMatch);
	}

	if (checkAllItemsForProperty("Brush Color", propertiesMatch, propertyValue))
	{
		mBrushColorButton = new ColorPushButton();

		if (propertyValue.isValid()) mBrushColorButton->setColor(propertyValue.value<QColor>());

		addWidget(penBrushLayout, "Brush Color: ", mBrushColorButton, propertiesMatch);
	}

	if (penBrushLayout)
	{
		penBrushGroup = new QGroupBox("Pen and Brush");
		penBrushGroup->setLayout(penBrushLayout);
	}

	return penBrushGroup;
}

//==================================================================================================

bool ItemPropertiesWidget::checkAllItemsForProperty(const QString& propertyName, bool& propertiesMatch, QVariant& propertyValue)
{
	bool allItemsHaveProperty = true;

	propertiesMatch = true;
	propertyValue = QVariant();

	for(auto itemIter = mItems.begin(); allItemsHaveProperty && itemIter != mItems.end(); itemIter++)
	{
		allItemsHaveProperty = (*itemIter)->hasProperty(propertyName);
		if (allItemsHaveProperty)
		{
			if (!propertyValue.isValid()) propertyValue = (*itemIter)->property(propertyName);
			else propertiesMatch = (propertiesMatch && propertyValue == (*itemIter)->property(propertyName));
		}
	}

	return allItemsHaveProperty;
}

void ItemPropertiesWidget::addWidget(QFormLayout*& formLayout, const QString& label, QWidget* widget, bool checked)
{
	if (formLayout == nullptr)
	{
		formLayout = new QFormLayout();
		formLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
		formLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	}

	if (mItems.size() > 1)
	{
		QCheckBox* checkBox = new QCheckBox(label);
		checkBox->setChecked(checked);
		widget->setEnabled(checked);
		connect(checkBox, SIGNAL(toggled(bool)), widget, SLOT(setEnabled(bool)));

		formLayout->addRow(checkBox, widget);
	}
	else
	{
		formLayout->addRow(label, widget);
	}

	if (formLayout->rowCount() == 1)
		formLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
}

int ItemPropertiesWidget::labelWidth() const
{
	QFontMetrics fontMetrics(font());
	return fontMetrics.width("Corner Radius:") + ((mItems.size() > 1) ? 20 : 4);
}
