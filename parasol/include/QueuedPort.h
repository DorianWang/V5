#ifndef QUEUEDPORT_H
#define QUEUEDPORT_H

#include "bumber_consts.h"
#include "bumber_shared.h"

#include <deque>
#include <cinttypes>



namespace bbs{

class QueuedPort
{
   size_t index;
   size_t assoc_dst_node; // Associated destination node index
   size_t assoc_task; // Associated task index
   size_t stat; // Statistics index
   QDiscipline discipline;

   std::deque <uint_fast32_t> mq; // Message index list
public:
   sc_event* message_ready; // Notifies when a message is received and queue is empty

   // Should be called by owning node, gives next message based on queuing discipline.
   int get_message(uint_fast32_t* message, QDiscipline curDisc = BS_NONE);
   size_t num_queued() const { return mq.size(); };

   void receive_message(uint_fast32_t message);

   void init(size_t assoc_dst_node, size_t assoc_task, size_t stat, QDiscipline discipline = BS_FIFO){
      this->assoc_dst_node = assoc_dst_node; this->assoc_task = assoc_task;
      this->stat = stat; this->discipline = discipline;
      // Depending on stat value, initialise stat collection.
   }

   QueuedPort(size_t index) : index(index){
      message_ready = new sc_event;
   };
   // virtual ~QueuedPort(); // Use default instead?

   size_t get_index() const {return index;};
   size_t get_associated_node() const {return assoc_dst_node;};
   size_t get_associated_task() const {return assoc_task;};
   void set_associated_node(size_t node){assoc_dst_node = node;};
   void set_associated_task(size_t task){assoc_task = task;};


};

}
#endif // QUEUEDPORT_H





