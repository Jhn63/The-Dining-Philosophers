FROM ubuntu:latest
RUN apt update && apt install -y g++
COPY . /app
WORKDIR /app
RUN g++ -o apl src/main.cpp
EXPOSE 8080
CMD ["./apl"]