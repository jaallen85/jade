// File: DrawingPropertiesWidget.cpp
// Copyright (C) 2023  Jason Allen
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
#include "HelperWidgets.h"
#include <QComboBox>
#include <QGroupBox>
#include <QFormLayout>
#include <QVBoxLayout>

DrawingPropertiesWidget::DrawingPropertiesWidget() : QWidget(),
    mUnitsCombo(nullptr), mPageSizeWidget(nullptr), mPageMarginsTopLeftWidget(nullptr),
    mPageMarginsBottomRightWidget(nullptr), mBackgroundColorWidget(nullptr), mGridEdit(nullptr),
    mGridStyleCombo(nullptr), mGridColorWidget(nullptr), mGridSpacingMajorEdit(nullptr), mGridSpacingMinorEdit(nullptr),
    mCachedGridSpacingMajor(8), mCachedGridSpacingMinor(2)
{
    const int labelWidth = QFontMetrics(font()).boundingRect("Margins Bottom-Right:").width() + 8;

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(createPageGroup(labelWidth));
    layout->addWidget(createGridGroup(labelWidth));
    setLayout(layout);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
}

//======================================================================================================================

QWidget* DrawingPropertiesWidget::createPageGroup(int labelWidth)
{
    mUnitsCombo = new QComboBox();
    mUnitsCombo->addItem("Millimeters");
    mUnitsCombo->addItem("Inches");

    mPageSizeWidget = new SizeWidget(QSizeF(), Odg::UnitsMillimeters, true);
    mPageMarginsTopLeftWidget = new SizeWidget(QSizeF(), Odg::UnitsMillimeters, true);
    mPageMarginsBottomRightWidget = new SizeWidget(QSizeF(), Odg::UnitsMillimeters, true);
    mBackgroundColorWidget = new ColorWidget(QColor(255, 255, 255));

    QGroupBox* pageGroup = new QGroupBox("Page");
    QFormLayout* pageLayout = new QFormLayout();
    pageLayout->addRow("Units:", mUnitsCombo);
    pageLayout->addRow("Page Size:", mPageSizeWidget);
    pageLayout->addRow("Margins Top-Left:", mPageMarginsTopLeftWidget);
    pageLayout->addRow("Margins Bottom-Right:", mPageMarginsBottomRightWidget);
    pageLayout->addRow("Background Color:", mBackgroundColorWidget);
    pageLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    pageLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    pageLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    pageLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth);
    pageGroup->setLayout(pageLayout);

    connect(mUnitsCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(handleUnitsChange(int)));
    connect(mPageSizeWidget, SIGNAL(sizeChanged(QSizeF)), this, SLOT(handlePageSizeChange(QSizeF)));
    connect(mPageMarginsTopLeftWidget, SIGNAL(sizeChanged(QSizeF)), this, SLOT(handlePageMarginsChange(QSizeF)));
    connect(mPageMarginsBottomRightWidget, SIGNAL(sizeChanged(QSizeF)), this, SLOT(handlePageMarginsChange(QSizeF)));
    connect(mBackgroundColorWidget, SIGNAL(colorChanged(QColor)), this, SLOT(handleBackgroundColorChange(QColor)));

    return pageGroup;
}

