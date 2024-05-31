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

    int bucket_size = L1_COUNTER + L1_FP + 1;
    int cell_size = L2_SIZE;

    int l1_depth = 2;
    int l2_depth = 2;

    int l1_width = l1_memory_size * 1024 * 8 / (bucket_size * l1_depth);
    int l2_width = l2_memory_size * 1024 * 8 / (cell_size * l2_depth);

    int sumerror = 0;

    const char *filenames = "iptraces.txt";
    unsigned long long buf_size = 5000000000;

    double thresh = 0.2; //persistent item threshold

    std::vector<std::pair<l2_fp_tp, val_tp>> results;
    int numfile = 0;
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
        memset(&t, 0, sizeof(tuple_t));
        while (adaptor->GetNext(&t) == 1)
        {
            key_tp key;
            memcpy(key.key, &(t.key), LGN);
            epoch = epoch + 1;
            if ((epoch) % LENGTH == 0)
            {
                for (auto &item : ground2)
                {
                    ground[item.first] += 1;
                }
                ground2.clear();
            }

            else
            {
                ground2[key] = 1;
            }
        }
        if (epoch % LENGTH != 0)
        {
            for (auto &item : ground2)
            {
                ground[item.first] += 1;
            }
            ground2.clear();
        }

        val_tp threshold = thresh * window_size;
        std::vector<std::pair<key_tp, val_tp>> v_ground;
        for (auto it = ground.begin(); it != ground.end(); it++)
        {
            std::pair<key_tp, val_tp> node;
            node.first = it->first;
            node.second = it->second;
            v_ground.push_back(node);
        }
        std::sort(v_ground.begin(), v_ground.end(), sortbysec);

        MESketch *sketch = new MESketch(l1_depth, l1_width, l2_depth, l2_width, 8 * LGN);
        uint64_t t1 = 0, t2 = 0;
        adaptor->Reset();
        memset(&t, 0, sizeof(tuple_t));
        int number = 0;
        while (adaptor->GetNext(&t) == 1)
        {
            ++number;
            if (number % LENGTH == 0)
            {
                sketch->NewWindow();
            }
            sketch->Update((unsigned char *)&(t.key), 1);
        }

        results.clear();
        sketch->Query(threshold, results);

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
        std::cout << "\nAAE (Persistence Estimation): " << aae << std::endl;
        std::cout << "ARE (Persistence Estimation): " << are << std::endl;

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
                    fp_pair p = sketch->Key2FP(it->first.key, LGN);
                    std::bitset<L2_FP> *l2_fp = p.second;
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
        delete adaptor;

        std::cout<<"\nPrecision (Persistent Item Lookup) "<<precision<<std::endl;
	std::cout<<"Recall (Persistent Item Lookup) "<<recall<<std::endl;
	std::cout<<"ARE (Persistent Item Lookup) "<<error<<std::endl;
}
}
