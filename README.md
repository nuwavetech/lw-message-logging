# LightWave Message Logging Sample
This LightWave sample illustrates how to create a Message Logging collector for use with the LightWave Client or LightWave Server Message Logging feature. The sample servers decompose the Message Logging request and write it to the console. The samples may be used as a basis for developing custom Message Logging collectors.

*Note: The Message Logging feature is currently in the pre-release stage and is not supported for production use.*

## Feature Documentation
+ [LightWave Client - Message Logging](https://docs.nuwavetech.com/display/LWCLIENT120/Message+Logging)
+ [LightWave Server - Message Logging](https://docs.nuwavetech.com/display/LWSERVER110/Message+Logging)

## Sample Prerequisites

+ NonStop C Compiler, if building the C sample.
+ NonStop COBOL Compiler, if building the COBOL sample.
+ An installed instance of [LightWave Client](https://docs.nuwavetech.com/display/LWCLIENT120) version 1.2.0-beta.2 or greater, or
+ An installed instance of [LightWave Server](https://docs.nuwavetech.com/display/LWCLIENT110) version 1.1.0-beta.2 or greater.
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
SERVC log message received:   2796
  code                    1
  version                 1
  len                     2796
  id                      212462209304290976
                          21670558706499584
  rq-start-time           212462209304171542
  rq-end-time             212462209304290973
  rq-total-time           119431
  rq-connect-time         0
  rq-connect-hs-time      107689
  rq-request-time         1860
  rq-response_time        22
  rq-serialize-time       22
  rq-deserialize-time     37
  rq-server-io            0
  rq-ipm-offset           2160
  rq-ipm-len              258
  rq-request-line-offset  981
  rq-request-line-len     41
  rq-request-line         POST /test/v1/simpleEcho HTTP/1.1
  rq-method-off           1022
  rq-method-len           5
  rq-method               POST
  rq-uri-off              1027
  rq-uri-len              27
  rq-uri                  /test/v1/simpleEcho
  rq-params-offset        0
  rq-params-len           0
  rq-params-count         0
  rq-request-http-ver-off 1054
  rq-request-http-ver-len 4
  rq-request-http-ver     1.1
  rq-headers-offset       1058
  rq-headers-len          789
  rq-headers-count        14
        host : nsx.nuwavetech.com:19099
        connection : keep-alive
        content-length : 44
        accept : application/json, text/plain, */*
        user-agent : Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/83.0.4103.116 Safari/53
7.36
        content-type : application/json
        origin : https://nsx.nuwavetech.com:19098
        sec-fetch-site : same-site
        sec-fetch-mode : cors
        sec-fetch-dest : empty
        referer : https://nsx.nuwavetech.com:19098/console/
        accept-encoding : gzip, deflate, br
        accept-language : en-US,en;q=0.9
        cookie : XSRF-TOKEN-da1d869d692dd256=da1d869d692dd256f30d04768c9e7f1de6e2457aa28c29cf0b76151a2633bfa9; XSRF-TOKEN-7c2fd7ff3d
185c02=7c2fd7ff3d185c022d50af3ae1b20d2d87244593c4a252fcb3876e19041fb8b3; XSRF-TOKEN-a061a6cbae41d860=a061a6cbae41d860b3819945283d529
097474de140e13e72012bc95bf0bfd6b9
  rq-body-offset          2432
  rq-body-len             44
  rp-ipm-offset           2480
  rp-ipm-len              258
  rp-status               200
  rp-status-line-offset   1847
  rp-status-line-len      16
  rp-status-line          HTTP/1.1 200 OK
  rp-headers-offset       1863
  rp-headers-len          287
  rp-headers-count        8
        date : Wed, 22 Jul 2020 20:21:44 GMT
        access-control-allow-origin : https://nsx.nuwavetech.com:19098
        access-control-allow-credentials : true
        content-length : 44
        cache-control : no-cache,no-store,must-revalidate
        content-type : application/json
        keep-alive : timeout=30, max=100
        connection : Keep-Alive
  rp-body-offset          2752
  rp-body-len             44
  user-data-offset        0
  user-data-len           0
  user-data-count         0
  metadata-offset         224
  metadata-len            757
  metadata-count          24
        config-collector-name : collector-c
        config-collector-enabled : true
        config-collector-type : serverclass
        config-collector-pathmon : \NODE.$LWML
        config-collector-serverclass : COLLECTOR-C
        config-collector-content : all
        config-filter-name : successfull-request
        api-method : POST
        api-name : com.nuwavetech.test.api
        api-path : /test/v1/simpleEcho
        api-process : \NODE.$TS00
        api-serverclass : TESTSVR
        api-alias : SimpleEchoRequest
        net-local-ip : 172.17.198.13
        net-local-port : 19099
        net-remote-ip : 172.17.201.242
        net-remote-port : 64218
        net-process : $ZTC0
        process-ancestor-name : \NODE.$Y9S0
        process-ancestor-program : \NODE.$SYSTEM.SYS01.TACL
        process-name : \NODE.$Y9X6
        process-program : \NODE.$VOL.SUBVOL.SERVER
        tls-cipher : TLS_AES_256_GCM_SHA384
        tls-version : TLSv1.3
end of request
```

Assistance is available through the [NuWave Technologies Support Center](http://support.nuwavetech.com).