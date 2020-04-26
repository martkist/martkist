// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Copyright (c) 2014-2020 The Dash Core developers
// Copyright (c) 2014-2020 The Martkist Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "consensus/merkle.h"

#include "tinyformat.h"
#include "util.h"
#include "utilstrencodings.h"

#include "arith_uint256.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

#include "chainparamsseeds.h"
// MARTKIST includes for gen block
#include "uint256.h"
#include "arith_uint256.h"
#include "hash.h"
#include "streams.h"
#include <time.h>
// MARTKIST generate block
/**
* Main network
*/
/**
* What makes a good checkpoint block?
* + Is surrounded by blocks with reasonable timestamps
*   (no blocks before with a timestamp after, none after with
*    timestamp before)
* + Contains no strange transactions
*/

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

static CBlock CreateDevNetGenesisBlock(const uint256 &prevBlockHash, const std::string& devNetName, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    assert(!devNetName.empty());

    CMutableTransaction txNew;
    txNew.nVersion = 4;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    // put height (BIP34) and devnet name into coinbase
    txNew.vin[0].scriptSig = CScript() << 1 << std::vector<unsigned char>(devNetName.begin(), devNetName.end());
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = CScript() << OP_RETURN;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock = prevBlockHash;
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=00000ffd590b14, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=e0028e, nTime=1390095618, nBits=1e0ffff0, nNonce=28917698, vtx=1)
 *   CTransaction(hash=e0028e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d01044c5957697265642030392f4a616e2f3230313420546865204772616e64204578706572696d656e7420476f6573204c6976653a204f76657273746f636b2e636f6d204973204e6f7720416363657074696e6720426974636f696e73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0xA9037BAC7050C479B121CF)
 *   vMerkleTree: e0028e
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "Op Ed: With Martkist, Anarchy Is the Point, Not the Problem";
    const CScript genesisOutputScript = CScript() << ParseHex("04f942f26515622b010eb91e46cd01f95fe06e15f51bf543364e47058d8760e672755be191a472aa02f3eb254e49f88ea012eadd6fa8e6c8bd384604872b8dfbc6") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}
// This will figure out a valid hash and Nonce if you're
// creating a different genesis block:
static void GenerateGenesisBlock(CBlockHeader &genesisBlock, uint256 &phash)
{
	arith_uint256 bnTarget;
	bnTarget.SetCompact(genesisBlock.nBits);
	uint32_t nOnce = 0;
	while (true) {
		genesisBlock.nNonce = nOnce;
		uint256 hash = genesisBlock.GetHash();
		if (UintToArith256(hash) <= bnTarget) {
			phash = hash;
			break;
		}
		nOnce++;
	}
	printf("genesis.nTime = %u \n", genesisBlock.nTime);
	printf("genesis.nNonce = %u \n", genesisBlock.nNonce);
	printf("Generate hash = %s\n", phash.ToString().c_str());
	printf("genesis.hashMerkleRoot = %s\n", genesisBlock.hashMerkleRoot.ToString().c_str());
}

static CBlock FindDevNetGenesisBlock(const Consensus::Params& params, const CBlock &prevBlock, const CAmount& reward)
{
    std::string devNetName = GetDevNetName();
    assert(!devNetName.empty());

    CBlock block = CreateDevNetGenesisBlock(prevBlock.GetHash(), devNetName.c_str(), prevBlock.nTime + 1, 0, prevBlock.nBits, prevBlock.nVersion, reward);

    arith_uint256 bnTarget;
    bnTarget.SetCompact(block.nBits);

    for (uint32_t nNonce = 0; nNonce < UINT32_MAX; nNonce++) {
        block.nNonce = nNonce;

        uint256 hash = block.GetHash();
        if (UintToArith256(hash) <= bnTarget)
            return block;
    }

    // This is very unlikely to happen as we start the devnet with a very low difficulty. In many cases even the first
    // iteration of the above loop will give a result already
    error("FindDevNetGenesisBlock: could not find devnet genesis block for %s", devNetName);
    assert(false);
}

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */


