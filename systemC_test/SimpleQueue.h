#ifndef SIMPLEQUEUE_H
#define SIMPLEQUEUE_H

#include "PacketType.h"
#include "QueuedType.h"

#include <vector>
#include <iostream>

// https://learnsystemc.com/basic/sc_ctor
// Example of constructor with more than 1 input.
// https://stackoverflow.com/questions/28175764/defining-custom-constructor-for-a-systemc-module
// Another example from user.
class SimpleQueue : public QueuedType
{
   public:

      SimpleQueue(size_t maxLength, QueueRules setting)
      {
         max_buffer_length = maxLength;
         currentSetting = setting;
         buffer.resize(max_buffer_length);
         if (maxLength < 2){
            throw; // Buffers cannot be 0 or 1 sized at this time.
         }
         head = 0; length = 0;
      }

      Packet get_packet()
      {
         if (length == 0){
            return Packet(); // Empty Packet
         }
         else{
            length--;
            Packet result = buffer[head];
            head = (head + 1) % max_buffer_length;
            return result;
         }
      }

      bool put_packet(Packet nextPacket)
      {
         // If buffer is full, then...
         if (length >= max_buffer_length){
            switch(currentSetting){
            case DROP_RECEIVE:
               std::cout << "Dropping packet!\n";
               return false; // Just do nothing. Queue is full.
            case DROP_FIRST:
               buffer[head] = nextPacket;
               head = (head + 1) % max_buffer_length;
               return false; // Replace head and then shift queue forwards.
            case DROP_LAST:
               if (head == 0){
                  buffer[max_buffer_length - 1] = nextPacket;
               }
               else{
                  buffer[head - 1] = nextPacket;
               }
               return false;
            default:
               // Put an error here I guess?
               return false;
            }
         }
         else{
            buffer[(head + length) % max_buffer_length] = nextPacket;
            length++;
            return true;
         }
      }

      size_t size()
      {
         return length;
      }

   private:
      size_t max_buffer_length;
      size_t head; size_t length;
      QueueRules currentSetting;
      std::vector <Packet> buffer;

};

#endif // SIMPLEQUEUE_H
