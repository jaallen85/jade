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

class OdgDrawing;
class OdgStyle;

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    PreferencesDialog(QWidget* parent = nullptr);

    void setPrompts(bool overwrite, bool closeUnsaved);
    void updatePrompts(bool overwrite, bool closeUnsaved);

    void setDrawingTemplate(OdgDrawing* drawingTemplate, OdgStyle* drawingTemplateStyle);
    void updateDrawingTemplate(OdgDrawing* drawingTemplate, OdgStyle* drawingTemplateStyle);
};

#endif
