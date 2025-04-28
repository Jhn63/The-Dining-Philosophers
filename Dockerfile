FROM ubuntu:latest
RUN apt update && apt install -y build-essential
WORKDIR /app
COPY .. /app
RUN g++ src/main.cpp src/semaphore.cpp src/posix.cpp src/posix_aging.cpp -o philosophers -I include -std=c++20
EXPOSE 8080
CMD ["./philosophers"]