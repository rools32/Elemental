/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#include "El.hpp"

#define ColDist MC
#define RowDist STAR

#include "./setup.hpp"

namespace El {

// Public section
// ##############

// Assignment and reconfiguration
// ==============================

// Return a view
// -------------

template<typename T>
DM DM::operator()( Range<Int> indVert, Range<Int> indHorz )
{
    DEBUG_ONLY(CallStackEntry cse("DM[MC,STAR]( ind, ind )"))
    if( this->Locked() )
        return LockedView( *this, indVert, indHorz );
    else
        return View( *this, indVert, indHorz );
}

template<typename T>
const DM DM::operator()( Range<Int> indVert, Range<Int> indHorz ) const
{
    DEBUG_ONLY(CallStackEntry cse("DM[MC,STAR]( ind, ind )"))
    return LockedView( *this, indVert, indHorz );
}

// Make a copy
// -----------
template<typename T>
DM& DM::operator=( const DM& A )
{
    DEBUG_ONLY(CallStackEntry cse("DM[MC,STAR] = DM[MC,STAR]"))
    A.Translate( *this );
    return *this;
}

template<typename T>
DM& DM::operator=( const DistMatrix<T,MC,MR>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[MC,STAR] = [MC,MR]"))
    A.RowAllGather( *this );
    return *this;
}

template<typename T>
DM& DM::operator=( const DistMatrix<T,STAR,MR>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[MC,STAR] = [STAR,MR]"))
    DistMatrix<T,MC,MR> A_MC_MR(this->Grid());
    A_MC_MR.AlignColsWith(*this);
    A_MC_MR = A;
    *this = A_MC_MR;
    return *this;
}

template<typename T>
DM& DM::operator=( const DistMatrix<T,MD,STAR>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[MC,STAR] = [MD,STAR]"))
    // TODO: More efficient implementation?
    DistMatrix<T,STAR,STAR> A_STAR_STAR( A );
    *this = A_STAR_STAR;
    return *this;
}

template<typename T>
DM& DM::operator=( const DistMatrix<T,STAR,MD>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[MC,STAR] = [STAR,MD]"))
    // TODO: More efficient implementation?
    DistMatrix<T,STAR,STAR> A_STAR_STAR( A );
    *this = A_STAR_STAR;
    return *this;
}

template<typename T>
DM& DM::operator=( const DistMatrix<T,MR,MC>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[MC,STAR] = [MR,MC]"))
    auto A_VR_STAR = MakeUnique<DistMatrix<T,VR,STAR>>( A );
    auto A_VC_STAR = MakeUnique<DistMatrix<T,VC,STAR>>( this->Grid() );
    A_VC_STAR->AlignColsWith(*this);
    *A_VC_STAR = *A_VR_STAR;
    A_VR_STAR.reset(); 
    *this = *A_VC_STAR;
    return *this;
}

template<typename T>
DM& DM::operator=( const DistMatrix<T,MR,STAR>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[MC,STAR] = [MR,STAR]"))
    auto A_VR_STAR = MakeUnique<DistMatrix<T,VR,STAR>>( A );
    auto A_VC_STAR = MakeUnique<DistMatrix<T,VC,STAR>>( this->Grid() );
    A_VC_STAR->AlignColsWith(*this);
    *A_VC_STAR = *A_VR_STAR;
    A_VR_STAR.reset(); 
    *this = *A_VC_STAR;
    return *this;
}

template<typename T>
DM& DM::operator=( const DistMatrix<T,STAR,MC>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[MC,STAR] = [STAR,MC]"))
    auto A_MR_MC = MakeUnique<DistMatrix<T,MR,MC>>( A );
    auto A_VR_STAR = MakeUnique<DistMatrix<T,VR,STAR>>( *A_MR_MC );
    A_MR_MC.reset();

    auto A_VC_STAR = MakeUnique<DistMatrix<T,VC,STAR>>( this->Grid() );
    A_VC_STAR->AlignColsWith(*this);
    *A_VC_STAR = *A_VR_STAR;
    A_VR_STAR.reset(); 

    *this = *A_VC_STAR;
    return *this;
}

template<typename T>
DM& DM::operator=( const DistMatrix<T,VC,STAR>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[MC,STAR] = [VC,STAR]"))
    A.PartialColAllGather( *this );
    return *this;
}

template<typename T>
DM& DM::operator=( const DistMatrix<T,STAR,VC>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[MC,STAR] = [STAR,VC]"))
    auto A_STAR_VR = MakeUnique<DistMatrix<T,STAR,VR>>( A );
    auto A_MC_MR = MakeUnique<DistMatrix<T,MC,MR>>( this->Grid() );
    A_MC_MR->AlignColsWith(*this);
    *A_MC_MR = *A_STAR_VR;
    A_STAR_VR.reset();
    *this = *A_MC_MR;
    return *this;
}

template<typename T>
DM& DM::operator=( const DistMatrix<T,VR,STAR>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[MC,STAR] = [VR,STAR]"))
    DistMatrix<T,VC,STAR> A_VC_STAR(this->Grid());
    A_VC_STAR.AlignColsWith(*this);
    A_VC_STAR = A;
    *this = A_VC_STAR;
    return *this;
}

template<typename T>
DM& DM::operator=( const DistMatrix<T,STAR,VR>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[MC,STAR] = [STAR,VR]"))
    DistMatrix<T,MC,MR> A_MC_MR(this->Grid());
    A_MC_MR.AlignColsWith(*this);
    A_MC_MR = A;
    *this = A_MC_MR;
    return *this;
}

template<typename T>
DM& DM::operator=( const DistMatrix<T,STAR,STAR>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[MC,STAR] = [STAR,STAR]"))
    this->ColFilterFrom( A );
    return *this;
}

template<typename T>
DM& DM::operator=( const DistMatrix<T,CIRC,CIRC>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[MC,STAR] = [CIRC,CIRC]"))
    DistMatrix<T,MC,MR> A_MC_MR( this->Grid() );
    A_MC_MR.AlignWith( *this );
    A_MC_MR = A;
    *this = A_MC_MR;
    return *this;
}

// Basic queries
// =============

template<typename T>
mpi::Comm DM::DistComm() const { return this->grid_->MCComm(); }
template<typename T>
mpi::Comm DM::RedundantComm() const { return this->grid_->MRComm(); }
template<typename T>
mpi::Comm DM::CrossComm() const { return mpi::COMM_SELF; }
template<typename T>
mpi::Comm DM::ColComm() const { return this->grid_->MCComm(); }
template<typename T>
mpi::Comm DM::RowComm() const { return mpi::COMM_SELF; }

template<typename T>
Int DM::ColStride() const { return this->grid_->MCSize(); }
template<typename T>
Int DM::RowStride() const { return 1; }
template<typename T>
Int DM::DistSize() const { return this->grid_->MCSize(); }
template<typename T>
Int DM::CrossSize() const { return 1; }
template<typename T>
Int DM::RedundantSize() const { return this->grid_->MRSize(); }

// Instantiate {Int,Real,Complex<Real>} for each Real in {float,double}
// ####################################################################

#define SELF(T,U,V) \
  template DistMatrix<T,ColDist,RowDist>::DistMatrix \
  ( const DistMatrix<T,U,V>& A );
#define OTHER(T,U,V) \
  template DistMatrix<T,ColDist,RowDist>::DistMatrix \
  ( const BlockDistMatrix<T,U,V>& A ); \
  template DistMatrix<T,ColDist,RowDist>& \
           DistMatrix<T,ColDist,RowDist>::operator= \
           ( const BlockDistMatrix<T,U,V>& A )
#define BOTH(T,U,V) \
  SELF(T,U,V); \
  OTHER(T,U,V)
#define PROTO(T) \
  template class DistMatrix<T,ColDist,RowDist>; \
  BOTH( T,CIRC,CIRC); \
  BOTH( T,MC,  MR  ); \
  OTHER(T,MC,  STAR); \
  BOTH( T,MD,  STAR); \
  BOTH( T,MR,  MC  ); \
  BOTH( T,MR,  STAR); \
  BOTH( T,STAR,MC  ); \
  BOTH( T,STAR,MD  ); \
  BOTH( T,STAR,MR  ); \
  BOTH( T,STAR,STAR); \
  BOTH( T,STAR,VC  ); \
  BOTH( T,STAR,VR  ); \
  BOTH( T,VC,  STAR); \
  BOTH( T,VR,  STAR);

#include "El/macros/Instantiate.h"

} // namespace El
