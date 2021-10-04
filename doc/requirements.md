# Software Bus Network Client (SBNC) Requirements

Refer to git commits for document history and revisions.

The primary requirement of the software is that it must correctly implement the SBN communication protocol.
Fulfillment of this requirement is confirmed via unit tests which are included with the source code.
Functional testing is performed with demonstration projects (not yet publicly available).

## 1. Implement SBN Protocol

SBNC shall implement the Software Bus Network (SBN) protocol for one or more of the supported modules (such as TCP / IP).
This includes subscribing, sending, and receiving messages to and from the Software Bus.

## 2. Build for cFS Applications 

SBNC shall support building and linking as a library for cFS applications that are isolated from cFS as an external process.

## 3. Build as Standalone Library

SBNC shall support building and linking as a standalone library for C applications (without linking cFS).

### 3.1 Language Support

The standalone library shall be usable by programs written in other languages such as Python through foreign function interfaces.
