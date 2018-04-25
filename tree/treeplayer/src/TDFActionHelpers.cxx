// Author: Enrico Guiraud, Danilo Piparo CERN  12/2016

/*************************************************************************
 * Copyright (C) 1995-2016, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include "ROOT/TDFActionHelpers.hxx"

namespace ROOT {
namespace Internal {
namespace TDF {

CountHelper::CountHelper(const std::shared_ptr<ULong64_t> &resultCount, const unsigned int nSlots)
   : fResultCount(resultCount), fCounts(nSlots, 0)
{
}

void CountHelper::Exec(unsigned int slot)
{
   fCounts[slot]++;
}

void CountHelper::Finalize()
{
   *fResultCount = 0;
   for (auto &c : fCounts) {
      *fResultCount += c;
   }
}

ULong64_t &CountHelper::PartialUpdate(unsigned int slot)
{
   return fCounts[slot];
}

// TODO
// template void MinHelper::Exec(unsigned int, const std::vector<float> &);
// template void MinHelper::Exec(unsigned int, const std::vector<double> &);
// template void MinHelper::Exec(unsigned int, const std::vector<char> &);
// template void MinHelper::Exec(unsigned int, const std::vector<int> &);
// template void MinHelper::Exec(unsigned int, const std::vector<unsigned int> &);

// template void MaxHelper::Exec(unsigned int, const std::vector<float> &);
// template void MaxHelper::Exec(unsigned int, const std::vector<double> &);
// template void MaxHelper::Exec(unsigned int, const std::vector<char> &);
// template void MaxHelper::Exec(unsigned int, const std::vector<int> &);
// template void MaxHelper::Exec(unsigned int, const std::vector<unsigned int> &);

MeanHelper::MeanHelper(const std::shared_ptr<double> &meanVPtr, const unsigned int nSlots)
   : fResultMean(meanVPtr), fCounts(nSlots, 0), fSums(nSlots, 0), fPartialMeans(nSlots)
{
}

void MeanHelper::Exec(unsigned int slot, double v)
{
   fSums[slot] += v;
   fCounts[slot]++;
}

void MeanHelper::Finalize()
{
   double sumOfSums = 0;
   for (auto &s : fSums)
      sumOfSums += s;
   ULong64_t sumOfCounts = 0;
   for (auto &c : fCounts)
      sumOfCounts += c;
   *fResultMean = sumOfSums / (sumOfCounts > 0 ? sumOfCounts : 1);
}

double &MeanHelper::PartialUpdate(unsigned int slot)
{
   fPartialMeans[slot] = fSums[slot] / fCounts[slot];
   return fPartialMeans[slot];
}

template void MeanHelper::Exec(unsigned int, const std::vector<float> &);
template void MeanHelper::Exec(unsigned int, const std::vector<double> &);
template void MeanHelper::Exec(unsigned int, const std::vector<char> &);
template void MeanHelper::Exec(unsigned int, const std::vector<int> &);
template void MeanHelper::Exec(unsigned int, const std::vector<unsigned int> &);

} // end NS TDF
} // end NS Internal
} // end NS ROOT
