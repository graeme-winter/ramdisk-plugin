import json
import os
import sys

import h5py
import tqdm


def meta_filename(filename):
    if filename.endswith(".nxs"):
        meta = filename.replace(".nxs", "_meta.h5")
    elif filename.endswith("_master.h5"):
        meta = filename.replace("_master.h5", "_meta.h5")
    if not os.path.exists(meta):
        raise OSError(f"File not found {meta}")
    return meta


def extract_header_packet(filename):
    meta = h5py.File(meta_filename(filename), "r")
    text = meta["config"][()].decode()
    with open("start_1", "w") as f:
        f.write(json.dumps(eval(text)))


def extract_chunks(filename):
    with h5py.File(filename, "r") as f:
        data = f["/entry/data"]

        i = 0

        for k in sorted(d for d in data if d.startswith("data_")):
            print(f"Extracting {k}")
            dataset = data[k]
            d_id = dataset.id

            nz = dataset.shape[0]

            for j in tqdm.tqdm(range(nz)):
                offset = (j, 0, 0)
                _, chunk = d_id.read_direct_chunk(offset)

                with open(f"image_{i:06d}_2", "wb") as f:
                    f.write(chunk)

                i += 1


extract_header_packet(sys.argv[1])
extract_chunks(sys.argv[1])
