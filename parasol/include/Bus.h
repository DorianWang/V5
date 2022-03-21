#ifndef BUS_H
#define BUS_H

#include "bumber_consts.h"
#include "bumber_shared.h"
#include "Message.h"

#include <set>
#include <queue>
#include <cinttypes>

namespace bbs{

class Node;

class Bus : public bbs_sc_module
{
   public:
      void push_message(uint_fast32_t mess_index);
      uint_fast32_t pop_message();

      void add_node(size_t node);
      bool remove_node(size_t node);
      bool find_node(size_t node);
      QDiscipline discipline; // Bus queuing discipline, can only be FIFO or RAND
      sc_event message_transfered;
      sc_event sendEvent;

      void send_message();
      void before_end_of_elaboration() override;

      Bus(const std::string& name, size_t index) : bbs_sc_module(name.c_str(), index){};

      virtual ~Bus(){
         for (size_t i = mq.size(); i > 0; i--){
            bs_mess_pool.free_mess(mq.front());
            mq.pop();
         }
         if (associated_nodes.size()){
            for (size_t node : associated_nodes){
               // Tell nodes that they're getting unlinked.
            }
         }
      }

      // discipline is currently not used, but it seems to exist in parasol.
      void init(int_fast64_t trate, QDiscipline discipline = BS_FIFO){
         if (trate < 0){
            trate = 1; // Also put an error somewhere I guess.
         }
         this->trate = trate; this->discipline = discipline;

      }
   private:
      std::queue <uint_fast32_t> mq;
      std::set <size_t> associated_nodes;
      size_t tableIndex; // The creating table should set the index
      // Used to calculate bus_delay/link_delay together with message->length,
      // so larger messages take more sim time to send.
      // This is a fixed point value equal to length/tick * TRATE_FIXED_MULTIPLIER
      int_fast64_t trate;
};



}

#endif // BUS_H



