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

DynamicPropertiesWidget::DynamicPropertiesWidget() : QScrollArea()
{
	mStackedWidget = new QStackedWidget();
	clear();

	mTabWidget = new QTabWidget();
	mTabWidget->addTab(createDiagramWidget(), "Diagram");
	mTabWidget->addTab(createItemDefaultsWidget(), "Item Defaults");
	mTabWidget->setTabPosition(QTabWidget::South);
	mStackedWidget->addWidget(mTabWidget);

	setWidget(mStackedWidget);
	setWidgetResizable(true);

	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	setSelectedItems(QList<DrawingItem*>());
}

DynamicPropertiesWidget::~DynamicPropertiesWidget() { }

//==================================================================================================

QSize DynamicPropertiesWidget::sizeHint() const
{
	return QSize(350, -1);
}

int DynamicPropertiesWidget::labelWidth() const
{
	return QFontMetrics(font()).width("Background Color:") + 8;
}

//==================================================================================================

void DynamicPropertiesWidget::setSelectedItems(const QList<DrawingItem*>& selectedItems)
{
	clear();

	if (selectedItems.size() > 1)
	{
		mItems = selectedItems;

		createStyleWidgets();

		mStackedWidget->addWidget(createItemsWidget());
		mStackedWidget->setCurrentIndex(1);
	}
	else if (selectedItems.size() == 1)
	{
		mItems = selectedItems;
		mItem = selectedItems.first();

		createGeometryWidgets();
		createStyleWidgets();

		mStackedWidget->addWidget(createItemsWidget());
		mStackedWidget->setCurrentIndex(1);
	}
}

void DynamicPropertiesWidget::setNewItem(DrawingItem* item)
{
	QList<DrawingItem*> items;
	if (item) items.append(item);
	setSelectedItems(items);
}

void DynamicPropertiesWidget::clear()
{
	QWidget* widget;

	while (mStackedWidget->count() > 1)
	{
		widget = mStackedWidget->widget(1);
		mStackedWidget->removeWidget(widget);
		delete widget;
	}
	mStackedWidget->setCurrentIndex(0);

	mItems.clear();
	mItem = nullptr;

	mPositionWidget = nullptr;
	mStartPositionWidget = nullptr;
	mEndPositionWidget = nullptr;
	mCurveStartPositionWidget = nullptr;
	mCurveStartControlPositionWidget = nullptr;
	mCurveEndPositionWidget = nullptr;
	mCurveEndControlPositionWidget = nullptr;
	mRectTopLeftWidget = nullptr;
	mRectBottomRightWidget = nullptr;
	mCornerRadiusWidget = nullptr;
	mPointPositionStackedWidget = nullptr;
	mPointPositionWidgets.clear();
	mPenStyleCombo = nullptr;
	mPenWidthEdit = nullptr;
	mPenColorWidget = nullptr;
	mBrushColorWidget = nullptr;
	mFontComboBox = nullptr;
	mFontSizeEdit = nullptr;
	mFontStyleWidget = nullptr;
	mTextAlignmentWidget = nullptr;
	mTextColorWidget = nullptr;
	mCaptionEdit = nullptr;
	mStartArrowCombo = nullptr;
	mStartArrowSizeEdit = nullptr;
	mEndArrowCombo = nullptr;
	mEndArrowSizeEdit = nullptr;
	mItemStyleLabels.clear();
}

//==================================================================================================

void DynamicPropertiesWidget::setItemGeometry(const QList<DrawingItem*>& items)
{
	DrawingItem* item = (!items.isEmpty()) ? items.first() : nullptr;

	if (item)
	{
		QList<DrawingItemPoint*> itemPoints = item->points();

		if (mPositionWidget) mPositionWidget->setPos(item->pos());

		if (mStartPositionWidget && itemPoints.size() >= 2)
			mStartPositionWidget->setPos(item->mapToScene(itemPoints.first()->pos()));
		if (mEndPositionWidget && itemPoints.size() >= 2)
			mEndPositionWidget->setPos(item->mapToScene(itemPoints.last()->pos()));

		if (mCurveStartPositionWidget && itemPoints.size() >= 4)
			mCurveStartPositionWidget->setPos(item->mapToScene(itemPoints[0]->pos()));
		if (mCurveEndPositionWidget && itemPoints.size() >= 4)
			mCurveEndPositionWidget->setPos(item->mapToScene(itemPoints[3]->pos()));
		if (mCurveStartControlPositionWidget && itemPoints.size() >= 4)
			mCurveStartControlPositionWidget->setPos(item->mapToScene(itemPoints[1]->pos()));
		if (mCurveEndControlPositionWidget && itemPoints.size() >= 4)
			mCurveEndControlPositionWidget->setPos(item->mapToScene(itemPoints[2]->pos()));

		if (mRectTopLeftWidget && itemPoints.size() >= 8)
			mRectTopLeftWidget->setPos(item->mapToScene(itemPoints[0]->pos()));
		if (mRectBottomRightWidget && itemPoints.size() >= 8)
			mRectBottomRightWidget->setPos(item->mapToScene(itemPoints[4]->pos()));

		if (mPointPositionStackedWidget)
		{
			if (itemPoints.size() != mPointPositionWidgets.size()) fillPointsWidgets();
			for(int i = 0; i < itemPoints.size() && i < mPointPositionWidgets.size(); i++)
				mPointPositionWidgets[i]->setPos(item->mapToScene(itemPoints[i]->pos()));
		}
	}
}

void DynamicPropertiesWidget::setItemCaption(DrawingItem* item)
{
	if (item)
	{
		DrawingTextEllipseItem* textEllipseItem = dynamic_cast<DrawingTextEllipseItem*>(item);
		DrawingTextItem* textItem = dynamic_cast<DrawingTextItem*>(item);
		DrawingTextPolygonItem* textPolygonItem = dynamic_cast<DrawingTextPolygonItem*>(item);
		DrawingTextRectItem* textRectItem = dynamic_cast<DrawingTextRectItem*>(item);

		// Caption widget
		if ((textItem || textRectItem || textEllipseItem || textPolygonItem) && mCaptionEdit)
		{
			QString caption;

			if (textRectItem) caption = textRectItem->caption();
			else if (textEllipseItem) caption = textEllipseItem->caption();
			else if (textPolygonItem) caption = textPolygonItem->caption();
			else caption = textItem->caption();

			if (mCaptionEdit->toPlainText() != caption)
			{
				disconnect(mCaptionEdit, SIGNAL(textChanged()), this, SLOT(handleItemGeometryChange()));
				mCaptionEdit->setPlainText(caption);
				connect(mCaptionEdit, SIGNAL(textChanged()), this, SLOT(handleItemGeometryChange()));
			}
		}
	}
}

void DynamicPropertiesWidget::setItemCornerRadius(DrawingItem* item)
{
	if (item)
	{
		DrawingRectItem* rectItem = dynamic_cast<DrawingRectItem*>(item);
		DrawingTextRectItem* textRectItem = dynamic_cast<DrawingTextRectItem*>(item);

		if ((rectItem || textRectItem) && mCornerRadiusWidget)
		{
			if (textRectItem)
				mCornerRadiusWidget->setSize(QSizeF(textRectItem->cornerRadiusX(), textRectItem->cornerRadiusY()));
			else
				mCornerRadiusWidget->setSize(QSizeF(rectItem->cornerRadiusX(), rectItem->cornerRadiusY()));
		}
	}
}

