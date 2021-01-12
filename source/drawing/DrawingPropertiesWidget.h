// DrawingPropertiesWidget.h
// Copyright (C) 2020  Jason Allen
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

#ifndef DRAWINGPROPERTIESWIDGET_H
#define DRAWINGPROPERTIESWIDGET_H

#include <QHash>
#include <QVariant>
#include <QWidget>

class DrawingColorWidget;
class DrawingHideableCheckBox;
class DrawingItem;
class DrawingPositionWidget;
class DrawingSizeEdit;
class DrawingSizeWidget;
class QComboBox;
class QFontComboBox;
class QGridLayout;
class QLineEdit;
class QStackedWidget;
class QTextEdit;
class QToolButton;

class DrawingPropertiesWidget : public QWidget
{
	Q_OBJECT

private:
	QString mPropertyName;
	QGridLayout* mLayout;

public:
	DrawingPropertiesWidget(const QString& propertyName, QWidget* parent = nullptr);
	virtual ~DrawingPropertiesWidget();

	QString propertyName() const;

	virtual void setProperties(const QHash<QString,QVariant>& properties) = 0;
	virtual QHash<QString,QVariant> properties() const = 0;

	virtual void setItems(const QList<DrawingItem*>& items);
	virtual bool isValidForItems() const;

	DrawingHideableCheckBox* addWidget(int row, const QString& text, QWidget* widget,
		Qt::Alignment checkAlignment = Qt::AlignLeft|Qt::AlignVCenter);
	void setLabelWidth(int width);

	template <class T> bool containsPropertyAndCanConvert(
		const QHash<QString,QVariant>& properties, const QString& propertyNamee, T& value) const;
	template <class T> bool allItemsContainProperty(
		const QList<DrawingItem*>& items, const QString& propertyName, QList<T>& values) const;
	bool checkForSender(QObject* sender, DrawingHideableCheckBox* check, QWidget* widget) const;

signals:
	void propertiesChanged(const QHash<QString,QVariant>& properties);
	void propertiesChanged(const QHash< DrawingItem*, QHash<QString,QVariant> >& properties);
};

//==================================================================================================

class DrawingPositionPropertyWidget : public DrawingPropertiesWidget
{
	Q_OBJECT

private:
	DrawingHideableCheckBox* mCheck;

	DrawingPositionWidget* mWidget;

	DrawingItem* mItem;
	bool mItemContainsPoint;

public:
	DrawingPositionPropertyWidget(const QString& propertyName, const QString& text,
		QWidget* parent = nullptr);
	~DrawingPositionPropertyWidget();

	void setProperties(const QHash<QString,QVariant>& properties) override;
	QHash<QString,QVariant> properties() const override;

	void setItems(const QList<DrawingItem*>& items) override;
	bool isValidForItems() const override;

private slots:
	void handleValueChange();
};

//==================================================================================================

class DrawingSizePropertyWidget : public DrawingPropertiesWidget
{
	Q_OBJECT

private:
	DrawingHideableCheckBox* mCheck;

	DrawingSizeEdit* mEdit;

	DrawingItem* mItem;
	bool mItemContainsSize;

public:
	DrawingSizePropertyWidget(const QString& propertyName, const QString& text,
		QWidget* parent = nullptr);
	~DrawingSizePropertyWidget();

	void setProperties(const QHash<QString,QVariant>& properties) override;
	QHash<QString,QVariant> properties() const override;

	void setItems(const QList<DrawingItem*>& items) override;
	bool isValidForItems() const override;

private slots:
	void handleValueChange();
};

//==================================================================================================

class DrawingLinePropertyWidget : public DrawingPropertiesWidget
{
	Q_OBJECT

private:
	DrawingHideableCheckBox* mP1Check;
	DrawingHideableCheckBox* mP2Check;

	DrawingPositionWidget* mP1Widget;
	DrawingPositionWidget* mP2Widget;

	DrawingItem* mItem;
	bool mItemContainsLine;

public:
	DrawingLinePropertyWidget(const QString& propertyName, const QString& text,
		QWidget* parent = nullptr);
	~DrawingLinePropertyWidget();

	void setProperties(const QHash<QString,QVariant>& properties) override;
	QHash<QString,QVariant> properties() const override;

	void setItems(const QList<DrawingItem*>& items) override;
	bool isValidForItems() const override;

private slots:
	void handleValueChange();
};

//==================================================================================================

class DrawingCurvePropertyWidget : public DrawingPropertiesWidget
{
	Q_OBJECT

private:
	DrawingHideableCheckBox* mP1Check;
	DrawingHideableCheckBox* mControlP1Check;
	DrawingHideableCheckBox* mControlP2Check;
	DrawingHideableCheckBox* mP2Check;

