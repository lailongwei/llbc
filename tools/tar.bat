@echo off

Rem argument_0: indicate tarball .git directory or not.
Rem             <no_input>/True: tarball .git directory
Rem             <other>: not tarball .git directory

py.exe tar.py %1