class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        consensus.nSubsidyHalvingInterval = 0; // not used anymore
		consensus.nSeniorityInterval = 0; // not used anymore
		consensus.nTotalSeniorityIntervals = 0; // not used anymore
        consensus.nMasternodePaymentsStartBlock = 2;
        consensus.nMasternodePaymentsIncreaseBlock = 2; // actual historical value
        consensus.nMasternodePaymentsIncreasePeriod = 576*30; // 17280 - actual historical value
        consensus.nInstantSendConfirmationsRequired = 6;
        consensus.nInstantSendKeepLock = 24;
		consensus.nShareFeeBlock = 0;
        consensus.nBudgetPaymentsStartBlock = 0; // actual historical value
        consensus.nBudgetPaymentsCycleBlocks = 43800; // ~(60*24*30)/2.6, actual number of blocks per month is 200700 / 12 = 16725
        consensus.nBudgetPaymentsWindowBlocks = 100;
        consensus.nSuperblockStartBlock = 0;
		consensus.nSuperblockStartHash = uint256();
        consensus.nSuperblockCycle = 49000;
        consensus.nGovernanceMinQuorum = 33;
        consensus.nGovernanceFilterElements = 20000;
        consensus.nMasternodeMinimumConfirmations = 13;
        consensus.BIP34Height = 0;
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 0; // 00000000000076d8fcea02ec0963de4abfd01e771fec0863f960c2c64fe6f357
        consensus.BIP66Height = 0; // 00000000000b1fa2dfa312863570e13fae9ca7b5566cb27e55422620b469aefa
        consensus.DIP0001Height = 0;
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 20
        consensus.nPowTargetTimespan = 180;// 00:03:00
        consensus.nPowTargetSpacing = 60;// 00:01:00
		consensus.nAuxpowChainId = 0xfff;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nPowKGWHeight = 15200;
        consensus.nPowDGWHeight = 0;
        consensus.nRuleChangeActivationThreshold = 1916; // 95% of 2016
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
		consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
		consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
		consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 0; // Feb 5th, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 999999999999ULL; // Feb 5th, 2018

        // Deployment of DIP0001
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nStartTime = 0; // Oct 15th, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nTimeout = 999999999999ULL; // Oct 15th, 2018

        // Deployment of BIP147
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nStartTime = 0; // Apr 23th, 2018
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nTimeout = 999999999999ULL; // Apr 23th, 2019


        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00"); // 307300

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
		pchMessageStart[0] = 0xf9;
		pchMessageStart[1] = 0xbe;
		pchMessageStart[2] = 0xb4;
		pchMessageStart[3] = 0xd9;
        vAlertPubKey = ParseHex("04412228975c883f72ec180073f33d1d5d46296f5af16d0628393124931078fe1f472380b0eadcd10bdafae59c3265659a04339a406c347d98f0c78cfe9a9a4444");
		nDefaultPort = 4040;
		nPruneAfterHeight = 1000000;
		uint256 hash;
		genesis = CreateGenesisBlock(1582711200, 1356960, 0x1e0fffff, 1, 1 * COIN);
		/*CBlockHeader genesisHeader = genesis.GetBlockHeader();
		GenerateGenesisBlock(genesisHeader, &hash);*/

		consensus.hashGenesisBlock = genesis.GetHash();
		assert(consensus.hashGenesisBlock == uint256S("0x00000a7f5622e1499ea8649f89c033973b9d5f4b96715ba21858af6234c7cc32"));
		assert(genesis.hashMerkleRoot == uint256S("0x9342d40305d508194d9fce5faab7d82cd6c9fea01fedc4b37557cb8b34a127c4"));

		vSeeds.push_back(CDNSSeedData("seed1.martkist.org", "seed1.martkist.org"));
		vSeeds.push_back(CDNSSeedData("seed2.martkist.org", "seed2.martkist.org"));
		vSeeds.push_back(CDNSSeedData("seed3.martkist.org", "seed3.martkist.org"));
		vSeeds.push_back(CDNSSeedData("seed4.martkist.org", "seed4.martkist.org"));



		base58Prefixes[PUBKEY_ADDRESS_MARTK] = std::vector<unsigned char>(1, 50);
		base58Prefixes[SCRIPT_ADDRESS_MARTK] = std::vector<unsigned char>(1, 63);
		base58Prefixes[SECRET_KEY_MARTK] = std::vector<unsigned char>(1, 45);

		base58Prefixes[PUBKEY_ADDRESS_BTC] = std::vector<unsigned char>(1, 0);
		base58Prefixes[SCRIPT_ADDRESS_BTC] = std::vector<unsigned char>(1, 5);
		base58Prefixes[SECRET_KEY_BTC] = std::vector<unsigned char>(1, 128);

		base58Prefixes[PUBKEY_ADDRESS_ZEC] = { 0x1C,0xB8 };
		base58Prefixes[SCRIPT_ADDRESS_ZEC] = { 0x1C,0xBD };
		base58Prefixes[SECRET_KEY_ZEC] = std::vector<unsigned char>(1, 128);
		// Martkist BIP32 pubkeys
		base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xb2)(0x1e).convert_to_container<std::vector<unsigned char> >();
		// Martkist BIP32 prvkeys
		base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xad)(0xe4).convert_to_container<std::vector<unsigned char> >();

        // Martkist BIP44 coin type is '0x800004d2'
        nExtCoinType = 0x800004d2;

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fAllowMultipleAddressesFromGroup = false;
        fAllowMultiplePorts = false;

        nPoolMaxTransactions = 3;
        nFulfilledRequestExpireTime = 5 * 60;// 5min

        strSporkAddress = "MVLzWjo2WfatRVBiZ3CrNPNUqVBs7FguU1";		

        checkpointData = {
            {
                {  1000, uint256S("0000000000000004f1046e1f388f9cab53bde759772f0b84c88465b60ea2f63d")},
                { 10000, uint256S("000000000000004f06428237f7e5e190df53e31c6232e0906bdb868ac9498f8d")},
                { 25000, uint256S("000000000000003a2aff9878fa2858e9f2960f3b768f15db28567c3e0fdcfe4a")},
                { 49000, uint256S("00000c9ec0f9d60ce297bf9f9cbe1f2eb39165a0d3f69c1c55fc3f6680fe45c8")}, // superblock 1
                { 50000, uint256S("00000000000000141073cebb246a9207d504a5c00b580907603ff870a19cbd3b")},
                { 75000, uint256S("00000000000000de94ba1fbf9a7fa5729d476a9912e81b34a314f390046b0c8a")}
            }
        };

		chainTxData = ChainTxData{
			// Data from rpc: getchaintxstats 4096 00000000000000de94ba1fbf9a7fa5729d476a9912e81b34a314f390046b0c8a
			/* nTime    */ 1587732478,
			/* nTxCount */ 4845,
			/* dTxRate  */ 0.01867757380436542
		};
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        consensus.nSubsidyHalvingInterval = 0; // not used anymore
		consensus.nSeniorityInterval = 0; // not used anymore
		consensus.nTotalSeniorityIntervals = 0; // not used anymore
        consensus.nMasternodePaymentsStartBlock = 2;
        consensus.nMasternodePaymentsIncreaseBlock = 2;
        consensus.nMasternodePaymentsIncreasePeriod = 10;
        consensus.nInstantSendConfirmationsRequired = 6;
        consensus.nInstantSendKeepLock = 6;
        consensus.nBudgetPaymentsStartBlock = 0;
        consensus.nBudgetPaymentsCycleBlocks = 50;
        consensus.nBudgetPaymentsWindowBlocks = 10;
		consensus.nShareFeeBlock = 0;
        consensus.nSuperblockStartBlock = 0;
        consensus.nSuperblockStartHash = uint256();
        consensus.nSuperblockCycle = 49000;
        consensus.nGovernanceMinQuorum = 1;
        consensus.nGovernanceFilterElements = 500;
        consensus.nMasternodeMinimumConfirmations = 1;
        consensus.BIP34Height = 0;
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 0; // 0000039cf01242c7f921dcb4806a5994bc003b48c1973ae0c89b67809c2bb2ab
        consensus.BIP66Height = 0; // 0000002acdd29a14583540cb72e1c5cc83783560e38fa7081495d474fe1671f7
        consensus.DIP0001Height = 5500;
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 20
		consensus.nPowTargetTimespan = 180;// 00:03:00
		consensus.nPowTargetSpacing = 60;// 00:01:00
		consensus.nAuxpowChainId = 0xfff;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nPowKGWHeight = 4001; // nPowKGWHeight >= nPowDGWHeight means "no KGW"
        consensus.nPowDGWHeight = 0;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 999999999999ULL; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 0; // September 28th, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 999999999999ULL; // September 28th, 2018

        // Deployment of DIP0001
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nStartTime = 0; // Sep 18th, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nTimeout = 999999999999ULL; // Sep 18th, 2018

        // Deployment of BIP147
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nStartTime = 0; // Feb 5th, 2018
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nTimeout = 999999999999ULL; // Feb 5th, 2019


        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        pchMessageStart[0] = 0xce;
        pchMessageStart[1] = 0xe2;
        pchMessageStart[2] = 0xca;
        pchMessageStart[3] = 0xff;
        vAlertPubKey = ParseHex("04fd1b9a4fefd0048ba3709e00c41888e62e0805f06470d236513b2fa53ba0ddbcac917b8d25355bb9d3040502f9c0de59afd07cda1632a3fc04c65889546c9c28"); // e91867736fe8087ac7d5ac0d9974b0340978eb8a1a7f0771b1d22682b68d0406
        nDefaultPort = 14040;
        nPruneAfterHeight = 1000;

		genesis = CreateGenesisBlock(1582711200, 1356960, 0x1e0fffff, 1, 1 * COIN);
		/*
		uint256 hash;
		CBlockHeader genesisHeader = genesis.GetBlockHeader();
		GenerateGenesisBlock(genesisHeader, &hash);*/
		consensus.hashGenesisBlock = genesis.GetHash();
		assert(consensus.hashGenesisBlock == uint256S("0x00000a7f5622e1499ea8649f89c033973b9d5f4b96715ba21858af6234c7cc32"));
		assert(genesis.hashMerkleRoot == uint256S("0x9342d40305d508194d9fce5faab7d82cd6c9fea01fedc4b37557cb8b34a127c4"));

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));
  		vSeeds.push_back(CDNSSeedData("seed1.test.martkist.org", "seed1.test.martkist.org"));
		vSeeds.push_back(CDNSSeedData("seed2.test.martkist.org", "seed2.test.martkist.org"));
		vSeeds.push_back(CDNSSeedData("seed3.test.martkist.org", "seed3.test.martkist.org"));
		vSeeds.push_back(CDNSSeedData("seed4.test.martkist.org", "seed4.test.martkist.org"));

		base58Prefixes[PUBKEY_ADDRESS_MARTK] = std::vector<unsigned char>(1, 65);
		base58Prefixes[SCRIPT_ADDRESS_MARTK] = std::vector<unsigned char>(1, 60);
		base58Prefixes[SECRET_KEY_MARTK] = std::vector<unsigned char>(1, 23);

		base58Prefixes[PUBKEY_ADDRESS_BTC] = std::vector<unsigned char>(1, 111);
		base58Prefixes[SCRIPT_ADDRESS_BTC] = std::vector<unsigned char>(1, 196);
		base58Prefixes[SECRET_KEY_BTC] = std::vector<unsigned char>(1, 239);

		base58Prefixes[PUBKEY_ADDRESS_ZEC] = std::vector<unsigned char>(0x1C, 0xB8);
		base58Prefixes[SCRIPT_ADDRESS_ZEC] = std::vector<unsigned char>(0x1C, 0xBD);
		base58Prefixes[SECRET_KEY_ZEC] = std::vector<unsigned char>(1, 239);

        // Testnet Martkist BIP44 coin type is '0x800004d3'
        nExtCoinType = 0x800004d3;


        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fAllowMultipleAddressesFromGroup = false;
        fAllowMultiplePorts = false;

        nPoolMaxTransactions = 3;
        nFulfilledRequestExpireTime = 5 * 60;// 5min

        strSporkAddress = "TL63qB18atqREkxAhxzvKVJUVa5MTfWuD4"; // 4Tn2CyZ7GABqT4DTDooN8SZuqRpoiBonRJY1Zf68WtUzUEwkZ9GJ

        checkpointData = {
            {
                {  1000, uint256S("000002999ad86a395eca7b720180479035b8008377a47fee5f0af825dabf1adc")},
                { 10000, uint256S("0000003ab0606045029160b93830230e497b651b3537c8b4358885924b2840f3")},
                { 25000, uint256S("0000006ad69ea5d3976e7c8d1b1664849b39ba58051150ae26ae24b998516242")},
                { 47500, uint256S("0000004ca32fd1e879da7ee908472e2d5cb6f8d7c66d14c8d9ce2c98bd2ade2a")}
            }
        };
        
		chainTxData = ChainTxData{
			// Data from rpc: getchaintxstats 4096 0000004ca32fd1e879da7ee908472e2d5cb6f8d7c66d14c8d9ce2c98bd2ade2a
			/* nTime    */ 1587918580,
			/* nTxCount */ 4096,
			/* dTxRate  */ 0.03110925454752592
		};
    }
};
static CTestNetParams testNetParams;

