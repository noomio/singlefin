'''
Created on Dec 22, 2017

@author: asahay
'''

import sys
import logging

from threading import Thread, Event
from serial.serialutil import SerialException, SerialTimeoutException

from QFLOGPacketPackage.QFLOGPacket import QFLOGPacket
from QFLOGPacketPackage.QFLOGPacketConsts import QFLOG_PKT_CFG, QFLOG_PKT_CMDS, QFLOG_PKT_CONSTS
from QFLOGPacketPackage.QFLOGPacketSerDes import QFLOGPacketSerDes
from QFLOGLoggerPackage.QFLOGLoggerConfig import QFLOG_LOGGER_CONFIG

logger = logging.getLogger(QFLOG_LOGGER_CONFIG['LOGGER'])

class QFLOGHello(object):
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
        logger.info("Sending HELLO to device")
    
        pktId = 1
        qflogPktTX = QFLOGPacket(ctxId = 0,
                                 cmd = QFLOG_PKT_CMDS['HELLO'],
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
            
            if (qflogPktRX.getCmd() == QFLOG_PKT_CMDS['OK']):
                logger.info("Received ACK")
            elif (qflogPktRX.getCmd() == QFLOG_PKT_CMDS['ERROR']):
                logger.info("Received ERR")
                args.error.set()
            else:
                logger.debug("Received neither ACK nor ERR")
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
        
        txThread = Thread(name = "TX", target = QFLOGHello.__tx, args = (serComm, args))
        rxThread = Thread(name = "RX", target = QFLOGHello.__rx, args = (serComm, args))
        
        txThread.start()
        rxThread.start()
        
        try:
            while (rxThread.is_alive()):
                rxThread.join(1.0)
        except (KeyboardInterrupt):
            args.keyboardInterrupt.set()
        finally:
            if (rxThread.is_alive()):
                rxThread.join()