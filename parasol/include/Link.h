#ifndef LINK_H
#define LINK_H

#include "bumber_consts.h"
#include "bumber_shared.h"
#include "Message.h"

#include <queue>
#include <cinttypes>

namespace bbs{


// Links are a connection from one source to one destination node.
// It checks for the destination port only for the target node.
// If the target node does not own that port, the send fails.
class Link : public bbs_sc_module
{
   // Used to calculate bus_delay/link_delay together with message->length,
   // so larger messages take more sim time to send.
   // Measured in units of length / SMALLER_TICK.
   // So transfering 0.5 length / TIME_TICK -> trate = 0.5 * TICK_CONV_MULT
   int_fast64_t trate;


   sc_event message_transfered;
public:
   size_t snode;   /* source node index	*/
   size_t dnode;   /* destination node index*/

   void transfer_message();
   void push_message(uint_fast32_t mess_index);
   uint_fast32_t pop_message();

   Link(std::string name, size_t index = NULL_INDEX) : bbs_sc_module(name.c_str(), index){
      SC_HAS_PROCESS(Link);
      if (trate < 0){
         trate = 1; // Also put an error somewhere I guess.
      }
      SC_METHOD(transfer_message);
      sensitive << message_transfered;
      dont_initialize();
   };

   void init(int_fast64_t trate, size_t snode, size_t dnode){
      this->trate = trate; this->snode = snode; this->dnode = dnode;
   }

private:
   std::queue <uint_fast32_t> mq;
};



}

#endif // LINK_H





