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
}

DrawingPropertiesWidget::~DrawingPropertiesWidget() { }

//==================================================================================================

QString DrawingPropertiesWidget::propertyName() const
{
	return mPropertyName;
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
	mTopLeftCheck = new DrawingHideableCheckBox(QString("%1 Top Left:").arg(text).trimmed());
	mBottomRightCheck = nullptr;
	mSizeCheck = nullptr;

	mTopLeftWidget = new DrawingPositionWidget();
	mBottomRightWidget = nullptr;
	mSizeWidget = nullptr;

	if (useRectSize)
	{
		mSizeCheck = new DrawingHideableCheckBox(QString("%1 Size:").arg(text).trimmed());
		mSizeWidget = new DrawingSizeWidget();
	}
	else
	{
		mBottomRightCheck = new DrawingHideableCheckBox(QString("%1 Bottom Right:").arg(text).trimmed());
		mBottomRightWidget = new DrawingPositionWidget();
	}

	mLayout = new QGridLayout();
	mLayout->addWidget(mTopLeftCheck, 0, 0);
	mLayout->addWidget(mTopLeftWidget, 0, 1);
	if (mBottomRightCheck) mLayout->addWidget(mBottomRightCheck, 1, 0);
	if (mBottomRightWidget) mLayout->addWidget(mBottomRightWidget, 1, 1);
	if (mSizeCheck) mLayout->addWidget(mSizeCheck, 1, 0);
	if (mSizeWidget) mLayout->addWidget(mSizeWidget, 1, 1);
	mLayout->setColumnStretch(1, 100);
	mLayout->setContentsMargins(0, 0, 0, 0);
	setLayout(mLayout);

	connect(mTopLeftCheck, SIGNAL(clicked(bool)), mTopLeftWidget, SLOT(setEnabled(bool)));
	if (mBottomRightCheck) connect(mBottomRightCheck, SIGNAL(clicked(bool)), mBottomRightCheck, SLOT(setEnabled(bool)));
	if (mSizeCheck) connect(mSizeCheck, SIGNAL(clicked(bool)), mSizeWidget, SLOT(setEnabled(bool)));

	connect(mTopLeftCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	if (mBottomRightCheck) connect(mBottomRightCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	if (mSizeCheck) connect(mSizeCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));

	connect(mTopLeftWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleValueChange()));
	if (mBottomRightWidget) connect(mBottomRightWidget, SIGNAL(sizeChanged(const QSizeF&)), this, SLOT(handleValueChange()));
	if (mSizeWidget) connect(mSizeWidget, SIGNAL(sizeChanged(const QSizeF&)), this, SLOT(handleValueChange()));
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

void DrawingRectPropertyWidget::setLabelWidth(int width)
{
	mLayout->setColumnMinimumWidth(0, width);
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
	mColorCheck = new DrawingHideableCheckBox(QString("%1 Color:").arg(text).trimmed());
	mColorWidget = new DrawingColorWidget();

	mLayout = new QGridLayout();
	mLayout->addWidget(mColorCheck, 0, 0);
	mLayout->addWidget(mColorWidget, 0, 1);
	mLayout->setColumnStretch(1, 100);
	mLayout->setContentsMargins(0, 0, 0, 0);
	setLayout(mLayout);

	connect(mColorCheck, SIGNAL(clicked(bool)), mColorWidget, SLOT(setEnabled(bool)));
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

void DrawingBrushPropertyWidget::setLabelWidth(int width)
{
	mLayout->setColumnMinimumWidth(0, width);
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
	mGridCheck = new DrawingHideableCheckBox("Grid:");
	mGridStyleCheck = new DrawingHideableCheckBox("Grid Style:");
	mGridColorCheck = new DrawingHideableCheckBox("Grid Color:");
	mGridSpacingMajorCheck = new DrawingHideableCheckBox("Major Spacing:");
	mGridSpacingMinorCheck = new DrawingHideableCheckBox("Minor Spacing:");

	mGridEdit = new DrawingSizeEdit();
	mGridStyleCombo = new QComboBox();
	mGridColorWidget = new DrawingColorWidget();
	mGridSpacingMajorEdit = new QLineEdit();
	mGridSpacingMinorEdit = new QLineEdit();

	mGridStyleCombo->addItem("None");
	mGridStyleCombo->addItem("Dotted");
	mGridStyleCombo->addItem("Lined");
	mGridStyleCombo->addItem("Graph Paper");
	mGridSpacingMajorEdit->setValidator(new QIntValidator(1, 1E6));
	mGridSpacingMinorEdit->setValidator(new QIntValidator(1, 1E6));

	mLayout = new QGridLayout();
	mLayout->addWidget(mGridCheck, 0, 0);
	mLayout->addWidget(mGridEdit, 0, 1);
	mLayout->addWidget(mGridStyleCheck, 1, 0);
	mLayout->addWidget(mGridStyleCombo, 1, 1);
	mLayout->addWidget(mGridColorCheck, 2, 0);
	mLayout->addWidget(mGridColorWidget, 2, 1);
	mLayout->addWidget(mGridSpacingMajorCheck, 3, 0);
	mLayout->addWidget(mGridSpacingMajorEdit, 3, 1);
	mLayout->addWidget(mGridSpacingMinorCheck, 4, 0);
	mLayout->addWidget(mGridSpacingMinorEdit, 4, 1);
	mLayout->setColumnStretch(1, 100);
	mLayout->setContentsMargins(0, 0, 0, 0);
	setLayout(mLayout);

	connect(mGridCheck, SIGNAL(clicked(bool)), mGridEdit, SLOT(setEnabled(bool)));
	connect(mGridStyleCheck, SIGNAL(clicked(bool)), mGridStyleCombo, SLOT(setEnabled(bool)));
	connect(mGridColorCheck, SIGNAL(clicked(bool)), mGridColorWidget, SLOT(setEnabled(bool)));
	connect(mGridSpacingMajorCheck, SIGNAL(clicked(bool)), mGridSpacingMajorEdit, SLOT(setEnabled(bool)));
	connect(mGridSpacingMinorCheck, SIGNAL(clicked(bool)), mGridSpacingMinorEdit, SLOT(setEnabled(bool)));

	connect(mGridCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mGridStyleCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mGridColorCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mGridSpacingMajorCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mGridSpacingMinorCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));

	connect(mGridEdit, SIGNAL(sizeChanged(qreal)), this, SLOT(handleValueChange()));
	connect(mGridStyleCombo, SIGNAL(activated(int)), this, SLOT(handleValueChange()));
	connect(mGridColorWidget, SIGNAL(colorChanged(const QColor&)), this, SLOT(handleValueChange()));
	connect(mGridSpacingMajorEdit, SIGNAL(editingFinished()), this, SLOT(handleValueChange()));
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

void DrawingGridPropertiesWidget::setLabelWidth(int width)
{
	mLayout->setColumnMinimumWidth(0, width);
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