	DrawingPositionWidget* mP1Widget;
	DrawingPositionWidget* mControlP1Widget;
	DrawingPositionWidget* mControlP2Widget;
	DrawingPositionWidget* mP2Widget;

	DrawingItem* mItem;
	bool mItemContainsCurve;

public:
	DrawingCurvePropertyWidget(const QString& propertyName, const QString& text,
		QWidget* parent = nullptr);
	~DrawingCurvePropertyWidget();

	void setProperties(const QHash<QString,QVariant>& properties) override;
	QHash<QString,QVariant> properties() const override;

	void setItems(const QList<DrawingItem*>& items) override;
	bool isValidForItems() const override;

private slots:
	void handleValueChange();
};

//==================================================================================================

class DrawingRectPropertyWidget : public DrawingPropertiesWidget
{
	Q_OBJECT

private:
	DrawingHideableCheckBox* mTopLeftCheck;
	DrawingHideableCheckBox* mBottomRightCheck;
	DrawingHideableCheckBox* mSizeCheck;
	DrawingHideableCheckBox* mCenterCheck;
	DrawingHideableCheckBox* mRadiusCheck;

	DrawingPositionWidget* mTopLeftWidget;
	DrawingPositionWidget* mBottomRightWidget;
	DrawingSizeWidget* mSizeWidget;
	DrawingPositionWidget* mCenterWidget;
	DrawingSizeWidget* mRadiusWidget;

	DrawingItem* mItem;
	bool mItemContainsRect;

public:
	DrawingRectPropertyWidget(const QString& propertyName, const QString& text, bool useRectSize,
		bool useRectCenterAndRadius, QWidget* parent = nullptr);
	~DrawingRectPropertyWidget();

	void setProperties(const QHash<QString,QVariant>& properties) override;
	QHash<QString,QVariant> properties() const override;

	void setItems(const QList<DrawingItem*>& items) override;
	bool isValidForItems() const override;

private slots:
	void handleValueChange();
};

//==================================================================================================

class DrawingPolygonPropertyWidget : public DrawingPropertiesWidget
{
	Q_OBJECT

private:
	QList<DrawingPositionWidget*> mWidgets;

	DrawingItem* mItem;
	bool mItemContainsPolygon;

public:
	DrawingPolygonPropertyWidget(const QString& propertyName, QWidget* parent = nullptr);
	~DrawingPolygonPropertyWidget();

	void setProperties(const QHash<QString,QVariant>& properties) override;
	QHash<QString,QVariant> properties() const override;

	void setItems(const QList<DrawingItem*>& items) override;
	bool isValidForItems() const override;

private slots:
	void handleValueChange();

private:
	void setPolygon(const QPolygonF& polygon);
	QPolygonF polygon() const;
};

//==================================================================================================

class DrawingTextPropertyWidget : public DrawingPropertiesWidget
{
	Q_OBJECT

private:
	DrawingHideableCheckBox* mCheck;

	QTextEdit* mEdit;

	DrawingItem* mItem;
	bool mItemContainsText;

public:
	DrawingTextPropertyWidget(const QString& propertyName, const QString& text,
		QWidget* parent = nullptr);
	~DrawingTextPropertyWidget();

	void setProperties(const QHash<QString,QVariant>& properties) override;
	QHash<QString,QVariant> properties() const override;

	void setItems(const QList<DrawingItem*>& items) override;
	bool isValidForItems() const override;

private slots:
	void handleValueChange();
};

//==================================================================================================

class DrawingPenPropertyWidget : public DrawingPropertiesWidget
{
	Q_OBJECT

private:
	DrawingHideableCheckBox* mStyleCheck;
	DrawingHideableCheckBox* mWidthCheck;
	DrawingHideableCheckBox* mColorCheck;

	QComboBox* mStyleCombo;
	DrawingSizeEdit* mWidthEdit;
	DrawingColorWidget* mColorWidget;

	QList<DrawingItem*> mItems;
	bool mAllItemsContainPen;

public:
	DrawingPenPropertyWidget(const QString& propertyName, const QString& text,
		QWidget* parent = nullptr);
	~DrawingPenPropertyWidget();

	void setProperties(const QHash<QString,QVariant>& properties) override;
	QHash<QString,QVariant> properties() const override;

	void setItems(const QList<DrawingItem*>& items) override;
	bool isValidForItems() const override;

private slots:
	void handleValueChange();
};

//==================================================================================================

class DrawingBrushPropertyWidget : public DrawingPropertiesWidget
{
	Q_OBJECT

private:
	DrawingHideableCheckBox* mColorCheck;
	DrawingColorWidget* mColorWidget;

