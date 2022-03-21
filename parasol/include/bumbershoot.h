#ifndef BUMBERSHOOT_H_INCLUDED
#define BUMBERSHOOT_H_INCLUDED
/// This should have all the functions and includes that a user of the library will need.
#include "bumber_consts.h"
#include "bumber_shared.h"


void code_tester(void* tPtr);

namespace bbs{

   // SystemC types, need special attention. Return value is success value, index is used to access.
   int bs_build_node(uint_fast32_t* index, const std::string& name, int ncpus, double speed,
                     double quantum, QDiscipline discipline, int statFlag);
   int bs_build_port(int stat, size_t dst_node = 0, size_t assoc_task = 0, QDiscipline discipline = BS_FIFO);
   int bs_build_bus(std::string name, int_fast64_t trate, QDiscipline discipline = BS_FIFO);
   int bs_build_link(std::string name, int_fast64_t trate);
   // Technically does not need a sc_module, but TaskThread is linked with it.
   int bs_build_task(std::string name, uint_fast32_t node, uint_fast32_t host, void (*code)(void*));
   // Builds a TaskThread, which creates SystemC threads.
   int bs_assign_task(uint_fast32_t node, uint_fast32_t task, uint_fast32_t host = NULL_INDEX);


   int bs_connect_link(uint_fast32_t src, uint_fast32_t dst);
   int bs_connect_bus(uint_fast32_t node);
   int bs_disconnect_bus(uint_fast32_t node);

   size_t bs_build_message(); // This is mostly for external callers so they don't need bs_message_t

   // How much can the old stats struct be reused?
   int bs_build_stat();


   int assign_port();



   int wait_for_message(void* tvPtr, uint_fast32_t portIndex, int timeout);

   int send_link(void* tvPtr, uint_fast32_t link, uint_fast32_t port, uint_fast32_t type, uint_fast32_t length, const std::string& text, uint_fast32_t ackPort);
   int send_bus(void* tvPtr, uint_fast32_t bus, uint_fast32_t port, uint_fast32_t type, uint_fast32_t length, const std::string& text, uint_fast32_t ackPort);




}




#endif // BUMBERSHOOT_H_INCLUDED
