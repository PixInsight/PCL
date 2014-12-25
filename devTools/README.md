Little Helpers for PI development
=================================

This directory contains a number of small helpers that make development and git operations easier. 
Before use, you **need to adapt the piEnv script** to contain some defaults used by your installation, for instance the PI_GIT_USER_NAME.

Regarding GitHub, the tools assume that you follow the model outlined in https://help.github.com/articles/set-up-git/

The tools are currently used and  tested on Fedora21. But with minor adaptions they should also be useful on other Fedora releases, FreeBSD, or MacOS. Feel free to share the necessary changes here.

##The commands
The following tools are provided here:

piEnv: set some environment variables as required for PCL development and some of the other tools in this directory. Then launch a new shell. You exit the shell with the exit command, which returns you to the calling shell. 
       * You should adapt this script to reflect your settings, in particular PI_GIT_USER_NAME.
       * Per default, it assumes that PI is installed in $HOME/PixInsight
       * The PCL and PJSR repositories are stored in $HOME/GitHub/PCL or PJSR

piGitClone: create local clones of your GitHub repositories. Assumes you alreafy have created forks of the PixInsight repositories in your own GitHub space.

piGitSyncMerge: sync and merge with Juan's master repositories. Done in master branch.

piGitPush: sync your local repositories to your forks on Github. You can then use Github to send pull requests for integrating your changes.

piDoxygen: create detailed doxygen documentation for PCL and PJSR. Contains more detail than the public version, in particular links to sources and PJSR

piMake: build PCL, modules, file-formats from source. It creates necessary sub directories and other minor chores necessary for the build. Note that a few modules **currently have problems** as documented in the source of the script.

g++-4.8.2 and gcc-4.8.2: Wrappers for g++ and gcc, so you can compile even if you dont have these exact compiler versions on your system. They are currently hardcoded in the makefiles.

All tools are released on the terms of the PixInsight Class Library License 1.0, see http://pixinsight.com/license/PCL_PJSR_1.0.html .