	QList<DrawingItem*> mItems;
	bool mAllItemsContainBrush;

public:
	DrawingBrushPropertyWidget(const QString& propertyName, const QString& text,
		QWidget* parent = nullptr);
	~DrawingBrushPropertyWidget();

	void setProperties(const QHash<QString,QVariant>& properties) override;
	QHash<QString,QVariant> properties() const override;

	void setItems(const QList<DrawingItem*>& items) override;
	bool isValidForItems() const override;

private slots:
	void handleValueChange();
};

//==================================================================================================

class DrawingFontPropertyWidget : public DrawingPropertiesWidget
{
	Q_OBJECT

private:
	DrawingHideableCheckBox* mFamilyCheck;
	DrawingHideableCheckBox* mSizeCheck;
	DrawingHideableCheckBox* mStyleCheck;

	QFontComboBox* mFamilyCombo;
	DrawingSizeEdit* mSizeEdit;
	QWidget* mStyleWidget;
	QToolButton* mBoldButton;
	QToolButton* mItalicButton;
	QToolButton* mUnderlineButton;
	QToolButton* mStrikeThroughButton;

	QList<DrawingItem*> mItems;
	bool mAllItemsContainFont;

public:
	DrawingFontPropertyWidget(const QString& propertyName, const QString& text,
		QWidget* parent = nullptr);
	~DrawingFontPropertyWidget();

	void setProperties(const QHash<QString,QVariant>& properties) override;
	QHash<QString,QVariant> properties() const override;

	void setItems(const QList<DrawingItem*>& items) override;
	bool isValidForItems() const override;

private slots:
	void handleValueChange();
};

//==================================================================================================

class DrawingAlignmentPropertyWidget : public DrawingPropertiesWidget
{
	Q_OBJECT

private:
	DrawingHideableCheckBox* mCheck;

	QWidget* mAlignWidget;
	QToolButton* mLeftAlignButton;
	QToolButton* mHCenterAlignButton;
	QToolButton* mRightAlignButton;
	QToolButton* mTopAlignButton;
	QToolButton* mVCenterAlignButton;
	QToolButton* mBottomAlignButton;

	QList<DrawingItem*> mItems;
	bool mAllItemsContainAlignment;

public:
	DrawingAlignmentPropertyWidget(const QString& propertyName, const QString& text,
		QWidget* parent = nullptr);
	~DrawingAlignmentPropertyWidget();

	void setProperties(const QHash<QString,QVariant>& properties) override;
	QHash<QString,QVariant> properties() const override;

	void setItems(const QList<DrawingItem*>& items) override;
	bool isValidForItems() const override;

private slots:
	void handleValueChange();
};

//==================================================================================================

class DrawingArrowPropertyWidget : public DrawingPropertiesWidget
{
	Q_OBJECT

private:
	DrawingHideableCheckBox* mStyleCheck;
	DrawingHideableCheckBox* mSizeCheck;

	QComboBox* mStyleCombo;
	DrawingSizeEdit* mSizeEdit;

	QList<DrawingItem*> mItems;
	bool mAllItemsContainArrow;

public:
	DrawingArrowPropertyWidget(const QString& propertyName, const QString& text,
		QWidget* parent = nullptr);
	~DrawingArrowPropertyWidget();

	void setProperties(const QHash<QString,QVariant>& properties) override;
	QHash<QString,QVariant> properties() const override;

	void setItems(const QList<DrawingItem*>& items) override;
	bool isValidForItems() const override;

private slots:
	void handleValueChange();
};

//==================================================================================================

class DrawingGridPropertiesWidget : public DrawingPropertiesWidget
{
	Q_OBJECT

private:
	DrawingHideableCheckBox* mGridCheck;
	DrawingHideableCheckBox* mGridStyleCheck;
	DrawingHideableCheckBox* mGridColorCheck;
	DrawingHideableCheckBox* mGridSpacingMajorCheck;
	DrawingHideableCheckBox* mGridSpacingMinorCheck;

	DrawingHideableCheckBox* mDynamicGridEnableCheck;
	DrawingHideableCheckBox* mDynamicGridCheck;

	DrawingSizeEdit* mGridEdit;
	QComboBox* mGridStyleCombo;
	DrawingColorWidget* mGridColorWidget;
	QLineEdit* mGridSpacingMajorEdit;
	QLineEdit* mGridSpacingMinorEdit;

	QComboBox* mDynamicGridEnableCombo;
	DrawingSizeEdit* mDynamicGridEdit;

public:
	DrawingGridPropertiesWidget(QWidget* parent = nullptr);
	~DrawingGridPropertiesWidget();

	void setProperties(const QHash<QString,QVariant>& properties) override;
	QHash<QString,QVariant> properties() const override;

private slots:
	void handleValueChange();
};

#endif
