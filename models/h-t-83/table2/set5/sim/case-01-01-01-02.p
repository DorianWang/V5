# petrisrvn 4.1
# petrisrvn -p  case-01-01-01-02.in
V y
C 9.8173e-06
I 296
PP 5
NP 1
#!User:  0:00:00.26
#!Sys:   0:00:00.14
#!Real:  0:00:00.65

W 16
  t1 : -1

:
     a1 d1 0.000000 -1
     a1 d2 0.000000 -1
     a1 d3 0.000000 -1
     a1 d4 0.000000 -1
     b1 d1 0.002358 -1
     b1 d2 0.002358 -1
     b1 d3 0.002358 -1
     b1 d4 0.002358 -1
     b2 d1 0.003593 -1
     b2 d2 0.003593 -1
     b2 d3 0.003593 -1
     b2 d4 0.003593 -1
     b3 d1 0.005076 -1
     b3 d2 0.005076 -1
     b3 d3 0.005076 -1
     b3 d4 0.005076 -1
  -1
-1


J 1
t1 : b3b b1 0.752958 0.000000
  -1
-1


X 5
t1 : e1 1.253004 -1
  -1

:
     a1 0.500000 -1
     b1 0.527589 -1
     b2 0.543720 -1
     b2a 0.000000 -1
     b2b 0.000000 -1
     b3 0.471250 -1
     b3a 0.000000 -1
     b3b 0.000000 -1
     c1 0.000000 -1
  -1
d1 : d1 0.040000 -1
  -1
d2 : d2 0.040000 -1
  -1
d3 : d3 0.040000 -1
  -1
d4 : d4 0.040000 -1
  -1
-1

FQ 5
t1 : e1 0.798082 1.000000 -1 1.000000
-1

:
    a1              0.798082 0.399041 -1
    b1              0.797976 0.421003 -1
    b2              0.399103 0.217000 -1
    b2a             0.798082 0.000000 -1
    b2b             0.798124 0.000000 -1
    b3              0.266089 0.125394 -1
    b3a             0.798082 0.000000 -1
    b3b             0.798131 0.000000 -1
    c1              0.798088 0.000000 -1
  -1
d1 : d1 5.639239 0.225570 -1 0.225570
-1
d2 : d2 5.639239 0.225570 -1 0.225570
-1
d3 : d3 5.639239 0.225570 -1 0.225570
-1
d4 : d4 5.639239 0.225570 -1 0.225570
-1
-1

P p1 1
t1 1 0 1 e1 0.201857 0.000000 -1 -1

:
         a1 0.0798082 0 -1
         b1 0.0796521 0.00504839 -1
         b2 0.0397565 0.0100672 -1
         b2a 0 0 -1
         b2b 0 0 -1
         b3 0.00264033 0.0145102 -1
         b3a 0 0 -1
         b3b 0 0 -1
         c1 0 0 -1
 -1
-1

P d1 1
d1 1 0 1 d1 0.225570 0.000000 -1 -1
-1

P d2 1
d2 1 0 1 d2 0.225570 0.000000 -1 -1
-1

P d3 1
d3 1 0 1 d3 0.225570 0.000000 -1 -1
-1

P d4 1
d4 1 0 1 d4 0.225570 0.000000 -1 -1
-1
-1