void DynamicPropertiesWidget::setItemsStyleProperties(const QList<DrawingItem*>& items)
{
	QVector<bool> allItemsHaveProperty(DrawingItemStyle::NumberOfProperties, true);
	QVector<bool> propertiesMatch(DrawingItemStyle::NumberOfProperties, true);
	QVector<QVariant> propertyValue(DrawingItemStyle::NumberOfProperties, QVariant());

	// Check for style properties within selected items
	for(int i = 0; i < DrawingItemStyle::NumberOfProperties; i++)
	{
		for(auto itemIter = items.begin(); allItemsHaveProperty[i] && itemIter != items.end(); itemIter++)
		{
			allItemsHaveProperty[i] = (*itemIter)->style()->hasValue((DrawingItemStyle::Property)i);
			if (allItemsHaveProperty[i])
			{
				if (propertyValue[i].isValid())
				{
					propertiesMatch[i] = (propertiesMatch[i] &&
						propertyValue[i] == (*itemIter)->style()->value((DrawingItemStyle::Property)i));
				}
				else propertyValue[i] = (*itemIter)->style()->value((DrawingItemStyle::Property)i);
			}
		}
	}

	// Pen / Brush widgets
	if (allItemsHaveProperty[DrawingItemStyle::PenStyle] && mPenStyleCombo)
	{
		mPenStyleCombo->setStyle((Qt::PenStyle)propertyValue[DrawingItemStyle::PenStyle].toUInt());
		mPenStyleCombo->setEnabled(propertiesMatch[DrawingItemStyle::PenStyle]);
		if (mItemStyleLabels.contains(mPenStyleCombo)) mItemStyleLabels.value(mPenStyleCombo)->setChecked(mPenStyleCombo->isEnabled());
	}

	if (allItemsHaveProperty[DrawingItemStyle::PenWidth] && mPenWidthEdit)
	{
		mPenWidthEdit->setSize(propertyValue[DrawingItemStyle::PenWidth].toDouble());
		mPenWidthEdit->setEnabled(propertiesMatch[DrawingItemStyle::PenWidth]);
		if (mItemStyleLabels.contains(mPenWidthEdit)) mItemStyleLabels.value(mPenWidthEdit)->setChecked(mPenWidthEdit->isEnabled());
	}

	if (allItemsHaveProperty[DrawingItemStyle::PenColor] && mPenColorWidget)
	{
		mPenColorWidget->setColor(propertyValue[DrawingItemStyle::PenColor].value<QColor>());
		mPenColorWidget->setEnabled(propertiesMatch[DrawingItemStyle::PenColor]);

		if (allItemsHaveProperty[DrawingItemStyle::PenOpacity])
		{
			QColor color = mPenColorWidget->color();
			color.setAlphaF(propertyValue[DrawingItemStyle::PenOpacity].toDouble());
			mPenColorWidget->setColor(color);
			mPenColorWidget->setEnabled(propertiesMatch[DrawingItemStyle::PenColor] &&
				propertiesMatch[DrawingItemStyle::PenOpacity]);
		}

		if (mItemStyleLabels.contains(mPenColorWidget)) mItemStyleLabels.value(mPenColorWidget)->setChecked(mPenColorWidget->isEnabled());
	}

	if (allItemsHaveProperty[DrawingItemStyle::BrushColor] && mBrushColorWidget)
	{
		mBrushColorWidget->setColor(propertyValue[DrawingItemStyle::BrushColor].value<QColor>());
		mBrushColorWidget->setEnabled(propertiesMatch[DrawingItemStyle::BrushColor]);

		if (allItemsHaveProperty[DrawingItemStyle::BrushOpacity])
		{
			QColor color = mBrushColorWidget->color();
			color.setAlphaF(propertyValue[DrawingItemStyle::BrushOpacity].toDouble());
			mBrushColorWidget->setColor(color);
			mBrushColorWidget->setEnabled(propertiesMatch[DrawingItemStyle::BrushColor] &&
				propertiesMatch[DrawingItemStyle::BrushOpacity]);
		}

		if (mItemStyleLabels.contains(mBrushColorWidget)) mItemStyleLabels.value(mBrushColorWidget)->setChecked(mBrushColorWidget->isEnabled());
	}

	// Text widgets
	if (allItemsHaveProperty[DrawingItemStyle::FontName] && mFontComboBox)
	{
		mFontComboBox->setCurrentFont(QFont(propertyValue[DrawingItemStyle::FontName].toString()));
		mFontComboBox->setEnabled(propertiesMatch[DrawingItemStyle::FontName]);
		if (mItemStyleLabels.contains(mFontComboBox)) mItemStyleLabels.value(mFontComboBox)->setChecked(mFontComboBox->isEnabled());
	}

	if (allItemsHaveProperty[DrawingItemStyle::FontSize] && mFontSizeEdit)
	{
		mFontSizeEdit->setSize(propertyValue[DrawingItemStyle::FontSize].toDouble());
		mFontSizeEdit->setEnabled(propertiesMatch[DrawingItemStyle::FontSize]);
		if (mItemStyleLabels.contains(mFontSizeEdit)) mItemStyleLabels.value(mFontSizeEdit)->setChecked(mFontSizeEdit->isEnabled());
	}

	if (allItemsHaveProperty[DrawingItemStyle::FontBold] && allItemsHaveProperty[DrawingItemStyle::FontItalic] &&
		allItemsHaveProperty[DrawingItemStyle::FontUnderline] && allItemsHaveProperty[DrawingItemStyle::FontStrikeThrough] && mFontStyleWidget)
	{
		mFontStyleWidget->setBold(propertyValue[DrawingItemStyle::FontBold].toBool());
		mFontStyleWidget->setItalic(propertyValue[DrawingItemStyle::FontItalic].toBool());
		mFontStyleWidget->setUnderline(propertyValue[DrawingItemStyle::FontUnderline].toBool());
		mFontStyleWidget->setStrikeThrough(propertyValue[DrawingItemStyle::FontStrikeThrough].toBool());
		mFontStyleWidget->setEnabled(propertiesMatch[DrawingItemStyle::FontBold] &&
			propertiesMatch[DrawingItemStyle::FontItalic] &&
			propertiesMatch[DrawingItemStyle::FontUnderline] &&
			propertiesMatch[DrawingItemStyle::FontStrikeThrough]);
		if (mItemStyleLabels.contains(mFontStyleWidget)) mItemStyleLabels.value(mFontStyleWidget)->setChecked(mFontStyleWidget->isEnabled());
	}

	if (allItemsHaveProperty[DrawingItemStyle::TextHorizontalAlignment] &&
		allItemsHaveProperty[DrawingItemStyle::TextVerticalAlignment] && mTextAlignmentWidget)
	{
		mTextAlignmentWidget->setAlignment(
			(Qt::Alignment)propertyValue[DrawingItemStyle::TextHorizontalAlignment].toUInt(),
			(Qt::Alignment)propertyValue[DrawingItemStyle::TextVerticalAlignment].toUInt());
		mTextAlignmentWidget->setEnabled(propertiesMatch[DrawingItemStyle::TextHorizontalAlignment] &&
			propertiesMatch[DrawingItemStyle::TextVerticalAlignment]);
		if (mItemStyleLabels.contains(mTextAlignmentWidget)) mItemStyleLabels.value(mTextAlignmentWidget)->setChecked(mTextAlignmentWidget->isEnabled());
	}

	if (allItemsHaveProperty[DrawingItemStyle::TextColor] && mTextColorWidget)
	{
		mTextColorWidget->setColor(propertyValue[DrawingItemStyle::TextColor].value<QColor>());
		mTextColorWidget->setEnabled(propertiesMatch[DrawingItemStyle::TextColor]);

		if (allItemsHaveProperty[DrawingItemStyle::TextOpacity])
		{
			QColor color = mTextColorWidget->color();
			color.setAlphaF(propertyValue[DrawingItemStyle::TextOpacity].toDouble());
			mTextColorWidget->setColor(color);
			mTextColorWidget->setEnabled(propertiesMatch[DrawingItemStyle::TextColor] &&
				propertiesMatch[DrawingItemStyle::TextOpacity]);
		}

		if (mItemStyleLabels.contains(mTextColorWidget)) mItemStyleLabels.value(mTextColorWidget)->setChecked(mTextColorWidget->isEnabled());
	}

	// Arrow widgets
	if (allItemsHaveProperty[DrawingItemStyle::StartArrowStyle] && mStartArrowCombo)
	{
		mStartArrowCombo->setStyle((DrawingItemStyle::ArrowStyle)propertyValue[DrawingItemStyle::StartArrowStyle].toUInt());
		mStartArrowCombo->setEnabled(propertiesMatch[DrawingItemStyle::StartArrowStyle]);
		if (mItemStyleLabels.contains(mStartArrowCombo)) mItemStyleLabels.value(mStartArrowCombo)->setChecked(mStartArrowCombo->isEnabled());
	}

	if (allItemsHaveProperty[DrawingItemStyle::StartArrowSize] && mStartArrowSizeEdit)
	{
		mStartArrowSizeEdit->setSize(propertyValue[DrawingItemStyle::StartArrowSize].toDouble());
		mStartArrowSizeEdit->setEnabled(propertiesMatch[DrawingItemStyle::StartArrowSize]);
		if (mItemStyleLabels.contains(mStartArrowSizeEdit)) mItemStyleLabels.value(mStartArrowSizeEdit)->setChecked(mStartArrowSizeEdit->isEnabled());
	}

	if (allItemsHaveProperty[DrawingItemStyle::EndArrowStyle] && mEndArrowCombo)
	{
		mEndArrowCombo->setStyle((DrawingItemStyle::ArrowStyle)propertyValue[DrawingItemStyle::EndArrowStyle].toUInt());
		mEndArrowCombo->setEnabled(propertiesMatch[DrawingItemStyle::EndArrowStyle]);
		if (mItemStyleLabels.contains(mEndArrowCombo)) mItemStyleLabels.value(mEndArrowCombo)->setChecked(mEndArrowCombo->isEnabled());
	}

	if (allItemsHaveProperty[DrawingItemStyle::EndArrowSize] && mEndArrowSizeEdit)
	{
		mEndArrowSizeEdit->setSize(propertyValue[DrawingItemStyle::EndArrowSize].toDouble());
		mEndArrowSizeEdit->setEnabled(propertiesMatch[DrawingItemStyle::EndArrowSize]);
		if (mItemStyleLabels.contains(mEndArrowSizeEdit)) mItemStyleLabels.value(mEndArrowSizeEdit)->setChecked(mEndArrowSizeEdit->isEnabled());
	}
}

