/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef EL_LU_LOCAL_HPP
#define EL_LU_LOCAL_HPP

namespace El {
namespace lu {

// Local LU _without_ partial pivoting

template<typename F> 
inline void
UnbObj( Matrix<F>& A )
{
    DEBUG_ONLY(CallStackEntry cse("lu::UnbObj"))
    const Int m = A.Height();
    const Int n = A.Width();
    const Int minDim = Min(m,n);
    for( Int k=0; k<minDim; ++k )
    {
        const Range<Int> ind1( k, k+1 ),
                         ind2Vert( k+1, m ),
                         ind2Horz( k+1, n );

        auto alpha11 = A( ind1,     ind1     );
        auto a12     = A( ind1,     ind2Horz );
        auto a21     = A( ind2Vert, ind1     );
        auto A22     = A( ind2Vert, ind2Horz );

        F alpha = alpha11.Get(0,0);
        if( alpha == F(0) )
            throw SingularMatrixException();
        Scale( 1/alpha, a21 );
        Geru( F(-1), a21, a12, A22 );
    }
}

template<typename F>
inline void
Unb( Matrix<F>& A )
{
    DEBUG_ONLY(CallStackEntry cse("lu::Unb"))
    const Int m = A.Height();
    const Int n = A.Width();
    for( Int j=0; j<Min(m,n); ++j )
    {
        const F alpha = A.Get(j,j);
        if( alpha == F(0) )
            throw SingularMatrixException();

        blas::Scal( m-(j+1), F(1)/alpha, A.Buffer(j+1,j), 1 );
        blas::Geru
        ( m-(j+1), n-(j+1),
          F(-1), A.LockedBuffer(j+1,j), 1, A.LockedBuffer(j,j+1), A.LDim(),
                 A.Buffer(j+1,j+1), A.LDim() );
    }
}

} // namespace lu
} // namespace El

#endif // ifndef EL_LU_LOCAL_HPP
