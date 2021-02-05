// PropertiesBrowser.cpp
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

#include "PropertiesBrowser.h"
#include "PropertiesWidgets.h"
#include <QGroupBox>
#include <QStackedWidget>
#include <QTabWidget>
#include <QVBoxLayout>

PropertiesBrowser::PropertiesBrowser(QWidget* parent) : QScrollArea(parent)
{
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	createPropertiesWidget(mDrawingPropertiesWidget, mDrawingPropertiesLayout);
	createPropertiesWidget(mDefaultItemPropertiesWidget, mDefaultItemPropertiesLayout);
	createPropertiesWidget(mItemsPropertiesWidget, mItemsPropertiesLayout);

	mTabWidget = new QTabWidget();
	mTabWidget->addTab(mDrawingPropertiesWidget, "Drawing");
	mTabWidget->addTab(mDefaultItemPropertiesWidget, "Item Defaults");
	mTabWidget->setTabPosition(QTabWidget::South);

	mStackedWidget = new QStackedWidget();
	mStackedWidget->addWidget(mTabWidget);
	mStackedWidget->addWidget(mItemsPropertiesWidget);
	setWidget(mStackedWidget);
	setWidgetResizable(true);

	registerDrawingWidget("Drawing", new RectPropertyWidget("sceneRect", "", true, false));
	registerDrawingWidget("Drawing", new BrushPropertyWidget("backgroundBrush", "Background"));
	registerDrawingWidget("Grid", new GridPropertiesWidget());

	registerDefaultItemWidget("Pen / Brush", new PenPropertyWidget("pen", "Pen"));
	registerDefaultItemWidget("Pen / Brush", new BrushPropertyWidget("brush", "Brush"));
	registerDefaultItemWidget("Text", new FontPropertyWidget("font", "Font"));
	registerDefaultItemWidget("Text", new AlignmentPropertyWidget("alignment", "Alignment"));
	registerDefaultItemWidget("Text", new BrushPropertyWidget("textBrush", "Text"));
	registerDefaultItemWidget("Arrow", new ArrowPropertyWidget("startArrow", "Start Arrow"));
	registerDefaultItemWidget("Arrow", new ArrowPropertyWidget("endArrow", "End Arrow"));

	registerItemsWidget("Position", new PositionPropertyWidget("position", "Position"));
	registerItemsWidget("Line", new LinePropertyWidget("line", ""));
	registerItemsWidget("Curve", new CurvePropertyWidget("curve", ""));
	registerItemsWidget("Rect", new RectPropertyWidget("rect", "", false, false));
	registerItemsWidget("Rect", new SizePropertyWidget("cornerRadius", "Corner Radius"));
	registerItemsWidget("Ellipse", new RectPropertyWidget("ellipse", "", false, true));
	registerItemsWidget("Pen / Brush", new PenPropertyWidget("pen", "Pen"));
	registerItemsWidget("Pen / Brush", new BrushPropertyWidget("brush", "Brush"));
	registerItemsWidget("Text", new FontPropertyWidget("font", "Font"));
	registerItemsWidget("Text", new AlignmentPropertyWidget("alignment", "Alignment"));
	registerItemsWidget("Text", new BrushPropertyWidget("textBrush", "Text"));
	registerItemsWidget("Text", new TextPropertyWidget("caption", "Caption"));
	registerItemsWidget("Arrow", new ArrowPropertyWidget("startArrow", "Start Arrow"));
	registerItemsWidget("Arrow", new ArrowPropertyWidget("endArrow", "End Arrow"));
	registerItemsWidget("Polyline", new PolygonPropertyWidget("polyline"));
	registerItemsWidget("Polygon", new PolygonPropertyWidget("polygon"));

	setPreferredWidth(360);
	setLabelWidth(QFontMetrics(font()).boundingRect("Background Color:").width() + 24);
}

PropertiesBrowser::~PropertiesBrowser() { }

//==================================================================================================

void PropertiesBrowser::registerDrawingWidget(const QString& groupTitle, PropertiesWidget* widget)
{
	if (widget)
	{
		QGroupBox* group = findOrCreateGroup(groupTitle, mDrawingPropertiesLayout, mDrawingPropertiesGroups);
		group->layout()->addWidget(widget);
		mDrawingPropertiesSubWidgets.append(widget);

		connect(widget, SIGNAL(propertiesChanged(const QHash<QString,QVariant>&)),
			this, SIGNAL(drawingPropertiesChanged(const QHash<QString,QVariant>&)));
	}
}

void PropertiesBrowser::registerDefaultItemWidget(const QString& groupTitle, PropertiesWidget* widget)
{
	if (widget)
	{
		QGroupBox* group = findOrCreateGroup(groupTitle, mDefaultItemPropertiesLayout, mDefaultItemPropertiesGroups);
		group->layout()->addWidget(widget);
		mDefaultItemPropertiesSubWidgets.append(widget);

		connect(widget, SIGNAL(propertiesChanged(const QHash<QString,QVariant>&)),
			this, SIGNAL(defaultItemPropertiesChanged(const QHash<QString,QVariant>&)));
	}
}

