// File: DrawingPropertiesWidget.h
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

#ifndef DRAWINGPROPERTIESWIDGET_H
#define DRAWINGPROPERTIESWIDGET_H

#include <QWidget>
#include "OdgGlobal.h"

class QComboBox;
class QLineEdit;
class ColorWidget;
class LengthEdit;
class SizeWidget;

class DrawingPropertiesWidget : public QWidget
{
    Q_OBJECT

private:
    QComboBox* mUnitsCombo;
    SizeWidget* mPageSizeWidget;
    SizeWidget* mPageMarginsTopLeftWidget;
    SizeWidget* mPageMarginsBottomRightWidget;
    ColorWidget* mBackgroundColorWidget;

    LengthEdit* mGridEdit;
    QComboBox* mGridStyleCombo;
    ColorWidget* mGridColorWidget;
    QLineEdit* mGridSpacingMajorEdit;
    QLineEdit* mGridSpacingMinorEdit;

    int mCachedGridSpacingMajor;
    int mCachedGridSpacingMinor;

public:
    DrawingPropertiesWidget();

private:
    QWidget* createPageGroup(int labelWidth);
    QWidget* createGridGroup(int labelWidth);

public:
    QSize sizeHint() const override;

    void setUnits(Odg::Units units);
    void setPageSize(const QSizeF& size);
    void setPageMargins(const QMarginsF& margins);
    void setBackgroundColor(const QColor& color);
    Odg::Units units() const;
    QSizeF pageSize() const;
    QMarginsF pageMargins() const;
    QColor backgroundColor() const;

    void setGrid(double grid);
    void setGridStyle(Odg::GridStyle style);
    void setGridColor(const QColor& color);
    void setGridSpacingMajor(int spacing);
    void setGridSpacingMinor(int spacing);
    double grid() const;
    Odg::GridStyle gridStyle() const;
    QColor gridColor() const;
    int gridSpacingMajor() const;
    int gridSpacingMinor() const;


public slots:
    void setProperty(const QString& name, const QVariant& value);

signals:
    void propertyChanged(const QString& name, const QVariant& value);
    void unitsChanged(int units);

private slots:
    void handleUnitsChange(int index);
    void handlePageSizeChange(const QSizeF& size);
    void handlePageMarginsChange(const QSizeF& size);
    void handleBackgroundColorChange(const QColor& color);

    void handleGridChange(double length);
    void handleGridStyleChange(int index);
    void handleGridColorChange(const QColor& color);
    void handleGridSpacingMajorChange();
    void handleGridSpacingMinorChange();
};

#endif
