#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "BloomFilter.h"
#include "murmurhash2.h"

#define MAKESTRING(n) STRING(n)
#define STRING(n) #n

//==============================================================================
// Bloom
Bloom::Bloom(int entries, int err_mode, int err_deno, int slice_num) {

    m_trans_period = TRANSITION_PERIOD_SECONDS;

    BloomInstance* instance = new BloomInstance(entries, err_mode, err_deno, slice_num);
    m_instances.push_back(instance);
}

Bloom::~Bloom() {
}

Bloom* InitBloom(string& pb) {
    // parse from pb message...fixme!!!
    return NULL;
}

bool Bloom::Add(string& key) {

    if(m_instances.size() == 0) {
        return false;
    }
    BloomInstance* instance = *(m_instances.rbegin());
    return instance->Add(key);
}

bool Bloom::Test(string& key) {

    for (auto it = m_instances.begin(); it != m_instances.end(); it++) {
        BloomInstance* instance = *it;
        if (instance->Test(key)) {
            return true;
        }
    }
    return false;
}

bool Bloom::GetParams(int& entries, int& err_mode, int& err_deno, int& slice_num) {

    if(m_instances.size() == 0) {
        return false;
    }
    BloomInstance* instance = *(m_instances.rbegin());

    entries = instance->GetEntries();
    err_mode = instance->GetErrMode();
    err_deno = instance->GetErrDeno();
    slice_num = instance->GetSliceNum();
}

BloomInstance* Bloom::NewBloomInstance(int entries, int err_mode, int err_deno, int slice_num) {
    m_trans_period = TRANSITION_PERIOD_SECONDS;
    
    BloomInstance* instance = new BloomInstance(entries, err_mode, err_deno, slice_num);
    m_instances.push_back(instance);
}

//==============================================================================
// BloomInstance
BloomInstance::BloomInstance(int entries, int err_mode, int err_deno, int slice_num) 
    : m_entries(entries), m_err_mode(err_mode), m_err_deno(err_deno), m_slice_num(slice_num) {

    double avg_entries = entries / slice_num;
    double avg_error = ((double)err_mode / (double)err_deno) / m_slice_num;

    for (int i = 0; i < m_slice_num; i++) {
        BloomSlice* slice = new BloomSlice(avg_entries, avg_error);
        m_slices.push_back(slice);
    }

    m_create_time = time(NULL);
}

BloomInstance::~BloomInstance() {
    for (auto it = m_slices.begin(); it != m_slices.end(); it++) {
        delete *it;
    }
}

bool BloomInstance::InitInstance(string& pb) {
    // parse from pb message...fixme!!!
    return true;
}

bool BloomInstance::Add(string& key) {

    for (auto it = m_slices.begin(); it != m_slices.end(); it++) {
        BloomSlice* slice = *it;
        if (!slice->Full()) {
            if(slice->Add(key)) {
                return true;
            } else {
                return false;
            }
        } 
    }
    return false;
}

bool BloomInstance::Test(string& key) {
    for (auto it = m_slices.begin(); it != m_slices.end(); it++) {
        BloomSlice* slice = *it;

        if (slice->Test(key)) {
            return true;
        }
    }

    return false;
}

bool BloomInstance::Full() {

    for (auto it = m_slices.begin(); it != m_slices.end(); it++) {
        BloomSlice* slice = *it;

        if (!slice->Full()) {
            return false;
        }
    }
    return true;
}

BloomSlice* BloomInstance::addSlice() { return NULL; }
BloomSlice* BloomInstance::addSlice(vector<uint32_t> data) { return NULL; };
bool BloomInstance::removeSlice(int idx) { return true; };

//==============================================================================
// BloomSlice

BloomSlice::BloomSlice(int entries, double error) {

    double num = log(error);
    double denom = 0.480453013918201; // ln(2)^2
    double bpe = -(num / denom);

    int bits = (int)(entries * bpe);
    int hashes = (int)ceil(0.693147180559945 * bpe); // ln(2)

    m_bits = bits;
    m_hashes = hashes;

    int elements;
    if (bits % 64) {
        elements = (bits / 64) + 1;
    } else {
        elements = bits / 64;
    }
    //printf("entries:%d, error:%lf, bits:%d, hashes:%d, elements:%d\n", entries, error, bits, hashes, elements);

    bitset<64> bit64;
    m_data.resize(elements, bit64);
}

BloomSlice::~BloomSlice() {}

bool BloomSlice::InitSlice(string& pb) {

    // parse from pb message BloomSlice, fixme!!!

    return true;
}

// if add success return true, or return false
bool BloomSlice::Add(string& key) {

    register unsigned int a = murmurhash2(key.c_str(), strlen(key.c_str()), 0x9747b28c);
    register unsigned int b = murmurhash2(key.c_str(), strlen(key.c_str()), a);

    int hits = 0;

    register unsigned int x;
    register unsigned int i;

    for (i = 0; i < m_hashes; i++) {
        x = (a + i*b) % m_bits;
        if (testOk(x, true)) {
            hits++; 
        }
    }

    if (hits == m_hashes) {
        return false;
    }

    return true;
}

// if bit at pos is set return true, or return false
bool BloomSlice::testOk(int pos, bool needSet) {

    unsigned int bitset_idx = pos >> 6;

    if (m_data.size() <= bitset_idx) {
        fprintf(stderr, "data maybe corrupted\n");
        return false;
    }

    if (m_data[bitset_idx].test(pos % 8)) {
        return true;
    } else {
        if (needSet) {
            m_data[bitset_idx].set(pos % 8);
        }
        return false;
    }
}

// if key is already stored, returns true
bool BloomSlice::Test(string& key) {
    
    register unsigned int a = murmurhash2(key.c_str(), strlen(key.c_str()), 0x9747b28c);
    register unsigned int b = murmurhash2(key.c_str(), strlen(key.c_str()), a);

    int hits = 0;

    register unsigned int x;
    register unsigned int i;

    for (i = 0; i < m_hashes; i++) {
        x = (a + i*b) % m_bits;
        //printf("hash:%d, bit pos: %d\n", i, x);
        if (testOk(x)) {
            hits++; 
        }
    }

    if (hits == m_hashes) {
        return true;
    }

    return false;
}

// if bits of ones takes up equal or larger than one half of m_bits, returns full
bool BloomSlice::Full() {

    int ones = 0;
    for (auto it = m_data.begin(); it != m_data.end(); it++) {
        ones += it->count();
    }

    if (ones >= m_bits / 2) {
        return true;
    }

    return false;
}

