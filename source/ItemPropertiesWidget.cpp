/* ItemPropertiesWidget.cpp
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

#include "ItemPropertiesWidget.h"
#include "ColorButton.h"
#include "PositionWidget.h"
#include <DrawingItemGroup.h>
#include <DrawingItemPoint.h>
#include <DrawingPathItem.h>
#include <DrawingShapeItems.h>
#include <DrawingTextItem.h>
#include <DrawingTwoPointItems.h>

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
	mStartControlPosWidget = nullptr;
	mStartArrowStyleCombo = nullptr;
	mStartArrowSizeEdit = nullptr;

	mEndPointGroup = nullptr;
	mEndPointLayout = nullptr;
	mEndPosWidget = nullptr;
	mEndControlPosWidget = nullptr;
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
	mFontStrikeOutButton = nullptr;
	mTextAlignmentWidget = nullptr;
	mLeftAlignButton = nullptr;
	mHCenterAlignButton = nullptr;
	mRightAlignButton = nullptr;
	mTopAlignButton = nullptr;
	mVCenterAlignButton = nullptr;
	mBottomAlignButton = nullptr;
	mTextColorButton = nullptr;
	mCaptionEdit = nullptr;

	mPolyGroup = nullptr;
	mPolyLayout = nullptr;

	createPositionGroup();
	createRectGroup();
	createStartPointGroup();
	createEndPointGroup();
	createPenBrushGroup();
	createTextGroup();
	createPolyGroup();

	QVBoxLayout* vLayout = new QVBoxLayout();
	if (mPositionGroup) vLayout->addWidget(mPositionGroup);
	if (mRectGroup) vLayout->addWidget(mRectGroup);
	if (mPolyGroup) vLayout->addWidget(mPolyGroup);
	if (mStartPointGroup) vLayout->addWidget(mStartPointGroup);
	if (mEndPointGroup) vLayout->addWidget(mEndPointGroup);
	if (mPenBrushGroup) vLayout->addWidget(mPenBrushGroup);
	if (mTextGroup) vLayout->addWidget(mTextGroup);
	vLayout->addWidget(new QWidget(), 100);
	setLayout(vLayout);

	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
}

ItemPropertiesWidget::~ItemPropertiesWidget() { }

//==================================================================================================

QSize ItemPropertiesWidget::sizeHint() const
{
	return QSize(280, layout()->sizeHint().height());
}

//==================================================================================================

void ItemPropertiesWidget::handlePositionChange()
{
	//mPositionWidget
}

void ItemPropertiesWidget::handleRectResize()
{
	//mRectTopLeftWidget
	//mRectBottomRightWidget
}

void ItemPropertiesWidget::handlePointResize()
{
	//mStartPosWidget
	//mStartControlPosWidget
	//mEndPosWidget
	//mEndtControlPosWidget
	//mPointPosWidgets
}

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

	//QComboBox* mStartArrowStyleCombo;
	//QLineEdit* mStartArrowSizeEdit;
	//QComboBox* mEndArrowStyleCombo;
	//QLineEdit* mEndArrowSizeEdit;

	/*QFontComboBox* mFontFamilyCombo;
	QLineEdit* mFontSizeEdit;
	QWidget* mFontStyleWidget;
	QToolButton* mFontBoldButton;
	QToolButton* mFontItalicButton;
	QToolButton* mFontUnderlineButton;
	QToolButton* mFontStrikeOutButton;
	QWidget* mTextAlignmentWidget;
	QToolButton* mLeftAlignButton;
	QToolButton* mHCenterAlignButton;
	QToolButton* mRightAlignButton;
	QToolButton* mTopAlignButton;
	QToolButton* mVCenterAlignButton;
	QToolButton* mBottomAlignButton;
	ColorPushButton* mTextColorButton;
	QTextEdit* mCaptionEdit;*/

	if (!newProperties.isEmpty()) emit propertiesUpdated(newProperties);
}

//==================================================================================================

