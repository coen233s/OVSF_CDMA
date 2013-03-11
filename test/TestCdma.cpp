#include <phy/SimplePhyChannel.h>
#include <dev/BaseStation.h>
#include <dev/MobileStation.h>
#include <sim/Simulator.h>
#include <stdio.h>
#include <string.h>

using namespace std;

#define UID_1	0x02
#define UID_2	0x03

/*-----------------------------------------------------------------------------
Read a line from stdin
-----------------------------------------------------------------------------*/
char getLine(char* line, int size)
{
    char* p = line;
    
    while (((line - p) < size - 1) &&('\n' != (*line = getchar())))
    {
        if (EOF == *line)
        {
            *line = '\0';
            return EOF;
        }
        
        line++;
    }
    
    *(++line) = '\0';

    return 1;
}

/*-----------------------------------------------------------------------------
Skip white spaces in a line
-----------------------------------------------------------------------------*/
char* trimSpace(char* line)
{
    // Skip leading space
    while (*line != '\0' && isspace(*line))
    {   
        line++;
    }

    // Skip comment
    if ('#' == *line)
    {
        *line = '\0';
        return line;
    }

    int len = (int)strlen(line) - 1;

    while (len > 0)
    {

        if (isspace(line[len]))
        {
            line[len] = '\0';
        }
        else
        {
            break;
        }
    }
    
    return line;
}

/*-----------------------------------------------------------------------------
Read a number from a line
Return: pointer to the same line at the next non-white space
-----------------------------------------------------------------------------*/
char* getNumber(char* line, int* number)
{
    if (NULL == line)
    {
        return NULL;
    }

    line = trimSpace(line);
    char* data = line;

    while (NULL != data)
    {
        if (('#' == *data) || ('\0' == *data)) // Skip comments
        {
            return NULL;
        }

        if (sscanf(data, "%d", number) == 1)
        {
            while (('\0' != *data) && !isspace(*data))
            {
                ++data;
            }
            break;
        }
        else 
        {
            return NULL;
        }
    }

    return data;
}

void addUser(Simulator& sim, SimplePhyChannel& pch)
{
    char status;
    char line[1024];
    // Read until EOF
    // States of input
    enum { PARSE_TICK_DELAY, PARSE_UID, PARSE_TR, PARSER_DATA_RATE };
    uint8_t state = PARSE_TICK_DELAY;
    int tickDelay;
    int uid;
    int tr;
    int dataRate;

    while ((status = getLine(line, sizeof(line))) && (EOF != status))
    {
        char* data = line;

        if (PARSE_TICK_DELAY == state)
        {
            data = getNumber(data, &tickDelay);
            if (NULL != data)
            {
                state = PARSE_UID;
            }
            else
            {
                continue;
            }
        }
        if (PARSE_UID == state)
        {
            data = getNumber(data, &uid);
            if (NULL != data)
            {
                state = PARSE_TR;
            }
            else
            {
                continue;
            }
        }
        if (PARSE_TR == state)
        {
            data = getNumber(data, &tr);
            if (NULL != data)
            {
                state = PARSER_DATA_RATE;
            }
            else
            {
                continue;
            }
        }
        if (PARSER_DATA_RATE == state)
        {
            data = getNumber(data, &dataRate);
            if (NULL != data)
            {
                state = PARSE_TICK_DELAY;
                string name = "Mobile Station #";
                
                MobileStation ms(string("Mobile Station #1"), pch, uid);
                ms.setRateRange(dataRate, dataRate);
                sim.addObject(&ms);
            }
            else
            {
                continue;
            }
        }
    }
}
int main(int argc, char* argv[])
{
    Simulator sim;
    SimplePhyChannel pch;
    BaseStation bs(string("BaseStation"), pch);
    sim.addObject(&bs);

#if 1
    MobileStation ms(string("Mobile Station #2"), pch, UID_1);
    sim.addObject(&ms);
#if 1
    MobileStation ms2(string("Mobile Station #3"), pch, UID_2, false, 40000);
    sim.addObject(&ms2);
#endif
#else // Add user from stdin by test file
    addUser(sim, pch);
#endif
    sim.addObject(&pch);

    const int timeSteps = 100000;
    sim.run(timeSteps);

    return 0;
}
