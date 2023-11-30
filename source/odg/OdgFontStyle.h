// File: OdgFontStyle.h
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

#ifndef ODGFONTSTYLE_H
#define ODGFONTSTYLE_H

class OdgFontStyle
{
private:
    bool mBold;
    bool mItalic;
    bool mUnderline;
    bool mStrikeOut;

public:
    OdgFontStyle(bool bold = false, bool italic = false, bool underline = false, bool strikeOut = false);
    OdgFontStyle(const OdgFontStyle& other);

    void setBold(bool bold);
    void setItalic(bool italic);
    void setUnderline(bool underline);
    void setStrikeOut(bool strikeOut);
    bool bold() const;
    bool italic() const;
    bool underline() const;
    bool strikeOut() const;

    OdgFontStyle& operator=(const OdgFontStyle &other);
};

bool operator==(const OdgFontStyle& fontStyle1, const OdgFontStyle& fontStyle2);
bool operator!=(const OdgFontStyle& fontStyle1, const OdgFontStyle& fontStyle2);

#endif
