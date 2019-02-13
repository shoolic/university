#!/bin/sh
for PRI in 1 2 3
do
	(time ./test $PRI &)
done


