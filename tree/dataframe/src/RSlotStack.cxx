// Author: Enrico Guiraud, Danilo Piparo CERN  03/2017

/*************************************************************************
 * Copyright (C) 1995-2018, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include <ROOT/TSeq.hxx>
#include <ROOT/RDF/RSlotStack.hxx>
#include <TError.h> // R__ASSERT

ROOT::Internal::RDF::RSlotStack::RSlotStack(unsigned int size)
{
   for (auto i : ROOT::TSeqU(size)) fStack.Push(i);
}

void ROOT::Internal::RDF::RSlotStack::ReturnSlot(unsigned int slot)
{
   fStack.Push(slot);
}

unsigned int ROOT::Internal::RDF::RSlotStack::GetSlot()
{
   return *fStack.Pop();
}
