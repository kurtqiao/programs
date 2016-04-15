'''
little puzzle: how many drinks would you get?
a bottle of wine: 2$
exchange rule:
  2 bottle for 1 wine
  4 caps for 1 wine
'''
wine = 0
bottle = 0
cap = 0
def buy_wine(money):
	global wine
	wine = money / 2
	rest_money = money % 2
	return rest_money

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
	b = 0
	h = 0
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
	 rstmoney = buy_wine(21)
	 rock_n_roll(wine)
	 print '\ndrinked total wine',wine,'lefted: bottle',bottle,'cap',cap 
	 if rstmoney != 0:
		 print 'you still got money:', rstmoney,'$'
