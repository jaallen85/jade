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

#ifndef SCENEPROPERTIESWIDGET_H
#define SCENEPROPERTIESWIDGET_H

#include <DrawingTypes.h>

class ColorPushButton;
class PositionWidget;

class ScenePropertiesWidget : public QWidget
{
	Q_OBJECT

private:
	PositionWidget* mTopLeftWidget;
	QLineEdit* mWidthEdit;
	QLineEdit* mHeightEdit;
	ColorPushButton* mBackgroundColorButton;

	QLineEdit* mGridEdit;
	ColorPushButton* mGridColorButton;
	QComboBox* mGridStyleCombo;
	QSpinBox* mGridSpacingMajorSpin;
	QSpinBox* mGridSpacingMinorSpin;

public:
	ScenePropertiesWidget();
	~ScenePropertiesWidget();

	QSize sizeHint() const;

public slots:
	void setProperties(const QRectF& sceneRect, qreal grid, const QBrush& backgroundBrush,
		DrawingGridStyle gridStyle, const QBrush& gridBrush, int gridSpacingMajor, int gridSpacingMinor);

signals:
	void propertiesUpdated(const QRectF& sceneRect, qreal grid, const QBrush& backgroundBrush,
		DrawingGridStyle gridStyle, const QBrush& gridBrush, int gridSpacingMajor, int gridSpacingMinor);

private slots:
	void handlePropertyChange();

private:
	QGroupBox* createDrawingGroup();
	QGroupBox* createGridGroup();

	void addWidget(QFormLayout*& formLayout, const QString& label, QWidget* widget);
	int labelWidth() const;
};

#endif
