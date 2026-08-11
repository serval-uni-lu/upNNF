import pandas as pd
import numpy as np
from mpmath import mp
from statistics import median
from statistics import mean
import re
import sys

sys.set_int_max_str_digits(100_000)

mc = pd.read_csv("csv/mc.csv", skipinitialspace = True, index_col = 'file')
divkc = pd.read_csv("csv/divkc.csv", skipinitialspace = True, index_col = 'file')
d4 = pd.read_csv("csv/d4.ddnnf.csv", skipinitialspace = True, index_col = 'file')
total = pd.read_csv("csv/total.csv", skipinitialspace = True)
cls = pd.read_csv("csv/cls.csv", skipinitialspace = True, index_col = 'file')
ck = pd.read_csv("csv/ck.comp.d19.csv", skipinitialspace = True, index_col = 'file')


# mc.dropna(inplace = True)
# divkc.dropna(inplace = True)
# total.dropna(inplace = True)

# d = divkc.join(mc, on = 'file')
# d.dropna(inplace = True)

divkc["ok"] = (divkc.splitter_status == "done") & (divkc.proj_status == "done") & (divkc.pd4_status == "done") & (divkc.ud4_status == "done") & (divkc.appmc_status == "done")
ck["ck_ok"] = (ck.ck_status == "done") & (ck.ck_d4_status == "done") & (ck.ck_s_status == "done")
ck["d4d_ok"] = (ck.d4d_status == "done") & (ck.d4d_s_status == "done")

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
	% \begin{adjustbox}{width=\textwidth}
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
            % \end{adjustbox}
            \caption{Dataset summary.
		The first column indicates the dataset, and the \#F column indicates how many formulae the dataset contains.
		The following columns indicate the minimum and maximum number of variables (resp. clauses) in the dataset.}
	\label{divkc:tab:datasets}
\end{table}""")

print(r"""\begin{table}[h!]
	\centering
    \begin{tabular}{l|c|c|c|c|c|c}
        Dataset & \#$F_\textit{total}$ & \#$\text{\dfour}$ & \#$\neg \text{\dfour}$ & \#$+\text{\dfour}_\textit{ld}$ & \#$+\textit{CK}$ & \#$+\textit{DivKC}$ \\ 
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
    ld4d = lck[lck.d4d_ok]

    sd4 = set(ld4.index)
    sdivkc = set(ldivkc.index)
    sck = set(lck.index)
    sd4d = set(ld4d.index)

    not_d4 = nbf - len(sd4)
    onlyd4 = len(sd4 - sdivkc - sck - sd4d)
    addd4d = len(sd4d - sd4)
    addck = len(sck - sd4d - sd4)
    adddivkc = len(sdivkc - sck - sd4d - sd4)

    k = len(sdivkc - sd4)
    onlyd4d = len(((sd4d - sck) - sdivkc) - sd4)
    onlydivkc = len(sdivkc - sd4 - sck - sd4d)

    if k > 0:
        k = "\\textbf{" + str(k) + "}"

    # print(f"{vsub} & {nbf} & {onlyd4} & {nbf - len(sd4)} & {k} \\\\")
    print(f"{vsub} & {nbf} & {onlyd4} & {not_d4} & {addd4d} & {addck} & {adddivkc} \\\\")

print(r"""    \end{tabular}
        \caption{Experimental results regarding the scalability of Algorithm~\ref{divkc:alg:main}.
		Column \#$F_\textit{total}$ indicates the total number of formulae in each dataset.
		The next column shows the number of formulae compiled only by \dfour~\cite{D4} but not by Algorithm~\ref{divkc:alg:main}.
        Column \#$\neg\text{\dfour}$ shows the number of formulae not compiled by \dfour.
        The last column indicates the number of formulae that were only compiled by Algorithm~\ref{divkc:alg:main}, but not by \dfour.
	}
	\label{divkc:tab:gen}
\end{table}""")

