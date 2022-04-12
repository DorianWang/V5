#ifndef BUMBERSHOOT_H_INCLUDED
#define BUMBERSHOOT_H_INCLUDED
/// This header should declare all functions and includes that a user of the library will need.
#include "bumber_consts.h"
#include "bumber_shared.h"

#ifdef BUMBERSHOOT_TESTING
   void code_tester(void* tPtr);
   void return_tester(void* tPtr);
#endif

namespace bbs{

   // SystemC types, need special attention. Return value is index used to access, or NULL_INDEX.
   size_t bs_build_node(const std::string& name, int ncpus, double speed,
                     double quantum, QDiscipline discipline, int statFlag);
   size_t bs_build_port(int stat, size_t dst_node = 0, size_t assoc_task = 0, QDiscipline discipline = BS_FIFO);
   size_t bs_build_bus(std::string name, int_fast64_t trate, QDiscipline discipline = BS_FIFO);
   size_t bs_build_link(std::string name, int_fast64_t trate);
   // Technically does not need a sc_module, but TaskThread is linked with it.
   size_t bs_build_task(const std::string& name, uint_fast32_t node, uint_fast32_t host, void (*code)(void*));
   // Builds a TaskThread, which creates SystemC threads.
   int bs_assign_task(uint_fast32_t node, uint_fast32_t task, uint_fast32_t host = NULL_INDEX);


   int bs_connect_link(uint_fast32_t link, uint_fast32_t src, uint_fast32_t dst);
   int bs_connect_bus(uint_fast32_t bus, uint_fast32_t node);
   int bs_disconnect_bus(uint_fast32_t node);

   // This is mostly for external callers so they don't need bs_message_t
   // Currently unused, but could be used later.
   size_t bs_build_message(
      uint_fast32_t type,			/* message type code	*/
      uint_fast32_t length,		/* message size, used for bus_delay and link_delay	*/
      uint_fast32_t mid,			/* Unique message id, should this be 64 bit? Not sure exactly where this is set */
      uint_fast32_t did,			/* dye id, unimplemented		*/
      uint_fast32_t pri,			/* Message priority	*/
      std::string text				/* message text, maybe use a shared pointer instead?	*/);

   // How much can the old stats functions be reused?
   size_t bs_build_stat(StatType type);
   int bs_record_stat(size_t stat, uint_fast64_t timeDelta);

   // Assign port to task, and if task is associated with a node, change port target to node.
   int assign_port(uint_fast32_t port, uint_fast32_t task);

   // Waits of a message at the port. Returns FUNC_GOOD when a message is available.
   int wait_for_message(void* tvPtr, uint_fast32_t portIndex, int timeout);
   // Fails if no message at the port.
   int get_message_from_port(void* tvPtr, uint_fast32_t port, uint_fast32_t* mess);

   // Creates and sends a message using the specified link/bus to the target port
   int send_link(void* tvPtr, uint_fast32_t link, uint_fast32_t port, uint_fast32_t type, uint_fast32_t length, const std::string& text, uint_fast32_t ackPort);
   int send_bus(void* tvPtr, uint_fast32_t bus, uint_fast32_t port, uint_fast32_t type, uint_fast32_t length, const std::string& text, uint_fast32_t ackPort);

   int send_direct(void* tvPtr, uint_fast32_t port, uint_fast32_t type, uint_fast32_t length, const std::string& text, uint_fast32_t ackPort);

   void bs_find_host(void* tvPtr);


}




#endif // BUMBERSHOOT_H_INCLUDED
