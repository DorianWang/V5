#include "NodeSC.h"


using bbs::CpuSC;
using bbs::NodeSC;

void CpuSC::checkTask()
{
   // Checks info of current (possibly new) task and sets event pointers appropriately.
   std::cout << sc_time_stamp() << " checkTask()!" << std::endl;
   std::cout << BSname << " checkTask()!" << std::endl;
   return;
}

void CpuSC::completeTask()
{
   while(true){
      wait(complete);
      // Do something like respond back to the message or execute the task.
   }
   return;
}

void CpuSC::before_end_of_elaboration()
{
   SC_HAS_PROCESS(CpuSC);
   state = BS_IDLE;
   // This should be called whenever a new task should be considered.
   SC_METHOD(checkTask);
   sensitive << wakeUp;
   dont_initialize();
   printf("Before End of Elaboration!\n");
   return;
}


void NodeSC::check_schedule()
{
   // Check to see if anything needs to be done regarding current task allocation.
   switch(discipline){

   case BS_FIFO:
      // Check all CPUs current state. If state is IDLE, assign the first task in the queue to it.
      break;
   case BS_LIFO:
      // Check all CPUs current state. If state is IDLE, assign the last task in the queue to it.
      // I don't think this does any preemption, I would have to check again.
      break;
   case BS_RAND:
      // If any CPUs are IDLE, assign them a task at random.
      break;
   case BS_NPRI:
      // Pick highest priority task out of queue to assign to CPUs.
      break;
   case BS_PPRI:
      // This one shouldn't be hard to do, but we're pretty strapped on time as is.
   case BS_NHOL:
   case BS_CFS:
      // Unimplemented, print an error.
      break;
   case BS_NONE:
   default:
      // Print an error, discipline value out of range.
      break;
   }


}





