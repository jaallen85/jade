// File: PreferencesDialog.h
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

#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

class QCheckBox;
class QListWidget;
class QPushButton;
class QStackedWidget;
class DrawingPropertiesWidget;
class OdgDrawing;
class OdgStyle;
class SingleItemPropertiesWidget;

class PreferencesDialog : public QDialog
{
    Q_OBJECT

private:
    QListWidget* mListWidget;
    QStackedWidget* mStackedWidget;

    QCheckBox* mPromptOverwriteCheck;
    QCheckBox* mPromptCloseUnsavedCheck;

    DrawingPropertiesWidget* mDrawingPropertiesWidget;
    SingleItemPropertiesWidget* mStylePropertiesWidget;

    QPushButton* mOkButton;
    QPushButton* mCancelButton;

public:
    PreferencesDialog(QWidget* parent = nullptr);

    void setPrompts(bool overwrite, bool closeUnsaved);
    void updatePrompts(bool& overwrite, bool& closeUnsaved);

    void setDrawingTemplate(OdgDrawing* drawingTemplate, OdgStyle* styleTemplate);
    void updateDrawingTemplate(OdgDrawing* drawingTemplate, OdgStyle* styleTemplate);
};

#endif
