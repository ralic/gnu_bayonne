// Copyright (C) 2010 David Sugar, Tycho Softworks.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "server.h"

using namespace BAYONNE_NAMESPACE;
using namespace UCOMMON_NAMESPACE;

static void init(int argc, char **argv, shell::mainproc_t svc = NULL)
{
    bool detached = false;

    if(svc)
        detached = Driver::getDetached();

}

static SERVICE_MAIN(main, argc, argv)
{
//  signals::service("bayonne");
    init(argc, argv);
}

PROGRAM_MAIN(argc, argv)
{
    init(argc, argv, &service_main);
    PROGRAM_EXIT(0);
}