QWidget* DrawingPropertiesWidget::createGridGroup(int labelWidth)
{
    mGridEdit = new LengthEdit(0, Odg::UnitsMillimeters, true);

    mGridStyleCombo = new QComboBox();
    mGridStyleCombo->addItem("Hidden");
    mGridStyleCombo->addItem("Lines");

    mGridColorWidget = new ColorWidget(QColor(77, 153, 153));

    mGridSpacingMajorEdit = new QLineEdit("8");
    mGridSpacingMinorEdit = new QLineEdit("2");

    QIntValidator* spacingValidator = new QIntValidator(0, 1E6, this);
    mGridSpacingMajorEdit->setValidator(spacingValidator);
    mGridSpacingMinorEdit->setValidator(spacingValidator);

    QGroupBox* gridGroup = new QGroupBox("Grid");
    QFormLayout* gridLayout = new QFormLayout();
    gridLayout->addRow("Grid:", mGridEdit);
    gridLayout->addRow("Grid Style:", mGridStyleCombo);
    gridLayout->addRow("Grid Color:", mGridColorWidget);
    gridLayout->addRow("Grid Major Spacing:", mGridSpacingMajorEdit);
    gridLayout->addRow("Grid Minor Spacing:", mGridSpacingMinorEdit);
    gridLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
    gridLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    gridLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    gridLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(labelWidth);
    gridGroup->setLayout(gridLayout);

    connect(mGridEdit, SIGNAL(lengthChanged(double)), this, SLOT(handleGridChange(double)));
    connect(mGridStyleCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(handleGridStyleChange(int)));
    connect(mGridColorWidget, SIGNAL(colorChanged(QColor)), this, SLOT(handleGridColorChange(QColor)));
    connect(mGridSpacingMajorEdit, SIGNAL(editingFinished()), this, SLOT(handleGridSpacingMajorChange()));
    connect(mGridSpacingMinorEdit, SIGNAL(editingFinished()), this, SLOT(handleGridSpacingMinorChange()));

    return gridGroup;
}

//======================================================================================================================

QSize DrawingPropertiesWidget::sizeHint() const
{
    return QSize(340, -1);
}

//======================================================================================================================

void DrawingPropertiesWidget::setUnits(Odg::Units units)
{
    mUnitsCombo->setCurrentIndex(static_cast<int>(units));
}

void DrawingPropertiesWidget::setPageSize(const QSizeF& size)
{
    mPageSizeWidget->setSize(size);
}

void DrawingPropertiesWidget::setPageMargins(const QMarginsF& margins)
{
    mPageMarginsTopLeftWidget->setSize(QSizeF(margins.left(), margins.top()));
    mPageMarginsBottomRightWidget->setSize(QSizeF(margins.right(), margins.bottom()));
}

void DrawingPropertiesWidget::setBackgroundColor(const QColor& color)
{
    mBackgroundColorWidget->setColor(color);
}

Odg::Units DrawingPropertiesWidget::units() const
{
    return static_cast<Odg::Units>(mUnitsCombo->currentIndex());
}

QSizeF DrawingPropertiesWidget::pageSize() const
{
    return mPageSizeWidget->size();
}

QMarginsF DrawingPropertiesWidget::pageMargins() const
{
    return QMarginsF(mPageMarginsTopLeftWidget->size().width(), mPageMarginsTopLeftWidget->size().height(),
                     mPageMarginsBottomRightWidget->size().width(), mPageMarginsBottomRightWidget->size().height());
}

QColor DrawingPropertiesWidget::backgroundColor() const
{
    return mBackgroundColorWidget->color();
}

//======================================================================================================================

void DrawingPropertiesWidget::setGrid(double grid)
{
    mGridEdit->setLength(grid);
}

void DrawingPropertiesWidget::setGridStyle(Odg::GridStyle style)
{
    mGridStyleCombo->setCurrentIndex(static_cast<int>(style));
}

void DrawingPropertiesWidget::setGridColor(const QColor& color)
{
    mGridColorWidget->setColor(color);
}

void DrawingPropertiesWidget::setGridSpacingMajor(int spacing)
{
    mGridSpacingMajorEdit->setText(QString::number(spacing));
    mCachedGridSpacingMajor = spacing;
}

void DrawingPropertiesWidget::setGridSpacingMinor(int spacing)
{
    mGridSpacingMinorEdit->setText(QString::number(spacing));
    mCachedGridSpacingMinor = spacing;
}

double DrawingPropertiesWidget::grid() const
{
    return mGridEdit->length();
}

Odg::GridStyle DrawingPropertiesWidget::gridStyle() const
{
    return static_cast<Odg::GridStyle>(mGridStyleCombo->currentIndex());
}

QColor DrawingPropertiesWidget::gridColor() const
{
    return mGridColorWidget->color();
}

