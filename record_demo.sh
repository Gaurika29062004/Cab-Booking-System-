#!/bin/bash

# Remove old recording if exists
[ -f cab_demo.cast ] && rm cab_demo.cast

# Start recording
asciinema rec --overwrite cab_demo.cast --command "/bin/bash -c '
clear
echo \"Starting Cab Booking Demo...\"
sleep 1

echo
echo \"=== List Cabs ===\"
sleep 1
./cab_booking_system <<EOF
2
EOF
sleep 2

echo
echo \"=== Book Ride at pickup 0 ===\"
sleep 1
./cab_booking_system <<EOF
3
0
EOF
sleep 2

echo
echo \"=== Book Ride at pickup 1 ===\"
sleep 1
./cab_booking_system <<EOF
3
1
EOF
sleep 2

echo
echo \"=== List Cabs after booking ===\"
sleep 1
./cab_booking_system <<EOF
2
EOF
sleep 2

echo
echo \"=== Cancel Ride 1 (Cab 101) ===\"
sleep 1
./cab_booking_system <<EOF
7
1
EOF
sleep 2

echo
echo \"=== List Cabs after cancellation & auto-assign ===\"
sleep 1
./cab_booking_system <<EOF
2
EOF
sleep 2

echo
echo \"=== List Queued Rides ===\"
sleep 1
./cab_booking_system <<EOF
8
EOF
sleep 2

echo
echo \"=== End Ride 6 (auto-assigned) ===\"
sleep 1
./cab_booking_system <<EOF
5
6
EOF
sleep 2

echo
echo \"=== List Completed Rides ===\"
sleep 1
./cab_booking_system <<EOF
6
EOF
sleep 2

echo
echo \"=== Exit ===\"
sleep 1
./cab_booking_system <<EOF
0
EOF
'"

