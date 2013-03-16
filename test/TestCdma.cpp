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
                string name = "MobileStation";

                // TODO: where do we clean this memory
                MobileStation* ms = new MobileStation(name, pch, uid);
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

class AutoMobileStation : public MobileStation {
protected:
    typedef MobileStation Base;

    enum STATE {
        STATE_NONE = 0,
        STATE_SEND,
        STATE_RECV,
        STATE_DONE,
    } m_state;

    int m_rxIdle;

public:
    AutoMobileStation(const string& name, AbsPhyChannel &pch, int uid, bool tr=true,
            int tickDelay = 0)
    : MobileStation(name, pch, uid, tr, tickDelay)
    , m_state(STATE_NONE)
    , m_rxIdle(0)
    { }

    virtual void onTick(int time) {
        MobileStation::onTick(time);

        switch (m_state) {
        case STATE_NONE:
            if (m_pDataChannel && m_pDataChannel->m_tx.hasPendingData()) {
                cout << getDeviceId() << ": STATE_NONE --> STATE_SEND" << endl;
                m_state = STATE_SEND;
            }
            if (m_pDataChannel && m_pDataChannel->m_rx.hasData()) {
                cout << getDeviceId() << ": STATE_NONE --> STATE_RECV" << endl;
                m_state = STATE_RECV;
            }
            break;
        case STATE_SEND:
            if (!m_pDataChannel->m_tx.hasPendingData()) {
                cout << getDeviceId() << ": STATE_SEND --> STATE_DONE" << endl;
                m_state = STATE_DONE;
                terminate();
            }
            break;
        case STATE_RECV:
            if (!m_pDataChannel->m_rx.hasData()) {
                if (++m_rxIdle > 10) {
                    cout << getDeviceId() << ": STATE_RECV --> STATE_DONE" << endl;
                    m_state = STATE_DONE;
                    terminate();
                }
            } else {
                m_rxIdle = 0;
            }
            break;
        default:
            break;
        }
    }
};

#define MIN_TIME 5000   // >= setup time

// Set to 0 if all MS can terminate, to N > 0, if N MS cannot
// terminate
#define TERMINATE_CONN_NUM 0

bool shouldStop(int time, void *arg) {
    BaseStation *bs = reinterpret_cast<BaseStation *>(arg);
    return time > MIN_TIME && bs->getDataConnections() == TERMINATE_CONN_NUM;
}

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
    BaseStation bs(string("BaseStation"), pch);
    sim.addObject(&bs);

    const int testRate = pch.getChipRate() / 8;

#if 1
    AutoMobileStation ms(string("MobileStation"), pch, UID_1);
    sim.addObject(&ms);
    AutoMobileStation ms2(string("MobileStation"), pch, UID_2, false, 40000);
    sim.addObject(&ms2);

#if !TEST_CODE_RANGE
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
