/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef EL_HERMITIANTRIDIAGEIG_SORT_HPP
#define EL_HERMITIANTRIDIAGEIG_SORT_HPP

namespace El {

// This routine is slightly more general than necessary (complex support) so
// that it may also be used for sorting Hermitian eigenpairs
namespace herm_eig {

template<typename F>
void Sort( Matrix<Base<F>>& w, Matrix<F>& Z, SortType sort )
{
    DEBUG_ONLY(CallStackEntry cse("herm_eig::Sort"))
    if( sort == UNSORTED )
        return;

    // Initialize the pairs of indices and eigenvalues
    typedef Base<F> Real;
    std::vector<ValueInt<Real>> pairs = TaggedSort( w, sort );

    // Reorder the eigenvectors and eigenvalues using the new ordering
    const Int n = Z.Height();
    const Int k = Z.Width();
    Matrix<F> ZPerm( n, k );
    for( Int j=0; j<k; ++j )
    {
        const Int source = pairs[j].index;
        MemCopy( ZPerm.Buffer(0,j), Z.LockedBuffer(0,source), n );
        w.Set(j,0,pairs[j].value);
    }
    Z = ZPerm;
}

template<typename Real,typename F>
void Sort
( AbstractDistMatrix<Real>& w, AbstractDistMatrix<F>& Z, SortType sort )
{
    DEBUG_ONLY(CallStackEntry cse("herm_eig::Sort"))
    if( sort == UNSORTED )
        return;

    // Get the sorted eigenvalue information
    std::vector<ValueInt<Real>> pairs = TaggedSort( w, sort );

    // Locally reorder the eigenvectors and eigenvalues using the new ordering
    const Int n = Z.Height();
    const Int k = Z.Width();
    const Grid& g = Z.Grid();
    DistMatrix<F,VC,STAR> Z_VC_STAR( Z );
    DistMatrix<F,VC,STAR> ZPerm_VC_STAR( n, k, g );
    const Int nLocal = Z_VC_STAR.LocalHeight();
    for( Int j=0; j<k; ++j )
    {
        MemCopy
        ( ZPerm_VC_STAR.Buffer(0,j), 
          Z_VC_STAR.LockedBuffer(0,pairs[j].index), nLocal );
        w.Set( j, 0, pairs[j].value );
    }
    Z_VC_STAR.Empty();
    Copy( ZPerm_VC_STAR, Z );
}

} // namespace herm_eig

} // namespace El

#endif // ifndef EL_HERMITIANTRIDIAGEIG_SORT_HPP
