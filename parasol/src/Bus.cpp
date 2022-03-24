#include "Bus.h"
#include "NodeSC.h"
#include "QueuedPort.h"

using bbs::Bus;

void Bus::send_message(){
   uint_fast32_t messID = mq.front();
   mq.pop();
   bs_message_t& mref = bs_mess_pool[messID];
   bm_port_vec[mref.port].receive_message(messID);
   std::cout << "sent message to port " << mref.port << std::endl;
   if (mq.size() != 0){
      bs_message_t& mref2 = bs_mess_pool[mq.front()];
      if (mref2.length == 0){
         // If the given message has a size of 0,
         // execute transfer_message in the next delta cycle.
         message_transfered.notify(SC_ZERO_TIME);
      }
      else{
         // Otherwise, calculate delay in terms of given message length.
         // Delay = ((length * TICK_CONV_MULT) / trate)
         uint_fast32_t delay = (mref2.length * TICK_CONV_MULT) / trate;
         message_transfered.notify(delay, DEFAULT_SMALLEST_TICK);
      }
   }
};

void Bus::push_message(uint_fast32_t mess_index)
{
   bs_message_t& mref = bs_mess_pool[mess_index];
   TaskSC& tref = bm_task_tab[mref.sender];
   if (associated_nodes.count(tref.node)){
      size_t port = mref.port;
      size_t node = bm_port_vec[port].get_associated_node();
      if (associated_nodes.count(node)){
         mq.push(mess_index);
         if (mq.size() == 1){
            if (mref.length == 0){
               // If the given message has a size of 0,
               // execute transfer_message in the next delta cycle.
               message_transfered.notify(SC_ZERO_TIME);
            }
            else{
               // Otherwise, calculate delay in terms of given message length.
               // Delay = ((length * TICK_CONV_MULT) / trate)
               uint_fast32_t delay = (mref.length * TICK_CONV_MULT) / trate;
               message_transfered.notify(delay, DEFAULT_SMALLEST_TICK);
            }
         }
         return;
      }
   }
   // Print error.
   std::cout << "Bus fail push" << std::endl;

}

void Bus::add_node(size_t node)
{
   associated_nodes.insert(node);
}

bool Bus::remove_node(size_t node)
{
   int res = associated_nodes.erase(node);
   if (res) return true;
   return false;
}

bool Bus::find_node(size_t node)
{
   return associated_nodes.count(node);
}

void Bus::before_end_of_elaboration()
{
   SC_HAS_PROCESS(Bus);
   SC_METHOD(send_message);
   sensitive << message_transfered;
   dont_initialize();
}