print(r"""\begin{table}[h!]
	\centering
    \begin{tabular}{l|c|c|c|c}
        Dataset & \#$F_\textit{total}$ & \#$\neg \text{\dfour}$ & \#$\neg \text{\dfour} \land \textit{CapKC}$ & \#$\neg \textit{\dfour} \land \textit{DivKC}$ \\ 
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
    ld4d = lck[lck.d4d_ok]

    sd4 = set(ld4.index)
    sdivkc = set(ldivkc.index)
    sck = set(lck.index)
    sd4d = set(ld4d.index)

    not_d4 = nbf - len(sd4)
    ck_and_not_d4 = len(sck - sd4)
    divkc_and_not_d4 = len(sdivkc - sd4)

    # print(f"{vsub} & {nbf} & {onlyd4} & {nbf - len(sd4)} & {k} \\\\")
    print(f"{vsub} & {nbf} & {not_d4} & {ck_and_not_d4} & {divkc_and_not_d4} \\\\")

print(r"""    \end{tabular}
        \caption{Experimental results regarding the scalability of Algorithm~\ref{divkc:alg:main}.
		Column \#$F_\textit{total}$ indicates the total number of formulae in each dataset.
		The next column shows the number of formulae compiled only by \dfour~\cite{D4} but not by Algorithm~\ref{divkc:alg:main}.
        Column \#$\neg\text{\dfour}$ shows the number of formulae not compiled by \dfour.
        The last column indicates the number of formulae that were only compiled by Algorithm~\ref{divkc:alg:main}, but not by \dfour.
	}
	\label{divkc:tab:gen}
\end{table}""")


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
    \caption{Experimental results for Algorithm~\ref{divkc:alg:appmc}.
		Column \#F indicates with how many formulae the statistics have been computed.
        % Column $Y_l \leq |R_F|$ indicates how often the lower bound returned by Algorithm~\ref{divkc:alg:appmc} is correct (i.e., smaller than the true model count of $F$).
		% Similarly, column $Y_h \geq |R_F|$ indicates how often the upper bound is correct.
		The 'Coverage' column indicates how often $|R_F|$ is within the confidence interval $[Y_l; Y_h]$ and thus measures the accuracy of our method.
        % The last column confirms the correctness of the bounds obtained using Lemma~\ref{divkc:lem:ebounds}.
	}
	\label{divkc:tab:appmc}
\end{table}""")


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
            \caption{Experimental results comparing the bounds obtained with Algorithm~\ref{divkc:alg:appmc} and with Lemma~\ref{divkc:lem:ebounds}.
	Column \#F indicates with how many formulae the statistics have been computed.
	% Column $Y_l \geq |R_{G_P}|$ indicates how often the lower bound returned by Algorithm~\ref{divkc:alg:appmc} is correct and larger than the lower bound obtained by using Lemma~\ref{divkc:lem:ebounds}.
	% Similarly, column $Y_h \leq |R_{G_U}|$ indicates how often the upper bound returned by Algorithm~\ref{divkc:alg:appmc} is correct and better than the upper bound obtained by using Lemma~\ref{divkc:lem:ebounds}.
	The 'Both' column indicates how often we have $Y_l \geq |R_{G_P}| \land Y_l \leq |R_F|$
    and $Y_h \leq |R_{G_U}| \land Y_h \geq |R_F|$.
	The last two columns represent the observed median and maximum values of the ratio $r_c = \frac{\textit{min}(Y_h, |R_{G_U}|) - \textit{max}(Y_l, |R_{G_P}|)}{|R_{G_U}| - |R_{G_P}|}$, which was calculated exclusively if $Y_l \leq |R_F| \leq Y_h$.
	The number of formulae on which the last two columns are computed can easily be obtained by multiplying the \#F column with the 'Coverage' column in Table~\ref{divkc:tab:appmc}.
	}
	\label{divkc:tab:coverage}
\end{table}""")


print(r"""
\end{document}
""")
sys.exit(0)
