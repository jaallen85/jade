// PropertiesWidgets.cpp
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

#include "PropertiesWidgets.h"
#include "HelperWidgets.h"
#include "ReferenceItemDialog.h"
#include <DrawingItem.h>
#include <DrawingTypes.h>
#include <QComboBox>
#include <QFontComboBox>
#include <QFontMetrics>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedWidget>
#include <QTextEdit>
#include <QToolButton>

PropertiesWidget::PropertiesWidget(const QString& propertyName, QWidget* parent) : QWidget(parent)
{
	mPropertyName = propertyName;

	mLayout = new QGridLayout();
	mLayout->setColumnStretch(1, 100);
	mLayout->setContentsMargins(0, 0, 0, 0);
	setLayout(mLayout);
}

PropertiesWidget::~PropertiesWidget() { }

//==================================================================================================

QString PropertiesWidget::propertyName() const
{
	return mPropertyName;
}

//==================================================================================================

void PropertiesWidget::setItems(const QList<DrawingItem*>& items)
{
	Q_UNUSED(items);
}

bool PropertiesWidget::isValidForItems() const
{
	return true;
}

//==================================================================================================

HideableCheckBox* PropertiesWidget::addWidget(int row, const QString& text,
	QWidget* widget, Qt::Alignment checkAlignment)
{
	HideableCheckBox* newCheck = nullptr;

	if (widget)
	{
		newCheck = new HideableCheckBox(text);

		mLayout->addWidget(newCheck, row, 0, checkAlignment);
		mLayout->addWidget(widget, row, 1);

		connect(newCheck, SIGNAL(clicked(bool)), widget, SLOT(setEnabled(bool)));
	}

	return newCheck;
}

void PropertiesWidget::setLabelWidth(int width)
{
	mLayout->setColumnMinimumWidth(0, width);
}

//==================================================================================================

template <class T> bool PropertiesWidget::containsPropertyAndCanConvert(
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

template <class T> bool PropertiesWidget::allItemsContainProperty(
	const QList<DrawingItem*>& items, const QString& propertyName, QList<T>& values) const
{
	bool allItemsContainProperty = false;

	if (!items.isEmpty())
	{
		QHash<QString,QVariant> properties;
		T value;

		allItemsContainProperty = true;
		for(auto itemIter = items.begin(); allItemsContainProperty && itemIter != items.end(); itemIter++)
		{
			allItemsContainProperty = containsPropertyAndCanConvert((*itemIter)->properties(),
				propertyName, value);
			values.append(value);
		}

		if (!allItemsContainProperty) values.clear();
	}

	return allItemsContainProperty;
}

bool PropertiesWidget::checkForSender(QObject* sender, HideableCheckBox* check,
	QWidget* widget) const
{
	return (sender && check && widget && (sender == widget ||
		(sender == check && !check->isCheckHidden() && check->isChecked())));
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

PositionPropertyWidget::PositionPropertyWidget(const QString& propertyName,
	const QString& text, QWidget* parent) : PropertiesWidget(propertyName, parent)
{
	mWidget = new PositionWidget();
	mCheck = addWidget(0, QString("%1:").arg(text).trimmed(), mWidget);
	connect(mWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleValueChange()));

	mItem = nullptr;
	mItemContainsPoint = false;
}

PositionPropertyWidget::~PositionPropertyWidget() { }

//==================================================================================================

void PositionPropertyWidget::setProperties(const QHash<QString,QVariant>& properties)
{
	QPointF point;

	if (containsPropertyAndCanConvert<QPointF>(properties, propertyName(), point))
		mWidget->setPosition(point);

	mItem = nullptr;
}

QHash<QString,QVariant> PositionPropertyWidget::properties() const
{
	QHash<QString,QVariant> properties;
	properties.insert(propertyName(), mWidget->position());
	return properties;
}

//==================================================================================================

void PositionPropertyWidget::setItems(const QList<DrawingItem*>& items)
{
	mItem = nullptr;
	mItemContainsPoint = false;

	if (items.size() == 1)
	{
		mItem = items.first();

		QHash<QString,QVariant> properties = mItem->properties();
		if (properties.contains(propertyName()))
		{
			QVariant value = properties.value(propertyName());
			mItemContainsPoint = value.canConvert<QPointF>();
			if (mItemContainsPoint)
			{
				QPointF point = value.value<QPointF>();
				mWidget->setPosition(point);
			}
		}
	}
}

bool PositionPropertyWidget::isValidForItems() const
{
	return mItemContainsPoint;
}

//==================================================================================================

void PositionPropertyWidget::handleValueChange()
{
	if (mItem)
	{
		QHash< DrawingItem*, QHash<QString,QVariant> > itemProperties;
		itemProperties.insert(mItem, properties());
		emit propertiesChanged(itemProperties);
	}
	else emit propertiesChanged(properties());
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

SizePropertyWidget::SizePropertyWidget(const QString& propertyName,
	const QString& text, QWidget* parent) : PropertiesWidget(propertyName, parent)
{
	mEdit = new SizeEdit();
	mCheck = addWidget(0, QString("%1:").arg(text).trimmed(), mEdit);
	connect(mEdit, SIGNAL(sizeChanged(qreal)), this, SLOT(handleValueChange()));

	mItem = nullptr;
	mItemContainsSize = false;
}

SizePropertyWidget::~SizePropertyWidget() { }

//==================================================================================================

void SizePropertyWidget::setProperties(const QHash<QString,QVariant>& properties)
{
	qreal size = 0;

	if (containsPropertyAndCanConvert<qreal>(properties, propertyName(), size))
		mEdit->setSize(size);

	mItem = nullptr;
}

QHash<QString,QVariant> SizePropertyWidget::properties() const
{
	QHash<QString,QVariant> properties;
	properties.insert(propertyName(), mEdit->size());
	return properties;
}

//==================================================================================================

void SizePropertyWidget::setItems(const QList<DrawingItem*>& items)
{
	mItem = nullptr;
	mItemContainsSize = false;

	if (items.size() == 1)
	{
		mItem = items.first();

		QHash<QString,QVariant> properties = mItem->properties();
		if (properties.contains(propertyName()))
		{
			QVariant value = properties.value(propertyName());
			mItemContainsSize = value.canConvert<qreal>();
			if (mItemContainsSize)
			{
				qreal size = value.value<qreal>();
				mEdit->setSize(size);
			}
		}
	}
}

bool SizePropertyWidget::isValidForItems() const
{
	return mItemContainsSize;
}

//==================================================================================================

void SizePropertyWidget::handleValueChange()
{
	if (mItem)
	{
		QHash< DrawingItem*, QHash<QString,QVariant> > itemProperties;
		itemProperties.insert(mItem, properties());
		emit propertiesChanged(itemProperties);
	}
	else emit propertiesChanged(properties());
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

LinePropertyWidget::LinePropertyWidget(const QString& propertyName,
	const QString& text, QWidget* parent) : PropertiesWidget(propertyName, parent)
{
	mP1Widget = new PositionWidget();
	mP1Check = addWidget(0, QString("%1 Start Point:").arg(text).trimmed(), mP1Widget);
	connect(mP1Widget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleValueChange()));

	mP2Widget = new PositionWidget();
	mP2Check = addWidget(1, QString("%1 End Point:").arg(text).trimmed(), mP2Widget);
	connect(mP2Widget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleValueChange()));

	mItem = nullptr;
	mItemContainsLine = false;
}

LinePropertyWidget::~LinePropertyWidget() { }

//==================================================================================================

void LinePropertyWidget::setProperties(const QHash<QString,QVariant>& properties)
{
	QLineF line;

	if (containsPropertyAndCanConvert<QLineF>(properties, propertyName(), line))
	{
		mP1Widget->setPosition(line.p1());
		mP2Widget->setPosition(line.p2());
	}

	mItem = nullptr;
}

QHash<QString,QVariant> LinePropertyWidget::properties() const
{
	QHash<QString,QVariant> properties;
	properties.insert(propertyName(), QLineF(mP1Widget->position(), mP2Widget->position()));
	return properties;
}

//==================================================================================================

void LinePropertyWidget::setItems(const QList<DrawingItem*>& items)
{
	mItem = nullptr;
	mItemContainsLine = false;

	if (items.size() == 1)
	{
		mItem = items.first();

		QHash<QString,QVariant> properties = mItem->properties();
		if (properties.contains(propertyName()))
		{
			QVariant value = properties.value(propertyName());
			mItemContainsLine = value.canConvert<QLineF>();
			if (mItemContainsLine)
			{
				QLineF line = value.value<QLineF>();
				mP1Widget->setPosition(line.p1());
				mP2Widget->setPosition(line.p2());
			}
		}
	}
}

bool LinePropertyWidget::isValidForItems() const
{
	return mItemContainsLine;
}

//==================================================================================================

void LinePropertyWidget::handleValueChange()
{
	if (mItem)
	{
		QHash< DrawingItem*, QHash<QString,QVariant> > itemProperties;
		itemProperties.insert(mItem, properties());
		emit propertiesChanged(itemProperties);
	}
	else emit propertiesChanged(properties());
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

CurvePropertyWidget::CurvePropertyWidget(const QString& propertyName,
	const QString& text, QWidget* parent) : PropertiesWidget(propertyName, parent)
{
	mP1Widget = new PositionWidget();
	mP1Check = addWidget(0, QString("%1 Start Point:").arg(text).trimmed(), mP1Widget);
	connect(mP1Widget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleValueChange()));

	mControlP1Widget = new PositionWidget();
	mControlP1Check = addWidget(1, QString("%1 Start Control Point:").arg(text).trimmed(), mControlP1Widget);
	connect(mControlP1Widget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleValueChange()));

	mControlP2Widget = new PositionWidget();
	mControlP2Check = addWidget(2, QString("%1 End Control Point:").arg(text).trimmed(), mControlP2Widget);
	connect(mControlP2Widget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleValueChange()));

	mP2Widget = new PositionWidget();
	mP2Check = addWidget(3, QString("%1 End Point:").arg(text).trimmed(), mP2Widget);
	connect(mP2Widget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleValueChange()));

	mItem = nullptr;
	mItemContainsCurve = false;
}