void DynamicPropertiesWidget::setDiagramProperties(const QHash<DiagramWidget::Property,QVariant>& properties)
{
	if (properties.contains(DiagramWidget::SceneRect))
	{
		QRectF sceneRect = properties.value(DiagramWidget::SceneRect).toRectF();
		mDiagramTopLeftWidget->setPos(sceneRect.topLeft());
		mDiagramRectSizeWidget->setSize(sceneRect.size());
	}

	if (properties.contains(DiagramWidget::BackgroundColor))
		mDiagramBackgroundColorWidget->setColor(properties.value(DiagramWidget::BackgroundColor).value<QColor>());

	if (properties.contains(DiagramWidget::Grid))
		mDiagramGridEdit->setSize(properties.value(DiagramWidget::Grid).toReal());

	if (properties.contains(DiagramWidget::GridStyle))
		mDiagramGridStyleCombo->setStyle((DiagramWidget::GridRenderStyle)properties.value(DiagramWidget::GridStyle).toUInt());

	if (properties.contains(DiagramWidget::GridColor))
		mDiagramGridColorWidget->setColor(properties.value(DiagramWidget::GridColor).value<QColor>());

	if (properties.contains(DiagramWidget::GridSpacingMajor))
		mDiagramGridSpacingMajorWidget->setText(QString::number(properties.value(DiagramWidget::GridSpacingMajor).toInt()));

	if (properties.contains(DiagramWidget::GridSpacingMinor))
		mDiagramGridSpacingMinorWidget->setText(QString::number(properties.value(DiagramWidget::GridSpacingMinor).toInt()));

	mDiagramGridSpacingMinorWidget->setEnabled(mDiagramGridStyleCombo->currentIndex() == 3);
}

//==================================================================================================

void DynamicPropertiesWidget::handleItemGeometryChange()
{
	QObject* sender = DynamicPropertiesWidget::sender();

	if (mPositionWidget && sender == mPositionWidget)
		emit selectedItemMoved(mPositionWidget->pos());
	else if (mCornerRadiusWidget && sender == mCornerRadiusWidget)
		emit selectedItemCornerRadiusChanged(mCornerRadiusWidget->size().width(), mCornerRadiusWidget->size().height());
	else if (mCaptionEdit && sender == mCaptionEdit)
		emit selectedItemCaptionChanged(mCaptionEdit->toPlainText());
	else if (mItem)
	{
		QList<DrawingItemPoint*> itemPoints = mItem->points();

		if (mStartPositionWidget && sender == mStartPositionWidget && itemPoints.size() >= 2)
			emit selectedItemResized(itemPoints.first(), mStartPositionWidget->pos());
		else if (mEndPositionWidget && sender == mEndPositionWidget && itemPoints.size() >= 2)
			emit selectedItemResized(itemPoints.last(), mEndPositionWidget->pos());
		else if (mCurveStartPositionWidget && sender == mCurveStartPositionWidget && itemPoints.size() >= 4)
			emit selectedItemResized(itemPoints[0], mCurveStartPositionWidget->pos());
		else if (mCurveStartControlPositionWidget && sender == mCurveStartControlPositionWidget && itemPoints.size() >= 4)
			emit selectedItemResized(itemPoints[1], mCurveStartControlPositionWidget->pos());
		else if (mCurveEndPositionWidget && sender == mCurveEndPositionWidget && itemPoints.size() >= 4)
			emit selectedItemResized(itemPoints[3], mCurveEndPositionWidget->pos());
		else if (mCurveEndControlPositionWidget && sender == mCurveEndControlPositionWidget && itemPoints.size() >= 4)
			emit selectedItemResized(itemPoints[2], mCurveEndControlPositionWidget->pos());
		else if (mRectTopLeftWidget && sender == mRectTopLeftWidget && itemPoints.size() >= 8)
			emit selectedItemResized(itemPoints[0], mRectTopLeftWidget->pos());
		else if (mRectBottomRightWidget && sender == mRectBottomRightWidget && itemPoints.size() >= 8)
			emit selectedItemResized(itemPoints[4], mRectBottomRightWidget->pos());
		else if	(!mPointPositionWidgets.isEmpty())
		{
			for(int i = 0; i < itemPoints.size() && i < mPointPositionWidgets.size(); i++)
			{
				if (sender == mPointPositionWidgets[i])
					emit selectedItemResized(itemPoints[i], mPointPositionWidgets[i]->pos());
			}
		}
	}
}

void DynamicPropertiesWidget::handleItemsStyleChange()
{
	QHash<DrawingItemStyle::Property,QVariant> newProperties;
	QObject* sender = DynamicPropertiesWidget::sender();

	if (mPenStyleCombo && (sender == mPenStyleCombo || (mItemStyleLabels.contains(mPenStyleCombo) && sender == mItemStyleLabels[mPenStyleCombo] && mItemStyleLabels[mPenStyleCombo]->isChecked())))
		newProperties[DrawingItemStyle::PenStyle] = (uint)mPenStyleCombo->style();
	else if (mPenWidthEdit && (sender == mPenWidthEdit || (mItemStyleLabels.contains(mPenWidthEdit) && sender == mItemStyleLabels[mPenWidthEdit] && mItemStyleLabels[mPenWidthEdit]->isChecked())))
		newProperties[DrawingItemStyle::PenWidth] = mPenWidthEdit->size();
	else if (mPenColorWidget && (sender == mPenColorWidget || (mItemStyleLabels.contains(mPenColorWidget) && sender == mItemStyleLabels[mPenColorWidget] && mItemStyleLabels[mPenColorWidget]->isChecked())))
	{
		QColor color = mPenColorWidget->color();
		newProperties[DrawingItemStyle::PenColor] = color;
		newProperties[DrawingItemStyle::PenOpacity] = color.alphaF();
	}
	else if (mBrushColorWidget && (sender == mBrushColorWidget || (mItemStyleLabels.contains(mBrushColorWidget) && sender == mItemStyleLabels[mBrushColorWidget] && mItemStyleLabels[mBrushColorWidget]->isChecked())))
	{
		QColor color = mBrushColorWidget->color();
		newProperties[DrawingItemStyle::BrushColor] = color;
		newProperties[DrawingItemStyle::BrushOpacity] = color.alphaF();
	}
	else if (mFontComboBox && (sender == mFontComboBox || (mItemStyleLabels.contains(mFontComboBox) && sender == mItemStyleLabels[mFontComboBox] && mItemStyleLabels[mFontComboBox]->isChecked())))
		newProperties[DrawingItemStyle::FontName] = mFontComboBox->currentFont().family();
	else if (mFontSizeEdit && (sender == mFontSizeEdit || (mItemStyleLabels.contains(mFontSizeEdit) && sender == mItemStyleLabels[mFontSizeEdit] && mItemStyleLabels[mFontSizeEdit]->isChecked())))
		newProperties[DrawingItemStyle::FontSize] = mFontSizeEdit->size();
	else if (mFontStyleWidget && (sender == mFontStyleWidget || (mItemStyleLabels.contains(mFontStyleWidget) && sender == mItemStyleLabels[mFontStyleWidget] && mItemStyleLabels[mFontStyleWidget]->isChecked())))
	{
		newProperties[DrawingItemStyle::FontBold] = mFontStyleWidget->isBold();
		newProperties[DrawingItemStyle::FontItalic] = mFontStyleWidget->isItalic();
		newProperties[DrawingItemStyle::FontUnderline] = mFontStyleWidget->isUnderline();
		newProperties[DrawingItemStyle::FontStrikeThrough] = mFontStyleWidget->isStrikeThrough();
	}
	else if (mTextAlignmentWidget && (sender == mTextAlignmentWidget || (mItemStyleLabels.contains(mTextAlignmentWidget) && sender == mItemStyleLabels[mTextAlignmentWidget] && mItemStyleLabels[mTextAlignmentWidget]->isChecked())))
	{
		newProperties[DrawingItemStyle::TextHorizontalAlignment] = (uint)mTextAlignmentWidget->horizontalAlignment();
		newProperties[DrawingItemStyle::TextVerticalAlignment] = (uint)mTextAlignmentWidget->verticalAlignment();
	}
	else if (mTextColorWidget && (sender == mTextColorWidget || (mItemStyleLabels.contains(mTextColorWidget) && sender == mItemStyleLabels[mTextColorWidget] && mItemStyleLabels[mTextColorWidget]->isChecked())))
	{
		QColor color = mTextColorWidget->color();
		newProperties[DrawingItemStyle::TextColor] = color;
		newProperties[DrawingItemStyle::TextOpacity] = color.alphaF();
	}
	else if (mStartArrowCombo && (sender == mStartArrowCombo || (mItemStyleLabels.contains(mStartArrowCombo) && sender == mItemStyleLabels[mStartArrowCombo] && mItemStyleLabels[mStartArrowCombo]->isChecked())))
		newProperties[DrawingItemStyle::StartArrowStyle] = (uint)mStartArrowCombo->style();
	else if (mStartArrowSizeEdit && (sender == mStartArrowSizeEdit || (mItemStyleLabels.contains(mStartArrowSizeEdit) && sender == mItemStyleLabels[mStartArrowSizeEdit] && mItemStyleLabels[mStartArrowSizeEdit]->isChecked())))
		newProperties[DrawingItemStyle::StartArrowSize] = mStartArrowSizeEdit->size();
	else if (mEndArrowCombo && (sender == mEndArrowCombo || (mItemStyleLabels.contains(mEndArrowCombo) && sender == mItemStyleLabels[mEndArrowCombo] && mItemStyleLabels[mEndArrowCombo]->isChecked())))
		newProperties[DrawingItemStyle::EndArrowStyle] = (uint)mEndArrowCombo->style();
	else if (mEndArrowSizeEdit && (sender == mEndArrowSizeEdit || (mItemStyleLabels.contains(mEndArrowSizeEdit) && sender == mItemStyleLabels[mEndArrowSizeEdit] && mItemStyleLabels[mEndArrowSizeEdit]->isChecked())))
		newProperties[DrawingItemStyle::EndArrowSize] = mEndArrowSizeEdit->size();

	if (!newProperties.isEmpty()) emit selectedItemsStyleChanged(newProperties);
}

