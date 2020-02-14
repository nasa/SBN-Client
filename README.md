# SBN Client

Client to enable communication with SBN.

There are two versions of SBN Client available: a standalone library meant to be used by outside programs, and a library that can be linked alongside of cFE for use in a cFS application that is isolated in a process (but is otherwise identical to regular cFS applications).
The source code remains the same, but the build process is different.

## Standalone Library

This version is meant to allow an outside program to communicate with a [cFS](https://github.com/NASA/cFS) instantiation through the Software Bus, mediated by the [Software Bus Network](https://github.com/nasa/SBN). It may be used for bindings to other languages, such as Python, and does not require the rest of cFE to be linked.

To see the functions provided, see functions exported in sbn_client.h and the redefined symbols found in unwrap_sybmols.txt.

### Building

The first thing to do is edit the `Makefile` and ensure that all of the included directories are set correctly.
Headers from SBN, cFE, OSAL, and PSP are all required so that things like the message size matches between cFS and the standalone application.
In addition, cFS has several generated headers in the mission defs folder that should be linked.
Once done, running `make` should produce `sbn_client.so` which may be linked by your program.

## Process Application Library

Intended to be used by cFS applications that are isolated as separate processes.

### Building

The sbn_client directory should be located with all of the other cFS applications.
The library will be built by cFS's CMake system.

## Why We Did It This Way

There are a number of workarounds used to allow for SBN Client to be used in both environments, but ultimately these are preferable to having diverging source code.
The crux of the problem is that for process applications the library needs to be linked alongside of the rest of cFS.
Thus we have to wrap function names that we want to override for the process applications, and then unwrap those calls for standalone applications.
