
#ifndef	TEST_STORAGE_H
#define	TEST_STORAGE_H

#include	"para_types.h"

#include <vector>
#include <string>
#include <cstdint>
#include <iostream>

class TestStorage
{
public:

   int add_node(const char* name, long ncpu, double speed, double quantum, long discipline, long sf);
   int get_node();
   int rem_node();






private:
   // A bit more work to handle pointers, but less parasol code changes are needed.
   // Since the number of nodes appears to be relatively static, a vector should be fine.
   std::vector <ps_node_t*> nodes;


};













#endif // TEST_STORAGE_H

