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

/**
 * Basic call statistics.
 * This provides the interface for managing stat nodes, which are kept in
 * shared memory.
 * @file bayonne/stats.h
 */

#ifndef _BAYONNE_STATS_H_
#define	_BAYONNE_STATS_H_

#ifndef _UCOMMON_LINKED_H_
#include <ucommon/linked.h>
#endif

#ifndef	_UCOMMON_THREAD_H_
#include <ucommon/thread.h>
#endif

#ifndef	_UCOMMON_STRING_H_
#include <ucommon/string.h>
#endif

#ifndef	_BAYONNE_NAMESPACE_H_
#include <bayonne/namespace.h>
#endif

namespace bayonne {

#define	STAT_MAP	"bayonne.sta"

class __EXPORT statmap 
{
public:
	char id[8];

	typedef	enum {INCOMING = 0, OUTGOING = 1} stat_t;

	enum {UNUSED = 0, SYSTEM, BOARD, SPAN, REGISTRY} type; 

	struct
	{
		unsigned long total, period, pperiod;
		unsigned short current, peak, min, max, pmin, pmax;
	} stats[2];

	time_t lastcall;
	unsigned short timeslots;

	void update(stat_t element);
	void assign(stat_t element);
	void release(stat_t element);
	unsigned active(void) const;

	static void period(FILE *fp = NULL);
	static statmap *create(unsigned count = 0);
	static statmap *getBoard(unsigned id);
	static statmap *getSpan(unsigned id);
	static statmap *getRegistry(const char *id, unsigned limit = 0);
};

} // end namespace

#endif
