
#include "../DataStore/Buffer.hpp"
#include "../Packet/Routing/AddressGraph.hpp"

// Packet buffers
template class DataStore::DynamicArrayBuffer<uint8_t,uint16_t>;
// Dynamic routes
template class DataStore::DynamicArrayBuffer<AddressFilter,uint8_t>;
// Dynamic route targets
template class DataStore::DynamicArrayBuffer<MAP::MAPPacketSink*,uint8_t>;
// Timed scheduler
template class DataStore::DynamicArrayBuffer<ProcessDefinition,uint8_t>;

