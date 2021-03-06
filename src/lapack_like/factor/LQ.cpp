/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#include "El.hpp"

#include "./LQ/ApplyQ.hpp"
#include "./LQ/Householder.hpp"

#include "./LQ/SolveAfter.hpp"

#include "./LQ/Explicit.hpp"

namespace El {

template<typename F> 
void LQ( Matrix<F>& A, Matrix<F>& t, Matrix<Base<F>>& d )
{
    DEBUG_ONLY(CallStackEntry cse("LQ"))
    lq::Householder( A, t, d );
}

template<typename F> 
void LQ
( AbstractDistMatrix<F>& A, AbstractDistMatrix<F>& t, 
  AbstractDistMatrix<Base<F>>& d )
{
    DEBUG_ONLY(CallStackEntry cse("LQ"))
    lq::Householder( A, t, d );
}

// Variants which perform (Businger-Golub) row-pivoting
// ====================================================
// TODO

#define PROTO(F) \
  template void LQ( Matrix<F>& A, Matrix<F>& t, Matrix<Base<F>>& d ); \
  template void LQ \
  ( AbstractDistMatrix<F>& A, \
    AbstractDistMatrix<F>& t, AbstractDistMatrix<Base<F>>& d ); \
  template void lq::ApplyQ \
  ( LeftOrRight side, Orientation orientation, \
    const Matrix<F>& A, const Matrix<F>& t, \
    const Matrix<Base<F>>& d, Matrix<F>& B ); \
  template void lq::ApplyQ \
  ( LeftOrRight side, Orientation orientation, \
    const AbstractDistMatrix<F>& A, const AbstractDistMatrix<F>& t, \
    const AbstractDistMatrix<Base<F>>& d, AbstractDistMatrix<F>& B ); \
  template void lq::SolveAfter \
  ( Orientation orientation, \
    const Matrix<F>& A, const Matrix<F>& t, \
    const Matrix<Base<F>>& d, const Matrix<F>& B, \
          Matrix<F>& X ); \
  template void lq::SolveAfter \
  ( Orientation orientation, \
    const AbstractDistMatrix<F>& A, const AbstractDistMatrix<F>& t, \
    const AbstractDistMatrix<Base<F>>& d, const AbstractDistMatrix<F>& B, \
          AbstractDistMatrix<F>& X ); \
  template void lq::Explicit( Matrix<F>& L, Matrix<F>& A ); \
  template void lq::Explicit \
  ( AbstractDistMatrix<F>& L, AbstractDistMatrix<F>& A ); \
  template void lq::ExplicitTriang( Matrix<F>& A ); \
  template void lq::ExplicitTriang( AbstractDistMatrix<F>& A ); \
  template void lq::ExplicitUnitary( Matrix<F>& A ); \
  template void lq::ExplicitUnitary( AbstractDistMatrix<F>& A );

#define EL_NO_INT_PROTO
#include "El/macros/Instantiate.h"

} // namespace El
