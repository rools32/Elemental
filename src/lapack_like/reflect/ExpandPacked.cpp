/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#include "El.hpp"

#include "./ApplyPacked/Util.hpp"
#include "./ExpandPacked/LV.hpp"

namespace El {

template<typename F> 
void ExpandPackedReflectors
( UpperOrLower uplo, VerticalOrHorizontal dir, Conjugation conjugation,
  Int offset, Matrix<F>& H, const Matrix<F>& t )
{
    DEBUG_ONLY(CallStackEntry cse("ExpandPackedReflectors"))
    if( uplo == LOWER && dir == VERTICAL )
        expand_packed_reflectors::LV( conjugation, offset, H, t );
    else
        LogicError("This option is not yet supported");
}

template<typename F> 
void ExpandPackedReflectors
( UpperOrLower uplo, VerticalOrHorizontal dir, Conjugation conjugation,
  Int offset, AbstractDistMatrix<F>& H, const AbstractDistMatrix<F>& t )
{
    DEBUG_ONLY(CallStackEntry cse("ExpandPackedReflectors"))
    if( uplo == LOWER && dir == VERTICAL )
        expand_packed_reflectors::LV( conjugation, offset, H, t );
    else
        LogicError("This option is not yet supported");
}

#define PROTO(F) \
  template void ExpandPackedReflectors \
  ( UpperOrLower uplo, VerticalOrHorizontal dir, Conjugation conjugation, \
    Int offset, Matrix<F>& H, const Matrix<F>& t ); \
  template void ExpandPackedReflectors \
  ( UpperOrLower uplo, VerticalOrHorizontal dir, Conjugation conjugation, \
    Int offset, AbstractDistMatrix<F>& H, const AbstractDistMatrix<F>& t );

#define EL_NO_INT_PROTO
#include "El/macros/Instantiate.h"

} // namespace El