QGroupBox* ItemPropertiesWidget::createPositionGroup()
{
	if (mItems.size() == 1)
	{
		DrawingRectResizeItem* rectItem = dynamic_cast<DrawingRectResizeItem*>(mItems.first());
		DrawingTwoPointItem* twoPointItem = dynamic_cast<DrawingTwoPointItem*>(mItems.first());
		DrawingPolylineItem* polylineItem = dynamic_cast<DrawingPolylineItem*>(mItems.first());
		DrawingPolygonItem* polygonItem = dynamic_cast<DrawingPolygonItem*>(mItems.first());

		if (polylineItem || polygonItem || !(rectItem || twoPointItem))
		{
			mPositionWidget = new PositionWidget();

			mPositionWidget->setPos(mItems.first()->pos());
			connect(mPositionWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handlePositionChange()));

			addWidget(mPositionLayout, "Position: ", mPositionWidget, true);
		}
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
			QRectF rect = rectItem->rect();

			mRectTopLeftWidget = new PositionWidget();
			mRectBottomRightWidget = new PositionWidget();

			mRectTopLeftWidget->setPos(rect.topLeft());
			mRectBottomRightWidget->setPos(rect.bottomRight());
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

QGroupBox* ItemPropertiesWidget::createStartPointGroup()
{
	bool propertiesMatch = false;
	QVariant propertyValue;

	if (mItems.size() == 1)
	{
		DrawingTwoPointItem* twoPointItem = dynamic_cast<DrawingTwoPointItem*>(mItems.first());
		DrawingPolylineItem* polylineItem = dynamic_cast<DrawingPolylineItem*>(mItems.first());

		if (twoPointItem && !polylineItem)
		{
			mStartPosWidget = new PositionWidget();

			mStartPosWidget->setPos(twoPointItem->mapToScene(twoPointItem->startPoint()->pos()));
			connect(mStartPosWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handlePointResize()));

			addWidget(mStartPointLayout, "Start Point: ", mStartPosWidget, true);
		}

		DrawingCurveItem* curveItem = dynamic_cast<DrawingCurveItem*>(mItems.first());

		if (curveItem)
		{
			mStartControlPosWidget = new PositionWidget();

			mStartControlPosWidget->setPos(curveItem->curveStartControlPos());
			connect(mStartControlPosWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handlePointResize()));

			addWidget(mStartPointLayout, "Control Point: ", mStartControlPosWidget, true);
		}
	}

	if (checkAllItemsForProperty("Start Arrow Style", propertiesMatch, propertyValue))
	{
		mStartArrowStyleCombo = new QComboBox();
		mStartArrowStyleCombo->addItem(QIcon(":/icons/arrow/arrow_none.png"), "None");
		mStartArrowStyleCombo->addItem(QIcon(":/icons/arrow/arrow_normal.png"), "Normal");
		mStartArrowStyleCombo->addItem(QIcon(":/icons/arrow/arrow_reverse.png"), "Reverse");
		mStartArrowStyleCombo->addItem(QIcon(":/icons/arrow/arrow_triangle.png"), "Triangle");
		mStartArrowStyleCombo->addItem(QIcon(":/icons/arrow/arrow_triangle_filled.png"), "Triangle Filled");
		mStartArrowStyleCombo->addItem(QIcon(":/icons/arrow/arrow_concave.png"), "Concave");
		mStartArrowStyleCombo->addItem(QIcon(":/icons/arrow/arrow_concave_filled.png"), "Concave Filled");
		mStartArrowStyleCombo->addItem(QIcon(":/icons/arrow/arrow_circle.png"), "Circle");
		mStartArrowStyleCombo->addItem(QIcon(":/icons/arrow/arrow_circle_filled.png"), "Circle Filled");
		mStartArrowStyleCombo->addItem(QIcon(":/icons/arrow/arrow_diamond.png"), "Diamond");
		mStartArrowStyleCombo->addItem(QIcon(":/icons/arrow/arrow_diamond_filled.png"), "Diamond Filled");
		mStartArrowStyleCombo->addItem(QIcon(":/icons/arrow/arrow_harpoon.png"), "Harpoon");
		mStartArrowStyleCombo->addItem(QIcon(":/icons/arrow/arrow_harpoon_mirrored.png"), "Harpoon Mirrored");
		mStartArrowStyleCombo->addItem(QIcon(":/icons/arrow/arrow_x.png"), "X");

		if (propertyValue.isValid()) mStartArrowStyleCombo->setCurrentIndex(propertyValue.toUInt());
		connect(mStartArrowStyleCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(handlePropertyChange()));

		addWidget(mStartPointLayout, "Arrow Style: ", mStartArrowStyleCombo, propertiesMatch);
	}

	if (checkAllItemsForProperty("Start Arrow Size", propertiesMatch, propertyValue))
	{
		mStartArrowSizeEdit = new QLineEdit("100");
		mStartArrowSizeEdit->setValidator(new QDoubleValidator(0, std::numeric_limits<qreal>::max(), 6));

		if (propertyValue.isValid()) mStartArrowSizeEdit->setText(QString::number(propertyValue.toDouble()));
		connect(mStartArrowSizeEdit, SIGNAL(editingFinished()), this, SLOT(handlePropertyChange()));

		addWidget(mStartPointLayout, "Arrow Size: ", mStartArrowSizeEdit, propertiesMatch);
	}

	if (mStartPointLayout)
	{
		mStartPointGroup = new QGroupBox("Start Point");
		mStartPointGroup->setLayout(mStartPointLayout);
	}

	return mStartPointGroup;
}

