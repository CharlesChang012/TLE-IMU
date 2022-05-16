# TLE-IMU
This is a project I cowork with my classmates, which grabs TLE and IMU data. Then further adjust parameters of the IMU to get better reception of signals from satellites.

# How to use
Download the githubpage.
### Get TLE
To get the TLE data from website (here we get access to [celestrak.com](https://celestrak.com) for data), run the code. Then should output TLE file.
```
$ gcc -c gettle.c -o gettle
$ gcc ./gettle
```
# reference
- [HTTP Client with C](https://notfalse.net/47/c-socket-http-client)
- [SGP4](https://github.com/aholinch/sgp4)
