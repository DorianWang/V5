#ifndef CPUSC_H
#define CPUSC_H

#include "systemc.h"
#include "para_types.h"

struct CpuSC : public sc_module
{

   bool addTask(size_t tID);
   void checkTasks();
   void completeTask(); // Waits for the "complete" event,
   bool swapTask(); // For use with time slice/round robin scheduling. Not in scope; Untested.


   CpuSC(sc_module_name name, const ps_node_t& parent_node) : sc_module(name), parent_node(parent_node){
      SC_HAS_PROCESS(CpuSC);

      SC_METHOD(checkTasks); // This should be called whenever a new task should be considered.
      //SC_METHOD(); // This should be the task interrupt function. It may be called from above.
      SC_THREAD(completeTask); // This should be the regular task complete function.
   };
   //virtual ~CpuSC();

   sc_event complete; // Tells the CPU to process the current task as it should be done.
   const ps_node_t& parent_node;
   long	discipline;
   uint64_t task_start_ts; // Time that sc_time_stamp().value() gives;

};

#endif // CPUSC_H
