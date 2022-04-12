#ifndef NODESC_H
#define NODESC_H

#include "bumber_consts.h"
#include "bumber_shared.h"
#include "Message.h"
#include "TaskSC.h"

#include <set>
#include <vector>
#include <string>
#include <cstdint>

namespace bbs{

class NodeSC;

struct CpuSC : public bbs_sc_module
{
   bool addTask(size_t tID);
   void checkTask();
   void completeTask(); // Waits for the "complete" event,
   bool swapTask(); // For use with time slice/round robin scheduling. Not in scope; Untested.
   void assignTask(TaskThread* newTask); // Used by node to tell cpu to change task.
   void init(ResourceState state){
      this->state = state;
   };

   CpuSC(const std::string& name, NodeSC* parentNode, size_t index) : bbs_sc_module(name.c_str(), index), parentNode(parentNode){ BSname = name; };

   void before_end_of_elaboration() override;

   sc_event complete; // Tells the CPU to process the current task as it should be done.
   sc_event wakeUp;   // Tells the CPU to try to reschedule or kill the task.
   NodeSC* parentNode;

   ResourceState state;
   uint32_t taskIndex = 0; // Current task index.
   uint64_t taskStartTS; // Time that sc_time_stamp().value() gives;

};

class NodeSC : public bbs_sc_module
{
public:
   friend class CpuSC;
   std::set <uint_fast32_t> busIDs;
   std::set <uint_fast32_t> linkSrcIDs;
   std::set <uint_fast32_t> linkDstIDs;
   sc_vector <CpuSC> cpus;
   double speed;
   double quantum;
   int statFlag;
   QDiscipline discipline;
   std::deque <TaskThread*> rtrq; // Ready to run queue, should probably use shared or unique pointers.
   uint_fast32_t stat; // Statistics index, unless statFlag is set differently.

   // Call this to get the node to check if it should change cpu running tasks.
   // Doesn't do much now as most scheduling/queuing disciplines aren't implemented.
   void check_schedule();

   size_t check_if_share_link(const NodeSC& otherNode);
   size_t check_if_share_bus(const NodeSC& otherNode);

   bool remove_bus_connection(uint_fast32_t bus);

   void init(int ncpus, double speed, double quantum, QDiscipline discipline, int statFlag){
      SC_HAS_PROCESS(CpuSC);
      this->discipline = discipline;
      std::string baseCPUName = this->BSname + "_cpu";
      cpus.init(ncpus, [=](const char*, size_t i){return new CpuSC(baseCPUName + std::to_string(i), this, i);});
      for (int i = 0; i < ncpus; i++){
         cpus[i].init(BS_IDLE);
      }
   };

   // (name, ncpu, speed, quantum, discipline, sf, ps_now)
   NodeSC(const std::string& name, size_t index) : bbs_sc_module(name.c_str(), index){
      BSname = name;
      // std::cout << "Building Node with name: " << BSname << std::endl;
   };

private:

};


}

#endif // NODESC_H
