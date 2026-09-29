import json
import numpy as np
from astropy.io import fits

from config.settings import DATA_DIR


def log10_eps(x):
    return np.log10(x + np.finfo(float).tiny)


def ebl_intensity_data_import(path, if_log_data: bool = False, if_log_wvl: bool = False):
    redshifts = np.array([0., 0.01, 0.03, 0.05, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.8, 1.0,
                          1.2, 1.4, 1.6, 1.8, 2.0, 2.2, 2.4, 2.6, 2.8, 3.0,
                          3.2, 3.4, 3.6, 3.8, 4.0, 4.2, 4.4, 4.6, 4.8, 5.0,
                          5.2, 5.4, 5.6, 5.8, 6.])  # [DL]
    n = redshifts.size

    with open(path, 'r') as open_file:
        all_data = open_file.readlines()

    m = len(all_data) - 7

    wavelength = np.zeros(m)  # [mkm]
    intensity = np.zeros([n, m])  # [nW m-2 sr-1], specific intensity

    for i in range(m):
        line_i = all_data[i + 7].split(' ')

        counter = 0
        for j, elem in enumerate(line_i):
            if elem == '':
                continue
            if counter == 0:
                wavelength[i] = float(elem)
                counter += 1
            else:
                intensity[counter - 1, i] = float(elem)
                counter += 1

    if if_log_data:
        data = log10_eps(intensity)
    else:
        data = intensity

    if if_log_wvl:
        wvl = log10_eps(wavelength)
    else:
        wvl = wavelength

    return redshifts, wvl, data


def table_to_fits_file(output_file, data, comment=""):

    redshift, wavelength, intensity = data

    # Sanity check
    if intensity.shape != ((len(redshift), len(wavelength))):
        raise ValueError(
            f"Intensity table has shape {intensity.shape}, "
            f"expected ({len(redshift)}, {len(wavelength)})"
        )

    # Primary HDU
    primary = fits.PrimaryHDU()
    primary.header["CONTENT"] = comment

    # Wavelength extension
    hdu_wave = fits.ImageHDU(
        data=wavelength.astype(np.float64),
        name="WAVELENGTH"
    )
    hdu_wave.header["BUNIT"] = "micron"

    # Redshift extension
    hdu_z = fits.ImageHDU(
        data=redshift.astype(np.float64),
        name="REDSHIFT"
    )

    # Intensity extension
    hdu_int = fits.ImageHDU(
        data=intensity.astype(np.float64),
        name="INTENSITY"
    )
    hdu_int.header["BUNIT"] = "nW m-2 sr-1"
    
    # Write FITS file
    hdul = fits.HDUList([primary, hdu_wave, hdu_z, hdu_int])
    hdul.writeto(output_file, overwrite=True)

    print(f"Saved {output_file}")
    
    return


def saldana_lopez_reader():
    ebl_file = DATA_DIR / "ebl_info.json"
    with open(ebl_file) as f:
        ebl_list = json.load(f)
        
    sl21_list = ebl_list["Saldana-Lopez2021"]
        
    sl_dir = DATA_DIR / "EBL_models" / sl21_list["folder"]
    
    table_to_fits_file(
        sl_dir / sl21_list["data_path"],
        ebl_intensity_data_import(sl_dir / sl21_list["data_raw"])
        )
    table_to_fits_file(
        sl_dir / sl21_list["err_path"],
        ebl_intensity_data_import(sl_dir / sl21_list["err_raw"])
        )
    
    return


if __name__ == '__main__':
    saldana_lopez_reader()
    