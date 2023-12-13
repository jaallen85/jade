// File: main.cpp
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

#include <QApplication>
#include "JadeWindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    JadeWindow window;
    if (app.arguments().size() > 1)
        window.openDrawing(app.arguments().at(1));
    else
        window.newDrawing();
    window.show();

    return app.exec();
}


// Todo:
// - Test, test test!  Especially file saving/loading and cut/copy/paste
// - Export to SVG
// - Preferences dialog
