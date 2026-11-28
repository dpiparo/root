#include "RZip.h"

#include "gtest/gtest.h"

#include <vector>

extern "C" void R__zipZSTD(int cxlevel, int *srcsize, const char *src, int *tgtsize, char *tgt, int *irep);

int Compress(int srcSize, char *src, int tgtSize, char *tgt)
{
   int res = -1;
   R__zipZSTD(1, &srcSize, src, &tgtSize, tgt, &res);
   return res;
}

TEST(ZipZSTD, sizeTooLarge)
{
   int maxSize = 0xffffff;
   int tooLarge = maxSize + 1;

   char *data = nullptr; // in any case, the check stops before writing here

   EXPECT_EQ(Compress(tooLarge, data, maxSize, data), 0);
}

TEST(ZipZSTD, sizeOk)
{
   const int okSize = 0xff1fff;

   std::vector<char> src(okSize, 'A');
   std::vector<char> tgt(okSize, 0);

   EXPECT_TRUE(Compress(okSize, src.data(), okSize, tgt.data()) != 0);
}

TEST(ZipZSTD, sizeNegative)
{
   int negSize = -1;
   int tgtSize = 16;

   char *data = nullptr; // in any case, the check stops before writing here

   EXPECT_EQ(Compress(negSize, data, tgtSize, data), 0);
}
