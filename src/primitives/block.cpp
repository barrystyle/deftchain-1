// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Copyright (c) 2018 The Deftchain developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <primitives/block.h>

#include <hash.h>
#include <tinyformat.h>
#include <utilstrencodings.h>
#include <crypto/common.h>
#include "crypto/balloon512.h"

uint32_t CBlockHeader::GetAlgo() const
{
    uint32_t nAlgoSelected = 0x00000000;
    if (nAlgo == 0x00000000 || /* balloon512-jh      */
        nAlgo == 0x00000001 || /* balloon512-luffa   */
        nAlgo == 0x00000002 || /* balloon512-shavite */
        nAlgo == 0x00000003 || /* balloon512-echo    */
        nAlgo == 0x00000004)   /* balloon512-fugue   */
        nAlgoSelected = nAlgo;
    return nAlgoSelected;
}

uint256 CBlockHeader::GetHash() const
{
    return SerializeHash(*this);
}

uint256 CBlockHeader::GetPoWHash() const
{
    uint256 thash;
    uint32_t nAlgo = GetAlgo();
    if (nAlgo == 0x00000000)
        balloon512(BEGIN(nVersion),BEGIN(thash),6144,4,8,0);
    if (nAlgo == 0x00000001)
        balloon512(BEGIN(nVersion),BEGIN(thash),6144,4,8,1);
    if (nAlgo == 0x00000002)
        balloon512(BEGIN(nVersion),BEGIN(thash),6144,4,8,2);
    if (nAlgo == 0x00000003)
        balloon512(BEGIN(nVersion),BEGIN(thash),6144,4,8,3);
    if (nAlgo == 0x00000004)
        balloon512(BEGIN(nVersion),BEGIN(thash),6144,4,8,4);
    return thash;
}

std::string CBlock::ToString() const
{
    std::stringstream s;
    s << strprintf("CBlock(hash=%s, ver=0x%08x, hashPrevBlock=%s, hashMerkleRoot=%s, nTime=%u, nBits=%08x, nNonce=%u, auxChainHash=%s, auxBlockState=%s, auxBlockData=%s, nAlgo=0x%08x, vtx=%u)\n",
        GetHash().ToString(),
        nVersion,
        hashPrevBlock.ToString(),
        hashMerkleRoot.ToString(),
        nTime, nBits, nNonce,
        auxChainHash.ToString(),
        auxBlockState.ToString(),
        auxBlockData.ToString(),
        nAlgo,
        vtx.size());
    for (const auto& tx : vtx) {
        s << "  " << tx->ToString() << "\n";
    }
    return s.str();
}

