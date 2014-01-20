#! /bin/bash

if [ "$(id -u)" != "0" ]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

echo "getpass - Universal Password Retrieval Utility"
echo "Author: Timothy Patterson (tim@pc-professionals.net)"
echo "Version: 2.1   (2010-07-28)"
echo "----------------------------------------------"
echo ""
echo "This program is designed to work with SLES 10 & Up. It has not"
echo "been tested on other distros or on OpenSuSE (though I don't see"
echo "why it would not work with OpenSuSE...)"
echo ""
echo "SOFTWARE REQUIREMENTS:"
echo "This program requires the cc compiler and make."
echo "For best results, install the C/C++ Compiler and Tools pattern"
echo "in YaST's Software Management module."
echo ""
echo "Additionally, the libopenssl-devel package is required. This"
echo "can be found on the SLES11 SDK repository. (openssl-dev on SLES10)"
echo ""
echo "PASSWORD POLICY REQUIREMENTS:"
echo "This program needs to be executed with a user that is authorized"
echo "to retrieve a user's password. This is set in your Universal"
echo "Password Policy. Use iManager to configure this."
echo ""
echo -n "Does your environment meet these requirements? [Y/n]: "
read -e REQUIREMENTS
echo ""

if [ "$REQUIREMENTS" == "" ]; then
   REQUIREMENTS="Y"
fi

if [ `echo $REQUIREMENTS | tr '[:lower:]' '[:upper:]'` != "Y" ]; then
   echo "Requirements not met, exiting."
   exit 1
fi

echo "Compiling getpass..."
echo ""

make
if [ "$?" -ne "0" ]; then
   echo "ERROR encountered during compilation! getpass NOT installed!"
   exit 1
fi

echo -n "Choose an installation directory [default: /usr/local/sbin/getpass]: "
read -e INSTALLPATH
echo ""

if [ "$INSTALLPATH" == "" ]; then
   INSTALLPATH="/usr/local/sbin/getpass"
fi

if [ ! -d "INSTALLPATH" ]; then
   echo "$INSTALLPATH does not exist... Creating directories."
   mkdir -p $INSTALLPATH   
fi

echo "Copying getpass to $INSTALLPATH/getpass..."
cp ./getpass $INSTALLPATH

if [ `uname -m` == "x86_64" ]; then
   LIBPATH="lib64"
else
   LIBPATH="lib"
fi

echo "Copying libraries to $INSTALLPATH/$LIBPATH..."
cp -r ./$LIBPATH $INSTALLPATH

echo "Creating /etc/ld.so.conf.d/getpass.conf..."
echo "$INSTALLPATH/$LIBPATH" > /etc/ld.so.conf.d/getpass.conf
ldconfig >/dev/null 2>&1  # Ignores the warnings...

echo "Installation Complete - Running $INSTALLPATH/getpass"

echo ""
$INSTALLPATH/getpass
