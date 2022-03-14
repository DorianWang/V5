#include "CpuSC.h"




void CpuSC::checkTasks()
{
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

