%!/bin/sh

../irenaeus -c l James2:3>test1.dat

../irenaeus -c 1 l James2:3 2 l Romans3:2 1 n 2 n >>test1.dat

echo 'l James3:2 q' |../irenaeus -f >>test1.dat
