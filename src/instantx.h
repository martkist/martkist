// Copyright (c) 2014-2023 The Dash Core developers
// Copyright (c) 2017-2018 The Martkist Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef INSTANTX_H
#define INSTANTX_H

#include "chain.h"
#include "net.h"
#include "primitives/transaction.h"

class CTxLockVote;
class COutPointLock;
class CTxLockRequest;
class CTxLockCandidate;
class CInstantSend;

extern CInstantSend instantsend;

/*
    At 15 signatures, 1/2 of the masternode network can be owned by
    one party without compromising the security of InstantSend
    (1000/2150.0)**10 = 0.00047382219560689856
    (1000/2900.0)**10 = 2.3769498616783657e-05

    ### getting 5 of 10 signatures w/ 1000 nodes of 2900
    (1000/2900.0)**5 = 0.004875397277841433
*/

static const int MIN_INSTANTSEND_PROTO_VERSION      = MIN_PEER_PROTO_VERSION;

// For how long we are going to accept votes/locks
// after we saw the first one for a specific transaction
static const int INSTANTSEND_LOCK_TIMEOUT_SECONDS   = 15;
// For how long we are going to keep invalid votes and votes for failed lock attempts,
// must be greater than INSTANTSEND_LOCK_TIMEOUT_SECONDS
static const int INSTANTSEND_FAILED_TIMEOUT_SECONDS = 60;

extern bool fEnableInstantSend;
extern int nInstantSendDepth;
extern int nCompleteTXLocks;

class CInstantSend
{
private:
    static const std::string SERIALIZATION_VERSION_STRING;

    // Keep track of current block height
    int nCachedBlockHeight;

    // maps for AlreadyHave
    std::map<uint256, CTxLockRequest> mapLockRequestAccepted; // tx hash - tx
    std::map<uint256, CTxLockRequest> mapLockRequestRejected; // tx hash - tx
    std::map<uint256, CTxLockVote> mapTxLockVotes; // vote hash - vote
    std::map<uint256, CTxLockVote> mapTxLockVotesOrphan; // vote hash - vote

    std::map<uint256, CTxLockCandidate> mapTxLockCandidates; // tx hash - lock candidate

    std::map<COutPoint, std::set<uint256> > mapVotedOutpoints; // utxo - tx hash set
    std::map<COutPoint, uint256> mapLockedOutpoints; // utxo - tx hash

    //track masternodes who voted with no txreq (for DOS protection)
    std::map<COutPoint, int64_t> mapMasternodeOrphanVotes; // mn outpoint - time

    bool CreateTxLockCandidate(const CTxLockRequest& txLockRequest);
    void CreateEmptyTxLockCandidate(const uint256& txHash);
    void Vote(CTxLockCandidate& txLockCandidate, CConnman& connman);

    //process consensus vote message
    bool ProcessNewTxLockVote(CNode* pfrom, const CTxLockVote& vote, CConnman& connman);

    void UpdateVotedOutpoints(const CTxLockVote& vote, CTxLockCandidate& txLockCandidate);
    bool ProcessOrphanTxLockVote(const CTxLockVote& vote);
    void ProcessOrphanTxLockVotes();
    int64_t GetAverageMasternodeOrphanVoteTime();

    void TryToFinalizeLockCandidate(const CTxLockCandidate& txLockCandidate);
    void LockTransactionInputs(const CTxLockCandidate& txLockCandidate);
    //update UI and notify external script if any
    void UpdateLockedTransaction(const CTxLockCandidate& txLockCandidate);
    bool ResolveConflicts(const CTxLockCandidate& txLockCandidate);

    bool IsInstantSendReadyToLock(const uint256 &txHash);

public:
    mutable CCriticalSection cs_instantsend;

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        std::string strVersion;
        if(ser_action.ForRead()) {
            READWRITE(strVersion);
        }
        else {
            strVersion = SERIALIZATION_VERSION_STRING;
            READWRITE(strVersion);
        }

