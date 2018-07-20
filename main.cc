#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "BloomFilter.h"

void BloomSliceTest();
void BloomInstanceTest();
void BloomTest();

int main(int argc, char ** argv) {

    //printf("BloomSlice Test:\n");
    //BloomSliceTest();

    //printf("BloomInstance Test;\n");
    //BloomInstanceTest();

    printf("Bloom Test;\n");
    BloomTest();
}

void BloomSliceTest() {

    BloomSlice* slice = new BloomSlice(4000, 0.001);
    string key = "hello";
    if (slice->Test(key)) {
        printf("hello existed\n");
    } else {
        printf("hello not existed\n");
    }

    if (slice->Add(key)) {
        printf("hello added succ\n");
    } else {
        printf("hello added failed\n");
    }

    if (slice->Test(key)) {
        printf("hello existed\n");
    } else {
        printf("hello not existed\n");
    }
}

void BloomInstanceTest() {

    string key = "hello";

    BloomInstance* instance = new BloomInstance(4000, 1, 1000, 2);

    if (instance->Test(key)) {
        printf("hello existed\n");
    } else {
        printf("hello not existed\n");
    }

    if (instance->Add(key)) {
        printf("hello added succ\n");
    } else {
        printf("hello added failed\n");
    }

    if (instance->Test(key)) {
        printf("hello existed\n");
    } else {
        printf("hello not existed\n");
    }

}

void BloomTest() {

    string key = "hello";

    Bloom * bloom = new Bloom(4000, 1, 1000, 2);

    if (bloom->Test(key)) {
        printf("hello existed\n");
    } else {
        printf("hello not existed\n");
    }

    if (bloom->Add(key)) {
        printf("hello added succ\n");
    } else {
        printf("hello added failed\n");
    }

    if (bloom->Test(key)) {
        printf("hello existed\n");
    } else {
        printf("hello not existed\n");
    }

}
