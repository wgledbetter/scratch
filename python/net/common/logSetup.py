import logging


def logSetup(level: int = logging.DEBUG):
    logging.basicConfig(
        level=level,
        format="%(asctime)s - %(levelname)s - %(filename)s:%(funcName)s:%(lineno)s - %(message)s",
        datefmt="%d-%b-%y %H:%M:%S",
    )
