import string, socket, subprocess, re, smtplib
from smtplib import SMTP_SSL

SMTP_SERVER = "smtp.qq.com"
#use your qq mail and psw
SENDER_USR = "xxxxxxxxx@qq.com"
SENDER_PSW = "x12345"

#setting mail list here
MAIL_LIST = "kurtqqh@gmail.com,xxxxxxxxx@qq.com"

def login_mail(svr, usr, psw):
    #print(svr+usr+psw)
    s = smtplib.SMTP(svr)
    s.ehlo()
    s.starttls()
    s.ehlo()
    s.login(usr, psw)
    return s

def mail_content():
    
    #add ipconfig result
    ipconfig_process = subprocess.Popen("ipconfig", stdout=subprocess.PIPE)
    output = ipconfig_process.stdout.read()
    #get host ip address
    #!!gethostbyname will get IP address from Ethernet LAN first priority,
    #if ethernet have no IP, then will get WLAN IP address!!
    hostname = socket.gethostname()
    ipaddr = socket.gethostbyname(hostname)
    SUBJECT = "[Notify]IP Address Change!!"+ipaddr
    TEXT = "System reboot, ip address change to "+ipaddr+'\r\n'+output

    TO = string.splitfields(MAIL_LIST, ",")
    #setting mail body
    mail_body = '\r\n'.join(['To: %s' % TO,
       'From: %s' % SENDER_USR,
       'Subject: %s' % SUBJECT,
       '', TEXT])
    return [mail_body,TO]

if __name__ == "__main__":
    server = login_mail(SMTP_SERVER, SENDER_USR, SENDER_PSW)
    [BODY,TO] = mail_content()
    server.sendmail(SENDER_USR, TO, BODY)

    print ('email sent to '+ MAIL_LIST)