void PropertiesBrowser::registerItemsWidget(const QString& groupTitle, PropertiesWidget* widget)
{
	if (widget)
	{
		QGroupBox* group = findOrCreateGroup(groupTitle, mItemsPropertiesLayout, mItemsPropertiesGroups);
		group->layout()->addWidget(widget);
		mItemsPropertiesSubWidgets.append(widget);

		connect(widget, SIGNAL(propertiesChanged(const QHash< DrawingItem*, QHash<QString,QVariant> >&)),
			this, SIGNAL(itemsPropertiesChanged(const QHash< DrawingItem*, QHash<QString,QVariant> >&)));
	}
}

//==================================================================================================

void PropertiesBrowser::setPreferredWidth(int width)
{
	mPreferredWidth = width;
}

void PropertiesBrowser::setLabelWidth(int width)
{
	for(auto subWidgetIter = mDrawingPropertiesSubWidgets.begin();
		subWidgetIter != mDrawingPropertiesSubWidgets.end(); subWidgetIter++)
	{
		(*subWidgetIter)->setLabelWidth(width);
	}

	for(auto subWidgetIter = mDefaultItemPropertiesSubWidgets.begin();
		subWidgetIter != mDefaultItemPropertiesSubWidgets.end(); subWidgetIter++)
	{
		(*subWidgetIter)->setLabelWidth(width);
	}

	for(auto subWidgetIter = mItemsPropertiesSubWidgets.begin();
		subWidgetIter != mItemsPropertiesSubWidgets.end(); subWidgetIter++)
	{
		(*subWidgetIter)->setLabelWidth(width);
	}
}

QSize PropertiesBrowser::sizeHint() const
{
	return QSize(mPreferredWidth, -1);
}

//==================================================================================================

void PropertiesBrowser::setItems(const QList<DrawingItem*>& items)
{
	for(auto subWidgetIter = mItemsPropertiesSubWidgets.begin();
		subWidgetIter != mItemsPropertiesSubWidgets.end(); subWidgetIter++)
	{
		(*subWidgetIter)->setItems(items);
		(*subWidgetIter)->setVisible((*subWidgetIter)->isValidForItems());
	}

	for(auto groupIter = mItemsPropertiesGroups.begin(); groupIter != mItemsPropertiesGroups.end(); groupIter++)
	{
		QLayout* groupLayout = (*groupIter)->layout();
		bool allChildrenHidden = true;
		PropertiesWidget* widget;

		if (groupLayout)
		{
			for(int i = 0; allChildrenHidden && i < groupLayout->count(); i++)
			{
				widget = qobject_cast<PropertiesWidget*>(groupLayout->itemAt(i)->widget());
				if (widget) allChildrenHidden = !widget->isValidForItems();
			}
		}

		(*groupIter)->setVisible(!allChildrenHidden);
	}

	mStackedWidget->setCurrentIndex(items.isEmpty() ? 0 : 1);
}

//==================================================================================================

void PropertiesBrowser::setDrawingProperties(const QHash<QString,QVariant>& properties)
{
	for(auto subWidgetIter = mDrawingPropertiesSubWidgets.begin();
		subWidgetIter != mDrawingPropertiesSubWidgets.end(); subWidgetIter++)
	{
		(*subWidgetIter)->setProperties(properties);
	}
}

void PropertiesBrowser::setDefaultItemProperties(const QHash<QString,QVariant>& properties)
{
	for(auto subWidgetIter = mDefaultItemPropertiesSubWidgets.begin();
		subWidgetIter != mDefaultItemPropertiesSubWidgets.end(); subWidgetIter++)
	{
		(*subWidgetIter)->setProperties(properties);
	}
}

void PropertiesBrowser::setItemsProperties(const QList<DrawingItem*>& items)
{
	for(auto subWidgetIter = mItemsPropertiesSubWidgets.begin();
		subWidgetIter != mItemsPropertiesSubWidgets.end(); subWidgetIter++)
	{
		(*subWidgetIter)->setItems(items);
	}
}

//==================================================================================================

void PropertiesBrowser::createPropertiesWidget(QWidget*& widget, QVBoxLayout*& layout)
{
	widget = new QWidget();
	layout = new QVBoxLayout();
	layout->addWidget(new QWidget(), 100);
	widget->setLayout(layout);
}

QGroupBox* PropertiesBrowser::findOrCreateGroup(const QString& groupTitle,
	QVBoxLayout* layout, QList<QGroupBox*>& groups)
{
	QGroupBox* group = nullptr;

	for(auto groupIter = groups.begin();
		group == nullptr && groupIter != groups.end(); groupIter++)
	{
		if ((*groupIter)->title() == groupTitle) group = (*groupIter);
	}

	if (group == nullptr)
	{
		QVBoxLayout* groupLayout = new QVBoxLayout();

		group = new QGroupBox(groupTitle);
		group->setLayout(groupLayout);
		layout->insertWidget(layout->count() - 1, group);
		groups.append(group);
	}

	return group;
}
