#include <iostream>
#include <assert.h>
#include <ovsf.h>
#include <phy/Receiver.h>
#include <phy/Transmitter.h>


using namespace std;

///////////////////////////////////////////////////////////////////////////////

void test_assign();
void test_capacity();
void greedy_server_test();
void test_256bit_code();
void test_generate_CDMACode();
void test_hex();
void test_multiple_assignment();

int main()
{
    //test_capacity();
    //greedy_server_test();
    //test_256bit_code();
    //test_generate_CDMACode();
    //test_hex();
    test_multiple_assignment();
    return 0;
}

void test_multiple_assignment()
{
    Assigner asg;
    // user id, code length
    // 100, 8
    // 200, 4
    // 300, 4
    std::vector<int> userId;
    userId.push_back(100);
    userId.push_back(200);
    userId.push_back(300);

    std::vector<int> codeLens;
    codeLens.push_back(8);
    codeLens.push_back(4);
    codeLens.push_back(4);

    std::vector<std::pair<int,WHCode> > results = asg.assignUserIds(userId, codeLens);
    assert(!results.empty());
    // should have no problem giving out these codes
    for (size_t k=0; k<results.size(); k++) {
        cout << "user(" << results[k].first << ") has code of "
                << results[k].second.toHexString().c_str()
                << endl;
    }

    cout << "current capacity is " << asg.calcCurrentCapacity() << endl;
    int freeCodeLen = asg.calcShortestFreeCode(4);
    cout << "shortest code possible is " << freeCodeLen
            << " has a rate of " << 1.0/(double)freeCodeLen
            << endl;

    // the new user 400 also want a code len of 4. This is possible.
    std::pair<bool,WHCode> r = asg.assignUserId(400,4);
    assert(r.first);
    cout << "user(400) has code of " << r.second << endl;
    //cout << "user(400) has code of " << r.second.toHexString().c_str() << endl;

    // another new guys want a code of length 4. Is this possible?
    cout << "current capacity is " << asg.calcCurrentCapacity() << endl;
    freeCodeLen = asg.calcShortestFreeCode(4);
    cout << "shortest code possible is " << freeCodeLen
            << " has a rate of " << 1.0/(double)freeCodeLen
            << endl;

    // This is not possible because the current capacity is 0.125 but you
    // are asking for 0.25.
    // But we are going to try to request it anyway...
    r = asg.assignUserId(500,4);
    if (r.first) {
        cout << "Request for a rate of (1/4) is possible." << endl;
    }
    else {
        cout << "There is not enough capacity for a rate of (1/4)" << endl;
    }

    // We cannot find the rate of 4 for the new guys.
    // What we can do is to reduce some users rate so that this man will be happy.

    // Get a snapshot of current code usage
    cout << endl;
    cout << "Current code usage:" << endl;
    std::vector<std::pair<int,WHCode> > snapshot = asg.listUsedCode();
    for (size_t k=0; k<snapshot.size(); k++) {
        cout << "User(" << snapshot[k].first << ") has code of "
                << snapshot[k].second.toHexString().c_str()
                << endl;
    }
    // Now clear everything
    asg.releaseAll();
    cout << "Release all code. Now the current capacity is " << asg.calcCurrentCapacity() << endl;

    // Well.. since 3 ppl has code of len 4. So we have 1/4 of capacity left. But we want to be fair.
    // So everybody should be penalized equally except the user 500.

    // But let try to cheat the Assigner. We will try to ask a code len of 4 for 4 users and a code len of 16 for one users.
    // Re-setup userIds
    userId.clear();
    userId.push_back(100);
    userId.push_back(200);
    userId.push_back(300);
    userId.push_back(400);
    userId.push_back(500);

    codeLens.clear();
    codeLens.push_back(4);
    codeLens.push_back(4);
    codeLens.push_back(4);
    codeLens.push_back(16);
    codeLens.push_back(4);

    // Can the Assigner know about this cheating?
    if (asg.validateRequestCodeLength(codeLens)) {
        cout << "it does not know! :)" << endl;
    }
    else {
        cout << "it knows that we are requesting too much capacity" << endl;
    }

    // Well.. we will request anyway
    results = asg.assignUserIds(userId, codeLens);
    if (results.empty()) {
        cout << "The assigner gives you nothing back because the request is not possible." << endl;
    }

    // Okay. Let reduce the user 100 rate by 50%
    codeLens[0] = codeLens[0] * 2;
    // Is it okay ?
    if (asg.validateRequestCodeLength(codeLens)) {
        cout << "This assignment is okay." << endl;
    }
    else {
        cout << "still requesting too much capacity" << endl;
    }

    cout << endl;

    results = asg.assignUserIds(userId, codeLens);
    assert(!results.empty());

    // should have no problem giving out these codes
    for (size_t k=0; k<results.size(); k++) {
        cout << "user(" << results[k].first << ") has code of "
                << results[k].second.toHexString().c_str()
                << endl;
    }

}

