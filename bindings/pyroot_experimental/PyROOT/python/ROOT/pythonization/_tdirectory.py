# Author: Danilo Piparo, Stefan Wunsch CERN  08/2018

################################################################################
# Copyright (C) 1995-2018, Rene Brun and Fons Rademakers.                      #
# All rights reserved.                                                         #
#                                                                              #
# For the licensing terms see $ROOTSYS/LICENSE.                                #
# For the list of contributors see $ROOTSYS/README/CREDITS.                    #
################################################################################

from libROOTPython import AddDirectoryGetPyz, AddDirectoryWritePyz
from ROOT import pythonization
import cppyy

# This is an instant pythonization. As such, no argument is needed since we know
# what we are pythonizing.

@pythonization(lazy = False)
def pythonize_tdirectory():

    tdirectoryClass = cppyy.gbl.TDirectory
    AddDirectoryGetPyz(tdirectoryClass)
    AddDirectoryWritePyz(tdirectoryClass)

    return True
