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

#include "driver.h"
#include <ctype.h>

namespace bayonne {

registration::registration(LinkedObject **list, keydata *keys, unsigned expiration, unsigned myport) :
Registration(list, keys, "sip:")
{
    const char *identity = keys->get("identity");
    const char *cp = keys->get("expires");
    char buffer[256], host[256];
    char iface[180], user[80];
    srv resolver;

    context = NULL;
    fwd = NULL;

    if(cp)
        expires = atoi(cp);
    else
        expires = expiration;

    if(!identity)
        identity = keys->get("uri");

    if(!identity) {
        context = NULL;
        return;
    }

    cp = keys->get("server");
    if(!cp)
        cp = keys->get("srv");
    if(!cp)
        cp = identity;

    context = resolver.route(buffer, sizeof(buffer), cp);
    if(!context)
        return;

    server = memcopy(buffer);
    userid = memcopy(keys->get("userid"));
    secret = memcopy(keys->get("secret"));
    script = memcopy(keys->get("script"));
    targets = memcopy(keys->get("targets"));
    localnames = memcopy(keys->get("localnames"));
    rid = -1;

    if(!script)
        script = id;

    if(!userid)
        userid = memcopy(keys->get("user"));

    uri::userid(user, sizeof(user), identity);
    uri::hostid(host, sizeof(host), identity);
    
    if(!userid) {
        uri::userid(buffer, sizeof(buffer), identity);
        userid = memcopy(buffer);
    }

    snprintf(buffer, sizeof(buffer), "%s%s@%s", schema, user, host);
    uri = memcopy(buffer);

    getInterface(server, iface, sizeof(iface));
    Random::uuid(uuid);

    localnames = memcopy(keys->get("localnames"));
    if(!localnames) {
        snprintf(buffer, sizeof(buffer), 
            "localhost, localhost.localdomain, %s", host);
        localnames = memcopy(host);
    }

    if(strchr(iface, ':'))
        snprintf(buffer, sizeof(buffer), "%s%s@[%s]:%u", schema, uuid, iface, myport);
    else
        snprintf(buffer, sizeof(buffer), "%s%s@%s:%u", schema, uuid, iface, myport);
    contact = memcopy(buffer);

    voip::msg_t msg = NULL;

    if(-1 != (rid = voip::make_registry_request(context, uri, server, contact, expires, &msg))) {
        osip_message_set_supported(msg, "100rel");
        osip_message_set_header(msg, "Event", "Registration");
        osip_message_set_header(msg, "Allow-Events", "presence");
        voip::send_registry_request(context, rid, msg);
    }
    else
        return;

    shell::debug(3, "registry id %d assigned to %s", rid, id);
}

void registration::confirm(void)
{
    __AUTOPROTECT(this);
    if(fwd) {
        fwd->confirm();
        return;
    }

    if(rid == -1 || !context || activated != 0)
        return;

    time(&activated);
    shell::debug(3, "registry id %d activated", rid);
}

void registration::release(void)
{
    __AUTOPROTECT(this);

    int pid = rid;

    if(fwd)
        return;

    if(rid == -1 || !context)
        return;

    Registration::release();
    voip::release_registry(context, rid);
    rid = -1;
    context = NULL;

    shell::debug(3, "registry id %d released", pid);
}

void registration::failed(void)
{
    __AUTOPROTECT(this);
    int pid = rid;
    if(fwd) {
        fwd->failed();
        rid = -1;
        return;
    }

    if(rid == -1 || !context)
        return;

    if(activated)
        voip::release_registry(context, rid);
    Registration::release();
    rid = -1;

    shell::debug(3, "registry id %d failed", pid);
}

void registration::cancel(void)
{
    __AUTOPROTECT(this);
    int pid = rid;
    if(fwd) {
        fwd->cancel();
        rid = -1;
        // context kept for attached scripts...
        return;
    }

    if(rid == -1)
        return;

    if(activated)
        voip::release_registry(context, rid);
    Registration::release();
    rid = -1;

    shell::debug(3, "registry id %d terminated", pid);
}

void registration::authenticate(const char *realm)
{
    voip::add_authentication(context, userid, secret, realm);
}

} // end namespace