void DynamicPropertiesWidget::handleItemDefaultsChange()
{
	QObject* sender = DynamicPropertiesWidget::sender();

	if (sender == mDefaultPenStyleCombo)
		DrawingItemStyle::setDefaultValue(DrawingItemStyle::PenStyle, (uint)mDefaultPenStyleCombo->style());
	else if (sender == mDefaultPenWidthEdit)
		DrawingItemStyle::setDefaultValue(DrawingItemStyle::PenWidth, mDefaultPenWidthEdit->size());
	else if (sender == mDefaultPenColorWidget)
	{
		QColor color = mDefaultPenColorWidget->color();
		DrawingItemStyle::setDefaultValue(DrawingItemStyle::PenColor, color);
		DrawingItemStyle::setDefaultValue(DrawingItemStyle::PenOpacity, color.alphaF());
	}
	else if (sender == mDefaultBrushColorWidget)
	{
		QColor color = mDefaultBrushColorWidget->color();
		DrawingItemStyle::setDefaultValue(DrawingItemStyle::BrushColor, color);
		DrawingItemStyle::setDefaultValue(DrawingItemStyle::BrushOpacity, color.alphaF());
	}
	else if (sender == mDefaultFontComboBox)
		DrawingItemStyle::setDefaultValue(DrawingItemStyle::FontName, mDefaultFontComboBox->currentFont().family());
	else if (sender == mDefaultFontSizeEdit)
		DrawingItemStyle::setDefaultValue(DrawingItemStyle::FontSize, mDefaultFontSizeEdit->size());
	else if (sender == mDefaultFontStyleWidget)
	{
		DrawingItemStyle::setDefaultValue(DrawingItemStyle::FontBold, mDefaultFontStyleWidget->isBold());
		DrawingItemStyle::setDefaultValue(DrawingItemStyle::FontItalic, mDefaultFontStyleWidget->isItalic());
		DrawingItemStyle::setDefaultValue(DrawingItemStyle::FontUnderline, mDefaultFontStyleWidget->isUnderline());
		DrawingItemStyle::setDefaultValue(DrawingItemStyle::FontStrikeThrough, mDefaultFontStyleWidget->isStrikeThrough());
	}
	else if (sender == mDefaultTextAlignmentWidget)
	{
		DrawingItemStyle::setDefaultValue(DrawingItemStyle::TextHorizontalAlignment, (uint)mDefaultTextAlignmentWidget->horizontalAlignment());
		DrawingItemStyle::setDefaultValue(DrawingItemStyle::TextVerticalAlignment, (uint)mDefaultTextAlignmentWidget->verticalAlignment());
	}
	else if (sender == mDefaultTextColorWidget)
	{
		QColor color = mDefaultTextColorWidget->color();
		DrawingItemStyle::setDefaultValue(DrawingItemStyle::TextColor, color);
		DrawingItemStyle::setDefaultValue(DrawingItemStyle::TextOpacity, color.alphaF());
	}
	else if (sender == mDefaultStartArrowCombo)
		DrawingItemStyle::setDefaultValue(DrawingItemStyle::StartArrowStyle, (uint)mDefaultStartArrowCombo->style());
	else if (sender == mDefaultStartArrowSizeEdit)
		DrawingItemStyle::setDefaultValue(DrawingItemStyle::StartArrowSize, mDefaultStartArrowSizeEdit->size());
	else if (sender == mDefaultEndArrowCombo)
		DrawingItemStyle::setDefaultValue(DrawingItemStyle::EndArrowStyle, (uint)mDefaultEndArrowCombo->style());
	else if (sender == mDefaultEndArrowSizeEdit)
		DrawingItemStyle::setDefaultValue(DrawingItemStyle::EndArrowSize, mDefaultEndArrowSizeEdit->size());
}

void DynamicPropertiesWidget::handleDiagramChange()
{
	QHash<DiagramWidget::Property,QVariant> newProperties;
	QObject* sender = DynamicPropertiesWidget::sender();

	if (sender == mDiagramGridStyleCombo)
		mDiagramGridSpacingMinorWidget->setEnabled(mDiagramGridStyleCombo->currentIndex() == 3);

	if (sender == mDiagramTopLeftWidget || sender == mDiagramRectSizeWidget)
		newProperties[DiagramWidget::SceneRect] = QRectF(mDiagramTopLeftWidget->pos(), mDiagramRectSizeWidget->size());
	else if (sender == mDiagramBackgroundColorWidget)
		newProperties[DiagramWidget::BackgroundColor] = mDiagramBackgroundColorWidget->color();
	else if (sender == mDiagramGridEdit)
		newProperties[DiagramWidget::Grid] = mDiagramGridEdit->size();
	else if (sender == mDiagramGridStyleCombo)
		newProperties[DiagramWidget::GridStyle] = (uint)mDiagramGridStyleCombo->style();
	else if (sender == mDiagramGridColorWidget)
		newProperties[DiagramWidget::GridColor] = mDiagramGridColorWidget->color();
	else if (sender == mDiagramGridSpacingMajorWidget)
		newProperties[DiagramWidget::GridSpacingMajor] = mDiagramGridSpacingMajorWidget->text().toInt();
	else if (sender == mDiagramGridSpacingMinorWidget)
		newProperties[DiagramWidget::GridSpacingMinor] = mDiagramGridSpacingMinorWidget->text().toInt();

	if (!newProperties.isEmpty()) emit diagramPropertiesChanged(newProperties);
}

//==================================================================================================

