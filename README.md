# Cab Booking System

A C++ console-based cab booking system demonstrating **data structures, graph algorithms, and file handling**.  

---

## Features

- Add, list, and manage **cabs** with unique IDs and drivers.
- Book rides based on **nearest available cab** using **Dijkstra's shortest path**.
- End rides, calculate **distance and fare**, and mark cabs as available.
- Track **active and completed rides** with full ride paths.
- Persistent storage using `cabs.txt` to save and load cab data.
- Fully connected graph of **nodes 0–9** to avoid unreachable destinations.
- Neatly formatted tables with **borders** for all listings.

---

## Technologies & Concepts Used

| Concept/Tech             | Description                                                      |
|---------------------------|------------------------------------------------------------------|
| C++                       | Programming language used                                        |
| Classes & Structs         | Encapsulation of `Cab` and `Ride` data                           |
| STL (map, vector, queue)  | Data structures for storing cabs, rides, and graph nodes         |
| File I/O                  | Persistent storage of cab data (`cabs.txt`)                      |
| Graph Algorithms          | Dijkstra's algorithm for shortest path calculation               |
| Console UI                | Clean, bordered tables for data display                           |

---

## Compilation
g++ cab_booking_system.cpp -o cab_booking_system

## Run
./cab_booking_system

## Usage
1. Add Cab
2. List Cabs
3. Book Ride
4. List Active Rides
5. End Ride
6. List Completed Rides
0. Exit
Choose: 

** Cab Table**

| ID  | Driver | Location | Available | RideID |
| --- | ------ | -------- | --------- | ------ |
| 101 | Arun   | 0        | Yes       | -      |
| 102 | Sunil  | 5        | No        | 1      |


**Active Rides**

| RideID | CabID | Pickup | Drop | Dist | InProgress | Path       |
|--------|-------|--------|------|------|------------|------------|
| 1      | 102   | 5      | 8    | 10   | Yes        | 5->6->7->8 |


**Queued Rides**

| Pos | Pickup Node |
| --- | ----------- |
| 1   | 3           |
| 2   | 7           |



## File Structure
| File/Folder              | Description                                     |
| ------------------------ | ----------------------------------------------- |
| `cab_booking_system.cpp` | Main C++ source code for the cab booking system |
| `cabs.txt`               | Stores persistent cab information               |
| `graph.txt`              | (Optional) Stores road connections if needed    |
| `README.md`              | Project documentation (this file)               |
| `build/`                 | Folder to store compiled binaries               |

## Future Enhancements

GUI integration for better user experience.

Dynamic fare calculation based on time of day and traffic simulation.

Multiple cab types (standard, premium) with different fare rates.

Integration with a database for real-time multi-user simulation.



