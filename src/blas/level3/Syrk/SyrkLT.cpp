/*
   This file is part of elemental, a library for distributed-memory dense 
   linear algebra.

   Copyright (C) 2009-2010 Jack Poulson <jack.poulson@gmail.com>

   This program is released under the terms of the license contained in the 
   file LICENSE.
*/
#include "elemental/blas_internal.hpp"
using namespace std;
using namespace elemental;

template<typename T>
void
elemental::blas::internal::SyrkLT
( T alpha, const DistMatrix<T,MC,MR>& A,
  T beta,        DistMatrix<T,MC,MR>& C )
{
#ifndef RELEASE
    PushCallStack("blas::internal::SyrkLT");
    if( A.GetGrid() != C.GetGrid() )
        throw logic_error( "A and C must be distributed over the same grid." );
    if( A.Width() != C.Height() || A.Width() != C.Width() )
    {
        ostringstream msg;
        msg << "Nonconformal SyrkLT:" << endl
            << "  A ~ " << A.Height() << " x " << A.Width() << endl
            << "  C ~ " << C.Height() << " x " << C.Width() << endl;
        throw logic_error( msg.str() );
    }
#endif
    const Grid& g = A.GetGrid();

    // Matrix views
    DistMatrix<T,MC,MR> AT(g),  A0(g),
                        AB(g),  A1(g),
                                A2(g);

    // Temporary distributions
    DistMatrix<T,MR,  Star> A1Trans_MR_Star(g);
    DistMatrix<T,Star,VR  > A1_Star_VR(g);
    DistMatrix<T,Star,MC  > A1_Star_MC(g);

    // Start the algorithm
    blas::Scal( beta, C );
    LockedPartitionDown
    ( A, AT, 
         AB, 0 );
    while( AB.Height() > 0 )
    {
        LockedRepartitionDown
        ( AT,  A0,
         /**/ /**/
               A1,
          AB,  A2 );

        A1Trans_MR_Star.AlignWith( C );
        A1_Star_MC.AlignWith( C );
        //--------------------------------------------------------------------//
        A1Trans_MR_Star.TransposeFrom( A1 );
        A1_Star_VR.TransposeFrom( A1Trans_MR_Star );
        A1_Star_MC = A1_Star_VR;

        blas::internal::LocalTriangularRankK
        ( Lower, Transpose, Transpose, 
          alpha, A1_Star_MC, A1Trans_MR_Star, (T)1, C );
        //--------------------------------------------------------------------//
        A1Trans_MR_Star.FreeAlignments();
        A1_Star_MC.FreeAlignments();

        SlideLockedPartitionDown
        ( AT,  A0,
               A1,
         /**/ /**/
          AB,  A2 );
    }
#ifndef RELEASE
    PopCallStack();
#endif
}

template void elemental::blas::internal::SyrkLT
( float alpha, const DistMatrix<float,MC,MR>& A,
  float beta,        DistMatrix<float,MC,MR>& C );

template void elemental::blas::internal::SyrkLT
( double alpha, const DistMatrix<double,MC,MR>& A,
  double beta,        DistMatrix<double,MC,MR>& C );

#ifndef WITHOUT_COMPLEX
template void elemental::blas::internal::SyrkLT
( scomplex alpha, const DistMatrix<scomplex,MC,MR>& A,
  scomplex beta,        DistMatrix<scomplex,MC,MR>& C );

template void elemental::blas::internal::SyrkLT
( dcomplex alpha, const DistMatrix<dcomplex,MC,MR>& A,
  dcomplex beta,        DistMatrix<dcomplex,MC,MR>& C );
#endif