QWidget* DynamicPropertiesWidget::createDiagramWidget()
{
	QWidget* diagramPropertiesWidget = new QWidget();
	QVBoxLayout* diagramPropertiesLayout = new QVBoxLayout();
	QGroupBox* groupBox;
	QFormLayout* groupLayout;

	// Create widgets
	mDiagramTopLeftWidget = new PositionWidget();
	connect(mDiagramTopLeftWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleDiagramChange()));

	mDiagramRectSizeWidget = new SizeWidget(QSizeF(10000, 7500));
	connect(mDiagramRectSizeWidget, SIGNAL(sizeChanged(const QSizeF&)), this, SLOT(handleDiagramChange()));

	mDiagramBackgroundColorWidget = new ColorWidget();
	connect(mDiagramBackgroundColorWidget, SIGNAL(colorChanged(const QColor&)), this, SLOT(handleDiagramChange()));

	mDiagramGridEdit = new SizeEdit();
	connect(mDiagramGridEdit, SIGNAL(sizeChanged(qreal)), this, SLOT(handleDiagramChange()));

	mDiagramGridStyleCombo = new GridStyleCombo();
	connect(mDiagramGridStyleCombo, SIGNAL(activated(int)), this, SLOT(handleDiagramChange()));

	mDiagramGridColorWidget = new ColorWidget();
	connect(mDiagramGridColorWidget, SIGNAL(colorChanged(const QColor&)), this, SLOT(handleDiagramChange()));

	mDiagramGridSpacingMajorWidget = new QLineEdit();
	mDiagramGridSpacingMajorWidget->setValidator(new QIntValidator(1, 1E6));
	connect(mDiagramGridSpacingMajorWidget, SIGNAL(returnPressed()), this, SLOT(handleDiagramChange()));

	mDiagramGridSpacingMinorWidget = new QLineEdit();
	mDiagramGridSpacingMinorWidget->setValidator(new QIntValidator(1, 1E6));
	connect(mDiagramGridSpacingMinorWidget, SIGNAL(returnPressed()), this, SLOT(handleDiagramChange()));

	// Assemble layout
	groupBox = new QGroupBox("Diagram");
	groupLayout = new QFormLayout();
	groupLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
	groupLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	groupLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	groupLayout->addRow("Top Left:", mDiagramTopLeftWidget);
	groupLayout->addRow("Size:", mDiagramRectSizeWidget);
	groupLayout->addRow("Background Color:", mDiagramBackgroundColorWidget);
	groupLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
	groupBox->setLayout(groupLayout);
	diagramPropertiesLayout->addWidget(groupBox);

	groupBox = new QGroupBox("Grid");
	groupLayout = new QFormLayout();
	groupLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
	groupLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	groupLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	groupLayout->addRow("Grid:", mDiagramGridEdit);
	groupLayout->addRow("Grid Style:", mDiagramGridStyleCombo);
	groupLayout->addRow("Grid Color:", mDiagramGridColorWidget);
	groupLayout->addRow("Major Spacing:", mDiagramGridSpacingMajorWidget);
	groupLayout->addRow("Minor Spacing:", mDiagramGridSpacingMinorWidget);
	groupLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
	groupBox->setLayout(groupLayout);
	diagramPropertiesLayout->addWidget(groupBox);

	diagramPropertiesLayout->addWidget(new QWidget(), 100);
	diagramPropertiesWidget->setLayout(diagramPropertiesLayout);
	return diagramPropertiesWidget;
}

QWidget* DynamicPropertiesWidget::createItemDefaultsWidget()
{
	QWidget* itemDefaultPropertiesWidget = new QWidget();
	QVBoxLayout* itemDefaultPropertiesLayout = new QVBoxLayout();
	QGroupBox* groupBox;
	QFormLayout* groupLayout;

	// Pen / Brush widgets
	mDefaultPenStyleCombo = new PenStyleCombo((Qt::PenStyle)DrawingItemStyle::defaultValue(DrawingItemStyle::PenStyle).toUInt());
	connect(mDefaultPenStyleCombo, SIGNAL(activated(int)), this, SLOT(handleItemDefaultsChange()));

	mDefaultPenWidthEdit = new SizeEdit(DrawingItemStyle::defaultValue(DrawingItemStyle::PenWidth).toDouble());
	connect(mDefaultPenWidthEdit, SIGNAL(sizeChanged(qreal)), this, SLOT(handleItemDefaultsChange()));

	mDefaultPenColorWidget = new ColorWidget();
	QColor color = DrawingItemStyle::defaultValue(DrawingItemStyle::PenColor).value<QColor>();
	color.setAlphaF(DrawingItemStyle::defaultValue(DrawingItemStyle::PenOpacity).toDouble());
	mDefaultPenColorWidget->setColor(color);
	connect(mDefaultPenColorWidget, SIGNAL(colorChanged(const QColor&)), this, SLOT(handleItemDefaultsChange()));

	mDefaultBrushColorWidget = new ColorWidget();
	color = DrawingItemStyle::defaultValue(DrawingItemStyle::BrushColor).value<QColor>();
	color.setAlphaF(DrawingItemStyle::defaultValue(DrawingItemStyle::BrushOpacity).toDouble());
	mDefaultBrushColorWidget->setColor(color);
	connect(mDefaultBrushColorWidget, SIGNAL(colorChanged(const QColor&)), this, SLOT(handleItemDefaultsChange()));

	// Text widgets
	mDefaultFontComboBox = new QFontComboBox();
	mDefaultFontComboBox->setCurrentFont(QFont(DrawingItemStyle::defaultValue(DrawingItemStyle::FontName).toString()));
	connect(mDefaultFontComboBox, SIGNAL(activated(int)), this, SLOT(handleItemDefaultsChange()));

	mDefaultFontSizeEdit = new SizeEdit(DrawingItemStyle::defaultValue(DrawingItemStyle::FontSize).toDouble());
	connect(mDefaultFontSizeEdit, SIGNAL(sizeChanged(qreal)), this, SLOT(handleItemDefaultsChange()));

	mDefaultFontStyleWidget = new FontStyleWidget();
	mDefaultFontStyleWidget->setBold(DrawingItemStyle::defaultValue(DrawingItemStyle::FontBold).toBool());
	mDefaultFontStyleWidget->setItalic(DrawingItemStyle::defaultValue(DrawingItemStyle::FontItalic).toBool());
	mDefaultFontStyleWidget->setUnderline(DrawingItemStyle::defaultValue(DrawingItemStyle::FontUnderline).toBool());
	mDefaultFontStyleWidget->setStrikeThrough(DrawingItemStyle::defaultValue(DrawingItemStyle::FontStrikeThrough).toBool());
	connect(mDefaultFontStyleWidget, SIGNAL(boldChanged(bool)), this, SLOT(handleItemDefaultsChange()));
	connect(mDefaultFontStyleWidget, SIGNAL(italicChanged(bool)), this, SLOT(handleItemDefaultsChange()));
	connect(mDefaultFontStyleWidget, SIGNAL(underlineChanged(bool)), this, SLOT(handleItemDefaultsChange()));
	connect(mDefaultFontStyleWidget, SIGNAL(strikeThroughChanged(bool)), this, SLOT(handleItemDefaultsChange()));

	mDefaultTextAlignmentWidget = new TextAlignmentWidget();
	mDefaultTextAlignmentWidget->setAlignment(
		(Qt::Alignment)DrawingItemStyle::defaultValue(DrawingItemStyle::TextHorizontalAlignment).toUInt(),
		(Qt::Alignment)DrawingItemStyle::defaultValue(DrawingItemStyle::TextVerticalAlignment).toUInt());
	connect(mDefaultTextAlignmentWidget, SIGNAL(horizontalAlignmentChanged(Qt::Alignment)), this, SLOT(handleItemDefaultsChange()));
	connect(mDefaultTextAlignmentWidget, SIGNAL(verticalAlignmentChanged(Qt::Alignment)), this, SLOT(handleItemDefaultsChange()));

	mDefaultTextColorWidget = new ColorWidget();
	color = DrawingItemStyle::defaultValue(DrawingItemStyle::TextColor).value<QColor>();
	color.setAlphaF(DrawingItemStyle::defaultValue(DrawingItemStyle::TextOpacity).toDouble());
	mDefaultTextColorWidget->setColor(color);
	connect(mDefaultTextColorWidget, SIGNAL(colorChanged(const QColor&)), this, SLOT(handleItemDefaultsChange()));

	// Arrow widgets
	mDefaultStartArrowCombo = new ArrowStyleCombo(
		(DrawingItemStyle::ArrowStyle)DrawingItemStyle::defaultValue(DrawingItemStyle::StartArrowStyle).toUInt());
	connect(mDefaultStartArrowCombo, SIGNAL(activated(int)), this, SLOT(handleItemDefaultsChange()));

	mDefaultStartArrowSizeEdit = new SizeEdit(DrawingItemStyle::defaultValue(DrawingItemStyle::StartArrowSize).toDouble());
	connect(mDefaultStartArrowSizeEdit, SIGNAL(sizeChanged(qreal)), this, SLOT(handleItemDefaultsChange()));

	mDefaultEndArrowCombo = new ArrowStyleCombo(
		(DrawingItemStyle::ArrowStyle)DrawingItemStyle::defaultValue(DrawingItemStyle::EndArrowStyle).toUInt());
	connect(mDefaultEndArrowCombo, SIGNAL(activated(int)), this, SLOT(handleItemDefaultsChange()));

	mDefaultEndArrowSizeEdit = new SizeEdit(DrawingItemStyle::defaultValue(DrawingItemStyle::EndArrowSize).toDouble());
	connect(mDefaultEndArrowSizeEdit, SIGNAL(sizeChanged(qreal)), this, SLOT(handleItemDefaultsChange()));

	// Assemble layout
	groupBox = new QGroupBox("Pen / Brush Defaults");
	groupLayout = new QFormLayout();
	groupLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
	groupLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	groupLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	groupLayout->addRow("Pen Style:", mDefaultPenStyleCombo);
	groupLayout->addRow("Pen Width:", mDefaultPenWidthEdit);
	groupLayout->addRow("Pen Color:", mDefaultPenColorWidget);
	groupLayout->addRow("Brush Color:", mDefaultBrushColorWidget);
	groupLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
	groupBox->setLayout(groupLayout);
	itemDefaultPropertiesLayout->addWidget(groupBox);

	groupBox = new QGroupBox("Text Defaults");
	groupLayout = new QFormLayout();
	groupLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
	groupLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	groupLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	groupLayout->addRow("Font:", mDefaultFontComboBox);
	groupLayout->addRow("Font Size:", mDefaultFontSizeEdit);
	groupLayout->addRow("Font Style:", mDefaultFontStyleWidget);
	groupLayout->addRow("Alignment:", mDefaultTextAlignmentWidget);
	groupLayout->addRow("Text Color:", mDefaultTextColorWidget);
	groupLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
	groupBox->setLayout(groupLayout);
	itemDefaultPropertiesLayout->addWidget(groupBox);

	groupBox = new QGroupBox("Arrow Defaults");
	groupLayout = new QFormLayout();
	groupLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
	groupLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	groupLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	groupLayout->addRow("Start Arrow:", mDefaultStartArrowCombo);
	groupLayout->addRow("Arrow Size:", mDefaultStartArrowSizeEdit);
	groupLayout->addRow("End Arrow:", mDefaultEndArrowCombo);
	groupLayout->addRow("Arrow Size:", mDefaultEndArrowSizeEdit);
	groupLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
	groupBox->setLayout(groupLayout);
	itemDefaultPropertiesLayout->addWidget(groupBox);

	itemDefaultPropertiesLayout->addWidget(new QWidget(), 100);
	itemDefaultPropertiesWidget->setLayout(itemDefaultPropertiesLayout);
	return itemDefaultPropertiesWidget;
}

