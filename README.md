# CpythonMinecraftLuncher
### Minecraft luncher dll for python

Libraries: pybind11, jsoncpp, zip

### Usage

import the .pyd in output directory

then use it.

##### example:

```python
import MinecraftLauncher

if __name__ == "__main__":
    #Launch arguments
    argv = [
        "D:\\Game\\MC\\.minecraft",
        "1.18.2",
        "D:\\Program Files\\Java\\jdk-17.0.2\\bin\\java.exe",
        "1024m",
        "512m",
        "User",
        "480",
        "854"
        ]
    # start game
    MinecraftLauncher.start(argv)
```
