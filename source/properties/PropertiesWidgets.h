// PropertiesWidgets.h
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

#ifndef PROPERTIESWIDGETS_H
#define PROPERTIESWIDGETS_H

#include <QHash>
#include <QVariant>
#include <QWidget>

class ColorWidget;
class HideableCheckBox;
class DrawingItem;
class PositionWidget;
class SizeEdit;
class SizeWidget;
class QComboBox;
class QFontComboBox;
class QGridLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QStackedWidget;
class QTextEdit;
class QToolButton;

class PropertiesWidget : public QWidget
{
	Q_OBJECT

private:
	QString mPropertyName;
	QGridLayout* mLayout;

public:
	PropertiesWidget(const QString& propertyName, QWidget* parent = nullptr);
	virtual ~PropertiesWidget();

	QString propertyName() const;

	virtual void setProperties(const QHash<QString,QVariant>& properties) = 0;
	virtual QHash<QString,QVariant> properties() const = 0;

	virtual void setItems(const QList<DrawingItem*>& items);
	virtual bool isValidForItems() const;

	HideableCheckBox* addWidget(int row, const QString& text, QWidget* widget,
		Qt::Alignment checkAlignment = Qt::AlignLeft|Qt::AlignVCenter);
	void setLabelWidth(int width);

	template <class T> bool containsPropertyAndCanConvert(
		const QHash<QString,QVariant>& properties, const QString& propertyNamee, T& value) const;
	template <class T> bool allItemsContainProperty(
		const QList<DrawingItem*>& items, const QString& propertyName, QList<T>& values) const;
	bool checkForSender(QObject* sender, HideableCheckBox* check, QWidget* widget) const;

signals:
	void propertiesChanged(const QHash<QString,QVariant>& properties);
	void propertiesChanged(const QHash< DrawingItem*, QHash<QString,QVariant> >& properties);
};

//==================================================================================================

class PositionPropertyWidget : public PropertiesWidget
{
	Q_OBJECT

private:
	HideableCheckBox* mCheck;

	PositionWidget* mWidget;

	DrawingItem* mItem;
	bool mItemContainsPoint;

public:
	PositionPropertyWidget(const QString& propertyName, const QString& text,
		QWidget* parent = nullptr);
	~PositionPropertyWidget();

	void setProperties(const QHash<QString,QVariant>& properties) override;
	QHash<QString,QVariant> properties() const override;

	void setItems(const QList<DrawingItem*>& items) override;
	bool isValidForItems() const override;

private slots:
	void handleValueChange();
};

//==================================================================================================

class SizePropertyWidget : public PropertiesWidget
{
	Q_OBJECT

private:
	HideableCheckBox* mCheck;

	SizeEdit* mEdit;

	DrawingItem* mItem;
	bool mItemContainsSize;

public:
	SizePropertyWidget(const QString& propertyName, const QString& text,
		QWidget* parent = nullptr);
	~SizePropertyWidget();

	void setProperties(const QHash<QString,QVariant>& properties) override;
	QHash<QString,QVariant> properties() const override;

	void setItems(const QList<DrawingItem*>& items) override;
	bool isValidForItems() const override;

private slots:
	void handleValueChange();
};

//==================================================================================================

class LinePropertyWidget : public PropertiesWidget
{
	Q_OBJECT

private:
	HideableCheckBox* mP1Check;
	HideableCheckBox* mP2Check;

	PositionWidget* mP1Widget;
	PositionWidget* mP2Widget;

	DrawingItem* mItem;
	bool mItemContainsLine;

public:
	LinePropertyWidget(const QString& propertyName, const QString& text,
		QWidget* parent = nullptr);
	~LinePropertyWidget();

	void setProperties(const QHash<QString,QVariant>& properties) override;
	QHash<QString,QVariant> properties() const override;

	void setItems(const QList<DrawingItem*>& items) override;
	bool isValidForItems() const override;

private slots:
	void handleValueChange();
};

//==================================================================================================

class CurvePropertyWidget : public PropertiesWidget
{
	Q_OBJECT

private:
	HideableCheckBox* mP1Check;
	HideableCheckBox* mControlP1Check;
	HideableCheckBox* mControlP2Check;
	HideableCheckBox* mP2Check;

	PositionWidget* mP1Widget;
	PositionWidget* mControlP1Widget;
	PositionWidget* mControlP2Widget;
	PositionWidget* mP2Widget;