        READWRITE(mapLockRequestAccepted);
        READWRITE(mapLockRequestRejected);
        READWRITE(mapTxLockVotes);
        READWRITE(mapTxLockVotesOrphan);
        READWRITE(mapTxLockCandidates);
        READWRITE(mapVotedOutpoints);
        READWRITE(mapLockedOutpoints);
        READWRITE(mapMasternodeOrphanVotes);
        READWRITE(nCachedBlockHeight);

        if(ser_action.ForRead() && (strVersion != SERIALIZATION_VERSION_STRING)) {
            Clear();
        }
    }

    void Clear();

    void ProcessMessage(CNode* pfrom, const std::string& strCommand, CDataStream& vRecv, CConnman& connman);

    bool ProcessTxLockRequest(const CTxLockRequest& txLockRequest, CConnman& connman);
    void Vote(const uint256& txHash, CConnman& connman);

    bool AlreadyHave(const uint256& hash);

    void AcceptLockRequest(const CTxLockRequest& txLockRequest);
    void RejectLockRequest(const CTxLockRequest& txLockRequest);
    bool HasTxLockRequest(const uint256& txHash);
    bool GetTxLockRequest(const uint256& txHash, CTxLockRequest& txLockRequestRet);

    bool GetTxLockVote(const uint256& hash, CTxLockVote& txLockVoteRet);

    bool GetLockedOutPointTxHash(const COutPoint& outpoint, uint256& hashRet);

    // verify if transaction is currently locked
    bool IsLockedInstantSendTransaction(const uint256& txHash);
    // get the actual number of accepted lock signatures
    int GetTransactionLockSignatures(const uint256& txHash);

    // remove expired entries from maps
    void CheckAndRemove();
    // verify if transaction lock timed out
    bool IsTxLockCandidateTimedOut(const uint256& txHash);

    void Relay(const uint256& txHash, CConnman& connman);

    void UpdatedBlockTip(const CBlockIndex *pindex);
    void SyncTransaction(const CTransaction& tx, const CBlockIndex *pindex, int posInBlock);

    std::string ToString() const;

    void DoMaintenance() { CheckAndRemove(); }
};

class CTxLockRequest
{

public:
	static const CAmount MIN_FEE = 0.001 * COIN;
    static const int WARN_MANY_INPUTS       = 100;

    CTransactionRef tx;

    CTxLockRequest() : tx(MakeTransactionRef()) {}
    CTxLockRequest(const CTransaction& _tx) : tx(MakeTransactionRef(_tx)) {};

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(tx);
    }

    bool IsValid() const;
    CAmount GetMinFee() const;
    int GetMaxSignatures() const;

    const uint256 &GetHash() const {
        return tx->GetHash();
    }

    std::string ToString() const {
        return tx->ToString();
    }

    friend bool operator==(const CTxLockRequest& a, const CTxLockRequest& b)
    {
        return *a.tx == *b.tx;
    }

    friend bool operator!=(const CTxLockRequest& a, const CTxLockRequest& b)
    {
        return *a.tx != *b.tx;
    }

    explicit operator bool() const
    {
        return *this != CTxLockRequest();
    }
};

class CTxLockVote
{
private:
    uint256 txHash;
    COutPoint outpoint;
    COutPoint outpointMasternode;
    std::vector<unsigned char> vchMasternodeSignature;
    // local memory only
    int nConfirmedHeight; // when corresponding tx is 0-confirmed or conflicted, nConfirmedHeight is -1
    int64_t nTimeCreated;

public:
    CTxLockVote() :
        txHash(),
        outpoint(),
        outpointMasternode(),
        vchMasternodeSignature(),
        nConfirmedHeight(-1),
        nTimeCreated(GetTime())
        {}

