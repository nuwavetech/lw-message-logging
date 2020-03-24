# LightWave Message Logging Sample
This LightWave sample illustrates how to create a Message Logging collector for use with the LightWave Client or LightWave Server Message Logging feature. The sample servers decompose the Message Logging request and write it to the console. The samples may be used as a basis for developing custom Message Logging collectors.

*Note: The Message Logging feature is currently in the alpha release stage and is not supported for production use.*
 
## Prerequisites

+ NonStop C Compiler, if building the C sample.
+ NonStop COBOL Compiler, if building the COBOL sample.
+ An installed instance of [LightWave Client](https://docs.nuwavetech.com/display/LWCLIENT120) version 1.2.0-alpha.2 or greater, or
+ An installed instance of [LightWave Server](https://docs.nuwavetech.com/display/LWCLIENT110) version 1.1.0-alpha.2 or greater.
+ The Message Logging DDL file LWMLDDL. This file is included in the LightWave Client and LightWave Server installation PAK file. 
 

The following values must be configured in the SETENV TACL macro after UNPAKing the sample files:

+ lwmlddl-isv - The subvolume containing the Message Logging DDL file LWMLDDL. This is typically the installation subvolume of your LightWave Client or LightWave Server software. 
+ pathmon-name - The Message Logging collector Pathmon name.

The value specified for the pathmon-name must also be configured in the MLCONF file, if the default is changed. In addition, the MLCONF file is configured with the C collector enabled and the COBOL collector disabled. Enable the COBOL collector if desired. Note that if both the C and COBOL collectors are enabled, the output from both collectors will likely be interleaved. It's best to only enable one collector at a time.

## Install & Build

The sample servers must be built from source. The source files are present in the repository for convenient viewing. 
In addition, a PAK archive containing all of the source files is available for transfer to your NonStop system.
 
| Repository File | NonStop File | 
| -- | -- |
| macros/build.txt | build |
| macros/setenv.txt | setenv |
| macros/startpw.txt | startpw |
| macros/stoppw.txt | stoppw |
| macros/unsetenv.txt | unsetenv |
| src/mlconf.txt | mlconf |
| src/servc.c | servcc |
| src/servcobs.txt | servcobs |
| mlpak.bin | mlpak |
  
#### Transfer the PAK archive to your NonStop System

Download `mlpak.bin` from this repository to your local system, then upload to your NonStop server using binary transfer mode.

Note: to download the PAK file, click `mlpak.bin` in the file list to display the file details, then click the *Download* button.

Logon to TACL on your NonStop system to peform the installation and build steps.

#### Unpak the PAK archive
```
TACL > UNPAK MLPAK ($*.*.*), VOL $vol.subvol, LISTALL, MYID
```
#### Customize and run the SETENV macro
````
TACL > T/EDIT SETENV
TACL > RUN SETENV
````
#### Build the sample servers
```
TACL > RUN BUILD
```
## Running the collector Pathmon
Customize the SETENV macro and optionally, the MLCONF file for your environment and run it to set the required PARAMs.
```
TACL> T/EDIT SETENV
TACL> RUN SETENV
```
Start the LightWave Message Logging Collector Pathway
```
TACL > RUN STARTPW
```

## Configuring the LightWave process
Configure the LightWave Client or LightWave Server process to enable the Message Logging feature by adding the *--msg-log* option to the SERVER process configuration or CLIENT process or serverclass configuration:

#### LightWave Server
````
TACL > RUN SERVER --msg-log +$vol.subvol.MLCONF ...
````
*In future releases of LightWave Server, Message Logging will be configured using the Console or LWSCOM utility. Message Logging configuration file monitoring is not currently supported in LightWave Server.*

#### LightWave Client
````
TACL RUN CLIENT --msg-log +$vol.subvol.MLCONF [ --monitor msg-log ] ...

or

SET SERVER PARAM MSG-LOG "+$vol.subvol.MLCONF"
SET SERVER PARAM MONITOR "msg-log"
````

#### Restart the CLIENT or SERVER process.

Note that the sample collectors write the request information to the serverclass OUT file. The STARTPW macro sets OUT to #MYTERM and pauses the terminal to allow the output to be written to the console.

Each time a request is processed by the CLIENT or SERVER/SWORKER process, the collector process will output the contents of the Message Logging request:

```
SERVC $RECEIVE message:   5248
  code                    1
  version                 1
  len                     5248
  id                      212445188021197443
                          1595503071002624
  rq-start-time           212445188021145983
  rq-end-time             212445188021197425
  rq-ipm-offset           1952
  rq-ipm-len              2048
  rq-request-line-offset  841
  rq-request-line-len     40
  rq-request-line         POST /test-endpoint HTTP/1.1
  rq-headers-offset       916
  rq-headers-len          184
  rq-headers-count        6
        user-agent : LightWave Client 1.2.0
        content-type : application/json
        accept : */*
        content-length : 496
        connection : keep-alive
        host : test.example.com
  rq-payload-offset       4000
  rq-payload-len          496
  rp-ipm-offset           4496
  rp-ipm-len              256
  rp-status               200
  rp-status-line-offset   1100
  rp-status-line-len      14
  rp-status-line          HTTP/1.1 200
  rp-headers-offset       1114
  rp-headers-len          824
  rp-headers-count        7
        date : Tue, 07 Jan 2020 20:13:41 GMT
        server : Apache/2.4.41 (Ubuntu)
        strict-transport-security : max-age=15768000
        content-type : application/json
        content-length : 496
        keep-alive : timeout=5, max=99
        connection : Keep-Alive
  rp-payload-offset       4752
  rp-payload-len          496
  user-data-offset        0
  user-data-len           0
  user-data-count         0
  metadata-offset         224
  metadata-len            617
  metadata-count          20
        config-name : COLLECTOR-C
        config-enabled : 1
        config-target : serverclass:$lwml
        config-content-filter : *
        api-base-url : https://test.example.com
        api-file-name : \NODE.$VOL.SUBVOL.API
        api-method : POST
        api-name : test-api
        api-path : /test-endpoint
        net-local-ip : 192.168.0.1
        net-local-port : 13297
        net-remote-ip : 192.168.0.2
        net-remote-port : 443
        net-process : \NODE.$ZTC0
        process-ancestor-name : \NODE.$X0TF
        process-ancestor-program : \NODE.$SYSTEM.SYSTEM.PATHMON
        process-name : \NODE.$X15F
        process-program : \NODE.$VOL.SUBVOL.CLIENT
        tls-cipher : TLS_AES_256_GCM_SHA384
        tls-version : TLSv1.3
end of request
```

Assistance is available through the [NuWave Technologies Support Center](http://support.nuwavetech.com).