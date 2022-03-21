
#include "bumbershoot.h"

#include "bumber_consts.h"
#include "bumber_shared.h"
#include "Message.h"

#include "Bus.h"
#include "Link.h"
#include "TaskSC.h"
#include "NodeSC.h"
#include "QueuedPort.h"

#include <iostream>
#include <string>
#include <queue>



class TopTest : public sc_module{
public:

   sc_event testEvent;

   void hello(){
      std::cout << sc_time_stamp() << " @ Hello World!" << std::endl;
   };
   void killer(){
      wait(1000, DEFAULT_TIME_TICK); // 1 millisecond
      bbs::TaskThread sendTester("Node0", 0, 1, 0);
      sendTester.resume.notify(SC_ZERO_TIME);
      wait(1000000, DEFAULT_TIME_TICK); // 1 second
      std::cout << sc_time_stamp() << std::endl;
      sc_stop();
   };
   TopTest(sc_module_name name, size_t index) : sc_module(name){
      SC_HAS_PROCESS(TopTest);
      bbs::bm_node_tab.add_entry(4);

      bbs::bm_port_vec.emplace_back(0, 0, 0);
      bbs::bm_port_vec.emplace_back(2, 1, 0);
      bbs::bm_port_vec.emplace_back(1, 2, 0);
      bbs::bm_port_vec.emplace_back(3, 3, 0);

      bbs::bm_link_tab.add_entry(2);

      bbs::bm_link_tab[0].init(5, 0, 2); // Link from 0 -> 2
      bbs::bm_link_tab[1].init(2, 3, 1); // Link from 3 -> 1

      bbs::bm_node_tab[0].init(2, 1.0, 1.0, bbs::BS_FIFO, 0);
      bbs::bm_node_tab[1].init(1, 1.0, 1.0, bbs::BS_FIFO, 0);
      bbs::bm_node_tab[2].init(4, 1.0, 1.0, bbs::BS_FIFO, 0);
      bbs::bm_node_tab[3].init(1, 1.0, 1.0, bbs::BS_FIFO, 0);

      bbs::bm_task_tab.add_entry();
      bbs::bm_task_tab[0].init(0, 1, code_tester);
      //for (bbs::NodeSC& iter : bbs::bm_node_tab){
      //   std::cout << iter.BSname << std::endl;
      //   iter.init(2, 1.2, 0.5, bbs::BS_FIFO, 1);
      //}

      testEvent.notify(12, DEFAULT_TIME_TICK);
      SC_METHOD(hello);
      sensitive << testEvent;
      dont_initialize();

      SC_THREAD(killer);
   }
};

#ifdef BUMBERSHOOT_TESTING


void code_tester(void* tvPtr)
{
   std::cout << "Testing code!" << std::endl;
   bbs::TaskThread* tPtr = reinterpret_cast<bbs::TaskThread*>(tvPtr);
   std::cout << tPtr->parentName << ": " << tPtr->taskIndex << std::endl;
   if (bbs::send_link(tvPtr, 0, 1, 0, 28000, "Hello!", 0) == 0){
      std::cout << "send_link success" << std::endl;
   }

   wait(40, DEFAULT_TIME_TICK);
}


// sc_main should probably be implemented in lqsim, and it should eventually call bs_run_bumbershoot()
int sc_main(int argc, char** argv)
{
   using namespace bbs;
   std::cout << "Hello World!" << std::endl;
   std::cout << "Hello Sky!" << std::endl;

   TopTest testing("TopTester", 0);


   std::cout << "Hello Sea!" << std::endl;
   std::cout << "End of main!" << std::endl;
   sc_start();

   return 0;
}

#endif

namespace bbs{
   bs_message_pool bs_mess_pool; // Temporary initialization here.
   uint_fast64_t bs_message_pool::nextMessageID; // Should be initialized to 0
   bm_table_t <Bus> bm_bus_tab("Bus");
   bm_table_t <Link> bm_link_tab("Link");
   bm_table_t <NodeSC> bm_node_tab("Node");
   bm_table_t <TaskSC> bm_task_tab("Task");
   std::vector <QueuedPort> bm_port_vec;

