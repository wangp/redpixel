#!/bin/sh

# Make source packages.  Got sick of repeating this process.
# 2001-12-23

VERSION=1.0
DOSVERSION=10

UNIXNAME=redpixel-$VERSION
TARBALL=$UNIXNAME.tar.gz
ZIP=redsrc$DOSVERSION.zip


if [ ! -d redpixel ]; then
	echo "Run me one level up from the Red Pixel root directory."
	exit 1
fi

if test -e .tmp || test -e $TARBALL || test -e $ZIP; then
	echo "Something is in the way, aborting"
	exit 1
fi

echo Copying
mkdir .tmp
cp -Rp redpixel .tmp/$UNIXNAME

echo Butchering
cd .tmp/$UNIXNAME
make distclean
make depend
make -C docs
rm -rf gfx
rm libnet/port.mak
rm -r misc/obsolete
rm -f music/*.{xm,mod,s3m}
rm cwsdpmi.exe

echo Making $TARBALL
cd ..  # in .tmp
tar zcf ../$TARBALL $UNIXNAME

echo Making $ZIP
mv $UNIXNAME redpixel
zip -9r ../$ZIP redpixel

cd ..  # up from .tmp
rm -r .tmp

echo All done.
