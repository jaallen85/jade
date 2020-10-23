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
class DrawingPositionWidget;
class DrawingSizeEdit;
class DrawingSizeWidget;
class QComboBox;
class QGridLayout;
class QLineEdit;

class DrawingPropertiesWidget : public QWidget
{
	Q_OBJECT

private:
	QString mPropertyName;

public:
	DrawingPropertiesWidget(const QString& propertyName, QWidget* parent = nullptr);
	virtual ~DrawingPropertiesWidget();

	virtual void setProperties(const QHash<QString,QVariant>& properties) = 0;
	virtual void setLabelWidth(int width) = 0;
	QString propertyName() const;

	template <class T> bool containsPropertyAndCanConvert(
		const QHash<QString,QVariant>& properties, const QString& propertyNamee, T& value) const;
	bool checkForSender(QObject* sender, DrawingHideableCheckBox* check, QWidget* widget) const;

signals:
	void propertiesChanged(const QHash<QString,QVariant>& properties);
};

//==================================================================================================

class DrawingRectPropertyWidget : public DrawingPropertiesWidget
{
	Q_OBJECT

private:
	QGridLayout* mLayout;

	DrawingHideableCheckBox* mTopLeftCheck;
	DrawingHideableCheckBox* mBottomRightCheck;
	DrawingHideableCheckBox* mSizeCheck;

	DrawingPositionWidget* mTopLeftWidget;
	DrawingPositionWidget* mBottomRightWidget;
	DrawingSizeWidget* mSizeWidget;

public:
	DrawingRectPropertyWidget(const QString& propertyName, const QString& text, bool useRectSize,
		QWidget* parent = nullptr);
	~DrawingRectPropertyWidget();

	void setProperties(const QHash<QString,QVariant>& properties) override;
	void setLabelWidth(int width) override;

private slots:
	void handleValueChange();
};

//==================================================================================================

class DrawingBrushPropertyWidget : public DrawingPropertiesWidget
{
	Q_OBJECT

private:
	QGridLayout* mLayout;

	DrawingHideableCheckBox* mColorCheck;

	DrawingColorWidget* mColorWidget;

public:
	DrawingBrushPropertyWidget(const QString& propertyName, const QString& text,
		QWidget* parent = nullptr);
	~DrawingBrushPropertyWidget();

	void setProperties(const QHash<QString,QVariant>& properties) override;
	void setLabelWidth(int width) override;

private slots:
	void handleValueChange();
};

//==================================================================================================

class DrawingGridPropertiesWidget : public DrawingPropertiesWidget
{
	Q_OBJECT

private:
	QGridLayout* mLayout;

	DrawingHideableCheckBox* mGridCheck;
	DrawingHideableCheckBox* mGridStyleCheck;
	DrawingHideableCheckBox* mGridColorCheck;
	DrawingHideableCheckBox* mGridSpacingMajorCheck;
	DrawingHideableCheckBox* mGridSpacingMinorCheck;

	DrawingSizeEdit* mGridEdit;
	QComboBox* mGridStyleCombo;
	DrawingColorWidget* mGridColorWidget;
	QLineEdit* mGridSpacingMajorEdit;
	QLineEdit* mGridSpacingMinorEdit;

public:
	DrawingGridPropertiesWidget(QWidget* parent = nullptr);
	~DrawingGridPropertiesWidget();

	void setProperties(const QHash<QString,QVariant>& properties) override;
	void setLabelWidth(int width) override;

private slots:
	void handleValueChange();
};

#endif
