#ifndef BUMB_SHARED_H_INCLUDED
#define BUMB_SHARED_H_INCLUDED

#include "bumber_consts.h"
#include "Message.h"

#include <exception>
#include <cstddef>   // Used for std::max_align_t

/**
Declares all the shared tables except for the message pool, which is in Message.h.
Also has the base sc_module class and the stats class.
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
   bbs_sc_module(sc_module_name name, size_t index) : sc_module(name), index(index){
      this->BSname = std::string(this->name());
   };
   size_t get_index() const { return index; };
   void set_index(size_t newIndex){ index = newIndex; };
   std::string BSname;

protected:
   size_t index;
};


struct bs_stat_t {			/* statistics struct	*/
   std::string	name;				/* statistic name	*/
   double resid;				/* non-rounding resid	*/
   // What does resid mean?
   // It seems to be residual, I guess for extra precision?
   union {
      struct {
         long count;		/* sample count		*/
         double sum;		/* sample sum		*/
      } sam;
      struct {
         double start;		/* start time		*/
         double old_value;	/* previous value	*/
         double old_time;	/* previous time	*/
         double integral;	/* variable integral	*/
      } var;
      struct {
         double start;	/* start time		*/
         long count;		/* # observations	*/
      } rat;
   } values;
   StatType type;		/* statistic type	*/
};

// Used for all SystemC modules, as they don't have copy or move constructors.
// Unfortunately the base sc_vector class does not appear to be resizable.
// This container stores contiguous memory segments equal to PAGE_SIZE input objects.
// Then the pointers to these memory segments are stored. Each sc_module is around 550 bytes.
// I guess I'll have to delete these things too, but that'll be for later.
// The next version of SystemC proposes a resizable sc_vector, so this will be obsolete then.
// This might be overkill, but at least I had fun making it.
template <class T> class bm_table_t {			/* dynamic table for bbs_sc_module type */
public:
   static constexpr int PAGE_SIZE = 64; // Around 8 and a half 4KiB pages. Oh well.

   class bm_table_t_IType{
   protected:
      T* pageRef;
      size_t currLoc;
      bm_table_t <T>& owner;
   public:
      bm_table_t_IType(bm_table_t <T>& owner, size_t currLoc) : owner(owner), currLoc(currLoc){
         pageRef = owner.table[currLoc / bm_table_t::PAGE_SIZE];
      };
      T& operator*(){ return pageRef[currLoc % bm_table_t::PAGE_SIZE]; };
      const T& operator*() const { return pageRef[currLoc % bm_table_t::PAGE_SIZE]; };
      void operator++(){
         currLoc++;
         if (currLoc % bm_table_t::PAGE_SIZE == 0)
            pageRef = owner.table[currLoc / bm_table_t::PAGE_SIZE];
         };
      bool operator!=(const bm_table_t_IType& rhs){ return currLoc != rhs.currLoc; };
   };
   friend class bm_table_t_IType;
   typedef bm_table_t_IType iterator;
   typedef const bm_table_t_IType const_iterator;
   bm_table_t(const std::string& namePrefix) : namePrefix(namePrefix){
      sizeVal = sizeof(T);
      alignVal = alignof(T);
      if (alignVal > alignof(std::max_align_t))
         throw std::invalid_argument("Cannot align object to memory!");
      sizeVal += sizeVal % alignVal; // In case of weird offset values, not sure if even allowed.
      char* page = new char[sizeVal * PAGE_SIZE];
      table.push_back(reinterpret_cast<T*>(page));
      numStored = 0; maxStored = PAGE_SIZE;
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
      char* page = new char[sizeVal * PAGE_SIZE];
      table.push_back(reinterpret_cast<T*>(page));
      maxStored += PAGE_SIZE;
   }
   size_t add_entry(int num_create = 1){
      if (num_create <= 0) throw std::out_of_range(namePrefix + " table: num_create value not positive!");
      while (numStored + num_create > maxStored){
         add_page();
      }
      if (num_create == 1){
         T* newObj = new(table[numStored / PAGE_SIZE] + (numStored % PAGE_SIZE)) T(namePrefix, numStored);
         if (newObj == nullptr)
            throw std::runtime_error(namePrefix + " table: failed to allocate object in table!");// Panic and die.
         numStored++;
         return numStored - 1;
      }
      for (int i = 0; i < num_create; i++){
         T* newObj = new(table[numStored / PAGE_SIZE] + (numStored % PAGE_SIZE)) T(namePrefix + std::to_string(numStored), numStored);
         if (newObj == nullptr)
            throw std::runtime_error(namePrefix + " table: failed to allocate object in table!");// Panic and die.
         numStored++;
      }
      return numStored - 1; // Current index of last object in container.
   };

   size_t add_entry(std::string name, int num_create = 1){
      if (num_create <= 0) throw std::out_of_range(namePrefix + " num_create value not positive!");
      for (int i = 0; i < num_create; i++){
         if (numStored == maxStored) add_page();
         std::string tempName;
         if (num_create == 1){
            tempName = name;
         }
         else{
            tempName = name + std::to_string(i);
         }
         void* tempPtr = table[numStored / PAGE_SIZE] + (numStored % PAGE_SIZE);
         T* newObj = new(tempPtr) T(tempName, numStored);
         if (newObj == nullptr)
            throw std::runtime_error(namePrefix + " table: failed to allocate object in table!");// Panic and die.
         numStored++;
      }
      return numStored - 1;
   };

   T& at(size_t index){
      if (index >= numStored){
         throw std::out_of_range(namePrefix + " table: get_entry index out of range!");
      }
      return table[index / PAGE_SIZE][index % PAGE_SIZE];
   }
   T& operator[](size_t index){
      return table[index / PAGE_SIZE][index % PAGE_SIZE];
   }
   const T& operator[](size_t index) const {
      return table[index / PAGE_SIZE][index % PAGE_SIZE];
   }
   T& back(){
      return table[(numStored - 1) / PAGE_SIZE][(numStored - 1) % PAGE_SIZE];
   }

   iterator begin() { return bm_table_t_IType(*this, 0); }
   const iterator begin() const { return bm_table_t_IType(&this, 0); }
   iterator end() {return bm_table_t_IType(*this, numStored);}
   const iterator end() const {return bm_table_t_IType(&this, numStored);}

   // Currently no entry removal. This would be rather complicated and would need much more code.
protected:
   size_t alignVal;
   size_t sizeVal;
   size_t numStored;
   size_t maxStored;
   std::vector <T*> table;
   std::string namePrefix;
};


extern bm_table_t <Bus> bm_bus_tab;
extern bm_table_t <Link> bm_link_tab;
extern bm_table_t <NodeSC> bm_node_tab;
extern bm_table_t <TaskSC> bm_task_tab;

extern std::vector <QueuedPort> bm_port_vec;
extern std::vector <bs_stat_t> bm_stat_vec;

// Still need a stats table, but it doesn't need to be hooked up to the SystemC engine.

int bs_run_bumbershoot(); // Check if it actually compiles.



}
#endif // BUMB_SHARED_H_INCLUDED










