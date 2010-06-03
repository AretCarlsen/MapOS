// Copyright (C) 2010, Aret N Carlsen (aretcarlsen@autonomoustools.com).
// MapOS: Micro Addressing Protocol Operating System.
// Licensed under GPLv3 and later versions. See license.txt or <http://www.gnu.org/licenses/>.

// MapOS required template definitions.


#include <ATcommon/DataStore/Buffer.hpp>
#include <Upacket/Routing/AddressGraph.hpp>

// Packet buffers
template class DataStore::DynamicArrayBuffer<uint8_t,uint16_t>;
// Dynamic routes
template class DataStore::DynamicArrayBuffer<AddressFilter,uint8_t>;
// Dynamic route targets
template class DataStore::DynamicArrayBuffer<MAP::MAPPacketSink*,uint8_t>;
// Timed scheduler
template class DataStore::DynamicArrayBuffer<ProcessDefinition,uint8_t>;