//==================================================================================================

QGroupBox* ItemPropertiesWidget::createEndPointGroup()
{
	bool propertiesMatch = false;
	QVariant propertyValue;

	if (mItems.size() == 1)
	{
		DrawingTwoPointItem* twoPointItem = dynamic_cast<DrawingTwoPointItem*>(mItems.first());
		DrawingPolylineItem* polylineItem = dynamic_cast<DrawingPolylineItem*>(mItems.first());

		if (twoPointItem && !polylineItem)
		{
			mEndPosWidget = new PositionWidget();

			mEndPosWidget->setPos(twoPointItem->mapToScene(twoPointItem->endPoint()->pos()));
			connect(mEndPosWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handlePointResize()));

			addWidget(mEndPointLayout, "End Point: ", mEndPosWidget, true);
		}

		DrawingCurveItem* curveItem = dynamic_cast<DrawingCurveItem*>(mItems.first());

		if (curveItem)
		{
			mEndControlPosWidget = new PositionWidget();

			mEndControlPosWidget->setPos(curveItem->curveEndControlPos());
			connect(mEndControlPosWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handlePointResize()));

			addWidget(mEndPointLayout, "Control Point: ", mEndControlPosWidget, true);
		}
	}

	if (checkAllItemsForProperty("End Arrow Style", propertiesMatch, propertyValue))
	{
		mEndArrowStyleCombo = new QComboBox();
		mEndArrowStyleCombo->addItem(QIcon(":/icons/arrow/arrow_none.png"), "None");
		mEndArrowStyleCombo->addItem(QIcon(":/icons/arrow/arrow_normal.png"), "Normal");
		mEndArrowStyleCombo->addItem(QIcon(":/icons/arrow/arrow_reverse.png"), "Reverse");
		mEndArrowStyleCombo->addItem(QIcon(":/icons/arrow/arrow_triangle.png"), "Triangle");
		mEndArrowStyleCombo->addItem(QIcon(":/icons/arrow/arrow_triangle_filled.png"), "Triangle Filled");
		mEndArrowStyleCombo->addItem(QIcon(":/icons/arrow/arrow_concave.png"), "Concave");
		mEndArrowStyleCombo->addItem(QIcon(":/icons/arrow/arrow_concave_filled.png"), "Concave Filled");
		mEndArrowStyleCombo->addItem(QIcon(":/icons/arrow/arrow_circle.png"), "Circle");
		mEndArrowStyleCombo->addItem(QIcon(":/icons/arrow/arrow_circle_filled.png"), "Circle Filled");
		mEndArrowStyleCombo->addItem(QIcon(":/icons/arrow/arrow_diamond.png"), "Diamond");
		mEndArrowStyleCombo->addItem(QIcon(":/icons/arrow/arrow_diamond_filled.png"), "Diamond Filled");
		mEndArrowStyleCombo->addItem(QIcon(":/icons/arrow/arrow_harpoon.png"), "Harpoon");
		mEndArrowStyleCombo->addItem(QIcon(":/icons/arrow/arrow_harpoon_mirrored.png"), "Harpoon Mirrored");
		mEndArrowStyleCombo->addItem(QIcon(":/icons/arrow/arrow_x.png"), "X");

		if (propertyValue.isValid()) mEndArrowStyleCombo->setCurrentIndex(propertyValue.toUInt());
		connect(mEndArrowStyleCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(handlePropertyChange()));

		addWidget(mEndPointLayout, "Arrow Style: ", mEndArrowStyleCombo, propertiesMatch);
	}

	if (checkAllItemsForProperty("End Arrow Size", propertiesMatch, propertyValue))
	{
		mEndArrowSizeEdit = new QLineEdit("100");
		mEndArrowSizeEdit->setValidator(new QDoubleValidator(0, std::numeric_limits<qreal>::max(), 6));

		if (propertyValue.isValid()) mEndArrowSizeEdit->setText(QString::number(propertyValue.toDouble()));
		connect(mEndArrowSizeEdit, SIGNAL(editingFinished()), this, SLOT(handlePropertyChange()));

		addWidget(mEndPointLayout, "Arrow Size: ", mEndArrowSizeEdit, propertiesMatch);
	}

	if (mEndPointLayout)
	{
		mEndPointGroup = new QGroupBox("End Point");
		mEndPointGroup->setLayout(mEndPointLayout);
	}

	return mEndPointGroup;
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

