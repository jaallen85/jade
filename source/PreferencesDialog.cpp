// PreferencesDialog.h
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

#include "PreferencesDialog.h"
#include <PropertiesWidgets.h>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

PreferencesDialog::PreferencesDialog(QWidget* parent) : QDialog(parent)
{
	QFontMetrics fontMetrics(font());
	int width = fontMetrics.boundingRect("Drawing Defaults").width() + 24;
	int height = fontMetrics.height();

	mListWidget = new QListWidget();
	mStackedWidget = new QStackedWidget();

	mListWidget->setIconSize(QSize(2 * height, 2 * height));
	mListWidget->setGridSize(QSize(width, 4 * height));
	mListWidget->setViewMode(QListView::IconMode);
	mListWidget->setMovement(QListView::Static);
	mListWidget->setFixedWidth(width + 4);
	mListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	connect(mListWidget, SIGNAL(currentRowChanged(int)), mStackedWidget, SLOT(setCurrentIndex(int)));

	setupGeneralFrame();
	setupDrawingDefaultsWidget();

	QWidget *widget = new QWidget();
	QHBoxLayout *hLayout = new QHBoxLayout();
	hLayout->addWidget(mListWidget);
	hLayout->addWidget(mStackedWidget, 100);
	hLayout->setSpacing(8);
	hLayout->setContentsMargins(0, 0, 0, 0);
	widget->setLayout(hLayout);

	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->addWidget(widget, 100);
	vLayout->addWidget(createButtonBox());
	setLayout(vLayout);

	setWindowTitle("Preferences");
	resize(460, 420);
}

PreferencesDialog::~PreferencesDialog() { }

//==================================================================================================

void PreferencesDialog::setPrompts(bool closingUnsaved, bool overwrite)
{
	mPromptCloseUnsavedCheck->setChecked(closingUnsaved);
	mPromptOverwriteCheck->setChecked(overwrite);
}

bool PreferencesDialog::shouldPromptOnClosingUnsaved() const
{
	return mPromptCloseUnsavedCheck->isChecked();
}

bool PreferencesDialog::shouldPromptOnOverwrite() const
{
	return mPromptOverwriteCheck->isChecked();
}

//==================================================================================================

void PreferencesDialog::setDefaultDrawingProperties(const QHash<QString,QVariant>& properties)
{
	mDefaultDrawingSceneRectWidget->setProperties(properties);
	mDefaultDrawingBackgroundBrushWidget->setProperties(properties);
	mDefaultDrawingGridWidget->setProperties(properties);
}

QHash<QString,QVariant> PreferencesDialog::defaultDrawingProperties() const
{
	QHash<QString,QVariant> properties;

	properties.insert(mDefaultDrawingSceneRectWidget->properties());
	properties.insert(mDefaultDrawingBackgroundBrushWidget->properties());
	properties.insert(mDefaultDrawingGridWidget->properties());

	return properties;
}

//==================================================================================================

void PreferencesDialog::setupGeneralFrame()
{
	mPromptOverwriteCheck = new QCheckBox("Prompt when overwriting existing files");
	mPromptCloseUnsavedCheck = new QCheckBox("Prompt when closing unsaved files");

	QGroupBox* promptGroup = new QGroupBox("Warnings");
	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->addWidget(mPromptOverwriteCheck);
	vLayout->addWidget(mPromptCloseUnsavedCheck);
	promptGroup->setLayout(vLayout);

	QFrame* generalFrame = new QFrame();
	vLayout = new QVBoxLayout();
	vLayout->addWidget(promptGroup);
	vLayout->addWidget(new QWidget(), 100);
	vLayout->setContentsMargins(0, 0, 0, 0);
	generalFrame->setLayout(vLayout);

	QListWidgetItem *item = new QListWidgetItem("General");
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	item->setTextAlignment(Qt::AlignCenter);
	item->setIcon(QIcon(":/icons/oxygen/configure.png"));
	mListWidget->addItem(item);
	mStackedWidget->addWidget(generalFrame);
}

void PreferencesDialog::setupDrawingDefaultsWidget()
{
	mDefaultDrawingSceneRectWidget = new RectPropertyWidget("sceneRect", "", true, false);
	mDefaultDrawingBackgroundBrushWidget = new BrushPropertyWidget("backgroundBrush", "Background");
	mDefaultDrawingGridWidget = new GridPropertiesWidget();

	int labelWidth = QFontMetrics(font()).boundingRect("Background Color:").width() + 24;
	mDefaultDrawingSceneRectWidget->setLabelWidth(labelWidth);
	mDefaultDrawingBackgroundBrushWidget->setLabelWidth(labelWidth);
	mDefaultDrawingGridWidget->setLabelWidth(labelWidth);

	QGroupBox* drawingGroup = new QGroupBox("Drawing");
	QVBoxLayout* drawingLayout = new QVBoxLayout();
	drawingLayout->addWidget(mDefaultDrawingSceneRectWidget);
	drawingLayout->addWidget(mDefaultDrawingBackgroundBrushWidget);
	drawingGroup->setLayout(drawingLayout);

	QGroupBox* gridGroup = new QGroupBox("Grid");
	QVBoxLayout* gridLayout = new QVBoxLayout();
	gridLayout->addWidget(mDefaultDrawingGridWidget);
	gridGroup->setLayout(gridLayout);

	QFrame* drawingDefaultsFrame = new QFrame();
	QVBoxLayout* drawingDefaultsLayout = new QVBoxLayout();
	drawingDefaultsLayout->addWidget(drawingGroup);
	drawingDefaultsLayout->addWidget(gridGroup);
	drawingDefaultsLayout->addWidget(new QWidget(), 100);
	drawingDefaultsLayout->setContentsMargins(0, 0, 0, 0);
	drawingDefaultsFrame->setLayout(drawingDefaultsLayout);

	QListWidgetItem *item = new QListWidgetItem("Drawing Defaults");
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	item->setTextAlignment(Qt::AlignCenter);
	item->setIcon(QIcon(":/icons/jade/jade.png"));
	mListWidget->addItem(item);
	mStackedWidget->addWidget(drawingDefaultsFrame);
}

//==================================================================================================

QDialogButtonBox* PreferencesDialog::createButtonBox()
{
	QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Horizontal);
	buttonBox->setCenterButtons(true);

	QPushButton* okButton = buttonBox->addButton("OK", QDialogButtonBox::AcceptRole);
	QPushButton* cancelButton = buttonBox->addButton("Cancel", QDialogButtonBox::RejectRole);
	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	okButton->setMinimumSize(72, 28);
	cancelButton->setMinimumSize(72, 28);

	return buttonBox;
}
