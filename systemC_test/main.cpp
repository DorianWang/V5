#include <iostream>

#include "systemc.h"

#include "ServerExample.h"
#include "ClientExample.h"
#include "SimpleRouter.h"


// First SystemC stuff use: http://www.asic-world.com/systemc/first1.html#Hello_World_Program
using std::cout;
using std::endl;

// If no main in program but "systemc.h" is included
// sc_main will be run instead like a main function.
int sc_main (int argc, char* argv[])
{
   sc_signal<bool> clock;
   sc_signal<bool> enable;
   size_t i = 0;

   SimpleRouter rt("RT");
   ServerExample sr("SR");
   ClientExample cl("CL");
   sr.router = &rt; cl.router = &rt;
   sr.addr = 1; cl.addr = 3;
   rt.addDest(1, &sr); rt.addDest(3, &cl);

   rt.clock(clock); sr.clock(clock); cl.clock(clock);
   rt.enable(enable); sr.enable(enable); cl.enable(enable);

   // Starts scheduler, then runs 1 delta cycle, still don't quite understand.
   // See section "4.3.4.2 Function sc_start" in IEEE Std 1666-2011.
   // https://ieeexplore.ieee.org/document/6134619, appears to be latest standard.
   sc_start(0, SC_NS); // SC_NS is from enum sc_time_unit in sc_time.h. It has a value of 2.

   //sc_trace_file* wf = sc_create_vcd_trace_file("counter");
   //sc_trace(wf, clock, "clock");
   //sc_trace(wf, enable, "enable");
   //sc_trace(wf, counter_out, "count");

   enable = 0;
   for (i = 0; i < 6; i++){
      clock = 0;
      sc_start(1, SC_NS);
      clock = 1;
      sc_start(1, SC_NS);
   }

   enable = 1;
   cout << "@" << sc_time_stamp() << " || Activate enable" << endl;
   for (i = 0; i < 14; i++){
      clock = 0;
      sc_start(1, SC_NS);
      clock = 1;
      sc_start(1, SC_NS);
   }

   for (i = 0; i < 32; i++){
      clock = 0;
      sc_start(1, SC_NS);
      clock = 1;
      sc_start(1, SC_NS);
   }


   enable = 0;
   cout << "@" << sc_time_stamp() << " || Deactivate enable" << endl;
   cout << "@" << sc_time_stamp() << " || Ending sumulation" << endl;
   //sc_close_vcd_trace_file(wf);
   return 0;
}



// https://forums.accellera.org/topic/6092-c-class-object-bringing-into-systemc-hierarchy/


