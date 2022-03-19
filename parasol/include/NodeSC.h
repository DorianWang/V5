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
   void checkTasks();
   void completeTask(); // Waits for the "complete" event,
   bool swapTask(); // For use with time slice/round robin scheduling. Not in scope; Untested.
   void init(int discipline){this->discipline = discipline;};

   CpuSC(const std::string& name, NodeSC* parentNode, size_t index) : bbs_sc_module(name.c_str(), index), parentNode(parentNode){
      SC_HAS_PROCESS(CpuSC);
      // This should be called whenever a new task should be considered.
      //SC_METHOD(checkTasks);

      //SC_THREAD(completeTask); // This should be the regular task complete function.
   };

   sc_event complete; // Tells the CPU to process the current task as it should be done.
   sc_event wakeUp;   // Tells the CPU to try to reschedule or kill the task.
   NodeSC* parentNode;

   int discipline;
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
   int discipline;
   uint_fast32_t stat; // Statistics index, unless statFlag is set differently.

   void init(int ncpus, double speed, double quantum, int discipline, int statFlag){
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
