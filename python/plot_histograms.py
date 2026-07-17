import numpy as np
import matplotlib.pyplot as plt

mass = np.loadtxt(
    "output/histograms/mass_distribution.txt"
)

sfr = np.loadtxt(
    "output/histograms/sfr_distribution.txt"
)


plt.figure(figsize=(6, 4))
plt.step(
    mass[:, 0],
    mass[:, 1],
    where="mid"
)

plt.xlabel(r"$\log_{10}(M_\star/M_\odot)$")
plt.ylabel("Number of galaxies")
plt.yscale("log")
plt.tight_layout()
plt.savefig(
    "output/histograms/mass_distribution.png",
    dpi=300
)


plt.figure(figsize=(6, 4))
plt.step(
    sfr[:, 0],
    sfr[:, 1],
    where="mid"
)

plt.xlabel(r"$\log_{10}(\mathrm{SFR}/M_\odot\,yr^{-1})$")
plt.ylabel("Number of galaxies")
plt.yscale("log")
plt.tight_layout()
plt.savefig(
    "output/histograms/sfr_distribution.png",
    dpi=300
)