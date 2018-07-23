#ifndef BLOOM_FILTER
#define BLOOM_FILTER 

#include <stdint.h>
#include <vector>
#include <bitset>

using namespace std;

class Bloom;
class BloomInstance;
class BloomSlice;

#define TRANSITION_PERIOD_SECONDS 7*24*3600

// Bloom
class Bloom {
public:
    Bloom(int entries, int err_mode, int err_deno, int slice_num = 2);
    Bloom();
    ~Bloom();

    bool InitBloom(string& pb);         // 从pb数据反串行化bloom
    bool SaveBloom(string& buf);        // 将bloom串行化为pb数据

    bool Add(string& key);              // Add key
    bool Test(string& key);             // Test key
    bool Reset(bool global = false);    // 重置

                                        // 容量、错误率调整时新建布隆实例
    BloomInstance* NewBloomInstance(int entries, int err_mode, int err_deno, int slice_num);

private:
    vector<BloomInstance*> m_instances;     // 多个布隆过滤器实例
    uint32_t m_trans_period;                // 过渡期时长，单位"秒"
};

// BloomInstance
class BloomInstance {
public:
    BloomInstance(int entries, int err_mode, int err_deno, int slice_num = 2);
    BloomInstance();
    ~BloomInstance();

    bool Add(string& key);
    bool Test(string& key);
    bool Full();

    int GetEntries() { return m_entries; }
    int GetErrMode() { return m_err_mode; }
    int GetErrDeno() { return m_err_deno; }
    int GetSliceNum() { return m_slice_num; }
    int GetCreateTime() { return m_create_time; }
    vector<BloomSlice*> GetSlices() { return m_slices; }

    void SetEntries(int entries) { m_entries = entries; }
    void SetErrMode(int err_mode) { m_err_mode = err_mode; }
    void SetErrDeno(int err_deno) { m_err_deno = err_deno; }
    void SetSliceNum(int slice_num) { m_slice_num = slice_num; }
    void SetCreateTime(uint32_t create_time) { m_create_time = create_time; }
    void AddSlice(BloomSlice* slice) { if (slice) m_slices.push_back(slice); }

private:
    //BloomSlice* addSlice();
    //BloomSlice* addSlice(vector<uint32_t>);
    //bool removeSlice(int idx);

private:
    int m_entries;              // 计划存储多少元素
    int m_err_mode;             // 错误率分子
    int m_err_deno;             // 错误率分母
    int m_slice_num;            // slice数量

    // 每块slice错误率为(m_err_mode/m_err_deno) * (1/m_slice_num)
    // 每块slice各存储(m_entries / m_slice_num)
    vector<BloomSlice*> m_slices; 

    uint32_t m_create_time;
};

// BloomSlice
class BloomSlice {
public:
    BloomSlice(int entries, double error);
    BloomSlice();
    ~BloomSlice();

    bool Add(string& key);
    bool Test(string& key);
    bool Full();

    uint32_t GetCreateTime() { return m_create_time; }
    uint32_t GetAccessTime() { return m_access_time; }
    vector<bitset<64>> GetData() { return m_data; }
    int GetBits() { return m_bits; }
    int GetHashes() { return m_hashes; }

    void SetCreateTime(uint32_t create_time) { m_create_time = create_time; }
    void SetAccessTime(uint32_t access_time) { m_access_time = access_time; }
    void SetData(vector<bitset<64>>& data) { m_data = data; }
    void SetBits(int bits) { m_bits = bits; }
    void SetHashes(int hashes) { m_hashes = hashes; }

private:
    bool testOk(int pos, bool needSet = false);

private:
    uint32_t m_create_time;
    uint32_t m_access_time; 
    vector<bitset<64>> m_data;
    int m_bits;
    int m_hashes;
};

#endif
