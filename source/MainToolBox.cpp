/* MainToolBox.cpp
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

#include "MainToolBox.h"
#include <DrawingPathItem.h>
#include <DrawingShapeItems.h>
#include <DrawingTextItem.h>
#include <DrawingTwoPointItems.h>

MainToolBox::MainToolBox() : QFrame()
{
	mDefaultItemProperties["Pen Style"] = QVariant((unsigned int)Qt::SolidLine);
	mDefaultItemProperties["Pen Width"] = QVariant(12.0);
	mDefaultItemProperties["Pen Color"] = QVariant(QColor(0, 0, 0));
	mDefaultItemProperties["Brush Color"] = QVariant(QColor(255, 255, 255));
	mDefaultItemProperties["Font Family"] = QVariant("Arial");
	mDefaultItemProperties["Font Size"] = QVariant(100.0);
	mDefaultItemProperties["Font Bold"] = QVariant(false);
	mDefaultItemProperties["Font Italic"] = QVariant(false);
	mDefaultItemProperties["Font Underline"] = QVariant(false);
	mDefaultItemProperties["Font Strike-Out"] = QVariant(false);
	mDefaultItemProperties["Text Horizontal Alignment"] = QVariant((unsigned int)Qt::AlignHCenter);
	mDefaultItemProperties["Text Vertical Alignment"] = QVariant((unsigned int)Qt::AlignVCenter);
	mDefaultItemProperties["Text Color"] = QVariant(QColor(0, 0, 0));
	mDefaultItemProperties["Start Arrow Style"] = QVariant((unsigned int)ArrowNone);
	mDefaultItemProperties["Start Arrow Size"] = QVariant(100.0);
	mDefaultItemProperties["End Arrow Style"] = QVariant((unsigned int)ArrowNone);
	mDefaultItemProperties["End Arrow Size"] = QVariant(100.0);

	mModeActionGroup = new QActionGroup(this);
	connect(mModeActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(setModeFromAction(QAction*)));

	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->addWidget(createButtonBox());
	vLayout->addWidget(createTreeWidget(), 100);
	setLayout(vLayout);
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
}

MainToolBox::~MainToolBox()
{
	QList<DrawingItem*> items = mPlaceItemsMap.values();
	while (!items.isEmpty()) delete items.takeFirst();
}

//==================================================================================================

QSize MainToolBox::sizeHint() const
{
	return QSize(140, 400);
}

//==================================================================================================

QMap<QString,QVariant> MainToolBox::defaultItemProperties() const
{
	return mDefaultItemProperties;
}

//==================================================================================================

void MainToolBox::updateMode(DrawingWidget::Mode mode)
{
	QList<QAction*> modeActions = mModeActionGroup->actions();
	if (mode == DrawingWidget::DefaultMode && ! modeActions[0]->isChecked())
		modeActions[0]->setChecked(true);
}

void MainToolBox::updateDefaultItemProperties(const QMap<QString,QVariant>& properties)
{
	for(auto iter = properties.begin(); iter != properties.end(); iter++)
	{
		if (mDefaultItemProperties.contains(iter.key()))
			mDefaultItemProperties[iter.key()] = iter.value();
	}
}

//==================================================================================================

void MainToolBox::setModeFromAction(QAction* action)
{
	if (action->text() == "Select Mode") emit defaultModeTriggered();
	else if (action->text() == "Scroll Mode") emit scrollModeTriggered();
	else if (action->text() == "Zoom Mode") emit zoomModeTriggered();
	else if (mPlaceItemsMap.contains(action))
	{
		DrawingItem* newItem = mPlaceItemsMap[action]->copy();

		for(auto iter = mDefaultItemProperties.begin(); iter != mDefaultItemProperties.end(); iter++)
		{
			if (newItem->hasProperty(iter.key()))
				newItem->setProperty(iter.key(), iter.value());
		}

		emit placeModeTriggered(newItem);
	}
	else emit defaultModeTriggered();
}

void MainToolBox::triggerAction(QTreeWidgetItem* item, int column)
{
	Q_UNUSED(column);
	if (mPlaceActionsMap.contains(item)) mPlaceActionsMap[item]->trigger();
}

//==================================================================================================

QDialogButtonBox* MainToolBox::createButtonBox()
{
	QAction* defaultModeAction = addModeAction("Select Mode", ":/icons/oxygen/edit-select.png", "Escape");
	QAction* scrollModeAction = addModeAction("Scroll Mode", ":/icons/oxygen/transform-move.png", "");
	QAction* zoomModeAction = addModeAction("Zoom Mode", ":/icons/oxygen/page-zoom.png", "");
	QAction* propertiesAction = addAction("Properties...", this, SIGNAL(propertiesTriggered()), ":/icons/oxygen/games-config-board.png");

	int buttonSize = 1.8 * QFontMetrics(font()).height();
	QDialogButtonBox* buttonBox = new QDialogButtonBox();
	buttonBox->setCenterButtons(true);

	mDefaultModeButton = new QToolButton();
	mDefaultModeButton->setDefaultAction(defaultModeAction);
	mDefaultModeButton->setToolTip(defaultModeAction->text());
	mDefaultModeButton->setIconSize(QSize(buttonSize, buttonSize));

	mScrollModeButton = new QToolButton();
	mScrollModeButton->setDefaultAction(scrollModeAction);
	mScrollModeButton->setToolTip(scrollModeAction->text());
	mScrollModeButton->setIconSize(QSize(buttonSize, buttonSize));

	mZoomModeButton = new QToolButton();
	mZoomModeButton->setDefaultAction(zoomModeAction);
	mZoomModeButton->setToolTip(zoomModeAction->text());
	mZoomModeButton->setIconSize(QSize(buttonSize, buttonSize));

	mPropertiesButton = new QToolButton();
	mPropertiesButton->setDefaultAction(propertiesAction);
	mPropertiesButton->setToolTip(propertiesAction->text());
	mPropertiesButton->setIconSize(QSize(buttonSize, buttonSize));

	buttonBox->addButton(mDefaultModeButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(mScrollModeButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(mZoomModeButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(mPropertiesButton, QDialogButtonBox::ActionRole);

	defaultModeAction->setChecked(true);

	return buttonBox;
}

QTreeWidget* MainToolBox::createTreeWidget()
{
	mPlaceItemsWidget = new QTreeWidget();
	mPlaceItemsWidget->setHeaderHidden(true);
	mPlaceItemsWidget->setIndentation(6);

	addItem(new DrawingLineItem(), "Basic Items", "Line", ":/icons/oxygen/draw-line.png");
	addItem(new DrawingArcItem(), "Basic Items", "Arc", ":/icons/oxygen/draw-arc.png");
	addItem(new DrawingPolylineItem(), "Basic Items", "Polyline", ":/icons/oxygen/draw-polyline.png");
	addItem(new DrawingCurveItem(), "Basic Items", "Curve", ":/icons/oxygen/draw-curve.png");
	addItem(new DrawingRectItem(), "Basic Items", "Rectangle", ":/icons/oxygen/draw-rectangle.png");
	addItem(new DrawingEllipseItem(), "Basic Items", "Ellipse", ":/icons/oxygen/draw-ellipse.png");
	addItem(new DrawingPolygonItem(), "Basic Items", "Polygon", ":/icons/oxygen/draw-polygon.png");
	addItem(new DrawingTextItem(), "Basic Items", "Text", ":/icons/oxygen/draw-text.png");

	connect(mPlaceItemsWidget, SIGNAL(itemActivated(QTreeWidgetItem*,int)),
		this, SLOT(triggerAction(QTreeWidgetItem*,int)));
	connect(mPlaceItemsWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
		this, SLOT(triggerAction(QTreeWidgetItem*,int)));

	mPlaceItemsWidget->expandAll();

	return mPlaceItemsWidget;
}

//==================================================================================================

QAction* MainToolBox::addAction(const QString& text, QObject* slotObj, const char* slotFunction,
	const QString& iconPath, const QString& shortcut)
{
	QAction* action = new QAction(text, this);
	connect(action, SIGNAL(triggered()), slotObj, slotFunction);

	if (!iconPath.isEmpty()) action->setIcon(QIcon(iconPath));
	if (!shortcut.isEmpty()) action->setShortcut(QKeySequence(shortcut));

	QFrame::addAction(action);
	return action;
}

QAction* MainToolBox::addModeAction(const QString& text, const QString& iconPath, const QString& shortcut)
{
	QAction* action = new QAction(text, this);

	if (!iconPath.isEmpty()) action->setIcon(QIcon(iconPath));
	if (!shortcut.isEmpty()) action->setShortcut(QKeySequence(shortcut));

	action->setCheckable(true);
	action->setActionGroup(mModeActionGroup);

	return action;
}

void MainToolBox::addItem(DrawingItem* item, const QString& section, const QString& text, const QString& iconPath)
{
	QTreeWidgetItem* newItem = nullptr;
	QTreeWidgetItem* sectionItem = nullptr;

	for(int i = 0; !sectionItem && i < mPlaceItemsWidget->topLevelItemCount(); i++)
	{
		if (mPlaceItemsWidget->topLevelItem(i)->text(0) == section)
			sectionItem = mPlaceItemsWidget->topLevelItem(i);
	}

	if (!sectionItem)
	{
		sectionItem = new QTreeWidgetItem();
		sectionItem->setText(0, section);
		sectionItem->setBackground(0, palette().brush(QPalette::Button));
		mPlaceItemsWidget->addTopLevelItem(sectionItem);
	}

	newItem = new QTreeWidgetItem(sectionItem);
	newItem->setText(0, text);
	if (!iconPath.isEmpty()) newItem->setIcon(0, QIcon(iconPath));

	QAction* newAction = addModeAction("Place " + text, iconPath, "");
	mPlaceActionsMap[newItem] = newAction;
	mPlaceItemsMap[newAction] = item;
}
