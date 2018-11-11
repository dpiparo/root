// Author: Enrico Guiraud, Danilo Piparo CERN  03/2017

/*************************************************************************
 * Copyright (C) 1995-2018, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include <ROOT/RDF/RSlotStack.hxx>

ROOT::Internal::RDF::RSlotStack::RSlotStack(unsigned int size)
  :fSlotStates(size, 1ULL)
{
   for (auto i = 0U; i < size; ++i) {
      fStack.Push(i);
   }
}

void ROOT::Internal::RDF::RSlotStack::ReturnSlot(unsigned int slot)
{
#ifndef NDEBUG
   // Check if the slot has been put back
   printf("Putting back slot %d (%d)\n", (int)slot, (int)fSlotStates[slot]);
   assert (0ULL == fSlotStates[slot] && "A slot was already put back on the stack.");
   fSlotStates[slot] = 1ULL;
#endif
   fStack.Push(slot);
}

unsigned int ROOT::Internal::RDF::RSlotStack::GetSlot()
{
   const auto slot = *fStack.Pop();
#ifndef NDEBUG
   // Check if the slot has been taken
   printf("Getting slot %d\n", (int)slot);
   assert (1ULL == fSlotStates[slot] && "A slot was already taken from the stack.");
   fSlotStates[slot] = 0ULL;
#endif
   return slot;
}
