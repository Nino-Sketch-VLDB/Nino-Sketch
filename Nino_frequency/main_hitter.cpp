#include "NinoSketch.hpp"
#include "adaptor.hpp"
#include <cstdlib>
#include <unordered_map>
#include <math.h>
#include <utility>
#include <iomanip>
#include "datatypes.hpp"
#include "util.h"

bool sortbysec(const std::pair<key_tp, val_tp> &a,
               const std::pair<key_tp, val_tp> &b)
{
    return (a.second > b.second);
}

int main(int argc, char *argv[])
{
    int total_memory;
    int l1_memory_size;
    int l2_memory_size;

    std::cin >> total_memory;

    l1_memory_size = total_memory / 4;
    l2_memory_size = total_memory - l1_memory_size;

    double aae = 0;

    int use_flag = false;

    int bucket_size = L1_COUNTER + L1_FP;
    int cell_size = L2_SIZE;

    int l1_depth = 2;
    int l2_depth = 2;

    int l1_width = l1_memory_size * 1024 * 8 / (bucket_size * l1_depth);
    int l2_width = l2_memory_size * 1024 * 8 / (cell_size * l2_depth);

    int sumerror = 0;

    const char *filenames = "iptraces.txt";
    unsigned long long buf_size = 5000000000;

    double thresh = 0.0001; //heavy hitter threshold

    std::vector<std::pair<l2_fp_tp, val_tp>> results;
    double precision = 0, recall = 0, error = 0;
    double avpre = 0, avrec = 0, averr = 0, averaae = 0;

    std::ifstream tracefiles(filenames);
    if (!tracefiles.is_open())
    {
        std::cout << "Error opening file" << std::endl;
        return -1;
    }

    for (std::string file; getline(tracefiles, file);)
    {

        Adaptor *adaptor = new Adaptor(file, buf_size);
        std::cout << "[Dataset]: " << file << std::endl;

        adaptor->Reset();
        mymap ground;
        mymap ground2;
        val_tp sum = 0;
        val_tp epoch = 0;
        val_tp window_size = 1600;
        val_tp LENGTH = 0;
        tuple_t t;
        while (adaptor->GetNext(&t) == 1)
        {
            sum++;
        }
        LENGTH = ceil((sum - 1) / window_size);
        adaptor->Reset();
        while (adaptor->GetNext(&t) == 1)
        {
            sum += 1;
            key_tp key;
            memcpy(key.key, &(t.key), LGN);
            if (ground.find(key) != ground.end())
            {
                ground[key] += 1;
            }
            else
            {
                ground[key] = 1;
            }
        }
        std::vector<std::pair<key_tp, val_tp>> v_ground;
        for (auto it = ground.begin(); it != ground.end(); it++)
        {
            std::pair<key_tp, val_tp> node;
            node.first = it->first;
            node.second = it->second;
            v_ground.push_back(node);
        }
        std::sort(v_ground.begin(), v_ground.end(), sortbysec);
        val_tp threshold = thresh * sum;

        // Create sketch
        MESketch *sketch = new MESketch(l1_depth, l1_width, l2_depth, l2_width, 8 * LGN, use_flag);

        //  Update sketch
        uint64_t t1 = 0, t2 = 0;
        adaptor->Reset();
        memset(&t, 0, sizeof(tuple_t));
        int number = 0;
        while (adaptor->GetNext(&t) == 1)
        {
            sketch->Update((unsigned char *)&(t.key), 1);
        }
        results.clear();
        sketch->Query(threshold, results);

        // evaluate mae, mape
        int ae = 0, cnt = 0;
        float re = 0;
        for (auto it = v_ground.begin(); it != v_ground.end(); it++)
        {
            cnt++;
            ae += abs((int)sketch->QueryL2(it->first.key) - (int)it->second);
            re += (float)abs((int)sketch->QueryL2(it->first.key) - (int)it->second) / (float)it->second;
        }
        float aae = (float)ae / (float)cnt;
        float are = (float)re / (float)cnt;
        std::cout << "\nAAE (Frequency Estimation): " << aae << std::endl;
        std::cout << "ARE (Frequency Estimation): " << are << std::endl;

        // Evaluate accuracy
        error = 0;
        int tp = 0;
        cnt = 0;
        aae = 0;
        for (auto it = ground.begin(); it != ground.end(); it++)
        {
            int flag = 0;
            if (it->second > threshold)
            {
                cnt++;
                for (auto res = results.begin(); res != results.end(); res++)
                {
                    fp_pair pair = sketch->Key2FP(it->first.key, LGN);
                    std::bitset<L2_FP> *l2_fp = pair.second;
                    if (*l2_fp == res->first)
                    {
                        double hh = res->second > it->second ? res->second - it->second : it->second - res->second;
                        flag = 1;
                        sumerror += (int)hh;
                        error = hh * 1.0 / it->second + error;
                        aae += hh;
                        tp++;
                    }
                }
            }
        }
        precision = tp * 1.0 / results.size();
        recall = tp * 1.0 / cnt;
        error = error / tp;
        aae = aae * 1.0 / tp;

        avpre += precision;
        avrec += recall;
        averr += error;
        averaae += aae;
        // delete sketch;
        delete adaptor;

        std::cout<<"\nPrecision (Heavy Hitter Detection) "<<precision<<std::endl;
	std::cout<<"Recall (Heavy Hitter Detection) "<<recall<<std::endl;
	std::cout<<"ARE (Heavy Hitter Detection) "<<error<<std::endl;
    }
}
