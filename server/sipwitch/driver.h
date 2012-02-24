// Copyright (C) 2008-2009 David Sugar, Tycho Softworks.
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

#include <common/server.h>
#include <ucommon/secure.h>

#pragma GCC diagnostic ignored "-Wvariadic-macros"
#include <eXosip2/eXosip.h>
#include <ortp/ortp.h>

#ifndef SESSION_EXPIRES
#define SESSION_EXPIRES "session-expires"
#endif

#ifndef ALLOW_EVENTS
#define ALLOW_EVENTS    "allow-events"
#endif

NAMESPACE_BAYONNE
using namespace UCOMMON_NAMESPACE;

class __LOCAL registry : public Registry
{
public:
    int rid;
    unsigned expires;
    const char *contact;
    const char *userid;
    const char *secret;
    const char *digest;
    const char *server;
    const char *domain;
    const char *method;
    const char *realm;
    const char *uri;
    char uuid[38];

    registry(keydata *keyset, unsigned port = 5060, unsigned expiration = 120);

    void shutdown(void);
};

class __LOCAL thread : public DetachedThread
{
private:
    unsigned instance;
    eXosip_event_t *sevent;

    void run(void);

public:
    thread(size_t size);

    static void shutdown();
};

END_NAMESPACE

