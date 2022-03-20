#ifndef QUEUEDPORT_H
#define QUEUEDPORT_H

#include "bumber_consts.h"

#include <deque>
#include <cinttypes>

// I don't think this is even relevant for any OS that uses C++11, but I'm not 100% sure.
#if !defined(HAVE_DRAND48)
#include "drand48.h"
#endif

namespace bbs{

class QueuedPort
{
   size_t assoc_dst_node; // Associated destination node index
   size_t assoc_task; // Associated task index
   size_t stat; // Statistics index
   QDiscipline discipline;

   std::deque <uint_fast32_t> mq; // Message index list
public:
   sc_event message_ready; // Notifies when a message is received and queue is empty

   // Should be called by owning node, gives next message based on queuing discipline.
   // Timeout time is measured in ticks. Returns 0 on empty, 1 on success.
   int get_message(uint_fast32_t* message, QDiscipline curDisc = BS_NONE);

   void receive_message(uint_fast32_t message);

   QueuedPort(size_t assoc_dst_node, size_t assoc_task, size_t stat, QDiscipline discipline = BS_FIFO) :
   assoc_dst_node(assoc_dst_node), assoc_task(assoc_task), stat(stat), discipline(discipline){
      // Just check the stat and initialize it I guess
   };
   // virtual ~QueuedPort(); // Use default instead?

   size_t get_associated_node() const {return assoc_dst_node;};
   size_t get_associated_task() const {return assoc_task;};

};

}
#endif // QUEUEDPORT_H





