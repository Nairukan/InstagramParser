#ifndef PARSINGPOOL_H
#define PARSINGPOOL_H

#include <vector>
#include <string>

using std::vector;
using std::string;

class ParsingPool
{
public:
    ParsingPool(time_t Start, time_t End);



    vector<string> usernames;
    vector<int> id_s;
    const time_t Start, End;
};

#endif // PARSINGPOOL_H
