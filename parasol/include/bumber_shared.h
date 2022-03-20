#ifndef BUMB_SHARED_H_INCLUDED
#define BUMB_SHARED_H_INCLUDED

#include "bumber_consts.h"
#include "Message.h"

#include <exception>
#include <cstddef>   // Used for std::max_align_t

#define NULL_INDEX ((size_t)(-1))

/**
Holds all the shared tables except for the message pool, as that is managed and shared in Message.h.
Also defines all the helper functions for finding IDs from pointers, pointers from IDs, etc.
**/

namespace bbs{
// All the forward definitions of classes!
class bs_message_t; class QueuedPort;

// These ones are bbs_sc_modules and need bm_table_t.
class Bus; class Link;
class NodeSC; class CpuSC;
class TaskSC;

class bbs_sc_module : public sc_module {
   // This class is used to store the index of objects.
   // Otherwise it just passes through name to sc_module.
public:
   bbs_sc_module(sc_module_name name, size_t index) : sc_module(name), index(index){};
   size_t get_index() const {return index;};
   void set_index(size_t newIndex){index = newIndex;};
protected:
   size_t index;
};


template <class T> struct bbs_putter : public sc_module {
   sc_event& sendEvent;
   T& owner;
   void send_message(){
      uint_fast32_t messID = owner.pop_message();
      bs_message_t* mp = bs_mess_pool.get_mp(messID);
      std::cout << mp->text << std::endl; // Link this up later.
   };
   bbs_putter(sc_module_name name, sc_event& sendEvent, T& owner) : sc_module(name), sendEvent(sendEvent), owner(owner){
      SC_HAS_PROCESS(bbs_putter);
      SC_METHOD(send_message);
      sensitive << sendEvent;
      dont_initialize();
   }
};



// Used for all SystemC modules, as they don't have copy or move constructors.
// Unfortunately the base sc_vector class does not appear to be resizable.
// This container stores contiguous memory segments equal to PAGE_SIZE input objects.
// Then the pointers to these memory segments are stored. Each sc_module is around 550 bytes.
// I guess I'll have to delete these things too, but that'll be for later.
// This might be overkill, but at least I had fun making it.

template <class T> class bm_table_t {			/* dynamic table for bbs_sc_module type */
public:
   static constexpr int PAGE_SIZE = 64; // Around 8 and a half 4KiB pages. Oh well.
   bm_table_t(const std::string& nameSuffix) : nameSuffix(nameSuffix){
      sizeVal = sizeof(T);
      alignVal = alignof(T);
      if (alignVal > alignof(std::max_align_t))
         throw std::invalid_argument("Cannot align object to memory!");
      sizeVal += sizeVal % alignVal; // In case of weird offset values, not sure if even allowed.
      char* page = new char[sizeVal * PAGE_SIZE];
      std::cout << alignof(page) << std::endl;
      table.push_back(reinterpret_cast<T*>(page));
      numStored = 0; maxStored = PAGE_SIZE;
      std::cout << table.back() << std::endl;
   };
   virtual ~bm_table_t(){
      size_t lastPage = numStored % PAGE_SIZE;
      size_t tabSize = table.size();
      for (int i = 0; i < lastPage; i++){
         table[tabSize - 1][i].~T();
      }
      for (int i = 0; i < tabSize - 1; i++){
         for (int j = 0; j < PAGE_SIZE; j++){
            table[i][j].~T();
         }
      }
      for (int i = tabSize - 1; i >= 0; i--){
         char* page = reinterpret_cast<char*>(table.back());
         table.pop_back();
         delete[] page; // Hope this isn't undefined behaviour lol.
      }
   };

   size_t size() const {return numStored;};
   void add_page(){
      char* page = new char(sizeVal * PAGE_SIZE);
      table.push_back(reinterpret_cast<T*>(page));
      maxStored += PAGE_SIZE;
   }
   size_t add_entry(int num_create = 1){
      if (num_create <= 0) throw std::out_of_range(nameSuffix + " table: num_create value not positive!");
      for (int i = 0; i < num_create; i++){
         if (numStored == maxStored) add_page();
         T* newObj = new(table[numStored / PAGE_SIZE][numStored % PAGE_SIZE]) T(nameSuffix + std::to_string(numStored), numStored);
         if (newObj == nullptr)
            throw std::runtime_error(nameSuffix + " table: failed to allocate object in table!");// Panic and die.
         numStored++;
      }
      return numStored - 1; // Current index of last object in container.
   };

   size_t add_entry(std::string name, int num_create = 1){
      if (num_create <= 0) throw std::out_of_range(nameSuffix + " num_create value not positive!");
      for (int i = 0; i < num_create; i++){
         if (numStored == maxStored) add_page();
         std::string tempName = name + std::to_string(i);
         void* tempPtr = table[numStored / PAGE_SIZE] + (numStored % PAGE_SIZE);
         std::cout << tempName << " " << tempPtr << std::endl;
         T* newObj = new(tempPtr) T(tempName, numStored);
         if (newObj == nullptr)
            throw std::runtime_error(nameSuffix + " table: failed to allocate object in table!");// Panic and die.
         numStored++;
      }
      return numStored - 1;
   };

   T& get_entry(size_t index){
      if (index >= numStored){
         throw std::out_of_range(nameSuffix + " table: get_entry index out of range!");
      }
      return table[numStored / PAGE_SIZE][numStored % PAGE_SIZE];
   }
   T& operator[](size_t index){
      return table[numStored / PAGE_SIZE][numStored % PAGE_SIZE];
   }
   // Currently no entry removal. This would be rather complicated and would need much more code.

private:
   size_t alignVal;
   size_t sizeVal;
   size_t numStored;
   size_t maxStored;
   std::vector <T*> table;
   std::string nameSuffix;
};


extern bm_table_t <Bus> bm_bus_tab;
extern bm_table_t <Link> bm_link_tab;
extern bm_table_t <NodeSC> bm_node_tab;
extern bm_table_t <TaskSC> bm_task_tab;

// Still need a stats table, but it doesn't need to be hooked up to the SystemC engine.

int bs_run_bumbershoot(); // Check if it actually compiles.


















}
#endif // BUMB_SHARED_H_INCLUDED