QGroupBox* ItemPropertiesWidget::createTextGroup()
{
	bool propertiesMatch = false;
	bool boldMatch = false, italicMatch = false, underlineMatch = false, strikeOutMatch = false;
	bool horizontalAlignMatch = false, verticalAlignMatch = false;
	QVariant propertyValue;
	QWidget* horizontalAlignWidget = nullptr;
	QWidget* verticalAlignWidget = nullptr;

	if (checkAllItemsForProperty("Font Family", propertiesMatch, propertyValue))
	{
		mFontFamilyCombo = new QFontComboBox();

		if (propertyValue.isValid()) mFontFamilyCombo->setCurrentFont(QFont(propertyValue.toString()));
		connect(mFontFamilyCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(handlePropertyChange()));

		addWidget(mTextLayout, "Font: ", mFontFamilyCombo, propertiesMatch);
	}

	if (checkAllItemsForProperty("Font Size", propertiesMatch, propertyValue))
	{
		mFontSizeEdit = new QLineEdit("100");
		mFontSizeEdit->setValidator(new QDoubleValidator(0, std::numeric_limits<qreal>::max(), 6));

		if (propertyValue.isValid()) mFontSizeEdit->setText(QString::number(propertyValue.toDouble()));
		connect(mFontSizeEdit, SIGNAL(editingFinished()), this, SLOT(handlePropertyChange()));

		addWidget(mTextLayout, "Font Size: ", mFontSizeEdit, propertiesMatch);
	}

	if (checkAllItemsForProperty("Font Bold", boldMatch, propertyValue))
	{
		mFontBoldButton = new QToolButton();
		mFontBoldButton->setIcon(QIcon(":/icons/oxygen/format-text-bold.png"));
		mFontBoldButton->setToolTip("Bold");
		mFontBoldButton->setCheckable(true);

		if (propertyValue.isValid()) mFontBoldButton->setChecked(propertyValue.toBool());
		connect(mFontBoldButton, SIGNAL(toggled(bool)), this, SLOT(handlePropertyChange()));
	}

	if (checkAllItemsForProperty("Font Italic", italicMatch, propertyValue))
	{
		mFontItalicButton = new QToolButton();
		mFontItalicButton->setIcon(QIcon(":/icons/oxygen/format-text-italic.png"));
		mFontItalicButton->setToolTip("Italic");
		mFontItalicButton->setCheckable(true);

		if (propertyValue.isValid()) mFontItalicButton->setChecked(propertyValue.toBool());
		connect(mFontItalicButton, SIGNAL(toggled(bool)), this, SLOT(handlePropertyChange()));
	}

	if (checkAllItemsForProperty("Font Underline", underlineMatch, propertyValue))
	{
		mFontUnderlineButton = new QToolButton();
		mFontUnderlineButton->setIcon(QIcon(":/icons/oxygen/format-text-underline.png"));
		mFontUnderlineButton->setToolTip("Underline");
		mFontUnderlineButton->setCheckable(true);

		if (propertyValue.isValid()) mFontUnderlineButton->setChecked(propertyValue.toBool());
		connect(mFontUnderlineButton, SIGNAL(toggled(bool)), this, SLOT(handlePropertyChange()));
	}

	if (checkAllItemsForProperty("Font Strike-Out", strikeOutMatch, propertyValue))
	{
		mFontStrikeOutButton = new QToolButton();
		mFontStrikeOutButton->setIcon(QIcon(":/icons/oxygen/format-text-strikethrough.png"));
		mFontStrikeOutButton->setToolTip("Strike-Out");
		mFontStrikeOutButton->setCheckable(true);

		if (propertyValue.isValid()) mFontStrikeOutButton->setChecked(propertyValue.toBool());
		connect(mFontStrikeOutButton, SIGNAL(toggled(bool)), this, SLOT(handlePropertyChange()));
	}

	if (mFontBoldButton || mFontItalicButton || mFontUnderlineButton || mFontStrikeOutButton)
	{
		mFontStyleWidget = new QWidget();

		QHBoxLayout* fontStyleLayout = new QHBoxLayout();
		if (mFontBoldButton) fontStyleLayout->addWidget(mFontBoldButton);
		if (mFontItalicButton) fontStyleLayout->addWidget(mFontItalicButton);
		if (mFontUnderlineButton) fontStyleLayout->addWidget(mFontUnderlineButton);
		if (mFontStrikeOutButton) fontStyleLayout->addWidget(mFontStrikeOutButton);
		fontStyleLayout->addWidget(new QWidget(), 100);
		fontStyleLayout->setSpacing(2);
		fontStyleLayout->setContentsMargins(0, 0, 0, 0);
		mFontStyleWidget->setLayout(fontStyleLayout);

		addWidget(mTextLayout, "Font Style: ", mFontStyleWidget, ((!mFontBoldButton || boldMatch) &&
			(!mFontItalicButton || italicMatch) && (!mFontUnderlineButton || underlineMatch) && (!mFontStrikeOutButton || strikeOutMatch)));
	}

	if (checkAllItemsForProperty("Text Horizontal Alignment", horizontalAlignMatch, propertyValue))
	{
		mLeftAlignButton = new QToolButton();
		mHCenterAlignButton = new QToolButton();
		mRightAlignButton = new QToolButton();

		mLeftAlignButton->setIcon(QIcon(":/icons/oxygen/align-horizontal-left.png"));
		mLeftAlignButton->setToolTip("Align Left");
		mLeftAlignButton->setCheckable(true);
		mLeftAlignButton->setAutoExclusive(true);
		mHCenterAlignButton->setIcon(QIcon(":/icons/oxygen/align-horizontal-center.png"));
		mHCenterAlignButton->setToolTip("Align Center");
		mHCenterAlignButton->setCheckable(true);
		mHCenterAlignButton->setAutoExclusive(true);
		mRightAlignButton->setIcon(QIcon(":/icons/oxygen/align-horizontal-right.png"));
		mRightAlignButton->setToolTip("Align Right");
		mRightAlignButton->setCheckable(true);
		mRightAlignButton->setAutoExclusive(true);

		connect(mLeftAlignButton, SIGNAL(toggled(bool)), this, SLOT(handlePropertyChange()));
		connect(mHCenterAlignButton, SIGNAL(toggled(bool)), this, SLOT(handlePropertyChange()));
		connect(mRightAlignButton, SIGNAL(toggled(bool)), this, SLOT(handlePropertyChange()));

		if (propertyValue.isValid())
		{
			Qt::Alignment align = (Qt::Alignment)propertyValue.toUInt();
			if (align & Qt::AlignHCenter) mHCenterAlignButton->setChecked(true);
			else if (align & Qt::AlignRight) mRightAlignButton->setChecked(true);
			else mLeftAlignButton->setChecked(true);
		}
		else mLeftAlignButton->setChecked(true);

		horizontalAlignWidget = new QWidget();
		QHBoxLayout* horizontalAlignLayout = new QHBoxLayout();
		horizontalAlignLayout->addWidget(mLeftAlignButton);
		horizontalAlignLayout->addWidget(mHCenterAlignButton);
		horizontalAlignLayout->addWidget(mRightAlignButton);
		horizontalAlignLayout->setSpacing(2);
		horizontalAlignLayout->setContentsMargins(0, 0, 0, 0);
		horizontalAlignWidget->setLayout(horizontalAlignLayout);
	}

	if (checkAllItemsForProperty("Text Vertical Alignment", verticalAlignMatch, propertyValue))
	{
		mTopAlignButton = new QToolButton();
		mVCenterAlignButton = new QToolButton();
		mBottomAlignButton = new QToolButton();

		mTopAlignButton->setIcon(QIcon(":/icons/oxygen/align-vertical-top.png"));
		mTopAlignButton->setToolTip("Align Top");
		mTopAlignButton->setCheckable(true);
		mTopAlignButton->setAutoExclusive(true);
		mVCenterAlignButton->setIcon(QIcon(":/icons/oxygen/align-vertical-center.png"));
		mVCenterAlignButton->setToolTip("Align Center");
		mVCenterAlignButton->setCheckable(true);
		mVCenterAlignButton->setAutoExclusive(true);
		mBottomAlignButton->setIcon(QIcon(":/icons/oxygen/align-vertical-bottom.png"));
		mBottomAlignButton->setToolTip("Align Bottom");
		mBottomAlignButton->setCheckable(true);
		mBottomAlignButton->setAutoExclusive(true);
		connect(mTopAlignButton, SIGNAL(toggled(bool)), this, SLOT(handlePropertyChange()));
		connect(mVCenterAlignButton, SIGNAL(toggled(bool)), this, SLOT(handlePropertyChange()));
		connect(mBottomAlignButton, SIGNAL(toggled(bool)), this, SLOT(handlePropertyChange()));

		if (propertyValue.isValid())
		{
			Qt::Alignment align = (Qt::Alignment)propertyValue.toUInt();
			if (align & Qt::AlignVCenter) mVCenterAlignButton->setChecked(true);
			else if (align & Qt::AlignBottom) mBottomAlignButton->setChecked(true);
			else mTopAlignButton->setChecked(true);
		}
		else mTopAlignButton->setChecked(true);

		verticalAlignWidget = new QWidget();
		QHBoxLayout* verticalAlignLayout = new QHBoxLayout();
		verticalAlignLayout->addWidget(mTopAlignButton);
		verticalAlignLayout->addWidget(mVCenterAlignButton);
		verticalAlignLayout->addWidget(mBottomAlignButton);
		verticalAlignLayout->setSpacing(2);
		verticalAlignLayout->setContentsMargins(0, 0, 0, 0);
		verticalAlignWidget->setLayout(verticalAlignLayout);
	}

	if (horizontalAlignWidget || verticalAlignWidget)
	{
		mTextAlignmentWidget = new QWidget();

		QFrame* separator = nullptr;

		if (horizontalAlignWidget && verticalAlignWidget)
		{
			separator = new QFrame();
			separator->setFrameStyle(QFrame::VLine | QFrame::Raised);
		}

		QHBoxLayout* textAlignmentLayout = new QHBoxLayout();
		if (horizontalAlignWidget) textAlignmentLayout->addWidget(horizontalAlignWidget);
		if (separator) textAlignmentLayout->addWidget(separator);
		if (verticalAlignWidget) textAlignmentLayout->addWidget(verticalAlignWidget);
		textAlignmentLayout->addWidget(new QWidget(), 100);
		textAlignmentLayout->setContentsMargins(0, 0, 0, 0);
		mTextAlignmentWidget->setLayout(textAlignmentLayout);

		addWidget(mTextLayout, "Text Alignment: ", mTextAlignmentWidget,
			((!horizontalAlignWidget || horizontalAlignMatch) && (!verticalAlignWidget || verticalAlignMatch)));
	}

	if (checkAllItemsForProperty("Text Color", propertiesMatch, propertyValue))
	{
		mTextColorButton = new ColorPushButton();
		mTextColorButton->setColor(Qt::black);

		if (propertyValue.isValid()) mTextColorButton->setColor(propertyValue.value<QColor>());
		connect(mTextColorButton, SIGNAL(colorChanged(const QColor&)), this, SLOT(handlePropertyChange()));

		addWidget(mTextLayout, "Text Color: ", mTextColorButton, propertiesMatch);
	}

	if (mItems.size() == 1 && checkAllItemsForProperty("Caption", propertiesMatch, propertyValue))
	{
		mCaptionEdit = new QTextEdit();

		if (propertyValue.isValid()) mCaptionEdit->setPlainText(propertyValue.toString());
		connect(mCaptionEdit, SIGNAL(textChanged()), this, SLOT(handlePropertyChange()));

		addWidget(mTextLayout, "Text: ", mCaptionEdit, propertiesMatch);
	}

	if (mTextLayout)
	{
		mTextGroup = new QGroupBox("Text");
		mTextGroup->setLayout(mTextLayout);
	}

	return mTextGroup;
}

