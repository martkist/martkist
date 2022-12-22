// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Copyright (c) 2014-2023 The Dash Core developers
// Copyright (c) 2014-2023 The Martkist Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef MARTKIST_SCRIPT_SIGCACHE_H
#define MARTKIST_SCRIPT_SIGCACHE_H

#include "script/interpreter.h"

#include <vector>

// DoS prevention: limit cache size to 192MB
static const unsigned int DEFAULT_MAX_SIG_CACHE_SIZE = 192;
// Maximum sig cache size allowed
static const int64_t MAX_MAX_SIG_CACHE_SIZE = (DEFAULT_MAX_SIG_CACHE_SIZE/2) * 1024;

class CPubKey;

/**
* We're hashing a nonce into the entries themselves, so we don't need extra
* blinding in the set hash computation.
*
* This may exhibit platform endian dependent behavior but because these are
* nonced hashes (random) and this state is only ever used locally it is safe.
* All that matters is local consistency.
*/
class SignatureCacheHasher
{
public:
	template <uint8_t hash_select>
	uint32_t operator()(const uint256& key) const
	{
		static_assert(hash_select <8, "SignatureCacheHasher only has 8 hashes available.");
		uint32_t u;
		std::memcpy(&u, key.begin() + 4 * hash_select, 4);
		return u;
	}
};

class CachingTransactionSignatureChecker : public TransactionSignatureChecker
{
private:
	bool store;

public:
	CachingTransactionSignatureChecker(const CTransaction* txToIn, unsigned int nInIn, bool storeIn) : TransactionSignatureChecker(txToIn, nInIn), store(storeIn) {}

	bool VerifySignature(const std::vector<unsigned char>& vchSig, const CPubKey& vchPubKey, const uint256& sighash) const override;
};

void InitSignatureCache();

#endif // MARTKIST_SCRIPT_SIGCACHE_H
