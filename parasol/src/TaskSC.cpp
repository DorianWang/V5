#include "TaskSC.h"

#include "QueuedPort.h"

using bbs::TaskSC;

void TaskSC::init(uint_fast32_t node, uint_fast32_t host, void (*code)(void*), TaskState state)
{
   this->node = node; this->host = host; this->code = code; this->state = state;
   this->bport = bs_build_port(NULL_INDEX, node, this->get_index(), BS_FIFO);
   if (this->bport == NULL_INDEX){
      // Panic! Probably out of memory.
   }
}
