#ifndef BUMBER_CONSTS_H_INCLUDED
#define BUMBER_CONSTS_H_INCLUDED

#include "systemc.h"
#include <cstdint>

#define DEFAULT_TIME_TICK SC_US
#define DEFAULT_SMALLER_TICK SC_NS
#define DEFAULT_SMALLEST_TICK SC_FS
#define TICK_CONV_MULT 1000 // SI unit prefix constant, in case we have to support others.

namespace bbs{

enum QDiscipline{
   _NONE, // Unset, possible uninitialized error
   _FIFO, // First In, First Out AKA First Come, First Serve
   _NHOL, // No Head Of Line blocking, means FIFO with ability to skip stalled tasks/messages.
   _LIFO, // Last In, First Out
   _RAND, // Random pick from queue
   _NPRI, // Non preemptive Priority
   _PPRI, // Preemptive Priority (with resume)
   _CFS   // Completely Fair Scheduler, whatever it is linux has I guess
};
}

#endif // BUMBER_CONSTS_H_INCLUDED
