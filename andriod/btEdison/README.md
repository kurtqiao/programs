Andriod APP to connect Intel Edison board by bluetooth
====

1. pair your phone with edison board  
2. modify SPP_loopback.py   
   when get "light on", pull high GPIO8  
   when get "light off", pull low GPIO8  
   you can refert to SPP_loopback.py in this folder  
3. run ./gpio8.sh to setup intel edison board GPIO8  
4. make sure bluetooth paired your phone with edison board  
5. run SPP_loopback.py in edison board as background by  
   python SPP_loopback.py &  

6. run andriod app and enjoy


    > To connect intel edison board through bluetooth SSP, you have to run DBUS APIs on edison board  
    > please refer intel document: http://download.intel.com/support/edison/sb/edisonbluetooth_331704007.pdf  
    > 1. download SPP-loopback.py and run as background in edison by: python SPP-loopback.py &  
    > 2. you probably need to pair your phone, if so, do belows before step 1  
    > 2.1 connect edison with putty.exe  
    > 2.2 rfkill unblock bluetooth  
    >    hciconfig hci0 up   [optional]  
    >    hciconfig hci0      [optional] check your bluetooth up  
    >    bluetoothctl  
    >2.3 when enter bluetooth environment,  
    >    pair Yo:ur:Ad:dr:es:s0  
    >    trust Yo:ur:Ad:dr:es:s0  
    >    'paired-device' to check if your phone paired success  
    >    now use 'show' to check if content 'UUID: Serial Port'  
    >    if yes, means your edison bluetooth can connect by SPP now  
   