XPChain Core version 0.17.0-2 is now available from:

  <https://www.xpchain.io/?loc=lnkwallet>

This is a new minor version release, with various bugfixes
and translation updates.

Please report bugs using the issue tracker at GitHub:

  <https://github.com/xpc-wg/xpchain/issues>

How to Upgrade
==============

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes for older versions), then run the
installer (on Windows) or just copy over `/Applications/XPChain-Qt` (on Mac).

<!-- commented out because there doesn't seems any change for the chainstate database-->
<!--
The first time you run version 0.17.0-0 or newer, your chainstate database will be converted to a
new format, which will take anywhere from a few minutes to half an hour,
depending on the speed of your machine.

Downgrading warning
-------------------

The chainstate database for this release is not compatible with previous
releases, so if you run 0.15 and then decide to switch back to any
older version, you will need to run the old release with the `-reindex-chainstate`
option to rebuild the chainstate data structures in the old format.

If your node has pruning enabled, this will entail re-downloading and
processing the entire blockchain. -->

Compatibility
==============

XPChain Core is extensively tested on multiple operating systems using
the Linux kernel, macOS 10.10+, and Windows 7 and newer (Windows XP is not supported).

XPChain Core should also work on most other Unix-like systems but is not
frequently tested on them.

Notable changes
===============

New RPC for listing states of minting
----------------------------------------

The `listmintings` RPC lists data of unspent transaction outputs (UTXO)
and the states of minting, including coin-age of the UTXO, the probability
of minting, and the expected amounts of the reward, which were displayed only
in Qt wallet. You can filter the result similarly to `listunspent`, as well as by
a minimum or maximum of coin-age.

Missing records in Minting tab
-----------------------------------

Some of the records of states of minting didn't show up in Minting tab when a transaction
had two or more outputs to your addresses, because the record of one in the table
overwrote those of the others. Now this is fixed so that you can see the complete
list of the states.

St13runtime_error in minting
----------------------------

Your wallet sometimes caused `EXCEPTION: St13runtime_error` and failed to mint
a new block when trying to create a block with one of your UTXOs. The function
of proof-of-stake has been so fully refactored that this error will never cause.

GUI changes
-----------

- In Minting tab, the setting for displaying probability *within 90 days* is replaced by that for *within 60 days*.

XPChain 0.17.0-2 change log
------------------

### Mining/Staking
- #52 `ce2f2fb` Mining: create coinstake tx & new block only after successful staking (mban259)

### RPC
- #61 `6f06c40` RPC/REST/ZMQ: Add new rpc endpoint for display the list of minting transaction. (serisia)

### GUI
- #51 `4cd39b3` Qt: Fix mintingtab identity (add vout number) (serisia)
- #54 `c83237d` Qt: Change Mintingtab timespan (serisia)
- #56 `8cc86ff` Qt: refactoring KernelRecord (serisia)

### Translation
- #53 `5987542` Translation: Fix wrong ja translation of label for expert section (MaySoMusician)
- #58 `62c312d` Translation: Change translation for shorter probability interval (MaySoMusician)
- #59 `d55a4e7` Translations: bugfix ko-KR.ts (Falconno7)

### Documentation
- #49 `1fe54a1` Docs: Fix typo in README.md (MaySoMusician)
- #60 `e20af01` Docs: Add warnings for exchanges (MaySoMusician)

### Miscellaneous
- #50 `c63ae68` Replace Bitcoin -> XPChain (serisia)
- #55 `b85b932` Wallet: Make description of -minting argument more clear (MaySoMusician)
- #57 `fded3b9` Scripts and tools: Add Dockerfile and compose sample file (moochannel)

Credits
=======

Thanks to everyone who directly contributed to this release:

- Falconno7
- MaySoMusician
- mban259
- moochannel
- serisia

As well as everyone that reported bugs & issues through Discord etc.
