g++ -std=c++11 -lpthread main.cpp pthreads_smith_waterman_skeleton.cpp -o pthreads_smith_waterman
g++ -std=c++11 ./serial/main.cpp ./serial/serial_smith_waterman.cpp -o ./serial_smith_waterman

num=8
test=./datasets/input1.txt
echo $num
echo $test
./pthreads_smith_waterman $test $num
./serial_smith_waterman $test

test=./datasets/input2.txt
echo $num
echo $test
./pthreads_smith_waterman $test $num
./serial_smith_waterman $test

test=./datasets/input3.txt
echo $num
echo $test
./pthreads_smith_waterman $test $num
./serial_smith_waterman $test

test=./datasets/sample.in
echo $num
echo $test
./pthreads_smith_waterman $test $num
./serial_smith_waterman $test

test=./datasets/1k.in
echo $num
echo $test
./pthreads_smith_waterman $test $num
./serial_smith_waterman $test