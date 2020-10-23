// DrawingPropertiesBrowser.h
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

#ifndef DRAWINGPROPERTIESBROWSER_H
#define DRAWINGPROPERTIESBROWSER_H

#include <QHash>
#include <QVariant>
#include <QWidget>

class DrawingPropertiesWidget;
class QGroupBox;
class QVBoxLayout;

class DrawingPropertiesBrowser : public QWidget
{
	Q_OBJECT

private:
	QWidget* mDrawingPropertiesWidget;
	QVBoxLayout* mDrawingPropertiesLayout;
	QList<QGroupBox*> mDrawingPropertiesGroups;
	QList<DrawingPropertiesWidget*> mDrawingPropertiesSubWidgets;

	int mPreferredWidth;

public:
	DrawingPropertiesBrowser(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	virtual ~DrawingPropertiesBrowser();

	void registerDrawingWidget(const QString& groupTitle, DrawingPropertiesWidget* widget);

	void setPreferredWidth(int width);
	void setLabelWidth(int width);
	QSize sizeHint() const;

public slots:
	void setDrawingProperties(const QHash<QString,QVariant>& properties);

signals:
	void drawingPropertiesChanged(const QHash<QString,QVariant>& properties);
};

#endif
