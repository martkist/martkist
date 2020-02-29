#!/bin/bash

TOPDIR=${TOPDIR:-$(git rev-parse --show-toplevel)}
BUILDDIR=${BUILDDIR:-$TOPDIR}

BINDIR=${BINDIR:-$BUILDDIR/src}
MANDIR=${MANDIR:-$TOPDIR/doc/man}

MARTKISTD=${MARTKISTD:-$BINDIR/martkistd}
MARTKISTCLI=${MARTKISTCLI:-$BINDIR/martkist-cli}
MARTKISTTX=${MARTKISTTX:-$BINDIR/martkist-tx}
MARTKISTQT=${MARTKISTQT:-$BINDIR/qt/martkist-qt}

[ ! -x $MARTKISTD ] && echo "$MARTKISTD not found or not executable." && exit 1

# The autodetected version git tag can screw up manpage output a little bit
BTCVER=($($MARTKISTCLI --version | head -n1 | awk -F'[ -]' '{ print $6, $7 }'))

# Create a footer file with copyright content.
# This gets autodetected fine for martkistd if --version-string is not set,
# but has different outcomes for martkist-qt and martkist-cli.
echo "[COPYRIGHT]" > footer.h2m
$MARTKISTD --version | sed -n '1!p' >> footer.h2m

for cmd in $MARTKISTD $MARTKISTCLI $MARTKISTTX $MARTKISTQT; do
  cmdname="${cmd##*/}"
  help2man -N --version-string=${BTCVER[0]} --include=footer.h2m -o ${MANDIR}/${cmdname}.1 ${cmd}
  sed -i "s/\\\-${BTCVER[1]}//g" ${MANDIR}/${cmdname}.1
done

rm -f footer.h2m
