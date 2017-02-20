#script for GB LSS fake data created ^-^ (i know, but this just works)
#the points are 
#1. to seprate a given data to 4 random range data
#2. time manipulate
#the script copy a template_folder to a random name folder
#output n=len(DATA_ARRAY) folders with filenam modified
#
from datetime import datetime
import random
import time
import os, sys, string, re, glob

template_folder = 'PC0FLR8T'
filenam = 'all_groups_items.xml'
TIME_PATTERN = 'time='
pattern_tst_steps = ['AVT', 'RUNIN','PRELOAD','FVT']


DATA_ARRAY = [
161.86,163.82,159.03,156.74,161.79,163.56,  #Unit1
158.46,161.1,161.82,165.83,162.46,157.97,   #Unit2
162.84,159.27,157.34,158.51,159.41,157.6,   #Unit3
160.41,158.21,164.34,162.49,157.57,158.03,  #Unit4
156.74,160.64,159.13,159.2,161.25,159.58    #Unit5
]

random_region = [[26,30],[36,42],[81,90],[6,8]] 
time_modify_slots = []  #get from split_time_by_data

#random string for random folder name
def random_str(randomlength=5):
    str = ''
    chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'
    length = len(chars) - 1
    for i in range(randomlength):
        str+=chars[random.randint(0, length)]
    return str

def prepare_copyfolder(orig_folder, desti_folder):
	cp_cmd = 'xcopy %s %s /i /q'%(orig_folder, desti_folder)
	os.system(cp_cmd)

def split_time_by_data(total_time):
	digit_time_slots = []
	sumdt=0
	for region in random_region:
		digit_time_slots.append(float('%.2f'%(random.uniform(region[0],region[1]))))
	for digt in digit_time_slots:
		sumdt+=digt
	#adjust data to fit input total time
	#just adjust the biggest data time
	if sumdt > total_time:
		digit_time_slots[2] -= sumdt-total_time
	else:
		digit_time_slots[2] += total_time -sumdt
	#adjust AVT time to add video test idle time 5min to 30mi
	video_idle_time = float('%.2f'%(random.uniform(5, 30)))
	digit_time_slots[0] += video_idle_time
	print "video idle time", video_idle_time
	#check if match total time
	sumdt = 0
	for val in digit_time_slots:
		sumdt += val
	print "after ajust total time:", sumdt
	#switch to HH:MM:SS format
	for index,digt in enumerate(digit_time_slots):
		digit_time_slots[index] = time.strftime("%H:%M:%S", time.gmtime(digt*60))
	print "adjust time", digit_time_slots
	return digit_time_slots

if __name__ == "__main__":
  for array_dt in DATA_ARRAY:
  #1. copy folder, generate random folder name
      dest_folder = os.path.join('output','PC0'+random_str())
      prepare_copyfolder(template_folder, dest_folder)
  #2. split total time to avt, runin, preload, fvt times, limited by random_region
      time_modify_slots = split_time_by_data(array_dt)
  #3. modify folder file: all_group_items.xml
      dest_file = os.path.join(dest_folder,filenam)
      of = open(dest_file, 'r')
      ins = of.readlines()
      of.close()
  #
      ofw = open(dest_file, 'w')
  #
      for line in ins:
  	    if 'time' in line:
  	        for index, pattern in enumerate(pattern_tst_steps):
  	    	    if pattern in line:
  			        #print 'DEBUG>>',line
  			        indx = line.index('time=')
  			        #print 'DEBUG>>'+time_modify_slots[index]
  			        line=line.replace(line[indx+6:-3], time_modify_slots[index])
  			        #print 'DEBUG after>>'+line
  			        ofw.write(line)
      ofw.close()
      print "Update: %s"%(dest_file)