import pandas as pd
import math
import numpy as np
from mpmath import mp
from statistics import median
from statistics import mean
import re
import sys

sys.set_int_max_str_digits(100_000)

mc = pd.read_csv("csv/mc.csv", skipinitialspace = True, index_col = 'file', dtype = str)
divkc = pd.read_csv("csv/divkc.csv", skipinitialspace = True, index_col = 'file', dtype = {'pmc': str, 'umc': str, 'appmc_y': str, 'appmc_yl': str, 'appmc_yh': str})
d4 = pd.read_csv("csv/d4.ddnnf.csv", skipinitialspace = True, index_col = 'file', dtype = {'mc': str})
total = pd.read_csv("csv/total.csv", skipinitialspace = True)
cls = pd.read_csv("csv/cls.csv", skipinitialspace = True, index_col = 'file')
ck = pd.read_csv("csv/ck.comp.d19.csv", skipinitialspace = True, index_col = 'file', dtype = {'ck_mc': str, 'd4d_mc': str})
ck_amc = pd.read_csv("csv/ck.appmc.wi.csv", skipinitialspace = True, index_col = 'file', dtype = {'mc': str, 'ck_amc_y': str, 'ck_amc_yl': str, 'ck_amc_yh': str})
amc = pd.read_csv("csv/approxmc_e0.8d0.1.run.csv", skipinitialspace = True, index_col = 'file', dtype = {'amc': str})

divkcn4 = pd.read_csv("csv/divkc.appmc.n4.csv", skipinitialspace = True, index_col = 'file', dtype = {'divkc_pmc': str, 'divkc_umc': str})
ckn4 = pd.read_csv("csv/ck.amc.n4.csv", skipinitialspace = True, index_col = 'file', dtype = {'mc': str})

amc = amc[amc.state == "done"]
amc.dropna(inplace = True)

divkc["ok"] = (divkc.splitter_status == "done") & (divkc.proj_status == "done") & (divkc.pd4_status == "done") & (divkc.ud4_status == "done") & (divkc.appmc_status == "done")
ck["ck_ok"] = (ck.ck_status == "done") & (ck.ck_d4_status == "done") & (ck.ck_s_status == "done") & ((ck.ck_time + ck.ck_d4_time + ck.ck_s_time) <= (5 * 3600))
# ck["d4d_ok"] = (ck.d4d_status == "done") & (ck.d4d_s_status == "done")

ck_amc["ok"] = (ck_amc.ck_status == "done") & (ck_amc.ck_d4_status == "done") & (ck_amc.ck_amc_status == "done")

divkcn4["ok"] = (divkcn4.divkc_splitter_status == "done") & (divkcn4.divkc_proj_status == "done") & (divkcn4.divkc_d4p_status == "done") & (divkcn4.divkc_d4u_status == "done") & (divkcn4.divkc_amc1_status == "done") & (divkcn4.divkc_amc2_status == "done") & (divkcn4.divkc_amc3_status == "done") & (divkcn4.divkc_amc4_status == "done")

ckn4["ok"] = (ckn4.ck_status == "done") & (ckn4.ck_d4_status == "done") & (ckn4.ck_amc1_status == "done") & (ckn4.ck_amc2_status == "done") & (ckn4.ck_amc3_status == "done") & (ckn4.ck_amc4_status == "done")


amc["rtime"] = amc.time * 5
divkcn4["rtime"] = (divkcn4.divkc_splitter_time + divkcn4.divkc_proj_time + divkcn4.divkc_d4p_time + divkcn4.divkc_d4u_time) + (divkcn4.divkc_amc1_time + divkcn4.divkc_amc2_time + divkcn4.divkc_amc3_time + divkcn4.divkc_amc4_time) * 5
ckn4["rtime"] = (ckn4.ck_time + ckn4.ck_d4_time) + (ckn4.ck_amc1_time + ckn4.ck_amc2_time + ckn4.ck_amc3_time + ckn4.ck_amc4_time) * 5

