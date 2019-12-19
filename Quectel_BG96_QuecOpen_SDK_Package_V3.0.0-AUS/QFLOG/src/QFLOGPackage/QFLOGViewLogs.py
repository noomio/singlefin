'''
Created on Dec 27, 2017

@author: asahay
'''

import sys
import logging
import struct

from threading import Thread, Event
from serial.serialutil import SerialException, SerialTimeoutException
from time import sleep

from QFLOGPackage.QFLOGConsts import QFLOG_CONSTS
from QFLOGPacketPackage.QFLOGPacket import QFLOGPacket
from QFLOGPacketPackage.QFLOGPacketSerDes import QFLOGPacketSerDes
from QFLOGPacketPackage.QFLOGPacketConsts import QFLOG_PKT_CFG, QFLOG_PKT_CMDS, QFLOG_PKT_CONSTS
from QFLOGLoggerPackage.QFLOGLoggerConfig import QFLOG_LOGGER_CONFIG

logger = logging.getLogger(QFLOG_LOGGER_CONFIG['LOGGER'])

class QFLOGViewLogs(object):
    '''
    classdocs
    '''

    def __init__(self):
        '''
        Constructor
        '''
        raise(TypeError("Attempting to instantiate a non-instantiable class"))
    
    @classmethod    
    def __tx(cls, serComm, args):
        logger.info("Enabling logging")
        
        pktId = 1
        qflogPktTX = QFLOGPacket(ctxId = 0,
                                 cmd = QFLOG_PKT_CMDS['LOG_ENABLE'],
                                 pktId = pktId,
                                 totSize = QFLOG_PKT_CONSTS['EMPTY_PAYLOAD_TOT_SIZE'],
                                 payloadLen = QFLOG_PKT_CONSTS['EMPTY_PAYLOAD_PAYLOAD_LEN'],
                                 pad = QFLOG_PKT_CONSTS['PAD'],
                                 payload = QFLOG_PKT_CONSTS['EMPTY_PAYLOAD'])
        
        logger.debug("packetTX: " + qflogPktTX.__dict__.__str__())
        byteArray = QFLOGPacketSerDes.serialize(qflogPktTX)
        # logger.debug("byteArray: " + byteArray.__str__())
        logger.debug("byteArray: " + "\\x" + ("\\x".join(format(byte, '02x') for byte in bytearray(byteArray))))
        
        try:
            logger.debug("bytes: " + serComm.write(byteArray).__str__())
        except (SerialException, SerialTimeoutException) as ex:
            logger.exception(ex)
            args.error.set()
            sys.exit(1)
        
        sleep(QFLOG_CONSTS['MIN_TX_DELAY'])
        
        while (not args.error.is_set() and not args.keyboardInterrupt.is_set()):
            args.keyboardInterrupt.wait(1.0)
        
        logger.info("Disabling logging")
            
        qflogPktTX = QFLOGPacket(ctxId = 0,
                                 cmd = QFLOG_PKT_CMDS['LOG_DISABLE'],
                                 pktId = pktId,
                                 totSize = QFLOG_PKT_CONSTS['EMPTY_PAYLOAD_TOT_SIZE'],
                                 payloadLen = QFLOG_PKT_CONSTS['EMPTY_PAYLOAD_PAYLOAD_LEN'],
                                 pad = QFLOG_PKT_CONSTS['PAD'],
                                 payload = QFLOG_PKT_CONSTS['EMPTY_PAYLOAD'])
        
        logger.debug("packetTX: " + qflogPktTX.__dict__.__str__())
        byteArray = QFLOGPacketSerDes.serialize(qflogPktTX)
        # logger.debug("byteArray: " + byteArray.__str__())
        logger.debug("byteArray: " + "\\x" + ("\\x".join(format(byte, '02x') for byte in bytearray(byteArray))))
        
        try:
            logger.debug("bytes: " + serComm.write(byteArray).__str__())
        except (SerialException, SerialTimeoutException) as ex:
            logger.exception(ex)
            args.error.set()
            sys.exit(1)
        
    @classmethod
    def __rx(cls, serComm, args):
        qflogPktRX = None
        while (qflogPktRX is None and not args.error.is_set() and not args.keyboardInterrupt.is_set()):
            try:
                qflogPktRX = QFLOGPacketSerDes.deserialize(serComm.read(QFLOG_PKT_CFG['HEADER_LEN']))
            except (SerialException) as ex:
                logger.exception(ex)
                args.error.set()
                sys.exit(1)
                
            logger.debug("packetRX: " + qflogPktRX.__dict__.__str__())
            
            if (qflogPktRX.getCmd() == QFLOG_PKT_CMDS['LOG']):
                logger.debug("Received LOG")
                try:
                    qflogPktRX.setPayload(serComm.read(qflogPktRX.getPayloadLen()))
                except (SerialException) as ex:
                    logger.exception(ex)
                    args.error.set()
                    sys.exit(1)
            
                logger.info("[LOG]: " + qflogPktRX.getPayload().__str__())
            
            qflogPktRX = None
    
        if (args.error.is_set()):
            logger.error("Encountered an error")
        elif (args.keyboardInterrupt.is_set()):
            logger.error("Received keyboard interrupt")
            
    @classmethod
    def execute(cls, serComm, args):
        error = Event()
        args.error = error
        
        keyboardInterrupt = Event()
        args.keyboardInterrupt = keyboardInterrupt
        
        txThread = Thread(name = "TX", target = QFLOGViewLogs.__tx, args = (serComm, args))
        rxThread = Thread(name = "RX", target = QFLOGViewLogs.__rx, args = (serComm, args))
        
        txThread.start()
        rxThread.start()
        
        try:
            while (txThread.is_alive() and rxThread.is_alive()):
                txThread.join(1.0)
                rxThread.join(1.0)
        except (KeyboardInterrupt):
            args.keyboardInterrupt.set()
        finally:
            if (txThread.is_alive()):
                txThread.join()
            if (rxThread.is_alive()):
                rxThread.join()