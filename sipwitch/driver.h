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

#include <server/server.h>
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
    bool active;

    registry(keydata *keyset, unsigned port = 5060, unsigned expiration = 120);

    void authenticate(const char *realm);

    void activate(void);

    void release(void);

    void shutdown(void);
};

class driver : public Driver
{
public:
    driver();

    int start(void);

    void stop(void);

    void automatic(void);

    static registry *locate(int rid);

};

class __LOCAL thread : public DetachedThread
{
private:
    unsigned instance;
    eXosip_event_t *sevent;
    registry *reg;

    void run(void);

public:
    thread(size_t size);

    static void shutdown();
};

class __LOCAL timeslot : public RTPTimeslot
{
public:
    timeslot(const char *addr, unsigned short port, int family);
};

END_NAMESPACE
