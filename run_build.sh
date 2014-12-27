#!/usr/bin/env sh
echo $TRAVIS_OS_NAME
if [ "$TRAVIS_OS_NAME" = "linux" ]; 
then
 export OS_PATH="linux"
 echo "Staring: tar --warning=no-unknown-keyword -xzf PCL-02.00.13.0689-20141030.tar.gz ..."
 tar --warning=no-unknown-keyword -xzf PCL-02.00.13.0689-20141030.tar.gz
 echo "done"
elif [ "$TRAVIS_OS_NAME" = "osx" ];
then
 export OS_PATH="macosx"
 echo "Starting: gunzip PCL-02.00.13.0689-20141030.tar.gz..."
 echo "Starting: tar -xf PCL-02.00.13.0689-20141030.tar.gz..."
 gunzip PCL-02.00.13.0689-20141030.tar.gz
 tar -xf PCL-02.00.13.0689-20141030.tar
 echo "done"
fi
echo $OS_PATH
export PCLLIBDIR64=../../PCL/lib/$OS_PATH/x64
echo $PCLLIBDIR64
pwd
cd module/$OS_PATH/g++/ && mkdir -p x64/Release  && make -f makefile-x64