## deftchain 0.16
_demonstrating skill and intelligence_


- built on bitcoin 0.16
- pow algorithm: balloon512 (with 5 variants, concurrently mineable; no per-algo daemon instances)
- retarget algorithm: shortened and tuned dual_kgw3
- block time: 4 minutes
- block maturity: 30 blocks
- block subsidy: 8.5 DEFT per block
- emission decay: block reward halves every 150,000 blocks
- emission simulation: included as 'deftreward.c'
- coin marketcap: maximum of 3 million DEFT
- coin swap: block 1 will contain 500,000 DEFT; which can be redeemed only by original users of the first deft release.
             coins present on the original chain, before height 30,000 will be accepted via crosschain transaction.


_what it is_

deftchain is an experimental digital currency, which allows users to roll out their own individual blockchain,
by holding an unspent transaction of 2500 DEFT in their wallet, similar to a masternode collateral payment. the
users blockchain will exist and function as long as the transaction is held; allowing the creation, storage or
transmission of a users own currency.


_why restart the chain_

the original deft release was promoted as a cpu-mineable/gpu-resistant currency, where the original balloon cpu hashing
algorithm (after widespread community gpu development), still managed to maintain a ratio of approx 1:4 (cpu/gpu) advantage.
unfortunately an early oversight where both internal hashing/pow hashing algorithm were set to balloon caused poor wallet
performance, potentially breaking the pub/private address pairing if a fork were introduced to remedy this error.

the opportunity was taken to 'harden' the pow algorithm and rectify the wallet's performance, with lessons learnt from a
public release, as well as implementing the 'sidechain as a service' functionality paradigm.

