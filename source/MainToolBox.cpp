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

MainToolBox::MainToolBox() : QFrame()
{
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

void MainToolBox::updateMode(DrawingWidget::Mode mode)
{
	QList<QAction*> modeActions = mModeActionGroup->actions();
	if (mode == DrawingWidget::DefaultMode && ! modeActions[0]->isChecked())
		modeActions[0]->setChecked(true);
}

void MainToolBox::setModeFromAction(QAction* action)
{
	if (action->text() == "Select Mode") emit defaultModeTriggered();
	else if (action->text() == "Scroll Mode") emit scrollModeTriggered();
	else if (action->text() == "Zoom Mode") emit zoomModeTriggered();
	else emit defaultModeTriggered();
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

	/*addModeAction("Place Line", ":/icons/oxygen/draw-line.png", "");
	addModeAction("Place Arc", ":/icons/oxygen/draw-arc.png", "");
	addModeAction("Place Polyline", ":/icons/oxygen/draw-polyline.png", "");
	addModeAction("Place Curve", ":/icons/oxygen/draw-curve.png", "");
	addModeAction("Place Rect", ":/icons/oxygen/draw-rectangle.png", "");
	addModeAction("Place Ellipse", ":/icons/oxygen/draw-ellipse.png", "");
	addModeAction("Place Polygon", ":/icons/oxygen/draw-polygon.png", "");
	addModeAction("Place Text", ":/icons/oxygen/draw-text.png", "");
	addModeAction("Place Path", ":/icons/oxygen/resistor1.png", "");*/

	//mPlaceItemsMap

	//connect(mPlaceItemsWidget, SIGNAL(itemActivated(QTreeWidgetItem*,int)),
	//	this, SLOT(triggerAction(QTreeWidgetItem*,int)));

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
