# lqsim 5.16
# lqsim --confidence=1.0,1000 --seed=1049217653 --parseable --output=71-fair.p 71-fair.in
# $Id: 71-fair.p 13764 2020-08-17 19:50:05Z greg $
V y
C 0.97631
I 4
PP 2
NP 1

#!Comment: Fair Queueing Test 1
#!User:  2:00:56.000
#!Sys:   0:24:43.000
#!Real:  2:27:28.000

B 7
c0             :c0              0.47619     
c1             :c1              0.322581    
s0             :s0              1           
s1             :s1              1           
s2             :s2              1           
s3             :s3              1           
s4             :s4              1           
-1

W 5
c0             :c0              s0              25.6359     -1 
                                            %95 1.33175     -1 
                                            %99 2.44462     -1 
                c0              s1              25.7991     -1 
                                            %95 1.40052     -1 
                                            %99 2.57085     -1 
                -1 
c1             :c1              s2              57.1806     -1 
                                            %95 3.63162     -1 
                                            %99 6.66633     -1 
                c1              s3              56.8009     -1 
                                            %95 2.93207     -1 
                                            %99 5.38222     -1 
                c1              s4              57.1774     -1 
                                            %95 4.08615     -1 
                                            %99 7.50068     -1 
                -1 
-1

VARW 5
c0             :c0              s0              260.205      -1 
                                            %95 5.62692      -1 
                                            %99 10.329       -1 
                c0              s1              261.008      -1 
                                            %95 3.7548       -1 
                                            %99 6.89246      -1 
                -1 
c1             :c1              s2              1916.95      -1 
                                            %95 225.507      -1 
                                            %99 413.949      -1 
                c1              s3              1900.54      -1 
                                            %95 227.045      -1 
                                            %99 416.772      -1 
                c1              s4              1905.44      -1 
                                            %95 69.5629      -1 
                                            %99 127.692      -1 
                -1 
-1

X 7
c0             :c0              57.0933     -1 
                            %95 0.158717    -1 
                            %99 0.291347    -1 
                -1 
c1             :c1              200.05      -1 
                            %95 3.42546     -1 
                            %99 6.28791     -1 
                -1 
s0             :s0              2.80553     -1 
                            %95 0.00412434  -1 
                            %99 0.00757079  -1 
                -1 
s1             :s1              2.8079      -1 
                            %95 0.00908257  -1 
                            %99 0.0166723   -1 
                -1 
s2             :s2              9.48429     -1 
                            %95 0.142023    -1 
                            %99 0.260703    -1 
                -1 
s3             :s3              9.47686     -1 
                            %95 0.0542844   -1 
                            %99 0.0996464   -1 
                -1 
s4             :s4              9.45985     -1 
                            %95 0.0929897   -1 
                            %99 0.170695    -1 
                -1 
-1

VAR 7
c0             :c0              5425.4      -1 
                            %95 42.1188     -1 
                            %99 77.3148     -1 
                -1 
c1             :c1              59335       -1 
                            %95 3082.53     -1 
                            %99 5658.4      -1 
                -1 
s0             :s0              7.88684     -1 
                            %95 0.0850614   -1 
                            %99 0.156142    -1 
                -1 
s1             :s1              7.92706     -1 
                            %95 0.14585     -1 
                            %99 0.267727    -1 
                -1 
s2             :s2              99.3731     -1 
                            %95 21.323      -1 
                            %99 39.1412     -1 
                -1 
s3             :s3              99.5595     -1 
                            %95 22.3923     -1 
                            %99 41.1042     -1 
                -1 
s4             :s4              99.0309     -1 
                            %95 20.7192     -1 
                            %99 38.0329     -1 
                -1 
-1

FQ 7
c0             :c0              0.350306    20          -1 20
                            %95 0.00100602  0           -1 0
                            %99 0.00184668  0           -1 0
                -1 
c1             :c1              0.0999764   20          -1 20
                            %95 0.00172342  0           -1 0
                            %99 0.00316358  0           -1 0
                -1 
s0             :s0              0.350081    0.982164    -1 0.982164
                            %95 0.0011508   0.00219108  -1 0.00219108
                            %99 0.00211245  0.00402202  -1 0.00402202
                -1 
s1             :s1              0.34985     0.982344    -1 0.982344
                            %95 0.000624203 0.00250901  -1 0.00250901
                            %99 0.00114581  0.00460564  -1 0.00460564
                -1 
s2             :s2              0.100136    0.949698    -1 0.949698
                            %95 0.00192845  0.00577197  -1 0.00577197
                            %99 0.00353993  0.0105952   -1 0.0105952
                -1 
s3             :s3              0.100096    0.948597    -1 0.948597
                            %95 0.000151636 0.00455874  -1 0.00455874
                            %99 0.000278349 0.00836819  -1 0.00836819
                -1 
s4             :s4              0.100241    0.948267    -1 0.948267
                            %95 0.000610437 0.00705083  -1 0.00705083
                            %99 0.00112054  0.0129428   -1 0.0129428
                -1 
-1

P client 2
c0              1  0 20 c0              0.0349716   0           -1 
                                    %95 0.000111389 0           -1 
                                    %99 0.00020447  0           -1 
                        -1 
c1              1  0 20 c1              0.0100086   0           -1 
                                    %95 0.000103605 0           -1 
                                    %99 0.00019018  0           -1 
                        -1 
                -1 
                                        0.0449801
                                    %95 0.000152123
                                    %99 0.000279243
-1 

P server 5
s0              1  0 1  s0              0.350081    1.80603     -1 
                                    %95 0.0011508   0.00647946  -1 
                                    %99 0.00211245  0.0118939   -1 
                        -1 
s1              1  0 1  s1              0.34985     1.80721     -1 
                                    %95 0.000624203 0.00512037  -1 
                                    %99 0.00114581  0.00939914  -1 
                        -1 
G g1                                    0.699931    
                                    %95 0.00130918  
                                    %99 0.00240319  
s2              1  0 1  s2              0.100136    8.48457     -1 
                                    %95 0.00192845  0.132247    -1 
                                    %99 0.00353993  0.242758    -1 
                        -1 
s3              1  0 1  s3              0.100096    8.47865     -1 
                                    %95 0.000151636 0.0459348   -1 
                                    %99 0.000278349 0.0843196   -1 
                        -1 
s4              1  0 1  s4              0.100241    8.46249     -1 
                                    %95 0.000610437 0.085575    -1 
                                    %99 0.00112054  0.157085    -1 
                        -1 
G g2                                    0.300474    
                                    %95 0.00202843  
                                    %99 0.00372347  
                -1 
                                        1.00041
                                    %95 0.00241423
                                    %99 0.00443165
-1 

-1

