
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
      bbs::TaskThread recTester("Node2", 2, 3, 1);
      sendTester.resume.notify(250, DEFAULT_TIME_TICK);
      recTester.resume.notify(100, DEFAULT_TIME_TICK);
      wait(1000, DEFAULT_TIME_TICK); // 1 millisecond
      wait(4000000, DEFAULT_TIME_TICK); // 1 second
      std::cout << sc_time_stamp() << std::endl;
      sc_stop();
   };
   TopTest(sc_module_name name, size_t index) : sc_module(name){
      SC_HAS_PROCESS(TopTest);
      bbs::bm_node_tab.add_entry(4);

      bbs::bm_port_vec.emplace_back(0);
      bbs::bm_port_vec.emplace_back(1);
      bbs::bm_port_vec[0].init(0, 0, 0);
      bbs::bm_port_vec[1].init(2, 1, 0);

      bbs::bm_link_tab.add_entry(2);

      bbs::bm_link_tab[0].init(5, 0, 2); // Link from 0 -> 2
      bbs::bm_link_tab[1].init(2, 3, 1); // Link from 3 -> 1

      bbs::bm_bus_tab.add_entry();
      bbs::bm_bus_tab[0].init(200);

      bbs::bm_node_tab[0].init(2, 1.0, 1.0, bbs::BS_FIFO, 0);
      bbs::bm_node_tab[1].init(1, 1.0, 1.0, bbs::BS_FIFO, 0);
      bbs::bm_node_tab[2].init(4, 1.0, 1.0, bbs::BS_FIFO, 0);
      bbs::bm_node_tab[3].init(1, 1.0, 1.0, bbs::BS_FIFO, 0);

      bbs::bs_connect_bus(0, 0);
      bbs::bs_connect_bus(0, 2);

      bbs::bm_task_tab.add_entry(2);
      bbs::bm_task_tab[0].init(0, 1, code_tester);
      bbs::bm_task_tab[1].init(2, 3, return_tester);
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
   std::cout << sc_time_stamp() << " @ Testing send!" << std::endl;
   bbs::TaskThread* tPtr = reinterpret_cast<bbs::TaskThread*>(tvPtr);
   // std::cout << tPtr->parentName << ": " << tPtr->taskIndex << std::endl;
   if (bbs::send_link(tvPtr, 0, 1, 0, 28000, "Hello!", 0) == bbs::FUNC_GOOD){
      std::cout << "send_link success" << std::endl;
   }
   int res = bbs::wait_for_message(tvPtr, 0, -1);
   if (res != bbs::FUNC_GOOD){
      std::cout << "waiting for message was interrupted @ " << sc_time_stamp() << std::endl;
   }
   uint_fast32_t mres;
   bbs::get_message_from_port(tvPtr, 0, &mres);
   std::string received = bbs::bs_mess_pool[mres].text;
   std::cout << sc_time_stamp() << " @ Received message: " << received << std::endl;

   wait(40, DEFAULT_TIME_TICK);
}

void return_tester(void* tvPtr)
{
   std::cout << sc_time_stamp() << " @ Testing receive -> return!" << std::endl;
   bbs::TaskThread* tPtr = reinterpret_cast<bbs::TaskThread*>(tvPtr);
   int res = bbs::wait_for_message(tvPtr, 1, -1); // Wait for message on port 1.
   if (res == bbs::FUNC_GOOD){
      std::cout << sc_time_stamp() << " @ Sending return message!" << std::endl;
      uint_fast32_t mres;
      bbs::get_message_from_port(tvPtr, 1, &mres);
      std::string received = bbs::bs_mess_pool[mres].text;
      std::string tosend = received + " Hey There!";
      std::cout << "Received message: " << received << ", Sending message: " << tosend << std::endl;
      if (bbs::send_bus(tvPtr, 0, 0, 0, 2800, tosend, 0) == bbs::FUNC_GOOD){
         std::cout << sc_time_stamp() << " @ send_bus success" << std::endl;
      }
   }
   else {
      // std::cout << "Ow!" << std::endl;
      std::cout << res << std::endl;
      // It would wait if interrupted, or print errors if it was FUNC_ERR
   }
}