    CTxLockVote(const uint256& txHashIn, const COutPoint& outpointIn, const COutPoint& outpointMasternodeIn) :
        txHash(txHashIn),
        outpoint(outpointIn),
        outpointMasternode(outpointMasternodeIn),
        vchMasternodeSignature(),
        nConfirmedHeight(-1),
        nTimeCreated(GetTime())
        {}

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(txHash);
        READWRITE(outpoint);
        READWRITE(outpointMasternode);
        if (!(s.GetType() & SER_GETHASH)) {
            READWRITE(vchMasternodeSignature);
        }
    }

    uint256 GetHash() const;
    uint256 GetSignatureHash() const;

    uint256 GetTxHash() const { return txHash; }
    COutPoint GetOutpoint() const { return outpoint; }
    COutPoint GetMasternodeOutpoint() const { return outpointMasternode; }

    bool IsValid(CNode* pnode, CConnman& connman) const;
    void SetConfirmedHeight(int nConfirmedHeightIn) { nConfirmedHeight = nConfirmedHeightIn; }
    bool IsExpired(int nHeight) const;
    bool IsTimedOut() const;
    bool IsFailed() const;

    bool Sign();
    bool CheckSignature() const;

    void Relay(CConnman& connman) const;
};

class COutPointLock
{
private:
    COutPoint outpoint; // utxo
    std::map<COutPoint, CTxLockVote> mapMasternodeVotes; // masternode outpoint - vote
    bool fAttacked = false;

public:
    static const int SIGNATURES_REQUIRED        = 6;
    static const int SIGNATURES_TOTAL           = 10;

    COutPointLock() {}

    COutPointLock(const COutPoint& outpointIn) :
        outpoint(outpointIn),
        mapMasternodeVotes()
        {}

    COutPoint GetOutpoint() const { return outpoint; }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(outpoint);
        READWRITE(mapMasternodeVotes);
        READWRITE(fAttacked);
    }

    bool AddVote(const CTxLockVote& vote);
    std::vector<CTxLockVote> GetVotes() const;
    bool HasMasternodeVoted(const COutPoint& outpointMasternodeIn) const;
    int CountVotes() const { return fAttacked ? 0 : mapMasternodeVotes.size(); }
    bool IsReady() const { return !fAttacked && CountVotes() >= SIGNATURES_REQUIRED; }
    void MarkAsAttacked() { fAttacked = true; }

    void Relay(CConnman& connman) const;
};

class CTxLockCandidate
{
private:
    int nConfirmedHeight; // when corresponding tx is 0-confirmed or conflicted, nConfirmedHeight is -1
    int64_t nTimeCreated;

public:
    CTxLockCandidate() :
        nConfirmedHeight(-1),
        nTimeCreated(GetTime())
    {}

    CTxLockCandidate(const CTxLockRequest& txLockRequestIn) :
        nConfirmedHeight(-1),
        nTimeCreated(GetTime()),
        txLockRequest(txLockRequestIn),
        mapOutPointLocks()
        {}

    CTxLockRequest txLockRequest;
    std::map<COutPoint, COutPointLock> mapOutPointLocks;

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(txLockRequest);
        READWRITE(mapOutPointLocks);
        READWRITE(nTimeCreated);
        READWRITE(nConfirmedHeight);
    }

    uint256 GetHash() const { return txLockRequest.GetHash(); }

    void AddOutPointLock(const COutPoint& outpoint);
    void MarkOutpointAsAttacked(const COutPoint& outpoint);
    bool AddVote(const CTxLockVote& vote);
    bool IsAllOutPointsReady() const;

    bool HasMasternodeVoted(const COutPoint& outpointIn, const COutPoint& outpointMasternodeIn);
    int CountVotes() const;

    void SetConfirmedHeight(int nConfirmedHeightIn) { nConfirmedHeight = nConfirmedHeightIn; }
    bool IsExpired(int nHeight) const;
    bool IsTimedOut() const;

    void Relay(CConnman& connman) const;
};

#endif
