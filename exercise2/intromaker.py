f = open('shoot.c', 'r')
lines = f.readlines()
f.close()
f = open('newshoot.c', 'w')
f.write(lines[0])
f.write(lines[1])
f.write(lines[2])

s = lines[3].split(',')
l = ''
for i in range(7000):
	l += s[i]
	if i != 6999:
		l += ','
f.write(l)
f.write('};')
f.close()
