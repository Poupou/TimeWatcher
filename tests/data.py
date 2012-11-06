DATA = {
    "detect":
    {
        "basic-1": [[198, 132, 96, 96, 0]],
        "registry-office": [[638, 678, 115, 115, 0]],
        "belfast": [[266, 389, 65, 65, 0]],
        "big-ben-2": [[397, 399, 124, 124, 0]],
        "clock-1": [[247, 135, 128, 128, 0]],
    },
    "read":
    {
        "basic-1": ["02:50"],
        "registry-office": ["01:04"],
        "belfast": ["11:35"],
        "big-ben-2": ["05:43"],
        "clock-1": ["01:01"],
    }
}

for f in DATA["read"]:
    DATA["read"][f] = map(lambda x: map(int, x.split(":")), DATA["read"][f])