	DrawingItem* mItem;
	bool mItemContainsCurve;

public:
	CurvePropertyWidget(const QString& propertyName, const QString& text,
		QWidget* parent = nullptr);
	~CurvePropertyWidget();

	void setProperties(const QHash<QString,QVariant>& properties) override;
	QHash<QString,QVariant> properties() const override;

	void setItems(const QList<DrawingItem*>& items) override;
	bool isValidForItems() const override;

private slots:
	void handleValueChange();
};

//==================================================================================================

class RectPropertyWidget : public PropertiesWidget
{
	Q_OBJECT

private:
	HideableCheckBox* mTopLeftCheck;
	HideableCheckBox* mBottomRightCheck;
	HideableCheckBox* mSizeCheck;
	HideableCheckBox* mCenterCheck;
	HideableCheckBox* mRadiusCheck;

	PositionWidget* mTopLeftWidget;
	PositionWidget* mBottomRightWidget;
	SizeWidget* mSizeWidget;
	PositionWidget* mCenterWidget;
	SizeWidget* mRadiusWidget;

	DrawingItem* mItem;
	bool mItemContainsRect;

public:
	RectPropertyWidget(const QString& propertyName, const QString& text, bool useRectSize,
		bool useRectCenterAndRadius, QWidget* parent = nullptr);
	~RectPropertyWidget();

	void setProperties(const QHash<QString,QVariant>& properties) override;
	QHash<QString,QVariant> properties() const override;

	void setItems(const QList<DrawingItem*>& items) override;
	bool isValidForItems() const override;

private slots:
	void handleValueChange();
};

//==================================================================================================

class PolygonPropertyWidget : public PropertiesWidget
{
	Q_OBJECT

private:
	QList<PositionWidget*> mWidgets;

	DrawingItem* mItem;
	bool mItemContainsPolygon;

public:
	PolygonPropertyWidget(const QString& propertyName, QWidget* parent = nullptr);
	~PolygonPropertyWidget();

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

class TextPropertyWidget : public PropertiesWidget
{
	Q_OBJECT

private:
	HideableCheckBox* mCheck;

	QTextEdit* mEdit;

	DrawingItem* mItem;
	bool mItemContainsText;

public:
	TextPropertyWidget(const QString& propertyName, const QString& text,
		QWidget* parent = nullptr);
	~TextPropertyWidget();

	void setProperties(const QHash<QString,QVariant>& properties) override;
	QHash<QString,QVariant> properties() const override;

	void setItems(const QList<DrawingItem*>& items) override;
	bool isValidForItems() const override;

private slots:
	void handleValueChange();
};

//==================================================================================================

class PenPropertyWidget : public PropertiesWidget
{
	Q_OBJECT

private:
	HideableCheckBox* mStyleCheck;
	HideableCheckBox* mWidthCheck;
	HideableCheckBox* mColorCheck;

	QComboBox* mStyleCombo;
	SizeEdit* mWidthEdit;
	ColorWidget* mColorWidget;

	QList<DrawingItem*> mItems;
	bool mAllItemsContainPen;

public:
	PenPropertyWidget(const QString& propertyName, const QString& text,
		QWidget* parent = nullptr);
	~PenPropertyWidget();

	void setProperties(const QHash<QString,QVariant>& properties) override;
	QHash<QString,QVariant> properties() const override;

	void setItems(const QList<DrawingItem*>& items) override;
	bool isValidForItems() const override;

private slots:
	void handleValueChange();
};

//==================================================================================================

class BrushPropertyWidget : public PropertiesWidget
{
	Q_OBJECT

private:
	HideableCheckBox* mColorCheck;
	ColorWidget* mColorWidget;

	QList<DrawingItem*> mItems;
	bool mAllItemsContainBrush;

public:
	BrushPropertyWidget(const QString& propertyName, const QString& text,
		QWidget* parent = nullptr);
	~BrushPropertyWidget();

	void setProperties(const QHash<QString,QVariant>& properties) override;
	QHash<QString,QVariant> properties() const override;

	void setItems(const QList<DrawingItem*>& items) override;
	bool isValidForItems() const override;

private slots:
	void handleValueChange();
};

//==================================================================================================

class FontPropertyWidget : public PropertiesWidget
{
	Q_OBJECT

private:
	HideableCheckBox* mFamilyCheck;
	HideableCheckBox* mSizeCheck;
	HideableCheckBox* mStyleCheck;

