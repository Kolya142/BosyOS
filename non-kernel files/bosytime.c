#include <stdio.h>
#include <time.h>

#define BOSYEPOCH (unsigned long long)1735689600

int main() {
    printf("time since bosy-epoch: %lld\n", time(NULL) - BOSYEPOCH);
}