print(r"""\documentclass{article}

\usepackage[T1]{fontenc}
\usepackage{amsfonts}
\usepackage{graphicx}
\usepackage{textcomp}
\usepackage{xcolor}
\usepackage{balance}
\usepackage{hyperref}
\usepackage[colorinlistoftodos]{todonotes}
\usepackage{xspace}
\usepackage{framed}
\usepackage{algorithm}
\usepackage{algorithmic}
\renewcommand{\algorithmiccomment}[1]{\hfill // #1}
\usepackage{graphics}
\usepackage{url}
\usepackage{longtable}
\usepackage{tikz}

\usepackage[a4paper,
            left=2cm,
            right=2cm,
            top=2cm,
            bottom=2cm,
            footskip=0.5cm]{geometry}

\usepackage{adjustbox}
\usepackage{subcaption}
\usepackage{bbm}
\usepackage{amsmath, amsfonts, amssymb, commath}

\newcommand\unigen[0]{UniGen3\xspace}
\newcommand\unigenone[0]{UniGen\xspace}
\newcommand\unigentwo[0]{UniGen2\xspace}
\newcommand\spur[0]{SPUR\xspace}
\newcommand\kus[0]{KUS\xspace}
\newcommand\smarch[0]{Smarch\xspace}
\newcommand\sts[0]{STS\xspace}
\newcommand\bddsampler[0]{BDDSampler\xspace}
\newcommand\quicksampler[0]{QuickSampler\xspace}
\newcommand\cmsgen[0]{CMSGen\xspace}

\newcommand\z[0]{Z3\xspace}
\newcommand\minisat[0]{MiniSAT\xspace}
\newcommand\cms[0]{CryptoMiniSAT\xspace}

\newcommand\mctw[0]{McTW\xspace}
\newcommand\dfour[0]{D4\xspace}
\newcommand\dmc[0]{DMC\xspace}
\newcommand\ctod[0]{C2D\xspace}
\newcommand\dsharp[0]{DSharp\xspace}
\newcommand\sharpsat[0]{sharpSAT\xspace}
\newcommand\ganak[0]{GANAK\xspace}

\newcommand\approxmc[0]{ApproxMC\xspace}
\newcommand\approxcount[0]{ApproxCount\xspace}
\newcommand\samplecount[0]{SampleCount\xspace}

\newcommand\barbarik[0]{Barbarik\xspace}

\newcommand\ksk[0]{{\textit k}\xspace}
\newcommand\kss[0]{{\textit s}\xspace}
\newcommand\kst[0]{{\textit t}\xspace}
\newcommand\tw[0]{{\textit tw}\xspace}
\newcommand\deff[0]{$\delta$\xspace}
\newcommand\edeff[0]{$\delta'$\xspace}

\newcommand\divkc[0]{DivKC\xspace}
\newcommand\divkcs[0]{DivKC-S\xspace}
\newcommand\divkcamc[0]{DivKC-AMC\xspace}
\newcommand\capkc[0]{CapKC\xspace}
\newcommand\capkcs[0]{CapKC-S\xspace}
\newcommand\capkcamc[0]{CapKC-AMC\xspace}

\newcommand\nmis[0]{\#{\textit MIS}\xspace}
\newcommand\neqc[0]{\#{\textit eqv}\xspace}

\newcommand\var{\textit{Var}}
\newcommand\forget{\textit{Forget}}
\newcommand\project{\textit{Project}}
\newcommand{\condition}[2]{\ensuremath{#1 \vert_{#2}}}
\newcommand\F{\textit{false}}
\newcommand\T{\textit{true}}
\DeclareMathOperator{\EX}{\mathbb{E}}

\begin{document}
""")

print(r"""\begin{table}[h!]
	\centering
		\begin{tabular}{l|c|c|c|c|c}
			Dataset & \#$F_\textit{total}$ & min($|\var(F)|$) & max($|\var(F)|$) & min($|F|$) & max($|F|$) \\
            \hline
""")

for x in total.index:
    sub = total['folder'][x]
    nbf = total['nbf'][x]
    vsub = total['map'][x]

    lcls = cls[cls.index.str.contains(sub)]

    v_lo = int(min(lcls['nbv']))
    v_hi = int(max(lcls['nbv']))
    v_avg = int(mean(lcls['nbv']))
    v_med = int(median(lcls['nbv']))

    c_lo = int(min(lcls['nbc']))
    c_hi = int(max(lcls['nbc']))
    c_avg = int(mean(lcls['nbc']))
    c_med = int(median(lcls['nbc']))

    print(f"{vsub} & {nbf} & {v_lo} & {v_hi} & {c_lo} & {c_hi} \\\\")

