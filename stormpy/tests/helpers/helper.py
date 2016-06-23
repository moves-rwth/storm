import os
example_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), os.pardir, os.pardir, os.pardir, "examples"))

def get_example_path(*paths):
    return os.path.join(example_dir, *paths)
