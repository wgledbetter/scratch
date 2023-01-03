import functools
from typing import List

import yfinance as yf


@functools.cache
def trend(
    symbol: str, intervals: List[str] = ["5d", "1mo", "3mo", "6mo"]
) -> List[float]:
    t = yf.Ticker(symbol)

    out = []
    for ivl in intervals:
        hist = t.history(period=ivl)
        old = hist.iloc[0]
        new = hist.iloc[-1]

        out.append((new["Close"] - old["Open"]) / old["Open"])

    return out
