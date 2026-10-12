#include <iostream>
#include <iomanip>
#include <cmath>
#include <hls_math.h>

#include "preprocessing_hls.hpp"

using namespace std;


//==============================================================
// Parameters
//==============================================================

const float EPSILON = 0.001f;
//==============================================================

bool test_window_function(
    WindowType type,
    int window_size,
    float expected[MAX_WINDOW_SIZE])
{
    data_t test_windows[MAX_WINDOWS][MAX_WINDOW_SIZE] = {0};

    // Put known value into first element of each position.
    // Using 1.0 makes the output equal to the window coefficient.
    for (int i = 0; i < window_size; i++)
    {
        test_windows[0][i] = 1.0f;
    }

    apply_window_hls(
        test_windows,
        1,
        window_size,
        type);

    for (int i = 0; i < window_size; i++)
    {
        float actual = float(test_windows[0][i]);
        float error = fabs(actual - expected[i]);

        if (error > EPSILON)
        {
            cout << "Mismatch at index "
                 << i
                 << " Actual=" << actual
                 << " Expected=" << expected[i]
                 << " Error=" << error
                 << endl;

            return false;
        }
    }

    return true;
}

//==============================================================

void generate_expected_window(
    float expected[MAX_WINDOW_SIZE],
    int window_size,
    WindowType type)
{
    const float PI_REF = 3.14159265358979323846f;

    for (int i = 0; i < window_size; i++)
    {
        switch (type)
        {
        case WindowType::RECTANGULAR:
            expected[i] = 1.0f;
            break;

        case WindowType::HAMMING:
            expected[i] =
                0.54f -
                0.46f *
                cos(
                    (2.0f * PI_REF * i) /
                    (window_size - 1));
            break;

        case WindowType::HANN:
            expected[i] =
                0.5f *
                (1.0f -
                 cos(
                    (2.0f * PI_REF * i) /
                    (window_size - 1)));
            break;
        }
    }
}
//==============================================================
// Print Window Matrix
//==============================================================

void print_windows(
    const string& name,
    data_t windows[MAX_WINDOWS][MAX_WINDOW_SIZE],
    int num_windows,
    int window_size)
{
    cout << "\n=========================================\n";
    cout << name << endl;
    cout << "=========================================\n";

    for(int i = 0; i < num_windows; i++)
    {
        cout << "Window " << i << ": ";

        for(int j = 0; j < window_size; j++)
        {
            cout << fixed
                 << setprecision(6)
                 << float(windows[i][j])
                 << " ";
        }

        cout << endl;
    }
}


//==============================================================
// Compare Windows
//==============================================================

bool compare_windows(
    data_t actual[MAX_WINDOWS][MAX_WINDOW_SIZE],
    data_t expected[MAX_WINDOWS][MAX_WINDOW_SIZE],
    int num_windows,
    int window_size)
{
    bool pass = true;

    for(int i = 0; i < num_windows; i++)
    {
        for(int j = 0; j < window_size; j++)
        {
            float error =
                fabs(float(actual[i][j]) -
                     float(expected[i][j]));

            if(error > EPSILON)
            {
                cout << "Mismatch at ["
                     << i << "][" << j << "] "
                     << "Actual="
                     << float(actual[i][j])
                     << " Expected="
                     << float(expected[i][j])
                     << " Error="
                     << error
                     << endl;

                pass = false;
            }
        }
    }

    return pass;
}


//==============================================================
// Print Result
//==============================================================

void print_result(
    const string& test,
    bool pass)
{
    cout << left
         << setw(40)
         << test
         << (pass ? "[PASS]" : "[FAIL]")
         << endl;
}


//==============================================================
// Main
//==============================================================

