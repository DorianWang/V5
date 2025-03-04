# lqns 5.14
# lqns --pragma=variance=mol,threads=mak --parseable 90-A01.in
# $Id: 90-A01.p 13675 2020-07-10 15:29:36Z greg $
V y
C 4.69881e-07
I 9
PP 4
NP 3

#!Comment: A1 
#!User:  0:00:00.011
#!Sys:   0:00:00.000
#!Real:  0:00:00.011
#!Solver: 3 27 298 3952 55140 3.35771e+08 0

B 7
t1             :e1              0.211416    
t2             :e2              0.15674     
t3             :e3              0.47619     
                e4              1.25        
t4             :e5              0.384615    
t5             :e6              0.333333    
                e7              1.25        
-1

W 7
t1             :e1              e3              0           0.441364    0           -1 
                e1              e4              0           0.441364    0           -1 
                e1              e5              0           9.03386     9.42145     -1 
                -1 
t2             :e2              e5              0           11.3721     10.3482     -1 
                e2              e6              0           3.08648     3.15379     -1 
                e2              e7              0           0           3.15379     -1 
                -1 
t4             :e5              e6              1.36074     1.82747     0           -1 
                -1 
-1

X 7
t1             :e1              0           23.4407     12.4921     -1 
                -1 
t2             :e2              0           36.5128     36.1197     -1 
                -1 
t3             :e3              0.1         2           0           -1 
                e4              0.3         0.5         0           -1 
                -1 
t4             :e5              1.48044     4.90692     0           -1 
                -1 
t5             :e6              1.00013     2.00013     0           -1 
                e7              0.100127    0.700127    0           -1 
                -1 
-1

VAR 7
t1             :e1              0           1773.08     923.083     -1 
                -1 
t2             :e2              0           3221.21     2268.68     -1 
                -1 
t3             :e3              0.01        4           0           -1 
                e4              0.09        0.25        0           -1 
                -1 
t4             :e5              9.62331     56.2961     0           -1 
                -1 
t5             :e6              1           4           0           -1 
                e7              0.01        0.49        0           -1 
                -1 
-1

FQ 5
t1             :e1              0.0278298   0           0.652348    0.347652    -1 1
                -1 
t2             :e2              0.0137679   0           0.502706    0.497294    -1 1
                -1 
t3             :e3              0.0278298   0.00278298  0.0556596   0           -1 0.0584425
                e4              0.0139149   0.00417447  0.00695745  0           -1 0.0111319
                -1 
                                0.0417447   0.00695745  0.062617    0           -1 0.0695745
t4             :e5              0.128953    0.190907    0.632762    0           -1 0.823669
                -1 
t5             :e6              0.331884    0.331927    0.663811    0           -1 0.995738
                e7              0.0137679   0.00137855  0.00963932  0           -1 0.0110179
                -1 
                                0.345652    0.333305    0.67345     0           -1 1.00676
-1

P p1 1
t1              1  0 1  e1              0.0556596   0           0           0           -1 
                        -1 
-1 

P p2 2
t2              1  0 1  e2              0.00413038  0           6.62108     9.26951     -1 
                        -1 
t5              2  0 1  e6              0.995653    0.000127457 0           0           -1 
                        e7              0.0110144   0.000127457 0.000127457 0           -1 
                        -1 
                                        1.00667     
                -1 
                                        1.0108
-1 

P p3 1
t3              2  0 1  e3              0.0584425   0           0           0           -1 
                        e4              0.0111319   0           0           0           -1 
                        -1 
                                        0.0695745   
-1 

P p4 1
t4              1  0 1  e5              0.0515812   0           0           0           -1 
                        -1 
-1 

-1

