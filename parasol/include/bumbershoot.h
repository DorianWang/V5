#ifndef BUMBERSHOOT_H_INCLUDED
#define BUMBERSHOOT_H_INCLUDED
/// This should have all the functions and includes that a user of the library will need.
#include "bumber_consts.h"
#include "bumber_shared.h"


void code_tester(void* tPtr);

namespace bbs{

   int wait_for_message(void* tvPtr, uint_fast32_t portIndex, int timeout);

   int send_link(void* tvPtr, uint_fast32_t link, uint_fast32_t port, uint_fast32_t type, uint_fast32_t length, const std::string& text, uint_fast32_t ackPort);
   int send_bus(void* tvPtr, uint_fast32_t bus, uint_fast32_t port, uint_fast32_t type, uint_fast32_t length, const std::string& text, uint_fast32_t ackPort);

}




#endif // BUMBERSHOOT_H_INCLUDED
