#include <iostream>
#include <pthread.h>

void* print(void* arg) {
    std::cout << "hello world!" << std::endl;
    pthread_exit(nullptr);
}

int main(int argc, char** argv) {
    pthread_t thr;

    pthread_create(&thr, nullptr, print, nullptr);
    pthread_join(thr, nullptr);

    std::cout << "success!";
    return 0;
}
