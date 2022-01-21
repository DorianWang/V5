
#include "ServerExample.h"


void ServerExample::do_stuff()
{
   // This function will just send a packet right back to the sender,
   // and set the output data to be the inverse of the input.

   // Run if enabled, otherwise just do nothing.
   if (enable.read() == 1){
      if (size()){
         Packet nextPacket = get_packet();
         nextPacket.dst = nextPacket.src;
         nextPacket.src = addr;
         nextPacket.data = ~nextPacket.data;
         router->put_packet(nextPacket);
      }
   }

};
