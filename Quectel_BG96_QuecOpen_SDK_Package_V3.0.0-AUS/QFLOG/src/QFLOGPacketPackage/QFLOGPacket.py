'''
Created on Dec 14, 2017

@author: asahay
'''

import logging

from QFLOGLoggerPackage.QFLOGLoggerConfig import QFLOG_LOGGER_CONFIG

logger = logging.getLogger(QFLOG_LOGGER_CONFIG['LOGGER'])

class QFLOGPacket(object):
    '''
    classdocs
    '''
    
    def __init__(self,
                 ctxId = 0,
                 cmd = 0,
                 pktId = 0,
                 totSize = 0,
                 payloadLen = 0,
                 pad = 0,
                 payload = b'\x00'):
        '''
        Constructor
        '''
        
        self.__ctxId = ctxId
        self.__cmd = cmd
        self.__pktId = pktId
        self.__totSize = totSize
        self.__payloadLen = payloadLen
        self.__pad = pad
        self.__payload = payload
        
        '''
        logging.debug("ctxId: " + self.__ctxId.__str__())
        logging.debug("cmd: " + self.__cmd.__str__())
        logging.debug("pktId: " + self.__pktId.__str__())
        logging.debug("totSize: " + self.__totSize.__str__())
        logging.debug("payloadLen: " + self.__payloadLen.__str__())
        logging.debug("pad: " + self.__pad.__str__())
        logging.debug("payload: " + self.__payload.__str__())
        '''
        
        logger.debug("QFLOGPacket: " + self.__dict__.__str__())
        
    def getCtxId(self):
        logger.debug("ctxId: " + self.__ctxId.__str__())
        return(self.__ctxId)
    
    def setCtxId(self, ctxId):
        logger.debug("Setting ctxId to " + self.__ctxId.__str__())
        self.__ctxId = ctxId
        
    def getCmd(self):
        logger.debug("cmd: " + self.__cmd.__str__())
        return (self.__cmd)
    
    def setCmd(self, cmd):
        logger.debug("Setting cmd to " + self.__cmd.__str__())
        self.__cmd = cmd
    
    def getPktId(self):
        logger.debug("pktId: " + self.__pktId.__str__())
        return (self.__pktId)
    
    def setPktId(self, pktId):
        logger.debug("Setting pktId to " + self.__cmd.__str__())
        self.__pktId = pktId
    
    def getTotSize(self):
        logger.debug("totSize: " + self.__totSize.__str__())
        return (self.__totSize)
    
    def setTotSize(self, totSize):
        logger.debug("Setting totSize to " + self.__totSize.__str__())
        self.__totSize = totSize
    
    def getPayloadLen(self):
        logger.debug("payloadLen: " + self.__payloadLen.__str__())
        return (self.__payloadLen)
    
    def setPayloadLen(self, payloadLen):
        logger.debug("Setting payloadLen to " + self.__payloadLen.__str__())
        self.__payloadLen = payloadLen
    
    def getPad(self):
        logger.debug("pad: " + self.__pad.__str__())
        return (self.__pad)
    
    def setPad(self, pad):
        logger.debug("Setting pad to " + self.__pad.__str__())
        self.__pad = pad
    
    def getPayload(self):
        # logger.debug("payload: " + self.__payload.__str__())
        logger.debug("payload: " + "\\x" + ("\\x".join(format(byte, '02x') for byte in bytearray(self.__payload))))
        return (self.__payload)
    
    def setPayload(self, payload):
        # logger.debug("Setting payload to " + self.__payload.__str__())
        logger.debug("Setting payload to " + self.__payload)
        self.__payload = payload