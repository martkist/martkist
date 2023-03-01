UNIX BUILD NOTES
====================
Some notes on how to build Martkist Core in Unix.

(For BSD specific instructions, see [build-openbsd.md](build-openbsd.md) and/or
[build-netbsd.md](build-netbsd.md))

Note
---------------------
Always use absolute paths to configure and compile martkist and the dependencies,
for example, when specifying the path of the dependency:

```shell
../dist/configure --enable-cxx --disable-shared --with-pic --prefix=$BDB_PREFIX
```

Here `BDB_PREFIX` must be an absolute path - it is defined using `$(pwd)` which ensures
the usage of the absolute path.

To Build
---------------------

```shell
bash
./autogen.sh
./configure
make
sudo make install # optional
```


This will build martkist-qt as well if the dependencies are met.

Dependencies
---------------------

These dependencies are required:

Library     | Purpose          | Description
------------|------------------|----------------------
libssl      | Crypto           | Random Number Generation, Elliptic Curve Cryptography
libboost    | Utility          | Library for threading, data structures, etc. >= v1.55
libevent    | Networking       | OS independent asynchronous networking

Optional dependencies:

Library     | Purpose          | Description
------------|------------------|----------------------
miniupnpc   | UPnP Support     | Firewall-jumping support
libdb4.8    | Berkeley DB      | Wallet storage (only needed when wallet enabled)
qt          | GUI              | GUI toolkit (only needed when GUI enabled)
protobuf    | Payments in GUI  | Data interchange format used for payment protocol (only needed when GUI enabled)
libqrencode | QR codes in GUI  | Optional for generating QR codes (only needed when GUI enabled)
univalue    | Utility          | JSON parsing and encoding (bundled version will be used unless --with-system-univalue passed to configure)
libzmq3     | ZMQ notification | Optional, allows generating ZMQ notifications (requires ZMQ version >= v4.x)

For the versions used, see [dependencies.md](dependencies.md)

Memory Requirements
--------------------

C++ compilers are memory-hungry. It is recommended to have at least 1.5 GB of
memory available when compiling Martkist Core. On systems with less, gcc can be
tuned to conserve memory with additional CXXFLAGS:


```shell
./configure CXXFLAGS="--param ggc-min-expand=1 --param ggc-min-heapsize=32768"
```

## Linux Distribution Specific Instructions

