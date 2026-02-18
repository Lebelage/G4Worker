import re
import numpy as np
import matplotlib.pyplot as plt

FNAME = "build/out_h1_Edep_vs_depth.csv"
N_EVENTS = 10_000

def parse_h1d_csv(fname: str):
    axis_re = re.compile(r"#axis\s+fixed\s+(\d+)\s+([0-9.eE+-]+)\s+([0-9.eE+-]+)")
    nbins = xmin = xmax = None

    data_lines = []
    in_data = False

    with open(fname, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue

            m = axis_re.match(line)
            if m:
                nbins = int(m.group(1))
                xmin = float(m.group(2))
                xmax = float(m.group(3))
                continue

            if line.startswith("entries,Sw,Sw2"):
                in_data = True
                continue

            if in_data and not line.startswith("#"):
                data_lines.append(line)

    if nbins is None or xmin is None or xmax is None:
        raise ValueError("Не нашёл строку '#axis fixed ...' в шапке файла.")

    data = np.genfromtxt(data_lines, delimiter=",")
    if data.ndim == 1:
        data = data.reshape(1, -1)

    return nbins, xmin, xmax, data

def main():
    nbins, xmin, xmax, data = parse_h1d_csv(FNAME)

    # В таких файлах часто есть under/overflow; берём первые nbins как обычные бины
    data = data[:nbins, :]

    entries = data[:, 0]
    sw = data[:, 1]   # сумма весов = энерговклад в бине (MeV)
    sw2 = data[:, 2]

    # Ось
    edges = np.linspace(xmin, xmax, nbins + 1)
    centers = 0.5 * (edges[:-1] + edges[1:])
    binw = edges[1] - edges[0]

    # Эвристика единиц: у вас xmax=0.0031 похоже на мм, но в title написано [um]
    # Сделаем "как правильно для человека": покажем глубину в µm
    if xmax <= 0.1:
        depth_um = centers * 1000.0  # мм -> µm
        dx_um = binw * 1000.0
    else:
        depth_um = centers
        dx_um = binw

    # ---- нормировка на число событий
    edep_per_evt = sw / N_EVENTS          # MeV/evt (в каждом бине)
    cum_edep_per_evt = np.cumsum(edep_per_evt)  # MeV/evt, накопленно по глубине

    # (опционально) оценка ошибки на бин: sigma(Sw)/N
    # часто Sw2 = сумма квадратов весов, тогда sigma ~ sqrt(Sw2)
    sigma_per_evt = np.sqrt(sw2) / N_EVENTS

    # ======== 1) Профиль E(depth) на событие ========
    plt.figure(figsize=(7, 5))
    plt.step(depth_um, edep_per_evt, where="mid")
    plt.grid(True)
    plt.xlabel("Depth (µm)")
    plt.ylabel("Edep per event (MeV/evt)")
    plt.title("Edep vs depth (normalized)")
    plt.tight_layout()
    plt.show()

    # ======== 2) Кумулятивная сумма ========
    plt.figure(figsize=(7, 5))
    plt.plot(depth_um, cum_edep_per_evt)
    plt.grid(True)
    plt.xlabel("Depth (µm)")
    plt.ylabel("Cumulative Edep per event (MeV/evt)")
    plt.title("Cumulative energy deposit vs depth")
    plt.tight_layout()
    plt.show()

    # Печать итогов
    total_edep_evt = edep_per_evt.sum()
    print(f"N events: {N_EVENTS}")
    print(f"Total Edep per event (sum over bins): {total_edep_evt:.6g} MeV/evt")
    print(f"Nonzero bins: {(sw>0).sum()} / {nbins}")

    # Если захотите "глубину пробега" как depth@90% deposited energy:
    frac = 0.90
    target = frac * cum_edep_per_evt[-1]
    idx = np.searchsorted(cum_edep_per_evt, target)
    if 0 <= idx < len(depth_um):
        print(f"Depth at {frac*100:.0f}% cumulative Edep: {depth_um[idx]:.6g} µm")

if __name__ == "__main__":
    main()