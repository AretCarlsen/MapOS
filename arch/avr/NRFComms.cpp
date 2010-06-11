
// nRF MAP/MEP I/O

// MIRF config server
Nordic_nRF24L01P::Nordic_nRF24L01P_Server<MIRF_t> process_mirf1Server(mirf1, &packet_memoryPool, &kernelSink);
// Routing graph element
packetSinks.setExpand(NRF_CONFIG_SINK_INDEX, &process_mirf1Server);
// 100us frequency
scheduler.add_process(&process_mirf1Server, 100);

// MIRF packet sink
Nordic_nRF24L01P::Nordic_nRF24L01P_MapIO<MIRF_t> process_mirf1Comms(mirf1, nRF_inputBuffer, nRF_outputBuffer);
// Routing graph element
packetSinks.setExpand(NRF_PACKET_SINK_INDEX, &process_mirf1Comms);
// 100us frequency
scheduler.add_process(&process_mirf1Comms, 100);

