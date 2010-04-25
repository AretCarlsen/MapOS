
MemoryPool primary_memoryPool(PRIMARY_MEMORY_POOL_LIMIT);
MemoryPool& packet_memoryPool(primary_memoryPool);
//MemoryPool& dynRoute_memoryPool = packet_memoryPool;
MemoryPool& dynRoute_memoryPool(packet_memoryPool);
MemoryPool& packetSink_memoryPool(packet_memoryPool);

void verify_memPool(){
// Verify memory pool
  uint16_t memSize;
  DEBUGprint_RARE("PackPool: st\n");
  for(memSize = 1; memSize < 3000; memSize++){
    void* test = packet_memoryPool.malloc(memSize);
    if(test == NULL){
      memSize--;
      break;
    }
    packet_memoryPool.free(test, memSize);
  }
  DEBUGprint_RARE("PackPool: %d\n", memSize);
}

