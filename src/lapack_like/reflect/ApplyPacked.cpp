/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#include "El.hpp"

#include "./ApplyPacked/Util.hpp"
#include "./ApplyPacked/LLHB.hpp"
#include "./ApplyPacked/LLHF.hpp"
#include "./ApplyPacked/LLVB.hpp"
#include "./ApplyPacked/LLVF.hpp"
#include "./ApplyPacked/LUHB.hpp"
#include "./ApplyPacked/LUHF.hpp"
#include "./ApplyPacked/LUVB.hpp"
#include "./ApplyPacked/LUVF.hpp"
#include "./ApplyPacked/RLHB.hpp"
#include "./ApplyPacked/RLHF.hpp"
#include "./ApplyPacked/RLVB.hpp"
#include "./ApplyPacked/RLVF.hpp"
#include "./ApplyPacked/RUHB.hpp"
#include "./ApplyPacked/RUHF.hpp"
#include "./ApplyPacked/RUVB.hpp"
#include "./ApplyPacked/RUVF.hpp"

namespace El {

template<typename F> 
void ApplyPackedReflectors
( LeftOrRight side, UpperOrLower uplo, 
  VerticalOrHorizontal dir, ForwardOrBackward order, 
  Conjugation conjugation,
  Int offset, const Matrix<F>& H, const Matrix<F>& t, Matrix<F>& A )
{
    DEBUG_ONLY(CallStackEntry cse("ApplyPackedReflectors"))
    if( side == LEFT )
    {
        if( uplo == LOWER )
        {
            if( dir == VERTICAL && order == FORWARD )
                apply_packed_reflectors::LLVF( conjugation, offset, H, t, A );
            else if( dir == VERTICAL )
                apply_packed_reflectors::LLVB( conjugation, offset, H, t, A );
            else if( order == FORWARD )
                apply_packed_reflectors::LLHF( conjugation, offset, H, t, A );
            else
                apply_packed_reflectors::LLHB( conjugation, offset, H, t, A );
        }
        else
        {
            if( dir == VERTICAL && order == FORWARD )
                apply_packed_reflectors::LUVF( conjugation, offset, H, t, A );
            else if( dir == VERTICAL )
                apply_packed_reflectors::LUVB( conjugation, offset, H, t, A );
            else if( order == FORWARD )
                apply_packed_reflectors::LUHF( conjugation, offset, H, t, A );
            else
                apply_packed_reflectors::LUHB( conjugation, offset, H, t, A );
        }
    }
    else
    {
        if( uplo == LOWER )
        {
            if( dir == VERTICAL && order == FORWARD )
                apply_packed_reflectors::RLVF( conjugation, offset, H, t, A );
            else if( dir == VERTICAL )
                apply_packed_reflectors::RLVB( conjugation, offset, H, t, A );
            else if( order == FORWARD )
                apply_packed_reflectors::RLHF( conjugation, offset, H, t, A );
            else
                apply_packed_reflectors::RLHB( conjugation, offset, H, t, A );
        }
        else
        {
            if( dir == VERTICAL && order == FORWARD )
                apply_packed_reflectors::RUVF( conjugation, offset, H, t, A );
            else if( dir == VERTICAL )
                apply_packed_reflectors::RUVB( conjugation, offset, H, t, A );
            else if( order == FORWARD )
                apply_packed_reflectors::RUHF( conjugation, offset, H, t, A );
            else
                apply_packed_reflectors::RUHB( conjugation, offset, H, t, A );
        }
    }
}

template<typename F> 
void ApplyPackedReflectors
( LeftOrRight side, UpperOrLower uplo, 
  VerticalOrHorizontal dir, ForwardOrBackward order, 
  Conjugation conjugation,
  Int offset,
  const AbstractDistMatrix<F>& H, const AbstractDistMatrix<F>& t, 
        AbstractDistMatrix<F>& A )
{
    DEBUG_ONLY(CallStackEntry cse("ApplyPackedReflectors"))
    if( side == LEFT )
    {
        if( uplo == LOWER )
        {
            if( dir == VERTICAL && order == FORWARD )
                apply_packed_reflectors::LLVF( conjugation, offset, H, t, A );
            else if( dir == VERTICAL )
                apply_packed_reflectors::LLVB( conjugation, offset, H, t, A );
            else if( order == FORWARD )
                apply_packed_reflectors::LLHF( conjugation, offset, H, t, A );
            else
                apply_packed_reflectors::LLHB( conjugation, offset, H, t, A );
        }
        else
        {
            if( dir == VERTICAL && order == FORWARD )
                apply_packed_reflectors::LUVF( conjugation, offset, H, t, A );
            else if( dir == VERTICAL )
                apply_packed_reflectors::LUVB( conjugation, offset, H, t, A );
            else if( order == FORWARD )
                apply_packed_reflectors::LUHF( conjugation, offset, H, t, A );
            else
                apply_packed_reflectors::LUHB( conjugation, offset, H, t, A );
        }
    }
    else
    {
        if( uplo == LOWER )
        {
            if( dir == VERTICAL && order == FORWARD )
                apply_packed_reflectors::RLVF( conjugation, offset, H, t, A );
            else if( dir == VERTICAL )
                apply_packed_reflectors::RLVB( conjugation, offset, H, t, A );
            else if( order == FORWARD )
                apply_packed_reflectors::RLHF( conjugation, offset, H, t, A );
            else
                apply_packed_reflectors::RLHB( conjugation, offset, H, t, A );
        }
        else
        {
            if( dir == VERTICAL && order == FORWARD )
                apply_packed_reflectors::RUVF( conjugation, offset, H, t, A );
            else if( dir == VERTICAL )
                apply_packed_reflectors::RUVB( conjugation, offset, H, t, A );
            else if( order == FORWARD )
                apply_packed_reflectors::RUHF( conjugation, offset, H, t, A );
            else
                apply_packed_reflectors::RUHB( conjugation, offset, H, t, A );
        }
    }
}

#define PROTO(F) \
  template void ApplyPackedReflectors \
  ( LeftOrRight side, UpperOrLower uplo, \
    VerticalOrHorizontal dir, ForwardOrBackward order, \
    Conjugation conjugation, Int offset, \
    const Matrix<F>& H, const Matrix<F>& t, \
          Matrix<F>& A ); \
  template void ApplyPackedReflectors \
  ( LeftOrRight side, UpperOrLower uplo, \
    VerticalOrHorizontal dir, ForwardOrBackward order, \
    Conjugation conjugation, Int offset, \
    const AbstractDistMatrix<F>& H, const AbstractDistMatrix<F>& t, \
          AbstractDistMatrix<F>& A );

#define EL_NO_INT_PROTO
#include "El/macros/Instantiate.h"

} // namespace El
