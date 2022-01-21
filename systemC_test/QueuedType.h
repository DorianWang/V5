#ifndef QUEUEDTYPE_H
#define QUEUEDTYPE_H

#include "PacketType.h"
#include <cstddef>

enum QueueRules
{
   DROP_RECEIVE,  // Reject new packets when full.
   DROP_FIRST,    // Remove the first packet from the queue and insert in the new one.
   DROP_LAST      // Replace the last packet with the incoming one.
};

// A QueuedType must implement adding and removing packets.
// If you forget what this is -> https://isocpp.org/wiki/faq/abcs
class QueuedType
{
   public:
      virtual Packet get_packet() = 0;
      virtual bool put_packet(Packet nextPacket) = 0;
      virtual size_t size() = 0;

};

#endif // QUEUEDTYPE_H
