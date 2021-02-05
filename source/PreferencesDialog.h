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

#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QHash>
#include <QVariant>

class RectPropertyWidget;
class BrushPropertyWidget;
class GridPropertiesWidget;
class QCheckBox;
class QDialogButtonBox;
class QListWidget;
class QStackedWidget;

class PreferencesDialog : public QDialog
{
	Q_OBJECT

private:
	QListWidget* mListWidget;
	QStackedWidget* mStackedWidget;

	QCheckBox* mPromptOverwriteCheck;
	QCheckBox* mPromptCloseUnsavedCheck;

	RectPropertyWidget* mDefaultDrawingSceneRectWidget;
	BrushPropertyWidget* mDefaultDrawingBackgroundBrushWidget;
	GridPropertiesWidget* mDefaultDrawingGridWidget;

public:
	PreferencesDialog(QWidget* parent = nullptr);
	~PreferencesDialog();

	void setPrompts(bool closingUnsaved, bool overwrite);
	bool shouldPromptOnClosingUnsaved() const;
	bool shouldPromptOnOverwrite() const;

	void setDefaultDrawingProperties(const QHash<QString,QVariant>& properties);
	QHash<QString,QVariant> defaultDrawingProperties() const;

private:
	void setupGeneralFrame();
	void setupDrawingDefaultsWidget();

	QDialogButtonBox* createButtonBox();
};

#endif
