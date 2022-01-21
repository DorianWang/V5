#include "SimpleRouter.h"



void SimpleRouter::routePacket()
{
   if (enable.read() == 1){
      if (size()){
         Packet nextPacket = get_packet();
         std::map <uint32_t, QueuedType*>::iterator it = routingTable.find(nextPacket.dst);
         if (it != routingTable.end()){
            // Maybe add a delay here later?
            (it->second)->put_packet(nextPacket);
         }
         else{
            std::cout << "Could not find destination for packet!" << nextPacket.dst << std::endl;
         }
         // Maybe put an error if the target is not found?
      }
   }
}

bool SimpleRouter::addDest(uint32_t address, QueuedType* target)
{
   auto ret = routingTable.insert(std::pair<uint32_t, QueuedType*>(address, target));
   return ret.second;
}



















