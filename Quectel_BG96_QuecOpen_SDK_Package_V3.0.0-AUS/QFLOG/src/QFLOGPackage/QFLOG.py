'''
Created on Dec 21, 2017

@author: asahay
'''

import sys
import os
import logging.config

from argparse import ArgumentParser
from serial.serialutil import SerialException

from dirpaths import LOGS_DIR 
from QFLOGLoggerPackage.QFLOGLoggerConfig import QFLOG_LOGGER_CONFIG
from QFLOGPackage.QFLOGConsts import QFLOG_CMDS, QFLOG_CONSTS
from QFLOGPackage.QFLOGHello import QFLOGHello
from QFLOGPackage.QFLOGPush import QFLOGPush
from QFLOGPackage.QFLOGDelete import QFLOGDelete
from QFLOGPackage.QFLOGViewLogs import QFLOGViewLogs
from SerialCommunicatorPackage.SerialCommunicator import SerialCommunicator

if __name__ == '__main__':
    logging.logfile = os.path.join(LOGS_DIR, "QFLOG.log")
    logging.config.fileConfig(QFLOG_LOGGER_CONFIG['CONFIG_FILE_PATH'])
    logger = logging.getLogger(QFLOG_LOGGER_CONFIG['LOGGER']) 
    
    argParser = ArgumentParser(add_help = False)
    
    argParser.add_argument("-h", "--help", help = "Help", action = "help")
    argParser.add_argument("-v", "--version", help = "Version", action = "version", version="Version")
    
    argParser.add_argument("-p", "--port", help = "Port", dest = "port")
    '''
    argParser.add_argument("-b", "--baudrate", help = "", dest = "baudrate")
    argParser.add_argument("-B", "--bytesize", help = "", dest = "bytesize")
    argParser.add_argument("-P", "--parity", help = "", dest = "parity")
    argParser.add_argument("-s", "--stopbits", help = "", dest = "stopbits")
    argParser.add_argument("-t", "--timeout", help = "", dest = "timeout")
    argParser.add_argument("-x", "--xonxoff", help = "", dest = "xonxoff")
    argParser.add_argument("-r", "--rtscts", help = "", dest = "rtscts")
    argParser.add_argument("-w", "--wt", help = "", dest = "wt")
    argParser.add_argument("-d", "--dsrdtr", help = "", dest = "dsrdtr")
    argParser.add_argument("-i", "--ibt", help = "", dest = "ibt")
    argParser.add_argument("-e", "--exclusive", help = "", dest = "exclusive")
    '''
    
    cmdArgParser = argParser.add_subparsers(help = "Command", dest = "Command")
    
    helloCmdArgParser = cmdArgParser.add_parser(QFLOG_CMDS['HELLO'], help="Hello", add_help = False)
    helloCmdArgParser.add_argument("-h", "--help", help = "Help", action = "help")
    helloCmdArgParser.set_defaults(func = QFLOGHello.execute)
    
    pushCmdArgParser = cmdArgParser.add_parser(QFLOG_CMDS['PUSH'], help="Push", add_help = False)
    pushCmdArgParser.add_argument("-h", "--help", help = "Help", action = "help")
    pushCmdArgParser.add_argument("-f", "--filepath", help = "Filepath", required = True, dest = "filepath")
    pushCmdArgParser.set_defaults(func = QFLOGPush.execute)
    
    delCmdArgParser = cmdArgParser.add_parser(QFLOG_CMDS['DELETE'], help="Delete", add_help = False)
    delCmdArgParser.add_argument("-h", "--help", help = "Help", action = "help")
    delCmdArgParser.add_argument("-f", "--filename", help = "Filename", required = True, dest = "filename")
    delCmdArgParser.set_defaults(func = QFLOGDelete.execute)
    
    vlCmdArgParser = cmdArgParser.add_parser(QFLOG_CMDS['VIEW_LOGS'], help="View Logs", add_help = False)
    vlCmdArgParser.add_argument("-h", "--help", help = "Help", action = "help")
    vlCmdArgParser.set_defaults(func = QFLOGViewLogs.execute)
    
    args = argParser.parse_args()
    
    try:
        serComm = SerialCommunicator(port = args.port,
                                     baudrate = QFLOG_CONSTS['BAUDRATE'],
                                     timeout = QFLOG_CONSTS['TIMEOUT'])
    except (ValueError, SerialException) as ex:
        logger.exception(ex)
        sys.exit(1)
    
    try:
        args.func(serComm, args)
    except (Exception, KeyboardInterrupt) as ex:
        logger.exception(ex)
        sys.exit(1)