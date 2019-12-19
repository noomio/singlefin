'''
Created on Dec 21, 2017

@author: asahay
'''

QFLOG_PKT_CFG = {'BYTE_ORDER':"little",
                 'CTX_ID_LEN': 1,
                 'CMD_LEN': 1,
                 'PKT_ID_LEN': 2,
                 'TOT_SIZE_LEN': 4,
                 'PAYLOAD_LEN_LEN': 2,
                 'PAD_LEN': 2,
                 'MAX_PAYLOAD_LEN': 490,
                 'CTX_ID_OFFSET': 0,
                 'CMD_OFFSET': 1,
                 'PKT_ID_OFFSET': 2,
                 'TOT_SIZE_OFFSET': 4,
                 'PAYLOAD_LEN_OFFSET': 8,
                 'PAD_OFFSET': 10,
                 'PAYLOAD_OFFSET': 12}

QFLOG_PKT_CFG.update({'HEADER_LEN': QFLOG_PKT_CFG['CTX_ID_LEN'] + \
                                    QFLOG_PKT_CFG['CMD_LEN'] + \
                                    QFLOG_PKT_CFG['PKT_ID_LEN'] + \
                                    QFLOG_PKT_CFG['TOT_SIZE_LEN'] + \
                                    QFLOG_PKT_CFG['PAYLOAD_LEN_LEN'] + \
                                    QFLOG_PKT_CFG['PAD_LEN']})

QFLOG_PKT_CMDS = {'ERROR': -1,
                  'HELLO': 0,
                  'OK': 1,
                  'PUSH': 2,
                  'DELETE': 4,
                  'LOG': 5,
                  'LOG_ENABLE': 6,
                  'LOG_DISABLE': 7}

QFLOG_PKT_CONSTS = {'PAD': 0,
                    'EMPTY_PAYLOAD_TOT_SIZE': 0,
                    'EMPTY_PAYLOAD_PAYLOAD_LEN': 0,
                    'EMPTY_PAYLOAD': b'\x00'}