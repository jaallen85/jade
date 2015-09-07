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

	addModeAction("Select Mode", ":/icons/oxygen/edit-select.png", "Escape");
	addModeAction("Scroll Mode", ":/icons/oxygen/transform-move.png", "");
	addModeAction("Zoom Mode", ":/icons/oxygen/page-zoom.png", "");


	QList<QAction*> actions = mModeActionGroup->actions();
	actions[0]->setChecked(true);

	QDialogButtonBox* buttonBox = new QDialogButtonBox();
	buttonBox->setCenterButtons(true);

	buttonBox->addButton(createButton(actions[0]), QDialogButtonBox::ActionRole);
	buttonBox->addButton(createButton(actions[1]), QDialogButtonBox::ActionRole);
	buttonBox->addButton(createButton(actions[2]), QDialogButtonBox::ActionRole);
	//buttonBox->addButton(createButton(
	//	mDiagramView->actions()[DiagramView::PropertiesAction]), QDialogButtonBox::ActionRole);

	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->addWidget(buttonBox);
	vLayout->addWidget(new QWidget(), 100);
	setLayout(vLayout);
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
}

MainToolBox::~MainToolBox()
{

}

//==================================================================================================

QSize MainToolBox::sizeHint() const
{
	return QSize(140, 400);
}

//==================================================================================================

void MainToolBox::updateMode(DrawingWidget::Mode mode)
{

}

/*

addModeAction("Select Mode", ":/icons/oxygen/edit-select.png", "Escape");
addModeAction("Scroll Mode", ":/icons/oxygen/transform-move.png", "");
addModeAction("Zoom Mode", ":/icons/oxygen/page-zoom.png", "");
addModeAction("Place Line", ":/icons/oxygen/draw-line.png", "");
addModeAction("Place Arc", ":/icons/oxygen/draw-arc.png", "");
addModeAction("Place Polyline", ":/icons/oxygen/draw-polyline.png", "");
addModeAction("Place Curve", ":/icons/oxygen/draw-curve.png", "");
addModeAction("Place Rect", ":/icons/oxygen/draw-rectangle.png", "");
addModeAction("Place Ellipse", ":/icons/oxygen/draw-ellipse.png", "");
addModeAction("Place Polygon", ":/icons/oxygen/draw-polygon.png", "");
addModeAction("Place Text", ":/icons/oxygen/draw-text.png", "");
addModeAction("Place Path", ":/icons/oxygen/resistor1.png", "");

actions()[DefaultModeAction]->setChecked(true);*/

//==================================================================================================

void MainToolBox::addAction(const QString& text, QObject* slotObj, const char* slotFunction,
	const QString& iconPath, const QString& shortcut)
{
	QAction* action = new QAction(text, this);
	connect(action, SIGNAL(triggered()), slotObj, slotFunction);

	if (!iconPath.isEmpty()) action->setIcon(QIcon(iconPath));
	if (!shortcut.isEmpty()) action->setShortcut(QKeySequence(shortcut));

	QFrame::addAction(action);
}

void MainToolBox::addModeAction(const QString& text, const QString& iconPath, const QString& shortcut)
{
	QAction* action = new QAction(text, this);

	if (!iconPath.isEmpty()) action->setIcon(QIcon(iconPath));
	if (!shortcut.isEmpty()) action->setShortcut(QKeySequence(shortcut));

	action->setCheckable(true);
	action->setActionGroup(mModeActionGroup);
}

QToolButton* MainToolBox::createButton(QAction* action)
{
	int size = 1.8 * QFontMetrics(font()).height();

	QToolButton* newButton = new QToolButton();
	newButton->setDefaultAction(action);
	newButton->setToolTip(action->text());
	newButton->setIconSize(QSize(size, size));

	return newButton;
}
