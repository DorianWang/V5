# lqsim 5.16
# lqsim --confidence=1.0,1000 --seed=1049217653 --parseable --output=72-fair.p 72-fair.in
# $Id: 72-fair.p 13764 2020-08-17 19:50:05Z greg $
V y
C 0.945148
I 3
PP 3
NP 1

#!Comment: Group scheduling test
#!User:  1:38:07.000
#!Sys:   0:24:49.000
#!Real:  2:04:31.000

B 4
c1             :c1              0.5         
c2             :c2              0.5         
s1             :s1              1           
s2             :s2              1           
-1

W 2
c1             :c1              s1              5.66933     -1 
                                            %95 0.0140289   -1 
                                            %99 0.032358    -1 
                -1 
c2             :c2              s2              38.9574     -1 
                                            %95 0.239479    -1 
                                            %99 0.552367    -1 
                -1 
-1

VARW 2
c1             :c1              s1              8.87077      -1 
                                            %95 0.18679      -1 
                                            %99 0.430836     -1 
                -1 
c2             :c2              s2              159.368      -1 
                                            %95 2.35747      -1 
                                            %99 5.43756      -1 
                -1 
-1

X 4
c1             :c1              13.3317     -1 
                            %95 0.0824144   -1 
                            %99 0.190091    -1 
                -1 
c2             :c2              80.0404     -1 
                            %95 1.33913     -1 
                            %99 3.08874     -1 
                -1 
s1             :s1              6.66821     -1 
                            %95 0.0186533   -1 
                            %99 0.0430243   -1 
                -1 
s2             :s2              39.9562     -1 
                            %95 0.223778    -1 
                            %99 0.516151    -1 
                -1 
-1

VAR 4
c1             :c1              384.21      -1 
                            %95 4.28139     -1 
                            %99 9.87515     -1 
                -1 
c2             :c2              14479.1     -1 
                            %95 63.1981     -1 
                            %99 145.768     -1 
                -1 
s1             :s1              45.4966     -1 
                            %95 0.402333    -1 
                            %99 0.927994    -1 
                -1 
s2             :s2              1665.03     -1 
                            %95 33.3104     -1 
                            %99 76.8315     -1 
                -1 
-1

FQ 4
c1             :c1              0.750088    10          -1 10
                            %95 0.00460136  0           -1 0
                            %99 0.0106132   0           -1 0
                -1 
c2             :c2              0.249875    20          -1 20
                            %95 0.00422784  0           -1 0
                            %99 0.00975163  0           -1 0
                -1 
s1             :s1              0.749804    4.99985     -1 4.99985
                            %95 0.00211998  2.31599e-05 -1 2.31599e-05
                            %99 0.0048898   5.3419e-05  -1 5.3419e-05
                -1 
s2             :s2              0.250274    10          -1 10
                            %95 0.00144303  1.38213e-05 -1 1.38213e-05
                            %99 0.0033284   3.18793e-05 -1 3.18793e-05
                -1 
-1

P p1 1
c1              1  0 10 c1              0.749257    0           -1 
                                    %95 0.00175     0           -1 
                                    %99 0.00403642  0           -1 
                        -1 
-1 

P p2 1
c2              1  0 20 c2              0.249998    0           -1 
                                    %95 0.00547333  0           -1 
                                    %99 0.0126244   0           -1 
                        -1 
-1 

P p3 2
s1              1  0 5  s1              0.749804    5.66795     -1 
                                    %95 0.00211998  0.01586     -1 
                                    %99 0.0048898   0.0365815   -1 
                        -1 
G g1                                    0.749804    
                                    %95 0.00211998  
                                    %99 0.0048898   
s2              1  0 10 s2              0.250274    38.9572     -1 
                                    %95 0.00144303  0.217061    -1 
                                    %99 0.0033284   0.500657    -1 
                        -1 
G g2                                    0.250274    
                                    %95 0.00144303  
                                    %99 0.0033284   
                -1 
                                        1.00008
                                    %95 0.0025645
                                    %99 0.0059151
-1 

-1