print(r"""            \end{tabular}
            \caption{Dataset summary.
		The first column indicates the dataset, and the \#F column indicates how many satisfiable formulae the dataset contains.
		The following columns indicate the minimum and maximum number of variables (resp. clauses) in the dataset.}
	\label{results:tab:datasets}
\end{table}

""")

print(r"""\begin{table}[h!]
	\centering
    \begin{tabular}{l|c|c|c|c|c}
        Dataset & \#$F_\textit{total}$ & \#$\text{\dfour}$ & \#$\neg \text{\dfour}$ & \#$\textit{\capkc} \land \neg \text{\dfour}$ & \#$\textit{\divkc} \land \neg \textit{\capkc} \land \neg \text{\dfour}$ \\ 
        \hline""")


for x in total.index:
    sub = total['folder'][x]
    nbf = total['nbf'][x]
    vsub = total['map'][x]

    ld4 = d4[d4.index.str.contains(sub)]
    ldivkc = divkc[divkc.index.str.contains(sub)]
    lck = ck[ck.index.str.contains(sub)]

    ld4 = ld4[ld4.status == "done"]
    ldivkc = ldivkc[ldivkc.ok]
    lck = lck[lck.ck_ok]
    #ld4d = lck[lck.d4d_ok]

    sd4 = set(ld4.index)
    sdivkc = set(ldivkc.index)
    sck = set(lck.index)
    #sd4d = set(ld4d.index)

    not_d4 = nbf - len(sd4)
    onlyd4 = len(sd4 - sdivkc - sck)
    #addd4d = len(sd4d - sd4)
    addck = len(sck - sd4)
    adddivkc = len(sdivkc - sck - sd4)

    # k = len(sdivkc - sd4)
    # onlyd4d = len(((sd4d - sck) - sdivkc) - sd4)
    # onlydivkc = len(sdivkc - sd4 - sck - sd4d)

    #if k > 0:
        # k = "\\textbf{" + str(k) + "}"

    # print(f"{vsub} & {nbf} & {onlyd4} & {nbf - len(sd4)} & {k} \\\\")
    print(f"{vsub} & {nbf} & {onlyd4} & {not_d4} & {addck} & {adddivkc} \\\\")

print(r"""    \end{tabular}
        \caption{Experimental results regarding the scalability of \divkc and \capkc.
		Column \#$F_\textit{total}$ indicates the total number of formulae in each dataset.
		The next column shows the number of formulae compiled only by \dfour~\cite{D4} but not by \divkc or \capkc.
        Column \#$\neg\text{\dfour}$ shows the number of formulae not compiled by \dfour.
        Column \#$\textit{CK} \land \neg \text{\dfour}$ shows the number of formulae that are compiled by \capkc but not by \dfour.
        The last column indicates the number of formulae that were only compiled by \divkc, but not by \dfour or \capkc.
	}
	\label{divkc:tab:gen}
\end{table}

""")

print(r"""\begin{table}[h!]
	\centering
    \begin{tabular}{l|c|c|c|c|c}
        Dataset & \#$F_\textit{total}$ & \#$\neg \text{\dfour}$ & \#$\neg \text{\dfour} \land \textit{CapKC}$ & \#$\neg \textit{\dfour} \land \textit{DivKC}$ &  \#$\textit{\capkc} \land \neg \textit{\divkc} \land \neg \text{\dfour}$\\ 
        \hline""")


for x in total.index:
    sub = total['folder'][x]
    nbf = total['nbf'][x]
    vsub = total['map'][x]

    ld4 = d4[d4.index.str.contains(sub)]
    ldivkc = divkc[divkc.index.str.contains(sub)]
    lck = ck[ck.index.str.contains(sub)]

    ld4 = ld4[ld4.status == "done"]
    ldivkc = ldivkc[ldivkc.ok]
    lck = lck[lck.ck_ok]
    # ld4d = lck[lck.d4d_ok]

    sd4 = set(ld4.index)
    sdivkc = set(ldivkc.index)
    sck = set(lck.index)
    # sd4d = set(ld4d.index)

    not_d4 = nbf - len(sd4)
    ck_and_not_d4 = len(sck - sd4)
    divkc_and_not_d4 = len(sdivkc - sd4)
    onlyck = len(sck - sdivkc - sd4)

    # print(f"{vsub} & {nbf} & {onlyd4} & {nbf - len(sd4)} & {k} \\\\")
    print(f"{vsub} & {nbf} & {not_d4} & {ck_and_not_d4} & {divkc_and_not_d4} & {onlyck} \\\\")

