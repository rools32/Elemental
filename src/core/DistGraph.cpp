/*
   Copyright (c) 2009-2014, Jack Poulson, Lexing Ying,
   The University of Texas at Austin, Stanford University, and the
   Georgia Insitute of Technology.
   All rights reserved.
 
   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#include "El.hpp"

namespace El {

// Constructors and destructors
// ============================

DistGraph::DistGraph()
: numSources_(0), numTargets_(0), comm_(mpi::COMM_WORLD), consistent_(true)
{ SetComm( mpi::COMM_WORLD ); }

DistGraph::DistGraph( mpi::Comm comm )
: numSources_(0), numTargets_(0), comm_(mpi::COMM_WORLD), consistent_(true)
{ SetComm( comm ); }

DistGraph::DistGraph( int numVertices, mpi::Comm comm )
: numSources_(numVertices), numTargets_(numVertices), comm_(mpi::COMM_WORLD),
  consistent_(true)
{ SetComm( comm ); }

DistGraph::DistGraph( int numSources, int numTargets, mpi::Comm comm )
: numSources_(numSources), numTargets_(numTargets), comm_(mpi::COMM_WORLD),
  consistent_(true)
{ SetComm( comm ); }

DistGraph::DistGraph( const Graph& graph )
{
    DEBUG_ONLY(CallStackEntry cse("DistGraph::DistGraph"))
    *this = graph;
}

DistGraph::DistGraph( const DistGraph& graph )
: comm_(mpi::COMM_WORLD)
{
    DEBUG_ONLY(CallStackEntry cse("DistGraph::DistGraph"))
    if( &graph != this )
        *this = graph;
    else
        LogicError("Tried to construct DistGraph with itself");
}

DistGraph::~DistGraph()
{ 
    if( comm_ != mpi::COMM_WORLD )
        mpi::Free( comm_ );
} 

// Assignment and reconfiguration
// ==============================

// Make a copy
// -----------
const DistGraph& DistGraph::operator=( const Graph& graph )
{
    DEBUG_ONLY(CallStackEntry cse("DistGraph::operator="))
    numSources_ = graph.numSources_; 
    numTargets_ = graph.numTargets_;

    SetComm( mpi::COMM_SELF );

    sources_ = graph.sources_;
    targets_ = graph.targets_;

    consistent_ = graph.consistent_;
    localEdgeOffsets_ = graph.edgeOffsets_;
    return *this;
}

const DistGraph& DistGraph::operator=( const DistGraph& graph )
{
    DEBUG_ONLY(CallStackEntry cse("DistGraph::operator="))
    numSources_ = graph.numSources_;
    numTargets_ = graph.numTargets_;

    SetComm( graph.comm_ );

    sources_ = graph.sources_;
    targets_ = graph.targets_;

    consistent_ = graph.consistent_;
    localEdgeOffsets_ = graph.localEdgeOffsets_;
    return *this;
}

// Change the graph size
// ---------------------
void DistGraph::Empty()
{
    numSources_ = 0;
    numTargets_ = 0;
    firstLocalSource_ = 0;
    numLocalSources_ = 0;
    blocksize_ = 0;
    SwapClear( sources_ );
    SwapClear( targets_ );
    SwapClear( localEdgeOffsets_ );
    consistent_ = true;
}

void DistGraph::Resize( Int numVertices ) 
{ Resize( numVertices, numVertices ); }

void DistGraph::Resize( Int numSources, Int numTargets )
{
    const int commRank = mpi::Rank( comm_ );
    const int commSize = mpi::Size( comm_ );
    numSources_ = numSources;
    numTargets_ = numTargets;
    blocksize_ = numSources/commSize;
    firstLocalSource_ = commRank*blocksize_;
    if( commRank < commSize-1 )
        numLocalSources_ = blocksize_;
    else
        numLocalSources_ = numSources - (commSize-1)*blocksize_;
    SwapClear( sources_ );
    SwapClear( targets_ );
    SwapClear( localEdgeOffsets_ );
    consistent_ = true;
}

// Change the distribution
// -----------------------
void DistGraph::SetComm( mpi::Comm comm )
{
    if( comm_ != mpi::COMM_WORLD )
        mpi::Free( comm_ );

    SwapClear( sources_ );
    SwapClear( targets_ );
    SwapClear( localEdgeOffsets_ );
    consistent_ = true;

    if( comm == mpi::COMM_WORLD )
        comm_ = comm;
    else
        mpi::Dup( comm, comm_ );

    const int commRank = mpi::Rank( comm );
    const int commSize = mpi::Size( comm );
    blocksize_ = numSources_/commSize;
    firstLocalSource_ = commRank*blocksize_;
    if( commRank < commSize-1 )
        numLocalSources_ = blocksize_;
    else
        numLocalSources_ = numSources_ - (commSize-1)*blocksize_;
}

// Assembly
// --------
void DistGraph::Reserve( Int numLocalEdges )
{ 
    sources_.reserve( numLocalEdges );
    targets_.reserve( numLocalEdges );
}

void DistGraph::Connect( Int source, Int target )
{
    DEBUG_ONLY(CallStackEntry cse("DistGraph::Connect"))
    QueueConnection( source, target );
    MakeConsistent();
}

void DistGraph::ConnectLocal( Int localSource, Int target )
{
    DEBUG_ONLY(CallStackEntry cse("DistGraph::ConnectLocal"))
    QueueLocalConnection( localSource, target );
    MakeConsistent();
}

void DistGraph::Disconnect( Int source, Int target )
{
    DEBUG_ONLY(CallStackEntry cse("DistGraph::Disconnect"))
    QueueDisconnection( source, target );
    MakeConsistent();
}

void DistGraph::DisconnectLocal( Int localSource, Int target )
{
    DEBUG_ONLY(CallStackEntry cse("DistGraph::DisconnectLocal"))
    QueueLocalDisconnection( localSource, target );
    MakeConsistent();
}

void DistGraph::QueueConnection( Int source, Int target )
{
    DEBUG_ONLY(CallStackEntry cse("DistGraph::QueueConnection"))
    if( source < firstLocalSource_ || 
        source >= firstLocalSource_+numLocalSources_ )
        QueueLocalConnection( source-firstLocalSource_, target );
}

void DistGraph::QueueLocalConnection( Int localSource, Int target )
{
    DEBUG_ONLY(
      CallStackEntry cse("DistGraph::QueueLocalConnection");
      const Int capacity = Capacity();
      const Int numLocalEdges = NumLocalEdges();
      if( numLocalEdges == capacity )
          std::cerr << "WARNING: Pushing back without first reserving space" 
                    << std::endl;
    )
    if( localSource < 0 || localSource >= numLocalSources_ )
        LogicError
        ("Local source was out of bounds: ",localSource," is not in [0,",
         numLocalSources_,")");
    if( target < 0 || target >= numTargets_ )
        LogicError
        ("Target was out of bounds: ",target," is not in [0,",numTargets_,")");
    sources_.push_back( firstLocalSource_+localSource );
    targets_.push_back( target );
    consistent_ = false;
}

void DistGraph::QueueDisconnection( Int source, Int target )
{
    DEBUG_ONLY(CallStackEntry cse("DistGraph::QueueDisconnection"))
    if( source < firstLocalSource_ || 
        source >= firstLocalSource_+numLocalSources_ )
        QueueLocalDisconnection( source-firstLocalSource_, target );
}

void DistGraph::QueueLocalDisconnection( Int localSource, Int target )
{
    DEBUG_ONLY(CallStackEntry cse("DistGraph::QueueLocalDisconnection"))
    if( localSource < 0 || localSource >= numLocalSources_ )
        LogicError
        ("Local source was out of bounds: ",localSource," is not in [0,",
         numLocalSources_,")");
    if( target < 0 || target >= numTargets_ )
        LogicError
        ("Target was out of bounds: ",target," is not in [0,",numTargets_,")");
    markedForRemoval_.insert
    ( std::pair<Int,Int>(firstLocalSource_+localSource,target) );
    consistent_ = false;
}

void DistGraph::MakeConsistent()
{
    DEBUG_ONLY(
        CallStackEntry cse("DistGraph::MakeConsistent");
        if( sources_.size() != targets_.size() )
            LogicError("Inconsistent graph buffer sizes");
    )
    if( !consistent_ )
    {
        const Int numLocalEdges = sources_.size();
        Int numRemoved = 0;
        std::vector<std::pair<Int,Int>> pairs( numLocalEdges );
        for( Int e=0; e<numLocalEdges; ++e )
        {
            std::pair<Int,Int> candidate(sources_[e],targets_[e]);
            if( markedForRemoval_.find(candidate) == markedForRemoval_.end() )
            {
                pairs[e-numRemoved].first = sources_[e];
                pairs[e-numRemoved].second = targets_[e];
            }
            else
            {
                ++numRemoved;
            }
        }
        markedForRemoval_.clear();
        pairs.resize( numLocalEdges-numRemoved );
        std::sort( pairs.begin(), pairs.end(), ComparePairs );

        // Compress out duplicates
        Int lastUnique=0;
        for( Int e=1; e<numLocalEdges; ++e )
        {
            if( pairs[e] != pairs[lastUnique] )
            {
                ++lastUnique;
                if( e != lastUnique )
                    pairs[lastUnique] = pairs[e];
            }
        }
        const Int numUnique = lastUnique+1;
        pairs.resize( numUnique );

        sources_.resize( numUnique );
        targets_.resize( numUnique );
        for( Int e=0; e<numUnique; ++e )
        {
            sources_[e] = pairs[e].first;
            targets_[e] = pairs[e].second;
        }

        ComputeEdgeOffsets();

        consistent_ = true;
    }
}

// Basic queries
// =============

// High-level information
// ----------------------
Int DistGraph::NumSources() const { return numSources_; }
Int DistGraph::NumTargets() const { return numTargets_; }
Int DistGraph::FirstLocalSource() const { return firstLocalSource_; }
Int DistGraph::NumLocalSources() const { return numLocalSources_; }

Int DistGraph::NumLocalEdges() const
{
    DEBUG_ONLY(CallStackEntry cse("DistGraph::NumLocalEdges"))
    return sources_.size();
}

Int DistGraph::Capacity() const
{
    DEBUG_ONLY(CallStackEntry cse("DistGraph::Capacity"))
    return std::min(sources_.capacity(),targets_.capacity());
}

bool DistGraph::Consistent() const { return consistent_; }

// Distribution information
// ------------------------
mpi::Comm DistGraph::Comm() const { return comm_; }
Int DistGraph::Blocksize() const { return blocksize_; }

// Detailed local information
// --------------------------
Int DistGraph::Source( Int localEdge ) const
{
    DEBUG_ONLY(
      CallStackEntry cse("DistGraph::Source");
      if( localEdge < 0 || localEdge >= (Int)sources_.size() )
          LogicError("Edge number out of bounds");
    )
    return sources_[localEdge];
}

Int DistGraph::Target( Int localEdge ) const
{
    DEBUG_ONLY(
      CallStackEntry cse("DistGraph::Target");
      if( localEdge < 0 || localEdge >= (Int)targets_.size() )
          LogicError("Edge number out of bounds");
    )
    return targets_[localEdge];
}

Int DistGraph::EdgeOffset( Int localSource ) const
{
    DEBUG_ONLY(
      CallStackEntry cse("DistGraph::EdgeOffset");
      if( localSource < 0 || localSource > numLocalSources_ )
          LogicError
          ("Out of bounds localSource: ",localSource,
           " is not in [0,",numLocalSources_,")");
      AssertConsistent();
    )
    return localEdgeOffsets_[localSource];
}

Int DistGraph::NumConnections( Int localSource ) const
{
    DEBUG_ONLY(
      CallStackEntry cse("DistGraph::NumConnections");
      AssertConsistent();
    )
    return EdgeOffset(localSource+1) - EdgeOffset(localSource);
}

Int* DistGraph::SourceBuffer() { return sources_.data(); }
Int* DistGraph::TargetBuffer() { return targets_.data(); }

const Int* DistGraph::LockedSourceBuffer() const { return sources_.data(); }
const Int* DistGraph::LockedTargetBuffer() const { return targets_.data(); }

// Auxiliary routines
// ==================

bool DistGraph::ComparePairs
( const std::pair<Int,Int>& a, const std::pair<Int,Int>& b )
{ return a.first < b.first || (a.first == b.first && a.second < b.second); }

void DistGraph::ComputeEdgeOffsets()
{
    DEBUG_ONLY(CallStackEntry cse("DistGraph::ComputeEdgeOffsets"))
    // Compute the local edge offsets
    Int sourceOffset = 0;
    Int prevSource = firstLocalSource_-1;
    localEdgeOffsets_.resize( numLocalSources_+1 );
    const Int numLocalEdges = NumLocalEdges();
    for( Int localEdge=0; localEdge<numLocalEdges; ++localEdge )
    {
        const Int source = Source( localEdge );
        DEBUG_ONLY(
          if( source < prevSource )
              RuntimeError("sources were not properly sorted");
        )
        while( source != prevSource )
        {
            localEdgeOffsets_[sourceOffset++] = localEdge;
            ++prevSource;
        }
    }
    localEdgeOffsets_[numLocalSources_] = numLocalEdges;
}

void DistGraph::AssertConsistent() const
{
    if( !consistent_ )
        LogicError("DistGraph was not consistent");
}

} // namespace El