/**
 * Devnet
 */
class CDevNetParams : public CChainParams {
public:
    CDevNetParams() {
        strNetworkID = "dev";
        consensus.nSubsidyHalvingInterval = 0; // not used anymore
        consensus.nMasternodePaymentsStartBlock = 2;
        consensus.nMasternodePaymentsIncreaseBlock = 4030;
        consensus.nMasternodePaymentsIncreasePeriod = 10;
        consensus.nInstantSendConfirmationsRequired = 6;
        consensus.nInstantSendKeepLock = 6;
        consensus.nBudgetPaymentsStartBlock = 4100;
        consensus.nBudgetPaymentsCycleBlocks = 50;
        consensus.nBudgetPaymentsWindowBlocks = 10;
        consensus.nSuperblockStartBlock = 0;
        consensus.nSuperblockStartHash = uint256(); // do not check this on devnet
        consensus.nSuperblockCycle = 49000;
        consensus.nGovernanceMinQuorum = 1;
        consensus.nGovernanceFilterElements = 500;
        consensus.nMasternodeMinimumConfirmations = 1;
        consensus.BIP34Height = 2; // BIP34 activated immediately on devnet
        consensus.BIP65Height = 2; // BIP65 activated immediately on devnet
        consensus.BIP66Height = 2; // BIP66 activated immediately on devnet
        consensus.DIP0001Height = 2; // DIP0001 activated immediately on devnet
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 1
        consensus.nPowTargetTimespan = 180;// 00:03:00
        consensus.nPowTargetSpacing = 60;// 00:01:00
		consensus.nAuxpowChainId = 0xfff;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nPowKGWHeight = 4001; // nPowKGWHeight >= nPowDGWHeight means "no KGW"
        consensus.nPowDGWHeight = 0;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 999999999999ULL; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 0; // September 28th, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 999999999999ULL; // September 28th, 2018

        // Deployment of DIP0001
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nStartTime = 0; // Sep 18th, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nTimeout = 999999999999ULL; // Sep 18th, 2018

        // Deployment of BIP147
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nStartTime = 0; // Feb 5th, 2018
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nTimeout = 999999999999ULL; // Feb 5th, 2019

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x000000000000000000000000000000000000000000000000000000000000000");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        pchMessageStart[0] = 0xe2;
        pchMessageStart[1] = 0xca;
        pchMessageStart[2] = 0xff;
        pchMessageStart[3] = 0xce;
        vAlertPubKey = ParseHex("04de8e07e7906f94a3a5f47fac812badf0dd34b586d759d5ad4969e3de97f0595de783ed893922f8305ec25a346d300e7967372c56b74b3fe83c464a1fdc3bbaef"); // 3c464804a17d2dab8aeb57a8a8c72463ba73e884fe11590c194e03d105311035
        nDefaultPort = 24040;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1582711200, 1356960, 0x1e0fffff, 1, 1 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x00000a7f5622e1499ea8649f89c033973b9d5f4b96715ba21858af6234c7cc32"));
        assert(genesis.hashMerkleRoot == uint256S("0x9342d40305d508194d9fce5faab7d82cd6c9fea01fedc4b37557cb8b34a127c4"));

        devnetGenesis = FindDevNetGenesisBlock(consensus, genesis, 50 * COIN);
        consensus.hashDevnetGenesisBlock = devnetGenesis.GetHash();

        vFixedSeeds.clear();
        vSeeds.clear();
        

		base58Prefixes[PUBKEY_ADDRESS_MARTK] = std::vector<unsigned char>(1, 66);
		base58Prefixes[SCRIPT_ADDRESS_MARTK] = std::vector<unsigned char>(1, 61);
		base58Prefixes[SECRET_KEY_MARTK] = std::vector<unsigned char>(1, 24);

		base58Prefixes[PUBKEY_ADDRESS_BTC] = std::vector<unsigned char>(1, 111);
		base58Prefixes[SCRIPT_ADDRESS_BTC] = std::vector<unsigned char>(1, 196);
		base58Prefixes[SECRET_KEY_BTC] = std::vector<unsigned char>(1, 239);

		base58Prefixes[PUBKEY_ADDRESS_ZEC] = { 0x1C,0xB8 };
		base58Prefixes[SCRIPT_ADDRESS_ZEC] = { 0x1C,0xBD };
		base58Prefixes[SECRET_KEY_ZEC] = std::vector<unsigned char>(1, 239);
		// Regtest Martkist BIP32 pubkeys
		base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xcf).convert_to_container<std::vector<unsigned char> >();
		// Regtest Martkist BIP32 prvkeys
		base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        // Testnet Martkist BIP44 coin type is '0x800004d4'
        nExtCoinType = 0x800004d4;

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fAllowMultipleAddressesFromGroup = true;
        fAllowMultiplePorts = false;