CurvePropertyWidget::~CurvePropertyWidget() { }

//==================================================================================================

void CurvePropertyWidget::setProperties(const QHash<QString,QVariant>& properties)
{
	QPolygonF curve;

	if (containsPropertyAndCanConvert<QPolygonF>(properties, propertyName(), curve) && curve.size() >= 4)
	{
		mP1Widget->setPosition(curve[0]);
		mControlP1Widget->setPosition(curve[1]);
		mControlP2Widget->setPosition(curve[2]);
		mP2Widget->setPosition(curve[3]);
	}

	mItem = nullptr;
}

QHash<QString,QVariant> CurvePropertyWidget::properties() const
{
	QPolygonF curve;
	curve << mP1Widget->position() << mControlP1Widget->position() <<
		mControlP2Widget->position() << mP2Widget->position();

	QHash<QString,QVariant> properties;
	properties.insert(propertyName(), curve);
	return properties;
}

//==================================================================================================

void CurvePropertyWidget::setItems(const QList<DrawingItem*>& items)
{
	mItem = nullptr;
	mItemContainsCurve = false;

	if (items.size() == 1)
	{
		mItem = items.first();

		QHash<QString,QVariant> properties = mItem->properties();
		if (properties.contains(propertyName()))
		{
			QVariant value = properties.value(propertyName());
			mItemContainsCurve = value.canConvert<QPolygonF>();
			if (mItemContainsCurve)
			{
				QPolygonF curve = value.value<QPolygonF>();
				if (curve.size() >= 4)
				{
					mP1Widget->setPosition(curve[0]);
					mControlP1Widget->setPosition(curve[1]);
					mControlP2Widget->setPosition(curve[2]);
					mP2Widget->setPosition(curve[3]);
				}
			}
		}
	}
}

bool CurvePropertyWidget::isValidForItems() const
{
	return mItemContainsCurve;
}

//==================================================================================================

