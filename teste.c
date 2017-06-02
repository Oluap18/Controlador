node 1 window 2 avg 10
node 2 window 2 max 10
node 3 filter 2 <= 3
node 4 const cooling
node 5 filter 2 > 3
node 6 const warming
node 7 filter 2 > 4
node 8 tee warm-log.txt
node 9 const 100
node 10 filter 2 > 5
node 11 spawn shutdown -n
node 12 tee log.txt
connect 1 2
connect 2 3 5
connect 3 4
connect 4 12
connect 5 6
connect 6 7 12
connect 7 8 9
connect 9 10
connect 10 11
inject 1 1020:25
inject 2 1021:25
inject 3 1021:26
inject 4 1022:28
inject 5 1023:17
inject 6 1023:18
inject 7 1023:19
inject 8 1023:20
inject 9 1023:21
inject 10 1023:22
inject 11 1023:12
inject 12 1023:114