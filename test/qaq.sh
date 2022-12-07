#!/usr/bin/env bash

# Start the server in the background
./code &

# Wait a few seconds for the server to start up
sleep 5

# Use `ab` (Apache Benchmark) to send 100 requests concurrently to the server
ab -n 100 -c 100 http://localhost:8080/

# Stop the server
pkill -f code
