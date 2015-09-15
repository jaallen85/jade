/* ScenePropertiesWidget.cpp
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

#include "ScenePropertiesWidget.h"
#include "ColorButton.h"
#include "PositionWidget.h"

ScenePropertiesWidget::ScenePropertiesWidget() : QWidget()
{
	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->addWidget(createDrawingGroup());
	vLayout->addWidget(createGridGroup());
	vLayout->addWidget(new QWidget(), 100);
	setLayout(vLayout);

	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
}

ScenePropertiesWidget::~ScenePropertiesWidget() { }

//==================================================================================================

QSize ScenePropertiesWidget::sizeHint() const
{
	return QSize(260, layout()->sizeHint().height());
}

//==================================================================================================

void ScenePropertiesWidget::setProperties(const QRectF& sceneRect, qreal grid, const QBrush& backgroundBrush,
	DrawingGridStyle gridStyle, const QBrush& gridBrush, int gridSpacingMajor, int gridSpacingMinor)
{
	mTopLeftWidget->setPos(sceneRect.topLeft());
	mWidthEdit->setText(QString::number(sceneRect.width()));
	mHeightEdit->setText(QString::number(sceneRect.height()));
	mBackgroundColorButton->setColor(backgroundBrush.color());

	mGridEdit->setText(QString::number(grid));
	mGridStyleCombo->setCurrentIndex((int)gridStyle);
	mGridColorButton->setColor(gridBrush.color());
	mGridSpacingMajorSpin->setValue(gridSpacingMajor);
	mGridSpacingMinorSpin->setValue(gridSpacingMinor);
}

//==================================================================================================

void ScenePropertiesWidget::handlePropertyChange()
{
	if (sender() == mGridStyleCombo)
		mGridSpacingMinorSpin->setEnabled(mGridStyleCombo->currentIndex() == 3);

	QRectF sceneRect(mTopLeftWidget->pos(), QSizeF(mWidthEdit->text().toDouble(), mHeightEdit->text().toDouble()));

	emit propertiesUpdated(sceneRect, mGridEdit->text().toDouble(), QBrush(mBackgroundColorButton->color()),
		(DrawingGridStyle)mGridStyleCombo->currentIndex(), QBrush(mGridColorButton->color()),
		mGridSpacingMajorSpin->value(), mGridSpacingMinorSpin->value());
}

//==================================================================================================

QGroupBox* ScenePropertiesWidget::createDrawingGroup()
{
	QGroupBox* drawingGroup = new QGroupBox("Drawing");
	QFormLayout* drawingLayout = nullptr;

	mTopLeftWidget = new PositionWidget(QPointF(-5000, -3750));
	connect(mTopLeftWidget, SIGNAL(positionChanged(const QPointF&)), this, SLOT(handlePropertyChange()));
	addWidget(drawingLayout, "Drawing Origin: ", mTopLeftWidget);

	mWidthEdit = new QLineEdit("10000");
	mHeightEdit = new QLineEdit("7500");
	mWidthEdit->setValidator(new QDoubleValidator(0, std::numeric_limits<qreal>::max(), 6));
	mHeightEdit->setValidator(new QDoubleValidator(0, std::numeric_limits<qreal>::max(), 6));
	connect(mWidthEdit, SIGNAL(editingFinished()), this, SLOT(handlePropertyChange()));
	connect(mHeightEdit, SIGNAL(editingFinished()), this, SLOT(handlePropertyChange()));

	QWidget* sizeWidget = new QWidget();
	QHBoxLayout* sizeLayout = new QHBoxLayout();
	sizeLayout->addWidget(mWidthEdit);
	sizeLayout->addWidget(mHeightEdit);
	sizeLayout->setContentsMargins(0, 0, 0, 0);
	sizeLayout->setSpacing(2);
	sizeWidget->setLayout(sizeLayout);
	addWidget(drawingLayout, "Drawing Size: ", sizeWidget);

	mBackgroundColorButton = new ColorPushButton();
	mBackgroundColorButton->setColor(QColor(255, 255, 255));
	connect(mBackgroundColorButton, SIGNAL(colorChanged(const QColor&)), this, SLOT(handlePropertyChange()));
	addWidget(drawingLayout, "Background Color: ", mBackgroundColorButton);

	drawingGroup->setLayout(drawingLayout);
	return drawingGroup;
}

QGroupBox* ScenePropertiesWidget::createGridGroup()
{
	QGroupBox* gridGroup = new QGroupBox("Grid");
	QFormLayout* gridLayout = nullptr;

	mGridEdit = new QLineEdit("100");
	mGridEdit->setValidator(new QDoubleValidator(0, std::numeric_limits<qreal>::max(), 6));
	connect(mGridEdit, SIGNAL(editingFinished()), this, SLOT(handlePropertyChange()));
	addWidget(gridLayout, "Grid: ", mGridEdit);

	mGridStyleCombo = new QComboBox();
	mGridStyleCombo->addItem("None");
	mGridStyleCombo->addItem("Dotted");
	mGridStyleCombo->addItem("Lined");
	mGridStyleCombo->addItem("Graph Paper");
	mGridStyleCombo->setCurrentIndex(3);
	connect(mGridStyleCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(handlePropertyChange()));
	addWidget(gridLayout, "Grid Style: ", mGridStyleCombo);

	mGridColorButton = new ColorPushButton();
	mGridColorButton->setColor(QColor(0, 128, 128));
	connect(mGridColorButton, SIGNAL(colorChanged(const QColor&)), this, SLOT(handlePropertyChange()));
	addWidget(gridLayout, "Grid Color: ", mGridColorButton);

	mGridSpacingMajorSpin = new QSpinBox();
	mGridSpacingMajorSpin->setRange(1, 100);
	mGridSpacingMajorSpin->setValue(8);
	connect(mGridSpacingMajorSpin, SIGNAL(editingFinished()), this, SLOT(handlePropertyChange()));
	addWidget(gridLayout, "Major Grid Spacing: ", mGridSpacingMajorSpin);

	mGridSpacingMinorSpin = new QSpinBox();
	mGridSpacingMinorSpin->setRange(1, 100);
	mGridSpacingMinorSpin->setValue(2);
	connect(mGridSpacingMinorSpin, SIGNAL(editingFinished()), this, SLOT(handlePropertyChange()));
	addWidget(gridLayout, "Minor Grid Spacing: ", mGridSpacingMinorSpin);

	gridGroup->setLayout(gridLayout);
	return gridGroup;
}

//==================================================================================================

void ScenePropertiesWidget::addWidget(QFormLayout*& formLayout, const QString& label, QWidget* widget)
{
	if (formLayout == nullptr)
	{
		formLayout = new QFormLayout();
		formLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
		formLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	}

	formLayout->addRow(label, widget);

	if (formLayout->rowCount() == 1)
		formLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth());
}

int ScenePropertiesWidget::labelWidth() const
{
	QFontMetrics fontMetrics(font());
	return fontMetrics.width("Major Grid Spacing:") + 8;
}
