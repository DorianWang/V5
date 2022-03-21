#include "NodeSC.h"

#include "TaskSC.h"

// Not really sure why this define exists, but it does and I don't want to break it.
#if !defined(HAVE_DRAND48)
#include "drand48.h"
#endif


using bbs::CpuSC;
using bbs::NodeSC;

void CpuSC::checkTask()
{
   // Checks info of current (possibly new) task and sets event pointers appropriately.
   // std::cout << sc_time_stamp() << " checkTask()!" << std::endl;
   // std::cout << BSname << " checkTask()!" << std::endl;
   // wait();
   return;
}

void CpuSC::completeTask()
{
   static int i = 0;
   while(true){
      // wait(complete);
      if (taskIndex != 0){ // Not IDLE_TASK
         TaskSC& task = bm_task_tab[taskIndex]; // Add a check before this.

         // Most functions will need to be changed to account for new loop.
         // I think it will end up as a callback of a callback of a callback...
         // Luckily any functions used can call SystemC's wait() function.
         task.code(this);
         complete.notify(1, DEFAULT_TIME_TICK); // Tell the Node that the task has been executed.
      }
      std::cout << sc_time_stamp() << " @ CpuSC::completeTask() " << BSname << " " << basename() << std::endl;
      wait(complete);
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

   SC_THREAD(completeTask);
   sensitive << complete;
   dont_initialize();
   // complete.notify((index + parentNode->index * 127) * 131071 % 8191, DEFAULT_SMALLER_TICK);

   // printf("Before End of Elaboration!\n");
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
   // Returns a random link from the set of shared links
   size_t NodeSC::check_if_share_link(const NodeSC& otherNode)
   {
      std::vector <uint_fast32_t> sharedLinks;
      std::set_intersection(linkSrcIDs.begin(), linkSrcIDs.end(), otherNode.linkDstIDs.begin(),
                           otherNode.linkDstIDs.end(), std::back_inserter(sharedLinks));
      if (sharedLinks.size() == 0){
         return NULL_INDEX;
      }
      size_t rand = sharedLinks.size() * drand48(); // Value from (0, sharedLinks.size - 1)
      return sharedLinks[rand];
   }

   // Returns a random bus from the set of shared buses
   size_t NodeSC::check_if_share_bus(const NodeSC& otherNode)
   {
      std::vector <uint_fast32_t> sharedBuses;
      std::set_intersection(busIDs.begin(), busIDs.end(), otherNode.busIDs.begin(),
                           otherNode.busIDs.end(), std::back_inserter(sharedBuses));
      if (sharedBuses.size() == 0){
         return NULL_INDEX;
      }
      size_t rand = sharedBuses.size() * drand48(); // Value from (0, sharedLinks.size - 1)
      return sharedBuses[rand];
   }

   bool NodeSC::remove_bus_connection(uint_fast32_t bus)
   {
      int res = busIDs.erase(bus);
      if (res) return true;
      return false;
   }