	QFontComboBox* mFamilyCombo;
	SizeEdit* mSizeEdit;
	QWidget* mStyleWidget;
	QToolButton* mBoldButton;
	QToolButton* mItalicButton;
	QToolButton* mUnderlineButton;
	QToolButton* mStrikeThroughButton;

	QList<DrawingItem*> mItems;
	bool mAllItemsContainFont;

public:
	FontPropertyWidget(const QString& propertyName, const QString& text,
		QWidget* parent = nullptr);
	~FontPropertyWidget();

	void setProperties(const QHash<QString,QVariant>& properties) override;
	QHash<QString,QVariant> properties() const override;

	void setItems(const QList<DrawingItem*>& items) override;
	bool isValidForItems() const override;

private slots:
	void handleValueChange();
};

//==================================================================================================

class AlignmentPropertyWidget : public PropertiesWidget
{
	Q_OBJECT

private:
	HideableCheckBox* mCheck;

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
	AlignmentPropertyWidget(const QString& propertyName, const QString& text,
		QWidget* parent = nullptr);
	~AlignmentPropertyWidget();

	void setProperties(const QHash<QString,QVariant>& properties) override;
	QHash<QString,QVariant> properties() const override;

	void setItems(const QList<DrawingItem*>& items) override;
	bool isValidForItems() const override;

private slots:
	void handleValueChange();
};

//==================================================================================================

class ArrowPropertyWidget : public PropertiesWidget
{
	Q_OBJECT

private:
	HideableCheckBox* mStyleCheck;
	HideableCheckBox* mSizeCheck;

	QComboBox* mStyleCombo;
	SizeEdit* mSizeEdit;

	QList<DrawingItem*> mItems;
	bool mAllItemsContainArrow;

public:
	ArrowPropertyWidget(const QString& propertyName, const QString& text,
		QWidget* parent = nullptr);
	~ArrowPropertyWidget();

	void setProperties(const QHash<QString,QVariant>& properties) override;
	QHash<QString,QVariant> properties() const override;

	void setItems(const QList<DrawingItem*>& items) override;
	bool isValidForItems() const override;

private slots:
	void handleValueChange();
};

//==================================================================================================

class GridPropertiesWidget : public PropertiesWidget
{
	Q_OBJECT

private:
	HideableCheckBox* mGridCheck;
	HideableCheckBox* mGridStyleCheck;
	HideableCheckBox* mGridColorCheck;
	HideableCheckBox* mGridSpacingMajorCheck;
	HideableCheckBox* mGridSpacingMinorCheck;

	HideableCheckBox* mDynamicGridEnableCheck;
	HideableCheckBox* mDynamicGridCheck;

	SizeEdit* mGridEdit;
	QComboBox* mGridStyleCombo;
	ColorWidget* mGridColorWidget;
	QLineEdit* mGridSpacingMajorEdit;
	QLineEdit* mGridSpacingMinorEdit;

	QComboBox* mDynamicGridEnableCombo;
	SizeEdit* mDynamicGridEdit;

public:
	GridPropertiesWidget(QWidget* parent = nullptr);
	~GridPropertiesWidget();

	void setProperties(const QHash<QString,QVariant>& properties) override;
	QHash<QString,QVariant> properties() const override;

private slots:
	void handleValueChange();
};

//==================================================================================================

class ReferenceNamePropertyWidget : public PropertiesWidget
{
	Q_OBJECT

private:
	HideableCheckBox* mReferenceCheck;

	QLabel* mReferenceLabel;
	QPushButton* mReferenceButton;

	QList<DrawingItem*> mReferenceItems;
	DrawingItem* mItem;
	bool mItemContainsReferenceName;

public:
	ReferenceNamePropertyWidget(const QString& propertyName, const QString& text,
		QWidget* parent = nullptr);
	~ReferenceNamePropertyWidget();

	void setProperties(const QHash<QString,QVariant>& properties) override;
	QHash<QString,QVariant> properties() const override;

	void setItems(const QList<DrawingItem*>& items) override;
	bool isValidForItems() const override;

public slots:
	void updateReferenceItems(const QList<DrawingItem*>& items);

private slots:
	void handleValueChange();
	void browseForReferenceItem();
};

#endif
