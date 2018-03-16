# Crypt0r

This was a small C++/Python project to learn about crypting malware in order to make it FUD (fully undetectable):

![Scan Results](https://NoDistribute.com/result/image/MWsaYO1lknjRybi5.png)

You still have to set up the build process yourself if you want to use this, take this as a challenge <:

## Features

1. FUD at time of release - scantime and runtime using encryption and delayed decryption.
2. No dependencies, statically linked binary.
3. Self bruteforcing - no included AES key.
4. Hidden startup.
5. Should work with every supplied shellcode.

## Compiling Notes (Windows)

1. Make sure Visual Studio, MSBuild and [cryptopp](https://www.cryptopp.com/) are available. Also, cryptopp has to be recognzed by Visual Studio.
2. Setup Visual Studio projects for `encryptShellcode.cpp` and `stub.cpp`.
3. Generate your shellcode in Python format using `msfvenom` and save it as `shellcode.py`.
4. Execute `builder.py shellcode.py`.
5. The path of the crypted malware should be printed on the screen.