//==================================================================================================

QGroupBox* ItemPropertiesWidget::createPolyGroup()
{
	if (mItems.size() == 1)
	{
		DrawingPolylineItem* polylineItem = dynamic_cast<DrawingPolylineItem*>(mItems.first());
		DrawingPolygonItem* polygonItem = dynamic_cast<DrawingPolygonItem*>(mItems.first());

		if (polylineItem || polygonItem)
		{
			DrawingItem* item = mItems.first();
			QList<DrawingItemPoint*> points = item->points();
			PositionWidget* posWidget = nullptr;
			QString label = "";

			if (points.size() > 1)
			{
				for(auto pointIter = points.begin(); pointIter != points.end(); pointIter++)
				{
					posWidget = new PositionWidget();

					posWidget->setPos(item->mapToScene((*pointIter)->pos()));
					connect(posWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handlePointResize()));

					if (pointIter == points.begin()) label = "First Point:";
					else if (pointIter == points.end() - 1) label = "Last Point:";
					else label = "";

					addWidget(mPolyLayout, label, posWidget, true);
					mPointPosWidgets.append(posWidget);
				}

				if (polylineItem) label = "Polyline";
				else if (polygonItem) label = "Polygon";
				else label = "Points";

				mPolyGroup = new QGroupBox(label);
				mPolyGroup->setLayout(mPolyLayout);
			}
		}
	}

	return mPolyGroup;
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