//==================================================================================================

void DynamicPropertiesWidget::createGeometryWidgets()
{
	DrawingArcItem* arcItem = dynamic_cast<DrawingArcItem*>(mItem);
	DrawingCurveItem* curveItem = dynamic_cast<DrawingCurveItem*>(mItem);
	DrawingEllipseItem* ellipseItem = dynamic_cast<DrawingEllipseItem*>(mItem);
	DrawingLineItem* lineItem = dynamic_cast<DrawingLineItem*>(mItem);
	DrawingPathItem* pathItem = dynamic_cast<DrawingPathItem*>(mItem);
	DrawingPolygonItem* polygonItem = dynamic_cast<DrawingPolygonItem*>(mItem);
	DrawingPolylineItem* polylineItem = dynamic_cast<DrawingPolylineItem*>(mItem);
	DrawingRectItem* rectItem = dynamic_cast<DrawingRectItem*>(mItem);
	DrawingTextEllipseItem* textEllipseItem = dynamic_cast<DrawingTextEllipseItem*>(mItem);
	DrawingTextItem* textItem = dynamic_cast<DrawingTextItem*>(mItem);
	DrawingTextPolygonItem* textPolygonItem = dynamic_cast<DrawingTextPolygonItem*>(mItem);
	DrawingTextRectItem* textRectItem = dynamic_cast<DrawingTextRectItem*>(mItem);
	DrawingItemGroup* groupItem = dynamic_cast<DrawingItemGroup*>(mItem);

	// Position widget
	if (pathItem || polygonItem || polylineItem || textItem || textPolygonItem || groupItem)
	{
		mPositionWidget = new PositionWidget(mItem->pos());
		connect(mPositionWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleItemGeometryChange()));
	}

	// Line/arc widgets
	if (arcItem || lineItem)
	{
		mStartPositionWidget = new PositionWidget(mItem->mapToScene(mItem->points().first()->pos()));
		connect(mStartPositionWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleItemGeometryChange()));

		mEndPositionWidget = new PositionWidget(mItem->mapToScene(mItem->points().last()->pos()));
		connect(mEndPositionWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleItemGeometryChange()));
	}

	// Curve widgets
	if (curveItem)
	{
		mCurveStartPositionWidget = new PositionWidget(mItem->mapToScene(mItem->points()[0]->pos()));
		connect(mCurveStartPositionWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleItemGeometryChange()));

		mCurveEndPositionWidget = new PositionWidget(mItem->mapToScene(mItem->points()[3]->pos()));
		connect(mCurveEndPositionWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleItemGeometryChange()));

		mCurveStartControlPositionWidget = new PositionWidget(mItem->mapToScene(mItem->points()[1]->pos()));
		connect(mCurveStartControlPositionWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleItemGeometryChange()));

		mCurveEndControlPositionWidget = new PositionWidget(mItem->mapToScene(mItem->points()[2]->pos()));
		connect(mCurveEndControlPositionWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleItemGeometryChange()));
	}

	// Rect widgets
	if (ellipseItem || pathItem || rectItem || textEllipseItem || textRectItem)
	{
		mRectTopLeftWidget = new PositionWidget(mItem->mapToScene(mItem->points()[0]->pos()));
		connect(mRectTopLeftWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleItemGeometryChange()));

		mRectBottomRightWidget = new PositionWidget(mItem->mapToScene(mItem->points()[4]->pos()));
		connect(mRectBottomRightWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleItemGeometryChange()));
	}

	if (rectItem || textRectItem)
	{
		mCornerRadiusWidget = new SizeWidget();
		if (textRectItem)
			mCornerRadiusWidget->setSize(QSizeF(textRectItem->cornerRadiusX(), textRectItem->cornerRadiusY()));
		else
			mCornerRadiusWidget->setSize(QSizeF(rectItem->cornerRadiusX(), rectItem->cornerRadiusY()));
		connect(mCornerRadiusWidget, SIGNAL(sizeChanged(const QSizeF&)), this, SLOT(handleItemGeometryChange()));
	}

	// Poly widgets
	if (polygonItem || polylineItem || textPolygonItem)
	{
		mPointPositionStackedWidget = new QStackedWidget();
		fillPointsWidgets();
	}

	// Caption widget
	if (textItem || textRectItem || textEllipseItem || textPolygonItem)
	{
		mCaptionEdit = new QTextEdit();
		mCaptionEdit->setMaximumHeight(QFontMetrics(mCaptionEdit->font()).height() * 6 + 8);
		if (textRectItem) mCaptionEdit->setPlainText(textRectItem->caption());
		else if (textEllipseItem) mCaptionEdit->setPlainText(textEllipseItem->caption());
		else if (textPolygonItem) mCaptionEdit->setPlainText(textPolygonItem->caption());
		else mCaptionEdit->setPlainText(textItem->caption());
		connect(mCaptionEdit, SIGNAL(textChanged()), this, SLOT(handleItemGeometryChange()));
	}
}

