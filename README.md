XPChain Core integration/staging tree
=====================================

[![Build Status](https://travis-ci.org/xpc-wg/xpchain.svg?branch=master)](https://travis-ci.org/xpc-wg/xpchain)

https://www.xpchain.io/

What is XPChain?
----------------

XPChain, which is usually abbreviated XPC, is a cryptocurrency based on Bitcoin 0.17.0.

XPChain introduces new proof-of-stake consensus as a security model, which solves
“nothing at stake” issue in a way proposed by Peercoin community, and enables you
to distribute your staking rewards to anyone in the world.

XPChain Core is the name of open source
software which enables the use of this currency.

For more information, <!-- as well as an immediately useable, binary version of
the XPChain Core software, see https://bitcoincore.org/en/download/, --> read the [original whitepaper](https://www.xpchain.io/?loc=lnkwhitepaper).

For Exchanges
-------
- The amounts of XPChain in any transaction must be decimals to at most 4 places,
while those of Bitcoin are ones to up to 8 places. If you try to generate a transaction,
which contains the amount that is a decimal to more than 4 places, an error will occur.
- Make sure you put no restrictions on the length of withdrawal addresses. Addresses of
XPChain can be around 75 characters. Nothing would probably go wrong if you support the latest
Bitcoin's address generation.
- Use native Segwit (bech32) addresses instead of legacy addresses, which enables you to send
XPC to either legacy, native Segwit, P2SH-Segwit, or P2WSH addresses.
- You can be sent XPC that cannot be spent in next 100 blocks, because XPChain wallet has the
function of sending proof-of-stake reward to arbitrary addresses and its users can easily transfer
their reward to an exchange's deposit address.
- Make sure your wallet is running with `-minting=0` to disable minting.
When you mint a block with your customer's funds, you receive a transaction of ‘payment to yourself’
(called ‘coinstake’ or ‘age-burn’ transaction) as well as a coinbase transaction. This would cause
incorrect double depositing if your deposit system were to monitor only payments to the customer's deposit addresses.

License
-------

XPChain Core is released under the terms of the MIT license. See [COPYING](COPYING) for more
information or see https://opensource.org/licenses/MIT.

Development Process
-------------------

The `master` branch is regularly built and tested, but is not guaranteed to be
completely stable. [Tags](https://github.com/xpc-wg/xpchain/tags) are created
regularly to indicate new official, stable release versions of XPChain Core.

The contribution workflow is described in [CONTRIBUTING.md](CONTRIBUTING.md).

Testing
-------

Testing and code review is the bottleneck for development; we get more pull
requests than we can review and test on short notice. Please be patient and help out by testing
other people's pull requests, and remember this is a security-critical project where any mistake might cost people
lots of money.

### Automated Testing

Developers are strongly encouraged to write [unit tests](src/test/README.md) for new code, and to
submit new unit tests for old code. Unit tests can be compiled and run
(assuming they weren't disabled in configure) with: `make check`. Further details on running
and extending unit tests can be found in [/src/test/README.md](/src/test/README.md).

There are also [regression and integration tests](/test), written
in Python, that are run automatically on the build server.
These tests can be run (if the [test dependencies](/test) are installed) with: `test/functional/test_runner.py`

The Travis CI system makes sure that every pull request is built for Windows, Linux, and macOS, and that unit/sanity tests are run automatically.

### Manual Quality Assurance (QA) Testing

Changes should be tested by somebody other than the developer who wrote the
code. This is especially important for large or high-risk changes. It is useful
to add a test plan to the pull request description if testing the changes is
not straightforward.

Translations
------------

While Bitcoin has [their Transifex page](https://www.transifex.com/projects/p/bitcoin/),
XPChain doesn't have such pages of external translation web service (e.g. Transifex or Crowdin). If you want to make some change as well as add new translations, just create a new GitHub pull request
to whose title is prefixed “Translation:”.
Read [translation_process.md](https://github.com/xpc-wg/xpchain/blob/master/doc/translation_process.md) for details.
