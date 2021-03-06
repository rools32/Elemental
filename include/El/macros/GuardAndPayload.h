/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
/* NOTE: This is a commonly-used hack that is only included in this file to
         help avoid redundancy. */
#define IF_GUARD_AND_PAYLOAD(CDIST,RDIST) \
  if( GUARD(CDIST,RDIST) ) { PAYLOAD(CDIST,RDIST) }

#define ELSEIF_GUARD_AND_PAYLOAD(CDIST,RDIST) \
  else IF_GUARD_AND_PAYLOAD(CDIST,RDIST)

IF_GUARD_AND_PAYLOAD(    CIRC,CIRC)
ELSEIF_GUARD_AND_PAYLOAD(MC,  MR  )
ELSEIF_GUARD_AND_PAYLOAD(MC,  STAR)
ELSEIF_GUARD_AND_PAYLOAD(MD,  STAR)
ELSEIF_GUARD_AND_PAYLOAD(MR,  MC  )
ELSEIF_GUARD_AND_PAYLOAD(MR,  STAR)
ELSEIF_GUARD_AND_PAYLOAD(STAR,MC  )
ELSEIF_GUARD_AND_PAYLOAD(STAR,MD  )
ELSEIF_GUARD_AND_PAYLOAD(STAR,MR  )
ELSEIF_GUARD_AND_PAYLOAD(STAR,STAR)
ELSEIF_GUARD_AND_PAYLOAD(STAR,VC  )
ELSEIF_GUARD_AND_PAYLOAD(STAR,VR  )
ELSEIF_GUARD_AND_PAYLOAD(VC,  STAR)
ELSEIF_GUARD_AND_PAYLOAD(VR,  STAR)

#undef ELSEIF_GUARD_AND_PAYLOAD
#undef IF_GUARD_AND_PAYLOAD
#undef PAYLOAD
#undef GUARD