print(r"""    \end{tabular}
        \caption{Experimental results regarding the scalability of \divkc and \capkc.
	}
	\label{divkc:tab:gen}
\end{table}

""")


print(r"""\begin{table}[h!]
	\centering
	% \begin{adjustbox}{width=\textwidth}
		\begin{tabular}{l|c|c|c|c|c}
			Dataset & \#F & $Y_l \leq |R_F|$ & $Y_h \geq |R_F|$ & Coverage & $|R_{G_P}| \leq |R_F| \leq |R_{G_U}|$ \\
            \hline""")

for x in total.index:
    sub = total['folder'][x]
    nbf = total['nbf'][x]
    vsub = total['map'][x]

    ldivkc = divkc[divkc.index.str.contains(sub)]
    ldivkc = ldivkc[ldivkc.ok]

    nlow = 0
    nhigh = 0
    nboth = 0
    nbothc = 0
    nb = 0

    for f in ldivkc.index:
        if f in mc.index:
            tm = mp.mpf(mc.mc[f])
            yl = mp.mpf(ldivkc.appmc_yl[f])
            yh = mp.mpf(ldivkc.appmc_yh[f])
            lo = mp.mpf(ldivkc.pmc[f])
            hi = mp.mpf(ldivkc.umc[f])

            nbothc += (tm >= lo) and (tm <= hi)

            nlow += yl <= tm
            nhigh += yh >= tm
            nboth += (yh >= tm) and (yl <= tm)
            nb += 1

    if nb > 0:
        nlow /= nb
        nhigh /= nb
        nboth /= nb
        nbothc /= nb

        print(f"{vsub} & {nb} & {nlow:5.3f} & {nhigh:5.3f} & {nboth:5.3f} & {nbothc:5.3f} \\\\")
    else:
        print(f"{vsub} & {nb} & & & & \\\\")

print(r"""        \end{tabular}
    % \end{adjustbox}
    \caption{Experimental results for \divkcamc.
		Column \#F indicates with how many formulae the statistics have been computed.
		The 'Coverage' column indicates how often $|R_F|$ is within the confidence interval $[Y_l; Y_h]$ and thus measures the accuracy of our method.
	}
	\label{divkc:tab:appmc}
\end{table}

""")

print(r"""\begin{table}[h!]
	\centering
	% \begin{adjustbox}{width=\textwidth}
		\begin{tabular}{l|c|c|c|c|c}
			Dataset & \#F & $Y_l \leq |R_F|$ & $Y_h \geq |R_F|$ & Coverage & $|R_F| \leq |R_{G_U}|$ \\
            \hline""")

for x in total.index:
    sub = total['folder'][x]
    nbf = total['nbf'][x]
    vsub = total['map'][x]

    lck_amc = ck_amc[ck_amc.index.str.contains(sub)]
    lck_amc = lck_amc[lck_amc.ok]

    nlow = 0
    nhigh = 0
    nboth = 0
    ncheck = 0
    nb = 0

    for f in lck_amc.index:
        if f in mc.index:
            pm = int(lck_amc.ck_umc[f])
            tm = int(mc.mc[f])
            amc_n = lck_amc.ck_amc_nsuccess[f]
            amc_s = lck_amc.ck_amc_n[f]
            if pm != tm:
                ncheck += tm <= pm
                tm = mp.mpf(tm)
                yl = mp.mpf(lck_amc.ck_amc_yl[f])
                yh = mp.mpf(lck_amc.ck_amc_yh[f])

                nlow += yl <= tm
                nhigh += yh >= tm
                nboth += (yh >= tm) and (yl <= tm)
                nb += 1

    if nb > 0:
        nlow /= nb
        nhigh /= nb
        nboth /= nb
        ncheck /= nb

        print(f"{vsub} & {nb} & {nlow:5.3f} & {nhigh:5.3f} & {nboth:5.3f} & {ncheck:5.3f} \\\\")
    else:
        print(f"{vsub} & {nb} & & & \\\\")

