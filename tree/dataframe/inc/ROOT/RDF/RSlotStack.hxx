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

#include "Rtypes.h"

namespace ROOT {
namespace Internal {
namespace RDF {

// This code is inspired by an excellent book about parallel programming:
// Anthony Williams,
// "C++ Concurrency in action", ISBN 9781933988771 (Manning)

template <typename T>
class RConcurrentStack {
private:
   struct RStackNode;

   struct RCountedStackNodePtr {
      unsigned int fExternalCount{0U};
      RStackNode *fPtr{nullptr};
      RCountedStackNodePtr(unsigned int externalCount, RStackNode *ptr) : fExternalCount(externalCount), fPtr(ptr) {}
      RCountedStackNodePtr() = default;
   };

   struct RStackNode {
      std::shared_ptr<T> fData{nullptr};
      std::atomic<unsigned int> fInternalCount{0U};
      RCountedStackNodePtr fNext{0U, nullptr};
      RStackNode(T const &data) : fData(std::make_shared<T>(data)) {}
      RStackNode(T &&data) : fData(std::make_shared<T>(data)) {}
   };

   std::atomic<RCountedStackNodePtr> fHead;

   void IncreaseHeadCount(RCountedStackNodePtr &old_counter)
   {
      RCountedStackNodePtr new_counter;
      do {
         new_counter = old_counter;
         ++new_counter.fExternalCount;
      } while (!fHead.compare_exchange_strong(old_counter, new_counter, std::memory_order_acquire,
                                              std::memory_order_relaxed));
      old_counter.fExternalCount = new_counter.fExternalCount;
   }

   std::shared_ptr<T> PopImpl(bool canThrow)
   {
      auto old_head = fHead.load(std::memory_order_relaxed);
      for (;;) {
         IncreaseHeadCount(old_head);
         const auto ptr = old_head.fPtr;
         if (!ptr) {
            // Popping from an empty std::stack based on an std::deque is undefined.
            // Here we are a bit more explicit.
            if (canThrow) throw std::runtime_error("Trying to pop from an empty RConcurrentStack.");
            return std::shared_ptr<T>(nullptr);
         }
         if (fHead.compare_exchange_strong(old_head, ptr->fNext, std::memory_order_relaxed)) {
            std::shared_ptr<T> res;
            res.swap(ptr->fData);
            const auto count_increase = old_head.fExternalCount - 2;
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

public:
   ~RConcurrentStack()
   {
      while (PopImpl(false /*canThrow*/));
   }

   void Push(T const &data) {
      RCountedStackNodePtr new_node(0UL, new RStackNode(data));
      new_node.fExternalCount = 1;
      new_node.fPtr->fNext = fHead.load(std::memory_order_relaxed);
      while (!fHead.compare_exchange_weak(new_node.fPtr->fNext, new_node, std::memory_order_release,
                                          std::memory_order_relaxed));
   }

   std::shared_ptr<T> Pop() { return PopImpl(true /*canThrow*/); }
};

class RSlotStack {
private:
   RConcurrentStack<unsigned int> fStack;
   std::vector<ULong64_t> fSlotStates; // no issues with MT

public:
   RSlotStack() = delete;
   RSlotStack(unsigned int size);
   void ReturnSlot(unsigned int slotNumber);
   unsigned int GetSlot();
};
} // ns RDF
} // ns Internal
} // ns ROOT

#endif
