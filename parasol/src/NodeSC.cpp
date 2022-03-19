#include "NodeSC.h"


using bbs::CpuSC;
using bbs::NodeSC;

void CpuSC::checkTasks()
{
   // Query the parent Processor node for higher priority tasks.
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



