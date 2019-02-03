#ifndef BITCOIN_KERNEL_H
#define BITCOIN_KERNEL_H

#include <primitives/transaction.h>
#include <amount.h>

class CValidationState;
class uint256;
class CBlock;
class CTxOut;
class COutPoint;

bool CheckStakeKernelHash(unsigned int nBits, const CBlock& blockFrom, unsigned int nTxPrevOffset, const CTxOut& txOutPrev, const COutPoint& prevout, uint32_t nTimeTx, uint256& hashProofOfStake);
bool CheckStakeKernelHash(unsigned int nBits, uint32_t nTimeBlockFrom, unsigned int nTxPrevOffset, CAmount nAmount, uint64_t n, uint32_t nTimeTx, uint256& hashProofOfStake);
bool CheckProofOfStake(const CTransactionRef& tx, unsigned int nBits, uint256& hashProofOfStake, unsigned int nBlockTime);
#endif // BITCOIN_KERNEL_H