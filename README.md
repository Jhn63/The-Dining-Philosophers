# The Dining Philosophers

This project implements a solution to the classic concurrency problem: The Dining Philosophers. This problem illustrates synchronization issues and techniques in concurrent programming, dealing with resource allocation and deadlock prevention.

## The Problem

The Dining Philosophers problem was first proposed by Edsger Dijkstra in 1965 to illustrate synchronization issues in concurrent programming. The problem is set up as follows:

- Five philosophers sit around a circular table
- Each philosopher has a plate of spaghetti in front of them
- Between each pair of plates is one fork (5 forks total)
- A philosopher needs two forks to eat (their own fork and the one to their right)
- When not eating, philosophers are thinking
- A philosopher can only pick up one fork at a time
- A philosopher must put down both forks after eating

The challenge is to design a solution where:
1. No philosopher will starve (they will all eventually get to eat)
2. No deadlocks occur (where all philosophers hold one fork and wait indefinitely for another)
3. Maximum concurrency is allowed (philosophers who can eat should be allowed to do so)

This seemingly simple setup illustrates core challenges in concurrent programming:
- Resource allocation
- Deadlock prevention
- Starvation avoidance
- Process synchronization


The implementation offers three different synchronization approaches:
1. Using Semaphores
2. Using POSIX Monitors
3. Using POSIX Monitors with Aging (anti-starvation mechanism)

## Interface

The application provides a simple text-based interface where users can select which implementation to run:

```
 ######   ##   ##  #######           #####     ####    ##   ##   ####    ##   ##    ####            ######   ##   ##   ####    ####      #####    #####    #####   ######   ##   ##  #######  ######    #####
 # ## #   ##   ##   ##   #            ## ##     ##     ###  ##    ##     ###  ##   ##  ##            ##  ##  ##   ##    ##      ##      ##   ##  ##   ##  ##   ##   ##  ##  ##   ##   ##   #   ##  ##  ##   ##
   ##     ##   ##   ## #              ##  ##    ##     #### ##    ##     #### ##  ##                 ##  ##  ##   ##    ##      ##      ##   ##  #        ##   ##   ##  ##  ##   ##   ## #     ##  ##  #
   ##     #######   ####              ##  ##    ##     ## ####    ##     ## ####  ##                 #####   #######    ##      ##      ##   ##   #####   ##   ##   #####   #######   ####     #####    #####
   ##     ##   ##   ## #              ##  ##    ##     ##  ###    ##     ##  ###  ##  ###            ##      ##   ##    ##      ##   #  ##   ##       ##  ##   ##   ##      ##   ##   ## #     ## ##        ##
   ##     ##   ##   ##   #            ## ##     ##     ##   ##    ##     ##   ##   ##  ##            ##      ##   ##    ##      ##  ##  ##   ##  ##   ##  ##   ##   ##      ##   ##   ##   #   ##  ##  ##   ##
  ####    ##   ##  #######           #####     ####    ##   ##   ####    ##   ##    #####           ####     ##   ##   ####    #######   #####    #####    #####   ####     ##   ##  #######  #### ##   #####

1. Método por Semáforo
2. Método com monitores POSIX
3. Método com monitores POSIX e Aging (Anti-Starvation)
4. Sair
Escolha uma opção:
```

## Implementations

### 1. Semaphores
This implementation uses semaphores to control access to the chopsticks but doesn't guarantee that no deadlock occurs.

### 2. POSIX Monitors
This implementation uses POSIX monitors (mutexes and condition variables) to coordinate dining philosophers and prevent deadlock.

### 3. POSIX Monitors with Aging
This implementation extends the POSIX Monitors approach by adding an aging mechanism to prevent starvation. Key features include:

- Each philosopher has a waiting counter that increases when they're unable to eat
- The system tracks timestamps of each philosopher's last meal
- A starvation threshold (10 seconds) is established
- Philosophers who exceed this threshold receive higher priority
- A priority-based selection method ensures that hungry philosophers who've waited the longest eat first
- After eating, a philosopher's waiting time resets to zero

This approach ensures that no philosopher can be indefinitely blocked from eating, as their priority continuously increases until they're selected, even in unfavorable positions.

## How to Compile and Execute

### Prerequisites
- C++ compiler with C++20 support
- Make sure you have the necessary directory structure: `bin/`, `include/`, and `src/`

### Compilation
Run the following command to compile the project:

```bash
g++ src/main.cpp src/semaphore.cpp src/posix.cpp src/posix_aging.cpp -o bin/philosophers -I include -std=c++20
```

### Running the Application

#### Option 1: Using Docker
If you have Docker installed, you can build and run the application as follows:

```bash
# Build Docker image
docker build -t philosophers .

# Run the container
docker run -p 8080:8080 philosophers
```

Then connect to the application:
- In your browser: `localhost:8080`
- Or in terminal: `nc localhost:8080` or `telnet localhost:8080`

#### Option 2: Running Locally
After compilation, execute the binary:

```bash
# Run the program
./bin/philosophers
```

## Usage Instructions

1. Choose an option from the menu by entering the corresponding number:
   - Option 1: Run the Dining Philosophers problem with Semaphore implementation
   - Option 2: Run the Dining Philosophers problem with POSIX Monitors implementation
   - Option 3: Run the Dining Philosophers problem with POSIX Monitors and Aging mechanism
   - Option 4: Exit the program

2. Observe the philosophers' states as they think and eat, demonstrating the synchronization mechanism in action.

## Project Contributors

| Name | Contributions |
|------|--------------|
| Paulo | Created the synchronization methods using both Semaphores and POSIX Monitors; Also defined the interface. |
| João | [Description of contributions] |