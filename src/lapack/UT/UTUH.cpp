/*
   This file is part of elemental, a library for distributed-memory dense 
   linear algebra.

   Copyright (C) 2009-2010 Jack Poulson <jack.poulson@gmail.com>

   This program is released under the terms of the license contained in the
   file LICENSE.
*/
#include "elemental/blas_internal.hpp"
#include "elemental/lapack_internal.hpp"
using namespace elemental;
using namespace std;

namespace {

template<typename T>
void
SetDiagonalToOne( int offset, DistMatrix<T,Star,VR>& A )
{
#ifndef RELEASE
    PushCallStack("SetDiagonalToOne");
#endif
    const int height = A.Height();
    const int localWidth = A.LocalWidth();
    const int p = A.GetGrid().Size();
    const int rowShift = A.RowShift();

    for( int jLoc=0; jLoc<localWidth; ++jLoc )
    {
        const int j = rowShift + jLoc*p;
        if( (j-offset) > 0 && (j-offset) < height )
            A.LocalEntry(j-offset,jLoc) = (T)1;
    }
#ifndef RELEASE
    PopCallStack();
#endif
}

template<typename T>
void 
HalveMainDiagonal( DistMatrix<T,Star,Star>& A )
{
#ifndef RELEASE
    PushCallStack("HalveMainDiagonal");
#endif
    for( int j=0; j<A.Height(); ++j )
        A.LocalEntry(j,j) /= (T)2;
#ifndef RELEASE
    PopCallStack();
#endif
}

}

// This routine reverses the accumulation of Householder transforms stored 
// in the portion of H above the diagonal marked by 'offset'. It is assumed 
// that the Householder transforms were accumulated right-to-left.
template<typename T>
void
elemental::lapack::internal::UTUH
( int offset, 
  const DistMatrix<T,MC,MR>& H,
        DistMatrix<T,MC,MR>& A )
{
#ifndef RELEASE
    PushCallStack("lapack::internal::UTUH");
    if( H.GetGrid() != A.GetGrid() )
        throw logic_error( "H and A must be distributed over the same grid." );
    if( offset > H.Height() )
        throw logic_error( "Transforms cannot extend above matrix." );
    if( offset < 0 )
        throw logic_error( "Transforms cannot extend below matrix." );
    if( H.Width() != A.Height() )
        throw logic_error
              ( "Width of transform must equal height of target matrix." );
#endif
    const Grid& g = H.GetGrid();

    // Matrix views    
    DistMatrix<T,MC,MR>
        HTL(g), HTR(g),  H00(g), H01(g), H02(g),  HPan(g),
        HBL(g), HBR(g),  H10(g), H11(g), H12(g),
                         H20(g), H21(g), H22(g);
    DistMatrix<T,MC,MR>
        ATL(g), ATR(g),  A00(g), A01(g), A02(g),
        ABL(g), ABR(g),  A10(g), A11(g), A12(g),
                         A20(g), A21(g), A22(g);

    DistMatrix<T,Star,VR  > HPan_Star_VR(g);
    DistMatrix<T,Star,MC  > HPan_Star_MC(g);
    DistMatrix<T,Star,Star> U_Star_Star(g);
    DistMatrix<T,Star,MR  > Z_Star_MR(g);
    DistMatrix<T,Star,VR  > Z_Star_VR(g);

    LockedPartitionDownDiagonal
    ( H, HTL, HTR,
         HBL, HBR, 0 );
    PartitionDownDiagonal
    ( A, ATL, ATR,
         ABL, ABR, 0 );
    while( HTL.Width() < H.Width() )
    {
        LockedRepartitionDownDiagonal
        ( HTL, /**/ HTR,  H00, /**/ H01, H02,
         /*************/ /******************/
               /**/       H10, /**/ H11, H12,
          HBL, /**/ HBR,  H20, /**/ H21, H22 );

        RepartitionDownDiagonal
        ( ATL, /**/ ATR,  A00, /**/ A01, A02,
         /*************/ /******************/
               /**/       A10, /**/ A11, A12,
          ABL, /**/ ABR,  A20, /**/ A21, A22 );

        HPan.LockedView1x2( H11, H12 );

        HPan_Star_MC.AlignWith( ABR );
        Z_Star_MR.AlignWith( ABR );
        Z_Star_VR.AlignWith( ABR );
        Z_Star_MR.ResizeTo( HPan.Width(), ABR.Width() );
        U_Star_Star.ResizeTo( HPan.Width(), HPan.Width() );
        //--------------------------------------------------------------------//
        HPan_Star_VR = HPan;
        HPan_Star_VR.MakeTrapezoidal( Left, Upper, offset );
        SetDiagonalToOne( offset, HPan_Star_VR );
        blas::Herk
        ( Upper, Normal, 
          (T)1, HPan_Star_VR.LockedLocalMatrix(),
          (T)0, U_Star_Star.LocalMatrix() ); 
        U_Star_Star.AllSum();
        HalveMainDiagonal( U_Star_Star );

        HPan_Star_MC = HPan_Star_VR;
        blas::internal::LocalGemm
        ( Normal, Normal, (T)1, HPan_Star_MC, ABR, (T)0, Z_Star_MR );
        Z_Star_VR.SumScatterFrom( Z_Star_MR );
        
        blas::internal::LocalTrsm
        ( Left, Upper, ConjugateTranspose, NonUnit, 
          (T)1, U_Star_Star, Z_Star_VR );

        Z_Star_MR = Z_Star_VR;
        blas::internal::LocalGemm
        ( ConjugateTranspose, Normal, 
          (T)-1, HPan_Star_MC, Z_Star_MR, (T)1, ABR );
        //--------------------------------------------------------------------//
        HPan_Star_MC.FreeAlignments();
        Z_Star_MR.FreeAlignments();
        Z_Star_VR.FreeAlignments();

        SlideLockedPartitionDownDiagonal
        ( HTL, /**/ HTR,  H00, H01, /**/ H02,
               /**/       H10, H11, /**/ H12,
         /*************/ /******************/
          HBL, /**/ HBR,  H20, H21, /**/ H22 );

        SlidePartitionUpDiagonal
        ( ATL, /**/ ATR,  A00, A01, /**/ A02,
         /*************/ /******************/
               /**/       A10, A11, /**/ A12,
          ABL, /**/ ABR,  A20, A21, /**/ A22 );
    }
#ifndef RELEASE
    PopCallStack();
#endif
}

template void elemental::lapack::internal::UTUH
( int offset,
  const DistMatrix<float,MC,MR>& H,
        DistMatrix<float,MC,MR>& A );

template void elemental::lapack::internal::UTUH
( int offset,
  const DistMatrix<double,MC,MR>& H,
        DistMatrix<double,MC,MR>& A );

#ifndef WITHOUT_COMPLEX
template void elemental::lapack::internal::UTUH
( int offset,
  const DistMatrix<scomplex,MC,MR>& H,
        DistMatrix<scomplex,MC,MR>& A );

template void elemental::lapack::internal::UTUH
( int offset,
  const DistMatrix<dcomplex,MC,MR>& H,
        DistMatrix<dcomplex,MC,MR>& A );
#endif
