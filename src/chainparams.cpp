// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2017 The PIVX developers
// Copyright (c) 2017-2020 The VALUTO Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "random.h"
#include "util.h"
#include "utilstrencodings.h"

#include <mutex>
#include <assert.h>
#include <limits>

#include <boost/assign/list_of.hpp>

using namespace std;
using namespace boost::assign;

struct SeedSpec6 {
    uint8_t addr[16];
    uint16_t port;
};

#include "chainparamsseeds.h"

/**
 * Main network
 */
//static bool regenerate = true;
/*
void MineGenesis(CBlock genesis) 
{
    uint256 hashGenesisBlock = uint256S("");
    genesis.nNonce = 0;
    if (true && (genesis.GetKeccakHash() != hashGenesisBlock)) {
        uint256 hashTarget = uint256().SetCompact(genesis.nBits);
        while (genesis.GetKeccakHash() > hashTarget)
        {
            ++genesis.nNonce;
            if (genesis.nNonce == 0)
            {
                ++genesis.nTime;
            }
        }
        std::cout << "// Mainnet ---";
        std::cout << " nonce: " << genesis.nNonce;
        std::cout << " time: " << genesis.nTime;
        std::cout << " hash: 0x" << genesis.GetKeccakHash().ToString().c_str();
        std::cout << " merklehash: 0x"  << genesis.hashMerkleRoot.ToString().c_str() <<  "\n";
    }
}
*/
//! Convert the pnSeeds6 array into usable address objects.
static void convertSeed6(std::vector<CAddress>& vSeedsOut, const SeedSpec6* data, unsigned int count)
{
    // It'll only connect to one or two seed nodes because once it connects,
    // it'll get a pile of addresses with newer timestamps.
    // Seed nodes are given a random 'last seen time' of between one and two
    // weeks ago.
    const int64_t nOneWeek = 7 * 24 * 60 * 60;
    for (unsigned int i = 0; i < count; i++) {
        struct in6_addr ip;
        memcpy(&ip, data[i].addr, sizeof(ip));
        CAddress addr(CService(ip, data[i].port));
        addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

//   What makes a good checkpoint block?
// + Is surrounded by blocks with reasonable timestamps
//   (no blocks before with a timestamp after, none after with
//    timestamp before)
// + Contains no strange transactions
static Checkpoints::MapCheckpoints mapCheckpoints =
    boost::assign::map_list_of(0, uint256("000008bc7f79ecc5b33272e437d1ff0e0f8177b2a3922243ab7f09b8b2e7d601"))
                              (25, uint256("00000026b7e4bab277155e08ed9154260b7db82e2905bd617aa8b3c9f7623d95"));

static const Checkpoints::CCheckpointData data = {
    &mapCheckpoints,
    1607727903, // * UNIX timestamp of last checkpoint block
    26,      // * total number of transactions between genesis and last checkpoint
                //   (the tx=... number in the SetBestChain debug.log lines)
    250        // * estimated number of transactions per day after checkpoint
};

static Checkpoints::MapCheckpoints mapCheckpointsTestnet =
    boost::assign::map_list_of(0, uint256("000006b020d0db323b363c4d762b6931cff1855fd8a85a4455f416a91e9424f1"));
static const Checkpoints::CCheckpointData dataTestnet = {
    &mapCheckpointsTestnet,
    1529667000,
    0,
    250};

static Checkpoints::MapCheckpoints mapCheckpointsRegtest =
    boost::assign::map_list_of(0, uint256("7b23852e8329f1731152ab98e59d5bfe8cb355342ec75bac7c87bcd819113af3"));
static const Checkpoints::CCheckpointData dataRegtest = {
    &mapCheckpointsRegtest,
    1529668200,
    0,
    100};

const CChainParams::SubsidySwitchPoints& CChainParams::GetSubsidySwitchPoints(uint32_t nTime, int nHeight) const
{
    return subsidySwitchPoints;
}

CAmount CChainParams::SubsidyValue(SubsidySwitchPoints::key_type level, uint32_t nTime, int nHeight) const
{
    const auto& switch_points = GetSubsidySwitchPoints(nTime, nHeight);

    SubsidySwitchPoints::const_iterator point = switch_points.upper_bound(level);

    if(point != switch_points.begin())
        point = std::prev(point);

    return point->second;
}

void CChainParams::initSubsidySwitchPointsSchedule()
{
    subsidySwitchPointsSchedule_F2[0u] = subsidySwitchPoints_F2_0;

    for(auto i = 1u; i <= subsidyDecreaseCount_F2; ++i)
    {
       subsidySwitchPointsSchedule_F2[i] = subsidySwitchPointsSchedule_F2[i - 1];

       for(auto& sp : subsidySwitchPointsSchedule_F2[i])
       {
           auto prev_value = sp.second;

           sp.second *= 10000u - subsidyDecreaseValue_F2;
           sp.second /= 10000u;
           sp.second += COIN / 10u - 1u;
           sp.second /= COIN / 10u;
           sp.second *= COIN / 10u;

           if(sp.second == prev_value && sp.second > COIN / 10u)
             sp.second -= COIN / 10u;
       }
    }
}

class CMainParams : public CChainParams
{
public:
    CMainParams()
    {
        networkID = CBaseChainParams::MAIN;
        strNetworkID = "main";
        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 4-byte int at any alignment.
         */
        pchMessageStart[0] = 0x90;
        pchMessageStart[1] = 0xc3;
        pchMessageStart[2] = 0xfe;
        pchMessageStart[3] = 0xe8;
        vAlertPubKey = ParseHex("04A2B684CBABE97BA08A35EA388B06A6B03E13DFBA974466880AF4CAE1C5B606A751BF7C5CBDE5AB90722CF5B1EC1AADA6D24D607870B6D6B5D684082655404C8D");
        //vVALUTODevKey = ParseHex("0315c79a9e9891b6df7d85a5f6035ca437d2749bdc07189cc2e31f4b7e74b1e83d"); // DevPubKey for fees
        //vVALUTOFundKey = ParseHex("0315c79a9e9891b6df7d85a5f6035ca437d2749bdc07189cc2e31f4b7e74b1e83d"); // FundPubKey for fees
        //nDevFee = 0; // DevFee 0%
        //nFundFee = 0; //FundFee 0%
        nDefaultPort = 1945;
        bnProofOfWorkLimit = ~uint256(0) >> 20;
        bnStartWork = ~uint256(0) >> 24;

        nMaxReorganizationDepth = 100;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 0;
        nTargetSpacing = 1 * 30;  // VALUTO Reward: Every 30 seconds
        nAntiInstamineTime = 100; // 100 blocks with 1 reward for instamine prevention
        nMaturity = 60;
        nMasternodeCountDrift = 20;
        nMaxMoneyOut = 133000007 * COIN;

        nStartMasternodePaymentsBlock = 100;

        /** Height or Time Based Activations **/
        nLastPOWBlock = 250;
        nModifierUpdateBlock = std::numeric_limits<decltype(nModifierUpdateBlock)>::max();
        nStartMasternodePayments = 1420837558; //Wed, 25 Jun 2014 20:36:16 GMT
        nHEXHashTimestamp = 1420837558; // 6  August  2018, 15:00:00 GMT+00:00
        nF2Timestamp      = 1420837558; // 28 October 2018, 12:00:00 GMT+00:00
        nF3ActivationHeight = 5;

        /**
         * Build the genesis block. Note that the output of the genesis coinbase cannot
         * be spent as it did not originally exist in the database.
         *
         * CBlock(hash=00000ffd590b14, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=e0028e, nTime=1390095618, nBits=1e0ffff0, nNonce=28917698, vtx=1)
         *   CTransaction(hash=e0028e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
         *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d01044c5957697265642030392f4a616e2f3230313420546865204772616e64204578706572696d656e7420476f6573204c6976653a204f76657273746f636b2e636f6d204973204e6f7720416363657074696e6720426974636f696e73)
         *     CTxOut(nValue=50.00000000, scriptPubKey=0xA9037BAC7050C479B121CF)
         *   vMerkleTree: e0028e
         */
        const char* pszTimestamp = "2017-12-14 03:03:03 The day the chicken crossed the road";
        CMutableTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 50 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("044a001040da79684a0544c2254eb6c896fae95a9ea7b51d889475eb57ab2051f1a5858cac61ae400e90ea08015263ad40c65d36f0edf19e996972e7d2cbd13c15") << OP_CHECKSIG;
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nTime = 1506654183;
        genesis.nBits = 0x1e0ffff0;
        genesis.nNonce = 1245242;

        hashGenesisBlock = genesis.GetKeccakHash();

        assert(hashGenesisBlock == uint256("000008bc7f79ecc5b33272e437d1ff0e0f8177b2a3922243ab7f09b8b2e7d601"));
        assert(genesis.hashMerkleRoot == uint256("17389656e476eefcb07768e4c55fe2cde6a1f57f789cdf287738dc3c2f323b3d"));

        vSeeds.push_back(CDNSSeedData("178.62.255.229", "178.62.255.229"));
        vSeeds.push_back(CDNSSeedData("128.199.60.61", "128.199.60.61"));
        vSeeds.push_back(CDNSSeedData("161.35.152.183", "161.35.152.183"));
        vSeeds.push_back(CDNSSeedData("167.71.77.5", "167.71.77.5"));
        vSeeds.push_back(CDNSSeedData("178.62.235.63", "178.62.235.63"));
        vSeeds.push_back(CDNSSeedData("64.227.35.216", "64.227.35.216"));
        vSeeds.push_back(CDNSSeedData("134.122.96.145", "134.122.96.145"));
        vSeeds.push_back(CDNSSeedData("167.172.50.18", "167.172.50.18"));
        vSeeds.push_back(CDNSSeedData("64.227.40.36", "64.227.40.36"));
        vSeeds.push_back(CDNSSeedData("64.227.35.93", "64.227.35.93"));

        //vSeeds.push_back(CDNSSeedData("valuto.io", "seeds.seeder01.valuto.io"));     // Primary DNS Seeder

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 70);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 8);
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 212);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x02)(0x2D)(0x25)(0x33).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x02)(0x21)(0x31)(0x2B).convert_to_container<std::vector<unsigned char> >();
        // BIP44 coin type is from https://github.com/satoshilabs/slips/blob/master/slip-0044.md
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x80)(0x00)(0x07)(0x99).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_main, ARRAYLEN(pnSeed6_main));

        fRequireRPCPassword = true;
        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fSkipProofOfWorkCheck = false;
        fTestnetToBeDeprecatedFieldRPC = false;
        fHeadersFirstSyncingActive = false;

        nPoolMaxTransactions = 3;
        strSporkKey = "04520C1E6A46596DD9CA9A1A69B96D630410CBA2A1047FC462ADAA5D3BE451CC43B2E30C64A03513F31B3DB9450A3FC2F742DCB4AD99450575219549890392F465";
        strObfuscationPoolDummyAddress = "XByRaZu3ZHiZownNf2pFAL2ciF7ZKTGnGE";

    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return data;
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CMainParams
{
public:
    CTestNetParams()
    {
        networkID = CBaseChainParams::TESTNET;
        strNetworkID = "test";
        pchMessageStart[0] = 0x47;
        pchMessageStart[1] = 0x77;
        pchMessageStart[2] = 0x66;
        pchMessageStart[3] = 0xbb;

        bnProofOfWorkLimit = ~uint256(0) >> 1;
        bnStartWork = bnProofOfWorkLimit;

        subsidySwitchPoints = {
           {0        ,   4 * COIN},
           {2   * 1e7,   5 * COIN},
           {3   * 1e7,   7 * COIN},
           {5   * 1e7,   9 * COIN},
           {8   * 1e7,  11 * COIN},
           {13  * 1e7,  15 * COIN},
           {21  * 1e7,  20 * COIN},
           {34  * 1e7,  27 * COIN},
           {55  * 1e7,  39 * COIN},
           {89  * 1e7,  57 * COIN},
           {144 * 1e7,  85 * COIN},
           {233 * 1e7, 131 * COIN},
           {377 * 1e7, 204 * COIN},
           {610 * 1e7, 321 * COIN},
           {987 * 1e7, 511 * COIN},
        };
        assert(subsidySwitchPoints.size());

        vAlertPubKey = ParseHex("04459DC949A9E2C2E1FA87ED9EE93F8D26CD52F95853EE24BCD4B07D4B7D79458E81F0425D81E52B797ED304A836667A1D2D422CD10F485B06CCBE906E1081FBAC");
        nDefaultPort = 11945;
        nEnforceBlockUpgradeMajority = 51;
        nRejectBlockOutdatedMajority = 75;
        nToCheckBlockUpgradeMajority = 100;
        nMinerThreads = 0;
        nTargetSpacing = 1 * 60;  // VALUTO: 1 minute
        nLastPOWBlock = std::numeric_limits<decltype(nLastPOWBlock)>::max();
        nMaturity = 15;
        nMasternodeCountDrift = 4;
        nModifierUpdateBlock = std::numeric_limits<decltype(nModifierUpdateBlock)>::max();
        nMaxMoneyOut = 1000000000 * COIN;


        //! Modify the testnet genesis block so the timestamp is valid for a later start.
        genesis.nTime = 1529667000;
        genesis.nNonce = 979797;

        hashGenesisBlock = genesis.GetKeccakHash();

        assert(hashGenesisBlock == uint256("00000ff793bfbdb26d2fd1a1254ce33bb54e5db649ae9fa6fa3a247359f85f67"));

        vFixedSeeds.clear();
        vSeeds.clear();
        //vSeeds.push_back(CDNSSeedData("valuto.io", "seed01.valuto.io"));     // Primary DNS Seeder

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 137); // Testnet VALUTO addresses start with 'x'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 19);  // Testnet VALUTO script addresses start with '8' or '9'
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 239);     // Testnet private keys start with '9' or 'c' (Bitcoin defaults)
        // Testnet VALUTO BIP32 pubkeys start with 'DRKV'
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x3a)(0x80)(0x61)(0xa0).convert_to_container<std::vector<unsigned char> >();
        // Testnet VALUTO BIP32 prvkeys start with 'DRKP'
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x3a)(0x80)(0x58)(0x37).convert_to_container<std::vector<unsigned char> >();
        // Testnet VALUTO BIP44 coin type is '1' (All coin's testnet default)
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x80)(0x00)(0x00)(0x01).convert_to_container<std::vector<unsigned char> >();

        convertSeed6(vFixedSeeds, pnSeed6_test, ARRAYLEN(pnSeed6_test));

        fRequireRPCPassword = true;
        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        nPoolMaxTransactions = 2;
        strSporkKey = "0421838CC1407E7B8C0C5F2379DF7EBD395181949CFA55124939B4980D5054A7926F88E3059921A50F0F81C5195E882D9A414EA0835BB89C9BB061511B9F132B31";
        strObfuscationPoolDummyAddress = "y57cqfGRkekRyDRNeJiLtYVEbvhXrNbmox";
        nStartMasternodePayments = 1420837558; //Fri, 09 Jan 2015 21:05:58 GMT
    }
    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataTestnet;
    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CTestNetParams
{
public:
    CRegTestParams()
    {
        networkID = CBaseChainParams::REGTEST;
        strNetworkID = "regtest";
        pchMessageStart[0] = 0xa1;
        pchMessageStart[1] = 0xcf;
        pchMessageStart[2] = 0x7e;
        pchMessageStart[3] = 0xac;

        bnStartWork = ~uint256(0) >> 20;

        subsidySwitchPoints = {
           {0        ,   4 * COIN},
           {2   * 1e7,   5 * COIN},
           {3   * 1e7,   7 * COIN},
           {5   * 1e7,   9 * COIN},
           {8   * 1e7,  11 * COIN},
           {13  * 1e7,  15 * COIN},
           {21  * 1e7,  20 * COIN},
           {34  * 1e7,  27 * COIN},
           {55  * 1e7,  39 * COIN},
           {89  * 1e7,  57 * COIN},
           {144 * 1e7,  85 * COIN},
           {233 * 1e7, 131 * COIN},
           {377 * 1e7, 204 * COIN},
           {610 * 1e7, 321 * COIN},
           {987 * 1e7, 511 * COIN},
        };
        assert(subsidySwitchPoints.size());

        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 1;
        nTargetSpacing = 1 * 60;        // VALUTO: 1 minute
        bnProofOfWorkLimit = ~uint256(0) >> 1;
        genesis.nTime = 1529668200;
        genesis.nBits = 0x207fffff;
        genesis.nNonce = 0;

        hashGenesisBlock = genesis.GetKeccakHash();
        nDefaultPort = 51476;

        assert(hashGenesisBlock == uint256("7b23852e8329f1731152ab98e59d5bfe8cb355342ec75bac7c87bcd819113af3"));

        vFixedSeeds.clear(); //! Testnet mode doesn't have any fixed seeds.
        vSeeds.clear();      //! Testnet mode doesn't have any DNS seeds.

        fRequireRPCPassword = false;
        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;
    }
    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataRegtest;
    }
};
static CRegTestParams regTestParams;

