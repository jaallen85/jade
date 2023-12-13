// File: SvgWriter.cpp
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

#include "SvgWriter.h"

SvgWriter::SvgWriter(const QRectF& rect, double scale) : mRect(rect), mScale(scale)
{
    // Nothing more to do here.
}

//======================================================================================================================

bool SvgWriter::write(const QString& path)
{
    if (mRect.width() <= 0 || mRect.height() <= 0 || mScale <= 0) return false;

    //const int exportWidth = qRound(mRect.width() * mScale);
    //const int exportHeight = qRound(mRect.height() * mScale);

    return true;
}
