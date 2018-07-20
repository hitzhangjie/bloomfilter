#ifndef BLOOM_FILTER
#define BLOOM_FILTER 

#include <stdint.h>
#include <vector>
#include <bitset>

using namespace std;

class Bloom;
class BloomInstance;
class BloomSlice;

// Bloom
class Bloom {
public:
    Bloom(int entries, int err_mode, int err_deno);
    ~Bloom();

    // 初始化（新创建 / pb重建）
    Bloom* InitBloom(string& pb);

    // Add / Test
    bool Add(string& key);
    bool Test(string& key);

private:
    BloomInstance* AddBloomInstance(int entries, int err_mode, int err_deno, int ins_num = 2, int slice_num = 2);
    bool RemBloomInstance(int idx);

private:
    // 新旧两个布隆实例 （instance0过渡期后作废）
    BloomInstance* m_instance;
    BloomInstance* m_instance0;

    // 过渡期时长 （单位秒）
    uint32_t m_trans_period;
};

// BloomInstance
class BloomInstance {
public:
    BloomInstance(int entries = 4000, int err_mode = 1, int err_deno = 1000, int slice_num = 2);
    ~BloomInstance();

    bool InitInstance(string& pb);

    bool Add(string& key);
    bool Test(string& key);
    bool Full();

private:
    BloomSlice* addSlice();
    BloomSlice* addSlice(vector<uint32_t>);
    bool removeSlice(int idx);

private:
    int m_entries;              // 计划存储多少元素
    int m_err_mode;             // 错误率分子
    int m_err_deno;             // 错误率分母
    int m_slice_num;            // slice数量

    // 每块slice错误率为(m_err_mode/m_err_deno) * (1/m_slice_num)
    // 每块slice各存储(m_entries / m_slice_num)
    vector<BloomSlice*> m_slices; 
};

// BloomSlice
class BloomSlice {
public:
    BloomSlice(int entries, double error);
    ~BloomSlice();

    bool InitSlice(string& pb);

    bool Add(string& key);
    bool Test(string& key);
    bool Full();

private:
    bool testOk(int pos, bool needSet = false);

private:
    uint32_t m_create_time;
    uint32_t m_access_time; 
    vector<bitset<64> > m_data;
    int m_bits;
    int m_hashes;
};

#endif
