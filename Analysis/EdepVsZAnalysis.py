import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

filename = "build/out_h1_Edep_vs_depth.csv"

nbins = 3100
dmin = 0.0
dmax = 3.1      # µm
n_events = 10000   # <-- поставь своё число

bin_width = (dmax - dmin) / nbins  # µm
print("Bin width =", bin_width, "µm")

# читаем CSV
df = pd.read_csv(filename, comment="#", sep=None, engine="python")

# убираем underflow если есть
if len(df) == nbins + 1:
    df = df.iloc[1:].reset_index(drop=True)
if len(df) == nbins + 2:
    df = df.iloc[1:-1].reset_index(drop=True)

# глубина (центр бина)
depth = np.linspace(dmin + bin_width/2,
                    dmax - bin_width/2,
                    nbins)

# ---- dE/dZ ----
dEdz = df["Sw"] / (n_events * bin_width)

# ---- Кумулятивная энергия ----
E_cum = np.cumsum(dEdz) * bin_width  # MeV

# ====== Графики ======
fig, ax = plt.subplots(2, 1, figsize=(8,8), sharex=True)

# dE/dZ
ax[0].plot(depth, dEdz)
ax[0].set_ylabel("dE/dZ (MeV/µm)")
ax[0].set_title("Energy loss profile")
ax[0].grid(True)

# cumulative
ax[1].plot(depth, E_cum)
ax[1].set_xlabel("Depth (µm)")
ax[1].set_ylabel("Cumulative Edep (MeV)")
ax[1].set_title("Cumulative deposited energy")
ax[1].grid(True)

plt.tight_layout()
plt.show()