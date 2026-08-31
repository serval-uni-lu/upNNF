To generate the uniformity test results tables, please run the python script `lv2b.py` on the desired files. The script depends on `pandas` and `numpy`.

Please see the statistical test [repository](https://github.com/serval-uni-lu/urs_test) or the paper for more details.

```
@article{zeyen2025testing,
  title={Testing Uniform Random Samplers: Methods, Datasets and Protocols},
  author={Zeyen, Olivier and Cordy, Maxime and Gubri, Martin and Perrouin, Gilles and Acher, Mathieu},
  journal={ACM Transactions on Software Engineering and Methodology},
  year={2025},
  publisher={ACM New York, NY}
}

```

As an example:
```
python3 lv2b.py ignore_25c/*
```

Generates:
```
File                                          |  #F |     HMP | #Fails | #Bonferroni | Time (h)
ignore_25c/omega_birthday_b1000_c10_mc.csv    | 144 | 0.074 + |      3 |           0 |   40.552
ignore_25c/omega_freq_nb_var_b1000_c10_mc.csv |  74 | 0.069 + |      1 |           0 |   30.854
ignore_25c/omega_freq_var_b1000_c10_mc.csv    | 177 | 0.037 + |      2 |           0 |   19.587
ignore_25c/omega_modbit_q128_b1000_c10_mc.csv |  86 | 0.220 + |      0 |           0 |   40.024
ignore_25c/omega_modbit_q16_b1000_c10_mc.csv  | 181 | 0.214 + |      1 |           0 |    9.044
ignore_25c/omega_modbit_q2_b1000_c10_mc.csv   | 181 | 0.055 + |      2 |           0 |    9.241
ignore_25c/omega_modbit_q32_b1000_c10_mc.csv  | 179 | 0.113 + |      3 |           0 |    8.231
ignore_25c/omega_modbit_q4_b1000_c10_mc.csv   | 181 | 0.219 + |      0 |           0 |    9.047
ignore_25c/omega_modbit_q64_b1000_c10_mc.csv  | 121 | 0.108 + |      1 |           0 |   37.265
ignore_25c/omega_modbit_q8_b1000_c10_mc.csv   | 181 | 0.111 + |      4 |           0 |    9.419
```

Here we can see the results for CapKC-S when we ignored the last 25 clauses (`ignore_25c`).

Similarly if we ignore the last 50 clauses:
```
python3 lv2b.py ignore_50c/*
```

```
File                                          |  #F |     HMP | #Fails | #Bonferroni | Time (h)
ignore_50c/omega_birthday_b1000_c10_mc.csv    |  54 | 0.208 + |      1 |           0 |   30.334
ignore_50c/omega_freq_nb_var_b1000_c10_mc.csv |  24 | 0.013 + |      2 |           0 |   10.949
ignore_50c/omega_freq_var_b1000_c10_mc.csv    | 104 | 0.062 + |      2 |           0 |   41.287
ignore_50c/omega_modbit_q128_b1000_c10_mc.csv |  21 | 0.126 + |      0 |           0 |   11.201
ignore_50c/omega_modbit_q16_b1000_c10_mc.csv  | 102 | 0.186 + |      1 |           0 |   37.377
ignore_50c/omega_modbit_q2_b1000_c10_mc.csv   | 104 | 0.183 + |      0 |           0 |   25.428
ignore_50c/omega_modbit_q32_b1000_c10_mc.csv  | 106 | 0.011 + |      3 |           0 |   48.840
ignore_50c/omega_modbit_q4_b1000_c10_mc.csv   | 132 | 0.202 + |      0 |           0 |   34.877
ignore_50c/omega_modbit_q64_b1000_c10_mc.csv  |  29 | 0.269 + |      0 |           0 |   13.679
ignore_50c/omega_modbit_q8_b1000_c10_mc.csv   | 132 | 0.048 + |      1 |           0 |   42.180
```

And for divkc:
```
python3 lv2b.py divkc/*     
```

```
File                                           |  #F |     HMP | #Fails | #Bonferroni | Time (h)
divkc/omega_birthday_b1000_c10_ksampler.csv    | 140 | 0.005   |      2 |           1 |   40.611
divkc/omega_birthday_b1000_c10_rsampler.csv    | 139 | 0.000   |      4 |           1 |   23.282
divkc/omega_chisquared_b1000_c10_ksampler.csv  |  65 | 0.000   |     11 |           9 |   40.851
divkc/omega_chisquared_b1000_c10_rsampler.csv  |  70 | 0.185 + |      0 |           0 |   25.985
divkc/omega_freq_nb_var_b1000_c10_ksampler.csv |  77 | 0.000   |     21 |          18 |   49.026
divkc/omega_freq_nb_var_b1000_c10_rsampler.csv |  82 | 0.196 + |      1 |           0 |   34.427
divkc/omega_freq_var_b1000_c10_ksampler.csv    | 176 | 0.000   |     65 |          54 |   26.862
divkc/omega_freq_var_b1000_c10_rsampler.csv    | 176 | 0.000   |      6 |           1 |   12.896
divkc/omega_modbit_q128_b1000_c10_ksampler.csv |  83 | 0.000   |     25 |          19 |   60.208
divkc/omega_modbit_q128_b1000_c10_rsampler.csv |  88 | 0.000   |      1 |           1 |   49.047
divkc/omega_modbit_q16_b1000_c10_ksampler.csv  | 178 | 0.000   |      9 |           3 |    3.291
divkc/omega_modbit_q16_b1000_c10_rsampler.csv  | 178 | 0.206 + |      0 |           0 |    0.947
divkc/omega_modbit_q2_b1000_c10_ksampler.csv   | 178 | 0.199 + |      0 |           0 |    3.124
divkc/omega_modbit_q2_b1000_c10_rsampler.csv   | 178 | 0.127 + |      3 |           0 |    0.842
divkc/omega_modbit_q32_b1000_c10_ksampler.csv  | 178 | 0.000   |     28 |          16 |    8.699
divkc/omega_modbit_q32_b1000_c10_rsampler.csv  | 178 | 0.277 + |      0 |           0 |    4.598
divkc/omega_modbit_q4_b1000_c10_ksampler.csv   | 178 | 0.114 + |      3 |           0 |    3.565
divkc/omega_modbit_q4_b1000_c10_rsampler.csv   | 178 | 0.124 + |      3 |           0 |    0.877
divkc/omega_modbit_q64_b1000_c10_ksampler.csv  | 126 | 0.000   |     40 |          27 |   63.311
divkc/omega_modbit_q64_b1000_c10_rsampler.csv  | 132 | 0.170 + |      1 |           0 |   44.348
divkc/omega_modbit_q8_b1000_c10_ksampler.csv   | 178 | 0.129 + |      1 |           0 |    3.205
divkc/omega_modbit_q8_b1000_c10_rsampler.csv   | 178 | 0.141 + |      2 |           0 |    0.919
```
