/*
   Copyright (c) 2009-2011, Jack Poulson
   All rights reserved.

   This file is part of Elemental.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

    - Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    - Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    - Neither the name of the owner nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
*/
#include "elemental/basic_internal.hpp"
using namespace std;
using namespace elemental;

// Template conventions:
//   G: general datatype
//
//   T: any ring, e.g., the (Gaussian) integers and the real/complex numbers
//   Z: representation of a real ring, e.g., the integers or real numbers
//   std::complex<Z>: representation of a complex ring, e.g. Gaussian integers
//                    or complex numbers
//
//   F: representation of real or complex number
//   R: representation of real number
//   std::complex<R>: representation of complex number

template<typename T>
void
elemental::basic::internal::HetrmmLVar1( DistMatrix<T,MC,MR>& L )
{
#ifndef RELEASE
    PushCallStack("basic::internal::HetrmmLVar1");
    if( L.Height() != L.Width() )
        throw std::logic_error("L must be square");
#endif
    const Grid& g = L.Grid();

    // Matrix views
    DistMatrix<T,MC,MR>
        LTL(g), LTR(g),  L00(g), L01(g), L02(g),
        LBL(g), LBR(g),  L10(g), L11(g), L12(g),
                         L20(g), L21(g), L22(g);

    // Temporary distributions
    DistMatrix<T,STAR,VR  > L10_STAR_VR(g);
    DistMatrix<T,STAR,VC  > L10_STAR_VC(g);
    DistMatrix<T,STAR,MC  > L10_STAR_MC(g);
    DistMatrix<T,STAR,MR  > L10_STAR_MR(g);
    DistMatrix<T,STAR,STAR> L11_STAR_STAR(g);

    PartitionDownDiagonal
    ( L, LTL, LTR,
         LBL, LBR, 0 );
    while( LTL.Height() < L.Height() && LTL.Width() < L.Height() )
    {
        RepartitionDownDiagonal 
        ( LTL, /**/ LTR,  L00, /**/ L01, L02,
         /*************/ /******************/
               /**/       L10, /**/ L11, L12,
          LBL, /**/ LBR,  L20, /**/ L21, L22 );

        L10_STAR_VR.AlignWith( L00 );
        L10_STAR_VC.AlignWith( L00 );
        L10_STAR_MC.AlignWith( L00 );
        L10_STAR_MR.AlignWith( L00 );
        //--------------------------------------------------------------------//
        L10_STAR_VR = L10;
        L10_STAR_VC = L10_STAR_VR;
        L10_STAR_MC = L10_STAR_VC;
        L10_STAR_MR = L10_STAR_VR;
        basic::internal::LocalTriangularRankK
        ( LOWER, ADJOINT, (T)1, L10_STAR_MC, L10_STAR_MR, (T)1, L22 );

        L11_STAR_STAR = L11;
        basic::internal::LocalTrmm
        ( LEFT, LOWER, ADJOINT, NON_UNIT, (T)1, L11_STAR_STAR, L10_STAR_VR );
        L10 = L10_STAR_VR;

        basic::internal::LocalHetrmm( LOWER, L11_STAR_STAR );
        L11 = L11_STAR_STAR;
        //--------------------------------------------------------------------//
        L10_STAR_MR.FreeAlignments();
        L10_STAR_MC.FreeAlignments();
        L10_STAR_VC.FreeAlignments();
        L10_STAR_VR.FreeAlignments();

        SlidePartitionDownDiagonal
        ( LTL, /**/ LTR,  L00, L01, /**/ L02,
               /**/       L10, L11, /**/ L12,
         /*************/ /******************/
          LBL, /**/ LBR,  L20, L21, /**/ L22 );
    }
#ifndef RELEASE
    PopCallStack();
#endif
}

template void elemental::basic::internal::HetrmmLVar1
( DistMatrix<float,MC,MR>& L );

template void elemental::basic::internal::HetrmmLVar1
( DistMatrix<double,MC,MR>& L );

#ifndef WITHOUT_COMPLEX
template void elemental::basic::internal::HetrmmLVar1
( DistMatrix<scomplex,MC,MR>& L );

template void elemental::basic::internal::HetrmmLVar1
( DistMatrix<dcomplex,MC,MR>& L );
#endif