   // So basically this function should be what the ps_run_parasol function was.
   int bs_run_bumbershoot()
   {
      return 0;
   }

   // timeout is an integer of units DEFAULT_SMALLER_TICK
   // if timeout < 0, just wait for event.
   int wait_for_message(void* tvPtr, uint_fast32_t portIndex, int timeout)
   {
      TaskThread* tPtr = reinterpret_cast<TaskThread*>(tvPtr);
      if (portIndex >= bm_port_vec.size()){
         // Port does not exist, give warning/error.
         return FUNC_ERR;
      }
      QueuedPort& port = bm_port_vec[portIndex];
      if (port.get_associated_node() != tPtr->nodeIndex){
         return FUNC_ERR; // Port not owned by executing node.
      }
      if (port.num_queued()){
         return FUNC_GOOD;
      }
      if (timeout < 0){
         tPtr->state = BS_BLOCKED;
         wait(*(port.message_ready) | tPtr->wakeUp);
         if (tPtr->state == BS_PREEMPTED) return FUNC_INTR; // Interrupted
         tPtr->state = BS_COMPUTING;
         return FUNC_GOOD;
      }
      else{
         tPtr->state = BS_BLOCKED;
         wait(timeout, DEFAULT_SMALLER_TICK, *(port.message_ready) | tPtr->wakeUp);
         if (tPtr->state == BS_PREEMPTED) return FUNC_INTR; // Interrupted
         tPtr->state = BS_COMPUTING;
         return ((port.num_queued()) ? FUNC_GOOD : FUNC_TIME);
      }
   }

   int send_link(void* tvPtr, uint_fast32_t link, uint_fast32_t port, uint_fast32_t type, uint_fast32_t length, const std::string& text, uint_fast32_t ackPort)
   {
      std::cout << "Sending message!" << std::endl;
      if (port >= bm_port_vec.size()) return FUNC_ERR;
      if (link >= bm_link_tab.size()) return FUNC_ERR;
      Link& linkRef = bm_link_tab.at(link);
      TaskThread* tPtr = reinterpret_cast<TaskThread*>(tvPtr);
      if (linkRef.snode != tPtr->nodeIndex) return FUNC_FAIL; // Does not own link.
      if (bm_port_vec[port].get_associated_node() != linkRef.dnode) return FUNC_FAIL; // Is not link end.
      uint_fast32_t mid = bs_mess_pool.get_mess();
      bs_mess_pool[mid] = bs_message_t {tPtr->taskIndex, port, ackPort, BS_LINK, link, type, length, bs_mess_pool.get_next_mid(), DEFAULT_DYE_ID, DEFAULT_PRIORITY, sc_time_stamp().value(), text};
      linkRef.push_message(mid);
      std::cout << "Sent a message using a link @ " << sc_time_stamp().value() / 1000 << "ns" << std::endl;
      return FUNC_GOOD;
   }

   int send_bus(void* tvPtr, uint_fast32_t bus, uint_fast32_t port, uint_fast32_t type, uint_fast32_t length, const std::string& text, uint_fast32_t ackPort)
   {
      if (port >= bm_port_vec.size()) return FUNC_ERR;
      if (bus >= bm_bus_tab.size()) return FUNC_ERR;
      Bus& busRef = bm_bus_tab.at(bus);
      TaskThread* tPtr = reinterpret_cast<TaskThread*>(tvPtr);
      if (busRef.find_node(tPtr->nodeIndex) == false) return FUNC_FAIL; // Sender not on bus.
      if (busRef.find_node(bm_port_vec[port].get_associated_node()) == false) return FUNC_FAIL; // Port not on bus.
      uint_fast32_t mid = bs_mess_pool.get_mess();
      bs_mess_pool[mid] = bs_message_t {tPtr->taskIndex, port, ackPort, BS_BUS, bus, type, length, bs_mess_pool.get_next_mid(), DEFAULT_DYE_ID, DEFAULT_PRIORITY, sc_time_stamp().value(), text};
      busRef.push_message(mid);
      return FUNC_GOOD;
   }
























}
