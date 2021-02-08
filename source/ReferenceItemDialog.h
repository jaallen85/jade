// ReferenceItemDialog.h
// Copyright (C) 2021  Jason Allen
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

#ifndef REFERENCEITEMDIALOG_H
#define REFERENCEITEMDIALOG_H

#include <QDialog>
#include <QList>
#include <QString>
#include <QWidget>

class DrawingItem;
class ReferenceItemPreviewWidget;
class QDialogButtonBox;
class QListWidget;
class QListWidgetItem;

class ReferenceItemDialog : public QDialog
{
	Q_OBJECT

private:
	QListWidget* mListWidget;
	ReferenceItemPreviewWidget* mPreviewWidget;

	QList<DrawingItem*> mReferenceItems;

public:
	ReferenceItemDialog(const QList<DrawingItem*>& referenceItems, QWidget* parent = nullptr);
	~ReferenceItemDialog();

	void setReferenceName(const QString& name);
	QString referenceName() const;

private slots:
	void updatePreview(QListWidgetItem* current, QListWidgetItem* previous);

private:
	QWidget* createContentWidget();
	QDialogButtonBox* createButtonBox();
};

//==================================================================================================

class ReferenceItemPreviewWidget : public QWidget
{
	Q_OBJECT

private:
	DrawingItem* mItem;

public:
	ReferenceItemPreviewWidget(QWidget* parent = nullptr);
	~ReferenceItemPreviewWidget();

	QSize sizeHint() const override;

	void setReferenceItem(DrawingItem* item);

private:
	void paintEvent(QPaintEvent* event);
};

#endif
