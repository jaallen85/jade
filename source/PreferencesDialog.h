/* PreferencesDialog.h
 *
 * Copyright (C) 2013-2017 Jason Allen
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

#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <DiagramWidget.h>

class PositionWidget;
class SizeWidget;
class SizeEdit;
class ColorWidget;
class GridStyleCombo;

class PreferencesDialog : public QDialog
{
	Q_OBJECT

private:
	QListWidget* listWidget;
	QStackedWidget* stackedWidget;

	QCheckBox* promptOverwriteCheck;
	QCheckBox* promptCloseUnsavedCheck;

	PositionWidget* mDiagramTopLeftWidget;
	SizeWidget* mDiagramRectSizeWidget;
	ColorWidget* mDiagramBackgroundColorWidget;
	SizeEdit* mDiagramGridEdit;
	GridStyleCombo* mDiagramGridStyleCombo;
	ColorWidget* mDiagramGridColorWidget;
	QLineEdit* mDiagramGridSpacingMajorWidget;
	QLineEdit* mDiagramGridSpacingMinorWidget;

public:
	PreferencesDialog(QWidget* parent = nullptr);
	~PreferencesDialog();

	void setPrompts(bool closingUnsaved, bool overwrite);
	bool shouldPromptOnClosingUnsaved() const;
	bool shouldPromptOnOverwrite() const;

	void setDiagramProperties(const QHash<DiagramWidget::Property,QVariant>& properties);
	QHash<DiagramWidget::Property,QVariant> diagramProperties() const;

private:
	void setupGeneralFrame();
	void setupDiagramDefaultsWidget();

	QDialogButtonBox* createButtonBox();
};

#endif
