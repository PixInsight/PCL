#!/usr/bin/env sh
if [$(TRAVIS_OS_NAME) -eq "linux"] 
then
 OS_PATH="linux"
elif [$(TRAVIS_OS_NAME) -eq "osx"]
then
 OS_PATH="macosx"
fi
export PCLLIBDIR64=../../PCL/lib/$(OS_PATH)/x64
cd $(OS_PATH)/g++/ && mkdir -p x86_64/Release  && make -f makefile-x86_64