void DynamicPropertiesWidget::createStyleWidgets()
{
	QVector<bool> allItemsHaveProperty(DrawingItemStyle::NumberOfProperties, true);
	QVector<bool> propertiesMatch(DrawingItemStyle::NumberOfProperties, true);
	QVector<QVariant> propertyValue(DrawingItemStyle::NumberOfProperties, QVariant());

	// Check for style properties within selected items
	for(int i = 0; i < DrawingItemStyle::NumberOfProperties; i++)
	{
		for(auto itemIter = mItems.begin(); allItemsHaveProperty[i] && itemIter != mItems.end(); itemIter++)
		{
			allItemsHaveProperty[i] = (*itemIter)->style()->hasValue((DrawingItemStyle::Property)i);
			if (allItemsHaveProperty[i])
			{
				if (propertyValue[i].isValid())
				{
					propertiesMatch[i] = (propertiesMatch[i] &&
						propertyValue[i] == (*itemIter)->style()->value((DrawingItemStyle::Property)i));
				}
				else propertyValue[i] = (*itemIter)->style()->value((DrawingItemStyle::Property)i);
			}
		}
	}

	// Pen / Brush widgets
	if (allItemsHaveProperty[DrawingItemStyle::PenStyle])
	{
		mPenStyleCombo = new PenStyleCombo((Qt::PenStyle)propertyValue[DrawingItemStyle::PenStyle].toUInt());
		mPenStyleCombo->setEnabled(propertiesMatch[DrawingItemStyle::PenStyle]);
		connect(mPenStyleCombo, SIGNAL(activated(int)), this, SLOT(handleItemsStyleChange()));
	}

	if (allItemsHaveProperty[DrawingItemStyle::PenWidth])
	{
		mPenWidthEdit = new SizeEdit(propertyValue[DrawingItemStyle::PenWidth].toDouble());
		mPenWidthEdit->setEnabled(propertiesMatch[DrawingItemStyle::PenWidth]);
		connect(mPenWidthEdit, SIGNAL(sizeChanged(qreal)), this, SLOT(handleItemsStyleChange()));
	}

	if (allItemsHaveProperty[DrawingItemStyle::PenColor])
	{
		mPenColorWidget = new ColorWidget(propertyValue[DrawingItemStyle::PenColor].value<QColor>());
		mPenColorWidget->setEnabled(propertiesMatch[DrawingItemStyle::PenColor]);
		connect(mPenColorWidget, SIGNAL(colorChanged(const QColor&)), this, SLOT(handleItemsStyleChange()));

		if (allItemsHaveProperty[DrawingItemStyle::PenOpacity])
		{
			QColor color = mPenColorWidget->color();
			color.setAlphaF(propertyValue[DrawingItemStyle::PenOpacity].toDouble());
			mPenColorWidget->setColor(color);
			mPenColorWidget->setEnabled(propertiesMatch[DrawingItemStyle::PenColor] &&
				propertiesMatch[DrawingItemStyle::PenOpacity]);
		}
	}

	if (allItemsHaveProperty[DrawingItemStyle::BrushColor])
	{
		mBrushColorWidget = new ColorWidget(propertyValue[DrawingItemStyle::BrushColor].value<QColor>());
		mBrushColorWidget->setEnabled(propertiesMatch[DrawingItemStyle::BrushColor]);
		connect(mBrushColorWidget, SIGNAL(colorChanged(const QColor&)), this, SLOT(handleItemsStyleChange()));

		if (allItemsHaveProperty[DrawingItemStyle::BrushOpacity])
		{
			QColor color = mBrushColorWidget->color();
			color.setAlphaF(propertyValue[DrawingItemStyle::BrushOpacity].toDouble());
			mBrushColorWidget->setColor(color);
			mBrushColorWidget->setEnabled(propertiesMatch[DrawingItemStyle::BrushColor] &&
				propertiesMatch[DrawingItemStyle::BrushOpacity]);
		}
	}

	// Text widgets
	if (allItemsHaveProperty[DrawingItemStyle::FontName])
	{
		mFontComboBox = new QFontComboBox();
		mFontComboBox->setCurrentFont(QFont(propertyValue[DrawingItemStyle::FontName].toString()));
		mFontComboBox->setEnabled(propertiesMatch[DrawingItemStyle::FontName]);
		connect(mFontComboBox, SIGNAL(activated(int)), this, SLOT(handleItemsStyleChange()));
	}

	if (allItemsHaveProperty[DrawingItemStyle::FontSize])
	{
		mFontSizeEdit = new SizeEdit(propertyValue[DrawingItemStyle::FontSize].toDouble());
		mFontSizeEdit->setEnabled(propertiesMatch[DrawingItemStyle::FontSize]);
		connect(mFontSizeEdit, SIGNAL(sizeChanged(qreal)), this, SLOT(handleItemsStyleChange()));
	}

	if (allItemsHaveProperty[DrawingItemStyle::FontBold] && allItemsHaveProperty[DrawingItemStyle::FontItalic] &&
		allItemsHaveProperty[DrawingItemStyle::FontUnderline] && allItemsHaveProperty[DrawingItemStyle::FontStrikeThrough])
	{
		mFontStyleWidget = new FontStyleWidget();
		mFontStyleWidget->setBold(propertyValue[DrawingItemStyle::FontBold].toBool());
		mFontStyleWidget->setItalic(propertyValue[DrawingItemStyle::FontItalic].toBool());
		mFontStyleWidget->setUnderline(propertyValue[DrawingItemStyle::FontUnderline].toBool());
		mFontStyleWidget->setStrikeThrough(propertyValue[DrawingItemStyle::FontStrikeThrough].toBool());
		mFontStyleWidget->setEnabled(propertiesMatch[DrawingItemStyle::FontBold] &&
			propertiesMatch[DrawingItemStyle::FontItalic] &&
			propertiesMatch[DrawingItemStyle::FontUnderline] &&
			propertiesMatch[DrawingItemStyle::FontStrikeThrough]);
		connect(mFontStyleWidget, SIGNAL(boldChanged(bool)), this, SLOT(handleItemsStyleChange()));
		connect(mFontStyleWidget, SIGNAL(italicChanged(bool)), this, SLOT(handleItemsStyleChange()));
		connect(mFontStyleWidget, SIGNAL(underlineChanged(bool)), this, SLOT(handleItemsStyleChange()));
		connect(mFontStyleWidget, SIGNAL(strikeThroughChanged(bool)), this, SLOT(handleItemsStyleChange()));
	}

	if (allItemsHaveProperty[DrawingItemStyle::TextHorizontalAlignment] &&
		allItemsHaveProperty[DrawingItemStyle::TextVerticalAlignment])
	{
		mTextAlignmentWidget = new TextAlignmentWidget();
		mTextAlignmentWidget->setAlignment(
			(Qt::Alignment)propertyValue[DrawingItemStyle::TextHorizontalAlignment].toUInt(),
			(Qt::Alignment)propertyValue[DrawingItemStyle::TextVerticalAlignment].toUInt());
		mTextAlignmentWidget->setEnabled(propertiesMatch[DrawingItemStyle::TextHorizontalAlignment] &&
			propertiesMatch[DrawingItemStyle::TextVerticalAlignment]);
		connect(mTextAlignmentWidget, SIGNAL(horizontalAlignmentChanged(Qt::Alignment)), this, SLOT(handleItemsStyleChange()));
		connect(mTextAlignmentWidget, SIGNAL(verticalAlignmentChanged(Qt::Alignment)), this, SLOT(handleItemsStyleChange()));
	}

	if (allItemsHaveProperty[DrawingItemStyle::TextColor])
	{
		mTextColorWidget = new ColorWidget(propertyValue[DrawingItemStyle::TextColor].value<QColor>());
		mTextColorWidget->setEnabled(propertiesMatch[DrawingItemStyle::TextColor]);
		connect(mTextColorWidget, SIGNAL(colorChanged(const QColor&)), this, SLOT(handleItemsStyleChange()));

		if (allItemsHaveProperty[DrawingItemStyle::TextOpacity])
		{
			QColor color = mTextColorWidget->color();
			color.setAlphaF(propertyValue[DrawingItemStyle::TextOpacity].toDouble());
			mTextColorWidget->setColor(color);
			mTextColorWidget->setEnabled(propertiesMatch[DrawingItemStyle::TextColor] &&
				propertiesMatch[DrawingItemStyle::TextOpacity]);
		}
	}

	// Arrow widgets
	if (allItemsHaveProperty[DrawingItemStyle::StartArrowStyle])
	{
		mStartArrowCombo = new ArrowStyleCombo(
			(DrawingItemStyle::ArrowStyle)propertyValue[DrawingItemStyle::StartArrowStyle].toUInt());
		mStartArrowCombo->setEnabled(propertiesMatch[DrawingItemStyle::StartArrowStyle]);
		connect(mStartArrowCombo, SIGNAL(activated(int)), this, SLOT(handleItemsStyleChange()));
	}

	if (allItemsHaveProperty[DrawingItemStyle::StartArrowSize])
	{
		mStartArrowSizeEdit = new SizeEdit(propertyValue[DrawingItemStyle::StartArrowSize].toDouble());
		mStartArrowSizeEdit->setEnabled(propertiesMatch[DrawingItemStyle::StartArrowSize]);
		connect(mStartArrowSizeEdit, SIGNAL(sizeChanged(qreal)), this, SLOT(handleItemsStyleChange()));
	}

	if (allItemsHaveProperty[DrawingItemStyle::EndArrowStyle])
	{
		mEndArrowCombo = new ArrowStyleCombo(
			(DrawingItemStyle::ArrowStyle)propertyValue[DrawingItemStyle::EndArrowStyle].toUInt());
		mEndArrowCombo->setEnabled(propertiesMatch[DrawingItemStyle::EndArrowStyle]);
		connect(mEndArrowCombo, SIGNAL(activated(int)), this, SLOT(handleItemsStyleChange()));
	}

	if (allItemsHaveProperty[DrawingItemStyle::EndArrowSize])
	{
		mEndArrowSizeEdit = new SizeEdit(propertyValue[DrawingItemStyle::EndArrowSize].toDouble());
		mEndArrowSizeEdit->setEnabled(propertiesMatch[DrawingItemStyle::EndArrowSize]);
		connect(mEndArrowSizeEdit, SIGNAL(sizeChanged(qreal)), this, SLOT(handleItemsStyleChange()));
	}
}

