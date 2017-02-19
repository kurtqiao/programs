# for parse MFG tester.log to find out time cost
# the script find specific pattern and split the steps and time
#
# Author: kurtqiao
# date:   2017-2-19
# usage:
# parselog.py  filename.log
# 
from datetime import datetime
import time
import os, sys, string, re, glob
import optparse

#time object, set first use as None
time_mark = None
#mark new test step for time calculate
marknew = False
#time sum
time_cal = 0

#parameters checking
opt = optparse.OptionParser()
opt = optparse.OptionParser(usage = 'usage: %prog [option] -project folder name') #version='%prog ' + __version__)
(options, args) = opt.parse_args()

if len(args) == 0:
#get current folders name
    print 'Please give a file name!'
    exit(1)
else:
	Project = args[0]
	

outfile = open('output.txt', 'w')
of = open(Project, 'r')
##data = of.readlines()
##of.close()
#for example, tester.log as below:
#
#===========[VGA Test]============
#VGA start  08:25:05    +===========xxx======
#VGA test pass  08:47:55  +==================
#
for line in of:
  if '=====' in line and '[' in line:
     if line.startswith('====='):
       #return strings between '['and ']'
       s = re.findall(r'\[([^]]*)\]', line)
       outfile.write('\n'+''.join(s))
       #a new test step start, use for time calculate
       marknew = True
     if ':' in line:
       #to locate the time, just simple locate the first ':' 
       ind = line.index(':')
       #then cut the strings in time format HH:MM:SS
       time_str = line[ind-2:ind+6]
       outfile.write('\t\t\t\t\t' + time_str+ '    ')
       
       if marknew:
         #the very fist steps for time_mark init
         if time_mark is None:
           time_mark=datetime.strptime(time_str, '%H:%M:%S')
         else:
           delta = datetime.strptime(time_str, '%H:%M:%S') - time_mark
           time_cal += delta.seconds
           time_mark = datetime.strptime(time_str, '%H:%M:%S')
           #convert seconds to HH:MM:SS format
           convert_time_cal=time.strftime("%H:%M:%S", time.gmtime(time_cal))
           convert_delta=time.strftime("%H:%M:%S", time.gmtime(delta.seconds))
           #add a indicate for long time consumption
           if delta.seconds > 1200:
             bigcct_prefix = "!!BIG CONCERNT!!"
           else:
             bigcct_prefix = ""
           tmpstr = '   >>%ss(%s), %ss(%s)'% (str(delta.seconds), str(convert_delta),str(time_cal), convert_time_cal)
           outfile.write(bigcct_prefix+tmpstr)
       marknew = False
       
     #outfile.write(line)
#print "total process time: %ds, %dhrs" %(time_cal, time_cal/(3600*60))
outfile.close()
of.close()