print(r"""        \end{tabular}
    % \end{adjustbox}
    \caption{Experimental results for CapKC.
		Column \#F indicates with how many formulae the statistics have been computed.
        % Column $Y_l \leq |R_F|$ indicates how often the lower bound returned by CapKC is correct (i.e., smaller than the true model count of $F$).
		% Similarly, column $Y_h \geq |R_F|$ indicates how often the upper bound is correct.
		The 'Coverage' column indicates how often $|R_F|$ is within the confidence interval $[Y_l; Y_h]$ and thus measures the accuracy of our method.
	}
	\label{capkc:tab:appmc}
\end{table}

""")


print(r"""\begin{table}[h!]
	\centering
	% \begin{adjustbox}{width=\textwidth}
		\begin{tabular}{l|c|c|c|c|c|c}
			Dataset & \#F & $Y_l \geq |R_{G_P}|$ & $Y_h \leq |R_{G_U}|$ & Both & \textit{median}($r_c$) & \textit{max}($r_c$) \\ 
            \hline """)

for x in total.index:
    sub = total['folder'][x]
    nbf = total['nbf'][x]
    vsub = total['map'][x]

    ldivkc = divkc[divkc.index.str.contains(sub)]
    ldivkc = ldivkc[ldivkc.ok]

    nlow = 0
    nhigh = 0
    nboth = 0
    nb = 0

    resl = []

    for f in ldivkc.index:
        if f in mc.index:
            tm = mp.mpf(mc.mc[f])
            yl = mp.mpf(ldivkc.appmc_yl[f])
            yh = mp.mpf(ldivkc.appmc_yh[f])
            lo = mp.mpf(ldivkc.pmc[f])
            hi = mp.mpf(ldivkc.umc[f])

            if hi != lo:
                nbothc += (tm >= lo) and (tm <= hi)

                nlow += (yl >= lo) and (yl <= tm)
                nhigh += (yh <= hi) and (yh >= tm)
                nboth += (yl >= lo) and (yh <= hi) and (yl <= tm) and (yh >= tm)
                nb += 1
                if yh < yl:
                    print(f"    error y: {f} : {yh} > {yl}")
                if hi < lo:
                    print(f"    error h: {f} : {hi} > {lo}")

                if (yl <= tm) and (yh >= tm):
                    resl.append((min(yh, hi) - max(yl, lo)) / (hi - lo))

    if nb > 0:
        nlow /= nb
        nhigh /= nb
        nboth /= nb

        print(f"{vsub} & {nb} & {nlow:5.3f} & {nhigh:5.3f} & {nboth:5.3f} & {mp.nstr(median(resl), 3)} & {mp.nstr(max(resl), 3)} \\\\")
    else:
        print(f"{vsub} & {nb} & & & & & \\\\")

print(r"""            \end{tabular}
            % \end{adjustbox}
            \caption{Experimental results comparing the bounds obtained with \divkcamc and with Lemma~\ref{divkc:lem:ebounds}.
	Column \#F indicates with how many formulae the statistics have been computed.
	The 'Both' column indicates how often we have $Y_l \geq |R_{G_P}| \land Y_l \leq |R_F|$
    and $Y_h \leq |R_{G_U}| \land Y_h \geq |R_F|$.
	The last two columns represent the observed median and maximum values of the ratio $r_c = \frac{\textit{min}(Y_h, |R_{G_U}|) - \textit{max}(Y_l, |R_{G_P}|)}{|R_{G_U}| - |R_{G_P}|}$, which was calculated exclusively if $Y_l \leq |R_F| \leq Y_h$.
	The number of formulae on which the last two columns are computed can easily be obtained by multiplying the \#F column with the 'Coverage' column in Table~\ref{divkc:tab:appmc}.
	}
	\label{divkc:tab:coverage}
\end{table}

""")

print(r"""\begin{table}[h!]
	\centering
	% \begin{adjustbox}{width=\textwidth}
		\begin{tabular}{l|c|c|c|c}
			Dataset & \#F & $Y_h \leq |R_{G_U}|$ & \textit{median}($r_c$) & \textit{max}($r_c$) \\ 
            \hline """)

