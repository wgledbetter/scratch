from datetime import datetime

from connect2etrade import connect2etrade
from enums import EtradeConnections
from nextFriday import nextFriday
from recommendedStrikePrices import recommendedStrikePrices


def bestOptions(symbol: str, safetyFactor: float = 1.2, weeks: int = 1):
    lo, hi = recommendedStrikePrices(symbol, safetyFactor)

    em = connect2etrade(EtradeConnections.MARKET, goLive=True)

    def getBoundingOps(nearPrice: float):
        ops = em.get_option_chains(
            symbol,
            nextFriday(datetime.today(), weeks),
            strike_price_near=int(nearPrice),
        )["OptionChainResponse"]["OptionPair"]

        loStrike = 0
        hiStrike = 0
        for op in ops:
            sp = float(op["Call"]["strikePrice"])
            if sp < nearPrice and sp > loStrike:
                loStrike = sp
                loOp = op
            if sp > nearPrice and sp > loStrike and loStrike > hiStrike:
                hiStrike = sp
                hiOp = op

        return (loOp, hiOp)

    return (getBoundingOps(lo), getBoundingOps(hi))
