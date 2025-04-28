FROM ubuntu:latest
RUN apt update && apt install -y build-essential
WORKDIR /app
COPY .. /app
RUN make
EXPOSE 8080
CMD ["./apl"]