from enum import Enum


class Mode(Enum):
    TCP4 = 0
    UDP4 = 1
    TCP6 = 2
    UDP6 = 3
    BLUETOOTH_UDP = 4
    BLUETOOTH_TCP = 5
