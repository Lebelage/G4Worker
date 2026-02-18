import re
import numpy as np
import matplotlib.pyplot as plt

FNAME = "build/out_h1_Edep_step.csv"  

def parse_h1d(fname):
    axis_re = re.compile(r"#axis\s+fixed\s+(\d+)\s+([0-9.eE+-]+)\s+([0-9.eE+-]+)")
    nbins = xmin = xmax = None
    data_lines = []
    read_data = False

    with open(fname, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()

            m = axis_re.match(line)
            if m:
                nbins = int(m.group(1))
                xmin = float(m.group(2))
                xmax = float(m.group(3))
                continue

            if line.startswith("entries,Sw,Sw2"):
                read_data = True
                continue

            if read_data and not line.startswith("#") and line:
                data_lines.append(line)

    data = np.genfromtxt(data_lines, delimiter=",")
    return nbins, xmin, xmax, data

def main():
    nbins, xmin, xmax, data = parse_h1d(FNAME)

    # берем первые nbins (игнорируем under/overflow)
    data = data[:nbins, :]

    entries = data[:, 0]  # counts per bin

    # восстановим ось
    edges = np.linspace(xmin, xmax, nbins + 1)
    centers = 0.5 * (edges[:-1] + edges[1:])
    bin_width = edges[1] - edges[0]

    # ---- построение гистограммы ----
    plt.figure(figsize=(7,5))
    plt.bar(centers, entries, width=bin_width, align='center')

    plt.xlabel("dE per step (MeV)")
    plt.ylabel("Counts")
    plt.title("Energy deposit per step")
    plt.grid(True)
    plt.yscale("log")

    plt.tight_layout()
    plt.show()

    print("Total steps counted:", int(entries.sum()))

if __name__ == "__main__":
    main()