/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#include "El.hpp"

#define ColDist MR
#define RowDist MC

#include "./setup.hpp"

namespace El {

// Public section
// ##############

// Assignment and reconfiguration
// ==============================

template<typename T>
BDM& BDM::operator=( const BlockDistMatrix<T,MC,MR>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[MR,MC] = [MC,MR]"))
    LogicError("This routine is not yet written");
    return *this;
}

template<typename T>
BDM& BDM::operator=( const BlockDistMatrix<T,MC,STAR>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[MR,MC] = [MC,STAR]"))
    auto A_VC_STAR = MakeUnique<BlockDistMatrix<T,VC,STAR>>( A );
    auto A_VR_STAR = MakeUnique<BlockDistMatrix<T,VR,STAR>>( this->Grid() );
    A_VR_STAR->AlignColsWith(*this);
    *A_VR_STAR = *A_VC_STAR;
    A_VC_STAR.reset(); 

    *this = *A_VR_STAR;
    return *this;
}

template<typename T>
BDM& BDM::operator=( const BlockDistMatrix<T,STAR,MR>& A )
{ 
    DEBUG_ONLY(CallStackEntry cse("[MR,MC] = [STAR,MR]"))
    auto A_STAR_VR = MakeUnique<BlockDistMatrix<T,STAR,VR>>( A );
    auto A_STAR_VC = MakeUnique<BlockDistMatrix<T,STAR,VC>>( this->Grid() );
    A_STAR_VR->AlignRowsWith(*this);
    *A_STAR_VC = *A_STAR_VR;
    A_STAR_VR.reset(); 

    *this = *A_STAR_VC;
    return *this;
}

template<typename T>
BDM& BDM::operator=( const BlockDistMatrix<T,MD,STAR>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[MR,MC] = [MD,STAR]"))
    // TODO: More efficient implementation?
    BlockDistMatrix<T,STAR,STAR> A_STAR_STAR( A );
    *this = A_STAR_STAR;
    return *this;
}

template<typename T>
BDM& BDM::operator=( const BlockDistMatrix<T,STAR,MD>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[MR,MC] = [STAR,MD]"))
    // TODO: More efficient implementation?
    BlockDistMatrix<T,STAR,STAR> A_STAR_STAR( A );
    *this = A_STAR_STAR;
    return *this;
}

template<typename T>
BDM& BDM::operator=( const BDM& A )
{ 
    DEBUG_ONLY(CallStackEntry cse("[MR,MC] = [MR,MC]"))
    A.Translate( *this );
    return *this;
}

template<typename T>
BDM& BDM::operator=( const BlockDistMatrix<T,MR,STAR>& A )
{ 
    DEBUG_ONLY(CallStackEntry cse("[MR,MC] = [MR,STAR]"))
    this->RowFilterFrom( A );
    return *this;
}

template<typename T>
BDM& BDM::operator=( const BlockDistMatrix<T,STAR,MC>& A )
{ 
    DEBUG_ONLY(CallStackEntry cse("[MR,MC] = [STAR,MC]"))
    this->ColFilterFrom( A );
    return *this;
}

template<typename T>
BDM& BDM::operator=( const BlockDistMatrix<T,VC,STAR>& A )
{ 
    DEBUG_ONLY(CallStackEntry cse("[MR,MC] = [VC,STAR]"))
    BlockDistMatrix<T,VR,STAR> A_VR_STAR( A );
    *this = A_VR_STAR;
    return *this;
}

template<typename T>
BDM& BDM::operator=( const BlockDistMatrix<T,STAR,VC>& A )
{ 
    DEBUG_ONLY(CallStackEntry cse("[MR,MC] = [STAR,VC]"))
    A.PartialRowAllToAll( *this );
    return *this;
}

template<typename T>
BDM& BDM::operator=( const BlockDistMatrix<T,VR,STAR>& A )
{ 
    DEBUG_ONLY(CallStackEntry cse("[MR,MC] = [VR,STAR]"))
    A.PartialColAllToAll( *this );
    return *this;
}

template<typename T>
BDM& BDM::operator=( const BlockDistMatrix<T,STAR,VR>& A )
{ 
    DEBUG_ONLY(CallStackEntry cse("[MR,MC] = [STAR,VR]"))
    BlockDistMatrix<T,STAR,VC> A_STAR_VC( A );
    *this = A_STAR_VC;
    return *this;
}

template<typename T>
BDM& BDM::operator=( const BlockDistMatrix<T,STAR,STAR>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[MR,MC] = [STAR,STAR]"))
    this->FilterFrom( A );
    return *this;
}

template<typename T>
BDM& BDM::operator=( const BlockDistMatrix<T,CIRC,CIRC>& A )
{
    DEBUG_ONLY(CallStackEntry cse("[MR,MC] = [CIRC,CIRC]"))
    LogicError("This routine is not yet written");
    return *this;
}

// Basic queries
// =============

template<typename T>
mpi::Comm BDM::DistComm() const { return this->grid_->VRComm(); }
template<typename T>
mpi::Comm BDM::CrossComm() const { return mpi::COMM_SELF; }
template<typename T>
mpi::Comm BDM::RedundantComm() const { return mpi::COMM_SELF; }
template<typename T>
mpi::Comm BDM::ColComm() const { return this->grid_->MRComm(); }
template<typename T>
mpi::Comm BDM::RowComm() const { return this->grid_->MCComm(); }

template<typename T>
Int BDM::ColStride() const { return this->grid_->MRSize(); }
template<typename T>
Int BDM::RowStride() const { return this->grid_->MCSize(); }
template<typename T>
Int BDM::DistSize() const { return this->grid_->VRSize(); }
template<typename T>
Int BDM::CrossSize() const { return 1; }
template<typename T>
Int BDM::RedundantSize() const { return 1; }

// Private section
// ###############

// Instantiate {Int,Real,Complex<Real>} for each Real in {float,double}
// ####################################################################

#define SELF(T,U,V) \
  template BlockDistMatrix<T,ColDist,RowDist>::BlockDistMatrix \
  ( const BlockDistMatrix<T,U,V>& A );
#define OTHER(T,U,V) \
  template BlockDistMatrix<T,ColDist,RowDist>::BlockDistMatrix \
  ( const DistMatrix<T,U,V>& A ); \
  template BlockDistMatrix<T,ColDist,RowDist>& \
           BlockDistMatrix<T,ColDist,RowDist>::operator= \
           ( const DistMatrix<T,U,V>& A )
#define BOTH(T,U,V) \
  SELF(T,U,V); \
  OTHER(T,U,V)
#define PROTO(T) \
  template class BlockDistMatrix<T,ColDist,RowDist>; \
  BOTH( T,CIRC,CIRC); \
  BOTH( T,MC,  MR  ); \
  BOTH( T,MC,  STAR); \
  BOTH( T,MD,  STAR); \
  OTHER(T,MR,  MC  ); \
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