[Ubuntu & Debian](#ubuntu--debian) | [Fedora](#fedora) | [CentOS](#centos) | [Arch Linux](#setup-and-build-example-arch-linux) | [ARM](#arm-cross-compilation) | [FreeBSD](#building-on-freebsd)

#### BerkeleyDB is required for the wallet.

Some Linux distributions have their own `libdb-dev` and `libdb++-dev` packages which install
BerkeleyDB 5.1 or later and break binary wallet compatibility with the distributed executables 
as they are based on BerkeleyDB 4.8. If you do not care about wallet compatibility, pass 
`--with-incompatible-bdb` to configure. See the section "Disable-wallet mode" to build Martkist 
Core without wallet.


### Ubuntu & Debian

#### Dependency Build Instructions

##### Build requirements:

```shell
sudo apt-get install -y git build-essential libtool autotools-dev automake pkg-config libssl-dev libevent-dev bsdmainutils python3
```

Options when installing required Boost library files:

1. On Ubuntu 14.04+ and Debian 7+ you can install individual boost development packages:

```shell
sudo apt-get install -y libboost-system-dev libboost-filesystem-dev libboost-chrono-dev libboost-program-options-dev libboost-test-dev libboost-thread-dev libboost-graph-dev libboost-regex-dev
```

2. Alternatively you can install all boost development packages with:

```shell
sudo apt-get install -y libboost-all-dev
```

##### BerkeleyDB 4.8

>**Ubuntu:** db4.8 packages are available [here](https://launchpad.net/~martkist/+archive/martkist).
>You can add the repository and install using the following commands:

```shell
sudo apt-get install -y software-properties-common
sudo add-apt-repository -y ppa:bitcoin/bitcoin
sudo apt-get update
sudo apt-get install -y libdb4.8-dev libdb4.8++-dev
```
    
**Debian:** you must compile [Berkeley DB](#berkeley-db) from source using the provided contrib script.

##### MiniUPNPC / Optional (see --with-miniupnpc and --enable-upnp-default):

```shell
sudo apt-get install -y libminiupnpc-dev
```

##### ZMQ dependencies / Optional:

```shell
sudo apt-get install -y libzmq3-dev
```

#### Dependencies for the GUI

If you want to build Martkist-Qt, make sure that the required packages for Qt development are 
installed. Either Qt 5 or Qt 4 are necessary to build the GUI. If both Qt 4 and Qt 5 are installed,
Qt 5 will be used. Pass `--with-gui=qt4` to configure to choose Qt4. To build without GUI pass 
`--without-gui`.

To build with Qt 5 (recommended) you need the following:

```shell
sudo apt-get install -y libqt5gui5 libqt5core5a libqt5webenginewidgets5 libqt5dbus5 qttools5-dev qttools5-dev-tools qtwebengine5-dev libprotobuf-dev protobuf-compiler
```

Alternatively, to build with Qt 4 you need the following:

```shell
sudo apt-get install -y libqt4-dev libprotobuf-dev protobuf-compiler
```

libqrencode (optional) can be installed with:

```shell
sudo apt-get install -y libqrencode-dev
```

Once these are installed, they will be found by configure and a martkist-qt executable will be
built by default.


### Fedora

#### Dependency Build Instructions

##### Build requirements:

```shell
sudo dnf install -y git gcc-c++ libtool make autoconf automake openssl-devel libevent-devel boost-devel python3
```

##### BerkeleyDB 4.8:

```shell
sudo dnf install -y libdb4-devel libdb4-cxx-devel 

```
##### MiniUPNP / Optional:

```shell
sudo dnf install -y miniupnpc-devel
```

##### ZMQ dependencies / Optional:

```shell
sudo dnf install -y czmq-devel
```

#### Dependencies for the GUI

To build with Qt 5 (recommended) you need the following:

```shell
sudo dnf install -y qt5-qttools-devel qt5-qtbase-devel protobuf-devel
```

libqrencode (optional) can be installed with:

```shell
sudo dnf install -y qrencode-devel
```


### CentOS

#### Dependency Build Instructions

##### Build requirements:

```shell
sudo yum install -y git gcc-c++ libtool make autoconf automake epel-release openssl-devel libevent-devel boost-devel libdb4-devel libdb4-cxx-devel python3 python-devel patch
```

##### Boost:

The version of Boost that ships with CentOS is too old to work with Martkist - version 1.55+ 
is required. You will also need to pass the argument `--with-boot-libdir="/usr/local/lib"` 
to `./configure`. You may want to consider adding `export LD_LIBRARY_PATH="/usr/local/lib"` 
to your `.bashrc` as well to configure this variable on when starting `martkistd`.


```shell
curl -sL https://dl.bintray.com/boostorg/release/1.65.1/source/boost_1_65_1.tar.gz -o boost_1_65_1.tar.gz
tar -xvf boost_1_65_1.tar.gz && cd boost_1_65_1
./bootstrap.sh
sudo ./b2 cxxflags=-fPIC -a --with=all -j$(nproc) toolset=gcc link=static runtime-link=static install
```

##### BerkeleyDB 4.8:


```shell
# berkeleydb 4.8 (requires epel-release)
sudo yum install -y libdb4-devel libdb4-cxx-devel
```

##### MiniUPNPC / Optional:

```shell
# upnp (requires epel-release)
sudo yum install -y miniupnpc-devel
```

##### ZMQ dependencies:

```shell
# zmq (requires epel-release)
sudo yum install -y czmq-devel
```

#### Dependencies for the GUI

To build with Qt 5 (recommended) you need the following:

```shell
sudo yum install -y qt5-qttools-devel qt5-qtbase-devel protobuf-devel
```

libqrencode (optional) can be installed with:

```shell
sudo yum install -y qrencode-devel
```


Notes
-----
The release is built with GCC and then "strip martkistd" to strip the debug symbols, 
which reduces the executable size by about 90%.


miniupnpc
---------

[miniupnpc](http://miniupnp.free.fr/) may be used for UPnP port mapping.  It can be 
downloaded from [here](http://miniupnp.tuxfamily.org/files/).  UPnP support is com piled 
in and turned off by default.  See the configure options for upnp behavior desired:

    --without-miniupnpc      No UPnP support miniupnp not required
    --disable-upnp-default   (the default) UPnP support turned off by default at runtime
    --enable-upnp-default    UPnP support turned on by default at runtime


Berkeley DB
-----------
It is recommended to use Berkeley DB 4.8. If you have to build it yourself, you can use 
the installation script included in [contrib/install_db4.sh](/contrib/install_db4.sh) like 
so from the root of the Martkist repository.

```shell
./contrib/install_db4.sh `pwd`
```

When compiling Martkist you will need to run `./configure` with the results of this script.

**Note**: You only need Berkeley DB if the wallet is enabled (see the section *Disable-Wallet mode* below).

Boost
-----
If you need to build Boost yourself:

```shell
./bootstrap.sh
sudo ./bjam install
```

Security
--------
To help make your martkist installation more secure by making certain attacks impossible to
exploit even if a vulnerability is found, binaries are hardened by default.
This can be disabled with:

Hardening Flags:
```shell
./configure --enable-hardening
./configure --disable-hardening
```

Hardening enables the following features:

* Position Independent Executable

  Build position independent code to take advantage of Address Space Layout Randomization
  offered by some kernels. Attackers who can cause execution of code at an arbitrary memory
  location are thwarted if they don't know where anything useful is located.
  The stack and heap are randomly located by default but this allows the code section to be
  randomly located as well.

  On an AMD64 processor where a library was not compiled with -fPIC, this will cause an error
  such as: `relocation R_X86_64_32 against '......' can not be used when making a shared object;`

  To test that you have built PIE executable, install scanelf, part of paxutils, and use:

  ```shell
  scanelf -e ./martkist
  ```

  The output should contain:

  ```shell
  TYPE
  ET_DYN
  ```

* Non-executable Stack

  If the stack is executable then trivial stack based buffer overflow exploits are possible if
  vulnerable buffers are found. By default, martkist should be built with a non-executable stack
  but if one of the libraries it uses asks for an executable stack or someone makes a mistake
  and uses a compiler extension which requires an executable stack, it will silently build an 
  executable without the non-executable stack protection.

  To verify that the stack is non-executable after compiling use:

  ```shell
  scanelf -e ./martkist
  ```

  The output should contain:

  ```shell
  STK/REL/PTL
  RW- R-- RW-
  ```

  The STK RW- means that the stack is readable and writeable but not executable.


Disable-wallet mode
--------------------
When the intention is to run only a P2P node without a wallet, martkist may be compiled in 
disable-wallet mode with:

```shell
./configure --disable-wallet
```

In this case there is no dependency on Berkeley DB 4.8.

Mining is also possible in disable-wallet mode, but only using the `getblocktemplate` RPC 
call not `getwork`.


Additional Configure Flags
--------------------------
A list of additional configure flags can be displayed with:

```shell
./configure --help
```


Setup and Build Example: Arch Linux
-----------------------------------
This example lists the steps necessary to setup and build a command line only, non-wallet 
distribution of the latest changes on Arch Linux:

```shell
pacman -S git base-devel boost libevent python
git clone https://github.com/martkist/martkist.git
cd martkist/
./autogen.sh
./configure --disable-wallet --without-gui --without-miniupnpc
make
```

Note:
Enabling wallet support requires either compiling against a Berkeley DB newer than 4.8 
(package `db`) using `--with-incompatible-bdb`, or building and depending on a local 
version of Berkeley DB 4.8. The readily available Arch Linux packages are currently built 
using `--with-incompatible-bdb` according to the 
[PKGBUILD](https://projects.archlinux.org/svntogit/community.git/tree/martkist/trunk/PKGBUILD). 
As mentioned above, when maintaining portability of the wallet between the standard Martkist 
Core distributions and independently built node software is desired, Berkeley DB 4.8 must 
be used.


ARM Cross-compilation
-------------------
These steps can be performed on, for example, an Ubuntu VM. The depends system will also work 
on other Linux distributions, however the commands for installing the toolchain will be different.

Make sure you install the build requirements mentioned above. Then install the toolchain and curl:

```shell
sudo apt-get install -y g++-arm-linux-gnueabihf curl
```

To build executables for ARM:

```shell
cd depends
make HOST=arm-linux-gnueabihf NO_QT=1
cd ..
./configure --prefix=$PWD/depends/arm-linux-gnueabihf --enable-glibc-back-compat --enable-reduce-exports LDFLAGS=-static-libstdc++
make
```

For further documentation on the depends system see [README.md](../depends/README.md) in the 
depends directory.


Building on FreeBSD
--------------------

(Updated as of FreeBSD 11.0)

Clang is installed by default as `cc` compiler, this makes it easier to get started than on [
OpenBSD](build-openbsd.md). Installing dependencies:

```shell
pkg install autoconf automake libtool pkgconf
pkg install boost-libs openssl libevent
pkg install gmake
```

You need to use GNU make (`gmake`) instead of `make`. (`libressl` instead of `openssl` will also work)

For the wallet (optional):


```shell
./contrib/install_db4.sh `pwd`
setenv BDB_PREFIX $PWD/db4
```

Then build using:

```shell
./autogen.sh
./configure --disable-wallet # OR
./configure BDB_CFLAGS="-I${BDB_PREFIX}/include" BDB_LIBS="-L${BDB_PREFIX}/lib -ldb_cxx"
gmake
```

*Note on debugging*: The version of `gdb` installed by default is [ancient and considered harmful](https://wiki.freebsd.org/GdbRetirement). 
It is not suitable for debugging a multi-threaded C++ program, not even for getting backtraces. Please 
install the package `gdb` and use the versioned gdb command e.g. `gdb7111`.
