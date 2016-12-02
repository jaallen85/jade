/* ExportOptionsDialog.h
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

#ifndef EXPORTOPTIONSDIALOG_H
#define EXPORTOPTIONSDIALOG_H

#include <QtWidgets>

class ExportOptionsDialog : public QDialog
{
	Q_OBJECT

private:
	QLineEdit* mWidthEdit;
	QLineEdit* mHeightEdit;
	QCheckBox* mMaintainAspectRatioCheck;

	QRectF mSceneRect;

public:
	ExportOptionsDialog(const QRectF& sceneRect, const QSize& exportSize, bool maintainAspectRatio,
		qreal defaultScale, QWidget* parent = nullptr);
	~ExportOptionsDialog();

	QSize exportSize() const;
	bool maintainAspectRatio() const;

private slots:
	void updateWidth();
	void updateHeight();

private:
	QGroupBox* createSizeGroup();
	QDialogButtonBox* createButtonBox();
};

#endif
