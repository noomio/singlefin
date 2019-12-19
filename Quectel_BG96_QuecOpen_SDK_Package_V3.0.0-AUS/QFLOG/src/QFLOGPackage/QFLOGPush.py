'''
Created on Dec 22, 2017

@author: asahay
'''

import sys
import logging
import struct

from threading import Thread, Event
from os.path import basename, getsize
from serial.serialutil import SerialException, SerialTimeoutException
from time import sleep

from QFLOGPackage.QFLOGConsts import QFLOG_CONSTS
from QFLOGPackage.QFLOGHello import QFLOGHello
from QFLOGPacketPackage.QFLOGPacket import QFLOGPacket
from QFLOGPacketPackage.QFLOGPacketConsts import QFLOG_PKT_CFG, QFLOG_PKT_CMDS, QFLOG_PKT_CONSTS
from QFLOGPacketPackage.QFLOGPacketSerDes import QFLOGPacketSerDes
from QFLOGLoggerPackage.QFLOGLoggerConfig import QFLOG_LOGGER_CONFIG

logger = logging.getLogger(QFLOG_LOGGER_CONFIG['LOGGER'])

class QFLOGPush(object):
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
        try:
            file_ = open(args.filepath, "rb")
        except (OSError, IOError) as ex:
            logger.exception(ex)
            args.error.set()
            sys.exit(1)
        
        filename = basename(file_.name)
        filesize = getsize(file_.name)
        
        pktId = 1
        qflogPktTX = QFLOGPacket(ctxId = 0,
                                 cmd = QFLOG_PKT_CMDS['PUSH'],
                                 pktId = pktId,
                                 totSize = filesize,
                                 payloadLen = len(filename),
                                 pad = QFLOG_PKT_CONSTS['PAD'],
                                 # payload = bytes(filename, "ascii"))
                                 payload = struct.pack("<" + str(len(filename)) + "s", bytes(filename, "ascii"))) #Hyman_20180904
        
        logger.debug("packetTX: " + qflogPktTX.__dict__.__str__())
        byteArray = QFLOGPacketSerDes.serialize(qflogPktTX)
        # logger.debug("byteArray: " + byteArray.__str__())
        logger.debug("byteArray: " + "\\x" + ("\\x".join(format(byte, '02x') for byte in bytearray(byteArray))))
            
        try:
            logger.debug("bytes: " + serComm.write(byteArray).__str__())
        except (SerialException, SerialTimeoutException) as ex:
            logger.exception(ex)
            args.error.set()
            file_.close()
            sys.exit(1)
            
        sleep(QFLOG_CONSTS['MIN_TX_DELAY'])
        
        try:
            payload = file_.read(QFLOG_PKT_CFG['MAX_PAYLOAD_LEN'])
        except (OSError, IOError) as ex:
            logger.exception(ex)
            args.error.set()
            file_.close()
            sys.exit(1)
            
        logger.info("Pushing " + filename + " to device")
        
        while (payload != b'' and not args.error.is_set() and not args.keyboardInterrupt.is_set()):
            pktId = pktId + 1
            qflogPktTX = QFLOGPacket(ctxId = 0,
                                     cmd = QFLOG_PKT_CMDS['PUSH'],
                                     pktId = pktId,
                                     totSize = filesize,
                                     payloadLen = len(payload),
                                     pad = QFLOG_PKT_CONSTS['PAD'],
                                     payload = payload)
            
            logger.debug("packetTX: " + qflogPktTX.__dict__.__str__())
            byteArray = QFLOGPacketSerDes.serialize(qflogPktTX)
            # logger.debug("byteArray: " + byteArray.__str__())
            logger.debug("byteArray: " + "\\x" + ("\\x".join(format(byte, '02x') for byte in bytearray(byteArray))))
            
            try:
                logger.debug("bytes: " + serComm.write(byteArray).__str__())
            except (SerialException, SerialTimeoutException) as ex:
                logger.exception(ex)
                args.error.set()
                file_.close()
                sys.exit(1)
                
            sleep(QFLOG_CONSTS['MIN_TX_DELAY'])
            
            try:
                payload = file_.read(QFLOG_PKT_CFG['MAX_PAYLOAD_LEN'])
            except (OSError, IOError) as ex:
                logger.exception(ex)
                args.error.set()
                file_.close()
                sys.exit(1)
        
        if (args.error.is_set()):
            logger.error("Encountered an error")
        elif (args.keyboardInterrupt.is_set()):
            logger.error("Received keyboard interrupt")
        else:
            logger.info("Push complete")
        
        file_.close()
        
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
        logger.info("Resetting the context by sending a HELLO packet")
        QFLOGHello.execute(serComm, args)
        
        error = Event()
        args.error = error
        
        keyboardInterrupt = Event()
        args.keyboardInterrupt = keyboardInterrupt
        
        txThread = Thread(name = "TX", target = QFLOGPush.__tx, args = (serComm, args))
        rxThread = Thread(name = "RX", target = QFLOGPush.__rx, args = (serComm, args))
        
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