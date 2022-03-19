#include "Bus.h"
#include "Node.h"

using bbs::Bus;

uint_fast32_t Bus::pop_message(){
   uint_fast32_t res = mq.front();
   mq.pop();
   return res;
}


void Bus::push_message(uint_fast32_t mess_index)
{
   bs_message_t* mptr = bs_mess_pool.get_mp(mess_index);
   if (mptr == nullptr){
      // Panic and die?
   }
   // Should be get_task_ID(sender)
   if (associated_nodes.count(mptr->sender)){
      //if (associated_nodes.count(mptr->))
   }

   mq.push(mess_index);
   if (mq.size() == 0){
      if (mptr->length == 0){
         // If the given message has a size of 0,
         // execute transfer_message in the next delta cycle.
         message_transfered.notify(SC_ZERO_TIME);
      }
      else{
         // Otherwise, calculate delay in terms of given message length.
         // Delay = ((length * TICK_CONV_MULT) / trate)
         uint_fast32_t delay = (mptr->length * TICK_CONV_MULT) / trate;
         message_transfered.notify(delay, DEFAULT_SMALLEST_TICK);
      }
   }
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






