
  WRAP_DIR=$HOME/wrapped
  CONFIGFLAGS="--enable-glibc-back-compat --enable-reduce-exports --with-libs=yes"
  FAKETIME_HOST_PROGS=""
  FAKETIME_PROGS="ar ranlib date dmg genisoimage"
  HOST_CFLAGS="-O2 -g"
  HOST_CXXFLAGS="-O2 -g"
  HOST_LDFLAGS=-static-libstdc++
  REFERENCE_DATETIME="2018-01-01 00:00:00"
  REFERENCE_DATE="2018-01-01"
  REFERENCE_TIME="00:00:00"
  export QT_RCC_TEST=1
  export GZIP="-9n"
  export TAR_OPTIONS="--mtime="$REFERENCE_DATE\\\ $REFERENCE_TIME""
  export TZ="UTC"
  export BUILD_DIR=`pwd`
  mkdir -p ${WRAP_DIR}

  export ZERO_AR_DATE=1

  # Create global faketime wrappers
  for prog in ${FAKETIME_PROGS}; do
    echo '#!/bin/bash' > ${WRAP_DIR}/${prog}
    echo "REAL=\`which -a ${prog} | grep -v ${WRAP_DIR}/${prog} | head -1\`" >> ${WRAP_DIR}/${prog}
    echo 'export LD_PRELOAD=/usr/lib/x86_64-linux-gnu/faketime/libfaketime.so.1' >> ${WRAP_DIR}/${prog}
    echo "export FAKETIME=\"${REFERENCE_DATETIME}\"" >> ${WRAP_DIR}/${prog}
    echo "\$REAL \$@" >> $WRAP_DIR/${prog}
    chmod +x ${WRAP_DIR}/${prog}
  done

  # Create per-host faketime wrappers
for prog in ${FAKETIME_HOST_PROGS}; do
    echo '#!/bin/bash' > ${WRAP_DIR}/${HOST}-${prog}
    echo "REAL=\`which -a ${HOST}-${prog} | grep -v ${WRAP_DIR}/${HOST}-${prog} | head -1\`" >> ${WRAP_DIR}/${HOST}-${prog}
    echo 'export LD_PRELOAD=/usr/lib/x86_64-linux-gnu/faketime/libfaketime.so.1' >> ${WRAP_DIR}/${HOST}-${prog}
    echo "export FAKETIME=\"${REFERENCE_DATETIME}\"" >> ${WRAP_DIR}/${HOST}-${prog}
    echo "\$REAL \$@" >> $WRAP_DIR/${HOST}-${prog}
    chmod +x ${WRAP_DIR}/${HOST}-${prog}
done
export PATH=${WRAP_DIR}:${PATH}
  
  create_global_faketime_wrappers "${REFERENCE_DATETIME}"
  create_per-host_faketime_wrappers "${REFERENCE_DATETIME}"

# Create the release tarball using (arbitrarily) the first host
./autogen.sh
CONFIG_SITE=${HOSTPREFIX}/share/config.site ./configure --prefix=/
make dist
SOURCEDIST=`echo martkistcore-*.tar.gz`
DISTNAME=`echo ${SOURCEDIST} | sed 's/.tar.*//'`

# Correct tar file order
mkdir -p temp
pushd temp
tar xf ../$SOURCEDIST
find martkistcore-* | sort | tar --no-recursion --mode='u+rw,go+r-w,a+X' --owner=0 --group=0 -c -T - | gzip -9n > ../$SOURCEDIST
popd

  # Workaround for tarball not building with the bare tag version (prep)
  make -C src obj/build.h

ORIGPATH="$PATH"
# Extract the release tarball into a dir for each host and build

export PATH=${BASEPREFIX}/${HOST}/native/bin:${ORIGPATH}
mkdir -p distsrc-${HOST}
cd distsrc-${HOST}
INSTALLPATH=`pwd`/installed/${DISTNAME}
mkdir -p ${INSTALLPATH}
tar --strip-components=1 -xf ../$SOURCEDIST

    # Workaround for tarball not building with the bare tag version
    echo '#!/bin/true' >share/genbuild.sh
    mkdir src/obj
    cp ../src/obj/build.h src/obj/

CONFIG_SITE=${BASEPREFIX}/${HOST}/share/config.site ./configure --prefix=/ --disable-ccache --disable-maintainer-mode --disable-dependency-tracking  --with-qt-bindir=${QTNATIVE}/bin --with-qt-translationdir=${QTDIR}/translations --with-gui=qt5 --with-qt-incdir=${QTDIR}/include --with-qt-libdir=${QTDIR}/lib --with-qt-plugindir=${QTDIR}/plugins ${CONFIGFLAGS} CFLAGS="${HOST_CFLAGS}" CXXFLAGS="${HOST_CXXFLAGS}" LDFLAGS="${HOST_LDFLAGS}"
make ${MAKEOPTS}
make ${MAKEOPTS} -C src check-security

make install DESTDIR=${INSTALLPATH}
cd installed

find . -name "lib*.la" -delete
find . -name "lib*.a" -delete
find . -name "*.dbg.*" -delete
rm -rf ${DISTNAME}/lib/pkgconfig

mv ${DISTNAME}/bin/* ${INSTALLPATH}/
rmdir ${DISTNAME}/bin
mv ${DISTNAME}/lib/* ${INSTALLPATH}/
rmdir ${DISTNAME}/lib
cp ${BASEPREFIX}/${HOST}/bin/freechd ${INSTALLPATH}/
cp -r ${BASEPREFIX}/../freech-html ${INSTALLPATH}/

xargs -a /martkist/releases/linux/qtso.txt cp -L -t ${INSTALLPATH}/
cp -r ${QTDIR}/plugins/imageformats ${INSTALLPATH}/
cp -r ${QTDIR}/plugins/platforms ${INSTALLPATH}/
cp -r ${QTDIR}/plugins/wayland-shell-integration ${INSTALLPATH}/
cp /martkist/releases/linux/martkist-qt.sh ${INSTALLPATH}/
chmod +x ${INSTALLPATH}/martkist-qt.sh

find ${DISTNAME} -not -name "*.so.*" -type f -executable -exec ../contrib/devtools/split-debug.sh {} {} {}.dbg \;
find ${DISTNAME} -not -name "*.so.*" -type f -exec ../contrib/devtools/split-debug.sh {} {} {}.dbg \;
find ${DISTNAME} -not -name "*.dbg" | sort | tar --no-recursion --mode='u+rw,go+r-w,a+X' --owner=0 --group=0 -c -T - | gzip -9n > ${OUTDIR}/${DISTNAME}-${HOST}.tar.gz
find ${DISTNAME} -name "*.dbg" | sort | tar --no-recursion --mode='u+rw,go+r-w,a+X' --owner=0 --group=0 -c -T - | gzip -9n > ${OUTDIR}/${DISTNAME}-${HOST}-debug.tar.gz
cd ../../

  mkdir -p $OUTDIR/src
  mv $SOURCEDIST $OUTDIR/src