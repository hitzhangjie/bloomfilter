#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sstream>

#include "Bloom.h"
#include "feeds.h"

void BloomSliceTest();
void BloomInstanceTest();
void BloomTest();
void BloomRebuild();
void BloomPerfTest();

int main(int argc, char ** argv) {

    //printf("BloomSlice Test:\n");
    //BloomSliceTest();

    //printf("BloomInstance Test:\n");
    //BloomInstanceTest();

    //printf("Bloom Test:\n");
    //BloomTest();

    //printf("Bloom Rebuild:\n");
    //BloomRebuild();

    printf("Bloom Perf test:\n");
    BloomPerfTest();

    return 0;
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

void BloomRebuild() {

    string key = "hello";

    Bloom * bloom = new Bloom(4000, 1, 1000, 2);

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

    // test Serialize/De-Serialize
    string buf;
    if (bloom->SaveBloom(buf)) {
        printf("bloom Serialize success, buf size:%d\n", buf.size());
    } else {
        return;
    }

    Bloom newBloom;
    if (newBloom.InitBloom(buf)) {
        printf("bloom De-Serialize success\n");
    } else {
        return;
    }

    if (newBloom.Test(key)) {
        printf("hello existed\n");
    } else {
        printf("hello not existed\n");
    }

    string any = "any";
    if (newBloom.Test(any)) {
        printf("any existed\n");
    } else {
        printf("any not existed\n");
    }

    // test reset
    if (newBloom.Reset(RESET_FIRST_INSTANCE_FIRST_SLICE)) {
        printf("reset success\n");
    } else {
        printf("reset failed\n");
    }

    if (newBloom.Test(key)) {
        printf("hello existed\n");
    } else {
        printf("hello not existed\n");
    }

    if (newBloom.Add(key)) {
        printf("add hello success\n");
    } else {
        printf("add hello failed\n");
    }

    if (newBloom.Test(key)) {
        printf("hello existed\n");
    } else {
        printf("hello not existed\n");
    }

}

void BloomPerfTest () {
    Bloom* b = new Bloom(4000, 1, 1000, 2);
    string key;

    for (int i=0; i<8000; i++) {

        //stringstream ss;
        //ss << "feedid_" << i;
        //key = ss.str();
        key = feeds[i];

        if(!b->Add(key)) {
            fprintf(stderr, "Add invalid, key: %s\n", key.c_str());
        }

        string buf;
        b->SaveBloom(buf);
        printf("add key -> %s, buf size -> %d\n", key.c_str(), buf.size());
    }
}
