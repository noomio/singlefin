'''
Created on Jan 8, 2018

@author: asahay
'''

import os

ROOT_DIR = os.path.dirname(os.path.abspath(__file__))

SRC_DIR = os.path.join(ROOT_DIR, "src")
LOGS_DIR = os.path.join(ROOT_DIR, "logs")

QFLOG_LOGGER_PKG_DIR = os.path.join(SRC_DIR, "QFLOGLoggerPackage")
QFLOG_PKG_DIR = os.path.join(SRC_DIR, "QFLOGPackage")
QFLOG_PKT_PKG_DIR = os.path.join(SRC_DIR, "QFLOGPacketPackage")
SER_COMM_PKG_DIR = os.path.join(SRC_DIR, "SerialCommunicatorPackage")