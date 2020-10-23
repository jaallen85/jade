// DrawingPropertiesWidget.cpp
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

#include "DrawingPropertiesWidget.h"
#include "DrawingHelperWidgets.h"
#include "DrawingTypes.h"
#include <QComboBox>
#include <QGridLayout>
#include <QLineEdit>

DrawingPropertiesWidget::DrawingPropertiesWidget(const QString& propertyName, QWidget* parent) : QWidget(parent)
{
	mPropertyName = propertyName;

	mLayout = new QGridLayout();
	mLayout->setColumnStretch(1, 100);
	mLayout->setContentsMargins(0, 0, 0, 0);
	setLayout(mLayout);
}

DrawingPropertiesWidget::~DrawingPropertiesWidget() { }

//==================================================================================================

QString DrawingPropertiesWidget::propertyName() const
{
	return mPropertyName;
}

//==================================================================================================

DrawingHideableCheckBox* DrawingPropertiesWidget::addWidget(int row, const QString& text, QWidget* widget)
{
	DrawingHideableCheckBox* newCheck = nullptr;

	if (widget)
	{
		newCheck = new DrawingHideableCheckBox(text);

		mLayout->addWidget(newCheck, row, 0);
		mLayout->addWidget(widget, row, 1);

		connect(newCheck, SIGNAL(clicked(bool)), widget, SLOT(setEnabled(bool)));
	}

	return newCheck;
}

void DrawingPropertiesWidget::setLabelWidth(int width)
{
	mLayout->setColumnMinimumWidth(0, width);
}

//==================================================================================================

template <class T> bool DrawingPropertiesWidget::containsPropertyAndCanConvert(
	const QHash<QString,QVariant>& properties, const QString& propertyName, T& value) const
{
	bool success = false;

	if (properties.contains(propertyName))
	{
		QVariant variant = properties.value(propertyName);
		if (variant.canConvert<T>())
		{
			value = variant.value<T>();
			success = true;
		}
	}

	return success;
}

