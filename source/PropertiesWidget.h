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

#ifndef PROPERTIESWIDGET_H
#define PROPERTIESWIDGET_H

#include <DrawingTypes.h>

class PropertiesWidget : public QFrame
{
	Q_OBJECT

private:
	// Item properties
	/*QLineEdit* mStartPosXEdit;
	QLineEdit* mStartPosYEdit;
	QComboBox* mStartArrowStyleCombo;
	QLineEdit* mStartArrowSizeEdit;

	QLineEdit* mEndPosXEdit;
	QLineEdit* mEndPosYEdit;
	QComboBox* mEndArrowStyleCombo;
	QLineEdit* mEndArrowSizeEdit;*/

	// Drawing properties

public:
	PropertiesWidget();
	~PropertiesWidget();

	QSize sizeHint() const;

public slots:
	// Show/hide widgets as needed
	void setFromItems(const QList<DrawingItem*>& items);
	void setFromItem(DrawingItem* item);

	// Just update widget values
	void updateItems(const QList<DrawingItem*>& items);
	void updateItem(DrawingItem* item);

signals:
	void itemPositionUpdated(const QPointF& pos);
	void itemPointPositionUpdated(DrawingItemPoint* itemPoint, const QPointF& pos);
	void itemPropertiesUpdated(const QMap<QString,QVariant>& properties);
	void drawingPropertiesUpdated(const QRectF& sceneRect, qreal grid, const QBrush& backgroundBrush,
		DrawingGridStyle gridStyle, const QBrush& gridBrush, int gridSpacingMajor, int gridSpacingMinor);
};

//==================================================================================================

class ColorPushButton;

class ItemPropertiesWidget : public QWidget
{
	Q_OBJECT

private:
	QGroupBox* mPositionGroup;
	QLineEdit* mPosXEdit;
	QLineEdit* mPosYEdit;

	QGroupBox* mRectGroup;
	QLineEdit* mRectLeftEdit;
	QLineEdit* mRectTopEdit;
	QLineEdit* mRectRightEdit;
	QLineEdit* mRectBottomEdit;
	QLineEdit* mRectCornerRadiusXEdit;
	QLineEdit* mRectCornerRadiusYEdit;

	QGroupBox* mStartPointGroup;
	QLineEdit* mStartPosXEdit;
	QLineEdit* mStartPosYEdit;
	QComboBox* mStartArrowStyleCombo;
	QLineEdit* mStartArrowSizeEdit;

	QGroupBox* mEndPointGroup;
	QLineEdit* mEndPosXEdit;
	QLineEdit* mEndPosYEdit;
	QComboBox* mEndArrowStyleCombo;
	QLineEdit* mEndArrowSizeEdit;

	QGroupBox* mPenBrushGroup;
	QComboBox* mPenStyleCombo;
	QLineEdit* mPenWidthEdit;
	ColorPushButton* mPenColorButton;
	ColorPushButton* mBrushColorButton;

	QGroupBox* mTextGroup;
	QFontComboBox* mFontFamilyCombo;
	QLineEdit* mFontSizeEdit;
	QWidget* mFontStyleWidget;
	QToolButton* mFontBoldButton;
	QToolButton* mFontItalicButton;
	QToolButton* mFontUnderlineButton;
	QToolButton* mFontOverlineButton;
	QToolButton* mFontStrikeOutButton;
	QComboBox* mTextAlignmentHorizontalCombo;
	QComboBox* mTextAlignmentVerticalCombo;
	ColorPushButton* mTextColorButton;
	QTextEdit* mCaptionEdit;

	//QGroupBox* mControlPointsGroup;
	//PointsListWidget* mControlPointsWidget;

	//QGroupBox* mPolyPointsGroup;
	//PointsListWidget* mPolyPointsWidget;

public:
	ItemPropertiesWidget(DrawingItem* item);
	ItemPropertiesWidget(const QList<DrawingItem*>& items);
	~ItemPropertiesWidget();

private:
	QGroupBox* createPositionGroup();
	QGroupBox* createRectGroup();
	QGroupBox* createStartPointGroup();
	QGroupBox* createEndPointGroup();
	QGroupBox* createPenBrushGroup();
	QGroupBox* createTextGroup();
	//QGroupBox* createControlPointsGroup();
	//QGroupBox* createPolyPointsGroup();
};

#endif
