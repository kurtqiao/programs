import Tkinter
from ScrolledText import ScrolledText
from collections import namedtuple
import time
import logging
import Queue
import os
import sys
import readline
import platform
import re
import ctypes
import win32api, win32process, win32security
import struct
import ConfigParser

logfn = './loglr.log'
configfn = './lrsession.cfg'

G_APP_VER = 'Ver 0.1'
#Global variables and default settings
BUFFER_LENGTH = 50
G_SHUTDOWN_DELAY = 10
G_WAKEUP_AFTER = 30
G_LOOP_NUMBER = 300
G_CURRENT_LOOPNUM = 0
#default S5
G_LONGRUN_ACTION = 3
G_RUN_STATUS = False
G_EDITABLE = True

LOG_FORMAT = '%(message)s'
UEFI_VAR_NAME = "RtcWakeData"
UEFI_VAR_GUID = "EC87D643-EBA4-4BB5-A1E5-3F3E36B20DA9"
RTC_WAKE_DATA_FORMAT = '=20B'

class RtcWakeData(
      namedtuple('RtcWakeData', 'Alarm_Wake, Alarm_Week, Alarm_Sunday, \
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
        #don't always raise WinError
           raise ctypes.WinError()
        return self.buffer[:BUFFER_LENGTH]

    def write(self, name, guid, data):
        self.SetFirmwareEnvironmentVariable(name, "{%s}" % guid, data, self.length)

#=====Config class==============
class lrConfigParser():
    
    def __init__(self, fn):
        self.cfg_file = fn
        self.config = ConfigParser.RawConfigParser()
        self.init_default(self.cfg_file)

    def init_default(self, fn):
        global G_RUN_STATUS
        global BUFFER_LENGTH
        global G_SHUTDOWN_DELAY
        global G_WAKEUP_AFTER
        global G_LOOP_NUMBER
        global G_CURRENT_LOOPNUM
        global G_LONGRUN_ACTION
        if os.path.isfile(fn):
            self.config.read(fn)
            (G_RUN_STATUS, G_CURRENT_LOOPNUM, G_SHUTDOWN_DELAY, G_LOOP_NUMBER, \
             G_WAKEUP_AFTER, G_LONGRUN_ACTION) = self.read_config()
                         
        else:
            self.config.add_section('Section')
            self.config.set('Section', 'run_status', 'False')
            self.config.set('Section', 'current_loop', str(G_CURRENT_LOOPNUM))
            self.config.set('Section', 'shutdown_delay', str(G_SHUTDOWN_DELAY))
            self.config.set('Section', 'loop_number', str(G_LOOP_NUMBER))
            self.config.set('Section', 'wake_after', str(G_WAKEUP_AFTER))
            self.config.set('Section', 'longrun_action', str(G_LONGRUN_ACTION))
            
    def update_file(self):
        with open(self.cfg_file, 'wb') as configfile:
            self.config.write(configfile)
            
    def read_config(self):
        runsts = self.config.getboolean('Section', 'run_status')
        crtloop = self.config.getint('Section', 'current_loop')
        shtdelay = self.config.getint('Section', 'shutdown_delay')
        loopnum = self.config.getint('Section', 'loop_number')
        wakeaftr = self.config.getint('Section', 'wake_after')
        lraction = self.config.getint('Section', 'longrun_action')
        return (runsts, crtloop, shtdelay, loopnum, wakeaftr, lraction)
    def read_run_status(self):
        return self.config.getboolean('Section', 'run_status')
    
    def set_config(self, itemx, val):
        self.config.set('Section', itemx, val)
#=====Queue logger for show log======================
class QueueLogger(logging.Handler):
    def __init__(self, queue):
        logging.Handler.__init__(self)
        self.queue = queue
    def emit(self, record):
        self.queue.put(self.format(record).rstrip('\n')+'\n')

#======main GUI===============       
class facades_main(Tkinter.Tk):
    
    def __init__(self, parent):
        Tkinter.Tk.__init__(self, parent)
        self.parent = parent

        self.initialize()
        self.clock_tick()

    def update_val_from_entry(self):
          global G_RUN_STATUS
          global G_SHUTDOWN_DELAY
          global G_WAKEUP_AFTER
          global G_LOOP_NUMBER
          global G_LONGRUN_ACTION
          G_SHUTDOWN_DELAY = int(self.entry_second.get(), base=10)
          G_WAKEUP_AFTER = int(self.entry_wakeafter.get(), base=10)
          G_LOOP_NUMBER = int(self.entry_loop.get(), base=10)
          G_LONGRUN_ACTION = self.var_action.get()

    def reset_rtcdata(self):
        efiapi = WinApiEfiVariables()
        rtcdata = efiapi.read(UEFI_VAR_NAME, UEFI_VAR_GUID)
        data = RtcWakeData(*struct.unpack_from(RTC_WAKE_DATA_FORMAT, rtcdata))
        #just reset to not wake up
        data2 = data._replace(Alarm_Wake=3,Alarm_Week=0, \
                          RtcWakeTime_Hour= 0, \
                          RtcWakeTime_Minute= 0, \
                          RtcWakeTime_Second= 0
                          )
        rtcdata = data2.packstring()
        efiapi.write(UEFI_VAR_NAME, UEFI_VAR_GUID, rtcdata)
        
    def func_shutdown(self):
        global G_CURRENT_LOOPNUM 
        G_CURRENT_LOOPNUM += 1
        self.confighandle.set_config('run_status', 'True')
        self.confighandle.set_config('current_loop', str(G_CURRENT_LOOPNUM))
        self.confighandle.update_file()
        
        if (G_LONGRUN_ACTION == 4):
          print "reboot"
          Hour = int(time.strftime("%H"), 10)
          Minute = int(time.strftime("%M"), 10)
          Second = int(time.strftime("%S"), 10)
          self.logger.info("reboot time: %d:%d:%d" %(Hour, Minute, Second))
          os.system('shutdown /r /f /t 0')
          while(1):
            pass

        efiapi = WinApiEfiVariables()
        rtcdata = efiapi.read(UEFI_VAR_NAME, UEFI_VAR_GUID)
        data = RtcWakeData(*struct.unpack_from(RTC_WAKE_DATA_FORMAT, rtcdata))

        (Hour, Minute, Second) = self.CalAlarmTimer()
        data2 = data._replace(Alarm_Wake=1,Alarm_Week=0, \
                          RtcWakeTime_Hour= Hour, \
                          RtcWakeTime_Minute= Minute, \
                          RtcWakeTime_Second= Second
                          )
        rtcdata = data2.packstring()
        efiapi.write(UEFI_VAR_NAME, UEFI_VAR_GUID, rtcdata)

        
        if (G_LONGRUN_ACTION == 3):
          print "shutdown"
          os.system('shutdown /s /f /t 0')
          
        if (G_LONGRUN_ACTION == 2):
          print "suspend"
          os.system('rundll32.exe PowrProf.dll,SetSuspendState')
        if (G_LONGRUN_ACTION == 1):
          print "sleep"
          os.system('rundll32.exe powrprof.dll,SetSuspendState 0,1,0')
        
    def CalAlarmTimer(self):
        Hour = int(time.strftime("%H"), 10)
        Minute = int(time.strftime("%M"), 10)
        Second = int(time.strftime("%S"), 10)
        print "current time:", Hour, Minute, Second
        print "wake after:", G_WAKEUP_AFTER
        self.logger.info("current time: %d:%d:%d" %(Hour, Minute, Second))
        #
        total_secs = Hour*3600 + Minute*60 + Second
        total_secs += G_WAKEUP_AFTER
        total_secs += G_SHUTDOWN_DELAY
        #
        Hour = total_secs / 3600
        Minute = (total_secs % 3600) / 60
        Second = (total_secs % 3600) % 60
        if Hour >= 24:
            Hour = 0
        print "wake time: %d:%d:%d" %(Hour, Minute, Second)
        self.logger.info("wake time: %d:%d:%d" %(Hour, Minute, Second))
        return (Hour,Minute,Second)

    def clock_tick(self):
        now = time.strftime("%H:%M:%S")
        self.label_ticker.configure(text=now)
        self.label_ticker.after(200, self.clock_tick)

    def clock_timeout(self):
        count = self.entry_second.get()
        #print count
        count = int(count, 10)
        print count
        if (G_RUN_STATUS):
          if (count > 0):
            self.count_second.set(count - 1)
            self._job = self.entry_second.after(1000, self.clock_timeout)
        if (count == 0):
            print "function to action!"
            self.func_shutdown()
        
    def initialize(self):
        global G_RUN_STATUS
        global G_SHUTDOWN_DELAY
        global G_WAKEUP_AFTER
        global G_LOOP_NUMBER
        global G_CURRENT_LOOPNUM
        global G_LONGRUN_ACTION
        self.geometry("400x450+300+100")
        self.minsize(350,250)
        self.grid()

        self._job = None
        self.confighandle = lrConfigParser(configfn)
        
        self.count_second = Tkinter.IntVar()
        self.str_action = Tkinter.StringVar()
        self.var_action = Tkinter.IntVar()
        self.loop_number = Tkinter.IntVar()
        self.current_loop = Tkinter.IntVar()
        self.wake_after = Tkinter.IntVar()
        self.str_start = Tkinter.StringVar()
              
        self.var_action.set(G_LONGRUN_ACTION )
        self.count_second.set(G_SHUTDOWN_DELAY)
        self.loop_number.set(G_LOOP_NUMBER)
        self.current_loop.set(G_CURRENT_LOOPNUM)
        self.wake_after.set(G_WAKEUP_AFTER)
        if (G_LONGRUN_ACTION == 4):
          self.str_action.set("reboot  ")
        if (G_LONGRUN_ACTION == 3):
          self.str_action.set("Shutdown")
        self.log_queue = Queue.Queue()
        
        self.label_blank = Tkinter.Label(self, text='')
        self.label_ticker = Tkinter.Label(self, text="test",anchor='e',font=('times', 20, 'bold'), bg='green')
        self.entry_second = Tkinter.Entry(self, text=self.count_second, font=('times', 20, 'bold'),width=5)
        self.label_str1 = Tkinter.Label(self, text='seconds to',font=('times', 20, 'bold'))
        self.label_str2 = Tkinter.Label(self, text='Loop',font=('times', 20, 'bold'))
        self.label_str3 = Tkinter.Label(self, text='seconds to',font=('times', 20, 'bold'))
        self.label_str4 = Tkinter.Label(self, text='wakeup', font=('times', 20, 'bold'))
        self.entry_wakeafter = Tkinter.Entry(self, text=self.wake_after, font=('times', 20, 'bold'),width=5)
        self.label_str_action = Tkinter.Label(self, textvariable=self.str_action, font=('times', 20, 'bold'))
        self.radiobtn_s3 = Tkinter.Radiobutton(self, text='S3',variable=self.var_action, value=1, command=self.radiobtn_callback, state='disabled')
        self.radiobtn_s4 = Tkinter.Radiobutton(self, text='S4',variable=self.var_action, value=2, command=self.radiobtn_callback, state='disabled')
        self.radiobtn_s5 = Tkinter.Radiobutton(self, text='S5',variable=self.var_action, value=3, command=self.radiobtn_callback)
        self.radiobtn_reboot = Tkinter.Radiobutton(self, text='reboot',variable=self.var_action, value=4, command=self.radiobtn_callback)
        self.entry_loop = Tkinter.Entry(self, text=self.loop_number, font=('times', 20, 'bold'),width=5)
        self.btn_start = Tkinter.Button(self, text="Start",bg="green",font=('times', 20, 'bold'), command=self.btn_start_callback)
        self.btn_edit = Tkinter.Button(self, text="Edit", font=('times', 20, 'bold'), state="disabled", command=self.btn_edit_callback)
        self.label_current_loop = Tkinter.Label(self, textvariable=self.current_loop, font=('times', 20, 'bold'))
        self.btn_clrlog = Tkinter.Button(self, text="Clear Log",font=('times', 15, 'bold'), command=self.btn_clearlog_callback)
        self.log_widget = ScrolledText(self, width = 50, heigh = 15)
        
        
        self.label_blank.grid(row=0,column=0,ipadx=20)
        self.label_ticker.grid(row=0,column=2,sticky='w')
        self.entry_second.grid(row=1,column=0)
        self.label_str1.grid(row=1,column=1,sticky='w')
        self.label_str_action.grid(row=1,column=2,sticky='w')
        self.entry_loop.grid(row=2,column=0)
        self.label_str2.grid(row=2, column=1,sticky='w')
        self.radiobtn_s3.grid(row=2,column=2,sticky='w')
        self.radiobtn_s4.grid(row=2,column=2,sticky='w', padx=40)
        self.radiobtn_s5.grid(row=2,column=2,sticky='w', padx=80)
        self.radiobtn_reboot.grid(row=2,column=2,sticky='w',padx=120)
        self.entry_wakeafter.grid(row=3, column=0)
        self.label_str3.grid(row=3, column=1)
        self.label_str4.grid(row=3, column=2,sticky='w')
        self.label_blank.grid(row=4,column=0)
        self.btn_start.grid(row=5, column=0)
        self.btn_edit.grid(row=5, column=1)
        self.btn_clrlog.grid(row=5,column=2,sticky='w')
        self.label_current_loop.grid(row=5, column=2,sticky='w',padx=120)
        self.log_widget.grid(row=6, column=0, columnspan=3, sticky='w')

        #init log queue
        self.logger = logging.getLogger(logfn)
        self.logger.setLevel(logging.INFO)
        logformat = logging.Formatter(LOG_FORMAT)
        hl = QueueLogger(queue=self.log_queue)
        hl.setFormatter(logformat)
        self.logger.addHandler(hl)

        self.start_logwidget()

        if G_CURRENT_LOOPNUM <= G_LOOP_NUMBER:
          if G_RUN_STATUS:
              print "auto run loop %d" %(G_CURRENT_LOOPNUM)
              self.logger.info("\ncurrent loop: %d" %(G_CURRENT_LOOPNUM))
              self.btn_start_callback()
        else:
          print "loop pass!"
          self.current_loop.set(G_CURRENT_LOOPNUM-1)
          
          if G_RUN_STATUS:
            print "reset run status here"
            G_RUN_STATUS = False
            G_CURRENT_LOOPNUM -= 1
            self.current_loop.set(str(G_CURRENT_LOOPNUM))
            self.confighandle.set_config('current_loop', str(G_CURRENT_LOOPNUM))
            self.confighandle.update_file()
            self.reset_rtcdata()

    def radiobtn_callback(self):
        global G_LONGRUN_ACTION
        seltmp = self.var_action.get()
        G_LONGRUN_ACTION = seltmp
        if (seltmp == 4):
            self.str_action.set("reboot")
        if (seltmp == 3):
            self.str_action.set("Shutdown")
        if (seltmp == 2):
            self.str_action.set("Suspend ")
        if (seltmp == 1):
            self.str_action.set("Sleep   ")

    def btn_clearlog_callback(self):
        global G_CURRENT_LOOPNUM
        self.log_widget.config(state='normal')
        self.log_widget.delete(0.0, Tkinter.END)
        self.log_widget.config(state='disabled')

        if os.path.isfile(logfn):
            with open(logfn, 'w'):
                pass
        #reset current loop to zero
        if G_CURRENT_LOOPNUM != 0:
          G_CURRENT_LOOPNUM = 0
          self.confighandle.set_config('current_loop', str(G_CURRENT_LOOPNUM))
          self.confighandle.update_file()
          self.current_loop.set(G_CURRENT_LOOPNUM)


    def btn_start_callback(self):
        global G_RUN_STATUS
        global G_EDITABLE
        if G_EDITABLE:
          G_EDITABLE = False
          print "set get from entry"
          self.update_val_from_entry()
          self.confighandle.set_config('shutdown_delay', str(G_SHUTDOWN_DELAY))
          self.confighandle.set_config('loop_number', str(G_LOOP_NUMBER))
          self.confighandle.set_config('wake_after', str(G_WAKEUP_AFTER))
          self.confighandle.set_config('longrun_action', str(G_LONGRUN_ACTION))
          self.confighandle.update_file()
        
        self.btn_start.config(text='Stop ', bg = "red", command=self.btn_stop_callback)
        self.entry_second.config(state='disabled')
        self.entry_loop.config(state='disabled')
        self.btn_edit.config(state='disabled')
        self.entry_wakeafter.config(state='disabled')
        G_EDITABLE = False

        if not G_RUN_STATUS:
          G_RUN_STATUS = True
        self.clock_timeout()
        

    def btn_stop_callback(self):
        global G_RUN_STATUS
        G_RUN_STATUS = False
        self.btn_start.config(text="Start", bg = "green", command=self.btn_start_callback)
        self.btn_edit.config(state='normal')
        
        self.confighandle.set_config('run_status', str(G_RUN_STATUS))
        self.confighandle.update_file()

        self.reset_rtcdata()


    def btn_edit_callback(self):
        global G_EDITABLE
        print "edit callback, to enable change variables"
        self.btn_edit.config(text="Apply", command=self.btn_apply_callback)
        self.entry_second.config(state='normal')
        self.entry_loop.config(state='normal')
        self.entry_wakeafter.config(state='normal')
        G_EDITABLE = True

    def btn_apply_callback(self):
        global G_EDITABLE
        print "apply callback, save changes to cfg file after edit"
        self.btn_edit.config(text="Edit", command=self.btn_edit_callback)
        self.entry_second.config(state='disabled')
        self.entry_loop.config(state='disabled')
        self.entry_wakeafter.config(state='disabled')
        G_EDITABLE = False
        
        self.update_val_from_entry()
        ##update into cfg file
        self.confighandle.set_config('shutdown_delay', str(G_SHUTDOWN_DELAY))
        self.confighandle.set_config('loop_number', str(G_LOOP_NUMBER))
        self.confighandle.set_config('wake_after', str(G_WAKEUP_AFTER))
        self.confighandle.set_config('longrun_action', str(G_LONGRUN_ACTION))
        self.confighandle.update_file()

    def start_logwidget(self):
        if os.path.isfile(logfn):
            self.log_widget.config(state='normal')
            print "read log file into log widget"
            with open(logfn) as fin:
                for line in fin:
                    self.log_widget.insert(Tkinter.END, line)
                    self.log_widget.see(Tkinter.END)
                    self.log_widget.update_idletasks()
            self.log_widget.config(state='disabled')

        self.update_logwidget(self.log_widget, self.log_queue)

    def update_logwidget(self, widget, queue):
        widget.config(state='normal')
        while not queue.empty():
            line = queue.get()
            widget.insert(Tkinter.END, line)
            widget.see(Tkinter.END)
            widget.update_idletasks()
        widget.config(state='disabled')
        self.logger_alarm = widget.after(10, self.update_logwidget, widget, queue)


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO,
                        filename=logfn,
                        format=LOG_FORMAT)
    app = facades_main(None)
    app.title('Shutdown Longrun %s' %G_APP_VER)
    
    app.mainloop()
