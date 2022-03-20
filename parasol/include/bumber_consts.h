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
   BS_NONE, // Unset, possible uninitialized error
   BS_FIFO, // First In, First Out AKA First Come, First Serve
   BS_NHOL, // No Head Of Line blocking, means FIFO with ability to skip stalled tasks/messages.
   BS_LIFO, // Last In, First Out
   BS_RAND, // Random pick from queue
   BS_NPRI, // Non preemptive Priority
   BS_PPRI, // Preemptive Priority (with resume)
   BS_CFS   // Completely Fair Scheduler, whatever it is linux has I guess
};

enum CPUState{
   BS_IDLE,
   BS_BUSY
};

enum TaskState{
   BS_READY,
   BS_COMPUTING,
   BS_BLOCKED
};

}

#endif // BUMBER_CONSTS_H_INCLUDED
