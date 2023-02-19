import logging
from socket import getaddrinfo
from socket import socket

import fire
from common.logSetup import logSetup
from common.modeHandle import modeHandle
from enums import Mode

# Main #################################################################################


def main(host: str, port: int, mode: Mode = Mode.TCP4):
    logSetup()
    logging.info(
        "Entering send.main with host = {}, port = {}, mode = {}.".format(
            host, port, mode
        )
    )

    # Process Arguments ================================================================

    mode, aFam, sKind = modeHandle(mode)

    # Get IP ===========================================================================

    aInfo = list(set(getaddrinfo(host, port, aFam, sKind)))[0]
    ip = aInfo[4][0]
    logging.info("Using host ip: '{}'.".format(ip))

    # Do socket stuff ==================================================================

    with socket(aFam, sKind) as s:
        logging.debug("Trying to connect...")
        s.connect((ip, port))
        logging.debug("Connected.")

        while True:
            msg = input("Write a message to send:\n")

            logging.debug("Trying to send...")
            s.sendall(bytes(msg, "utf-8"))
            logging.debug("Sent.")


# Run ##################################################################################

if __name__ == "__main__":
    fire.Fire(main)