// sc_main should probably be implemented in lqsim, and it should eventually call bs_run_bumbershoot()
int sc_main(int argc, char** argv)
{
   using namespace bbs;

   TopTest testing("TopTester", 0);
   std::cout << "Starting simulation!" << std::endl;
   sc_start();

   return 0;
}

#endif

namespace bbs{
   bs_message_pool bs_mess_pool; // Temporary initialization here.
   uint_fast64_t bs_message_pool::nextMessageID = 0; // Should automatically be initialized to 0
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


   int bs_build_node(uint_fast32_t* index, const std::string& name, int ncpus, double speed,
                     double quantum, QDiscipline discipline, int statFlag)
   {
      if (index == nullptr) return FUNC_ERR;
      if (ncpus < 1 || speed < 0.0 || quantum < 0.0) return FUNC_ERR;
      if (name.length() == 0){
         bm_node_tab.add_entry();
      }
      else {
         bm_node_tab.add_entry(name);
      }
      NodeSC& temp = bm_node_tab.back();
      *index = temp.get_index();
      temp.init(ncpus, speed, quantum, discipline, statFlag);
      return FUNC_GOOD;
   }

   int bs_build_port(int stat, size_t dst_node, size_t assoc_task, QDiscipline discipline)
   {
      size_t newIndex = bm_port_vec.size();
      bm_port_vec.emplace_back(newIndex);
      bm_port_vec[newIndex].init(dst_node, assoc_task, stat, discipline);
      return FUNC_GOOD;
   }

   int bs_build_bus(std::string name, int_fast64_t trate, QDiscipline discipline)
   {
      if (name.length()){
         bm_bus_tab.add_entry(name);
      }
      else {
         bm_bus_tab.add_entry();
      }
      Bus& newBus = bm_bus_tab.back();
      newBus.init(trate, discipline);
      return FUNC_GOOD;
   }

   int bs_build_link(std::string name, int_fast64_t trate)
   {
      if (name.length()){
         bm_link_tab.add_entry(name);
      }
      else {
         bm_link_tab.add_entry();
      }
      Link& newLink = bm_link_tab.back();
      newLink.init(trate, NULL_INDEX, NULL_INDEX);
      return FUNC_GOOD;
   }

   int bs_build_task(std::string name, uint_fast32_t node, uint_fast32_t host, void (*code)(void*))
   {
      if (name.length()){
         bm_task_tab.add_entry(name);
      }
      else {
         bm_task_tab.add_entry();
      }
      TaskSC& newTask = bm_task_tab.back();
      newTask.init(node, host, code);
      return FUNC_GOOD;
   }

   int bs_assign_task(uint_fast32_t node, uint_fast32_t task, uint_fast32_t host)
   {
      if (node >= bm_node_tab.size()) return FUNC_ERR;
      if (task >= bm_task_tab.size()) return FUNC_ERR;
      NodeSC& nref = bm_node_tab[node];
      TaskSC& tref = bm_task_tab[task];
      TaskThread* newThread = new TaskThread(nref.BSname + '_' + tref.BSname, node, host, task);
      if (newThread == nullptr) return FUNC_ERR;
      nref.rtrq.push_back(newThread);
      tref.state = BS_READY; tref.node = node; tref.host = host;
      return FUNC_GOOD;
   }

   int bs_connect_link(uint_fast32_t link, uint_fast32_t src, uint_fast32_t dst)
   {
      if (link >= bm_link_tab.size()) return FUNC_ERR;
      Link& lref = bm_link_tab[link];
      lref.set_link(src, dst);
      if (src < bm_node_tab.size()) bm_node_tab[src].linkSrcIDs.insert(link);
      if (dst < bm_node_tab.size()) bm_node_tab[dst].linkDstIDs.insert(link);
      return FUNC_GOOD;
   }

