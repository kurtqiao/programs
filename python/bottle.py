'''
little puzzle: how many drinks would you get?
a bottle of wine: 2$
exchange rule:
  2 bottle for 1 wine
  4 caps for 1 wine
usage:
  python bottle.py integer
  integer = money
'''
import sys
wine = 0
bottle = 0
cap = 0
def buy_wine(money):
	global wine
	wine = money / 2
	return money % 2

def drink(w):
	global bottle, cap
	bottle += w
	cap += w
	return (bottle, cap)

def exchange(b, h):
	global bottle, cap, wine
	w = 0
	w += b / 2
	w += h / 4
	bottle = b%2
	cap = h%4
	wine += w
	return (w)

def rock_n_roll(w):
	global bottle, cap, wine
	print 'wine', w
	if w == 0:
		return
	else:
		b, h = drink(w)
		print 'drink up: bottle', bottle, 'cap', cap
		w = exchange(b, h)
		print 'do exchange: wine', w, 'bottle', bottle, 'cap', cap
		rock_n_roll(w)

if __name__ == '__main__':
        arg = sys.argv[1]
        restmoney = buy_wine(int(arg))
        rock_n_roll(wine)
        print '\ndrunk total wine',wine,'lefted: bottle',bottle,'cap',cap 
        if restmoney != 0:
            print 'you still got money:', restmoney,'$'
