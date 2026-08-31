To generate the uniformity test results tables, please run the python script `lv2b.py` on the desired files. The script depends on `pandas` and `numpy`.

As an example:
```
python3 lv2b.py ignore_25c/*
```

Generates:
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
