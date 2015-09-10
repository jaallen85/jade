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
#include <DrawingItemGroup.h>
#include <DrawingPathItem.h>
#include <DrawingShapeItems.h>
#include <DrawingTextItem.h>
#include <DrawingTwoPointItems.h>

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

	mPositionGroup = nullptr;
	mPositionLayout = nullptr;
	mPositionWidget = nullptr;

	mRectGroup = nullptr;
	mRectLayout = nullptr;
	mRectTopLeftWidget = nullptr;
	mRectBottomRightWidget = nullptr;
	mCornerRadiusWidget = nullptr;

	mStartPointGroup = nullptr;
	mStartPointLayout = nullptr;
	mStartPosWidget = nullptr;
	mStartArrowStyleCombo = nullptr;
	mStartArrowSizeEdit = nullptr;

	mEndPointGroup = nullptr;
	mEndPointLayout = nullptr;
	mEndPosWidget = nullptr;
	mEndArrowStyleCombo = nullptr;
	mEndArrowSizeEdit = nullptr;

	mPenBrushGroup = nullptr;
	mPenBrushLayout = nullptr;
	mPenStyleCombo = nullptr;
	mPenWidthEdit = nullptr;
	mPenColorButton = nullptr;
	mBrushColorButton = nullptr;

	mTextGroup = nullptr;
	mTextLayout = nullptr;
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

	createPositionGroup();
	createRectGroup();
	//createStartPointGroup();
	//createEndPointGroup();
	createPenBrushGroup();
	//createTextGroup();
	//createControlPointsGroup();
	//createPolyPointsGroup();

	QVBoxLayout* vLayout = new QVBoxLayout();
	if (mPositionGroup) vLayout->addWidget(mPositionGroup);
	if (mRectGroup) vLayout->addWidget(mRectGroup);
	if (mStartPointGroup) vLayout->addWidget(mStartPointGroup);
	if (mEndPointGroup) vLayout->addWidget(mEndPointGroup);
	if (mPenBrushGroup) vLayout->addWidget(mPenBrushGroup);
	if (mTextGroup) vLayout->addWidget(mTextGroup);
	//if (controlPointsGroup) vLayout->addWidget(controlPointsGroup);
	//if (polyPointsGroup) vLayout->addWidget(polyPointsGroup);
	vLayout->addWidget(new QWidget(), 100);
	vLayout->setContentsMargins(0, 0, 0, 0);
	setLayout(vLayout);
}

ItemPropertiesWidget::~ItemPropertiesWidget() { }

//==================================================================================================

void ItemPropertiesWidget::handlePropertyChange()
{
	QMap<QString,QVariant> newProperties;

	if (isSender(sender(), mCornerRadiusWidget, mRectLayout))
	{
		newProperties["Corner Radius X"] = QVariant(mCornerRadiusWidget->x());
		newProperties["Corner Radius Y"] = QVariant(mCornerRadiusWidget->y());
	}
	else if (isSender(sender(), mPenStyleCombo, mPenBrushLayout))
		newProperties["Pen Style"] = QVariant((quint32)mPenStyleCombo->currentIndex());
	else if (isSender(sender(), mPenWidthEdit, mPenBrushLayout))
		newProperties["Pen Width"] = QVariant(mPenWidthEdit->text());
	else if (isSender(sender(), mPenColorButton, mPenBrushLayout))
		newProperties["Pen Color"] = QVariant(mPenColorButton->color());
	else if (isSender(sender(), mBrushColorButton, mPenBrushLayout))
		newProperties["Brush Color"] = QVariant(mBrushColorButton->color());

	if (!newProperties.isEmpty()) emit propertiesUpdated(newProperties);
}

//==================================================================================================

QGroupBox* ItemPropertiesWidget::createPositionGroup()
{
	if (mItems.size() == 1)
	{
		mPositionWidget = new PositionWidget();

		mPositionWidget->setPos(mItems.first()->pos());
		connect(mPositionWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handlePositionChange()));

		addWidget(mPositionLayout, "Position: ", mPositionWidget, true);
	}

	if (mPositionLayout)
	{
		mPositionGroup = new QGroupBox("Position");
		mPositionGroup->setLayout(mPositionLayout);
	}

	return mPositionGroup;
}

//==================================================================================================

QGroupBox* ItemPropertiesWidget::createRectGroup()
{
	bool cornerRadiusXMatch = false, cornerRadiusYMatch = false;
	QVariant cornerRadiusXValue, cornerRadiusYValue;

	if (mItems.size() == 1)
	{
		DrawingRectResizeItem* rectItem = dynamic_cast<DrawingRectResizeItem*>(mItems.first());

		if (rectItem)
		{
			mRectTopLeftWidget = new PositionWidget();
			mRectBottomRightWidget = new PositionWidget();

			//mRectTopLeftWidget->setPos(rectItem->topLeft());
			//mRectBottomRightWidget->setPos(rectItem->bottomRight());
			connect(mRectTopLeftWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleRectResize()));
			connect(mRectBottomRightWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleRectResize()));

			addWidget(mRectLayout, "Top Left: ", mRectTopLeftWidget, true);
			addWidget(mRectLayout, "Bottom Right: ", mRectBottomRightWidget, true);
		}
	}

	if (checkAllItemsForProperty("Corner Radius X", cornerRadiusXMatch, cornerRadiusXValue) &&
		checkAllItemsForProperty("Corner Radius Y", cornerRadiusYMatch, cornerRadiusYValue))
	{
		mCornerRadiusWidget = new PositionWidget();

		if (cornerRadiusXValue.isValid()) mCornerRadiusWidget->setX(cornerRadiusXValue.toDouble());
		if (cornerRadiusYValue.isValid()) mCornerRadiusWidget->setY(cornerRadiusYValue.toDouble());
		connect(mCornerRadiusWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handlePropertyChange()));

		addWidget(mRectLayout, "Corner Radius: ", mCornerRadiusWidget, (cornerRadiusXMatch && cornerRadiusYMatch));
	}

	if (mRectLayout)
	{
		mRectGroup = new QGroupBox("Rect");
		mRectGroup->setLayout(mRectLayout);
	}

	return mRectGroup;
}

