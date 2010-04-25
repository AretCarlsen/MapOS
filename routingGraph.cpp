
/* ROUTING GRAPH TARGETS */

// Kernel router
packetSinks.setExpand(0, &kernelSink);

// Port router
packetSinks.setExpand(1, &kernelSink);

// Outgoing sink
packetSinks.setExpand(2, &outgoingSink);

// Echo sink
packetSinks.setExpand(3, &outgoingSink);
// packetSinks.setExpand(3, &process_echoServer);

