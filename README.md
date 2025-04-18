# The-Dining-Philosophers
Implementation and resolution of the synchronization problem: The Dining Philosophers

## follow the steps to run project locally

### Option 1
you must have Docker installed in your machine, get on [Docker](https://www.docker.com)

- docker build -t app .
- docker run -p 8080:8080 app

- in your browser type localhost:8080
- or in terminal type: nc localhost:8080 or telnet localhost:8080

### Option 2
ONLY if you are in a Ubuntu machine. Make sure to have g++ compiler intalled

- g++ --version

getting gnu compilers <br>
- sudo apt update
- sudo apt install build-essential

compiling and run <br>
- g++ -o apl src/*.cpp
- ./apl

connect via another terminal
- nc localhost:8080 or telnet localhost:8080