        nPoolMaxTransactions = 3;
        nFulfilledRequestExpireTime = 5 * 60;// 5min

        strSporkAddress = "TvV2R8L9rnzVD5H8PKoJfCzHJFFmZuLcNv"; // 4gLGkvSyBQXh6Ubd465MB7RVwLBpLScC66AKh6XTFV2eCP5KyAdh

		

		chainTxData = ChainTxData{
			0,
			0,
			0
		};
    }
};
static CDevNetParams *devNetParams;


/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
		strNetworkID = "regtest";
		consensus.nSeniorityInterval = 0; // not used anymore
		consensus.nTotalSeniorityIntervals = 0; // not used anymore

        consensus.nSubsidyHalvingInterval = 0; // not used anymore
        consensus.nMasternodePaymentsStartBlock = 2;
        consensus.nMasternodePaymentsIncreaseBlock = 350;
        consensus.nMasternodePaymentsIncreasePeriod = 10;
        consensus.nInstantSendConfirmationsRequired = 6;
        consensus.nInstantSendKeepLock = 6;
        consensus.nBudgetPaymentsStartBlock = 0;
        consensus.nBudgetPaymentsCycleBlocks = 50;
        consensus.nBudgetPaymentsWindowBlocks = 10;
        consensus.nSuperblockStartBlock = 0;
        consensus.nSuperblockStartHash = uint256(); // do not check this on regtest
        consensus.nSuperblockCycle = 49000;
		consensus.nShareFeeBlock = 0;
        consensus.nGovernanceMinQuorum = 1;
        consensus.nGovernanceFilterElements = 100;
        consensus.nMasternodeMinimumConfirmations = 1;
        consensus.BIP34Height = 0; // BIP34 has not activated on regtest (far in the future so block v1 are not rejected in tests)
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 1351; // BIP65 activated on regtest (Used in rpc activation tests)
        consensus.BIP66Height = 1251; // BIP66 activated on regtest (Used in rpc activation tests)
        consensus.DIP0001Height = 2000;
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 1
        consensus.nPowTargetTimespan = 180;// 00:03:00
        consensus.nPowTargetSpacing = 60;// 00:01:00
		consensus.nAuxpowChainId = 0xfff;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nPowKGWHeight = 15200; // same as mainnet
        consensus.nPowDGWHeight = 0; // same as mainnet
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_DIP0001].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_BIP147].nTimeout = 999999999999ULL;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

		pchMessageStart[0] = 0xfa;
		pchMessageStart[1] = 0xbf;
		pchMessageStart[2] = 0xb5;
		pchMessageStart[3] = 0xda;
        nDefaultPort = 34040;
        nPruneAfterHeight = 1000;

		genesis = CreateGenesisBlock(1582711200, 1356960, 0x1e0fffff, 1, 1 * COIN);
		/*
		uint256 hash;
		CBlockHeader genesisHeader = genesis.GetBlockHeader();
		GenerateGenesisBlock(genesisHeader, &hash);*/
		consensus.hashGenesisBlock = genesis.GetHash();
		assert(consensus.hashGenesisBlock == uint256S("0x00000a7f5622e1499ea8649f89c033973b9d5f4b96715ba21858af6234c7cc32"));
		assert(genesis.hashMerkleRoot == uint256S("0x9342d40305d508194d9fce5faab7d82cd6c9fea01fedc4b37557cb8b34a127c4"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = true;
        fAllowMultipleAddressesFromGroup = true;
        fAllowMultiplePorts = true;

        nFulfilledRequestExpireTime = 5 * 60;// 5min

        
        strSporkAddress = "TfpShWBvNqMRhDbXYrf1HzymEGrMZuBFov"; // 4aF2Q9pMjw6wYXH8wsEVkniDBSh2n4nV4woVodAMYaN4Cmf18fpm

   

		base58Prefixes[PUBKEY_ADDRESS_MARTK] = std::vector<unsigned char>(1, 66);
		base58Prefixes[SCRIPT_ADDRESS_MARTK] = std::vector<unsigned char>(1, 61);
		base58Prefixes[SECRET_KEY_MARTK] = std::vector<unsigned char>(1, 24);

		base58Prefixes[PUBKEY_ADDRESS_BTC] = std::vector<unsigned char>(1, 111);
		base58Prefixes[SCRIPT_ADDRESS_BTC] = std::vector<unsigned char>(1, 196);
		base58Prefixes[SECRET_KEY_BTC] = std::vector<unsigned char>(1, 239);

		base58Prefixes[PUBKEY_ADDRESS_ZEC] = { 0x1C,0xB8 };
		base58Prefixes[SCRIPT_ADDRESS_ZEC] = { 0x1C,0xBD };
		base58Prefixes[SECRET_KEY_ZEC] = std::vector<unsigned char>(1, 239);
		// Regtest Martkist BIP32 pubkeys
		base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xcf).convert_to_container<std::vector<unsigned char> >();
		// Regtest Martkist BIP32 prvkeys
		base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        // Regtest Martkist BIP44 coin type is '0x800004d5'
        nExtCoinType = 0x800004d5;
		

		chainTxData = ChainTxData{
			0,
			0,
			0
		};
   }

    void UpdateBIP9Parameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
    {
        consensus.vDeployments[d].nStartTime = nStartTime;
        consensus.vDeployments[d].nTimeout = nTimeout;
    }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = 0;

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
            return mainParams;
    else if (chain == CBaseChainParams::TESTNET)
            return testNetParams;
    else if (chain == CBaseChainParams::DEVNET) {
            assert(devNetParams);
            return *devNetParams;
    } else if (chain == CBaseChainParams::REGTEST)
            return regTestParams;
    else
        throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    if (network == CBaseChainParams::DEVNET) {
        devNetParams = new CDevNetParams();
    }

    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

void UpdateRegtestBIP9Parameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
{
    regTestParams.UpdateBIP9Parameters(d, nStartTime, nTimeout);
}
