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

#include "RtypesCore.h"

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
   struct RCountedNode {
      RNode *fPtr;
      Long64_t fExternalCount; // to avoid padding
   };

   struct RNode {
      RNode() : fNext(new RCountedNode) {}
      ~RNode() { delete fNext; }
      std::shared_ptr<T> fData;
      std::atomic<int> fInternalCount;
      RCountedNode *fNext;
      RNode(T const &data_) : fData(std::make_shared<T>(data_)), fInternalCount(0) {}
   };

   std::atomic<RCountedNode*> fHead;

   void IncreaseHeadCount(RCountedNode *old_counter)
   {
      auto new_counter = new RCountedNode();
      do {
         new_counter->fPtr = old_counter->fPtr;
         new_counter->fExternalCount = old_counter->fExternalCount;
         ++new_counter->fExternalCount;
      } while (!fHead.compare_exchange_strong(old_counter, new_counter, std::memory_order_acquire,
                                              std::memory_order_relaxed));
      old_counter->fExternalCount = new_counter->fExternalCount;
   }

public:
   ~RLockFreeStack()
   {
      while (Pop()) {
      };
   }
   void Push(T const &data)
   {
      auto new_node = new RCountedNode();
      new_node->fPtr = new RNode(data);
      new_node->fExternalCount = 1;
      new_node->fPtr->fNext = fHead.load(std::memory_order_relaxed);
      while (!fHead.compare_exchange_weak(new_node->fPtr->fNext, new_node, std::memory_order_release,
                                          std::memory_order_relaxed)) {};
   }
   std::shared_ptr<T> Pop()
   {
      auto old_head = fHead.load(std::memory_order_relaxed);
      for (;;) {
         IncreaseHeadCount(old_head);
         auto *const ptr = old_head->fPtr;
         if (!ptr) {
            return std::shared_ptr<T>();
         }
         if (fHead.compare_exchange_strong(old_head, ptr->fNext, std::memory_order_relaxed)) {
            std::shared_ptr<T> res;
            res.swap(ptr->fData);
            const auto count_increase = old_head->fExternalCount - 2;
            if (ptr->fInternalCount.fetch_add(count_increase, std::memory_order_release) == -count_increase) {
               delete ptr;
            }
            return res;
         } else if (ptr->fInternalCount.fetch_add(-1, std::memory_order_relaxed) == 1) {
            ptr->fInternalCount.load(std::memory_order_acquire);
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

