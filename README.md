# IpMsgTester
Just an assignment I have to do for my job application. The goal of the program is to create a certain socket, listen in on a certain IP address and port, receive byte arrays starting and ending with 0x7E, and log this into a text file.

The IP address, port, and address of the log file are intended to be given in command line parameters, doing 

```console
IpMsgTester -ip <IP address> -p <Port address> -j <Address of the log file>
```

On Unix-based systems, and 

```console
IpMsgTester.exe -ip <IP address> -p <Port address> -j <Address of the log file>
```

On Windows-based systems.

Very much a work in progress, still undocumented.
