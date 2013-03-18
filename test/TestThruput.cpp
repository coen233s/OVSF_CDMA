#include <phy/SimplePhyChannel.h>
#include <dev/BaseStation.h>
#include <dev/MobileStation.h>
#include <dev/protocol/ProtocolData.h>
#include <sim/Simulator.h>
#include <stdio.h>
#include <string.h>
#include <Configuration.h>

using namespace std;

#define UID_1	0x02
#define UID_2	0x03

#ifndef WIN32
#define stricmp strcasecmp
#endif

class AutoMobileStation : public MobileStation {
protected:
    typedef MobileStation Base;

    enum STATE {
        STATE_NONE = 0,
        STATE_SEND,
        STATE_RECV,
        STATE_DONE,
    } m_state;

    int m_coolingOff;

public:
    AutoMobileStation(const string& name, AbsPhyChannel &pch, int uid, bool tr=true,
            int tickDelay = 0)
    : MobileStation(name, pch, uid, tr, tickDelay)
    , m_state(STATE_NONE)
    , m_coolingOff(0)
    { }

    virtual void onTick(int time) {
        MobileStation::onTick(time);

        switch (m_state) {
        case STATE_NONE:
            if (m_pDataChannel && m_pDataChannel->m_tx.hasPendingData()) {
                cout << getDeviceId() << m_uid << ": STATE_NONE --> STATE_SEND" << endl;
                m_state = STATE_SEND;
                m_tickCount = time;
            }
            if (m_pDataChannel && m_pDataChannel->m_rx.hasData()) {
                cout << getDeviceId() << m_uid << ": STATE_NONE --> STATE_RECV" << endl;
                m_state = STATE_RECV;
                m_tickCount = time;
            }
            break;
        case STATE_SEND:
            if (!m_pDataChannel->m_tx.hasPendingData()) {
                if (m_coolingOff++ > m_pDataChannel->m_tx.getWalshLength()) {
                    cout << getDeviceId() << m_uid << ": STATE_SEND --> STATE_DONE" << endl;
                    m_state = STATE_DONE;
                    terminate();
                    m_tickCount = time -m_tickCount;
                }
            } else {
                m_coolingOff = 0;
            }
            break;
        case STATE_RECV:
            if (!m_pDataChannel->m_rx.hasData()) {
				if (++m_coolingOff > m_pDataChannel->m_rx.getWalshLength()) {
                    cout << getDeviceId() << m_uid << ": STATE_RECV --> STATE_DONE" << endl;
                    m_state = STATE_DONE;
                    terminate();
                    m_tickCount = time -m_tickCount;
                }
            } else {
                m_coolingOff = 0;
            }
            break;
        default:
            break;
        }
    }
};

static int s_totalConnections = 0;

bool shouldStop(int time, void *arg) {
    BaseStation *bs = reinterpret_cast<BaseStation *>(arg);
    return bs->getTotalConnections() == s_totalConnections &&
           bs->getTotalDisconnections() == s_totalConnections;
}

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
            if (line > p)
            {
                return 1;
            }
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
                string name = "MobileStation";
                cout << "Adding " << name << uid << endl;
                // TODO: where do we clean this memory
                AutoMobileStation* ms = new AutoMobileStation(name, pch, uid, tr ? true: false, tickDelay);
                ++s_totalConnections;
                cout << "s_totalConnections: " << s_totalConnections << endl;
                ms->setRateRange(dataRate, dataRate);
                sim.addObject(ms);
            }
            else
            {
                continue;
            }
        }
    }
}


#define MOBILE2_JOIN_TIME   40000

class Timer : public SimObject {
private:
    int m_lastTime;
    virtual void onTick(int time) {
        m_lastTime = time;
    }

public:
    int getTime() {
        return m_lastTime;
    }
};

int main(int argc, char* argv[])
{
    Simulator sim;
    SimplePhyChannel pch;
    BaseStation::MODE mode = BaseStation::VAR_DYNAMIC;
    bool variableRate = true;
    bool dynamicCode = true;
    for (int i = 1; i < argc; ++ i)
    {
        if ((0 == stricmp("-f", argv[i])) || (0 == stricmp("--fixed-rate", argv[i])))
        {
            variableRate = false;
            if (dynamicCode)
            {
                mode = BaseStation::FIXED_DYNAMIC;
            }
            else
            {
                mode = BaseStation::FIXED_ONCE;
            }
        }
        else if ((0 == stricmp("-o", argv[i])) || (0 == stricmp("--once-code", argv[i])))
        {
            dynamicCode = false;
            if (variableRate)
            {
                mode = BaseStation::VAR_ONCE;
            }
            else
            {
                mode = BaseStation::FIXED_ONCE;
            }
        }
    }

    BaseStation bs(string("BaseStation"), pch, mode);
    sim.addObject(&bs);

    const int testRate = pch.getChipRate() / 8;

#if TEST_DEFAULT
    AutoMobileStation ms(string("MobileStation"), pch, UID_1);
    ms.setRateRange(testRate, testRate);
    sim.addObject(&ms);
    AutoMobileStation ms2(string("MobileStation"), pch, UID_2, false, MOBILE2_JOIN_TIME);
    ms2.setRateRange(testRate, testRate);
    sim.addObject(&ms2);

#if TEST_CODE_RANGE
    // Fix MS data rate if TEST_CODE_RANGE is not set
    ms.setRateRange(testRate, testRate);
    ms2.setRateRange(testRate, testRate);
#endif

#else // Add user from stdin by test file
    addUser(sim, pch);
#endif
    sim.addObject(&pch);

    Timer timer;
    sim.addObject(&timer);

    sim.run(shouldStop, &bs);

    cout << "Time: " << timer.getTime() << endl;

    return 0;
}
