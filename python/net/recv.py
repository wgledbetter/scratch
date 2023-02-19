import logging
from socket import getaddrinfo
from socket import socket

import fire
from common.logSetup import logSetup
from common.modeHandle import modeHandle
from enums import Mode

# Main #################################################################################


def main(host: str, port: int, mode: Mode = Mode.TCP4, buf: int = 1024):
    logSetup()
    logging.info(
        "Entering recv.main with host = {}, port = {}, mode = {}.".format(
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
        logging.debug("Trying to bind...")
        s.bind((ip, port))
        logging.debug("Bound.")

        if mode == Mode.TCP4 or mode == Mode.TCP6:
            logging.debug("Trying to listen...")
            s.listen()
            logging.debug("Listening.")

            conn, addr = s.accept()
            with conn:
                logging.info("Got connection from '{}'.".format(addr))
                while True:
                    data = conn.recv(buf)  # This call blocks until it gets data
                    if not data:
                        logging.error("Got no data during recieve.")
                        break
                    logging.info("Recieved data: '{}' from '{}'.".format(data, addr))
        elif mode == Mode.UDP4 or mode == Mode.UDP6:
            logging.info("Reading UDP...")
            while True:
                data, addr = s.recvfrom(buf)
                if not data:
                    logging.error("Got no data from UDP.")
                    break
                logging.info("Recieved data: '{}' from '{}'.".format(data, addr))
        else:
            raise NotImplementedError()


# Run ##################################################################################

if __name__ == "__main__":
    fire.Fire(main)
