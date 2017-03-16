#!/bin/sh

INSTALL_PREFIX=$1
if [ -z "$INSTALL_PREFIX" ]; then
	echo "Usage: $0 <install_prefix>"
	exit
fi

update-desktop-database $INSTALL_PREFIX/share/applications
update-mime-database $INSTALL_PREFIX/share/mime

