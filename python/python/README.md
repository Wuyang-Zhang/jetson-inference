# Python Package Layout

This directory contains the Python package files that get installed into Python's
`site-packages` / `dist-packages`.

## Why is it `python/python`?

The outer [`python`](../) directory is the Python-related workspace for this
repository. It contains:

- `bindings/`: the C++ binding build scripts and extension module build
- `examples/`: Python example programs
- `www/`: web/demo code
- `python/`: the pure-Python package directories that are copied into the final
  Python install location

In other words:

- outer `python/` = source tree for all Python-related code in this repo
- inner `python/python/` = package root that is installed into Python

This matches the CMake install logic in
[`python/bindings/CMakeLists.txt`](../bindings/CMakeLists.txt), which installs
these package directories into Python's package path:

- `../python/Jetson`
- `../python/jetson`
- `../python/jetson_inference`

## What is the difference between `jetson`, `jetson_inference`, and `Jetson/Inference`?

`jetson_inference` is the current package name and the recommended import path.
New code should use:

```python
from jetson_inference import detectNet, imageNet
```

Its initializer imports the compiled extension module
`jetson_inference_python` and exposes the actual Python API.

The other names exist for backward compatibility with older code:

- `jetson.inference`
- `Jetson.Inference`

Those legacy package paths point to the same underlying extension module, but
they are deprecated. The legacy initializer prints a warning telling users to
switch to `import jetson_inference` instead.

So the practical split is:

- `jetson_inference`: current public package name
- `jetson` / `Jetson`: compatibility namespace for old import styles

## About uppercase/lowercase names

This repository keeps legacy names for compatibility. On Linux, path case is
significant, so `Jetson` and `jetson` can be different package paths. On
Windows, the filesystem is usually case-insensitive, so these paths may appear
to overlap when you browse the checkout.

That can make the layout look more confusing than it really is, but the intent
is simply to preserve old imports while moving users to `jetson_inference`.
