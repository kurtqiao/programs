Andriod APP to connect Intel Edison board by bluetooth
====

1. pair your phone with edison board
2. modify SPP_loopback.py 
   when get "light on", pull high GPIO8
   when get "light off", pull low GPIO8
3. run SPP_loopback.py in edison board
4. run andriod app and enjoy 