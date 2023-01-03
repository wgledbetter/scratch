from bestOptions import bestOptions
from stocks import nasdaq
from stocks import nyse
from symbolFilter import symbolFilter
from trend import trend


def main(reFilter: bool = False):
    # Get filtered symbols
    if reFilter:
        symbols = symbolFilter(nasdaq + nyse)
    else:
        try:
            from filtered import symbols
        except:
            symbols = symbolFilter(nasdaq + nyse)

    # Get trends of symbols
    trends = {}
    for s in symbols:
        print("Calculating trend of {}.".format(s))
        try:
            trends[s] = trend(s)
        except:
            pass

    consistent = [
        s
        for s in trends
        if all([t > 0 for t in trends[s]]) or all([t < 0 for t in trends[s]])
    ]

    direction = {}
    for s in consistent:
        if trends[s][0] < 0:
            direction[s] = -1
        elif trends[s][0] > 0:
            direction[s] = 1
        else:
            direction[s] = 0

    # Get available options near recommendations
    ops = {}
    for s in trends:
        if s in consistent:
            try:
                ops[s] = bestOptions(s)
            except:
                pass

    # Determine whether CALL or PUT is preferable for given symbol
    candidates = []
    for s in ops:
        if direction[s] == -1:
            # Sell call if it's going down
            candidates.append(ops[s][1][1]["Call"])
        elif direction[s] == 1:
            # Sell put if it's going up
            candidates.append(ops[s][0][0]["Put"])
