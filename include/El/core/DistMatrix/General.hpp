/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef EL_DISTMATRIX_GENERAL_DECL_HPP
#define EL_DISTMATRIX_GENERAL_DECL_HPP

namespace El {

template<typename T,Dist U,Dist V> 
class GeneralDistMatrix : public AbstractDistMatrix<T>
{
public:
    // Typedefs
    // ========
    typedef AbstractDistMatrix<T> absType;
    typedef GeneralDistMatrix<T,U,V> type;
    static constexpr Dist UDiag = DiagColDist<U,V>();
    static constexpr Dist VDiag = DiagRowDist<U,V>();
    static constexpr Dist UGath = GatheredDist<U>();
    static constexpr Dist VGath = GatheredDist<V>();
    static constexpr Dist UPart = PartialDist<U>();
    static constexpr Dist VPart = PartialDist<V>();
    static constexpr Dist UScat = PartialUnionColDist<U,V>();
    static constexpr Dist VScat = PartialUnionRowDist<U,V>();

    // Constructors and destructors
    // ============================
    // Move constructor
    GeneralDistMatrix( type&& A ) EL_NOEXCEPT;

    // Assignment and reconfiguration
    // ==============================
    // Move assignment
    type& operator=( type&& A );

    virtual void AlignColsWith
    ( const El::DistData& data, bool constrain=true, bool allowMismatch=false )
     override;
    virtual void AlignRowsWith
    ( const El::DistData& data, bool constrain=true, bool allowMismatch=false )
    override;

    void Translate( DistMatrix<T,U,V>& A ) const;

    void AllGather( DistMatrix<T,UGath,VGath>& A ) const;
    void ColAllGather( DistMatrix<T,UGath,V>& A ) const;
    void RowAllGather( DistMatrix<T,U,VGath>& A ) const;
    void PartialColAllGather( DistMatrix<T,UPart,V>& A ) const;
    void PartialRowAllGather( DistMatrix<T,U,VPart>& A ) const;

    void FilterFrom( const DistMatrix<T,UGath,VGath>& A );
    void ColFilterFrom( const DistMatrix<T,UGath,V>& A );
    void RowFilterFrom( const DistMatrix<T,U,VGath>& A );
    void PartialColFilterFrom( const DistMatrix<T,UPart,V>& A );
    void PartialRowFilterFrom( const DistMatrix<T,U,VPart>& A );

    void PartialColAllToAllFrom( const DistMatrix<T,UPart,VScat>& A );
    void PartialRowAllToAllFrom( const DistMatrix<T,UScat,VPart>& A );
    void PartialColAllToAll( DistMatrix<T,UPart,VScat>& A ) const;
    void PartialRowAllToAll( DistMatrix<T,UScat,VPart>& A ) const;

    void SumScatterFrom( const DistMatrix<T,UGath,VGath>& A );
    void RowSumScatterFrom( const DistMatrix<T,U,VGath>& A );
    void ColSumScatterFrom( const DistMatrix<T,UGath,V>& A );
    void PartialRowSumScatterFrom( const DistMatrix<T,U,VPart>& A );
    void PartialColSumScatterFrom( const DistMatrix<T,UPart,V>& A );

    void SumScatterUpdate( T alpha, const DistMatrix<T,UGath,VGath>& A );
    void RowSumScatterUpdate( T alpha, const DistMatrix<T,U,VGath>& A );
    void ColSumScatterUpdate( T alpha, const DistMatrix<T,UGath,V>& A );
    void PartialRowSumScatterUpdate( T alpha, const DistMatrix<T,U,VPart>& A );
    void PartialColSumScatterUpdate( T alpha, const DistMatrix<T,UPart,V>& A );

    // Transposed variants of some of the above routines which avoid 
    // large amounts of non-uniform data access
    // -------------------------------------------------------------
    void TransposeColAllGather
    ( DistMatrix<T,V,UGath>& A, bool conjugate=false ) const;
    void TransposePartialColAllGather
    ( DistMatrix<T,V,UPart>& A, bool conjugate=false ) const;
    void AdjointColAllGather( DistMatrix<T,V,UGath>& A ) const;
    void AdjointPartialColAllGather( DistMatrix<T,V,UPart>& A ) const;

