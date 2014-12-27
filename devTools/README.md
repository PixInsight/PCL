Little Helpers for PI Development
=================================

This directory contains a number of small helpers that make development and git operations easier. 
Before use, you **need to adapt the piEnv script** to contain some defaults used by your installation, for instance the PI_GIT_USER_NAME.

Regarding GitHub, the tools assume that you follow the model outlined in https://help.github.com/articles/set-up-git/ .

The tools are currently used and  tested on Fedora 21. But with minor adaptions they should also be useful on other Linux releases, FreeBSD, or MacOS. Feel free to share the necessary changes here.

##Commands
The following tools are provided:

* piEnv: set some environment variables as required for PCL development and some of the other tools in this directory. Then launch a new shell. You exit the shell with the exit command, which returns you to the calling shell. 
       * You should adapt this script to reflect your settings, in particular PI_GIT_USER_NAME.
       * Per default, it assumes that PI is installed in $HOME/PixInsight
       * The PCL and PJSR repositories are stored in $HOME/GitHub/PCL or PJSR

* piGitClone: create local clones of your GitHub repositories. Assumes you already have created forks of the PixInsight repositories in your own GitHub space. Will also set the upstream repository to the official PixInsight repositories, so that piGitSyncMerge will get the current master branch from there.

* piGitSyncMerge: sync and merge with Juan's master repositories. Done in master branch.

* piGitPush: sync your local repositories to your forks on Github. You can then use Github to send pull requests for integrating your changes.

* piDoxygen: create detailed doxygen documentation for PCL and PJSR. Contains more detail than the public version, in particular links to sources and PJSR scripts.

* piMake: build PCL, modules and file-formats from source. It creates sub-directories and other minor chores necessary for the build. Note that a few modules **currently have build problems** as documented in the source of the script.

* piMakeClean: "make clean" for everything.

* g++-4.8.2 and gcc-4.8.2: Wrappers for g++ and gcc, so you can compile even if you dont have these exact compiler versions on your system. Version 4.8.2 are currently hardcoded in the makefiles, while Fedora 21 provides 4.9.2 . Of course, you are using a compiler different from the "official" version at your own risk. You can select between the system compiler and your private version of gcc4.8.2 by editing piEnv.

## License

All tools are released under the terms of the PixInsight Class Library License 1.0, see http://pixinsight.com/license/PCL_PJSR_1.0.html .
