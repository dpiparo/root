#include <ROOT/RDataFrame.hxx>
#include <ROOT/RVec.hxx>
#include <TBranchElement.h>
#include <TFile.h>
#include <TTree.h>
#include <TSystem.h> // Unlink
#include <gtest/gtest.h>

using namespace ROOT::VecOps;
using namespace ROOT;

TEST(RDFAndVecOps, ReadStdVectorAsRVec)
{
   const auto fname = "rdfandvecops.root";
   const auto treename = "t";
   const auto nEntries = 5u;
   // write out a small file with std::vector columns
   auto makeStdVecInt = []() { return std::vector<int>({1, 2, 3}); };
   auto makeStdVecBool = []() { return std::vector<bool>({true, false, false}); };
   RDataFrame(nEntries).Define("vi", makeStdVecInt)
                       .Define("vb", makeStdVecBool)
                       .Snapshot<std::vector<int>, std::vector<bool>>(treename, fname, {"vi", "vb"});

   // read it from a non-jitted action
   RDataFrame d(treename, fname);
   // To be FIXED: for the moment read vector<bool> as such
   auto checkRVec = [](const RVec<int> &vi, const std::vector<bool> &vb) {
      EXPECT_EQ(vi.size(), 3u);
      EXPECT_TRUE(All(vi == RVec<int>{1, 2, 3}));
      EXPECT_EQ(vb.size(), 3u);
      EXPECT_TRUE((vb == std::vector<bool>{true, false, false}));
   };
   d.Foreach(checkRVec, {"vi", "vb"});

   // read it from a jitted string as a RVec
   // filter string would be invalid if vi was read as a std::vector
   auto filterIntStr = "ROOT::VecOps::RVec<int> v2 = Map(vi, [](int i) { return i*i; }); return true;";
   auto filterBoolStr = "ROOT::VecOps::RVec<bool> v3 = Map(vb, [](bool i) { return (not i); }); return true;";
   auto c = d.Filter(filterIntStr).Filter(filterBoolStr).Count();
   EXPECT_EQ(*c, nEntries);

   gSystem->Unlink(fname);
}

TEST(RDFAndVecOps, DefineRVec)
{
   auto makeRVec = []() { return RVec<double>({1., 2., 3.}); };
   auto max = *RDataFrame(1).Define("v", makeRVec).Max<RVec<double>>("v");
   EXPECT_DOUBLE_EQ(max, 3.);
}

TEST(RDFAndVecOps, SnapshotRVec)
{
   // write RVec to file
   const auto fname = "tdfandvecops.root";
   const auto treename = "t";
   const auto nEntries = 5u;
   auto makeRVec = []() { return RVec<int>({1, 2, 3}); };
   RDataFrame(nEntries).Define("v", makeRVec).Snapshot<RVec<int>>(treename, fname, {"v"});

   // check the RVec was written as a RVec
   TFile f(fname);
   auto t = static_cast<TTree *>(f.Get(treename));
   auto b = static_cast<TBranchElement *>(t->GetBranch("v"));
   ASSERT_TRUE(b != nullptr);
   auto branchTypeName = b->GetClassName();
   EXPECT_STREQ(branchTypeName, "vector<int,ROOT::Detail::VecOps::RAdoptAllocator<int> >");

   gSystem->Unlink(fname);
}