for x in total.index:
    sub = total['folder'][x]
    nbf = total['nbf'][x]
    vsub = total['map'][x]

    lck_amc = ck_amc[ck_amc.index.str.contains(sub)]
    lck_amc = lck_amc[lck_amc.ok]

    nlow = 0
    nhigh = 0
    nboth = 0
    nb = 0

    resl = []

    for f in lck_amc.index:
        if f in mc.index:
            tm = int(mc.mc[f])
            pm = int(lck_amc.ck_umc[f])
            yl = mp.mpf(lck_amc.ck_amc_yl[f])
            yh = mp.mpf(lck_amc.ck_amc_yh[f])

            if pm != tm:
                tm = mp.mpf(tm)
                pm = mp.mpf(pm)
                nlow += (yl <= tm) and (yl <= pm)
                nhigh += (yh >= tm) and (yh <= pm)
                nboth += (yl <= tm) and (yh >= tm)

                nb += 1

                # if pm > 0:
                if (yl <= tm) and (yh >= tm):
                    resl.append((min(yh, pm) - max(yl, 0)) / (pm - 0))
                    # resl.append(yl)

    if nb > 0:
        nlow /= nb
        nhigh /= nb
        nboth /= nb

        if len(resl) > 0:
            med = mp.nstr(median(resl), 3)
            ma = mp.nstr(max(resl), 3)
        else:
            med = "-"
            ma = "-"


        print(f"{vsub} & {nb} & {nhigh:5.3f} & {med} & {ma} \\\\")
    else:
        print(f"{vsub} & {nb} & & & & & \\\\")

print(r"""            \end{tabular}
            % \end{adjustbox}
            \caption{CapKC.
	}
	\label{divkc:tab:coverage}
\end{table}

""")


print(r"""\begin{table}[h!]
	\centering
	% \begin{adjustbox}{width=\textwidth}
		\begin{tabular}{l|c|c|c|c}
			Dataset & \#F & $l \leq Y_{\text{\approxmc}} \leq h$ & $l \leq Y_{\text{\divkcamc}} \leq h$ & $l \leq Y_{\text{\capkcamc}} \leq h$ \\ 
            \hline """)

for x in total.index:
    sub = total['folder'][x]
    nbf = total['nbf'][x]
    vsub = total['map'][x]

    ldivkc = divkc[divkc.index.str.contains(sub)]
    ldivkc = ldivkc[ldivkc.ok]

    lck_amc = ck_amc[ck_amc.index.str.contains(sub)]
    lck_amc = lck_amc[lck_amc.ok]

    lamc = amc[amc.index.str.contains(sub)]

    epsilon = 1.2

    nb = 0
    nappmc = 0
    ndivkc = 0
    nck = 0

    resl = []

    for f in lck_amc.index:
        if f in mc.index and f in lamc.index and f in ldivkc.index:
            ydivkc = mp.mpf(ldivkc.appmc_y[f])
            yck = mp.mpf(lck_amc.ck_amc_y[f])
            yamc = mp.mpf(lamc.amc[f])

            tm = int(mc.mc[f])

            high = epsilon * mp.mpf(tm)
            low = mp.mpf(tm) / epsilon

            nb += 1

            nappmc += (yamc <= high) and (low <= yamc)
            ndivkc += (ydivkc <= high) and (low <= ydivkc)
            nck += (yck <= high) and (low <= yck)

    if nb > 0:
        nappmc /= nb
        ndivkc /= nb
        nck /= nb

        print(f"{vsub} & {nb} & {nappmc:5.3f} & {ndivkc:5.3f} & {nck:5.3f} \\\\")
    else:
        print(f"{vsub} & {nb} & & & \\\\")

print(r"""            \end{tabular}
            % \end{adjustbox}
            \caption{CapKC.
	}
	%\label{divkc:tab:coverage}
\end{table}

""")

print(r"""\begin{table}[h!]
	\centering
	% \begin{adjustbox}{width=\textwidth}
		\begin{tabular}{l|c|c|c|c|c|c}
            Dataset & \#F & \#\divkcamc & $\textit{log}_{10}(\textit{min})$ & \textit{mean} & \textit{median} & $\textit{log}_{10}(\textit{max})$ \\
            \hline """)

