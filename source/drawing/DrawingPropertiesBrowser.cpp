// DrawingPropertiesBrowser.cpp
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

#include "DrawingPropertiesBrowser.h"
#include "DrawingPropertiesWidget.h"
#include <QGroupBox>
#include <QVBoxLayout>

DrawingPropertiesBrowser::DrawingPropertiesBrowser(QWidget* parent, Qt::WindowFlags f) : QWidget(parent, f)
{
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	mDrawingPropertiesWidget = new QWidget();
	mDrawingPropertiesLayout = new QVBoxLayout();
	mDrawingPropertiesLayout->setContentsMargins(0, 0, 0, 0);
	mDrawingPropertiesLayout->addWidget(new QWidget(), 100);
	mDrawingPropertiesWidget->setLayout(mDrawingPropertiesLayout);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(mDrawingPropertiesWidget);
	setLayout(layout);

	registerDrawingWidget("Drawing", new DrawingRectPropertyWidget("sceneRect", "", true));
	registerDrawingWidget("Drawing", new DrawingBrushPropertyWidget("backgroundBrush", "Background"));
	registerDrawingWidget("Grid", new DrawingGridPropertiesWidget());

	setPreferredWidth(340);
	setLabelWidth(QFontMetrics(font()).boundingRect("Background Color:").width() + 24);
}

DrawingPropertiesBrowser::~DrawingPropertiesBrowser()
{

}

//==================================================================================================

void DrawingPropertiesBrowser::registerDrawingWidget(const QString& groupTitle, DrawingPropertiesWidget* widget)
{
	if (widget)
	{
		QGroupBox* group = nullptr;

		for(auto groupIter = mDrawingPropertiesGroups.begin();
			group == nullptr && groupIter != mDrawingPropertiesGroups.end(); groupIter++)
		{
			if ((*groupIter)->title() == groupTitle) group = (*groupIter);
		}

		if (group == nullptr)
		{
			QVBoxLayout* groupLayout = new QVBoxLayout();

			group = new QGroupBox(groupTitle);
			group->setLayout(groupLayout);
			mDrawingPropertiesLayout->insertWidget(mDrawingPropertiesLayout->count() - 1, group);
			mDrawingPropertiesGroups.append(group);
		}

		group->layout()->addWidget(widget);
		mDrawingPropertiesSubWidgets.append(widget);

		connect(widget, SIGNAL(propertiesChanged(const QHash<QString,QVariant>&)),
			this, SIGNAL(drawingPropertiesChanged(const QHash<QString,QVariant>&)));
	}
}

//==================================================================================================

void DrawingPropertiesBrowser::setPreferredWidth(int width)
{
	mPreferredWidth = width;
}

void DrawingPropertiesBrowser::setLabelWidth(int width)
{
	for(auto subWidgetIter = mDrawingPropertiesSubWidgets.begin();
		subWidgetIter != mDrawingPropertiesSubWidgets.end(); subWidgetIter++)
	{
		(*subWidgetIter)->setLabelWidth(width);
	}
}

QSize DrawingPropertiesBrowser::sizeHint() const
{
	return QSize(mPreferredWidth, -1);
}

//==================================================================================================

void DrawingPropertiesBrowser::setDrawingProperties(const QHash<QString,QVariant>& properties)
{
	for(auto subWidgetIter = mDrawingPropertiesSubWidgets.begin();
		subWidgetIter != mDrawingPropertiesSubWidgets.end(); subWidgetIter++)
	{
		(*subWidgetIter)->setProperties(properties);
	}
}