    void TransposeColFilterFrom
    ( const DistMatrix<T,V,UGath>& A, bool conjugate=false );
    void TransposeRowFilterFrom
    ( const DistMatrix<T,VGath,U>& A, bool conjugate=false );
    void TransposePartialColFilterFrom
    ( const DistMatrix<T,V,UPart>& A, bool conjugate=false );
    void TransposePartialRowFilterFrom
    ( const DistMatrix<T,VPart,U>& A, bool conjugate=false );
    void AdjointColFilterFrom( const DistMatrix<T,V,UGath>& A );
    void AdjointRowFilterFrom( const DistMatrix<T,VGath,U>& A );
    void AdjointPartialColFilterFrom( const DistMatrix<T,V,UPart>& A );
    void AdjointPartialRowFilterFrom( const DistMatrix<T,VPart,U>& A );

    void TransposeColSumScatterFrom
    ( const DistMatrix<T,V,UGath>& A, bool conjugate=false );
    void TransposePartialColSumScatterFrom
    ( const DistMatrix<T,V,UPart>& A, bool conjugate=false );
    void AdjointColSumScatterFrom( const DistMatrix<T,V,UGath>& A );
    void AdjointPartialColSumScatterFrom( const DistMatrix<T,V,UPart>& A );

    void TransposeColSumScatterUpdate
    ( T alpha, const DistMatrix<T,V,UGath>& A, bool conjugate=false );
    void TransposePartialColSumScatterUpdate
    ( T alpha, const DistMatrix<T,V,UPart>& A, bool conjugate=false );
    void AdjointColSumScatterUpdate
    ( T alpha, const DistMatrix<T,V,UGath>& A );
    void AdjointPartialColSumScatterUpdate
    ( T alpha, const DistMatrix<T,V,UPart>& A );

    // Basic queries
    // =============
    // Distribution information
    // ------------------------
    Dist ColDist() const override;
    Dist RowDist() const override;
    Dist PartialColDist() const override;
    Dist PartialRowDist() const override;
    Dist PartialUnionColDist() const override;
    Dist PartialUnionRowDist() const override;

    // Diagonal manipulation
    // =====================
    void GetDiagonal
    ( AbstractDistMatrix<T>& d, Int offset=0 ) const override;
    void GetRealPartOfDiagonal
    ( AbstractDistMatrix<Base<T>>& d, Int offset=0 ) const override;
    void GetImagPartOfDiagonal
    ( AbstractDistMatrix<Base<T>>& d, Int offset=0 ) const override;

    DistMatrix<T,UDiag,VDiag> GetDiagonal( Int offset=0 ) const;
    DistMatrix<Base<T>,UDiag,VDiag> GetRealPartOfDiagonal( Int offset=0 ) const;
    DistMatrix<Base<T>,UDiag,VDiag> GetImagPartOfDiagonal( Int offset=0 ) const;

    void SetDiagonal
    ( const AbstractDistMatrix<T>& d, Int offset=0 ) override;
    void SetRealPartOfDiagonal
    ( const AbstractDistMatrix<Base<T>>& d, Int offset=0 ) override;
    void SetImagPartOfDiagonal
    ( const AbstractDistMatrix<Base<T>>& d, Int offset=0 ) override;

    void UpdateDiagonal
    ( T alpha, const AbstractDistMatrix<T>& d, Int offset=0 ) 
    override;
    void UpdateRealPartOfDiagonal
    ( Base<T> alpha, const AbstractDistMatrix<Base<T>>& d, Int offset=0 ) 
    override;
    void UpdateImagPartOfDiagonal
    ( Base<T> alpha, const AbstractDistMatrix<Base<T>>& d, Int offset=0 ) 
     override;

protected:

    // Private constructors
    // ====================

    // Inherited constructors are part of C++11 but not yet widely supported.
    //using AbstractDistMatrix<T>::AbstractDistMatrix;

    // Create a 0 x 0 distributed matrix
    GeneralDistMatrix( const El::Grid& g=DefaultGrid(), Int root=0 );

    // Diagonal helper routines
    // ========================
    template<typename S,class Function>
    void GetDiagonalHelper
    ( AbstractDistMatrix<S>& d, Int offset, Function func ) const;
    template<typename S,class Function>
    void SetDiagonalHelper
    ( const AbstractDistMatrix<S>& d, Int offset, Function func );

    // Friend declarations
    // ===================
    template<typename S,Dist J,Dist K> friend class DistMatrix;
    template<typename S,Dist J,Dist K> friend class BlockDistMatrix;
};

} // namespace El

#endif // ifndef EL_DISTMATRIX_GENERAL_DECL_HPP