for x in total.index:
    sub = total['folder'][x]
    nbf = total['nbf'][x]
    vsub = total['map'][x]

    ldivkc = divkcn4[divkcn4.index.str.contains(sub)]
    ldivkc = ldivkc[ldivkc.ok]

    lamc = amc[amc.index.str.contains(sub)]

    nb = 0
    ndivkc = 0
    ratio = []

    for f in ldivkc.index:
        if f in lamc.index:
            nb += 1

            dtime = ldivkc.rtime[f]
            atime = lamc.rtime[f]

            ndivkc += dtime <= atime
            ratio.append(atime / dtime)

    if nb > 0:
        mi = f"{math.log(min(ratio), 10):5.1f}"
        me = f"{mean(ratio):5.1f}"
        med = f"{median(ratio):5.1f}"
        ma = f"{math.log(max(ratio), 10):5.1f}"

        print(f"{vsub} & {nb} & {ndivkc} & {mi} & {me} & {med} & {ma} \\\\")
    else:
        print(f"{vsub} & {nb} & & & \\\\")

print(r"""            \end{tabular}
            % \end{adjustbox}
            \caption{DivKC vs Approxmc7.
	}
	%\label{divkc:tab:coverage}
\end{table}

""")

print(r"""\begin{table}[h!]
	\centering
	% \begin{adjustbox}{width=\textwidth}
		\begin{tabular}{l|c|c|c|c|c|c}
            Dataset & \#F & \#\capkcamc & $\textit{log}_{10}(\textit{min})$ & \textit{mean} & \textit{median} & $\textit{log}_{10}(\textit{max})$ \\
            \hline """)

for x in total.index:
    sub = total['folder'][x]
    nbf = total['nbf'][x]
    vsub = total['map'][x]

    lck = ckn4[ckn4.index.str.contains(sub)]
    lck = lck[lck.ok]

    lamc = amc[amc.index.str.contains(sub)]

    nb = 0
    nck = 0
    ratio = []

    for f in lck.index:
        if f in lamc.index:
            nb += 1

            dtime = lck.rtime[f]
            atime = lamc.rtime[f]

            nck += dtime <= atime
            ratio.append(atime / dtime)

    if nb > 0:
        mi = f"{math.log(min(ratio), 10):5.1f}"
        me = f"{mean(ratio):5.1f}"
        med = f"{median(ratio):5.1f}"
        ma = f"{math.log(max(ratio), 10):5.1f}"

        print(f"{vsub} & {nb} & {nck} & {mi} & {me} & {med} & {ma} \\\\")
    else:
        print(f"{vsub} & {nb} & & & \\\\")

print(r"""            \end{tabular}
            % \end{adjustbox}
            \caption{CapKC vs Approxmc7.
	}
	%\label{divkc:tab:coverage}
\end{table}

""")

print(r"""\begin{table}[h!]
	\centering
	% \begin{adjustbox}{width=\textwidth}
		\begin{tabular}{l|c|c|c|c|c|c}
            Dataset & \#F & \#\capkcamc & $\textit{log}_{10}(\textit{min})$ & \textit{mean} & \textit{median} & $\textit{log}_{10}(\textit{max})$ \\
            \hline """)

for x in total.index:
    sub = total['folder'][x]
    nbf = total['nbf'][x]
    vsub = total['map'][x]

    lck = ckn4[ckn4.index.str.contains(sub)]
    lck = lck[lck.ok]

    lamc = amc[amc.index.str.contains(sub)]

    nb = 0
    nck = 0
    ratio = []

    for f in lck.index:
        if f in lamc.index and (not f in mc.index or int(lck.ck_umc[f]) != int(mc.mc[f])):
            nb += 1

            dtime = lck.rtime[f]
            atime = lamc.rtime[f]

            nck += dtime <= atime
            ratio.append(atime / dtime)

    if nb > 0:
        mi = f"{math.log(min(ratio), 10):5.1f}"
        me = f"{mean(ratio):5.1f}"
        med = f"{median(ratio):5.1f}"
        ma = f"{math.log(max(ratio), 10):5.1f}"

        print(f"{vsub} & {nb} & {nck} & {mi} & {me} & {med} & {ma} \\\\")
    else:
        print(f"{vsub} & {nb} & & & \\\\")

print(r"""            \end{tabular}
            % \end{adjustbox}
            \caption{CapKC vs Approxmc7.
	}
	%\label{divkc:tab:coverage}
\end{table}

""")


print(r"""
\end{document}
""")
