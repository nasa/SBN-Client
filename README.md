# SBN Client

The Software Bus Network Client (SBNC) communicates with NASA's core Flight System (cFS) Software Bus Network (SBN) application.
Its main purpose it to facilitate two-way communication from the cFS Software Bus service to an external application (i.e. an application that is not a child task of cFS).
SBNC implements the SBN communication protocol and provides access to the Software Bus through a standalone C library.

We also support using SBNC within existing cFS applications through a modified build system and function wrappers.
The function wrappers allow the source code of the library to remain unchanged while optionally linking to cFS's libraries.
This configuration allows the cFS application to be isolated in an OS process which provides memory isolation from the rest of cFS.

SBNC is intended to support the rapid development of software concepts for future flight software and technology demonstrations.
It is not intended to be used in flight software.

## Compatible cFS and SBN Version

Currently works with the cFS integration candidate from 2020-05-27: [github.com/nasa/cFS](https://github.com/nasa/cFS/tree/da695db7daaf3ca417662b81c5b9ea48c67be78f), which uses cFE 6.7.19 [github.com/nasa/cFE](https://github.com/nasa/cFE/tree/ad2190af9a2b40c9b9d0f3b88f601e931ad4059d).

Tested with SBN 1.11.0, specifically: [github.com/nasa/SBN](https://github.com/nasa/SBN/tree/ea45ea4a075b1e28e0fd3413c63a3ad6ce57aba0).

## Configuration

Configuration is set by defines in [`sbn_client_defs.h`](./fsw/src/sbn_client_defs.h).
For now, this is only the IP address and port used by SBN, which should match that in `sbn_conf_tbl.c`.

## Standalone Library

This version is meant to allow an outside program to communicate with a [cFS](https://github.com/NASA/cFS) instantiation through the Software Bus, mediated by the [Software Bus Network](https://github.com/nasa/SBN). It may be used for bindings to other languages, such as Python, and does not require the rest of cFE to be linked.

The SBN communicates with sbn_client via TCP/IP with the port and IP address set in the sbn_client_defs.h file. These may be updated for the user's particular instance.   

The functions provided are exported in sbn_client.h and the redefined symbols can be found in unwrap_sybmols.txt.

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

## License and Copyright

Please refer to [NOSA GSC-188396-1.pdf](NOSA%20GSC-18396-1.pdf) and [COPYRIGHT](COPYRIGHT).
