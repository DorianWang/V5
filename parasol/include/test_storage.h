
#ifndef	TEST_STORAGE_H
#define	TEST_STORAGE_H

#include	"para_types.h"
//#include "para_protos.h"
#include "para_privates.h"

#include <vector>
#include <string>
#include <cstdint>
#include <iostream>

#include <exception>

#define NOT_IMPLEMENTED_ERR throw NotImplemented(__FILE__, __LINE__);
#define NOT_IMPLEMENTED_WAR std::cout << "Unimplemented at " << __FILE__ << " line " << __LINE__ << std::endl;


/** This class is just to try to restore some of the functionality that parasol had.
*** Mostly it's just about remaking the node/task/message structure without the stack.
*** Should be useful for our replacement, since we can use it to passthrough later.
**/
class TestStorage
{
public:

   int add_stat(const std::string& name, long type, double ps_now);
   int add_stat(const char* name, long type, double ps_now);
   ps_stat_t* get_stat(size_t index);
   int rem_stat(size_t index);

   std::vector <ps_stat_t*> sorted_stats(); // Sorted by name, analogue of qsort(..., stat_compare)


   int add_node(const char* name, long ncpu, double speed, double quantum, long discipline, long sf, double ps_now);
   ps_node_t* get_node(size_t index);
   int rem_node(size_t index);

   int add_task(const char* name, long node, long host, void (*code)(void*), long priority, double ps_now, long group = -1, double stackscale = 1.0); // ps_create2()
   ps_task_t* get_task(size_t index);
   long get_task_index(const ps_task_t* in);
   int rem_task(size_t index);

   // Constant getters
   const ps_stat_t* get_stat(size_t index) const;
   const ps_node_t* get_node(size_t index) const;
   const ps_task_t* get_task(size_t index) const;

   size_t get_stats_size() const {return stats.size();};
   size_t get_nodes_size() const {return nodes.size();};
   size_t get_tasks_size() const {return tasks.size();};

   static void print_stat(const ps_stat_t* in);
   static void print_cpu(const ps_cpu_t* in, const TestStorage* db = nullptr); // Used for print_node
   static void print_node(const ps_node_t* in, const TestStorage* db = nullptr);
   static void print_task(const ps_task_t* in, const TestStorage* db = nullptr);

   void print_all_stored() const;



private:
   // A bit more work to handle pointers, but less parasol code changes are needed.
   // Since the number of nodes appears to be relatively static, a vector should be fine.
   // Shared pointers might be better, but this hopefully isn't the final product.
   std::vector <ps_stat_t*> stats;
   std::vector <ps_node_t*> nodes;
   std::vector <ps_task_t*> tasks;

};


class NotImplemented : public std::logic_error
{
public:
   NotImplemented(const char* fileName, size_t lineNum) : std::logic_error(""){
      message = "Unimplemented section reached in ";
      message += fileName; message += " line ";
      message += std::to_string(lineNum);
   }
   std::string message;
   virtual const char* what() const noexcept {return message.c_str();};
};










#endif // TEST_STORAGE_H

