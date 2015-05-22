import string
import socket
import subprocess
import re
import smtplib
from smtplib import SMTP_SSL

#use your qq mail and psw
send_user = "xxxxxxxxx@qq.com"
send_pwd = "x12345"

#setting mail list here
mail_list = "kurtqqh@gmail.com,kurtqiao@qq.com"
TO = string.splitfields(mail_list, ",")

#get host ip address
#!!gethostbyname will get IP address from Ethernet LAN first priority,
#if ethernet have no IP, then will get WLAN IP address!!
#
hostname = socket.gethostname()
ipaddr = socket.gethostbyname(hostname)

#add ipconfig result
ipconfig_process = subprocess.Popen("ipconfig", stdout=subprocess.PIPE)
output = ipconfig_process.stdout.read()


SUBJECT = "[Notify]IP Address Change!!"+ipaddr
TEXT = "System reboot, ip address change to "+ipaddr+'\r\n'+output

#setting mail server
server = smtplib.SMTP("smtp.qq.com")
server.ehlo()
server.starttls()
server.ehlo()
server.login(send_user, send_pwd)

#setting mail body
BODY = '\r\n'.join(['To: %s' % TO,
       'From: %s' % send_user,
       'Subject: %s' % SUBJECT,
       '', TEXT])

server.sendmail(send_user, TO, BODY)

print ('email sent to '+ mail_list)
