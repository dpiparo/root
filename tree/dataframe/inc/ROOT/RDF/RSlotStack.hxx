// Author: Enrico Guiraud, Danilo Piparo CERN  03/2017

/*************************************************************************
 * Copyright (C) 1995-2018, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_RSLOTSTACK
#define ROOT_RSLOTSTACK

#include <atomic>
#include <memory>

namespace ROOT {
namespace Internal {
namespace RDF {

// inspired from Manning, C++ Concurrency in Action
template <typename T>
class RLockFreeStack {
private:
   struct RNode;
   struct counted_node_ptr {
      int external_count;
      RNode *ptr;
   };
   struct RNode {
      std::shared_ptr<T> data;
      std::atomic<int> internal_count;
      counted_node_ptr next;
      RNode(T const &data_) : data(std::make_shared<T>(data_)), internal_count(0) {}
   };
   std::atomic<counted_node_ptr> head;
   void increase_head_count(counted_node_ptr &old_counter)
   {
      counted_node_ptr new_counter;
      do {
         new_counter = old_counter;
         ++new_counter.external_count;
      } while (
         !head.compare_exchange_strong(old_counter, new_counter, std::memory_order_acquire, std::memory_order_relaxed));
      old_counter.external_count = new_counter.external_count;
   }

public:
   ~RLockFreeStack()
   {
      while (Pop())
         ;
   }
   void Push(T const &data)
   {
      counted_node_ptr new_node;
      new_node.ptr = new RNode(data);
      new_node.external_count = 1;
      new_node.ptr->next = head.load(std::memory_order_relaxed);
      while (!head.compare_exchange_weak(new_node.ptr->next, new_node, std::memory_order_release,
                                         std::memory_order_relaxed));
   }
   std::shared_ptr<T> Pop()
   {
      counted_node_ptr old_head = head.load(std::memory_order_relaxed);
      for (;;) {
         increase_head_count(old_head);
         auto *const ptr = old_head.ptr;
         if (!ptr) {
            return std::shared_ptr<T>();
         }
         if (head.compare_exchange_strong(old_head, ptr->next, std::memory_order_relaxed)) {
            std::shared_ptr<T> res;
            res.swap(ptr->data);
            int const count_increase = old_head.external_count - 2;
            if (ptr->internal_count.fetch_add(count_increase, std::memory_order_release) == -count_increase) {
               delete ptr;
            }
            return res;
         } else if (ptr->internal_count.fetch_add(-1, std::memory_order_relaxed) == 1) {
            ptr->internal_count.load(std::memory_order_acquire);
            delete ptr;
         }
      }
   }
};

class RSlotStack {
private:
   RLockFreeStack<unsigned int> fStack;

public:
   RSlotStack() = delete;
   RSlotStack(unsigned int size);
   void ReturnSlot(unsigned int slotNumber);
   unsigned int GetSlot();
};
} // namespace RDF
} // namespace Internal
} // namespace ROOT

#endif
