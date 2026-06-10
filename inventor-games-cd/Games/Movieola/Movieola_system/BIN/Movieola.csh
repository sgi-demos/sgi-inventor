#! /bin/csh -f
#

##########################################################################
# 
# Copyright (c) 1993 by Radiance Software
#
# All rights reserved. This listing of source code is proprietary and
# confidential to Radiance Software. It may not be copied or made 
# available in any form (electronic or hard copy) to any person not
# authorized by Radiance Software.
# Unpublished - Rights reserved under all copyright laws
#
# Radiance Software, 1726 Francisco Street, Berkeley, CA 94703
#
##########################################################################
###########################################################################
#
# Script to run Movieola
#
###########################################################################


# Assume MOVIEOLAHOME is set
if (! $?MOVIEOLAHOME) then
   echo "Movieola not installed properly (MOVIEOLAHOME not set)"
   exit 1
endif

# Banner
($MOVIEOLAHOME/bin/rspaste -o 285 146 15 \
 $MOVIEOLAHOME/resources/Movieola.rgb &) >/dev/null

# Run-time environment
source $MOVIEOLAHOME/bin/Movieola.rc

# The real thing
$MOVIEOLAHOME/bin/movieola $*

# Clean up
#rm -rf $HOME/Movieola_projects/UNTITLED.*
