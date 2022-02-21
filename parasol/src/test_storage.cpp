
#include "test_storage.h"

#include <algorithm> // std::sort


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
      throw std::out_of_range("get_stat(index) is out of range");
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
   // NULL_COMM_PTR == ((ps_comm_t*) 0)
   nNode->sl_list = nullptr; nNode->rl_list = nullptr; nNode->bus_list = nullptr;
   nNode->ngroup = 0;


   nNode->build_time = ps_now; // Needs to get SystemC time somehow, but later.
   nNode->cpu = std::vector<ps_cpu_t>(ncpu); // vector with ncpu elements.
   if (sf & 0x02){ // SF_PER_NODE
      nNode->stat = add_stat(nNode->name + " Utilization", 29382731, ps_now); //VARIABLE
      for (size_t i = 0; i < nNode->cpu.size(); i++){
         nNode->cpu[i].stat = add_stat(nNode->name + " (cpu " + std::to_string(i) +") Utilization", 29382731, ps_now);
      }
   }
   else {
      NOT_IMPLEMENTED_WAR;
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
      throw std::out_of_range("get_node(index) is out of range");
   }
}




void TestStorage::print_nodes()
{
   for (size_t i = 0; i < nodes.size(); i++){
      if (nodes[i] != nullptr){
         std::cout << i << " " << nodes[i]->name << " " << nodes[i]->ncpu << "\n";
      }
   }
   std::cout << "Total size of nodes vector: " << nodes.size() << std::endl;
}

int TestStorage::add_task(const char* name, long node, long host, void (*code)(void*), long priority, double ps_now, long group, double stackscale){
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
      throw std::out_of_range("get_task(index) is out of range");
   }
}






