# lqsim 5.16
# lqsim --confidence=1.0,1000 --seed=1049217653 --parseable --output=42-activities.p 42-activities.in
# $Id: 42-activities.p 13764 2020-08-17 19:50:05Z greg $
V y
C 0.815583
I 3
PP 2
NP 2

#!Comment: Activities with AND fork-join - reply on branch.
#!User:  0:22:22.000
#!Sys:   0:08:42.000
#!Real:  0:34:21.000

B 2
client         :client          0.606061    
server         :server          0.666667    
-1

W 1
client         :client          server          0.199111    0           -1 
                                            %95 0.00822146  0           -1 
                                            %99 0.0189631   0           -1 
                -1 
-1

VARW 1
client         :client          server          0.192777     0            -1 
                                            %95 0.0158152    0            -1 
                                            %99 0.0364782    0            -1 
                -1 
-1

J 0
server         :fork1           fork2           0.998867    0.517716   
                                            %95 0.0100661   0.00756105 
                                            %99 0.0232177   0.0174398  
-1 
-1

X 2
client         :client          2.14663     0           -1 
                            %95 0.0227703   0           -1 
                            %99 0.0525205   0           -1 
                -1 
server         :server          0.949096    0.548964   -1 
                            %95 0.00233132  0.00757292  -1 
                            %99 0.00537725  0.0174672   -1 
                -1
:
                fork1           0.69919     -1 
                            %95 0.00234821  -1 
                            %99 0.00541622  -1 
                fork2           0.798522    -1 
                            %95 0.0084802   -1 
                            %99 0.0195598   -1 
                join            0.249288    -1 
                            %95 0.00240958  -1 
                            %99 0.00555777  -1 
                server          0.249906    -1 
                            %95 0.00173926  -1 
                            %99 0.00401165  -1 
                -1 
-1

VAR 2
client         :client          6.78496     0           -1 
                            %95 0.367075    0           -1 
                            %99 0.84667     0           -1 
                -1 
server         :server          0.492199    0.33026    -1 
                            %95 0.0110237   0.0231964   -1 
                            %99 0.0254265   0.0535032   -1 
                -1
:
                fork1           0.428708    -1 
                            %95 0.0125636   -1 
                            %99 0.0289783   -1 
                fork2           0.477102    -1 
                            %95 0.0164034   -1 
                            %99 0.0378349   -1 
                join            0.0622783   -1 
                            %95 0.00219326  -1 
                            %99 0.00505881  -1 
                server          0.0627822   -1 
                            %95 0.000503361 -1 
                            %99 0.00116102  -1 
                -1 
-1

FQ 2
client         :client          0.465851    1           0           -1 1
                            %95 0.00494065  0           0           -1 0
                            %99 0.0113957   0           0           -1 0
                -1 
server         :server          0.465438    5.36505e-07 2.26215e-07-1 0.697254
                            %95 0.0028492   0.0031518   0.0020466   -1 0.00375798
                            %99 0.00657177  0.00726971  0.00472054  -1 0.00866788
                -1
:
                fork1           0.465438    0.32543     -1 
                            %95 1.63674     0.00173513  -1 
                            %99 3.77519     0.00400213  -1 
                fork2           0.465438    0.371662    -1 
                            %95 1.64251     0.00190854  -1 
                            %99 3.78849     0.00440211  -1 
                join            0.465438    0.116028    -1 
                            %95 1.63674     0.00164019  -1 
                            %99 3.77519     0.00378315  -1 
                server          0.465438    0.116316    -1 
                            %95 1.64251     0.00152017  -1 
                            %99 3.78849     0.00350633  -1 
                -1 
-1

P client 1
client          1  0 1  client          0.465581    0           0           -1 
                                    %95 0.000342996 0           0           -1 
                                    %99 0.00079113  0           0           -1 
                        -1 
-1 

P server 1
server          1  0 1  server          0.697254    0           0          -1 
                                    %95 0.00181793  0           0           -1 
                                    %99 0.00419312  0           0           -1 
-1
                       :
                        fork1           1           0.299491    -1 
                                    %95 0           0.00307207  -1 
                                    %99 0           0.00708583  -1 
                        fork2           1           0.199354    -1 
                                    %95 0           0.00165412  -1 
                                    %99 0           0.00381528  -1 
                        join            1           0           -1 
                                    %95 0           0           -1 
                                    %99 0           0           -1 
                        server          1           0           -1 
                                    %95 0           0           -1 
                                    %99 0           0           -1 
                        -1 
                                        0.697254    
                                    %95 0.00181793  
                                    %99 0.00419312  
-1 

-1

