from typing import List

from connect2etrade import connect2etrade
from enums import EtradeConnections


def symbolFilter(
    allSymbs: List[str],
    marketCapMin: float = 10e9,
    dailyVolumeMin: float = 1e5,
    divYieldMin: float = 0,
) -> List[str]:
    em = connect2etrade(EtradeConnections.MARKET, goLive=True)

    out = []
    for s in allSymbs:
        try:
            q = em.get_quote([s])["QuoteResponse"]["QuoteData"]["All"]
        except:
            continue

        mc = float(q["marketCap"])
        dv = float(q["averageVolume"])
        dy = float(q["dividend"])

        accept = True
        accept = accept and (marketCapMin < mc)
        accept = accept and (dailyVolumeMin < dv)
        accept = accept and (divYieldMin < dy)

        if accept:
            out.append(s)

    return out
