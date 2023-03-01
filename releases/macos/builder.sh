
  WRAP_DIR=$HOME/wrapped
  CONFIGFLAGS="--enable-reduce-exports --disable-bench --disable-gui-tests GENISOIMAGE=$WRAP_DIR/genisoimage"
  FAKETIME_HOST_PROGS=""
  FAKETIME_PROGS="ar ranlib date dmg genisoimage"
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

ORIGPATH="$PATH"
# Extract the release tarball into a dir for each host and build

export PATH=${BASEPREFIX}/${HOST}/native/bin:${ORIGPATH}
mkdir -p distsrc-${HOST}
cd distsrc-${HOST}
INSTALLPATH=`pwd`/installed/${DISTNAME}
mkdir -p ${INSTALLPATH}
tar --strip-components=1 -xf ../$SOURCEDIST

CONFIG_SITE=${BASEPREFIX}/${HOST}/share/config.site ./configure --prefix=/ --disable-ccache --disable-maintainer-mode --disable-dependency-tracking --with-qt-bindir=$QTNATIVE/bin --with-qt-translationdir=$QTDIR/translations ${CONFIGFLAGS}
make ${MAKEOPTS}
make install-strip DESTDIR=${INSTALLPATH}

make deploydir
mkdir -p unsigned-app-${HOST}
cp contrib/macdeploy/detached-sig-apply.sh unsigned-app-${HOST}
cp contrib/macdeploy/detached-sig-create.sh unsigned-app-${HOST}
cp ${BASEPREFIX}/${HOST}/native/bin/dmg ${BASEPREFIX}/${HOST}/native/bin/genisoimage unsigned-app-${HOST}
cp ${BASEPREFIX}/${HOST}/native/bin/${HOST}-codesign_allocate unsigned-app-${HOST}/codesign_allocate
cp ${BASEPREFIX}/${HOST}/native/bin/${HOST}-pagestuff unsigned-app-${HOST}/pagestuff
mv dist unsigned-app-${HOST}
pushd unsigned-app-${HOST}
find . | sort | tar --no-recursion --mode='u+rw,go+r-w,a+X' --owner=0 --group=0 -c -T - | gzip -9n > ${OUTDIR}/${DISTNAME}-macos.tar.gz
popd

mkdir -p dist/Martkist-Qt.app/Contents/MacOS
cp ${BASEPREFIX}/${HOST}/bin/freechd dist/Martkist-Qt.app/Contents/MacOS
cp -r ${BASEPREFIX}/../freech-html dist/Martkist-Qt.app/Contents/MacOS/

make deploy
${WRAP_DIR}/dmg dmg Martkist-Core.dmg ${OUTDIR}/${DISTNAME}-macos.dmg

cd installed
find . -name "lib*.la" -delete
find . -name "lib*.a" -delete
rm -rf ${DISTNAME}/lib/pkgconfig
find ${DISTNAME} | sort | tar --no-recursion --mode='u+rw,go+r-w,a+X' --owner=0 --group=0 -c -T - | gzip -9n > ${OUTDIR}/${DISTNAME}-${HOST}.tar.gz
cd ../../

mkdir -p $OUTDIR/src
mv $SOURCEDIST $OUTDIR/src
mv ${OUTDIR}/${DISTNAME}-x86_64-*.tar.gz ${OUTDIR}/${DISTNAME}-macos.tar.gz
