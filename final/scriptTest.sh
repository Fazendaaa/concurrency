#/bin/bash

echo "2NodesSingle1"
make 2NodesSingle1

echo "2Nodes4Threads1"
make 2Nodes4Threads1

echo "2Nodes8Threads1"
make 2Nodes8Threads1

#4 cluster nodes
echo "4NodesSingle1"
make 4NodesSingle1

echo "4Nodes4Threads1"
make 4Nodes4Threads1

echo "4Nodes8Threads1"
make 4Nodes8Threads1

#8 cluster nodes
echo "8NodesSingle1"
make 8NodesSingle1

echo "8Nodes4Threads1"
make 8Nodes4Threads1

echo "8Nodes8Threads1"
make 8Nodes8Threads1

echo "MATRIZ 5000"
./criar << 50000
echo "2NodesSingle1"
make 2NodesSingle1

echo "2Nodes4Threads1"
make 2Nodes4Threads1

echo "2Nodes8Threads1"
make 2Nodes8Threads1

#4 cluster nodes
echo "4NodesSingle1"
make 4NodesSingle1

echo "4Nodes4Threads1"
make 4Nodes4Threads1

echo "4Nodes8Threads1"
make 4Nodes8Threads1

#8 cluster nodes
echo "8NodesSingle1"
make 8NodesSingle1

echo "8Nodes4Threads1"
make 8Nodes4Threads1

echo "8Nodes8Threads1"
make 8Nodes8Threads1

echo "MATRIZ 10000"
./criar << 100000

echo "2NodesSingle1"
make 2NodesSingle1

echo "2Nodes4Threads1"
make 2Nodes4Threads1

echo "2Nodes8Threads1"
make 2Nodes8Threads1

#4 cluster nodes
echo "4NodesSingle1"
make 4NodesSingle1

echo "4Nodes4Threads1"
make 4Nodes4Threads1

echo "4Nodes8Threads1"
make 4Nodes8Threads1

#8 cluster nodes
echo "8NodesSingle1"
make 8NodesSingle1

echo "8Nodes4Threads1"
make 8Nodes4Threads1

echo "8Nodes8Threads1"
make 8Nodes8Threads1
