#include "Link.h"

namespace bbs{



uint_fast32_t Link::pop_message(){
   uint_fast32_t res = mq.front();
   mq.pop();
   if (mq.size() != 0){
      // Add an event for the next message in the queue.
      // Use message_transfered.notify(SC_ZERO_TIME) if the message has a size of 0.
   }
   return res;
}

void Link::push_message(uint_fast32_t mess_index)
{
   bs_message_t* mess = bs_mess_pool.get_mp(mess_index);
   if (mess == nullptr){
      // Panic and die?
   }
   mq.push(mess_index);
   if (mq.size() == 0){
      if (mess->length == 0){
         // If the given message has a size of 0,
         // execute transfer_message in the next delta cycle.
         message_transfered.notify(SC_ZERO_TIME);
      }
      else{
         // Otherwise, calculate delay in terms of given message length.
         // Delay = ((length * TICK_CONV_MULT) / trate)
         uint_fast32_t delay = (mess->length * TICK_CONV_MULT) / trate;
         message_transfered.notify(delay, DEFAULT_SMALLEST_TICK);
      }
   }
}



}