bool DrawingPropertiesWidget::checkForSender(QObject* sender, DrawingHideableCheckBox* check,
	QWidget* widget) const
{
	return (sender && check && widget && (sender == widget ||
		(sender == check && !check->isCheckHidden() && check->isChecked())));
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingRectPropertyWidget::DrawingRectPropertyWidget(const QString& propertyName,
	const QString& text, bool useRectSize, QWidget* parent) : DrawingPropertiesWidget(propertyName, parent)
{
	mTopLeftWidget = new DrawingPositionWidget();
	mTopLeftCheck = addWidget(0, QString("%1 Top Left:").arg(text).trimmed(), mTopLeftWidget);
	connect(mTopLeftCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mTopLeftWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleValueChange()));

	if (!useRectSize)
	{
		mBottomRightWidget = new DrawingPositionWidget();
		mBottomRightCheck = addWidget(1, QString("%1 Bottom Right:").arg(text).trimmed(), mBottomRightWidget);
		connect(mBottomRightCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
		connect(mBottomRightWidget, SIGNAL(sizeChanged(const QSizeF&)), this, SLOT(handleValueChange()));

		mSizeCheck = nullptr;
		mSizeWidget = nullptr;
	}
	else
	{
		mBottomRightCheck = nullptr;
		mBottomRightWidget = nullptr;

		mSizeWidget = new DrawingSizeWidget();
		mSizeCheck = addWidget(1, QString("%1 Size:").arg(text).trimmed(), mSizeWidget);
		connect(mSizeCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
		connect(mSizeWidget, SIGNAL(sizeChanged(const QSizeF&)), this, SLOT(handleValueChange()));
	}
}

DrawingRectPropertyWidget::~DrawingRectPropertyWidget() { }

//==================================================================================================

void DrawingRectPropertyWidget::setProperties(const QHash<QString,QVariant>& properties)
{
	QRectF rect(0, 0, 8000, 6000);

	if (containsPropertyAndCanConvert<QRectF>(properties, propertyName(), rect))
	{
		mTopLeftWidget->setPosition(rect.topLeft());
		if (mBottomRightWidget) mBottomRightWidget->setPosition(rect.bottomRight());
		if (mSizeWidget) mSizeWidget->setSize(rect.size());
	}
}

//==================================================================================================

void DrawingRectPropertyWidget::handleValueChange()
{
	QHash<QString,QVariant> properties;
	QObject* sender = this->sender();

	if (checkForSender(sender, mTopLeftCheck, mTopLeftWidget) ||
		checkForSender(sender, mBottomRightCheck, mBottomRightWidget) ||
		checkForSender(sender, mSizeCheck, mSizeWidget))
	{
		QRectF rect;

		if (mBottomRightWidget)
			rect = QRectF(mTopLeftWidget->position(), mBottomRightWidget->position());
		else if (mSizeWidget)
			rect = QRectF(mTopLeftWidget->position(), mSizeWidget->size());

		properties.insert(propertyName(), rect);
	}

	if (!properties.isEmpty()) emit propertiesChanged(properties);
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingBrushPropertyWidget::DrawingBrushPropertyWidget(const QString& propertyName,
	const QString& text, QWidget* parent) : DrawingPropertiesWidget(propertyName, parent)
{
	mColorWidget = new DrawingColorWidget();
	mColorCheck = addWidget(0, QString("%1 Color:").arg(text).trimmed(), mColorWidget);
	connect(mColorCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mColorWidget, SIGNAL(colorChanged(const QColor&)), this, SLOT(handleValueChange()));
}

DrawingBrushPropertyWidget::~DrawingBrushPropertyWidget() { }

//==================================================================================================

void DrawingBrushPropertyWidget::setProperties(const QHash<QString,QVariant>& properties)
{
	QBrush brush = Qt::white;

	if (containsPropertyAndCanConvert<QBrush>(properties, propertyName(), brush))
		mColorWidget->setColor(brush.color());
}

//==================================================================================================

void DrawingBrushPropertyWidget::handleValueChange()
{
	QHash<QString,QVariant> properties;
	QObject* sender = this->sender();

	if (checkForSender(sender, mColorCheck, mColorWidget))
		properties.insert(propertyName(), QBrush(mColorWidget->color()));

	if (!properties.isEmpty()) emit propertiesChanged(properties);
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

DrawingGridPropertiesWidget::DrawingGridPropertiesWidget(QWidget* parent) : DrawingPropertiesWidget("", parent)
{
	mGridEdit = new DrawingSizeEdit();
	mGridCheck = addWidget(0, "Grid:", mGridEdit);
	connect(mGridCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mGridEdit, SIGNAL(sizeChanged(qreal)), this, SLOT(handleValueChange()));

	mGridStyleCombo = new QComboBox();
	mGridStyleCombo->addItem("None");
	mGridStyleCombo->addItem("Dotted");
	mGridStyleCombo->addItem("Lined");
	mGridStyleCombo->addItem("Graph Paper");
	mGridStyleCheck = addWidget(1, "Grid Style:", mGridStyleCombo);
	connect(mGridStyleCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mGridStyleCombo, SIGNAL(activated(int)), this, SLOT(handleValueChange()));

	mGridColorWidget = new DrawingColorWidget();
	mGridColorCheck = addWidget(2, "Grid Color:", mGridColorWidget);
	connect(mGridColorCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mGridColorWidget, SIGNAL(colorChanged(const QColor&)), this, SLOT(handleValueChange()));

	mGridSpacingMajorEdit = new QLineEdit();
	mGridSpacingMajorEdit->setValidator(new QIntValidator(1, 1E6));
	mGridSpacingMajorCheck = addWidget(3, "Major Spacing:", mGridSpacingMajorEdit);
	connect(mGridSpacingMajorCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mGridSpacingMajorEdit, SIGNAL(editingFinished()), this, SLOT(handleValueChange()));

	mGridSpacingMinorEdit = new QLineEdit();
	mGridSpacingMinorEdit->setValidator(new QIntValidator(1, 1E6));
	mGridSpacingMinorCheck = addWidget(4, "Minor Spacing:", mGridSpacingMinorEdit);
	connect(mGridSpacingMinorCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mGridSpacingMinorEdit, SIGNAL(editingFinished()), this, SLOT(handleValueChange()));
}

DrawingGridPropertiesWidget::~DrawingGridPropertiesWidget() { }

//==================================================================================================

void DrawingGridPropertiesWidget::setProperties(const QHash<QString,QVariant>& properties)
{
	qreal grid = 1;
	uint gridStyle = Drawing::GridNone;
	QBrush gridBrush = QColor(128, 128, 128);
	int gridSpacingMajor = 0, gridSpacingMinor = 0;

	if (containsPropertyAndCanConvert<qreal>(properties, "grid", grid))
		mGridEdit->setSize(grid);
	if (containsPropertyAndCanConvert<uint>(properties, "gridStyle", gridStyle))
		mGridStyleCombo->setCurrentIndex(gridStyle);
	if (containsPropertyAndCanConvert<QBrush>(properties, "gridBrush", gridBrush))
		mGridColorWidget->setColor(gridBrush.color());
	if (containsPropertyAndCanConvert<int>(properties, "gridSpacingMajor", gridSpacingMajor))
		mGridSpacingMajorEdit->setText(QString::number(gridSpacingMajor));
	if (containsPropertyAndCanConvert<int>(properties, "gridSpacingMinor", gridSpacingMinor))
		mGridSpacingMinorEdit->setText(QString::number(gridSpacingMinor));
}

//==================================================================================================

void DrawingGridPropertiesWidget::handleValueChange()
{
	QHash<QString,QVariant> properties;
	QObject* sender = this->sender();

	if (checkForSender(sender, mGridCheck, mGridEdit))
		properties.insert("grid", mGridEdit->size());
	else if (checkForSender(sender, mGridStyleCheck, mGridStyleCombo))
		properties.insert("gridStyle", (uint)mGridStyleCombo->currentIndex());
	else if (checkForSender(sender, mGridColorCheck, mGridColorWidget))
		properties.insert("gridBrush", QBrush(mGridColorWidget->color()));
	else if (checkForSender(sender, mGridSpacingMajorCheck, mGridSpacingMajorEdit))
		properties.insert("gridSpacingMajor", mGridSpacingMajorEdit->text().toInt());
	else if (checkForSender(sender, mGridSpacingMinorCheck, mGridSpacingMinorEdit))
		properties.insert("gridSpacingMinor", mGridSpacingMinorEdit->text().toInt());

	if (!properties.isEmpty()) emit propertiesChanged(properties);
}
