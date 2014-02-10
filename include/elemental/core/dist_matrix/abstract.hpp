/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef ELEM_DISTMATRIX_ABSTRACT_DECL_HPP
#define ELEM_DISTMATRIX_ABSTRACT_DECL_HPP

namespace elem {

template<typename T> 
class AbstractDistMatrix
{
public:
    // Typedefs
    // ========
    typedef AbstractDistMatrix<T> type;

    // Constructors and destructors
    // ============================
#ifndef SWIG
    // Move constructor
    AbstractDistMatrix( type&& A );
#endif
    virtual ~AbstractDistMatrix();

    // Assignment and reconfiguration
    // ==============================
#ifndef SWIG
    // Move assignment
    type& operator=( type&& A );
#endif
    void Empty();
    void EmptyData();
    void SetGrid( const elem::Grid& grid );
    void Resize( Int height, Int width );
    void Resize( Int height, Int width, Int ldim );
    void MakeConsistent();
    // Realignment
    // -----------
    void Align( Int colAlign, Int rowAlign );
    void AlignCols( Int colAlign );
    void AlignRows( Int rowAlign );
    void FreeAlignments();
    void SetRoot( Int root );
    virtual void AlignWith( const elem::DistData& data );
    virtual void AlignColsWith( const elem::DistData& data );
    virtual void AlignRowsWith( const elem::DistData& data );
    // Buffer attachment
    // -----------------
    // (Immutable) view of a distributed matrix's buffer
    void Attach
    ( Int height, Int width, Int colAlign, Int rowAlign,
      T* buffer, Int ldim, const elem::Grid& grid, Int root=0 );
    void LockedAttach
    ( Int height, Int width, Int colAlign, Int rowAlign,
      const T* buffer, Int ldim, const elem::Grid& grid, Int root=0 );
    void Attach
    ( Matrix<T>& A, Int colAlign, Int rowAlign, const elem::Grid& grid, 
      Int root=0 );
    void LockedAttach
    ( const Matrix<T>& A, Int colAlign, Int rowAlign, const elem::Grid& grid, 
      Int root=0 );

    // Basic queries
    // =============

    // Global matrix information
    // -------------------------
    Int Height() const;
    Int Width() const;
    Int DiagonalLength( Int offset=0 ) const;
    bool Viewing() const;
    bool Locked()  const;

    // Local matrix information
    // ------------------------
    Int LocalHeight() const;
    Int LocalWidth() const;
    Int LDim() const;
          elem::Matrix<T>& Matrix();
    const elem::Matrix<T>& LockedMatrix() const;
    size_t AllocatedMemory() const;
          T* Buffer();
          T* Buffer( Int iLoc, Int jLoc );
    const T* LockedBuffer() const;
    const T* LockedBuffer( Int iLoc, Int jLoc ) const;

    // Distribution information
    // ------------------------
    const elem::Grid& Grid() const;
    bool ColConstrained() const;
    bool RowConstrained() const;
    Int ColAlign() const;
    Int RowAlign() const;
    Int ColShift() const;
    Int RowShift() const;
    Int ColRank() const;
    Int RowRank() const;
    Int PartialColRank() const;
    Int PartialRowRank() const;
    Int PartialUnionColRank() const;
    Int PartialUnionRowRank() const; 
    Int DistRank() const;
    Int CrossRank() const;
    Int RedundantRank() const;
    Int DistSize() const;
    Int CrossSize() const;
    Int RedundantSize() const;
    Int Root() const;
    bool Participating() const;
    Int RowOwner( Int i ) const;     // rank in ColComm
    Int ColOwner( Int j ) const;     // rank in RowComm
    Int Owner( Int i, Int j ) const; // rank in DistComm
    Int LocalRow( Int i ) const; // debug throws if row i is not locally owned
    Int LocalCol( Int j ) const; // debug throws if col j is not locally owned
    bool IsLocalRow( Int i ) const; 
    bool IsLocalCol( Int j ) const;
    bool IsLocal( Int i, Int j ) const;
    // Must be overridden
    // ^^^^^^^^^^^^^^^^^^
    virtual elem::DistData DistData() const = 0;
    virtual mpi::Comm DistComm() const = 0;
    virtual mpi::Comm CrossComm() const = 0;
    virtual mpi::Comm RedundantComm() const = 0;
    virtual mpi::Comm ColComm() const = 0;
    virtual mpi::Comm RowComm() const = 0;
    virtual mpi::Comm PartialColComm() const;
    virtual mpi::Comm PartialRowComm() const;
    virtual mpi::Comm PartialUnionColComm() const;
    virtual mpi::Comm PartialUnionRowComm() const;
    virtual Int ColStride() const = 0;
    virtual Int RowStride() const = 0;
    virtual Int PartialColStride() const;
    virtual Int PartialRowStride() const;
    virtual Int PartialUnionColStride() const;
    virtual Int PartialUnionRowStride() const;

