#ifndef BUMBER_CONSTS_H_INCLUDED
#define BUMBER_CONSTS_H_INCLUDED

#include "systemc.h"
#include <cstdint>

#define DEFAULT_TIME_TICK SC_US
#define DEFAULT_SMALLER_TICK SC_NS
#define DEFAULT_SMALLEST_TICK SC_FS
#define TICK_CONV_MULT 1000 // SI unit prefix constant, in case we have to support others.

#define NULL_INDEX ((size_t)(-1))
#define NO_TIMEOUT ((int)(-1))

#define DEFAULT_DYE_ID ((unsigned int)(-1))
#define DEFAULT_PRIORITY ((unsigned int)(10))

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

enum ResourceState{
   BS_IDLE,
   BS_BUSY
};

enum TaskState{
   BS_READY,      // Ready to run
   BS_COMPUTING,  // Currently executing
   BS_BLOCKED,    // Waiting on a blocking call
   BS_PREEMPTED,  // Preempted by another task, will set back to BS_READY or BS_SUSPENDED.
   BS_SUSPENDED,  // Thread is not executing and is waiting to resume. Currently not different from BS_READY.
   BS_COMPLETE    // Thread has finished execution, and would need to be reset before executing again.
};

enum FuncRes{
   FUNC_GOOD,
   FUNC_TIME,     // Function timed out
   FUNC_INTR,     // A wait was interrupted, so the function returned.
   FUNC_ERR,      // Some sort of problem happened, and execution may need to be aborted.
   FUNC_FAIL,     // Function did not succeed, but program state is fine.
};

enum MessCCode{   // What was the source of the message
   BS_BUS,
   BS_LINK,
   BS_MAGIC
};

}

#endif // BUMBER_CONSTS_H_INCLUDED
