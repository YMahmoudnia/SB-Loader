# SB-Loader
SBLoader is a memory patcher to patch and execute the child process under the desired parent process.

# Description
One of the methods of debugger detection used in malware or commercial software is parent process checking. In debugger we can use Anti-Debug plugins, but what about out of debugger ?

With **SB-Loader** you can patch process memory or run any process under explorer.exe or any desired process.

# Build & Installation
This project written in C++ with microsoft foundation class in visual studio 2019. 
You don't need any extra libraries in compilation process.

# Features
**First Release (v1.0.0)**
- Select the parent process with process name
- Select the parent process with process id
- Run the child process with process argument
- Memory patcher

# Credits
- Yashar Mahmoudnia - https://t.me/securebyte
- Anasazi - Rohitab