/**
 * Unit test
 */
class CUnitTestParams : public CMainParams, public CModifiableParams
{
public:
    CUnitTestParams()
    {
        networkID = CBaseChainParams::UNITTEST;
        strNetworkID = "unittest";
        nDefaultPort = 51478;
        vFixedSeeds.clear(); //! Unit test mode doesn't have any fixed seeds.
        vSeeds.clear();      //! Unit test mode doesn't have any DNS seeds.

        fRequireRPCPassword = false;
        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fMineBlocksOnDemand = true;

        subsidySwitchPoints = {
            {0         ,   1 * COIN},
            {2   * 1e5,   2 * COIN},
            {3   * 1e5,   3 * COIN},
            {5   * 1e5,   5 * COIN},
            {8   * 1e5,   8 * COIN},
            {13  * 1e5,  13 * COIN},
            {21  * 1e5,  21 * COIN},
            {34  * 1e5,  34 * COIN},
            {55  * 1e5,  55 * COIN},
            {89  * 1e5,  89 * COIN},
            {144 * 1e5, 144 * COIN},
            {233 * 1e5, 233 * COIN},
            {377 * 1e5, 377 * COIN},
            {610 * 1e5, 610 * COIN},
            {987 * 1e5, 987 * COIN},
        };
        assert(subsidySwitchPoints.size());

    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        // UnitTest share the same checkpoints as MAIN
        return data;
    }

