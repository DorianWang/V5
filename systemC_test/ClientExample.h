#ifndef CLIENTEXAMPLE_H
#define CLIENTEXAMPLE_H

#include "systemc.h"
#include "PacketType.h"
#include "SimpleQueue.h"
#include "SimpleRouter.h"


class ClientExample : public sc_module, public SimpleQueue
{
   public:
      sc_in_clk     clock ;      // Clock input of the design
      sc_in<bool>   enable;      // Active high to enable. Mostly for testing right now.

      SimpleRouter* router;
      uint32_t addr;
      uint64_t nextPID;

      void send_stuff();
      bool put_packet(Packet newPacket);

      // Should make a different kind of queue for clients instead of overriding function (null queue?)
      SC_CTOR(ClientExample) : SimpleQueue(4, QueueRules::DROP_RECEIVE) {
         std::cout << "Executing new client" << std::endl;
         nextPID = 0;
         SC_METHOD(send_stuff);
         // https://www.doulos.com/knowhow/systemc/systemc-tutorial/modules-and-processes/
         sensitive << clock.pos();
         dont_initialize(); // Don't run send_stuff yet?
      } // End of Constructor

   private:
};

#endif // CLIENTEXAMPLE_H
