#include "constants.h"
#include <string>
using std::string;

// PolicyEngine class definition
class PolicyEngine
{
private:
    string mode;
    int threshold;

public:
    PolicyEngine(string input_mode = "dynamic", int input_threshold = 1 * 1024 * 1024)
    {
        mode = input_mode;
        threshold = input_threshold;
    }

    int decide_page_size(int request_size)
    {
        if (mode == "small")
        {
            return SMALL_PAGE_SIZE;
        }
        else if (mode == "large")
        {
            return LARGE_PAGE_SIZE;
        }
        else if (mode == "dynamic")
        {
            return request_size > threshold ? LARGE_PAGE_SIZE : SMALL_PAGE_SIZE;
        }
        else
        {
            // Default to small page size if mode is unrecognized
            return SMALL_PAGE_SIZE;
        }
    }
};