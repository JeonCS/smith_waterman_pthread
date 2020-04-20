g++ -std=c++11 -lpthread main.cpp pthreads_smith_waterman_skeleton.cpp -o pthreads_smith_waterman
num=4
test=./datasets/1k.in
echo $num
echo $test
./pthreads_smith_waterman $test $num

g++ -std=c++11 ./serial/main.cpp ./serial/serial_smith_waterman.cpp -o ./serial_smith_waterman
./serial_smith_waterman $test
