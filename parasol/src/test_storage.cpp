
#include "test_storage.h"

#include <algorithm> // std::sort


// These are for my own personal testing/curiosity
#include <type_traits> // For is_pod<T>
#include <iomanip> // For stream modifiers like std::hex


int TestStorage::add_stat(const char* name, long type, double ps_now)
{
   return add_stat(std::string(name), type, ps_now);
}

int TestStorage::add_stat(const std::string& name, long type, double ps_now)
{
   ps_stat_t* nStat = new ps_stat_t;

   nStat->name = name; nStat->resid = 0.0; nStat->type = type;

   switch(type){
   case 87264502: //SAMPLE:
      nStat->values.sam.count = 0;
      nStat->values.sam.sum = 0.0;
      break;;
   case 29382731: //VARIABLE:
      nStat->values.var.start = nStat->values.var.old_time = ps_now;
      nStat->values.var.old_value = 0.0;
      nStat->values.var.integral = 0.0;
      break;
   case 43928290: //RATE:
      nStat->values.rat.start = ps_now;
      nStat->values.rat.count = 0;
      break;
   default:
      // Panic and die?
      break;
   }
   stats.push_back(nStat);
   return stats.size() - 1;
}

ps_stat_t* TestStorage::get_stat(size_t index)
{
   if (index < stats.size()){
      return stats[index];
   }
   else{
      std::cout << std::hex << index << std::endl;
      std::string temp = std::string("get_stat(") + std::to_string(index) + ") is out of range";
      throw std::out_of_range(temp);
   }
}


std::vector <ps_stat_t*> TestStorage::sorted_stats()
{
   std::vector <ps_stat_t*> sorted = stats;
   std::sort(sorted.begin(), sorted.end(), [](ps_stat_t* lhs, ps_stat_t* rhs)
            { return lhs->name.compare(rhs->name);});
   return sorted;
}



int TestStorage::add_node(const char* name, long ncpu, double speed, double quantum, long discipline, long sf, double ps_now)
{
   // This is probably a lot slower that old parasol, especially from inefficient string manipulation stuff.
   // No out of memory checks, just let the std::bad_alloc exceptions go.
   ps_node_t* nNode = new ps_node_t;

   nNode->name = std::string(name);
   nNode->ncpu = ncpu; nNode->nfree = ncpu; nNode->sf = sf;
   nNode->speed = speed; nNode->rtrq = NULL_TASK; nNode->quantum = quantum;
   // NULL_COMM_PTR = ((ps_comm_t*) 0)
   // nNode->sl_list = nullptr; nNode->rl_list = nullptr; nNode->bus_list = nullptr;
   nNode->ngroup = 0;


   nNode->build_time = ps_now; // Needs to get SystemC time somehow, but later.
   nNode->cpu = std::vector<ps_cpu_t>(ncpu); // vector with ncpu elements.
   nNode->stat = -1; // NULL_STAT
   nNode->ts_tab = nullptr;
   if (sf & 0x02){ // SF_PER_NODE
      nNode->stat = add_stat(nNode->name + " Utilization", 29382731, ps_now); //VARIABLE
      for (size_t i = 0; i < nNode->cpu.size(); i++){
         nNode->cpu[i].stat = add_stat(nNode->name + " (cpu " + std::to_string(i) +") Utilization", 29382731, ps_now);
      }
   }
   if (sf & 0x08){ // SF_PER_TASK_NODE
      nNode->ts_tab = new ps_table_t<long>(10); // DEFAULT_MAX_TASKS = 10
      long* it = nNode->ts_tab->tab();
      for (size_t i = 0; i < 10; i++){
         *it = -1; // NULL_STAT
         it++;
      }
      for (size_t i = 0; i < nNode->cpu.size(); i++){
         nNode->cpu[i].ts_tab = new ps_table_t<long>(10);
         it = nNode->cpu[i].ts_tab->tab();
         for (size_t i = 0; i < 10; i++){
            *it = -1; // NULL_STAT
            it++;
         }
      }
   }

   if (discipline == 5){ // CFS
      NOT_IMPLEMENTED_WAR;
   }
   else {
      nNode->host_rq = nullptr; //NULL_CFSRQ_PTR;
   }



   nodes.push_back(nNode);
   return nodes.size() - 1;
}

ps_node_t* TestStorage::get_node(size_t index)
{
   if (index < nodes.size()){
      return nodes[index];
   }
   else{
      std::cout << std::hex << index << std::endl;
      std::string temp = std::string("get_node(") + std::to_string(index) + ") is out of range";
      throw std::out_of_range(temp);
   }
}


