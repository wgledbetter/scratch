import json
import os

import pyetrade
from mySecrets import ETRADE_API_KEY
from mySecrets import ETRADE_API_SECRET
from mySecrets import ETRADE_SANDBOX_API_KEY
from mySecrets import ETRADE_SANDBOX_API_SECRET
from settings import ETRADE_LIVE_OAUTH_JSON
from settings import ETRADE_SAND_OAUTH_JSON
from enums import EtradeConnections

# Specific Endpoint Connections ########################################################


def connect2account(tokens, goLive=False) -> pyetrade.ETradeAccounts:
    if goLive:
        apiKey = ETRADE_API_KEY
        apiSecret = ETRADE_API_SECRET
    else:
        apiKey = ETRADE_SANDBOX_API_KEY
        apiSecret = ETRADE_SANDBOX_API_SECRET

    return pyetrade.ETradeAccounts(
        apiKey,
        apiSecret,
        tokens["oauth_token"],
        tokens["oauth_token_secret"],
        dev=not goLive,
    )


def connect2market(tokens, goLive=False) -> pyetrade.ETradeMarket:
    if goLive:
        apiKey = ETRADE_API_KEY
        apiSecret = ETRADE_API_SECRET
    else:
        apiKey = ETRADE_SANDBOX_API_KEY
        apiSecret = ETRADE_SANDBOX_API_SECRET

    return pyetrade.ETradeMarket(
        apiKey,
        apiSecret,
        tokens["oauth_token"],
        tokens["oauth_token_secret"],
        dev=not goLive,
    )


# Main Connection Function #############################################################


def connect2etrade(
    endpoint: EtradeConnections, goLive=False
) -> pyetrade.ETradeAccounts:
    tokens = {}

    if goLive:
        file = ETRADE_LIVE_OAUTH_JSON
    else:
        file = ETRADE_SAND_OAUTH_JSON

    fileExisted = os.path.isfile(file)
    if fileExisted:
        with open(file, "r") as f:
            try:
                tokens = json.load(f)
            except:
                tokens = None

        try:
            if endpoint == EtradeConnections.ACCOUNTS:
                conn = connect2account(tokens, goLive)
                conn.list_accounts()
                return conn
            elif endpoint == EtradeConnections.MARKET:
                return connect2market(tokens, goLive)
            else:
                raise ValueError("Unimplemented endpoint")
        except:
            os.remove(file)
            pass

    # Tokens didn't exist or didn't work
    if goLive:
        apiKey = ETRADE_API_KEY
        apiSecret = ETRADE_API_SECRET
    else:
        apiKey = ETRADE_SANDBOX_API_KEY
        apiSecret = ETRADE_SANDBOX_API_SECRET

    oauth = pyetrade.ETradeOAuth(apiKey, apiSecret)
    print(oauth.get_request_token())  # Use the printed URL

    verifier_code = input("Enter verification code: ")
    tokens = oauth.get_access_token(verifier_code)

    with open(file, "w") as f:
        json.dump(tokens, f)

    if endpoint == EtradeConnections.ACCOUNTS:
        return connect2account(tokens, goLive)
    elif endpoint == EtradeConnections.MARKET:
        return connect2market(tokens, goLive)
    else:
        raise ValueError("Unimplemented endpoint")