    // Single-entry manipulation
    // =========================

    // Global entry manipulation 
    // -------------------------
    // NOTE: Local entry manipulation is often much faster and should be
    //       preferred in most circumstances where performance matters.
    T Get( Int i, Int j ) const;
    BASE(T) GetRealPart( Int i, Int j ) const;
    BASE(T) GetImagPart( Int i, Int j ) const;
    void Set( Int i, Int j, T alpha );
    void SetRealPart( Int i, Int j, BASE(T) alpha );
    void SetImagPart( Int i, Int j, BASE(T) alpha );
    void Update( Int i, Int j, T alpha );
    void UpdateRealPart( Int i, Int j, BASE(T) alpha );
    void UpdateImagPart( Int i, Int j, BASE(T) alpha );
    void MakeReal( Int i, Int j );
    void Conjugate( Int i, Int j );

    // Local entry manipulation
    // ------------------------
    T GetLocal( Int iLoc, Int jLoc ) const;
    BASE(T) GetLocalRealPart( Int iLoc, Int jLoc ) const;
    BASE(T) GetLocalImagPart( Int iLoc, Int jLoc ) const;
    void SetLocal( Int iLoc, Int jLoc, T alpha );
    void SetLocalRealPart( Int iLoc, Int jLoc, BASE(T) alpha );
    void SetLocalImagPart( Int iLoc, Int jLoc, BASE(T) alpha );
    void UpdateLocal( Int iLoc, Int jLoc, T alpha );
    void UpdateLocalRealPart( Int iLoc, Int jLoc, BASE(T) alpha );
    void UpdateLocalImagPart( Int iLoc, Int jLoc, BASE(T) alpha );
    void MakeRealLocal( Int iLoc, Int jLoc );
    void ConjugateLocal( Int iLoc, Int jLoc );

    // Diagonal manipulation
    // =====================
    void MakeDiagonalReal( Int offset=0 );
    void ConjugateDiagonal( Int offset=0 );

    // Arbitrary-submatrix manipulation
    // ================================

