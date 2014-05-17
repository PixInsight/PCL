#!/usr/bin/env sh
echo $TRAVIS_OS_NAME
if [ "$TRAVIS_OS_NAME" = "linux" ]; 
then
 export OS_PATH="linux"
elif [ "$TRAVIS_OS_NAME" = "osx" ];
then
 export OS_PATH="macosx"
fi
echo $OS_PATH
export PCLLIBDIR64=../../PCL/lib/$OS_PATH/x64
echo $PCLLIBDIR64
cd $OS_PATH/g++/ && mkdir -p x86_64/Release  && make -f makefile-x86_64