'''
Created on Dec 15, 2017

@author: asahay
'''

import logging
import struct

from QFLOGPacketPackage.QFLOGPacket import QFLOGPacket
from QFLOGPacketPackage.QFLOGPacketConsts import QFLOG_PKT_CFG
from QFLOGLoggerPackage.QFLOGLoggerConfig import QFLOG_LOGGER_CONFIG

logger = logging.getLogger(QFLOG_LOGGER_CONFIG['LOGGER'])

class QFLOGPacketSerDes(object):
    '''
    classdocs
    '''
    
    def __init__(self):
        '''
        Constructor
        '''
        raise(TypeError("Attempting to instantiate a non-instantiable class"))
    
    @classmethod    
    def serialize(self, qflogPkt):
        
        ctxId = qflogPkt.getCtxId()
        cmd = qflogPkt.getCmd()
        pktId = qflogPkt.getPktId()
        totSize = qflogPkt.getTotSize()
        payloadLen = qflogPkt.getPayloadLen()
        pad = qflogPkt.getPad()
        payload = qflogPkt.getPayload()
        
        logger.debug("ctxId: " + ctxId.__str__())
        logger.debug("cmd: " + cmd.__str__())
        logger.debug("pktId: " + pktId.__str__())
        logger.debug("totSize: " + totSize.__str__())
        logger.debug("payloadLen: " + payloadLen.__str__())
        logger.debug("pad: " + pad.__str__())
        # logger.debug("payload: " + payload.__str__())
        logger.debug("payload: " + "\\x" + ("\\x".join(format(byte, '02x') for byte in bytearray(payload))))
        
        '''
        serQFLOGPkt = ctxId.to_bytes(QFLOG_PKT_CFG['CTX_ID_LEN'], QFLOG_PKT_CFG['BYTE_ORDER'], signed = True) + \
                    cmd.to_bytes(QFLOG_PKT_CFG['CMD_LEN'], QFLOG_PKT_CFG['BYTE_ORDER'], signed = True) + \
                    pktId.to_bytes(QFLOG_PKT_CFG['PKT_ID_LEN'], QFLOG_PKT_CFG['BYTE_ORDER'], signed = True) + \
                    totSize.to_bytes(QFLOG_PKT_CFG['TOT_SIZE_LEN'], QFLOG_PKT_CFG['BYTE_ORDER'], signed = True) + \
                    payloadLen.to_bytes(QFLOG_PKT_CFG['PAYLOAD_LEN_LEN'], QFLOG_PKT_CFG['BYTE_ORDER'], signed = True) + \
                    pad.to_bytes(QFLOG_PKT_CFG['PAD_LEN'], QFLOG_PKT_CFG['BYTE_ORDER'], signed = True) + \
                    payload
                    
        logger.debug("serPkt: " + serQFLOGPkt.__str__())
        '''

        serQFLOGPkt = struct.pack('<bbhihH', ctxId, cmd, pktId, totSize, payloadLen, pad) + \
                    payload
        
        serQFLOGPktByteArray = "\\x" + ("\\x".join(format(byte, '02x') for byte in bytearray(serQFLOGPkt)))
        
        logger.debug("serQFLOGPkt: " + serQFLOGPktByteArray)
        
        return(serQFLOGPkt)
    
    @classmethod
    def deserialize(self, byteArray):
        
        bCtxId = byteArray[QFLOG_PKT_CFG['CTX_ID_OFFSET']:(QFLOG_PKT_CFG['CTX_ID_OFFSET'] + QFLOG_PKT_CFG['CTX_ID_LEN'])]
        bCmd = byteArray[QFLOG_PKT_CFG['CMD_OFFSET']:(QFLOG_PKT_CFG['CMD_OFFSET'] + QFLOG_PKT_CFG['CMD_LEN'])]
        bPktId = byteArray[QFLOG_PKT_CFG['PKT_ID_OFFSET']:(QFLOG_PKT_CFG['PKT_ID_OFFSET'] + QFLOG_PKT_CFG['PKT_ID_LEN'])]
        bTotSize = byteArray[QFLOG_PKT_CFG['TOT_SIZE_OFFSET']:(QFLOG_PKT_CFG['TOT_SIZE_OFFSET'] + QFLOG_PKT_CFG['TOT_SIZE_LEN'])]
        bPayloadLen = byteArray[QFLOG_PKT_CFG['PAYLOAD_LEN_OFFSET']:(QFLOG_PKT_CFG['PAYLOAD_LEN_OFFSET'] + QFLOG_PKT_CFG['PAYLOAD_LEN_LEN'])]
        bPad = byteArray[QFLOG_PKT_CFG['PAD_OFFSET']:(QFLOG_PKT_CFG['PAD_OFFSET'] + QFLOG_PKT_CFG['PAD_LEN'])]
        bPayload = byteArray[QFLOG_PKT_CFG['PAYLOAD_OFFSET']:]
        
        if (not bCtxId):
            bCtxId = b'\x00'
            
        if (not bCmd):
            bCmd = b'\x00'
            
        if (not bPktId):
            bPktId = b'\x00\x00'
            
        if (not bTotSize):
            bTotSize = b'\x00\x00\x00\x00'
            
        if (not bPayloadLen):
            bPayloadLen = b'\x00\x00'
            
        if (not bPad):
            bPad = b'\x00\x00'
            
        if (not bPayload):
            bPayload = b'\x00'
        
        '''
        logger.debug("bCtxId: " + bCtxId.__str__())
        logger.debug("bCmd: " + bCmd.__str__())
        logger.debug("bPktId: " + bPktId.__str__())
        logger.debug("bTotSize: " + bTotSize.__str__())
        logger.debug("bPayloadLen: " + bPayloadLen.__str__())
        logger.debug("bPad: " + bPad.__str__())
        logger.debug("bPayload: " + bPayload.__str__())
        '''

        logger.debug("bCtxId: " + "\\x" + ("\\x".join(format(byte, '02x') for byte in bytearray(bCtxId))))
        logger.debug("bCmd: " + "\\x" + ("\\x".join(format(byte, '02x') for byte in bytearray(bCmd))))
        logger.debug("bPktId: " + "\\x" + ("\\x".join(format(byte, '02x') for byte in bytearray(bPktId))))
        logger.debug("bTotSize: " + "\\x" + ("\\x".join(format(byte, '02x') for byte in bytearray(bTotSize))))
        logger.debug("bPayloadLen: " + "\\x" + ("\\x".join(format(byte, '02x') for byte in bytearray(bPayloadLen))))
        logger.debug("bPad: " + "\\x" + ("\\x".join(format(byte, '02x') for byte in bytearray(bPad))))
        logger.debug("bPayload: " + "\\x" + ("\\x".join(format(byte, '02x') for byte in bytearray(bPayload))))
        
        '''
        deserQFLOGPkt = QFLOGPacket(ctxId = int.from_bytes(bCtxId, QFLOG_PKT_CFG['BYTE_ORDER'], signed = True),
                          cmd = int.from_bytes(bCmd, QFLOG_PKT_CFG['BYTE_ORDER'], signed = True),
                          pktId = int.from_bytes(bPktId, QFLOG_PKT_CFG['BYTE_ORDER'], signed = True),
                          totSize = int.from_bytes(bTotSize, QFLOG_PKT_CFG['BYTE_ORDER'], signed = True),
                          payloadLen = int.from_bytes(bPayloadLen, QFLOG_PKT_CFG['BYTE_ORDER'], signed = True),
                          pad = int.from_bytes(bPad, QFLOG_PKT_CFG['BYTE_ORDER'], signed = True),
                          payload = bPayload)
        '''

        deserQFLOGPkt = QFLOGPacket(ctxId = struct.unpack("<b", bCtxId)[0],
                          cmd = struct.unpack("<b", bCmd)[0],
                          pktId = struct.unpack("<h", bPktId)[0],
                          totSize = struct.unpack("<i", bTotSize)[0],
                          payloadLen = struct.unpack("<h", bPayloadLen)[0],
                          pad = struct.unpack("<H", bPad)[0],
                          payload = bPayload)
        
        logger.debug("deserPkt: " + deserQFLOGPkt.__dict__.__str__())
                        
        return(deserQFLOGPkt)