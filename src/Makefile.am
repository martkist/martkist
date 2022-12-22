# Copyright (c) 2013-2016 The Bitcoin Core developers
# Copyright (c) 2014-2023 The Martkist Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

DIST_SUBDIRS = secp256k1 univalue

AM_LDFLAGS = $(PTHREAD_CFLAGS) $(LIBTOOL_LDFLAGS) $(HARDENED_LDFLAGS)
AM_CXXFLAGS = $(HARDENED_CXXFLAGS) $(ERROR_CXXFLAGS)
AM_CPPFLAGS = $(HARDENED_CPPFLAGS)
EXTRA_LIBRARIES =

if EMBEDDED_UNIVALUE
LIBUNIVALUE = univalue/libunivalue.la

$(LIBUNIVALUE): $(wildcard univalue/lib/*) $(wildcard univalue/include/*)
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F)
else
LIBUNIVALUE = $(UNIVALUE_LIBS)
endif

MARTKIST_CONFIG_INCLUDES=-I$(builddir)/config
MARTKIST_INCLUDES=-I$(builddir) -I$(builddir)/obj $(BDB_CPPFLAGS) $(BOOST_CPPFLAGS) $(LEVELDB_CPPFLAGS) $(CRYPTO_CFLAGS) $(SSL_CFLAGS)

MARTKIST_INCLUDES += -I$(srcdir)/secp256k1/include
MARTKIST_INCLUDES += $(UNIVALUE_CFLAGS)

LIBMARTKIST_SERVER=libmartkist_server.a
LIBMARTKIST_COMMON=libmartkist_common.a
LIBMARTKIST_CONSENSUS=libmartkist_consensus.a
LIBMARTKIST_CLI=libmartkist_cli.a
LIBMARTKIST_UTIL=libmartkist_util.a
LIBMARTKIST_CRYPTO=crypto/libmartkist_crypto.a
LIBMARTKISTQT=qt/libmartkistqt.a
LIBSECP256K1=secp256k1/libsecp256k1.la

if ENABLE_ZMQ
LIBMARTKIST_ZMQ=libmartkist_zmq.a
endif
if BUILD_MARTKIST_LIBS
LIBMARTKISTCONSENSUS=libmartkistconsensus.la
endif
if ENABLE_WALLET
LIBMARTKIST_WALLET=libmartkist_wallet.a
endif

$(LIBSECP256K1): $(wildcard secp256k1/src/*) $(wildcard secp256k1/include/*)
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F)

# Make is not made aware of per-object dependencies to avoid limiting building parallelization
# But to build the less dependent modules first, we manually select their order here:
EXTRA_LIBRARIES += \
  $(LIBMARTKIST_CRYPTO) \
  $(LIBMARTKIST_UTIL) \
  $(LIBMARTKIST_COMMON) \
  $(LIBMARTKIST_CONSENSUS) \
  $(LIBMARTKIST_SERVER) \
  $(LIBMARTKIST_CLI) \
  $(LIBMARTKIST_WALLET) \
  $(LIBMARTKIST_ZMQ)

lib_LTLIBRARIES = $(LIBMARTKISTCONSENSUS)

bin_PROGRAMS =
TESTS =
BENCHMARKS =

if BUILD_MARTKISTD
  bin_PROGRAMS += martkistd
endif

if BUILD_MARTKIST_UTILS
  bin_PROGRAMS += martkist-cli
endif

.PHONY: FORCE check-symbols check-security
# martkist core #
MARTKIST_CORE_H = \
  alias.h \
  offer.h \
  cert.h \
  ranges.h \
  graph.h \
  asset.h \
  assetallocation.h \
  escrow.h \
  feedback.h \
  auxpow.h \
  thread_pool/fixed_function.hpp \
  thread_pool/mpmc_bounded_queue.hpp \
  thread_pool/thread_pool.hpp \
  thread_pool/thread_pool_options.hpp \
  thread_pool/worker.hpp \
  addrdb.h \
  activemasternode.h \
  addressindex.h \
  spentindex.h \
  addrman.h \
  alert.h \
  base58.h \
  bip39.h \
  bip39_english.h \
  blockencodings.h \
  bloom.h \
  cachemap.h \
  cachemultimap.h \
  chain.h \
  chainparams.h \
  chainparamsbase.h \
  chainparamsseeds.h \
  checkpoints.h \
  checkqueue.h \
  clientversion.h \
  coins.h \
  compat.h \
  compat/byteswap.h \
  compat/endian.h \
  compat/sanity.h \
  compressor.h \
  consensus/consensus.h \
  core_io.h \
  core_memusage.h \
  cuckoocache.h \
  privatesend.h \
  privatesend-client.h \
  privatesend-server.h \
  privatesend-util.h \
  dsnotificationinterface.h \
  governance.h \
  governance-classes.h \
  governance-exceptions.h \
  governance-object.h \
  governance-validators.h \
  governance-vote.h \
  governance-votedb.h \
  flat-database.h \
  hdchain.h \
  httprpc.h \
  httpserver.h \
  indirectmap.h \
  init.h \
  instantx.h \
  key.h \
  keepass.h \
  keystore.h \
  dbwrapper.h \
  limitedmap.h \
  masternode.h \
  masternode-payments.h \
  masternode-sync.h \
  masternodeman.h \
  masternodeconfig.h \
  memusage.h \
  merkleblock.h \
  messagesigner.h \
  miner.h \
  net.h \
  net_processing.h \
  netaddress.h \
  netbase.h \
  netfulfilledman.h \
  netmessagemaker.h \
  noui.h \
  policy/fees.h \
  policy/policy.h \
  policy/rbf.h \
  pow.h \
  protocol.h \
  random.h \
  reverselock.h \
  rpc/client.h \
  rpc/mining.h \
  rpc/protocol.h \
  rpc/server.h \
  rpc/register.h \
  scheduler.h \
  script/sigcache.h \
  script/sign.h \
  script/standard.h \
  script/ismine.h \
  spork.h \
  streams.h \
  support/allocators/secure.h \
  support/allocators/zeroafterfree.h \
  support/cleanse.h \
  support/events.h \
  support/lockedpool.h \
  sync.h \
  threadsafety.h \
  threadinterrupt.h \
  timedata.h \
  torcontrol.h \
  txdb.h \
  txmempool.h \
  ui_interface.h \
  undo.h \
  util.h \
  utilmoneystr.h \
  utiltime.h \
  validation.h \
  validationinterface.h \
  versionbits.h \
  wallet/coincontrol.h \
  wallet/crypter.h \
  wallet/db.h \
  wallet/rpcwallet.h \
  wallet/wallet.h \
  wallet/walletdb.h \
  warnings.h \
  zmq/zmqabstractnotifier.h \
  zmq/zmqconfig.h \
  zmq/zmqnotificationinterface.h \
  zmq/zmqpublishnotifier.h


obj/build.h: FORCE
	@$(MKDIR_P) $(builddir)/obj
	@$(top_srcdir)/share/genbuild.sh $(abs_top_builddir)/src/obj/build.h \
	  $(abs_top_srcdir)
libmartkist_util_a-clientversion.$(OBJEXT): obj/build.h

# server: shared between martkistd and martkist-qt
libmartkist_server_a_CPPFLAGS = $(AM_CPPFLAGS) $(MARTKIST_INCLUDES) $(MINIUPNPC_CPPFLAGS) $(EVENT_CFLAGS) $(EVENT_PTHREADS_CFLAGS)
libmartkist_server_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libmartkist_server_a_SOURCES = \
  alias.cpp \
  offer.cpp \
  cert.cpp \
  ranges.cpp \
  graph.cpp \
  asset.cpp \
  assetallocation.cpp \
  escrow.cpp \
  auxpow.cpp \
  activemasternode.cpp \
  addrman.cpp \
  addrdb.cpp \
  alert.cpp \
  bloom.cpp \
  blockencodings.cpp \
  chain.cpp \
  checkpoints.cpp \
  dsnotificationinterface.cpp \
  httprpc.cpp \
  httpserver.cpp \
  init.cpp \
  instantx.cpp \
  dbwrapper.cpp \
  governance.cpp \
  governance-classes.cpp \
  governance-object.cpp \
  governance-validators.cpp \
  governance-vote.cpp \
  governance-votedb.cpp \
  masternode.cpp \
  masternode-payments.cpp \
  masternode-sync.cpp \
  masternodeconfig.cpp \
  masternodeman.cpp \
  merkleblock.cpp \
  messagesigner.cpp \
  miner.cpp \
  net.cpp \
  netfulfilledman.cpp \
  net_processing.cpp \
  noui.cpp \
  policy/fees.cpp \
  policy/policy.cpp \
  pow.cpp \
  privatesend.cpp \
  privatesend-server.cpp \
  rest.cpp \
  rpc/blockchain.cpp \
  rpc/masternode.cpp \
  rpc/governance.cpp \
  rpc/mining.cpp \
  rpc/misc.cpp \
  rpc/net.cpp \
  rpc/rawtransaction.cpp \
  rpc/server.cpp \
  script/sigcache.cpp \
  script/ismine.cpp \
  sendalert.cpp \
  spork.cpp \
  timedata.cpp \
  torcontrol.cpp \
  txdb.cpp \
  txmempool.cpp \
  ui_interface.cpp \
  validation.cpp \
  validationinterface.cpp \
  versionbits.cpp \
  $(MARTKIST_CORE_H)

if ENABLE_ZMQ
libmartkist_zmq_a_CPPFLAGS = $(MARTKIST_INCLUDES) $(ZMQ_CFLAGS)
libmartkist_zmq_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libmartkist_zmq_a_SOURCES = \
  zmq/zmqabstractnotifier.cpp \
  zmq/zmqnotificationinterface.cpp \
  zmq/zmqpublishnotifier.cpp
endif


# wallet: shared between martkistd and martkist-qt, but only linked
# when wallet enabled
libmartkist_wallet_a_CPPFLAGS = $(AM_CPPFLAGS) $(MARTKIST_INCLUDES)
libmartkist_wallet_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libmartkist_wallet_a_SOURCES = \
  keepass.cpp \
  privatesend-client.cpp \
  privatesend-util.cpp \
  wallet/crypter.cpp \
  wallet/db.cpp \
  wallet/rpcdump.cpp \
  wallet/rpcwallet.cpp \
  wallet/wallet.cpp \
  wallet/walletdb.cpp \
  policy/rbf.cpp \
  $(MARTKIST_CORE_H)

# crypto primitives library
crypto_libmartkist_crypto_a_CPPFLAGS = $(AM_CPPFLAGS) $(MARTKIST_CONFIG_INCLUDES) $(PIC_FLAGS)
crypto_libmartkist_crypto_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS) $(PIC_FLAGS)
crypto_libmartkist_crypto_a_SOURCES = \
  crypto/aes.cpp \
  crypto/aes.h \
  crypto/common.h \
  crypto/hmac_sha256.cpp \
  crypto/hmac_sha256.h \
  crypto/hmac_sha512.cpp \
  crypto/hmac_sha512.h \
  crypto/ripemd160.cpp \
  crypto/ripemd160.h \
  crypto/sha1.cpp \
  crypto/sha1.h \
  crypto/sha256.cpp \
  crypto/sha256.h \
  crypto/sha512.cpp \
  crypto/sha512.h

if USE_ASM
crypto_libmartkist_crypto_a_SOURCES += crypto/sha256_sse4.cpp
endif

# consensus: shared between all executables that validate any consensus rules.
libmartkist_consensus_a_CPPFLAGS = $(AM_CPPFLAGS) $(MARTKIST_INCLUDES)
libmartkist_consensus_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libmartkist_consensus_a_SOURCES = \
  amount.h \
  arith_uint256.cpp \
  arith_uint256.h \
  consensus/merkle.cpp \
  consensus/merkle.h \
  consensus/params.h \
  consensus/validation.h \
  hash.cpp \
  hash.h \
  prevector.h \
  primitives/block.cpp \
  primitives/block.h \
  primitives/pureheader.cpp \
  primitives/pureheader.h \
  primitives/transaction.cpp \
  primitives/transaction.h \
  pubkey.cpp \
  pubkey.h \
  script/martkistconsensus.cpp \
  script/interpreter.cpp \
  script/interpreter.h \
  script/script.cpp \
  script/script.h \
  script/script_error.cpp \
  script/script_error.h \
  serialize.h \
  tinyformat.h \
  uint256.cpp \
  uint256.h \
  utilstrencodings.cpp \
  utilstrencodings.h \
  version.h

# common: shared between martkistd, and martkist-qt and non-server tools
libmartkist_common_a_CPPFLAGS = $(AM_CPPFLAGS) $(MARTKIST_INCLUDES)
libmartkist_common_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libmartkist_common_a_SOURCES = \
  amount.cpp \
  base58.cpp \
  bip39.cpp \
  chainparams.cpp \
  coins.cpp \
  compressor.cpp \
  core_read.cpp \
  core_write.cpp \
  hdchain.cpp \
  key.cpp \
  keystore.cpp \
  netaddress.cpp \
  netbase.cpp \
  protocol.cpp \
  scheduler.cpp \
  script/sign.cpp \
  script/standard.cpp \
  warnings.cpp \
  $(MARTKIST_CORE_H)

# util: shared between all executables.
# This library *must* be included to make sure that the glibc
# backward-compatibility objects and their sanity checks are linked.
libmartkist_util_a_CPPFLAGS = $(AM_CPPFLAGS) $(MARTKIST_INCLUDES)
libmartkist_util_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libmartkist_util_a_SOURCES = \
  support/lockedpool.cpp \
  chainparamsbase.cpp \
  clientversion.cpp \
  compat/glibc_sanity.cpp \
  compat/glibcxx_sanity.cpp \
  compat/strnlen.cpp \
  random.cpp \
  rpc/protocol.cpp \
  support/cleanse.cpp \
  sync.cpp \
  threadinterrupt.cpp \
  util.cpp \
  utilmoneystr.cpp \
  utilstrencodings.cpp \
  utiltime.cpp \
  $(MARTKIST_CORE_H)

if GLIBC_BACK_COMPAT
libmartkist_util_a_SOURCES += compat/glibc_compat.cpp
endif

# cli: shared between martkist-cli and martkist-qt
libmartkist_cli_a_CPPFLAGS = $(AM_CPPFLAGS) $(MARTKIST_INCLUDES)
libmartkist_cli_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libmartkist_cli_a_SOURCES = \
  rpc/client.cpp \
  $(MARTKIST_CORE_H)

nodist_libmartkist_util_a_SOURCES = $(srcdir)/obj/build.h
#

# martkistd binary #
martkistd_SOURCES = martkistd.cpp
martkistd_CPPFLAGS = $(AM_CPPFLAGS) $(MARTKIST_INCLUDES)
martkistd_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
martkistd_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if TARGET_WINDOWS
martkistd_SOURCES += martkistd-res.rc
endif

martkistd_LDADD = \
  $(LIBMARTKIST_SERVER) \
  $(LIBMARTKIST_COMMON) \
  $(LIBUNIVALUE) \
  $(LIBMARTKIST_UTIL) \
  $(LIBMARTKIST_WALLET) \
  $(LIBMARTKIST_ZMQ) \
  $(LIBMARTKIST_CONSENSUS) \
  $(LIBMARTKIST_CRYPTO) \
  $(LIBLEVELDB) \
  $(LIBLEVELDB_SSE42) \
  $(LIBMEMENV) \
  $(LIBSECP256K1)

martkistd_LDADD += $(BOOST_LIBS) $(BDB_LIBS) $(SSL_LIBS) $(CRYPTO_LIBS) $(MINIUPNPC_LIBS) $(EVENT_PTHREADS_LIBS) $(EVENT_LIBS) $(ZMQ_LIBS)

# martkist-cli binary #
martkist_cli_SOURCES = martkist-cli.cpp
martkist_cli_CPPFLAGS = $(AM_CPPFLAGS) $(MARTKIST_INCLUDES) $(EVENT_CFLAGS)
martkist_cli_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
martkist_cli_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if TARGET_WINDOWS
martkist_cli_SOURCES += martkist-cli-res.rc
endif

martkist_cli_LDADD = \
  $(LIBMARTKIST_CLI) \
  $(LIBUNIVALUE) \
  $(LIBMARTKIST_UTIL) \
  $(LIBMARTKIST_CRYPTO)
martkist_cli_LDADD += $(BOOST_LIBS) $(SSL_LIBS) $(CRYPTO_LIBS) $(EVENT_LIBS)
#

# martkist-tx binary #
martkist_tx_SOURCES = martkist-tx.cpp
martkist_tx_CPPFLAGS = $(AM_CPPFLAGS) $(MARTKIST_INCLUDES)
martkist_tx_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
martkist_tx_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if TARGET_WINDOWS
martkist_tx_SOURCES += martkist-tx-res.rc
endif

martkist_tx_LDADD = \
  $(LIBUNIVALUE) \
  $(LIBMARTKIST_COMMON) \
  $(LIBMARTKIST_UTIL) \
  $(LIBMARTKIST_CONSENSUS) \
  $(LIBMARTKIST_CRYPTO) \
  $(LIBSECP256K1)

martkist_tx_LDADD += $(BOOST_LIBS) $(CRYPTO_LIBS)
#

# martkistconsensus library #
if BUILD_MARTKIST_LIBS
include_HEADERS = script/martkistconsensus.h
libmartkistconsensus_la_SOURCES = $(crypto_libmartkist_crypto_a_SOURCES) $(libmartkist_consensus_a_SOURCES)

if GLIBC_BACK_COMPAT
  libmartkistconsensus_la_SOURCES += compat/glibc_compat.cpp
endif

libmartkistconsensus_la_LDFLAGS = $(AM_LDFLAGS) -no-undefined $(RELDFLAGS)
libmartkistconsensus_la_LIBADD = $(LIBSECP256K1)
libmartkistconsensus_la_CPPFLAGS = $(AM_CPPFLAGS) -I$(builddir)/obj -I$(srcdir)/secp256k1/include -DBUILD_MARTKIST_INTERNAL
libmartkistconsensus_la_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)

endif
#

CTAES_DIST =  crypto/ctaes/bench.c
CTAES_DIST += crypto/ctaes/ctaes.c
CTAES_DIST += crypto/ctaes/ctaes.h
CTAES_DIST += crypto/ctaes/README.md
CTAES_DIST += crypto/ctaes/test.c

CLEANFILES = $(EXTRA_LIBRARIES)

CLEANFILES += *.gcda *.gcno
CLEANFILES += compat/*.gcda compat/*.gcno
CLEANFILES += consensus/*.gcda consensus/*.gcno
CLEANFILES += crypto/*.gcda crypto/*.gcno
CLEANFILES += policy/*.gcda policy/*.gcno
CLEANFILES += primitives/*.gcda primitives/*.gcno
CLEANFILES += script/*.gcda script/*.gcno
CLEANFILES += support/*.gcda support/*.gcno
CLEANFILES += univalue/*.gcda univalue/*.gcno
CLEANFILES += wallet/*.gcda wallet/*.gcno
CLEANFILES += wallet/test/*.gcda wallet/test/*.gcno
CLEANFILES += zmq/*.gcda zmq/*.gcno

DISTCLEANFILES = obj/build.h

EXTRA_DIST = $(CTAES_DIST)


config/martkist-config.h: config/stamp-h1
	@$(MAKE) -C $(top_builddir) $(subdir)/$(@)
config/stamp-h1: $(top_srcdir)/$(subdir)/config/martkist-config.h.in $(top_builddir)/config.status
	$(AM_V_at)$(MAKE) -C $(top_builddir) $(subdir)/$(@)
$(top_srcdir)/$(subdir)/config/martkist-config.h.in:  $(am__configure_deps)
	$(AM_V_at)$(MAKE) -C $(top_srcdir) $(subdir)/config/martkist-config.h.in

clean-local:
	-$(MAKE) -C secp256k1 clean
	-$(MAKE) -C univalue clean
	-rm -f leveldb/*/*.gcda leveldb/*/*.gcno leveldb/helpers/memenv/*.gcda leveldb/helpers/memenv/*.gcno
	-rm -f config.h
	-rm -rf test/__pycache__

.rc.o:
	@test -f $(WINDRES)
	## FIXME: How to get the appropriate modulename_CPPFLAGS in here?
	$(AM_V_GEN) $(WINDRES) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(CPPFLAGS) -DWINDRES_PREPROC -i $< -o $@

check-symbols: $(bin_PROGRAMS)
if GLIBC_BACK_COMPAT
	@echo "Checking glibc back compat..."
	$(AM_V_at) READELF=$(READELF) CPPFILT=$(CPPFILT) $(top_srcdir)/contrib/devtools/symbol-check.py < $(bin_PROGRAMS)
endif

check-security: $(bin_PROGRAMS)
if HARDEN
	@echo "Checking binary security..."
	$(AM_V_at) READELF=$(READELF) OBJDUMP=$(OBJDUMP) $(top_srcdir)/contrib/devtools/security-check.py < $(bin_PROGRAMS)
endif

%.pb.cc %.pb.h: %.proto
	@test -f $(PROTOC)
	$(AM_V_GEN) $(PROTOC) --cpp_out=$(@D) --proto_path=$(<D) $<

if EMBEDDED_LEVELDB
include Makefile.leveldb.include
endif

if ENABLE_TESTS
include Makefile.test.include
endif

if ENABLE_BENCH
include Makefile.bench.include
endif

if ENABLE_QT
include Makefile.qt.include
endif

if ENABLE_QT_TESTS
include Makefile.qttest.include
endif