void CurvePropertyWidget::handleValueChange()
{
	if (mItem)
	{
		QHash< DrawingItem*, QHash<QString,QVariant> > itemProperties;
		itemProperties.insert(mItem, properties());
		emit propertiesChanged(itemProperties);
	}
	else emit propertiesChanged(properties());
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

RectPropertyWidget::RectPropertyWidget(const QString& propertyName,
	const QString& text, bool useRectSize, bool useRectCenterAndRadius, QWidget* parent) :
	PropertiesWidget(propertyName, parent)
{
	mTopLeftWidget = new PositionWidget();
	mTopLeftCheck = addWidget(0, QString("%1 Top Left:").arg(text).trimmed(), mTopLeftWidget);
	connect(mTopLeftCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mTopLeftWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleValueChange()));

	if (!useRectSize)
	{
		mBottomRightWidget = new PositionWidget();
		mBottomRightCheck = addWidget(1, QString("%1 Bottom Right:").arg(text).trimmed(), mBottomRightWidget);
		connect(mBottomRightCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
		connect(mBottomRightWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleValueChange()));

		mSizeCheck = nullptr;
		mSizeWidget = nullptr;
	}
	else
	{
		mBottomRightCheck = nullptr;
		mBottomRightWidget = nullptr;

		mSizeWidget = new SizeWidget();
		mSizeCheck = addWidget(1, QString("%1 Size:").arg(text).trimmed(), mSizeWidget);
		connect(mSizeCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
		connect(mSizeWidget, SIGNAL(sizeChanged(const QSizeF&)), this, SLOT(handleValueChange()));
	}

	if (useRectCenterAndRadius)
	{
		mCenterWidget = new PositionWidget();
		mCenterCheck = addWidget(2, QString("%1 Center:").arg(text).trimmed(), mCenterWidget);
		connect(mCenterCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
		connect(mCenterWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleValueChange()));

		mRadiusWidget = new SizeWidget();
		mRadiusCheck = addWidget(3, QString("%1 Radius:").arg(text).trimmed(), mRadiusWidget);
		connect(mRadiusCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
		connect(mRadiusWidget, SIGNAL(sizeChanged(const QSizeF&)), this, SLOT(handleValueChange()));
	}
	else
	{
		mCenterCheck = nullptr;
		mRadiusCheck = nullptr;
		mCenterWidget = nullptr;
		mRadiusWidget = nullptr;
	}

	mItem = nullptr;
	mItemContainsRect = false;
}

RectPropertyWidget::~RectPropertyWidget() { }

//==================================================================================================

void RectPropertyWidget::setProperties(const QHash<QString,QVariant>& properties)
{
	QRectF rect(0, 0, 8000, 6000);

	if (containsPropertyAndCanConvert<QRectF>(properties, propertyName(), rect))
	{
		mTopLeftWidget->setPosition(rect.topLeft());
		if (mBottomRightWidget) mBottomRightWidget->setPosition(rect.bottomRight());
		if (mSizeWidget) mSizeWidget->setSize(rect.size());
		if (mCenterWidget) mCenterWidget->setPosition(rect.center());
		if (mRadiusWidget) mRadiusWidget->setSize(QSizeF(rect.width() / 2, rect.height() / 2));
	}

	mItem = nullptr;
}

QHash<QString,QVariant> RectPropertyWidget::properties() const
{
	QHash<QString,QVariant> properties;
	QRectF rect;

	if (mBottomRightWidget)
		rect = QRectF(mTopLeftWidget->position(), mBottomRightWidget->position());
	else if (mSizeWidget)
		rect = QRectF(mTopLeftWidget->position(), mSizeWidget->size());

	properties.insert(propertyName(), rect);
	return properties;
}

//==================================================================================================

void RectPropertyWidget::setItems(const QList<DrawingItem*>& items)
{
	mItem = nullptr;
	mItemContainsRect = false;

	if (items.size() == 1)
	{
		mItem = items.first();

		QHash<QString,QVariant> properties = mItem->properties();
		if (properties.contains(propertyName()))
		{
			QVariant value = properties.value(propertyName());
			mItemContainsRect = value.canConvert<QRectF>();
			if (mItemContainsRect)
			{
				QRectF rect = value.value<QRectF>();
				mTopLeftWidget->setPosition(rect.topLeft());
				if (mBottomRightWidget) mBottomRightWidget->setPosition(rect.bottomRight());
				if (mSizeWidget) mSizeWidget->setSize(rect.size());
				if (mCenterWidget) mCenterWidget->setPosition(rect.center());
				if (mRadiusWidget) mRadiusWidget->setSize(QSizeF(rect.width() / 2, rect.height() / 2));
			}
		}
	}
}

bool RectPropertyWidget::isValidForItems() const
{
	return mItemContainsRect;
}

//==================================================================================================

void RectPropertyWidget::handleValueChange()
{
	if (mItem)
	{
		if (checkForSender(sender(), mCenterCheck, mCenterWidget) || checkForSender(sender(), mRadiusCheck, mRadiusWidget))
		{
			QRectF rect(QPointF(0, 0), 2 * mRadiusWidget->size());
			rect.moveCenter(mCenterWidget->position());

			mTopLeftWidget->setPosition(rect.topLeft());
			if (mBottomRightWidget) mBottomRightWidget->setPosition(rect.bottomRight());
			if (mSizeWidget) mSizeWidget->setSize(rect.size());
		}

		QHash< DrawingItem*, QHash<QString,QVariant> > itemProperties;
		itemProperties.insert(mItem, properties());
		emit propertiesChanged(itemProperties);
	}
	else emit propertiesChanged(properties());
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

PolygonPropertyWidget::PolygonPropertyWidget(const QString& propertyName,
	QWidget* parent) : PropertiesWidget(propertyName, parent)
{
	mItem = nullptr;
	mItemContainsPolygon = false;
}

PolygonPropertyWidget::~PolygonPropertyWidget() { }

//==================================================================================================

void PolygonPropertyWidget::setProperties(const QHash<QString,QVariant>& properties)
{
	QPolygonF polygon;

	if (containsPropertyAndCanConvert<QPolygonF>(properties, propertyName(), polygon))
		setPolygon(polygon);

	mItem = nullptr;
}

QHash<QString,QVariant> PolygonPropertyWidget::properties() const
{
	QHash<QString,QVariant> properties;
	properties.insert(propertyName(), polygon());
	return properties;
}

//==================================================================================================

void PolygonPropertyWidget::setItems(const QList<DrawingItem*>& items)
{
	mItem = nullptr;
	mItemContainsPolygon = false;

	if (items.size() == 1)
	{
		mItem = items.first();

		QHash<QString,QVariant> properties = mItem->properties();
		if (properties.contains(propertyName()))
		{
			QVariant value = properties.value(propertyName());
			mItemContainsPolygon = value.canConvert<QPolygonF>();
			if (mItemContainsPolygon)
			{
				QPolygonF polygon = value.value<QPolygonF>();
				setPolygon(polygon);
			}
		}
	}
}

bool PolygonPropertyWidget::isValidForItems() const
{
	return mItemContainsPolygon;
}

//==================================================================================================

void PolygonPropertyWidget::handleValueChange()
{
	if (mItem)
	{
		QHash< DrawingItem*, QHash<QString,QVariant> > itemProperties;
		itemProperties.insert(mItem, properties());
		emit propertiesChanged(itemProperties);
	}
	else emit propertiesChanged(properties());
}

//==================================================================================================

void PolygonPropertyWidget::setPolygon(const QPolygonF& polygon)
{
	if (polygon.size() != mWidgets.size())
	{
		QGridLayout* layout = dynamic_cast<QGridLayout*>(this->layout());

		if (layout)
		{
			// Clear old position widgets
			QLayoutItem* child;

			while ((child = layout->takeAt(0)) != nullptr)
			{
				delete child->widget();
				delete child;
			}

			mWidgets.clear();

			// Create new position widgets
			for(auto pointIter = polygon.begin(); pointIter != polygon.end(); pointIter++)
			{
				PositionWidget* widget = new PositionWidget(*pointIter);
				connect(widget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handleValueChange()));
				mWidgets.append(widget);
			}

			// Assemble position widgets into layout
			if (!mWidgets.isEmpty())
			{
				for(int i = 0; i < mWidgets.size(); i++)
				{
					layout->addWidget(mWidgets[i], i, 1);

					if (i == 0)
						layout->addWidget(new QLabel(propertyName() == "polyline" ? "Start Point:" : "First Point:"), i, 0);
					else if (i == mWidgets.size() - 1)
						layout->addWidget(new QLabel(propertyName() == "polyline" ? "End Point:" : "Last Point:"), i, 0);
				}
			}
		}
	}

	auto widgetIter = mWidgets.begin();
	auto pointIter = polygon.begin();
	for(; widgetIter != mWidgets.end() && pointIter != polygon.end(); widgetIter++, pointIter++)
		(*widgetIter)->setPosition(*pointIter);
}

QPolygonF PolygonPropertyWidget::polygon() const
{
	QPolygonF polygon;

	for(auto widgetIter = mWidgets.begin(); widgetIter != mWidgets.end(); widgetIter++)
		polygon << (*widgetIter)->position();

	return polygon;
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

TextPropertyWidget::TextPropertyWidget(const QString& propertyName,
	const QString& text, QWidget* parent) : PropertiesWidget(propertyName, parent)
{
	mEdit = new QTextEdit();
	mEdit->setMaximumHeight(QFontMetrics(mEdit->font()).height() * 6 + 8);
	mCheck = addWidget(0, QString("%1:").arg(text).trimmed(), mEdit, (Qt::AlignLeft | Qt::AlignTop));

	connect(mEdit, SIGNAL(textChanged()), this, SLOT(handleValueChange()));

	mItem = nullptr;
	mItemContainsText = false;
}

TextPropertyWidget::~TextPropertyWidget() { }

//==================================================================================================

void TextPropertyWidget::setProperties(const QHash<QString,QVariant>& properties)
{
	QString text;

	if (containsPropertyAndCanConvert<QString>(properties, propertyName(), text))
	{
		if (mEdit->toPlainText() != text)
		{
			disconnect(mEdit, SIGNAL(textChanged()), this, SLOT(handleValueChange()));
			mEdit->setPlainText(text);
			connect(mEdit, SIGNAL(textChanged()), this, SLOT(handleValueChange()));
		}
	}

	mItem = nullptr;
}

QHash<QString,QVariant> TextPropertyWidget::properties() const
{
	QHash<QString,QVariant> properties;
	properties.insert(propertyName(), mEdit->toPlainText());
	return properties;
}

//==================================================================================================

void TextPropertyWidget::setItems(const QList<DrawingItem*>& items)
{
	mItem = nullptr;
	mItemContainsText = false;

	if (items.size() == 1)
	{
		mItem = items.first();

		QHash<QString,QVariant> properties = mItem->properties();
		if (properties.contains(propertyName()))
		{
			QVariant value = properties.value(propertyName());
			mItemContainsText = value.canConvert<QString>();
			if (mItemContainsText)
			{
				QString text = value.value<QString>();
				if (mEdit->toPlainText() != text)
				{
					disconnect(mEdit, SIGNAL(textChanged()), this, SLOT(handleValueChange()));
					mEdit->setPlainText(text);
					connect(mEdit, SIGNAL(textChanged()), this, SLOT(handleValueChange()));
				}
			}
		}
	}
}

bool TextPropertyWidget::isValidForItems() const
{
	return mItemContainsText;
}

//==================================================================================================

void TextPropertyWidget::handleValueChange()
{
	if (mItem)
	{
		QHash< DrawingItem*, QHash<QString,QVariant> > itemProperties;
		itemProperties.insert(mItem, properties());
		emit propertiesChanged(itemProperties);
	}
	else emit propertiesChanged(properties());
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

PenPropertyWidget::PenPropertyWidget(const QString& propertyName, const QString& text,
	QWidget* parent) : PropertiesWidget(propertyName, parent)
{
	mStyleCombo = new QComboBox();
	mStyleCombo->addItem("No Pen");
	mStyleCombo->addItem("Solid Line");
	mStyleCombo->addItem("Dashed Line");
	mStyleCombo->addItem("Dotted Line");
	mStyleCombo->addItem("Dash-Dotted Line");
	mStyleCombo->addItem("Dash-Dot-Dotted Line");
	mStyleCheck = addWidget(0, QString("%1 Style:").arg(text).trimmed(), mStyleCombo);
	connect(mStyleCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mStyleCombo, SIGNAL(activated(int)), this, SLOT(handleValueChange()));

	mWidthEdit = new SizeEdit();
	mWidthCheck = addWidget(1, QString("%1 Width:").arg(text).trimmed(), mWidthEdit);
	connect(mWidthCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mWidthEdit, SIGNAL(sizeChanged(qreal)), this, SLOT(handleValueChange()));

	mColorWidget = new ColorWidget();
	mColorCheck = addWidget(2, QString("%1 Color:").arg(text).trimmed(), mColorWidget);
	connect(mColorCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mColorWidget, SIGNAL(colorChanged(const QColor&)), this, SLOT(handleValueChange()));

	mAllItemsContainPen = false;
}

PenPropertyWidget::~PenPropertyWidget() { }

//==================================================================================================

void PenPropertyWidget::setProperties(const QHash<QString,QVariant>& properties)
{
	QPen pen(Qt::black, 12.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

	if (containsPropertyAndCanConvert<QPen>(properties, propertyName(), pen))
	{
		mStyleCombo->setCurrentIndex((int)pen.style());
		mWidthEdit->setSize(pen.widthF());
		mColorWidget->setColor(pen.brush().color());
	}

	mItems.clear();
}

QHash<QString,QVariant> PenPropertyWidget::properties() const
{
	QHash<QString,QVariant> properties;
	QPen pen(Qt::black, 12.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

	pen.setStyle((Qt::PenStyle)mStyleCombo->currentIndex());
	pen.setWidthF(mWidthEdit->size());
	pen.setBrush(QBrush(mColorWidget->color()));

	properties.insert(propertyName(), pen);
	return properties;
}

//==================================================================================================

void PenPropertyWidget::setItems(const QList<DrawingItem*>& items)
{
	QList<QPen> itemPens;

	mItems = items;
	mAllItemsContainPen = allItemsContainProperty<QPen>(mItems, propertyName(), itemPens);

	if (mAllItemsContainPen)
	{
		QPen firstPen = itemPens.first();
		bool multipleItems = (mItems.size() > 1);
		bool stylesMatch = true;
		bool widthsMatch = true;
		bool colorsMatch = true;
		for(auto penIter = itemPens.begin() + 1; penIter != itemPens.end(); penIter++)
		{
			stylesMatch = (stylesMatch && firstPen.style() == penIter->style());
			widthsMatch = (widthsMatch && firstPen.widthF() == penIter->widthF());
			colorsMatch = (colorsMatch && firstPen.brush().color() == penIter->brush().color());
		}

		mStyleCheck->setCheckHidden(!multipleItems);
		mWidthCheck->setCheckHidden(!multipleItems);
		mColorCheck->setCheckHidden(!multipleItems);

		mStyleCheck->setChecked(stylesMatch);
		mWidthCheck->setChecked(widthsMatch);
		mColorCheck->setChecked(colorsMatch);

		mStyleCombo->setEnabled(stylesMatch);
		mWidthEdit->setEnabled(widthsMatch);
		mColorWidget->setEnabled(colorsMatch);

		mStyleCombo->setCurrentIndex((int)firstPen.style());
		mWidthEdit->setSize(firstPen.widthF());
		mColorWidget->setColor(firstPen.brush().color());
	}
}

bool PenPropertyWidget::isValidForItems() const
{
	return mAllItemsContainPen;
}

//==================================================================================================

void PenPropertyWidget::handleValueChange()
{
	if (!mItems.isEmpty())
	{
		QList<QPen> itemPens;

		if (allItemsContainProperty<QPen>(mItems, propertyName(), itemPens))
		{
			const bool styleChange = checkForSender(sender(), mStyleCheck, mStyleCombo);
			const bool widthChange = checkForSender(sender(), mWidthCheck, mWidthEdit);
			const bool colorChange = checkForSender(sender(), mColorCheck, mColorWidget);

			if (styleChange || widthChange || colorChange)
			{
				QHash< DrawingItem*, QHash<QString,QVariant> > itemProperties;
				QHash<QString,QVariant> properties;
				QPen pen;

				auto itemIter = mItems.begin();
				auto penIter = itemPens.begin();
				for(; itemIter != mItems.end() && penIter != itemPens.end(); itemIter++, penIter++)
				{
					pen = *penIter;
					if (styleChange) pen.setStyle((Qt::PenStyle)mStyleCombo->currentIndex());
					if (widthChange) pen.setWidthF(mWidthEdit->size());
					if (colorChange) pen.setBrush(QBrush(mColorWidget->color()));

					properties.clear();
					properties.insert(propertyName(), QVariant(pen));
					itemProperties.insert(*itemIter, properties);
				}

				if (!itemProperties.isEmpty()) emit propertiesChanged(itemProperties);
			}
		}
	}
	else emit propertiesChanged(properties());
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

BrushPropertyWidget::BrushPropertyWidget(const QString& propertyName,
	const QString& text, QWidget* parent) : PropertiesWidget(propertyName, parent)
{
	mColorWidget = new ColorWidget();
	mColorCheck = addWidget(0, QString("%1 Color:").arg(text).trimmed(), mColorWidget);
	connect(mColorCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mColorWidget, SIGNAL(colorChanged(const QColor&)), this, SLOT(handleValueChange()));

	mAllItemsContainBrush = false;
}

BrushPropertyWidget::~BrushPropertyWidget() { }

//==================================================================================================

void BrushPropertyWidget::setProperties(const QHash<QString,QVariant>& properties)
{
	QBrush brush = Qt::white;

	if (containsPropertyAndCanConvert<QBrush>(properties, propertyName(), brush))
		mColorWidget->setColor(brush.color());

	mItems.clear();
}

QHash<QString,QVariant> BrushPropertyWidget::properties() const
{
	QHash<QString,QVariant> properties;
	properties.insert(propertyName(), QBrush(mColorWidget->color()));
	return properties;
}

//==================================================================================================

void BrushPropertyWidget::setItems(const QList<DrawingItem*>& items)
{
	QList<QBrush> itemBrushes;

	mItems = items;
	mAllItemsContainBrush = allItemsContainProperty<QBrush>(mItems, propertyName(), itemBrushes);

	if (mAllItemsContainBrush)
	{
		QBrush firstBrush = itemBrushes.first();
		bool multipleItems = (mItems.size() > 1);
		bool colorsMatch = true;
		for(auto brushIter = itemBrushes.begin() + 1; brushIter != itemBrushes.end(); brushIter++)
			colorsMatch = (colorsMatch && firstBrush.color() == brushIter->color());

		mColorCheck->setCheckHidden(!multipleItems);
		mColorCheck->setChecked(colorsMatch);
		mColorWidget->setEnabled(colorsMatch);
		mColorWidget->setColor(firstBrush.color());
	}
}

bool BrushPropertyWidget::isValidForItems() const
{
	return mAllItemsContainBrush;
}

//==================================================================================================

void BrushPropertyWidget::handleValueChange()
{
	if (!mItems.isEmpty())
	{
		QList<QBrush> itemBrushes;

		if (allItemsContainProperty<QBrush>(mItems, propertyName(), itemBrushes))
		{
			if (checkForSender(sender(), mColorCheck, mColorWidget))
			{
				QHash< DrawingItem*, QHash<QString,QVariant> > itemProperties;
				QHash<QString,QVariant> properties;
				QBrush brush;

				auto itemIter = mItems.begin();
				auto brushIter = itemBrushes.begin();
				for(; itemIter != mItems.end() && brushIter != itemBrushes.end(); itemIter++, brushIter++)
				{
					brush = *brushIter;
					brush.setColor(mColorWidget->color());

					properties.clear();
					properties.insert(propertyName(), QVariant(brush));
					itemProperties.insert(*itemIter, properties);
				}

				if (!itemProperties.isEmpty()) emit propertiesChanged(itemProperties);
			}
		}
	}
	else emit propertiesChanged(properties());
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

FontPropertyWidget::FontPropertyWidget(const QString& propertyName,
	const QString& text, QWidget* parent) : PropertiesWidget(propertyName, parent)
{
	mFamilyCombo = new QFontComboBox();
	mFamilyCheck = addWidget(0, QString("%1 Family:").arg(text).trimmed(), mFamilyCombo);
	connect(mFamilyCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mFamilyCombo, SIGNAL(activated(int)), this, SLOT(handleValueChange()));

	mSizeEdit = new SizeEdit();
	mSizeCheck = addWidget(1, QString("%1 Size:").arg(text).trimmed(), mSizeEdit);
	connect(mSizeCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mSizeEdit, SIGNAL(sizeChanged(qreal)), this, SLOT(handleValueChange()));

	mBoldButton = new QToolButton();
	mBoldButton->setIcon(QIcon(":/icons/oxygen/format-text-bold.png"));
	mBoldButton->setToolTip("Bold");
	mBoldButton->setCheckable(true);

	mItalicButton = new QToolButton();
	mItalicButton->setIcon(QIcon(":/icons/oxygen/format-text-italic.png"));
	mItalicButton->setToolTip("Italic");
	mItalicButton->setCheckable(true);

	mUnderlineButton = new QToolButton();
	mUnderlineButton->setIcon(QIcon(":/icons/oxygen/format-text-underline.png"));
	mUnderlineButton->setToolTip("Underline");
	mUnderlineButton->setCheckable(true);

	mStrikeThroughButton = new QToolButton();
	mStrikeThroughButton->setIcon(QIcon(":/icons/oxygen/format-text-strikethrough.png"));
	mStrikeThroughButton->setToolTip("Strike-Through");
	mStrikeThroughButton->setCheckable(true);

	mStyleWidget = new QWidget();
	QHBoxLayout* styleLayout = new QHBoxLayout();
	styleLayout->addWidget(mBoldButton);
	styleLayout->addWidget(mItalicButton);
	styleLayout->addWidget(mUnderlineButton);
	styleLayout->addWidget(mStrikeThroughButton);
	styleLayout->addWidget(new QWidget(), 100);
	styleLayout->setSpacing(2);
	styleLayout->setContentsMargins(0, 0, 0, 0);
	mStyleWidget->setLayout(styleLayout);

	mStyleCheck = addWidget(2, QString("%1 Style:").arg(text).trimmed(), mStyleWidget);
	connect(mStyleCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mBoldButton, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mItalicButton, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mUnderlineButton, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mStrikeThroughButton, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));

	mAllItemsContainFont = false;
}

FontPropertyWidget::~FontPropertyWidget() { }

//==================================================================================================

void FontPropertyWidget::setProperties(const QHash<QString,QVariant>& properties)
{
	QFont font("Arial");
	font.setPointSizeF(100.0);

	if (containsPropertyAndCanConvert<QFont>(properties, propertyName(), font))
	{
		mFamilyCombo->setCurrentFont(font);
		mSizeEdit->setSize(font.pointSizeF());
		mBoldButton->setChecked(font.bold());
		mItalicButton->setChecked(font.italic());
		mUnderlineButton->setChecked(font.underline());
		mStrikeThroughButton->setChecked(font.strikeOut());
	}

	mItems.clear();
}

QHash<QString,QVariant> FontPropertyWidget::properties() const
{
	QHash<QString,QVariant> properties;
	QFont font;

	font.setFamily(mFamilyCombo->currentFont().family());
	font.setPointSizeF(mSizeEdit->size());
	font.setBold(mBoldButton->isChecked());
	font.setItalic(mItalicButton->isChecked());
	font.setUnderline(mUnderlineButton->isChecked());
	font.setStrikeOut(mStrikeThroughButton->isChecked());

	properties.insert(propertyName(), font);
	return properties;
}

//==================================================================================================

void FontPropertyWidget::setItems(const QList<DrawingItem*>& items)
{
	QList<QFont> itemFonts;

	mItems = items;
	mAllItemsContainFont = allItemsContainProperty<QFont>(mItems, propertyName(), itemFonts);

	if (mAllItemsContainFont)
	{
		QFont firstFont = itemFonts.first();
		bool multipleItems = (mItems.size() > 1);
		bool familiesMatch = true;
		bool sizesMatch = true;
		bool stylesMatch = true;
		for(auto fontIter = itemFonts.begin() + 1; fontIter != itemFonts.end(); fontIter++)
		{
			familiesMatch = (familiesMatch && firstFont.family() == fontIter->family());
			sizesMatch = (sizesMatch && firstFont.pointSizeF() == fontIter->pointSizeF());
			stylesMatch = (stylesMatch && firstFont.bold() == fontIter->bold() &&
				firstFont.italic() == fontIter->italic() && firstFont.underline() == fontIter->underline() &&
				firstFont.strikeOut() == fontIter->strikeOut());
		}

		mFamilyCheck->setCheckHidden(!multipleItems);
		mSizeCheck->setCheckHidden(!multipleItems);
		mStyleCheck->setCheckHidden(!multipleItems);

		mFamilyCheck->setChecked(familiesMatch);
		mSizeCheck->setChecked(sizesMatch);
		mStyleCheck->setChecked(stylesMatch);

		mFamilyCombo->setEnabled(familiesMatch);
		mSizeEdit->setEnabled(sizesMatch);
		mStyleWidget->setEnabled(stylesMatch);

		mFamilyCombo->setCurrentFont(firstFont);
		mSizeEdit->setSize(firstFont.pointSizeF());
		mBoldButton->setChecked(firstFont.bold());
		mItalicButton->setChecked(firstFont.italic());
		mUnderlineButton->setChecked(firstFont.underline());
		mStrikeThroughButton->setChecked(firstFont.strikeOut());
	}
}

bool FontPropertyWidget::isValidForItems() const
{
	return mAllItemsContainFont;
}

//==================================================================================================

void FontPropertyWidget::handleValueChange()
{
	if (!mItems.isEmpty())
	{
		QList<QFont> itemFonts;

		if (allItemsContainProperty<QFont>(mItems, propertyName(), itemFonts))
		{
			const bool familyChange = checkForSender(sender(), mFamilyCheck, mFamilyCombo);
			const bool sizeChange = checkForSender(sender(), mSizeCheck, mSizeEdit);
			const bool styleChange = (checkForSender(sender(), mStyleCheck, mBoldButton) ||
				sender() == mItalicButton || sender() == mUnderlineButton || sender() == mStrikeThroughButton);

			if (familyChange || sizeChange || styleChange)
			{
				QHash< DrawingItem*, QHash<QString,QVariant> > itemProperties;
				QHash<QString,QVariant> properties;
				QFont font;

				auto itemIter = mItems.begin();
				auto fontIter = itemFonts.begin();
				for(; itemIter != mItems.end() && fontIter != itemFonts.end(); itemIter++, fontIter++)
				{
					font = *fontIter;
					if (familyChange) font.setFamily(mFamilyCombo->currentFont().family());
					if (sizeChange) font.setPointSizeF(mSizeEdit->size());
					if (styleChange)
					{
						font.setBold(mBoldButton->isChecked());
						font.setItalic(mItalicButton->isChecked());
						font.setUnderline(mUnderlineButton->isChecked());
						font.setStrikeOut(mStrikeThroughButton->isChecked());
					}

					properties.clear();
					properties.insert(propertyName(), QVariant(font));
					itemProperties.insert(*itemIter, properties);
				}

				if (!itemProperties.isEmpty()) emit propertiesChanged(itemProperties);
			}
		}
	}
	else emit propertiesChanged(properties());
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

AlignmentPropertyWidget::AlignmentPropertyWidget(const QString& propertyName,
	const QString& text, QWidget* parent) : PropertiesWidget(propertyName, parent)
{
	mLeftAlignButton = new QToolButton();
	mLeftAlignButton->setIcon(QIcon(":/icons/oxygen/align-horizontal-left.png"));
	mLeftAlignButton->setToolTip("Align Left");
	mLeftAlignButton->setCheckable(true);
	mLeftAlignButton->setAutoExclusive(true);

	mHCenterAlignButton = new QToolButton();
	mHCenterAlignButton->setIcon(QIcon(":/icons/oxygen/align-horizontal-center.png"));
	mHCenterAlignButton->setToolTip("Align Center");
	mHCenterAlignButton->setCheckable(true);
	mHCenterAlignButton->setAutoExclusive(true);
	mHCenterAlignButton->setChecked(true);

	mRightAlignButton = new QToolButton();
	mRightAlignButton->setIcon(QIcon(":/icons/oxygen/align-horizontal-right.png"));
	mRightAlignButton->setToolTip("Align Right");
	mRightAlignButton->setCheckable(true);
	mRightAlignButton->setAutoExclusive(true);

	mTopAlignButton = new QToolButton();
	mTopAlignButton->setIcon(QIcon(":/icons/oxygen/align-vertical-top.png"));
	mTopAlignButton->setToolTip("Align Top");
	mTopAlignButton->setCheckable(true);
	mTopAlignButton->setAutoExclusive(true);

	mVCenterAlignButton = new QToolButton();
	mVCenterAlignButton->setIcon(QIcon(":/icons/oxygen/align-vertical-center.png"));
	mVCenterAlignButton->setToolTip("Align Center");
	mVCenterAlignButton->setCheckable(true);
	mVCenterAlignButton->setAutoExclusive(true);
	mVCenterAlignButton->setChecked(true);

	mBottomAlignButton = new QToolButton();
	mBottomAlignButton->setIcon(QIcon(":/icons/oxygen/align-vertical-bottom.png"));
	mBottomAlignButton->setToolTip("Align Bottom");
	mBottomAlignButton->setCheckable(true);
	mBottomAlignButton->setAutoExclusive(true);

	QWidget* horizontalAlignWidget = new QWidget();
	QHBoxLayout* horizontalAlignLayout = new QHBoxLayout();
	horizontalAlignLayout->addWidget(mLeftAlignButton);
	horizontalAlignLayout->addWidget(mHCenterAlignButton);
	horizontalAlignLayout->addWidget(mRightAlignButton);
	horizontalAlignLayout->setSpacing(2);
	horizontalAlignLayout->setContentsMargins(0, 0, 0, 0);
	horizontalAlignWidget->setLayout(horizontalAlignLayout);

	QWidget* verticalAlignWidget = new QWidget();
	QHBoxLayout* verticalAlignLayout = new QHBoxLayout();
	verticalAlignLayout->addWidget(mTopAlignButton);
	verticalAlignLayout->addWidget(mVCenterAlignButton);
	verticalAlignLayout->addWidget(mBottomAlignButton);
	verticalAlignLayout->setSpacing(2);
	verticalAlignLayout->setContentsMargins(0, 0, 0, 0);
	verticalAlignWidget->setLayout(verticalAlignLayout);

	QFrame* separator = new QFrame();
	separator->setFrameStyle(QFrame::VLine | QFrame::Raised);

	mAlignWidget = new QWidget();
	QHBoxLayout* textAlignmentLayout = new QHBoxLayout();
	textAlignmentLayout->addWidget(horizontalAlignWidget);
	textAlignmentLayout->addWidget(separator);
	textAlignmentLayout->addWidget(verticalAlignWidget);
	textAlignmentLayout->addWidget(new QWidget(), 100);
	textAlignmentLayout->setContentsMargins(0, 0, 0, 0);
	mAlignWidget->setLayout(textAlignmentLayout);

	mCheck = addWidget(2, QString("%1:").arg(text).trimmed(), mAlignWidget);
	connect(mCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mLeftAlignButton, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mHCenterAlignButton, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mRightAlignButton, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mTopAlignButton, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mVCenterAlignButton, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mBottomAlignButton, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));

	mAllItemsContainAlignment = false;
}

AlignmentPropertyWidget::~AlignmentPropertyWidget() { }

//==================================================================================================

void AlignmentPropertyWidget::setProperties(const QHash<QString,QVariant>& properties)
{
	uint alignment = (uint)(Qt::AlignCenter);

	if (containsPropertyAndCanConvert<uint>(properties, propertyName(), alignment))
	{
		Qt::Alignment horizontal = ((Qt::Alignment)alignment & Qt::AlignHorizontal_Mask);
		Qt::Alignment vertical = ((Qt::Alignment)alignment & Qt::AlignVertical_Mask);

		if (horizontal & Qt::AlignLeft) mLeftAlignButton->setChecked(true);
		else if (horizontal & Qt::AlignRight) mRightAlignButton->setChecked(true);
		else mHCenterAlignButton->setChecked(true);

		if (vertical & Qt::AlignTop) mTopAlignButton->setChecked(true);
		else if (vertical & Qt::AlignBottom) mBottomAlignButton->setChecked(true);
		else mVCenterAlignButton->setChecked(true);
	}

	mItems.clear();
}

QHash<QString,QVariant> AlignmentPropertyWidget::properties() const
{
	QHash<QString,QVariant> properties;
	Qt::Alignment horizontal = Qt::AlignHCenter;
	Qt::Alignment vertical = Qt::AlignVCenter;

	if (mLeftAlignButton->isChecked()) horizontal = Qt::AlignLeft;
	else if (mRightAlignButton->isChecked()) horizontal = Qt::AlignRight;

	if (mTopAlignButton->isChecked()) vertical = Qt::AlignTop;
	else if (mBottomAlignButton->isChecked()) vertical = Qt::AlignBottom;

	properties.insert(propertyName(), (uint)(horizontal | vertical));
	return properties;
}

//==================================================================================================

void AlignmentPropertyWidget::setItems(const QList<DrawingItem*>& items)
{
	QList<uint> itemAlignments;

	mItems = items;
	mAllItemsContainAlignment = allItemsContainProperty<uint>(mItems, propertyName(), itemAlignments);

	if (mAllItemsContainAlignment)
	{
		uint firstAlignment = itemAlignments.first();
		bool multipleItems = (mItems.size() > 1);
		bool alignmentsMatch = true;
		for(auto alignIter = itemAlignments.begin() + 1; alignIter != itemAlignments.end(); alignIter++)
			alignmentsMatch = (alignmentsMatch && firstAlignment == *alignIter);

		mCheck->setCheckHidden(!multipleItems);

		mCheck->setChecked(alignmentsMatch);

		mAlignWidget->setEnabled(alignmentsMatch);

		Qt::Alignment horizontal = ((Qt::Alignment)firstAlignment & Qt::AlignHorizontal_Mask);
		Qt::Alignment vertical = ((Qt::Alignment)firstAlignment & Qt::AlignVertical_Mask);

		if (horizontal & Qt::AlignLeft) mLeftAlignButton->setChecked(true);
		else if (horizontal & Qt::AlignRight) mRightAlignButton->setChecked(true);
		else mHCenterAlignButton->setChecked(true);

		if (vertical & Qt::AlignTop) mTopAlignButton->setChecked(true);
		else if (vertical & Qt::AlignBottom) mBottomAlignButton->setChecked(true);
		else mVCenterAlignButton->setChecked(true);
	}
}

bool AlignmentPropertyWidget::isValidForItems() const
{
	return mAllItemsContainAlignment;
}

//==================================================================================================

void AlignmentPropertyWidget::handleValueChange()
{
	if (!mItems.isEmpty())
	{
		QList<uint> itemAlignments;

		if (allItemsContainProperty<uint>(mItems, propertyName(), itemAlignments))
		{
			const bool alignmentChange = (checkForSender(sender(), mCheck, mLeftAlignButton) ||
				sender() == mHCenterAlignButton || sender() == mRightAlignButton || sender() == mTopAlignButton ||
				sender() == mVCenterAlignButton || sender() == mBottomAlignButton);

			if (alignmentChange)
			{
				QHash< DrawingItem*, QHash<QString,QVariant> > itemProperties;
				QHash<QString,QVariant> properties;
				Qt::Alignment horizontal = Qt::AlignHCenter;
				Qt::Alignment vertical = Qt::AlignVCenter;

				for(auto itemIter = mItems.begin(); itemIter != mItems.end(); itemIter++)
				{
					if (mLeftAlignButton->isChecked()) horizontal = Qt::AlignLeft;
					else if (mRightAlignButton->isChecked()) horizontal = Qt::AlignRight;
					else horizontal = Qt::AlignHCenter;

					if (mTopAlignButton->isChecked()) vertical = Qt::AlignTop;
					else if (mBottomAlignButton->isChecked()) vertical = Qt::AlignBottom;
					else vertical = Qt::AlignVCenter;

					properties.clear();
					properties.insert(propertyName(), QVariant((uint)(horizontal | vertical)));
					itemProperties.insert(*itemIter, properties);
				}

				if (!itemProperties.isEmpty()) emit propertiesChanged(itemProperties);
			}
		}
	}
	else emit propertiesChanged(properties());
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

ArrowPropertyWidget::ArrowPropertyWidget(const QString& propertyName, const QString& text,
	QWidget* parent) : PropertiesWidget(propertyName, parent)
{
	mStyleCombo = new QComboBox();
	mStyleCombo->addItem(QIcon(":/icons/arrow/arrow_none.png"), "None");
	mStyleCombo->addItem(QIcon(":/icons/arrow/arrow_normal.png"), "Normal");
	mStyleCombo->addItem(QIcon(":/icons/arrow/arrow_triangle.png"), "Triangle");
	mStyleCombo->addItem(QIcon(":/icons/arrow/arrow_triangle_filled.png"), "Triangle Filled");
	mStyleCombo->addItem(QIcon(":/icons/arrow/arrow_concave.png"), "Concave");
	mStyleCombo->addItem(QIcon(":/icons/arrow/arrow_concave_filled.png"), "Concave Filled");
	mStyleCombo->addItem(QIcon(":/icons/arrow/arrow_circle.png"), "Circle");
	mStyleCombo->addItem(QIcon(":/icons/arrow/arrow_circle_filled.png"), "Circle Filled");
	mStyleCheck = addWidget(0, QString("%1 Style:").arg(text).trimmed(), mStyleCombo);
	connect(mStyleCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mStyleCombo, SIGNAL(activated(int)), this, SLOT(handleValueChange()));

	mSizeEdit = new SizeEdit();
	mSizeCheck = addWidget(1, QString("%1 Size:").arg(text).trimmed(), mSizeEdit);
	connect(mSizeCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mSizeEdit, SIGNAL(sizeChanged(qreal)), this, SLOT(handleValueChange()));

	mAllItemsContainArrow = false;
}

ArrowPropertyWidget::~ArrowPropertyWidget() { }

//==================================================================================================

void ArrowPropertyWidget::setProperties(const QHash<QString,QVariant>& properties)
{
	DrawingArrow arrow;

	if (containsPropertyAndCanConvert<DrawingArrow>(properties, propertyName(), arrow))
	{
		mStyleCombo->setCurrentIndex((int)arrow.style());
		mSizeEdit->setSize(arrow.size());
	}

	mItems.clear();
}

QHash<QString,QVariant> ArrowPropertyWidget::properties() const
{
	QHash<QString,QVariant> properties;
	DrawingArrow arrow;

	arrow.setStyle((Drawing::ArrowStyle)mStyleCombo->currentIndex());
	arrow.setSize(mSizeEdit->size());

	QVariant arrowVariant;
	arrowVariant.setValue(arrow);
	properties.insert(propertyName(), arrowVariant);
	return properties;
}

//==================================================================================================

void ArrowPropertyWidget::setItems(const QList<DrawingItem*>& items)
{
	QList<DrawingArrow> itemArrows;

	mItems = items;
	mAllItemsContainArrow = allItemsContainProperty<DrawingArrow>(mItems, propertyName(), itemArrows);

	if (mAllItemsContainArrow)
	{
		DrawingArrow firstArrow = itemArrows.first();
		bool multipleItems = (mItems.size() > 1);
		bool stylesMatch = true;
		bool sizesMatch = true;
		for(auto arrowIter = itemArrows.begin() + 1; arrowIter != itemArrows.end(); arrowIter++)
		{
			stylesMatch = (stylesMatch && firstArrow.style() == arrowIter->style());
			sizesMatch = (sizesMatch && firstArrow.size() == arrowIter->size());
		}

		mStyleCheck->setCheckHidden(!multipleItems);
		mSizeCheck->setCheckHidden(!multipleItems);

		mStyleCheck->setChecked(stylesMatch);
		mSizeCheck->setChecked(sizesMatch);

		mStyleCombo->setEnabled(stylesMatch);
		mSizeEdit->setEnabled(sizesMatch);

		mStyleCombo->setCurrentIndex((int)firstArrow.style());
		mSizeEdit->setSize(firstArrow.size());
	}
}

bool ArrowPropertyWidget::isValidForItems() const
{
	return mAllItemsContainArrow;
}

//==================================================================================================

void ArrowPropertyWidget::handleValueChange()
{
	if (!mItems.isEmpty())
	{
		QList<DrawingArrow> itemArrows;

		if (allItemsContainProperty<DrawingArrow>(mItems, propertyName(), itemArrows))
		{
			const bool styleChange = checkForSender(sender(), mStyleCheck, mStyleCombo);
			const bool sizeChange = checkForSender(sender(), mSizeCheck, mSizeEdit);

			if (styleChange || sizeChange)
			{
				QHash< DrawingItem*, QHash<QString,QVariant> > itemProperties;
				QHash<QString,QVariant> properties;
				DrawingArrow arrow;

				auto itemIter = mItems.begin();
				auto arrowIter = itemArrows.begin();
				for(; itemIter != mItems.end() && arrowIter != itemArrows.end(); itemIter++, arrowIter++)
				{
					arrow = *arrowIter;
					if (styleChange) arrow.setStyle((Drawing::ArrowStyle)mStyleCombo->currentIndex());
					if (sizeChange) arrow.setSize(mSizeEdit->size());

					properties.clear();
					QVariant arrowVariant;
					arrowVariant.setValue(arrow);
					properties.insert(propertyName(), arrowVariant);
					itemProperties.insert(*itemIter, properties);
				}

				if (!itemProperties.isEmpty()) emit propertiesChanged(itemProperties);
			}
		}
	}
	else emit propertiesChanged(properties());
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

GridPropertiesWidget::GridPropertiesWidget(QWidget* parent) : PropertiesWidget("", parent)
{
	mGridEdit = new SizeEdit();
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

	mGridColorWidget = new ColorWidget();
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

	mDynamicGridEnableCombo = new QComboBox();
	mDynamicGridEnableCombo->addItem("Disabled");
	mDynamicGridEnableCombo->addItem("Enabled");
	mDynamicGridEnableCheck = addWidget(5, "Dynamic Grid:", mDynamicGridEnableCombo);
	connect(mDynamicGridEnableCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mDynamicGridEnableCombo, SIGNAL(activated(int)), this, SLOT(handleValueChange()));

	mDynamicGridEdit = new SizeEdit();
	mDynamicGridCheck = addWidget(6, "Dynamic Grid Value:", mDynamicGridEdit);
	connect(mDynamicGridCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mDynamicGridEdit, SIGNAL(sizeChanged(qreal)), this, SLOT(handleValueChange()));

	mDynamicGridEdit->setEnabled(false);
}

GridPropertiesWidget::~GridPropertiesWidget() { }

//==================================================================================================

void GridPropertiesWidget::setProperties(const QHash<QString,QVariant>& properties)
{
	qreal grid = 1;
	uint gridStyle = Drawing::GridNone;
	QBrush gridBrush = QColor(128, 128, 128);
	int gridSpacingMajor = 0, gridSpacingMinor = 0;
	qreal dynamicGrid = 1;
	bool dynamicGridEnabled = false;

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

	if (containsPropertyAndCanConvert<qreal>(properties, "dynamicGrid", dynamicGrid))
		mDynamicGridEdit->setSize(dynamicGrid);
	if (containsPropertyAndCanConvert<bool>(properties, "dynamicGridEnabled", dynamicGridEnabled))
		mDynamicGridEnableCombo->setCurrentIndex(dynamicGridEnabled ? 1 : 0);

	mGridEdit->setEnabled(mDynamicGridEnableCombo->currentIndex() == 0);
	mDynamicGridEdit->setEnabled(mDynamicGridEnableCombo->currentIndex() == 1);
}

QHash<QString,QVariant> GridPropertiesWidget::properties() const
{
	QHash<QString,QVariant> properties;

	properties.insert("grid", mGridEdit->size());
	properties.insert("gridStyle", (uint)mGridStyleCombo->currentIndex());
	properties.insert("gridBrush", QBrush(mGridColorWidget->color()));
	properties.insert("gridSpacingMajor", mGridSpacingMajorEdit->text().toInt());
	properties.insert("gridSpacingMinor", mGridSpacingMinorEdit->text().toInt());
	properties.insert("dynamicGrid", mDynamicGridEdit->size());
	properties.insert("dynamicGridEnabled", mDynamicGridEnableCombo->currentIndex() == 1);

	return properties;
}

//==================================================================================================

void GridPropertiesWidget::handleValueChange()
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
	else if (checkForSender(sender, mDynamicGridCheck, mDynamicGridEdit))
		properties.insert("dynamicGrid", mDynamicGridEdit->size());
	else if (checkForSender(sender, mDynamicGridEnableCheck, mDynamicGridEnableCombo))
		properties.insert("dynamicGridEnabled", mDynamicGridEnableCombo->currentIndex() == 1);

	mGridEdit->setEnabled(mDynamicGridEnableCombo->currentIndex() == 0);
	mDynamicGridEdit->setEnabled(mDynamicGridEnableCombo->currentIndex() == 1);

	if (!properties.isEmpty()) emit propertiesChanged(properties);
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

ReferenceNamePropertyWidget::ReferenceNamePropertyWidget(const QString& propertyName, const QString& text,
	QWidget* parent) : PropertiesWidget(propertyName, parent)
{
	mReferenceLabel = new QLabel();
	mReferenceButton = new QPushButton("...");
	mReferenceButton->setMaximumWidth(QFontMetrics(mReferenceButton->font()).boundingRect("...").width() + 20);

	QWidget* referenceWidget = new QWidget();
	QHBoxLayout* referenceLayout = new QHBoxLayout();
	referenceLayout->addWidget(mReferenceLabel, 100);
	referenceLayout->addWidget(mReferenceButton);
	referenceLayout->setContentsMargins(0, 0, 0, 0);
	referenceWidget->setLayout(referenceLayout);

	mReferenceCheck = addWidget(0, QString("%1:").arg(text).trimmed(), referenceWidget);
	connect(mReferenceCheck, SIGNAL(clicked(bool)), this, SLOT(handleValueChange()));
	connect(mReferenceButton, SIGNAL(clicked(bool)), this, SLOT(browseForReferenceItem()));

	mItem = nullptr;
	mItemContainsReferenceName = false;
}

ReferenceNamePropertyWidget::~ReferenceNamePropertyWidget()
{
	mReferenceItems.clear();
}

//==================================================================================================

void ReferenceNamePropertyWidget::setProperties(const QHash<QString,QVariant>& properties)
{
	QString referenceName;
	if (containsPropertyAndCanConvert<QString>(properties, propertyName(), referenceName))
	{
		if (referenceName.isEmpty()) referenceName = "<Unknown>";
		mReferenceLabel->setText(referenceName);
	}
}

QHash<QString,QVariant> ReferenceNamePropertyWidget::properties() const
{
	QHash<QString,QVariant> properties;

	if (mReferenceLabel->text() == "<Unknown>")
		properties.insert(propertyName(), "");
	else
		properties.insert(propertyName(), mReferenceLabel->text());

	return properties;
}

//==================================================================================================

void ReferenceNamePropertyWidget::setItems(const QList<DrawingItem*>& items)
{
	mItem = nullptr;
	mItemContainsReferenceName = false;

	if (items.size() == 1)
	{
		mItem = items.first();

		QHash<QString,QVariant> properties = mItem->properties();
		if (properties.contains(propertyName()))
		{
			QVariant value = properties.value(propertyName());
			mItemContainsReferenceName = value.canConvert<QString>();
			if (mItemContainsReferenceName)
			{
				QString referenceName = value.value<QString>();
				if (referenceName.isEmpty()) referenceName = "<Unknown>";
				mReferenceLabel->setText(referenceName);
			}
		}
	}
}

bool ReferenceNamePropertyWidget::isValidForItems() const
{
	return mItemContainsReferenceName;
}

//==================================================================================================

void ReferenceNamePropertyWidget::updateReferenceItems(const QList<DrawingItem*>& items)
{
	mReferenceItems = items;
}

//==================================================================================================

void ReferenceNamePropertyWidget::handleValueChange()
{
	if (mItem)
	{
		QHash< DrawingItem*, QHash<QString,QVariant> > itemProperties;
		itemProperties.insert(mItem, properties());
		emit propertiesChanged(itemProperties);
	}
	else emit propertiesChanged(properties());
}

void ReferenceNamePropertyWidget::browseForReferenceItem()
{
	ReferenceItemDialog dialog(mReferenceItems, this);
	if (mReferenceLabel->text() != "<Unknown>") dialog.setReferenceName(mReferenceLabel->text());

	if (dialog.exec() == QDialog::Accepted)
	{
		QString referenceName = dialog.referenceName();
		if (referenceName.isEmpty()) referenceName = "<Unknown>";
		mReferenceLabel->setText(referenceName);

		handleValueChange();
	}
}
