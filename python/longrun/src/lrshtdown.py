#https://bitbucket.org/troeger/efinject/src/4a7de40d2945/src/efipython/efivaraccess.py?fileviewer=file-view-default

from collections import namedtuple
import os
import sys
import time
import logging
import readline
import platform
import re
import ctypes
import win32api, win32process, win32security
import struct

BUFFER_LENGTH = 50
SHUTDOWN_DELAY = 10
WAKEUP_AFTER = 30

UEFI_VAR_NAME = "RtcWakeData"
UEFI_VAR_GUID = "EC87D643-EBA4-4BB5-A1E5-3F3E36B20DA9"

logfn = './lrlog.log'
tRound = 0

RTC_WAKE_DATA_FORMAT = '=20B'
class RtcWakeData( namedtuple('RtcWakeData', 'Alarm_Wake, Alarm_Week, Alarm_Sunday, \
                              Alarm_Monday, Alarm_Tuesday, Alarm_Wednesday, Alarm_Thursday, \
                              Alarm_Friday, Alarm_Saturday, \
                              RtcWakeTime1_Hour, RtcWakeTime1_Minute, RtcWakeTime1_Second, \
                              RtcStartupSequence, RtcWakeTime_Hour, RtcWakeTime_Minute, \
                              RtcWakeTime_Second, RtcWakeDate_Year, RtcWakeDate_Month, \
                              RtcWakeDate_Day, RtcReservedByte')):
      def __str__(self):
          return """
          -----------------------
          Lenovo UEFI variable: RtcWakeData
          00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 
          %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X
          %02X %02X %02X %02X
          --------------------------
          """ % (self.Alarm_Wake, self.Alarm_Week, self.Alarm_Sunday, \
                 self.Alarm_Monday, self.Alarm_Tuesday, self.Alarm_Wednesday, self.Alarm_Thursday, \
                 self.Alarm_Friday, self.Alarm_Saturday, \
                 self.RtcWakeTime1_Hour, self.RtcWakeTime1_Minute, self.RtcWakeTime1_Second, \
                 self.RtcStartupSequence, self.RtcWakeTime_Hour, self.RtcWakeTime_Minute, \
                 self.RtcWakeTime_Second, self.RtcWakeDate_Year, self.RtcWakeDate_Month, \
                 self.RtcWakeDate_Day, self.RtcReservedByte)

      def packstring(self):
            return struct.pack(RTC_WAKE_DATA_FORMAT, self.Alarm_Wake, self.Alarm_Week, self.Alarm_Sunday, \
                 self.Alarm_Monday, self.Alarm_Tuesday, self.Alarm_Wednesday, self.Alarm_Thursday, \
                 self.Alarm_Friday, self.Alarm_Saturday, \
                 self.RtcWakeTime1_Hour, self.RtcWakeTime1_Minute, self.RtcWakeTime1_Second, \
                 self.RtcStartupSequence, self.RtcWakeTime_Hour, self.RtcWakeTime_Minute, \
                 self.RtcWakeTime_Second, self.RtcWakeDate_Year, self.RtcWakeDate_Month, \
                 self.RtcWakeDate_Day, self.RtcReservedByte)