int TestStorage::add_task(const char* name, long node, long host, void (*code)(void*), long priority, double ps_now, long group, double stackscale)
{
   ps_task_t* nTask = new ps_task_t;
   long port = 0; // ps_allocate_port("Broadcast", task);

   nTask->name = std::string(name);
   nTask->state = TASK_SUSPENDED; nTask->node = node; nTask->port_list = -1 ; //NULL_PORT
   nTask->code = code; nTask->host = nTask->uhost = host;
   nTask->hp = nullptr; // NULL_HOST_PTR;
   nTask->stack_base = nullptr; // This is the stack pointer which I'll leave alone
   nTask->stack_limit = 0;
   nTask->priority = nTask->upriority = priority;
   nTask->next = -1; // NULL_TASK;
   nTask->parent = 0;
   nTask->sibling = -1; // NULL_TASK;
   nTask->son = -1; // NULL_TASK;
   nTask->bport = port; nTask->blind_port = -1; nTask->wport = -1; // NULL_PORT;
   nTask->lock_list = -1; nTask->spin_lock = -1; // NULL_LOCK;
   nTask->tep = nTask->qep = nTask->rtoep = nullptr; // NULL_EVENT_PTR;
   nTask->rct = 0.0; nTask->qx = 0; nTask->tsn = task_count++;

   if (group >= 0) {NOT_IMPLEMENTED_WAR;};
   nTask->group = -1;
   nTask->group_id = -1;
   nTask->si = nullptr; // NULL_SCHED_PTR;

   // Ignore cfs check for now

   nTask->sched_time = ps_now; nTask->tbp = -2;
   tasks.push_back(nTask);
   return tasks.size() - 1;

} // ps_create2()


ps_task_t* TestStorage::get_task(size_t index)
{
   if (index < tasks.size()){
      return tasks[index];
   }
   else{
      std::cout << std::hex << index << std::endl;
      std::string temp = std::string("get_task(") + std::to_string(index) + ") is out of range";
      throw std::out_of_range(temp);
   }
}

long TestStorage::get_task_index(const ps_task_t* in)
{
   for (size_t i = 0; i < tasks.size(); i++){
      if (tasks[i] == in){
         return i;
      }
   }
   return -1; // Not found?
}

long TestStorage::get_task_index(const ps_task_t* in)
{
   for (size_t i = 0; i < tasks.size(); i++){
      if (tasks[i] == in){
         return i;
      }
   }
   return -1; // Not found?
}


/******************************************************/
/****************** Constant Getters ******************/
/******************************************************/
// They're the same, just return const pointers.
const ps_stat_t* TestStorage::get_stat(size_t index) const
{
   if (index < stats.size()){
      return stats[index];
   }
   else{
      throw std::out_of_range("get_stat(index) is out of range");
   }
}
const ps_node_t* TestStorage::get_node(size_t index) const
{
   if (index < nodes.size()){
      return nodes[index];
   }
   else{
      throw std::out_of_range("get_node(index) is out of range");
   }
}
const ps_task_t* TestStorage::get_task(size_t index) const
{
   if (index < tasks.size()){
      return tasks[index];
   }
   else{
      throw std::out_of_range("get_task(index) is out of range");
   }
}



/******************************************************/
/************** All the printing stuff! ***************/
/******************************************************/
// These assume that the given pointer is valid.
// The calling function should check before calling.

void TestStorage::print_stat(const ps_stat_t* in)
{
   std::cout << in->name << " " << in->resid;
   switch(in->type){
   case 87264502: //SAMPLE:
      std::cout << " SAMPLE " << in->values.sam.count << " " << in->values.sam.sum << "\n";
      break;
   case 29382731: //VARIABLE:
      std::cout << " VARIABLE " << in->values.var.start << " " << in->values.var.old_value
      << " " << in->values.var.old_time << " " << in->values.var.integral << "\n";
      break;
   case 43928290: //RATE:
      std::cout << " RATE " << in->values.rat.start << " " << in->values.rat.count << "\n";
      break;
   default:
      std::cout << " UNKNOWN TYPE\n";
      // Panic and die?
      break;
   }
}

