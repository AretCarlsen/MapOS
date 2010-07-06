// Copyright (C) 2010, Aret N Carlsen (aretcarlsen@autonomoustools.com).
// MapOS: Micro Addressing Protocol Operating System.
// Licensed under GPLv3 and later versions. See license.txt or <http://www.gnu.org/licenses/>.

// MapOS kernel routers.


/* ROUTING GRAPH TARGETS */
// Kernel router
packetSinks.setExpand(0, &kernelSink);

// Port router
packetSinks.setExpand(1, &kernelSink);

