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
	mStackedWidget = new QStackedWidget();
	clear();

	setWidget(mStackedWidget);
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

		// search for common item style properties
		// create widgets
		// assemble layout
	}
	else if (selectedItems.size() == 1)
	{
		mItems = selectedItems;
		mItem = selectedItems.first();

		// search for common item style properties
		// create widgets
		// assemble layout
	}
	else
	{
		// create widgets
		// assemble layout
	}
}

void DynamicPropertiesWidget::setNewItem(DrawingItem* item)
{
	QList<DrawingItem*> items;
	items.append(item);
	setSelectedItems(items);
}

void DynamicPropertiesWidget::clear()
{
	QWidget* widget;

	while (mStackedWidget->count() > 0)
	{
		widget = mStackedWidget->widget(0);
		mStackedWidget->removeWidget(widget);
		delete widget;
	}

	mItems.clear();
	mItem = nullptr;

	mDiagramTopLeftWidget = nullptr;
	mDiagramRectSizeWidget = nullptr;
	mDiagramBackgroundColorWidget = nullptr;
	mDiagramGridStyleCombo = nullptr;
	mDiagramGridColorWidget = nullptr;
	mDiagramGridSpacingMajorWidget = nullptr;
	mDiagramGridSpacingMinorWidget = nullptr;

	mDefaultPenStyleCombo = nullptr;
	mDefaultPenWidthEdit = nullptr;
	mDefaultPenColorWidget = nullptr;
	mDefaultBrushColorWidget = nullptr;
	mDefaultFontComboBox = nullptr;
	mDefaultFontSizeEdit = nullptr;
	mDefaultFontStyleWidget = nullptr;
	mDefaultTextAlignmentWidget = nullptr;
	mDefaultTextColorWidget = nullptr;
	mDefaultStartArrowCombo = nullptr;
	mDefaultStartArrowSizeEdit = nullptr;
	mDefaultEndArrowCombo = nullptr;
	mDefaultEndArrowSizeEdit = nullptr;

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
		connect(mPositionWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleGeometryChange()));
	}

	// Line/arc widgets
	if (arcItem || lineItem)
	{
		mStartPositionWidget = new PositionWidget(mItem->mapToScene(mItem->points().first()->pos()));
		connect(mStartPositionWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleGeometryChange()));

		mEndPositionWidget = new PositionWidget(mItem->mapToScene(mItem->points().last()->pos()));
		connect(mEndPositionWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleGeometryChange()));
	}

	// Curve widgets
	if (curveItem)
	{
		mCurveStartPositionWidget = new PositionWidget(mItem->mapToScene(mItem->points()[0]->pos()));
		connect(mCurveStartPositionWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleGeometryChange()));

		mCurveEndPositionWidget = new PositionWidget(mItem->mapToScene(mItem->points()[1]->pos()));
		connect(mCurveEndPositionWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleGeometryChange()));

		mCurveStartControlPositionWidget = new PositionWidget(mItem->mapToScene(mItem->points()[2]->pos()));
		connect(mCurveStartControlPositionWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleGeometryChange()));

		mCurveEndControlPositionWidget = new PositionWidget(mItem->mapToScene(mItem->points()[3]->pos()));
		connect(mCurveEndControlPositionWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleGeometryChange()));
	}

	// Rect widgets
	if (ellipseItem || pathItem || rectItem || textEllipseItem || textRectItem)
	{
		mRectTopLeftWidget = new PositionWidget(mItem->mapToScene(mItem->points()[0]->pos()));
		connect(mRectTopLeftWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleGeometryChange()));

		mRectBottomRightWidget = new PositionWidget(mItem->mapToScene(mItem->points()[4]->pos()));
		connect(mRectBottomRightWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleGeometryChange()));
	}

	if (rectItem || textRectItem)
	{
		mCornerRadiusWidget = new SizeWidget();
		if (textRectItem)
			mCornerRadiusWidget->setSize(QSizeF(textRectItem->cornerRadiusX(), textRectItem->cornerRadiusY()));
		else
			mCornerRadiusWidget->setSize(QSizeF(rectItem->cornerRadiusX(), rectItem->cornerRadiusY()));
		connect(mCornerRadiusWidget, SIGNAL(sizeChanged(const QSizeF&)), this, SLOT(handleGeometryChange()));
	}

	// Poly widgets
	if (polygonItem || polylineItem || textPolygonItem)
	{
		QList<DrawingItemPoint*> points = mItem->points();
		for(int i = 0; i < points.size(); i++)
		{
			PositionWidget* posWidget = new PositionWidget(mItem->mapToScene(points[i]->pos()));
			connect(posWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleGeometryChange()));
			mPointPositionWidgets.append(posWidget);
		}
	}

	// Caption widget
	if (textItem || textRectItem || textEllipseItem || textPolygonItem)
	{
		mCaptionEdit = new QTextEdit();
		if (textRectItem) mCaptionEdit->setPlainText(textRectItem->caption());
		else if (textEllipseItem) mCaptionEdit->setPlainText(textEllipseItem->caption());
		else if (textPolygonItem) mCaptionEdit->setPlainText(textPolygonItem->caption());
		else mCaptionEdit->setPlainText(textItem->caption());
		connect(mCaptionEdit, SIGNAL(textChanged()), this, SLOT(handleGeometryChange()));
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
		connect(mPenStyleCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(handleStyleChange()));
	}

	if (allItemsHaveProperty[DrawingItemStyle::PenWidth])
	{
		mPenWidthEdit = new SizeEdit(propertyValue[DrawingItemStyle::PenWidth].toDouble());
		mPenWidthEdit->setEnabled(propertiesMatch[DrawingItemStyle::PenWidth]);
		connect(mPenWidthEdit, SIGNAL(sizeChanged(qreal)), this, SLOT(handleStyleChange()));
	}

	if (allItemsHaveProperty[DrawingItemStyle::PenColor])
	{
		mPenColorWidget = new ColorWidget(propertyValue[DrawingItemStyle::PenColor].value<QColor>());
		mPenColorWidget->setEnabled(propertiesMatch[DrawingItemStyle::PenColor]);
		connect(mPenColorWidget, SIGNAL(colorChanged(const QColor&)), this, SLOT(handleStyleChange()));

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
		connect(mBrushColorWidget, SIGNAL(colorChanged(const QColor&)), this, SLOT(handleStyleChange()));

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
		connect(mFontComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleStyleChange()));
	}

	if (allItemsHaveProperty[DrawingItemStyle::FontSize])
	{
		mFontSizeEdit = new SizeEdit(propertyValue[DrawingItemStyle::FontSize].toDouble());
		mFontSizeEdit->setEnabled(propertiesMatch[DrawingItemStyle::FontSize]);
		connect(mFontSizeEdit, SIGNAL(sizeChanged(qreal)), this, SLOT(handleStyleChange()));
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
		connect(mFontStyleWidget, SIGNAL(boldChanged(bool)), this, SLOT(handleStyleChange()));
		connect(mFontStyleWidget, SIGNAL(italicChanged(bool)), this, SLOT(handleStyleChange()));
		connect(mFontStyleWidget, SIGNAL(underlineChanged(bool)), this, SLOT(handleStyleChange()));
		connect(mFontStyleWidget, SIGNAL(strikeThroughChanged(bool)), this, SLOT(handleStyleChange()));
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
		connect(mTextAlignmentWidget, SIGNAL(horizontalAlignmentChanged(Qt::Alignment)), this, SLOT(handleStyleChange()));
		connect(mTextAlignmentWidget, SIGNAL(verticalAlignmentChanged(Qt::Alignment)), this, SLOT(handleStyleChange()));
	}

	if (allItemsHaveProperty[DrawingItemStyle::TextColor])
	{
		mTextColorWidget = new ColorWidget(propertyValue[DrawingItemStyle::TextColor].value<QColor>());
		mTextColorWidget->setEnabled(propertiesMatch[DrawingItemStyle::TextColor]);
		connect(mTextColorWidget, SIGNAL(colorChanged(const QColor&)), this, SLOT(handleStyleChange()));

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
		connect(mStartArrowCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(handleStyleChange()));
	}

	if (allItemsHaveProperty[DrawingItemStyle::StartArrowSize])
	{
		mStartArrowSizeEdit = new SizeEdit(propertyValue[DrawingItemStyle::StartArrowSize].toDouble());
		mStartArrowSizeEdit->setEnabled(propertiesMatch[DrawingItemStyle::StartArrowSize]);
		connect(mStartArrowSizeEdit, SIGNAL(sizeChanged(qreal)), this, SLOT(handleStyleChange()));
	}

	if (allItemsHaveProperty[DrawingItemStyle::EndArrowStyle])
	{
		mEndArrowCombo = new ArrowStyleCombo(
			(DrawingItemStyle::ArrowStyle)propertyValue[DrawingItemStyle::EndArrowStyle].toUInt());
		mEndArrowCombo->setEnabled(propertiesMatch[DrawingItemStyle::EndArrowStyle]);
		connect(mEndArrowCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(handleStyleChange()));
	}

	if (allItemsHaveProperty[DrawingItemStyle::EndArrowSize])
	{
		mEndArrowSizeEdit = new SizeEdit(propertyValue[DrawingItemStyle::EndArrowSize].toDouble());
		mEndArrowSizeEdit->setEnabled(propertiesMatch[DrawingItemStyle::EndArrowSize]);
		connect(mEndArrowSizeEdit, SIGNAL(sizeChanged(qreal)), this, SLOT(handleStyleChange()));
	}
}