class WinApiEfiVariables():
#    """EFI variable access for Windows platforms"""

    def __init__(self):
        # enable required SeSystemEnvironmentPrivilege privilege
        privilege = win32security.LookupPrivilegeValue(None, 'SeSystemEnvironmentPrivilege')
        token = win32security.OpenProcessToken(win32process.GetCurrentProcess(), win32security.TOKEN_READ|win32security.TOKEN_ADJUST_PRIVILEGES)
        win32security.AdjustTokenPrivileges(token, False, [(privilege, win32security.SE_PRIVILEGE_ENABLED)])
        win32api.CloseHandle(token)
        
        self.buffer = ctypes.create_string_buffer(BUFFER_LENGTH)
        self.length = 0
        # import firmware variable API
        self.GetFirmwareEnvironmentVariable = ctypes.windll.kernel32.GetFirmwareEnvironmentVariableW
        self.GetFirmwareEnvironmentVariable.restype = ctypes.c_int
        self.GetFirmwareEnvironmentVariable.argtypes = [ctypes.c_wchar_p, ctypes.c_wchar_p, ctypes.c_void_p, ctypes.c_int]

        self.SetFirmwareEnvironmentVariable = ctypes.windll.kernel32.SetFirmwareEnvironmentVariableW
        self.SetFirmwareEnvironmentVariable.restype = ctypes.c_int
        self.SetFirmwareEnvironmentVariable.argtypes = [ctypes.c_wchar_p, ctypes.c_wchar_p, ctypes.c_void_p, ctypes.c_int]

    def read(self, name, guid):
        
        self.length = self.GetFirmwareEnvironmentVariable(name, "{%s}" % guid, self.buffer, BUFFER_LENGTH)
        if self.length == 0:
        # FIXME: don't always raise WinError
           raise ctypes.WinError()
        return self.buffer[:BUFFER_LENGTH]

    def write(self, name, guid, data):
        self.SetFirmwareEnvironmentVariable(name, "{%s}" % guid, data, self.length)

def CalAlarmTimer():
    Hour = int(time.strftime("%H"), 10)
    Minute = int(time.strftime("%M"), 10)
    Second = int(time.strftime("%S"), 10)
    print "current time:", Hour, Minute, Second
    print "wake after:", WAKEUP_AFTER
    #
    total_secs = Hour*3600 + Minute*60 + Second
    total_secs += WAKEUP_AFTER
    total_secs += SHUTDOWN_DELAY
    #
    Hour = total_secs / 3600
    Minute = (total_secs % 3600) / 60
    Second = (total_secs % 3600) % 60
    if Hour >= 24:
          Hour = 0
    print "wake time: %d:%d:%d" %(Hour, Minute, Second)
    return (Hour,Minute,Second)

def TimeOutTick(delayTime):
    count=0
    while (count <= delayTime):
          time.sleep(1)
          sys.stdout.write("time out: %d  \r" % (delayTime - count))
          sys.stdout.flush()
          count += 1

if __name__ == "__main__":

    tRound = 0
    if os.path.isfile(logfn):
      firstline = open(logfn, 'r').readline().rstrip()
      tRound = int(firstline, 10)

    logging.basicConfig(level=logging.INFO,
                    #format='%(asctime)s %(levelname)s %(message)s',
                    format='%(message)s',
                    filename=logfn,
                    #filemode='w'
                    )

    tRound+=1
    print "loop of number: ",tRound
    logging.info("%d\n" % tRound)
    now = time.strftime("%H:%M:%S")
    logging.info("shutdown: %s" %now)
    #replace the first line, easy to record
    with open(logfn) as fin:
       lines = fin.readlines()
       lines[0] = "%d\n" % tRound
    with open(logfn, 'w') as fout:
       for line in lines:
         fout.write(line)
      
    efiapi = WinApiEfiVariables()
    rtcdata = efiapi.read(UEFI_VAR_NAME, UEFI_VAR_GUID)
    data = RtcWakeData( *struct.unpack_from(RTC_WAKE_DATA_FORMAT, rtcdata))
    #print data

    (Hour, Minute, Second) = CalAlarmTimer()
    logging.info("wake time: %d:%d:%d"%(Hour, Minute, Second))
    data2 = data._replace(Alarm_Wake=1,Alarm_Week=0, \
                          RtcWakeTime_Hour= Hour, \
                          RtcWakeTime_Minute= Minute, \
                          RtcWakeTime_Second= Second
                          )
    print data2
    
    rtcdata = data2.packstring()

    TimeOutTick(SHUTDOWN_DELAY)
    efiapi.write(UEFI_VAR_NAME, UEFI_VAR_GUID, rtcdata)

    os.system('shutdown /s /f /t 0')
