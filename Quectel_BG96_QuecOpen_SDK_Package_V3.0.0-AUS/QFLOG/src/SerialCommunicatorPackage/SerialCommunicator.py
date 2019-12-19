'''
Created on Dec 8, 2017

@author: asahay
'''

import logging

from serial import Serial
from serial.serialutil import SerialException, SerialTimeoutException

from QFLOGLoggerPackage.QFLOGLoggerConfig import QFLOG_LOGGER_CONFIG

logger = logging.getLogger(QFLOG_LOGGER_CONFIG['LOGGER'])

class SerialCommunicator(object):
    '''
    classdocs
    '''

    def __init__(self, **params):
        '''
        Constructor
        '''
        try:
            self.__serConn = Serial(**params)
            # logger.debug("serConn: " + self.__serConn.__dict__.__str__())
        except (ValueError, SerialException) as ex:
            logger.exception(ex)
            raise
        
    def write(self, data):
        # logger.debug("data: " + data.__str__())
        logger.debug("data: " + "\\x" + ("\\x".join(format(byte, '02x') for byte in bytearray(data))))
        
        if (data is None):
            logger.debug("Data is None; nothing to write")
            return(0)
        
        try:
            return(self.__serConn.write(data))
        except (SerialException, SerialTimeoutException) as ex:
            logger.exception(ex)
            raise
    
    def read(self, size = 1):
        try:
            data = self.__serConn.read(size)
            # logger.debug("data: " + data.__str__())
            logger.debug("data: " + "\\x" + ("\\x".join(format(byte, '02x') for byte in bytearray(data))))
            return(data)
        except (SerialException) as ex:
            logger.exception(ex)
            raise