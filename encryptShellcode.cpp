#include "cryptopp/aes.h"
#include "cryptopp/filters.h"
#include "cryptopp/hex.h"
#include "cryptopp/modes.h"
#include <iostream>

// 16 bytes --> AES-128
#define K_LEN 16

using namespace std;
using namespace CryptoPP;

int main()
{
    // contains placeholder for the shellcode, see crypter.py for more info
    const char shellcode[] = "PUTYOURSHELLCODEHERE!!1!";

    byte key[] = "0000000000513370";
    string cipher;

    try {
        // define the encryption-object and set the key
        ECB_Mode<AES>::Encryption e;
        e.SetKey(key, K_LEN);

        // We use the crypto++ pipelining paradigm:
        // Processing takes place from the source, over a filter into a sink.

        // So we first define the source using the shellcode-source,
        // defining that all data should be pumped to the TransformationFilter
        // at once (the boolean is true) and we create a
        // StreamTransformationFilter,
        // which will handle the data of the StringSource
        StringSource src(shellcode, true,
            // This filter will Transform the data into the desired
            // representation using the previously defined AES-encryption and
            // the HexEncoder.
            new StreamTransformationFilter(e,
                // Before the data will reach the sink, it will be processed
                // byte the HexEncoder which we use for data-portability
                new HexEncoder(new StringSink(cipher))));

    } catch (Exception& e) {
        cerr << e.what() << endl;
        exit(1);
    }

    cout << "ciphertext: " << cipher << endl;
    return 0;
}
