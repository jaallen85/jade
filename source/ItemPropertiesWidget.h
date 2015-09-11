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

#ifndef ITEMPROPERTIESWIDGET_H
#define ITEMPROPERTIESWIDGET_H

#include <DrawingTypes.h>

class ColorPushButton;
class PositionWidget;

class ItemPropertiesWidget : public QWidget
{
	Q_OBJECT

private:
	QGroupBox* mPositionGroup;
	QFormLayout* mPositionLayout;
	PositionWidget* mPositionWidget;

	QGroupBox* mRectGroup;
	QFormLayout* mRectLayout;
	PositionWidget* mRectTopLeftWidget;
	PositionWidget* mRectBottomRightWidget;
	PositionWidget* mCornerRadiusWidget;

	QGroupBox* mStartPointGroup;
	QFormLayout* mStartPointLayout;
	PositionWidget* mStartPosWidget;
	PositionWidget* mStartControlPosWidget;
	QComboBox* mStartArrowStyleCombo;
	QLineEdit* mStartArrowSizeEdit;

	QGroupBox* mEndPointGroup;
	QFormLayout* mEndPointLayout;
	PositionWidget* mEndPosWidget;
	PositionWidget* mEndControlPosWidget;
	QComboBox* mEndArrowStyleCombo;
	QLineEdit* mEndArrowSizeEdit;

	QGroupBox* mPenBrushGroup;
	QFormLayout* mPenBrushLayout;
	QComboBox* mPenStyleCombo;
	QLineEdit* mPenWidthEdit;
	ColorPushButton* mPenColorButton;
	ColorPushButton* mBrushColorButton;

	QGroupBox* mTextGroup;
	QFormLayout* mTextLayout;
	QFontComboBox* mFontFamilyCombo;
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
	QTextEdit* mCaptionEdit;

	QGroupBox* mPolyGroup;
	QFormLayout* mPolyLayout;
	QList<PositionWidget*> mPointPosWidgets;

	QList<DrawingItem*> mItems;

public:
	ItemPropertiesWidget(const QList<DrawingItem*>& items);
	~ItemPropertiesWidget();

	QSize sizeHint() const;

public slots:
	void updateGeometry();

signals:
	void propertiesUpdated(const QMap<QString,QVariant>& properties);

private slots:
	void handlePositionChange();
	void handleRectResize();
	void handlePointResize();
	void handlePropertyChange();

private:
	QGroupBox* createPositionGroup();
	QGroupBox* createRectGroup();
	QGroupBox* createStartPointGroup();
	QGroupBox* createEndPointGroup();
	QGroupBox* createPenBrushGroup();
	QGroupBox* createTextGroup();
	QGroupBox* createPolyGroup();

	bool isSender(QObject* sender, QWidget* widget, QFormLayout* layout);
	bool checkAllItemsForProperty(const QString& propertyName, bool& propertiesMatch, QVariant& propertyValue);
	void addWidget(QFormLayout*& formLayout, const QString& label, QWidget* widget, bool checked);
	int labelWidth() const;
};

#endif