QWidget* DynamicPropertiesWidget::createItemsWidget()
{
	QWidget* mainWidget = new QWidget();
	QVBoxLayout* mainLayout = new QVBoxLayout();
	QGroupBox* groupBox = nullptr;
	QFormLayout* groupLayout = nullptr;

	if (mPositionWidget)
	{
		groupBox = new QGroupBox("Position");
		groupLayout = nullptr;
		addWidget(groupLayout, "Position:", mPositionWidget);
		groupBox->setLayout(groupLayout);
		mainLayout->addWidget(groupBox);
	}

	if (mStartPositionWidget || mEndPositionWidget)
	{
		DrawingArcItem* arcItem = dynamic_cast<DrawingArcItem*>(mItem);

		if (arcItem) groupBox = new QGroupBox("Arc");
		else groupBox = new QGroupBox("Line");
		groupLayout = nullptr;
		addWidget(groupLayout, "Start Point:", mStartPositionWidget);
		addWidget(groupLayout, "End Point:", mEndPositionWidget);
		groupBox->setLayout(groupLayout);
		mainLayout->addWidget(groupBox);
	}

	if (mCurveStartPositionWidget || mCurveStartControlPositionWidget ||
		mCurveEndPositionWidget || mCurveEndControlPositionWidget)
	{
		groupBox = new QGroupBox("Curve");
		groupLayout = nullptr;
		addWidget(groupLayout, "Start Point:", mCurveStartPositionWidget);
		addWidget(groupLayout, "Control Point:", mCurveStartControlPositionWidget);
		addWidget(groupLayout, "End Point:", mCurveEndPositionWidget);
		addWidget(groupLayout, "Control Point:", mCurveEndControlPositionWidget);
		groupBox->setLayout(groupLayout);
		mainLayout->addWidget(groupBox);
	}

	if (mRectTopLeftWidget || mRectBottomRightWidget || mCornerRadiusWidget)
	{
		DrawingEllipseItem* ellipseItem = dynamic_cast<DrawingEllipseItem*>(mItem);
		DrawingTextEllipseItem* textEllipseItem = dynamic_cast<DrawingTextEllipseItem*>(mItem);

		if (ellipseItem || textEllipseItem) groupBox = new QGroupBox("Ellipse");
		else groupBox = new QGroupBox("Rectangle");
		groupLayout = nullptr;
		addWidget(groupLayout, "Top Left:", mRectTopLeftWidget);
		addWidget(groupLayout, "Bottom Right:", mRectBottomRightWidget);
		addWidget(groupLayout, "Corner Radius:", mCornerRadiusWidget);
		groupBox->setLayout(groupLayout);
		mainLayout->addWidget(groupBox);
	}

	if (mPointPositionStackedWidget)
	{
		DrawingPolylineItem* polylineItem = dynamic_cast<DrawingPolylineItem*>(mItem);

		if (polylineItem) groupBox = new QGroupBox("Polyline");
		else groupBox = new QGroupBox("Polygon");
		QVBoxLayout* vLayout = new QVBoxLayout();
		vLayout->addWidget(mPointPositionStackedWidget);
		vLayout->setContentsMargins(0, 0, 0, 0);
		groupBox->setLayout(vLayout);
		mainLayout->addWidget(groupBox);
	}

	if (mPenStyleCombo || mPenWidthEdit || mPenColorWidget || mBrushColorWidget)
	{
		groupBox = new QGroupBox("Pen / Brush");
		groupLayout = nullptr;
		addWidget(groupLayout, "Pen Style:", mPenStyleCombo);
		addWidget(groupLayout, "Pen Width:", mPenWidthEdit);
		addWidget(groupLayout, "Pen Color:", mPenColorWidget);
		addWidget(groupLayout, "Brush Color:", mBrushColorWidget);
		groupBox->setLayout(groupLayout);
		mainLayout->addWidget(groupBox);
	}

	if (mFontComboBox || mFontSizeEdit || mFontStyleWidget || mTextAlignmentWidget ||
		mTextColorWidget || mCaptionEdit)
	{
		groupBox = new QGroupBox("Text");
		groupLayout = nullptr;
		addWidget(groupLayout, "Font:", mFontComboBox);
		addWidget(groupLayout, "Font Size:", mFontSizeEdit);
		addWidget(groupLayout, "Font Style:", mFontStyleWidget);
		addWidget(groupLayout, "Alignment:", mTextAlignmentWidget);
		addWidget(groupLayout, "Text Color:", mTextColorWidget);
		addWidget(groupLayout, "Caption:", mCaptionEdit);
		groupBox->setLayout(groupLayout);
		mainLayout->addWidget(groupBox);
	}

	if (mStartArrowCombo || mStartArrowSizeEdit || mEndArrowCombo || mEndArrowSizeEdit)
	{
		groupBox = new QGroupBox("Arrow");
		groupLayout = nullptr;
		addWidget(groupLayout, "Start Arrow:", mStartArrowCombo);
		addWidget(groupLayout, "Arrow Size:", mStartArrowSizeEdit);
		addWidget(groupLayout, "End Arrow:", mEndArrowCombo);
		addWidget(groupLayout, "Arrow Size:", mEndArrowSizeEdit);
		groupBox->setLayout(groupLayout);
		mainLayout->addWidget(groupBox);
	}

	mainLayout->addWidget(new QWidget(), 100);
	mainWidget->setLayout(mainLayout);
	return mainWidget;
}

void DynamicPropertiesWidget::addWidget(QFormLayout*& formLayout, const QString& label,	QWidget* widget)
{
	if (widget)
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
			checkBox->setChecked(widget->isEnabled());
			connect(checkBox, SIGNAL(clicked(bool)), widget, SLOT(setEnabled(bool)));
			connect(checkBox, SIGNAL(clicked(bool)), this, SLOT(handleItemsStyleChange()));

			formLayout->addRow(checkBox, widget);
			mItemStyleLabels[widget] = checkBox;
		}
		else formLayout->addRow(label, widget);

		if (formLayout->rowCount() == 1)
			formLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
	}
}

//==================================================================================================

void DynamicPropertiesWidget::fillPointsWidgets()
{
	// Clear old point position widgets
	if (mPointPositionStackedWidget)
	{
		QWidget* widget = nullptr;

		while (mPointPositionStackedWidget->count() > 0)
		{
			widget = mPointPositionStackedWidget->widget(0);
			mPointPositionStackedWidget->removeWidget(widget);
			delete widget;
		}
	}

	mPointPositionWidgets.clear();

	// Create new point position widgets
	QList<DrawingItemPoint*> points = mItem->points();
	for(int i = 0; i < points.size(); i++)
	{
		PositionWidget* posWidget = new PositionWidget(mItem->mapToScene(points[i]->pos()));
		connect(posWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleItemGeometryChange()));
		mPointPositionWidgets.append(posWidget);
	}

	// Assemble point position widgets into layout
	if (!mPointPositionWidgets.isEmpty())
	{
		QWidget* pointsWidget = new QWidget();
		QFormLayout* pointsLayout = nullptr;
		pointsLayout = nullptr;
		for(int i = 0; i < mPointPositionWidgets.size(); i++)
		{
			if (i == 0)
				addWidget(pointsLayout, "Start Point:", mPointPositionWidgets[i]);
			else if (i == mPointPositionWidgets.size() - 1)
				addWidget(pointsLayout, "End Point:", mPointPositionWidgets[i]);
			else
				addWidget(pointsLayout, "", mPointPositionWidgets[i]);

		}
		pointsWidget->setLayout(pointsLayout);
		mPointPositionStackedWidget->addWidget(pointsWidget);
	}
}
