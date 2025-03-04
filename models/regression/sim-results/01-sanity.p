# lqsim 5.16
# lqsim --confidence=1.0,1000 --seed=1049217653 --parseable --output=01-sanity.p 01-sanity.in
# $Id: 01-sanity.p 13764 2020-08-17 19:50:05Z greg $
V y
C 0.659747
I 4
PP 2
NP 1

#!Comment: Simplest model - rendezvous.
#!User:  0:14:49.000
#!Sys:   0:05:59.000
#!Real:  0:22:58.000

B 2
client         :client          0.5         
server         :server          1           
-1

W 1
client         :client          server          0           -1 
                                            %95 0           -1 
                                            %99 0           -1 
                -1 
-1

VARW 1
client         :client          server          0            -1 
                                            %95 0            -1 
                                            %99 0            -1 
                -1 
-1

X 2
client         :client          1.99453     -1 
                            %95 0.0115167   -1 
                            %99 0.0211404   -1 
                -1 
server         :server          0.999098    -1 
                            %95 0.00732229  -1 
                            %99 0.0134411   -1 
                -1 
-1

VAR 2
client         :client          5.98525     -1 
                            %95 0.0463522   -1 
                            %99 0.0850859   -1 
                -1 
server         :server          0.999326    -1 
                            %95 0.0154415   -1 
                            %99 0.0283449   -1 
                -1 
-1

FQ 2
client         :client          0.501371    1           -1 1
                            %95 0.0028918   0           -1 0
                            %99 0.00530831  0           -1 0
                -1 
server         :server          0.49993     0.499478    -1 0.499478
                            %95 0.00319051  0.00233262  -1 0.00233262
                            %99 0.00585663  0.00428184  -1 0.00428184
                -1 
-1

P client 1
client          1  0 1  client          0.500522    0           -1 
                                    %95 0.00233262  0           -1 
                                    %99 0.00428184  0           -1 
                        -1 
-1 

P server 1
server          1  0 1  server          0.499478    0           -1 
                                    %95 0.00233262  0           -1 
                                    %99 0.00428184  0           -1 
                        -1 
-1 

-1

