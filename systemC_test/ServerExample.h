#ifndef SERVEREXAMPLE_H_INCLUDED
#define SERVEREXAMPLE_H_INCLUDED

#include "systemc.h"
#include "SimpleQueue.h"
#include "SimpleRouter.h"
#include "PacketType.h"

#define DEFAULT_BUFFER_SIZE 4

using std::cout;
using std::endl;

class ServerExample : public sc_module, public SimpleQueue {
   public:
      sc_in_clk     clock ;      // Clock input of the design
      sc_in<bool>   enable;      // Active high to enable. Mostly for testing right now.

      SimpleRouter* router;
      uint32_t addr;
      void do_stuff();
      void sendPacket(); // Calls receivePacket on the target.


      SC_CTOR(ServerExample) : SimpleQueue(DEFAULT_BUFFER_SIZE, QueueRules::DROP_RECEIVE) {
         cout << "Executing new server" << endl;
         SC_METHOD(do_stuff); // Run incr_counter() as SystemC function.
         // https://www.doulos.com/knowhow/systemc/systemc-tutorial/modules-and-processes/
         sensitive << clock.pos();
      } // End of Constructor

   private:


};

#endif // SERVEREXAMPLE_H_INCLUDED




