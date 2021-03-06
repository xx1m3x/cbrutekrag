[![license](https://img.shields.io/github/license/jorge-matricali/cbrutekrag.svg)](https://jorge-matricali.mit-license.org/2014) [![GitHub contributors](https://img.shields.io/github/contributors/jorge-matricali/cbrutekrag.svg)](https://github.com/jorge-matricali/cbrutekrag/graphs/contributors) [![Build Status](https://travis-ci.org/jorge-matricali/cbrutekrag.svg?branch=master)](https://travis-ci.org/jorge-matricali/cbrutekrag)

# cbrutekrag
Penetration tests on SSH servers using dictionary attacks. Written in _C_.

> _brute krag_ means "brute force" in afrikáans

## Disclaimer
>This tool is for ethical testing purpose only.   
>cbrutekrag and its owners can't be held responsible for misuse by users.   
>Users have to act as permitted by local law rules.

## Requeriments
* `gcc` compiler
* `libssh`

## Build
```bash
git clone --depth=1 https://github.com/jorge-matricali/cbrutekrag.git
cd cbrutekrag
make
make install
```
Then you can do
```bash
$ cbrutekrag -h
       _                _       _
      | |              | |     | |
  ___ | |__  _ __ _   _| |_ ___| | ___ __ __ _  __ _
 / __|| '_ \| '__| | | | __/ _ \ |/ / '__/ _` |/ _` |
| (__ | |_) | |  | |_| | ||  __/   <| | | (_| | (_| |
 \___||_.__/|_|   \__,_|\__\___|_|\_\_|  \__,_|\__, |
          OpenSSH Brute force tool 0.3.0        __/ |
      (c) Copyright 2014-2018 Jorge Matricali  |___/


usage: ./cbrutekrag [-h] [-v] [-T TARGETS.lst] [-C combinations.lst]
                [-t THREADS] [-o OUTPUT.txt]

  -h                This help
  -v                Verbose mode
  -T <targets>      Targets file
  -C <combinations> Username and password file
  -t <threads>      Max threads
  -o <output>       Output log file
```

## Example usages
```bash

# Many targets, many pre-made combinations of user and password separated by space.
cbrutekrag -T targets.txt -C combinations.txt -o result.log
```
