from typing import Tuple

from connect2etrade import connect2etrade
from enums import EtradeConnections


def recommendedStrikePrices(
    symbol: str, safetyFactor: float = 1.2
) -> Tuple[float, float]:
    # Get 52-week high
    # Get 52-week low
    # Get last closing price
    em = connect2etrade(EtradeConnections.MARKET, goLive=True)
    q = em.get_quote([symbol])["QuoteResponse"]["QuoteData"]["All"]

    h = float(q["high52"])
    l = float(q["low52"])
    c = float(q["previousClose"])

    distanceRatio = (h - l) / ((h + l) / 2)
    strikePriceSafeDistance = distanceRatio * safetyFactor / 10
    return (c - (c * strikePriceSafeDistance), c + (c * strikePriceSafeDistance))