//==================================================================================================

QGroupBox* ItemPropertiesWidget::createPenBrushGroup()
{
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
		connect(mPenStyleCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(handlePropertyChange()));

		addWidget(mPenBrushLayout, "Pen Style: ", mPenStyleCombo, propertiesMatch);
	}

	if (checkAllItemsForProperty("Pen Width", propertiesMatch, propertyValue))
	{
		mPenWidthEdit = new QLineEdit("12");
		mPenWidthEdit->setValidator(new QDoubleValidator(0, std::numeric_limits<qreal>::max(), 6));

		if (propertyValue.isValid()) mPenWidthEdit->setText(QString::number(propertyValue.toDouble()));
		connect(mPenWidthEdit, SIGNAL(editingFinished()), this, SLOT(handlePropertyChange()));

		addWidget(mPenBrushLayout, "Pen Width: ", mPenWidthEdit, propertiesMatch);
	}

	if (checkAllItemsForProperty("Pen Color", propertiesMatch, propertyValue))
	{
		mPenColorButton = new ColorPushButton();

		if (propertyValue.isValid()) mPenColorButton->setColor(propertyValue.value<QColor>());
		connect(mPenColorButton, SIGNAL(colorChanged(const QColor&)), this, SLOT(handlePropertyChange()));

		addWidget(mPenBrushLayout, "Pen Color: ", mPenColorButton, propertiesMatch);
	}

	if (checkAllItemsForProperty("Brush Color", propertiesMatch, propertyValue))
	{
		mBrushColorButton = new ColorPushButton();
		mBrushColorButton->setColor(Qt::white);

		if (propertyValue.isValid()) mBrushColorButton->setColor(propertyValue.value<QColor>());
		connect(mBrushColorButton, SIGNAL(colorChanged(const QColor&)), this, SLOT(handlePropertyChange()));

		addWidget(mPenBrushLayout, "Brush Color: ", mBrushColorButton, propertiesMatch);
	}

	if (mPenBrushLayout)
	{
		mPenBrushGroup = new QGroupBox("Pen and Brush");
		mPenBrushGroup->setLayout(mPenBrushLayout);
	}

	return mPenBrushGroup;
}

//==================================================================================================

bool ItemPropertiesWidget::isSender(QObject* sender, QWidget* widget, QFormLayout* layout)
{
	return (widget && (sender == widget ||
		(layout && sender == layout->labelForField(widget) && widget->isEnabled())));
}

bool ItemPropertiesWidget::checkAllItemsForProperty(const QString& propertyName,
	bool& propertiesMatch, QVariant& propertyValue)
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

void ItemPropertiesWidget::addWidget(QFormLayout*& formLayout, const QString& label,
	QWidget* widget, bool checked)
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
		connect(checkBox, SIGNAL(toggled(bool)), this, SLOT(handlePropertyChange()));

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
	return fontMetrics.width("Corner Radius:") + ((mItems.size() > 1) ? 24 : 8);
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

PositionWidget::PositionWidget(QWidget* parent) : QWidget(parent)
{
	setupWidget();
}

PositionWidget::PositionWidget(const QPointF& pos, QWidget* parent) : QWidget(parent)
{
	setupWidget();
	setPos(pos);
}

PositionWidget::~PositionWidget() { }

//==================================================================================================

void PositionWidget::setPos(const QPointF& pos)
{
	mXEdit->setText(QString::number(pos.x()));
	mYEdit->setText(QString::number(pos.y()));
}

void PositionWidget::setX(qreal x)
{
	mXEdit->setText(QString::number(x));
}

void PositionWidget::setY(qreal y)
{
	mYEdit->setText(QString::number(y));
}

QPointF PositionWidget::pos() const
{
	return QPointF(mXEdit->text().toDouble(), mYEdit->text().toDouble());
}

qreal PositionWidget::x() const
{
	return mXEdit->text().toDouble();
}

qreal PositionWidget::y() const
{
	return mYEdit->text().toDouble();
}

//==================================================================================================

void PositionWidget::handleEditingFinished()
{
	emit positionChanged(pos());
}

//==================================================================================================

void PositionWidget::setupWidget()
{
	mXEdit = new QLineEdit("0");
	mYEdit = new QLineEdit("0");

	mXEdit->setValidator(new QDoubleValidator(std::numeric_limits<qreal>::min(), std::numeric_limits<qreal>::max(), 6));
	mYEdit->setValidator(new QDoubleValidator(std::numeric_limits<qreal>::min(), std::numeric_limits<qreal>::max(), 6));

	QHBoxLayout* hLayout = new QHBoxLayout();
	hLayout->addWidget(mXEdit);
	hLayout->addWidget(mYEdit);
	hLayout->setSpacing(2);
	hLayout->setContentsMargins(0, 0, 0, 0);
	setLayout(hLayout);

	connect(mXEdit, SIGNAL(editingFinished()), this, SLOT(handleEditingFinished()));
	connect(mYEdit, SIGNAL(editingFinished()), this, SLOT(handleEditingFinished()));
}
