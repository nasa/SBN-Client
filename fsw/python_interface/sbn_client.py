#
# GSC-18396-1, “Software Bus Network Client for External Process”
#
# Copyright © 2019 United States Government as represented by
# the Administrator of the National Aeronautics and Space Administration.
# No copyright is claimed in the United States under Title 17, U.S. Code.
# All Other Rights Reserved.
#
# Licensed under the NASA Open Source Agreement version 1.3
# See "NOSA GSC-18396-1.pdf"
#

import ctypes
from ctypes import *

# SB
CFE_SB_PEND_FOREVER = -1


# typedef struct {
#    uint8   StreamId[2];  /* packet identifier word (stream ID) */
#       /*  bits  shift   ------------ description ---------------- */
#       /* 0x07FF    0  : application ID                            */
#       /* 0x0800   11  : secondary header: 0 = absent, 1 = present */
#       /* 0x1000   12  : packet type:      0 = TLM, 1 = CMD        */
#       /* 0xE000   13  : CCSDS version:    0 = ver 1, 1 = ver 2    */
#    uint8   Sequence[2];  /* packet sequence word */
#       /*  bits  shift   ------------ description ---------------- */
#       /* 0x3FFF    0  : sequence count                            */
#       /* 0xC000   14  : segmentation flags:  3 = complete packet  */
#    uint8  Length[2];     /* packet length word */
#       /*  bits  shift   ------------ description ---------------- */
#       /* 0xFFFF    0  : (total packet length) - 7                 */
# } CCSDS_PriHdr_t;

# Version 2 of CCSDS has 4 extra bytes for the secondary header
# typedef struct {
#    uint8 APIDQSubsystem[2];
#    uint8 APIDQSystemId[2];
# } CCSDS_APIDqualifiers_t;

# The time format is configurable... using 2 bytes right now
# typedef struct {
#    uint8  Time[CCSDS_TIME_SIZE];
# } CCSDS_TlmSecHdr_t;

# typedef union {
#     CCSDS_PriHdr_t      Hdr;   /**< \brief CCSDS Primary Header #CCSDS_PriHdr_t */
#     CCSDS_SpacePacket_t SpacePacket;
#     uint32              Dword; /**< \brief Forces minimum of 32-bit alignment for this object */
#     uint8               Byte[sizeof(CCSDS_PriHdr_t)];   /**< \brief Allows byte-level access */
# }CFE_SB_Msg_t;

# Note: Not currently dealing with the whole union issue
class Primary_Header_t(BigEndianStructure):
    _pack_ = 1
    _fields_ = [("StreamId", c_uint16),
                ("Sequence", c_uint16),
                ("Length", c_uint16)]

class Secondary_Header_t(BigEndianStructure):
    _pack_ = 1
    _fields_ = [("Seconds", c_uint32),
                ("Subseconds", c_uint16)]

# typedef struct {
#    uint8 FunctionCode; /* Command Function Code */
#                        /* bits shift ---------description-------- */
#                        /* 0x7F  0    Command function code        */
#                        /* 0x80  7    Reserved                     */
#
#    uint8 Checksum;     /* Command checksum  (all bits, 0xFF)      */
# } CCSDS_CmdSecHdr_t;

class Command_Header_t(Structure):
    _pack_ = 1
    _fields_ = [("FunctionCode", c_uint8),
                ("Checksum", c_uint8)]

class CFE_SB_Msg_t(Structure):
    _pack_ = 1
    _fields_ = [("Primary", Primary_Header_t),
                ("Secondary", Secondary_Header_t)]

#for generic data type
# TODO Should the max message size be hardcoded or somehow taken from the mps_defs directory?
class sbn_data_generic_t(Structure):
    _pack_ = 1
    _fields_ = [("TlmHeader", CFE_SB_Msg_t),
                ("byte_array", c_ubyte * 65536)]

sbn_client = None
cmd_pipe = c_ushort()
cmd_pipe_name = create_string_buffer(b'cmd_pipe')


def print_header(message_p):
    recv_msg = message_p.contents
    print("Message Header: {} {} {}".format(hex(recv_msg.TlmHeader.Primary.StreamId),
                                            hex(recv_msg.TlmHeader.Primary.Sequence),
                                            hex(recv_msg.TlmHeader.Primary.Length)))
    print("Message Time: {} {}".format(hex(recv_msg.TlmHeader.Secondary.Seconds),
                                       hex(recv_msg.TlmHeader.Secondary.Subseconds)))

# TODO: Common file?
def cfs_error_convert (number):
    if number < 0:
        return number + (2**32)
    else:
        return number

def sbn_load_and_init():
    global sbn_client
    global cmd_pipe
    global cmd_pipe_name

    ctypes.cdll.LoadLibrary('./sbn_client.so')
    sbn_client = CDLL('sbn_client.so')
    print("SBN Client library loaded: '{}'".format(sbn_client))
    status = sbn_client.SBN_Client_Init()
    print("SBN Client init: {}".format(status))
    status = sbn_client.__wrap_CFE_SB_CreatePipe(byref(cmd_pipe), 10, cmd_pipe_name)
    print("SBN Client command pipe: {}".format(status))

def send_msg(send_msg_p):
    global sbn_client

    sbn_client.__wrap_CFE_SB_SendMsg(send_msg_p)

def recv_msg(recv_msg_p):
    global sbn_client
    global cmd_pipe

    status = sbn_client.__wrap_CFE_SB_RcvMsg(byref(recv_msg_p), cmd_pipe, CFE_SB_PEND_FOREVER)
    print("status of __wrap_CFE_SB_RcvMsg = %X" % cfs_error_convert(status))
    #recv_msg = recv_msg_p.contents
    #print("Message: {} {} {}".format(hex(recv_msg.Hdr.StreamId), hex(recv_msg.Hdr.Sequence), hex(recv_msg.Hdr.Length)))
    #print_header(recv_msg_p)

def subscribe(msgid):
    global cmd_pipe

    status = sbn_client.__wrap_CFE_SB_Subscribe(msgid, cmd_pipe)
    print("SBN Client subscribe msg (id {}): {}".format(hex(msgid), status))