int main()
{
    int failures = 0;

    //----------------------------------------------------------
    // Input
    //----------------------------------------------------------

    data_t input[MAX_INPUT_SIZE] =
    {
        1,2,3,4,5,6,7,8
    };

    data_t windows[MAX_WINDOWS][MAX_WINDOW_SIZE] = {0};

    int num_windows = 0;


    //----------------------------------------------------------
    // Sliding Window
    //----------------------------------------------------------

    sliding_window_hls(
        input,
        windows,
        8,
        4,
        2,
        num_windows);

    bool pass = (num_windows == 3);

    print_result(
        "Sliding Window Count",
        pass);

    if (!pass)
        failures++;


    data_t expected_windows[MAX_WINDOWS][MAX_WINDOW_SIZE] =
    {
        {1,2,3,4},
        {3,4,5,6},
        {5,6,7,8}
    };

    pass = compare_windows(
        windows,
        expected_windows,
        num_windows,
        4);

    print_result(
        "Sliding Window Values",
        pass);

    if (!pass)
        failures++;


    //----------------------------------------------------------
    // Window Function Tests
    //----------------------------------------------------------

    float expected_coeff[MAX_WINDOW_SIZE];


    //----------------------------------------------------------
    // Hamming - Window Size 4
    //----------------------------------------------------------

    generate_expected_window(
        expected_coeff,
        4,
        WindowType::HAMMING);

    pass = test_window_function(
        WindowType::HAMMING,
        4,
        expected_coeff);

    print_result(
        "Hamming Window Size 4",
        pass);

    if (!pass)
        failures++;


    //----------------------------------------------------------
    // Hamming - Window Size 8
    //----------------------------------------------------------

    generate_expected_window(
        expected_coeff,
        8,
        WindowType::HAMMING);

    pass = test_window_function(
        WindowType::HAMMING,
        8,
        expected_coeff);

    print_result(
        "Hamming Window Size 8",
        pass);

    if (!pass)
        failures++;


    //----------------------------------------------------------
    // Hamming - Window Size 16
    //----------------------------------------------------------

    generate_expected_window(
        expected_coeff,
        16,
        WindowType::HAMMING);

    pass = test_window_function(
        WindowType::HAMMING,
        16,
        expected_coeff);

    print_result(
        "Hamming Window Size 16",
        pass);

    if (!pass)
        failures++;


    //----------------------------------------------------------
    // Hann - Window Size 4
    //----------------------------------------------------------

    generate_expected_window(
        expected_coeff,
        4,
        WindowType::HANN);

    pass = test_window_function(
        WindowType::HANN,
        4,
        expected_coeff);

    print_result(
        "Hann Window Size 4",
        pass);

    if (!pass)
        failures++;


    //----------------------------------------------------------
    // Hann - Window Size 8
    //----------------------------------------------------------

    generate_expected_window(
        expected_coeff,
        8,
        WindowType::HANN);

    pass = test_window_function(
        WindowType::HANN,
        8,
        expected_coeff);

    print_result(
        "Hann Window Size 8",
        pass);

    if (!pass)
        failures++;


    //----------------------------------------------------------
    // Hann - Window Size 16
    //----------------------------------------------------------

    generate_expected_window(
        expected_coeff,
        16,
        WindowType::HANN);

    pass = test_window_function(
        WindowType::HANN,
        16,
        expected_coeff);

    print_result(
        "Hann Window Size 16",
        pass);

    if (!pass)
        failures++;


    //----------------------------------------------------------
    // Rectangular - Window Size 4
    //----------------------------------------------------------

    generate_expected_window(
        expected_coeff,
        4,
        WindowType::RECTANGULAR);

    pass = test_window_function(
        WindowType::RECTANGULAR,
        4,
        expected_coeff);

    print_result(
        "Rectangular Window Size 4",
        pass);

    if (!pass)
        failures++;


    //----------------------------------------------------------
    // Rectangular - Window Size 8
    //----------------------------------------------------------

    generate_expected_window(
        expected_coeff,
        8,
        WindowType::RECTANGULAR);

    pass = test_window_function(
        WindowType::RECTANGULAR,
        8,
        expected_coeff);

    print_result(
        "Rectangular Window Size 8",
        pass);

    if (!pass)
        failures++;


    //----------------------------------------------------------
    // Rectangular - Window Size 16
    //----------------------------------------------------------

    generate_expected_window(
        expected_coeff,
        16,
        WindowType::RECTANGULAR);

    pass = test_window_function(
        WindowType::RECTANGULAR,
        16,
        expected_coeff);

    print_result(
        "Rectangular Window Size 16",
        pass);

    if (!pass)
        failures++;


    //----------------------------------------------------------
    // Hamming Window - Original Functional Test
    //----------------------------------------------------------

    sliding_window_hls(
        input,
        windows,
        8,
        4,
        2,
        num_windows);

    apply_window_hls(
        windows,
        num_windows,
        4,
        WindowType::HAMMING);

    print_windows(
        "Hamming Output",
        windows,
        num_windows,
        4);

    data_t expected_hamming[MAX_WINDOWS][MAX_WINDOW_SIZE] =
    {
        {0.08f, 1.54f, 2.31f, 0.32f},
        {0.24f, 3.08f, 3.85f, 0.48f},
        {0.40f, 4.62f, 5.39f, 0.64f}
    };

    pass = compare_windows(
        windows,
        expected_hamming,
        num_windows,
        4);

    print_result(
        "Hamming Window Functional Test",
        pass);

    if (!pass)
        failures++;


    //----------------------------------------------------------
    // Min-Max Normalization
    //----------------------------------------------------------

    normalize_windows_hls(
        windows,
        num_windows,
        4,
        NormalizationType::MIN_MAX);

    print_windows(
        "Min-Max Output",
        windows,
        num_windows,
        4);

    data_t expected_minmax[MAX_WINDOWS][MAX_WINDOW_SIZE] =
    {
        {0.000f,0.655f,1.000f,0.108f},
        {0.000f,0.787f,1.000f,0.066f},
        {0.000f,0.846f,1.000f,0.048f}
    };

    pass = compare_windows(
        windows,
        expected_minmax,
        num_windows,
        4);

    print_result(
        "Min-Max Normalization",
        pass);

    if (!pass)
        failures++;


    //----------------------------------------------------------
    // Z-Score
    //----------------------------------------------------------

    sliding_window_hls(
        input,
        windows,
        8,
        4,
        2,
        num_windows);

    apply_window_hls(
        windows,
        num_windows,
        4,
        WindowType::HAMMING);

    normalize_windows_hls(
        windows,
        num_windows,
        4,
        NormalizationType::Z_SCORE);

    print_windows(
        "Z-Score Output",
        windows,
        num_windows,
        4);

    data_t expected_zscore[MAX_WINDOWS][MAX_WINDOW_SIZE] =
    {
        {-1.082f,0.526f,1.373f,-0.817f},
        {-1.060f,0.740f,1.227f,-0.908f},
        {-1.045f,0.822f,1.162f,-0.939f}
    };

    pass = compare_windows(
        windows,
        expected_zscore,
        num_windows,
        4);

    print_result(
        "Z-Score Normalization",
        pass);

    if (!pass)
        failures++;


    //----------------------------------------------------------
    // Complete Pipeline
    //----------------------------------------------------------

    data_t output[MAX_WINDOWS][MAX_WINDOW_SIZE] = {0};

    ml_preprocessing_hls(
        input,
        output,
        8,
        4,
        2,
        WindowType::HAMMING,
        NormalizationType::MIN_MAX,
        num_windows);

    pass = compare_windows(
        output,
        expected_minmax,
        num_windows,
        4);

    print_result(
        "Complete Pipeline",
        pass);

    if (!pass)
        failures++;


    //----------------------------------------------------------
    // Summary
    //----------------------------------------------------------

    cout << "\n=========================================\n";

    if (failures == 0)
        cout << "ALL TESTS PASSED\n";
    else
        cout << failures << " TEST(S) FAILED\n";

    cout << "=========================================\n";

    return failures;
}
