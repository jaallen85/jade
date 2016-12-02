/* PreferencesDialog.cpp
 *
 * Copyright (C) 2013-2016 Jason Allen
 *
 * This file is part of the jade application.
 *
 * jade is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * jade is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with jade.  If not, see <http://www.gnu.org/licenses/>
 */

#include "PreferencesDialog.h"
#include "HelperWidgets.h"

PreferencesDialog::PreferencesDialog(QWidget* parent) : QDialog(parent)
{
	QFontMetrics fontMetrics(font());
	int width = fontMetrics.width("Drawing Defaults") + 24;
	int height = fontMetrics.height();

	listWidget = new QListWidget();
	stackedWidget = new QStackedWidget();

	listWidget->setIconSize(QSize(2 * height, 2 * height));
	listWidget->setGridSize(QSize(width, 4 * height));
	listWidget->setViewMode(QListView::IconMode);
	listWidget->setMovement(QListView::Static);
	listWidget->setFixedWidth(width + 4);
	listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	connect(listWidget, SIGNAL(currentRowChanged(int)), stackedWidget, SLOT(setCurrentIndex(int)));

	setupGeneralFrame();
	setupDiagramDefaultsWidget();

	QWidget *widget = new QWidget();
	QHBoxLayout *hLayout = new QHBoxLayout();
	hLayout->addWidget(listWidget);
	hLayout->addWidget(stackedWidget, 100);
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
	promptCloseUnsavedCheck->setChecked(closingUnsaved);
	promptOverwriteCheck->setChecked(overwrite);
}

bool PreferencesDialog::shouldPromptOnClosingUnsaved() const
{
	return promptCloseUnsavedCheck->isChecked();
}

bool PreferencesDialog::shouldPromptOnOverwrite() const
{
	return promptOverwriteCheck->isChecked();
}

//==================================================================================================

void PreferencesDialog::setDiagramProperties(const QHash<DiagramWidget::Property,QVariant>& properties)
{
	if (properties.contains(DiagramWidget::SceneRect))
	{
		QRectF sceneRect = properties.value(DiagramWidget::SceneRect).toRectF();
		mDiagramTopLeftWidget->setPos(sceneRect.topLeft());
		mDiagramRectSizeWidget->setSize(sceneRect.size());
	}

	if (properties.contains(DiagramWidget::BackgroundColor))
		mDiagramBackgroundColorWidget->setColor(properties.value(DiagramWidget::BackgroundColor).value<QColor>());

	if (properties.contains(DiagramWidget::Grid))
		mDiagramGridEdit->setSize(properties.value(DiagramWidget::Grid).toReal());

	if (properties.contains(DiagramWidget::GridStyle))
		mDiagramGridStyleCombo->setStyle((DiagramWidget::GridRenderStyle)properties.value(DiagramWidget::GridStyle).toUInt());

	if (properties.contains(DiagramWidget::GridColor))
		mDiagramGridColorWidget->setColor(properties.value(DiagramWidget::GridColor).value<QColor>());

	if (properties.contains(DiagramWidget::GridSpacingMajor))
		mDiagramGridSpacingMajorWidget->setText(QString::number(properties.value(DiagramWidget::GridSpacingMajor).toInt()));

	if (properties.contains(DiagramWidget::GridSpacingMinor))
		mDiagramGridSpacingMinorWidget->setText(QString::number(properties.value(DiagramWidget::GridSpacingMinor).toInt()));

	mDiagramGridSpacingMinorWidget->setEnabled(mDiagramGridStyleCombo->currentIndex() == 3);
}

