#include "ClientExample.h"

void ClientExample::send_stuff()
{
   if (enable.read() == 1){
      nextPID++;
      if (nextPID < 32){
         Packet newPacket{addr, 1, nextPID, (uint64_t)0xFFFFFFFFFFFFFF2A + nextPID};
         std::cout << "@" << sc_time_stamp() <<" :: Sent packet with PID: " << newPacket.PID << " and value: " << newPacket.data << "\n";
         router->put_packet(newPacket);
      }
   }
}

// Replacement function to just ignore inputs and print them out instead.
bool ClientExample::put_packet(Packet newPacket)
{
   std::cout << "@" << sc_time_stamp() <<" :: Got packet with PID: " << newPacket.PID << " and value: " << newPacket.data << "\n";
   return true;
}


