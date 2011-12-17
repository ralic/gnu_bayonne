// Copyright (C) 2008-2011 David Sugar, Tycho Softworks.
//
// This file is part of GNU Bayonne.
//
// GNU Bayonne is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// GNU Bayonne is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GNU Bayonne.  If not, see <http://www.gnu.org/licenses/>.

#include <config.h>
#include <ucommon/ucommon.h>
#include <ucommon/export.h>
#include <bayonne.h>

using namespace BAYONNE_NAMESPACE;
using namespace UCOMMON_NAMESPACE;

Device::Serial::Serial()
{
    errno = 0;
}

Device::Serial::~Serial()
{
}

#if !defined(_MSWINDOWS_) && !defined(HAVE_TERMIOS_H)

Device::Serial *Device::open(const char *name)
{
    return NULL;
}

stringpager *Device::serial_list(void)
{
    return new stringpager();
}

#endif