   int bs_connect_bus(uint_fast32_t bus, uint_fast32_t node)
   {
      if (node >= bm_node_tab.size()) return FUNC_ERR;
      if (bus >= bm_bus_tab.size()) return FUNC_ERR;
      Bus& bref = bm_bus_tab[bus];
      bref.add_node(node);
      bm_node_tab[node].busIDs.insert(bus);
      return FUNC_GOOD;
   }

   int bs_disconnect_bus(uint_fast32_t bus, uint_fast32_t node)
   {
      if (node >= bm_node_tab.size()) return FUNC_ERR;
      if (bus >= bm_bus_tab.size()) return FUNC_ERR;
      Bus& bref = bm_bus_tab[bus];
      if (bref.remove_node(node)){
         if (bm_node_tab[node].remove_bus_connection(bus)){
            return FUNC_GOOD;
         }
         return FUNC_FAIL; // Maybe also give a warning/error?
      }
      else {
         return FUNC_FAIL;
      }
   }

   // Currently unused, but could be used later.
   size_t bs_build_message(
   uint_fast32_t type,			/* message type code	*/
   uint_fast32_t length,		/* message size, used for bus_delay and link_delay	*/
   uint_fast32_t mid,			/* Unique message id, should this be 64 bit? Not sure exactly where this is set */
   uint_fast32_t did,			/* dye id, unimplemented		*/
   uint_fast32_t pri,			/* Message priority	*/
   std::string text				/* message text, maybe use a shared pointer instead?	*/)
   {
      size_t index = bs_mess_pool.get_mess();
      bs_message_t& mess = bs_mess_pool[index];
      mess.ts = sc_time_stamp().value();
      mess.type = type;
      mess.mid = mid;
      mess.pri = pri;
      mess.text = text;
      return index;
   }

   int assign_port(uint_fast32_t port, uint_fast32_t task)
   {
      if (port >= bm_port_vec.size()) return FUNC_ERR;
      if (task >= bm_task_tab.size()) return FUNC_ERR;
      QueuedPort& pref = bm_port_vec[port];
      TaskSC& tref = bm_task_tab[task];
      if (tref.node < bm_node_tab.size()){
         pref.set_associated_node(tref.node);
      }
      pref.set_associated_task(task);
      tref.port_list.push_back(port);

      return FUNC_GOOD;
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
         std::cout << "Waiting for message at port " << portIndex << " @ " << sc_time_stamp() << std::endl;
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

   // If there is no message at the port, returns a timeout value.
   int get_message_from_port(void* tvPtr, uint_fast32_t port, uint_fast32_t* mess)
   {
      TaskThread* tPtr = reinterpret_cast<TaskThread*>(tvPtr);
      if (port >= bm_port_vec.size()) return FUNC_ERR;
      if (tPtr->nodeIndex != bm_port_vec[port].get_associated_node()) return FUNC_FAIL; // Does not own port.
      QueuedPort& pref = bm_port_vec[port];
      if (pref.num_queued() == 0) return FUNC_FAIL;
      pref.get_message(mess);
      return FUNC_GOOD;
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
      uint_fast32_t index = bs_mess_pool.get_mess();
      bs_mess_pool[index] = bs_message_t {tPtr->taskIndex, port, ackPort, BS_LINK, link, type, length, bs_mess_pool.get_next_mid(), DEFAULT_DYE_ID, DEFAULT_PRIORITY, sc_time_stamp().value(), text};
      linkRef.push_message(index);
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
      uint_fast32_t index = bs_mess_pool.get_mess();
      bs_mess_pool[index] = bs_message_t {tPtr->taskIndex, port, ackPort, BS_BUS, bus, type, length, bs_mess_pool.get_next_mid(), DEFAULT_DYE_ID, DEFAULT_PRIORITY, sc_time_stamp().value(), text};
      busRef.push_message(index);
      return FUNC_GOOD;
   }

   void bs_find_host(void* tvPtr)
   {
      TaskThread* tPtr = reinterpret_cast<TaskThread*>(tvPtr);
      return;
   }



















}