    //! Published setters to allow changing values in unit test cases
    virtual void setEnforceBlockUpgradeMajority(int anEnforceBlockUpgradeMajority) { nEnforceBlockUpgradeMajority = anEnforceBlockUpgradeMajority; }
    virtual void setRejectBlockOutdatedMajority(int anRejectBlockOutdatedMajority) { nRejectBlockOutdatedMajority = anRejectBlockOutdatedMajority; }
    virtual void setToCheckBlockUpgradeMajority(int anToCheckBlockUpgradeMajority) { nToCheckBlockUpgradeMajority = anToCheckBlockUpgradeMajority; }
    virtual void setDefaultConsistencyChecks(bool afDefaultConsistencyChecks) { fDefaultConsistencyChecks = afDefaultConsistencyChecks; }
    virtual void setSkipProofOfWorkCheck(bool afSkipProofOfWorkCheck) { fSkipProofOfWorkCheck = afSkipProofOfWorkCheck; }
};
static CUnitTestParams unitTestParams;


static CChainParams* pCurrentParams = 0;

CModifiableParams* ModifiableParams()
{
    assert(pCurrentParams);
    assert(pCurrentParams == &unitTestParams);
    return (CModifiableParams*)&unitTestParams;
}

const CChainParams& Params()
{
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(CBaseChainParams::Network network)
{
    switch (network) {
    case CBaseChainParams::MAIN:
        return mainParams;
    case CBaseChainParams::TESTNET:
        return testNetParams;
    case CBaseChainParams::REGTEST:
        return regTestParams;
    case CBaseChainParams::UNITTEST:
        return unitTestParams;
    default:
        assert(false && "Unimplemented network");
        return mainParams;
    }
}

void SelectParams(CBaseChainParams::Network network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

bool SelectParamsFromCommandLine()
{
    CBaseChainParams::Network network = NetworkIdFromCommandLine();
    if (network == CBaseChainParams::MAX_NETWORK_TYPES)
        return false;

    SelectParams(network);
    return true;
}
