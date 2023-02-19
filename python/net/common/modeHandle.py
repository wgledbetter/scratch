import logging
from socket import AddressFamily
from socket import SocketKind
from typing import Tuple

from enums import Mode


def modeHandle(mode: Mode) -> Tuple[Mode, AddressFamily, SocketKind]:
    if type(mode) != Mode:
        if type(mode) == int:
            debStr = "Casting '{}'".format(mode)
            mode = Mode(mode)
            debStr = debStr + " to mode '{}'.".format(mode)
            logging.debug(debStr)
        elif type(mode) == str:
            debStr = "Casting '{}'".format(mode)
            mode = Mode[mode]
            debStr = debStr + " to mode '{}'.".format(mode)
            logging.debug(debStr)
        else:
            errStr = "Your mode argument '{}' of type '{}' cannot be cast to a common.Mode.".format(
                mode, type(mode)
            )
            logging.error(errStr)
            raise TypeError(errStr)

    if mode == Mode.UDP4:
        aFam = AddressFamily.AF_INET
        sKind = SocketKind.SOCK_DGRAM
    elif mode == Mode.TCP4:
        aFam = AddressFamily.AF_INET
        sKind = SocketKind.SOCK_STREAM
    elif mode == Mode.UDP6:
        aFam = AddressFamily.AF_INET6
        sKind = SocketKind.SOCK_DGRAM
    elif mode == Mode.TCP6:
        aFam = AddressFamily.AF_INET6
        sKind = SocketKind.SOCK_STREAM
    elif mode == Mode.BLUETOOTH_UDP:
        aFam = AddressFamily.AF_BLUETOOTH
        sKind = SocketKind.SOCK_DGRAM
    elif mode == Mode.BLUETOOTH_TCP:
        aFam = AddressFamily.AF_BLUETOOTH
        sKind = SocketKind.SOCK_STREAM
    else:
        raise NotImplementedError()

    return (mode, aFam, sKind)
