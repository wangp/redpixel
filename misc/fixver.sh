#!/bin/sh

if [ "$1" = "" ]; then
	echo 'Pass a version number, e.g. 1.0'
	exit 1
fi

tmpfile=_tmp

if [ -f $tmpfile ]; then
	echo 'tmpfile already exists, abort'
	exit 1
fi

echo 'makefile.red'
sed -e "s/^VERSION.*/VERSION := $1/" makefile.red > $tmpfile
mv $tmpfile makefile.red

echo 'src/include/version.h'
sed -e "s/#define VERSION_STR.*/#define VERSION_STR	\"$1\"/" \
	src/include/version.h > $tmpfile
mv $tmpfile src/include/version.h

echo 'misc/redpixel.spec'
sed -e "s/^Version: .*/Version: $1/" misc/redpixel.spec > $tmpfile
mv $tmpfile misc/redpixel.spec

echo 'Done.'
