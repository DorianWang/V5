#ifndef PACKETTYPE_H_INCLUDED
#define PACKETTYPE_H_INCLUDED

#include <cinttypes>

struct Packet
{
   // Source and destination object indicies.
   uint32_t src;
   uint32_t dst;

   uint64_t PID; // Packet ID for tracking, not yet used
   uint64_t data; // Data for "processing"
};



#endif // PACKETTYPE_H_INCLUDED