void TestStorage::print_cpu(const ps_cpu_t* in, const TestStorage* db)
{
   // #define	CPU_IDLE	0 #define	CPU_BUSY	1 #define	CPU_DOWN	2
   switch(in->state){
   case 0:
      std::cout << "CPU_IDLE ";
      break;
   case 1:
      std::cout << "CPU_BUSY ";
      break;
   case 2:
      std::cout << "CPU_DOWN ";
      break;
   default:
      std::cout << "NOTVALID ";
      break;
   }

   if (in->run_task == -1){ // NULL_TASK -1
      std::cout << "NULL_TASK ";
   }
   else{
      if (db == nullptr){
         std::cout << in->run_task << " ";
      }
      else{
         try{
            const ps_task_t* running_task = db->get_task(in->run_task);
            std::cout << running_task->name << " ";
         } catch(std::out_of_range& e){
            std::cout << "INV_TASK:" << in->run_task << " ";
         }
      }
   }

   if (in->stat == -1){ // NULL_STAT -1
      std::cout << "NULL_STAT ";
   }
   else{
      if (db == nullptr){
         std::cout << in->stat << " ";
      }
      else{
         try{
            const ps_stat_t* sp = db->get_stat(in->stat);
            std::cout << sp->name << " ";
         } catch(std::out_of_range& e){
            std::cout << "INV_STAT:" << in->stat << " ";
         }
      }
   }

   std::cout << in->scheduler << " " << in->catcher << " ";

   if (in->last_task == -1){
      std::cout << "NULL_TASK ";
   }
   else{
      if (db == nullptr){
         std::cout << in->last_task << " ";
      }
      else{
         try{
            const ps_task_t* last_task = db->get_task(in->last_task);
            std::cout << last_task->name << " ";
         } catch(std::out_of_range& e){
            std::cout << "INV_TASK:" << in->last_task << " ";
         }
      }
   }

   std::cout << in->port_n << "\n";
   return;
}

void TestStorage::print_node(const ps_node_t* in, const TestStorage* db)
{
   std::cout << in->name << " " << in->ncpu << " " << in->nfree << "\n";
   for (size_t i = 0; i < in->cpu.size(); i++){
      print_cpu(in->cpu.data() + i);
   }

   std::cout << in->rtrq << " " << in->speed << " " << in->quantum << " " << in->discipline << " ";

   if (in->stat == -1){ // NULL_STAT -1
      std::cout << "NULL_STAT ";
   }
   else{
      if (db == nullptr){
         std::cout << in->stat << " ";
      }
      else{
         try{
            const ps_stat_t* sp = db->get_stat(in->stat);
            std::cout << sp->name << " ";
         } catch(std::out_of_range& e){
            std::cout << "INV_STAT:" << in->stat << " ";
         }
      }
   }
   std::cout << in->sf << " " << in->build_time << " " << in->ngroup << "\n";
}

void TestStorage::print_task(const ps_task_t* in, const TestStorage* db)
{
   std::cout << in->name;
   switch(in->state){
   case TASK_FREE:
      std::cout << " TASK_FREE ";
      break;
   case TASK_SUSPENDED:
      std::cout << " TASK_SUSPENDED ";
      break;
   case TASK_READY:
      std::cout << " TASK_READY ";
      break;
   case TASK_HOT:
      std::cout << " TASK_HOT ";
      break;
   case TASK_RECEIVING:
      std::cout << " TASK_RECEIVING ";
      break;
   case TASK_SLEEPING:
      std::cout << " TASK_SLEEPING ";
      break;
   case TASK_SYNC:
      std::cout << " TASK_SYNC ";
      break;
   case TASK_SYNC_SUSPEND:
      std::cout << " TASK_SYNC_SUSPEND ";
      break;
   case TASK_SYNC_FREE:
      std::cout << " TASK_SYNC_FREE ";
      break;
   case TASK_SPINNING:
      std::cout << " TASK_SPINNING ";
      break;
   case TASK_COMPUTING:
      std::cout << " TASK_COMPUTING ";
      break;
   case TASK_BLOCKED:
      std::cout << " TASK_BLOCKED ";
      break;
   default:
      std::cout << " UNKNOWN ";
      break;
   }

   // Skipping ones I haven't stubbed yet.
   std::cout << in->node << " " << in->host  << " " << in->uhost << " " << in->upriority
             << " " << in->priority << " " << in->parent << " " << in->son << "\n";

   return;
}

void TestStorage::print_all_stored() const{
   std::cout << "Printing " << stats.size() << " stats...\n";
   for (size_t i = 0; i < stats.size(); i++){
      if (stats[i] != nullptr){
         std::cout << i << ": ";
         print_stat(stats[i]);
      }
   }

   std::cout << std::endl << "Printing " << nodes.size() << " nodes...\n";

   for (size_t i = 0; i < nodes.size(); i++){
      if (nodes[i] != nullptr){
         std::cout << i << ": ";
         print_node(nodes[i]);
      }
   }


   std::cout << std::endl << "Printing " << tasks.size() << " tasks...\n";

      for (size_t i = 0; i < tasks.size(); i++){
      if (tasks[i] != nullptr){
         std::cout << i << ": ";
         print_task(tasks[i]);
      }
   }


   // Flush in case the next operation is really mean and doesn't let it write to console.
   std::cout << std::endl;
   return;
}








