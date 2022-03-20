#ifndef NODESC_H
#define NODESC_H

#include "bumber_consts.h"
#include "bumber_shared.h"
#include "Message.h"

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
   void assignTask(uint_fast32_t newTask); // Used by node to tell cpu to change task.
   void init(){;};

   CpuSC(const std::string& name, NodeSC* parentNode, size_t index) : bbs_sc_module(name.c_str(), index), parentNode(parentNode){
      //SC_THREAD(completeTask); // This should be the regular task complete function.
   };

   void before_end_of_elaboration() override;

   sc_event complete; // Tells the CPU to process the current task as it should be done.
   sc_event wakeUp;   // Tells the CPU to try to reschedule or kill the task.
   NodeSC* parentNode;

   CPUState state;
   uint32_t taskIndex; // Current task index.
   uint64_t taskStartTS; // Time that sc_time_stamp().value() gives;

};

class NodeSC : public bbs_sc_module
{
public:
   std::set <uint_fast32_t> busIDs;
   std::set <uint_fast32_t> linkIDs;
   sc_vector <CpuSC> cpus;
   std::string name;
   double speed;
   double quantum;
   int statFlag;
   QDiscipline discipline;
   std::deque <uint_fast32_t> rtrq; // Ready to run queue
   uint_fast32_t stat; // Statistics index, unless statFlag is set differently.

   // Call this to get the node to check if it should change cpu running tasks.
   // Doesn't do much now as most scheduling/queuing disciplines aren't implemented.
   void check_schedule();

   void init(int ncpus, double speed, double quantum, QDiscipline discipline, int statFlag){
      this->discipline = discipline;
      std::string baseCPUName = this->name + "_cpu";
      cpus.init(ncpus, [=](const char*, size_t i){return new CpuSC(baseCPUName + std::to_string(i), this, i);});
   };

   // (name, ncpu, speed, quantum, discipline, sf, ps_now)
   NodeSC(const std::string& name, size_t index) : bbs_sc_module(name.c_str(), index), name(name){};

private:

};


}

#endif // NODESC_H