int DrawingPropertiesWidget::gridSpacingMajor() const
{
    bool spacingOk = false;
    int spacing = mGridSpacingMajorEdit->text().toInt(&spacingOk);
    return (spacingOk) ? spacing : mCachedGridSpacingMajor;
}

int DrawingPropertiesWidget::gridSpacingMinor() const
{
    bool spacingOk = false;
    int spacing = mGridSpacingMinorEdit->text().toInt(&spacingOk);
    return (spacingOk) ? spacing : mCachedGridSpacingMinor;
}

//======================================================================================================================

void DrawingPropertiesWidget::setProperty(const QString& name, const QVariant& value)
{
    blockSignals(true);

    if (name == "units" && value.canConvert<int>()) setUnits(static_cast<Odg::Units>(value.toInt()));
    else if (name == "pageSize" && value.canConvert<QSizeF>()) setPageSize(value.value<QSizeF>());
    else if (name == "pageMargins" && value.canConvert<QMarginsF>()) setPageMargins(value.value<QMarginsF>());
    else if (name == "backgroundColor" && value.canConvert<QColor>()) setBackgroundColor(value.value<QColor>());
    else if (name == "grid" && value.canConvert<double>()) setGrid(value.toDouble());
    else if (name == "gridStyle" && value.canConvert<int>()) setGridStyle(static_cast<Odg::GridStyle>(value.toInt()));
    else if (name == "gridColor" && value.canConvert<QColor>()) setGridColor(value.value<QColor>());
    else if (name == "gridSpacingMajor" && value.canConvert<int>()) setGridSpacingMajor(value.toInt());
    else if (name == "gridSpacingMinor" && value.canConvert<int>()) setGridSpacingMinor(value.toInt());

    blockSignals(false);
}

//======================================================================================================================

void DrawingPropertiesWidget::handleUnitsChange(int index)
{
    Odg::Units newUnits = static_cast<Odg::Units>(index);

    emit unitsChanged(newUnits);
    mPageSizeWidget->setUnits(newUnits);
    mPageMarginsTopLeftWidget->setUnits(newUnits);
    mPageMarginsBottomRightWidget->setUnits(newUnits);
    mGridEdit->setUnits(newUnits);

    emit propertyChanged("units", index);
}

void DrawingPropertiesWidget::handlePageSizeChange(const QSizeF& size)
{
    emit propertyChanged("pageSize", size);
}

void DrawingPropertiesWidget::handlePageMarginsChange(const QSizeF& size)
{
    emit propertyChanged("pageMargins", QVariant::fromValue<QMarginsF>(pageMargins()));
}

void DrawingPropertiesWidget::handleBackgroundColorChange(const QColor& color)
{
    emit propertyChanged("backgroundColor", color);
}

//======================================================================================================================

void DrawingPropertiesWidget::handleGridChange(double length)
{
    emit propertyChanged("grid", length);
}

void DrawingPropertiesWidget::handleGridStyleChange(int index)
{
    emit propertyChanged("gridStyle", index);
}

void DrawingPropertiesWidget::handleGridColorChange(const QColor& color)
{
    emit propertyChanged("gridColor", color);
}

void DrawingPropertiesWidget::handleGridSpacingMajorChange()
{
    bool spacingOk = false;
    int spacing = mGridSpacingMajorEdit->text().toInt(&spacingOk);
    if (spacingOk)
    {
        emit propertyChanged("gridSpacingMajor", spacing);

        blockSignals(true);
        mGridSpacingMajorEdit->clearFocus();
        blockSignals(false);
    }
    else setGridSpacingMajor(mCachedGridSpacingMajor);
}

void DrawingPropertiesWidget::handleGridSpacingMinorChange()
{
    bool spacingOk = false;
    int spacing = mGridSpacingMinorEdit->text().toInt(&spacingOk);
    if (spacingOk)
    {
        emit propertyChanged("gridSpacingMinor", spacing);

        blockSignals(true);
        mGridSpacingMinorEdit->clearFocus();
        blockSignals(false);
    }
    else setGridSpacingMajor(mCachedGridSpacingMinor);
}
