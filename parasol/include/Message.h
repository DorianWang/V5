#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

// All Message and Message related functions, including the Message Pool.

#include <vector>
#include <queue>
#include <cstdint>
#include <string>

namespace bbs{

struct bs_message_t{
   // This should total around 80 bytes, excluding the dynamic string data.
   // I think it will still be best to use a pool for them,
   // since there's probably a lot of them and they have short lifespans
   uint_fast32_t sender;		/* sending task index		*/
   uint_fast32_t port;			/* port index		*/
   uint_fast32_t ack_port;		/* ack port index	*/
   uint_fast32_t org_port;		/* original port index	*/
   uint_fast32_t c_code;		/* comm media code	*/
   uint_fast32_t blid;			/* bus/link index	*/
   uint_fast32_t type;			/* message type code	*/
   uint_fast32_t length;		/* message size, used for bus_delay and link_delay	*/
   uint_fast32_t mid;			/* Unique message id, should this be 64 bit? Not sure exactly where this is set */
   uint_fast32_t did;			/* dye id, unimplemented		*/
   uint_fast32_t pri;			/* Message priority	*/
   uint_fast64_t ts;		      // Message time stamp, generally obtained from sc_time_stamp().value()
   std::string text;				/* message text, maybe use a shared pointer instead?	*/
};

// Might be too much overhead, but I'll fix it later if that's what we need.
class	bs_message_pool {
public:
   bs_message_pool(){
      storage.resize(baseSize);
      for (unsigned int i = 0; i < baseSize; i++){
         unusedMessageIndex.push(i);
      }
   };
   static constexpr int baseSize = 400;
   static constexpr int growSize = 200;
   bs_message_t* get_mp(size_t index){
      if (index < storage.size())
         return storage.data();
      return nullptr;
   }; // This pointer may be invalidated if the vector resizes.
   uint32_t get_mess(){
      uint32_t result;
      if (unusedMessageIndex.empty()){
         size_t oldSize = storage.size();
         storage.resize(oldSize + growSize);
         for (unsigned int i = oldSize; i < oldSize + growSize; i++){
            unusedMessageIndex.push(i);
         }
      }
      result = unusedMessageIndex.front();
      unusedMessageIndex.pop();
      return result;
   }
   void free_mess(size_t index){
      if (index < storage.size()){
         unusedMessageIndex.push(index);
         storage[index].text.clear(); // This probably won't free the internal buffer, but it can.
      }
      else {
         printf("Trying to free message with invalid index %llu!\n", index);
         // Maybe throw an error I guess.
      }
   }
private:
	std::vector <bs_message_t> storage;
	std::queue <uint_fast32_t> unusedMessageIndex; // Only 4 billion messages, possible to need more?
};


extern bs_message_pool bs_mess_pool;

}

#define get_mess_ptr(ID) bs_mess_pool.get_mp(ID)


#endif // MESSAGE_H_INCLUDED