void test_hex()
{
    std::vector<WHCode> codes = CDMA_GenerateCode(8);
    for (size_t i=0; i<codes.size(); i++) {
        cout << i << " : ";
        codes[i].print();
        cout << "=" << codes[i].toByteArray().c_str();
        cout << endl;
    }
}

void test_generate_CDMACode()
{
    cout << "Generate CDMA chip sequence 8-bit" << endl;
    std::vector<WHCode> codes = CDMA_GenerateCode(8);
    for (size_t i=0; i<codes.size(); i++) {
        cout << i << " : ";
        codes[i].print();
        cout << "=" << codes[i].toHexString().c_str();
        cout << endl;
    }

    cout << endl << endl;
    cout << "Generate CDMA chip sequence 256-bit" << endl;
    codes = CDMA_GenerateCode(256);
    for (size_t i=0; i<codes.size(); i++) {
        cout << i << " : ";
        codes[i].print();
        cout << "=" << codes[i].toHexString().c_str();
        cout << endl;
    }
    cout << "successfully generate 256 256-bit chip sequence." << endl;
    cout << endl;
    cout << "Test Orthogonal..." << endl;
    for (size_t i=0; i<codes.size(); i++) {
        for (size_t j=0; j<codes.size(); j++) {
            if (i != j) {
                int v = codes[i].dot(codes[j]);
                if (v != 0) {
                    cout << "code:" << i << " is not orthogonal to code: " << j << endl;
                }
            }
        }
    }
    cout << "all codes are orthogonal." << endl;
}

void test_256bit_code()
{
    Assigner assigner;

    std::pair<bool,WHCode> result = assigner.assignUserId(100,256);
    if (result.first) {
        cout << "user 100's WHCode = ";
        result.second.print();
        cout << endl;
        cout << "code Len = " << result.second.length() << endl;
    }
    else {
        cout << "fail: user 100 got nothing back." << endl;
    }

}

void greedy_server_test()
{
    Assigner assigner;
    // we will assign the 3 users in the following requests:
    // user 1 wants 1/8 rate = code len = 8
    // user 2 wants 1/4 rate = code len = 4
    // user 3 wants 1/4 rate = code len = 4

    std::pair<bool,WHCode> result = assigner.assignUserId(100,8);
    if (result.first) {
        cout << "user 100's WHCode = ";
        result.second.print();
        cout << endl;
    }
    else {
        cout << "fail: user 100 got nothing back." << endl;
    }
    //assigner.print();

    std::pair<bool,WHCode> result2 = assigner.assignUserId(200,4);
    if (result2.first) {
        cout << "user 200's WHCode = ";
        result2.second.print();
        cout << endl;
    }
    else {
        cout << "fail: user 200 got nothing back." << endl;
    }
    //assigner.print();

    std::pair<bool,WHCode> result3 = assigner.assignUserId(300,4);
    if (result3.first) {
        cout << "user 300's WHCode = ";
        result3.second.print();
        cout << endl;
    }
    else {
        cout << "fail: user 300 got nothing back." << endl;
    }

    cout << "Check if the assignment are correct..." << endl;
    std::vector<std::pair<int,WHCode> > codes = assigner.listUsedCode();
    for (size_t k=0; k<codes.size(); k++) {
        cout << "User " << codes[k].first << " has WHCode of ";
        codes[k].second.print();
        cout << endl;
    }

    cout << "Current capacity = " << assigner.calcCurrentCapacity() << endl;
    cout << "shortest code you can get is " << assigner.calcShortestFreeCode(2) << endl;

    assigner.print();

}

void test_capacity()
{
    Assigner assigner;
    assigner.assignUserId(100,4);
    assigner.print();

    double capacity = assigner.calcCurrentCapacity();
    cout <<" capacity = " << capacity << endl;

    assigner.releaseUserId(100);
    cout <<" capacity = " << assigner.calcCurrentCapacity() << endl;

    int codeLen = assigner.calcShortestFreeCode(2);
    cout <<" codeLen = " << codeLen << endl;

    std::pair<bool,WHCode> result = assigner.assignUserId(200,4);
    if (result.first) {
        cout << "WHCode is ";
        result.second.print();
        cout << endl;
    }
    else {
        cout << "unsuccessful assign this user to WHCode of length 4." << endl;
    }

    result = assigner.assignUserId(300,4);
    if (result.first) {
        cout << "WHCode is ";
        result.second.print();
        cout << endl;
    }
    else {
        cout << "unsuccessful assign this user to WHCode of length 4." << endl;
    }
}

void test_assign()
{
    OVSFTree tree;
    tree.assign(3,1,100);
    tree.print();
    tree.assign(1,1,200);
    tree.print();

    WHCode c1;
    tree.peek(3,1,c1);
    WHCode c2;
    tree.peek(1,1,c2);

    cout << "c1 = ";
    c1.print();
    cout << endl;
    cout << "c2 = ";
    c2.print();
    cout << endl;

    tree.release(1,1);
    tree.print();

    tree.release(3,1);
    tree.print();
}
