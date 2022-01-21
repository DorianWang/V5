#ifndef SIMPLEROUTER_H
#define SIMPLEROUTER_H

#include "systemc.h"
#include "SimpleQueue.h"
#include "PacketType.h"
#include <map>

class SimpleRouter : public sc_module, public SimpleQueue
{
   public:
      sc_in_clk     clock ;      // Clock input of the design
      sc_in<bool>   enable;      // Active high to enable. Mostly for testing right now.

      std::map <uint32_t, QueuedType*> routingTable;

      void routePacket();
      bool addDest(uint32_t address, QueuedType* target);

      SC_CTOR(SimpleRouter) : SimpleQueue(8, QueueRules::DROP_RECEIVE) {
         SC_METHOD(routePacket); // Run incr_counter() as SystemC function.
         // https://www.doulos.com/knowhow/systemc/systemc-tutorial/modules-and-processes/
         sensitive << clock.pos();
      }


   private:
};

#endif // SIMPLEROUTER_H
