q1: q1.c
	gcc -pthread q1.c -o q1

q2: q2.c
	gcc -pthread q2.c -o q2 -lm

rand_c: random.c
	gcc random.c -o rand

rand_r: rand_c
	./rand

e1: q1
	./q1 $(t_c)
	
e2: q2 rand_r
	./q2 $(t_c)