    // Global submatrix manipulation
    // -----------------------------
    void Get
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd,
      DistMatrix<T,STAR,STAR>& ASub ) const;
    void GetRealPart
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd,
      DistMatrix<BASE(T),STAR,STAR>& ASub ) const;
    void GetImagPart
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd,
      DistMatrix<BASE(T),STAR,STAR>& ASub ) const;
    DistMatrix<T,STAR,STAR> Get
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd ) const;
    DistMatrix<BASE(T),STAR,STAR> GetRealPart
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd ) const;
    DistMatrix<BASE(T),STAR,STAR> GetImagPart
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd ) const;

    void Set
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd,
      const DistMatrix<T,STAR,STAR>& ASub );
    void SetRealPart
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd,
      const DistMatrix<BASE(T),STAR,STAR>& ASub );
    void SetImagPart
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd,
      const DistMatrix<BASE(T),STAR,STAR>& ASub );

    void Update
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd,
      T alpha, const DistMatrix<T,STAR,STAR>& ASub );
    void UpdateRealPart
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd,
      BASE(T) alpha, const DistMatrix<BASE(T),STAR,STAR>& ASub );
    void UpdateImagPart
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd,
      BASE(T) alpha, const DistMatrix<BASE(T),STAR,STAR>& ASub );

    void MakeReal
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd );
    void Conjugate
    ( const std::vector<Int>& rowInd, const std::vector<Int>& colInd );

    // Local submatrix manipulation
    // ----------------------------
    void GetLocal
    ( const std::vector<Int>& rowIndLoc, const std::vector<Int>& colIndLoc,
      elem::Matrix<T>& ASub ) const;
    void GetLocalRealPart
    ( const std::vector<Int>& rowIndLoc, const std::vector<Int>& colIndLoc,
      elem::Matrix<BASE(T)>& ASub ) const;
    void GetLocalImagPart
    ( const std::vector<Int>& rowIndLoc, const std::vector<Int>& colIndLoc,
      elem::Matrix<BASE(T)>& ASub ) const;
    elem::Matrix<T> GetLocal
    ( const std::vector<Int>& rowIndLoc, 
      const std::vector<Int>& colIndLoc ) const;
    elem::Matrix<BASE(T)> GetLocalRealPart
    ( const std::vector<Int>& rowIndLoc, 
      const std::vector<Int>& colIndLoc ) const;
    elem::Matrix<BASE(T)> GetLocalImagPart
    ( const std::vector<Int>& rowIndLoc, 
      const std::vector<Int>& colIndLoc ) const;

    void SetLocal
    ( const std::vector<Int>& rowIndLoc, const std::vector<Int>& colIndLoc,
      const elem::Matrix<T>& ASub );
    void SetLocalRealPart
    ( const std::vector<Int>& rowIndLoc, const std::vector<Int>& colIndLoc,
      const elem::Matrix<BASE(T)>& ASub );
    void SetLocalImagPart
    ( const std::vector<Int>& rowIndLoc, const std::vector<Int>& colIndLoc,
      const elem::Matrix<BASE(T)>& ASub );

    void UpdateLocal
    ( const std::vector<Int>& rowIndLoc, const std::vector<Int>& colIndLoc,
      T alpha, const elem::Matrix<T>& ASub );
    void UpdateLocalRealPart
    ( const std::vector<Int>& rowIndLoc, const std::vector<Int>& colIndLoc,
      BASE(T) alpha, const elem::Matrix<BASE(T)>& ASub );
    void UpdateLocalImagPart
    ( const std::vector<Int>& rowIndLoc, const std::vector<Int>& colIndLoc,
      BASE(T) alpha, const elem::Matrix<BASE(T)>& ASub );

    void MakeRealLocal
    ( const std::vector<Int>& rowIndLoc, const std::vector<Int>& colIndLoc );
    void ConjugateLocal
    ( const std::vector<Int>& rowIndLoc, const std::vector<Int>& colIndLoc );

    // Combined realignment and resize
    // ===============================
    void AlignAndResize
    ( Int colAlign, Int rowAlign, Int height, Int width, bool force=false );
    void AlignColsAndResize
    ( Int colAlign, Int height, Int width, bool force=false );
    void AlignRowsAndResize
    ( Int rowAlign, Int height, Int width, bool force=false );

    // Assertions
    // ==========
    void ComplainIfReal() const;
    void AssertNotLocked() const;
    void AssertNotStoringData() const;
    void AssertValidEntry( Int i, Int j ) const;
    void AssertValidSubmatrix( Int i, Int j, Int height, Int width ) const;
    void AssertSameGrid( const elem::Grid& grid ) const;
    void AssertSameSize( Int height, Int width ) const;

protected:
    // Member variables
    // ================

    // Global and local matrix information 
    // -----------------------------------
    ViewType viewType_;
    Int height_, width_;
    Memory<T> auxMemory_;
    elem::Matrix<T> matrix_;
    
    // Process grid and distribution metadata
    // --------------------------------------
    bool colConstrained_, rowConstrained_;
    Int colAlign_, rowAlign_,
        colShift_, rowShift_;
    Int root_;
    const elem::Grid* grid_;

    // Construct using a particular process grid
    // =========================================
    AbstractDistMatrix( const elem::Grid& g );

    // Exchange metadata with another matrix
    // =====================================
    virtual void ShallowSwap( type& A );

    // Modify the distribution metadata
    // ================================
    void SetShifts();
    void SetColShift();
    void SetRowShift();
    void SetGrid();

    // Friend declarations
    // ===================
#ifndef SWIG
    template<typename S,Dist J,Dist K> friend class DistMatrix;
#endif
};

template<typename T>
void AssertConforming1x2
( const AbstractDistMatrix<T>& AL, const AbstractDistMatrix<T>& AR );

template<typename T>
void AssertConforming2x1
( const AbstractDistMatrix<T>& AT, const AbstractDistMatrix<T>& AB );

template<typename T>
void AssertConforming2x2
( const AbstractDistMatrix<T>& ATL, const AbstractDistMatrix<T>& ATR,
  const AbstractDistMatrix<T>& ABL, const AbstractDistMatrix<T>& ABR );

} // namespace elem

#endif // ifndef ELEM_DISTMATRIX_ABSTRACT_DECL_HPP
