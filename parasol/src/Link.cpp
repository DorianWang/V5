#include "Link.h"

#include "QueuedPort.h"

namespace bbs{


void Link::transfer_message(){
   uint_fast32_t mid = mq.front();
   mq.pop();
   std::cout << "In transfer_message" << std::endl;
   if (mid < bs_mess_pool.size()){
      bs_message_t& mess = bs_mess_pool[mid];
      if (mess.port < bm_port_vec.size()){
         bm_port_vec[mess.port].receive_message(mid);
      }
   // Print an error or something.
   }
   if (mq.size() != 0){
      // Add an event for the next message in the queue.
      bs_message_t* mess = bs_mess_pool.get_mp(mq.front());
      if (mess == nullptr){
         // Panic and die?
      }
      while (mess->length == 0){
         if (mess->port < bm_port_vec.size()){
            bm_port_vec[mess->port].receive_message(mq.front());
         }
         mq.pop();
         if (mq.size() == 0) return;
         mess = bs_mess_pool.get_mp(mq.front());
         if (mess == nullptr){
            // Panic and die?
         }
      }
      // Otherwise, calculate delay in terms of given message length.
      // Delay = ((length * TICK_CONV_MULT) / trate)
      uint_fast32_t delay = (mess->length * TICK_CONV_MULT) / trate;
      message_transfered.notify(delay, DEFAULT_SMALLEST_TICK);
   }
}

void Link::push_message(uint_fast32_t mess_index)
{
   bs_message_t* mess = bs_mess_pool.get_mp(mess_index);
   std::cout << "In push_message" << std::endl;
   if (mess == nullptr){
      // Panic and die?
   }
   mq.push(mess_index);
   if (mq.size() == 1){ // Only message in send queue, immediately begin transfer.
      if (mess->length == 0){
         // If the given message has a size of 0,
         // execute transfer_message in the next delta cycle.
         message_transfered.notify(SC_ZERO_TIME);
      }
      else {
         // Otherwise, calculate delay in terms of given message length.
         // Delay = ((length * TICK_CONV_MULT) / trate)
         uint_fast32_t delay = (mess->length * TICK_CONV_MULT) / trate;
         message_transfered.notify(delay, DEFAULT_SMALLER_TICK);
      }
   }
}



}



