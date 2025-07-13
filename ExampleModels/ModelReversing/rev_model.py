import h5py
import json
import numpy as np

# Custom JSON encoder to handle NumPy types
class NumpyEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, (np.integer,)):
            return int(obj)
        elif isinstance(obj, (np.floating,)):
            return float(obj)
        elif isinstance(obj, (np.ndarray,)):
            return obj.tolist()
        elif isinstance(obj, bytes):
            return obj.decode('utf-8', errors='ignore')
        return super().default(obj)

def extract_dataset(ds):
    data = ds[()]
    return np.round(data.tolist(), 3) if isinstance(data, np.ndarray) else data

def extract_group(group):
    group_json = {}
    for name, item in group.items():
        if isinstance(item, h5py.Dataset):
            group_json[name] = extract_dataset(item)
        elif isinstance(item, h5py.Group):
            group_json[name] = extract_group(item)
    return group_json

def extract_model_info(h5_file):
    model_info = {}

    with h5py.File(h5_file, 'r') as f:
        # Extract architecture
        if 'model_config' in f.attrs:
            model_config = f.attrs['model_config']
            model_info['architecture'] = json.loads(model_config)
        else:
            model_info['architecture'] = "Not found in file"

        # Extract weights
        if 'model_weights' in f:
            weights_group = f['model_weights']
            model_info['weights'] = extract_group(weights_group)
        else:
            model_info['weights'] = "Not found in file"

    return model_info

if __name__ == "__main__":
    model_path = "model.h5"
    output_path = "model_dump.json"

    model_json = extract_model_info(model_path)

    with open(output_path, 'w') as f:
        json.dump(model_json, f, indent=2, cls=NumpyEncoder)

    print(f"✅ Model structure and weights exported to {output_path}")
