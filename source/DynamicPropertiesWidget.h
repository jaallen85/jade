/* DynamicPropertiesWidget.h
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

#ifndef DYNAMICPROPERTIESWIDGET_H
#define DYNAMICPROPERTIESWIDGET_H

#include <DiagramWidget.h>

class PositionWidget;
class SizeWidget;
class SizeEdit;
class ColorWidget;
class ArrowStyleCombo;
class PenStyleCombo;
class GridStyleCombo;
class FontStyleWidget;
class TextAlignmentWidget;

class DynamicPropertiesWidget : public QScrollArea
{
	Q_OBJECT

protected:
	QStackedWidget* mStackedWidget;
	QList<DrawingItem*> mItems;
	DrawingItem* mItem;

	// Diagram properties
	QTabWidget* mTabWidget;

	PositionWidget* mDiagramTopLeftWidget;
	SizeWidget* mDiagramRectSizeWidget;
	ColorWidget* mDiagramBackgroundColorWidget;
	SizeEdit* mDiagramGridEdit;
	GridStyleCombo* mDiagramGridStyleCombo;
	ColorWidget* mDiagramGridColorWidget;
	QLineEdit* mDiagramGridSpacingMajorWidget;
	QLineEdit* mDiagramGridSpacingMinorWidget;

	// Item default properties
	PenStyleCombo* mDefaultPenStyleCombo;
	SizeEdit* mDefaultPenWidthEdit;
	ColorWidget* mDefaultPenColorWidget;
	ColorWidget* mDefaultBrushColorWidget;

	QFontComboBox* mDefaultFontComboBox;
	SizeEdit* mDefaultFontSizeEdit;
	FontStyleWidget* mDefaultFontStyleWidget;
	TextAlignmentWidget* mDefaultTextAlignmentWidget;
	ColorWidget* mDefaultTextColorWidget;

	ArrowStyleCombo* mDefaultStartArrowCombo;
	SizeEdit* mDefaultStartArrowSizeEdit;
	ArrowStyleCombo* mDefaultEndArrowCombo;
	SizeEdit* mDefaultEndArrowSizeEdit;

	// Item properties
	PositionWidget* mPositionWidget;

	PositionWidget* mStartPositionWidget;
	PositionWidget* mEndPositionWidget;

	PositionWidget* mCurveStartPositionWidget;
	PositionWidget* mCurveStartControlPositionWidget;
	PositionWidget* mCurveEndPositionWidget;
	PositionWidget* mCurveEndControlPositionWidget;

	PositionWidget* mRectTopLeftWidget;
	PositionWidget* mRectBottomRightWidget;
	SizeWidget* mCornerRadiusWidget;

	QList<PositionWidget*> mPointPositionWidgets;

	PenStyleCombo* mPenStyleCombo;
	SizeEdit* mPenWidthEdit;
	ColorWidget* mPenColorWidget;
	ColorWidget* mBrushColorWidget;

	QFontComboBox* mFontComboBox;
	SizeEdit* mFontSizeEdit;
	FontStyleWidget* mFontStyleWidget;
	TextAlignmentWidget* mTextAlignmentWidget;
	ColorWidget* mTextColorWidget;
	QTextEdit* mCaptionEdit;

	ArrowStyleCombo* mStartArrowCombo;
	SizeEdit* mStartArrowSizeEdit;
	ArrowStyleCombo* mEndArrowCombo;
	SizeEdit* mEndArrowSizeEdit;

	QHash<QWidget*,QCheckBox*> mItemStyleLabels;

public:
	DynamicPropertiesWidget();
	virtual ~DynamicPropertiesWidget();

	QSize sizeHint() const;
	int labelWidth() const;

public slots:
	void setSelectedItems(const QList<DrawingItem*>& selectedItems);
	void setNewItem(DrawingItem* item);
	void clear();

	void setItemGeometry(const QList<DrawingItem*>& item);
	void setItemCaption(DrawingItem* item);
	void setItemCornerRadius(DrawingItem* item);
	void setItemsStyleProperties(const QList<DrawingItem*>& items);
	void setDiagramProperties(const QHash<DiagramWidget::Property,QVariant>& properties);

signals:
	void selectedItemMoved(const QPointF& scenePos);
	void selectedItemResized(DrawingItemPoint* itemPoint, const QPointF& scenePos);
	void selectedItemCaptionChanged(const QString& newCaption);
	void selectedItemCornerRadiusChanged(qreal radiusX, qreal radiusY);
	void selectedItemsStyleChanged(const QHash<DrawingItemStyle::Property,QVariant>& properties);
	void diagramPropertiesChanged(const QHash<DiagramWidget::Property,QVariant>& properties);

private slots:
	void handleItemGeometryChange();
	void handleItemsStyleChange();
	void handleItemDefaultsChange();
	void handleDiagramChange();

private:
	QWidget* createDiagramWidget();
	QWidget* createItemDefaultsWidget();

	void createGeometryWidgets();
	void createStyleWidgets();
	QWidget* createItemsWidget();
	void addWidget(QFormLayout*& formLayout, const QString& label, QWidget* widget);
};

#endif
