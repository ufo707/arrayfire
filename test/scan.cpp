#include <gtest/gtest.h>
#include <arrayfire.h>
#include <af/dim4.hpp>
#include <af/traits.hpp>
#include <af/array.h>
#include <vector>
#include <iostream>
#include <string>
#include <testHelpers.hpp>
#include <af/device.h>

using std::vector;
using std::string;
using std::cout;
using std::endl;
using af::af_cfloat;
using af::af_cdouble;

typedef af_err (*scanFunc)(af_array *, const af_array, const int);

template<typename Ti, typename To, scanFunc af_scan>
void scanTest(string pTestFile, int off = 0, bool isSubRef=false, const vector<af_seq> seqv=vector<af_seq>())
{
    vector<af::dim4> numDims;

    vector<vector<int>> data;
    vector<vector<int>> tests;
    readTests<int,int,int> (pTestFile,numDims,data,tests);
    af::dim4 dims       = numDims[0];

    vector<Ti> in(data[0].begin(), data[0].end());

    af_array inArray   = 0;
    af_array outArray  = 0;
    af_array tempArray = 0;

    int nDevices = 0;
    ASSERT_EQ(AF_SUCCESS, af_get_device_count(&nDevices));

    for (int dev = 0; dev < nDevices; dev++) {

#if defined(AF_CPU)
        ASSERT_EQ(AF_ERR_RUNTIME, af_set_device(dev));
#else
        ASSERT_EQ(AF_SUCCESS, af_set_device(dev));
#endif

        // Get input array
        if (isSubRef) {
            ASSERT_EQ(AF_SUCCESS, af_create_array(&tempArray, &in.front(), dims.ndims(), dims.get(), (af_dtype) af::dtype_traits<Ti>::af_type));
            ASSERT_EQ(AF_SUCCESS, af_index(&inArray, tempArray, seqv.size(), &seqv.front()));
        } else {

            ASSERT_EQ(AF_SUCCESS, af_create_array(&inArray, &in.front(), dims.ndims(), dims.get(), (af_dtype) af::dtype_traits<Ti>::af_type));
        }

        // Compare result
        for (int d = 0; d < (int)tests.size(); ++d) {
            vector<To> currGoldBar(tests[d].begin(), tests[d].end());

            // Run sum
            ASSERT_EQ(AF_SUCCESS, af_scan(&outArray, inArray, d + off));

            // Get result
            To *outData;
            outData = new To[dims.elements()];
            ASSERT_EQ(AF_SUCCESS, af_get_data_ptr((void*)outData, outArray));

            size_t nElems = currGoldBar.size();
            for (size_t elIter = 0; elIter < nElems; ++elIter) {
                ASSERT_EQ(currGoldBar[elIter], outData[elIter]) << "at: " << elIter
                                                                << " for dim " << d +off
                                                                << " for device " << dev
                                                                << std::endl;
            }

            // Delete
            delete[] outData;
        }

        if(inArray   != 0) af_destroy_array(inArray);
        if(outArray  != 0) af_destroy_array(outArray);
        if(tempArray != 0) af_destroy_array(tempArray);
    }
}

vector<af_seq> init_subs()
{
    vector<af_seq> subs;
    subs.push_back({2, 6, 1});
    subs.push_back({1, 5, 1});
    subs.push_back({1, 3, 1});
    subs.push_back({1, 2, 1});
    return subs;
}

#define SCAN_TESTS(FN, TAG, Ti, To)             \
    TEST(Scan,Test_##FN##_##TAG)                \
    {                                           \
        scanTest<Ti, To, af_##FN>(              \
            string(TEST_DIR"/scan/"#FN".test")  \
            );                                  \
    }                                           \

SCAN_TESTS(accum, float   , float     , float     );
SCAN_TESTS(accum, double  , double    , double    );
SCAN_TESTS(accum, int     , int       , int       );
SCAN_TESTS(accum, cfloat  , af_cfloat , af_cfloat );
SCAN_TESTS(accum, cdouble , af_cdouble, af_cdouble);
SCAN_TESTS(accum, unsigned, unsigned  , unsigned  );
SCAN_TESTS(accum, uchar   , unsigned char, unsigned);

TEST(Scan,Test_Scan_Big0)
{
    scanTest<int, int, af_accum>(
        string(TEST_DIR"/scan/big0.test"),
        0
        );
}

TEST(Scan,Test_Scan_Big1)
{
    scanTest<int, int, af_accum>(
        string(TEST_DIR"/scan/big1.test"),
        1
        );
}