QHash<DiagramWidget::Property,QVariant> PreferencesDialog::diagramProperties() const
{
	QHash<DiagramWidget::Property,QVariant> properties;

	properties[DiagramWidget::SceneRect] = QRectF(mDiagramTopLeftWidget->pos(), mDiagramRectSizeWidget->size());
	properties[DiagramWidget::BackgroundColor] = mDiagramBackgroundColorWidget->color();
	properties[DiagramWidget::Grid] = mDiagramGridEdit->size();
	properties[DiagramWidget::GridStyle] = (uint)mDiagramGridStyleCombo->style();
	properties[DiagramWidget::GridColor] = mDiagramGridColorWidget->color();
	properties[DiagramWidget::GridSpacingMajor] = mDiagramGridSpacingMajorWidget->text().toInt();
	properties[DiagramWidget::GridSpacingMinor] = mDiagramGridSpacingMinorWidget->text().toInt();

	return properties;
}

//==================================================================================================

void PreferencesDialog::setupGeneralFrame()
{
	promptOverwriteCheck = new QCheckBox("Prompt when overwriting existing files");
	promptCloseUnsavedCheck = new QCheckBox("Prompt when closing unsaved files");

	QGroupBox* promptGroup = new QGroupBox("Warnings");
	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->addWidget(promptOverwriteCheck);
	vLayout->addWidget(promptCloseUnsavedCheck);
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
	listWidget->addItem(item);
	stackedWidget->addWidget(generalFrame);
}

void PreferencesDialog::setupDiagramDefaultsWidget()
{
	QWidget* diagramPropertiesWidget = new QWidget();
	QVBoxLayout* diagramPropertiesLayout = new QVBoxLayout();
	QGroupBox* groupBox;
	QFormLayout* groupLayout;

	int labelWidth = QFontMetrics(font()).width("Background Color:") + 8;

	// Create widgets
	mDiagramTopLeftWidget = new PositionWidget();
	mDiagramRectSizeWidget = new SizeWidget(QSizeF(10000, 7500));
	mDiagramBackgroundColorWidget = new ColorWidget();
	mDiagramGridEdit = new SizeEdit();
	mDiagramGridStyleCombo = new GridStyleCombo();
	mDiagramGridColorWidget = new ColorWidget();
	mDiagramGridSpacingMajorWidget = new QLineEdit();
	mDiagramGridSpacingMinorWidget = new QLineEdit();

	mDiagramGridSpacingMajorWidget->setValidator(new QIntValidator(1, 1E6));
	mDiagramGridSpacingMinorWidget->setValidator(new QIntValidator(1, 1E6));

	// Assemble layout
	groupBox = new QGroupBox("Diagram");
	groupLayout = new QFormLayout();
	groupLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
	groupLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	groupLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	groupLayout->addRow("Top Left:", mDiagramTopLeftWidget);
	groupLayout->addRow("Size:", mDiagramRectSizeWidget);
	groupLayout->addRow("Background Color:", mDiagramBackgroundColorWidget);
	groupLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth);
	groupBox->setLayout(groupLayout);
	diagramPropertiesLayout->addWidget(groupBox);

	groupBox = new QGroupBox("Grid");
	groupLayout = new QFormLayout();
	groupLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
	groupLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	groupLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	groupLayout->addRow("Grid:", mDiagramGridEdit);
	groupLayout->addRow("Grid Style:", mDiagramGridStyleCombo);
	groupLayout->addRow("Grid Color:", mDiagramGridColorWidget);
	groupLayout->addRow("Major Spacing:", mDiagramGridSpacingMajorWidget);
	groupLayout->addRow("Minor Spacing:", mDiagramGridSpacingMinorWidget);
	groupLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth);
	groupBox->setLayout(groupLayout);
	diagramPropertiesLayout->addWidget(groupBox);

	diagramPropertiesLayout->addWidget(new QWidget(), 100);
	diagramPropertiesLayout->setContentsMargins(0, 0, 0, 0);
	diagramPropertiesWidget->setLayout(diagramPropertiesLayout);

	QListWidgetItem *item = new QListWidgetItem("Diagram Defaults");
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	item->setTextAlignment(Qt::AlignCenter);
	item->setIcon(QIcon(":/icons/jade/diagram.png"));
	listWidget->addItem(item);
	stackedWidget->addWidget(diagramPropertiesWidget);
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
