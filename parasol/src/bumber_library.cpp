
#include "bumber_consts.h"
#include "bumber_shared.h"
#include "Message.h"

#include "Bus.h"
#include "Link.h"
#include "TaskSC.h"
#include "NodeSC.h"

#include <iostream>
#include <string>
#include <queue>

namespace bbs{
   bs_message_pool bs_mess_pool; // Temporary initialization here.
   bm_table_t <Bus> bm_bus_tab("Bus");
   //bm_table_t <Link> bm_link_tab("Link");
   //bm_table_t <NodeSC> bm_node_tab("Node");
   //bm_table_t <TaskSC> bm_task_tab("Task");

   // So basically this function should be what the ps_run_parasol function was.
   int bs_run_bumbershoot()
   {
      return 0;
   }

}

class TopTest : public sc_module{
public:

   sc_event testEvent;
   sc_vector <bbs::Bus> testVec;
   sc_vector <bbs::NodeSC> testVec2;
   sc_vector <bbs::CpuSC> testVec3;
   void hello(){
      std::cout << "Hello World!" << std::endl;
   };
   TopTest(sc_module_name name, size_t index) : sc_module(name), testVec(name){
      SC_HAS_PROCESS(TopTest);
      testVec.init(12, [=](const char* nm, size_t i){return new bbs::Bus(nm, i);});
      testVec2.init(4, [=](const char* nm, size_t i){return new bbs::NodeSC(nm, i);});
      testVec3.init(4, [=](const char* nm, size_t i){return new bbs::CpuSC(nm, nullptr, i);});
      testEvent.notify(12, DEFAULT_TIME_TICK);
      testVec3[0].wakeUp.notify(4, DEFAULT_TIME_TICK);
      SC_METHOD(hello);
      sensitive << testEvent;
      dont_initialize();
   }
};





/** Code that should be moved to unit tests if we have time.
// This is a quick test that the tables are working as expected.
// It should output Test_Links0 ... Test_Links4
bm_bus_tab.add_entry("Test_Links", 4);
for (size_t i = 0; i < bm_bus_tab.size(); i++){
   std::cout << bm_bus_tab[i].basename() << std::endl;
}


**/
#ifdef BUMBERSHOOT_TESTING

int sc_main(int argc, char** argv)
{
   using namespace bbs;
   std::cout << "Hello World!" << std::endl;
   // bm_bus_tab.add_entry("Test_Buses", 1);
   std::cout << "Hello Sky!" << std::endl;

   TopTest testing("TopTester", 4);

   //for (size_t i = 0; i < bm_bus_tab.size(); i++){
   //   printf("%s\n", bm_bus_tab[i].basename());
   //}
   std::cout << "Hello Sea!" << std::endl;
   std::cout << "Hello Sea!" << std::endl;
   std::cout << "Hello Sea!" << std::endl;


   //sc_vector<Bus> test;
   //test.init(12, [=](const char* nm, size_t i){return new Bus(nm, i);});

   //sc_vector<Bus> test2;
   //test2.init(12, [=](const char* nm, size_t i){return new Bus(nm, i);});

   //std::cout << test.size() << test2.size() << std::endl;

   //for (int i = 0; i < 12; i++){
   //   std::cout << test[i].name() << " " << test2[i].name() << std::endl;
   //}
   std::cout << "End of main!" << std::endl;
   sc_start();

   return 0;
}













#endif











