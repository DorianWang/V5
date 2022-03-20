#include "QueuedPort.h"

using bbs::QueuedPort;

int QueuedPort::get_message(uint_fast32_t* message, QDiscipline curDisc)
{
   int rand;
   int_fast32_t highest = -1; // Minimum priority should be 0
   int_fast32_t index = 0;
   if (mq.size() == 0){
      return 0;
   }
   if (curDisc == BS_NONE){
      curDisc = discipline;
   }

   switch(curDisc){
   case BS_FIFO:
      *message = mq.front();
      mq.pop_front();
      break;
   case BS_LIFO:
      *message = mq.back();
      mq.pop_back();
      break;
   case BS_RAND:
      if (mq.size() == 1){
         *message = mq[0];
         mq.pop_back();
         break;
      }
      rand = mq.size() * drand48(); // Value from (0, mq.size - 1)
      *message = mq[rand];
      mq.erase(mq.begin() + rand); // This is somewhat inefficient for deques if in the middle.
      break;
   case BS_NPRI:

      for (int i = 0; i < mq.size(); i++){
         // If message priority is greater, set index and new highest priority.
      }
      *message = mq[highest];
      mq.erase(mq.begin() + rand);
      break;
   default:
      // Panic?
      return 0;
   }
   return 1;
}

void QueuedPort::receive_message(uint_fast32_t message)
{
   if (mq.size() == 0){
      mq.push_back(message);
      message_ready.notify(SC_ZERO_TIME);
   }
   else{
      mq.push_back(message);
   }
}






