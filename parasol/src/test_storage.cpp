
#include "test_storage.h"
#include <cstring>




int add_node(const char* name, long ncpu, double speed, double quantum, long discipline, long sf)
{
   ps_node_t* newNode = new ps_node_t;
   if (newNode == nullptr) throw; // Out of memory, just suicide for now.

   // Would rather use std::string, but might be a bit too much work to refactor for me right now.
   newNode->name = new char[strlen(name) + 1];
   if (newNode->name == nullptr) throw; // Out of memory, just suicide for now.
   strcpy(newNode->name, name);

   newNode->ncpu = ncpu; newNode->nfree = ncpu;
   return 0;
}
