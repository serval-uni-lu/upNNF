The tables are generated with the `generate_tables.py` python script which depends on `pandas`, `numpy` and `mpmath`.
To generate the file `tables.detail.tex` and associated PDF, please modify the `generate_tables.py` script in line 12 and 13 to use the file `total.detail.csv` instead of `total.csv`.

The files in the `csv` folder contains the raw experiment results with the commands that were used to execute them. The memory usages available in the files are expressed in KB and the time usage in seconds.
The `status` columns contain whether the command terminated successfully (`done`) or with an error (`err`) or were killed because of a timeout (`timeout`) or memory usage (`mem`).

Columns such as `mc` indicate the model count of the formula (in `d4.ddnnf.csv` for example).
Columns such as `umc` indicate the model count of the upper bound (in `ck.comp.d19.csv` for example).


The file `ck.comp.d19.csv` contains CapKC experiments as well as the experiments conducted with a modified version of `D4` which limites the depth of the d-DNNF.
