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

using namespace UCOMMON_NAMESPACE;
using namespace BAYONNE_NAMESPACE;

sip_context_t driver::tcp_context = NULL;
sip_context_t driver::udp_context = NULL;
sip_context_t driver::out_context = NULL;

driver::driver() : Driver("sip", "registry")
{
    autotimer = 500;

#ifdef  EXOSIP_API4
    udp_context = eXosip_malloc();
    tcp_context = eXosip_malloc();
#endif
    out_context = udp_context;
}

int driver::start(void)
{
    linked_pointer<keydata> kp = keyserver.get("registry");
    const char *cp = NULL, *iface = NULL, *transport = NULL, *agent = NULL;
    unsigned port = 5060, expires = 300, timeslots = 16;
    unsigned rtp = 0;
    int protocol = IPPROTO_UDP;
    int family = AF_INET;
    int tlsmode = 0;
    size_t stack = 0;
    unsigned priority = 0, mpri = 1;

    if(keys)
        cp = keys->get("port");
    else
        cp = NULL;

    if(cp)
        port = atoi(cp);

    if(keys)
        cp = keys->get("expires");
    else
        cp = NULL;

    if(cp)
        expires = atoi(cp);

    if(keys)
        cp = keys->get("stack");
    else
        cp = NULL;

    if(cp)
        stack = atoi(cp);

    if(keys)
        cp = keys->get("priority");
    else
        cp = NULL;

    if(cp)
        priority = atoi(cp);

    if(keys)
        cp = keys->get("media");
    else
        cp = NULL;

    if(cp)
        mpri = atoi(cp);

    if(keys)
        iface = keys->get("interface");

    if(iface) {
#ifdef  AF_INET6
        if(strchr(iface, ':'))
            family = AF_INET6;
#endif
        if(eq(iface, ":::") || eq(iface, "::0") || eq(iface, "*") || iface[0] == 0)
            iface = NULL;
    }

    if(keys)
        transport = keys->get("transport");
    if(transport && eq(transport, "tcp")) {
        protocol = IPPROTO_TCP;
        out_context = tcp_context;
    }
/*
    else if(transport && eq(transport, "tls")) {
        protocol = IPPROTO_TCP;
        out_context = tls_context;
        tlsmode = 1;
    }
*/

    if(keys)
        agent = keys->get("agent");

    if(!agent)
        agent = "bayonne-" VERSION "/exosip2";

    const char *addr = iface;

    ortp_init();
    ortp_scheduler_init();

    ortp_set_log_level_mask(ORTP_WARNING|ORTP_ERROR);
    ortp_set_log_file(stdout);

#ifdef  EXOSIP_API4
    eXosip_init(tcp_context);
    eXosip_init(udp_context);
#else
    eXosip_init();
#endif

#ifdef  AF_INET6
    if(family == AF_INET6) {
        eXosip_enable_ipv6(1);
        if(!iface)
            addr = "::0";
    }
#endif
    if(!addr)
        addr = "*";

#if UCOMMON_ABI > 5
    Socket::query(family);
#else
    Socket::family(family);
#endif
    if(eXosip_listen_addr(OPTION_CONTEXT protocol, iface, port, family, tlsmode))
        shell::log(shell::FAIL, "driver cannot bind %s, port=%d", addr, port);
    else
        shell::log(shell::NOTIFY, "driver binding %s, port=%d", addr, port);

    osip_trace_initialize_syslog(TRACE_LEVEL0, (char *)"bayonne");
    eXosip_set_user_agent(OPTION_CONTEXT agent);

    // create only one media thread for now...
    media *m = new media(stack * 1024l);
    m->start(mpri);

    thread *t;

#ifdef  EXOSIP_API4
    t = new thread(udp_context, stack * 1024l);
    t->start(priority);
    t = new thread(tcp_context, stack * 1024l);
    t->start(priority);
#else
    t = new thread(out_context, stack * 1024l);
    t->start(priority);
#endif

    if(is(kp)) {
        new registry(*kp, port, expires);
        return 0;
    }

    kp = keygroup.begin();
    while(is(kp)) {
        new registry(*kp, port, expires);
        kp.next();
    }

    if(keys)
        cp = keys->get("timeslots");
    if(cp)
        timeslots = atoi(cp);

    if(keys)
        cp = keys->get("rtp");
    if(cp)
        rtp = atoi(cp);

    if(!rtp)
        rtp = ((port / 2) * 2) + 2;

    tsIndex = new Timeslot *[timeslots];
    while(timeslots--) {
        timeslot *ts = new timeslot(iface, rtp, family);
        tsIndex[tsCount++] = (Timeslot *)ts;
        rtp += 2;
    }

    return tsCount;
}

void driver::stop(void)
{
    eXosip_quit(EXOSIP_CONTEXT);
    thread::shutdown();

    unsigned index = 0;
    while(index < tsCount) {
        Timeslot *ts = tsIndex[index++];
        ts->shutdown();
    }

    media::shutdown();
}

void driver::automatic(void)
{
    EXOSIP_LOCK
    eXosip_automatic_action(EXOSIP_CONTEXT);
    EXOSIP_UNLOCK
}

registry *driver::locate(int rid)
{
    linked_pointer<registry>  rp = Group::begin();

    while(is(rp)) {
        if(rp->rid == rid)
            return *rp;
        rp.next();
    }
    return NULL;
}

static SERVICE_MAIN(main, argc, argv)
{
    signals::service("bayonne");
    server::start(argc, argv);
}

PROGRAM_MAIN(argc, argv)
{
    new driver;
    server::start(argc, argv, &service_main);
    PROGRAM_EXIT(0);
}


