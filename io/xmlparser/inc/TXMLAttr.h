// @(#)root/xmlparser:$Id$
// Author: Jose Lo   12/4/2005

/*************************************************************************
 * Copyright (C) 1995-2017, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TXMLAttr
#define ROOT_TXMLAttr

#include "TObject.h"


class TXMLAttr : public TObject {

private:
   TXMLAttr(const TXMLAttr&); // Not implemented
   TXMLAttr& operator=(const TXMLAttr&); // Not implemented

   const char *fKey;        ///< XML attribute key
   const char *fValue;      ///< XML attribute value

public:
   TXMLAttr(const char *key, const char *value) : fKey(key), fValue(value) { }
   virtual ~TXMLAttr() { }

   const char *GetName() const { return fKey; }
   const char *Key() const { return fKey; }
   const char *GetValue() const { return fValue; }

   ClassDef(TXMLAttr,0)  //XML attribute pair
};

#endif
