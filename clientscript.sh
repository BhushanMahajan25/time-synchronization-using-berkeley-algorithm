#!/bin/bash
for i in {1..100}
    do
        echo "spawning a client ${i}"
        ./client 10.0.0.237 8080 input-files/Transactions.txt &
        sleep 0.3s
    done

